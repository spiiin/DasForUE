param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
[ordered]@{state='pending';feature='Authored DataTable JSON creation and typed rows'}|ConvertTo-Json|Set-Content "$Repo/logs/data-tables-validation.json" -Encoding utf8
function Read-DataTablesSourceHashes {
 $Result=@{}
 foreach($File in Get-ChildItem "$Repo/Plugins/DasForUE/Source" -Recurse -File){$Result[$File.FullName]=(Get-FileHash -LiteralPath $File.FullName -Algorithm SHA256).Hash}
 # Hash versioned scripts and this feature's new modules; ignore unrelated ephemeral probes.
 $Scripts=@(& git -C $Repo ls-files -- 'Plugins/DasForUE/Content/Das/Scripts/*.das')+@('Plugins/DasForUE/Content/Das/Scripts/ue_data_tables.das','Plugins/DasForUE/Content/Das/Scripts/datatable_item_types.das','Plugins/DasForUE/Content/Das/Scripts/datatable_items.das')
 foreach($Relative in $Scripts|Select-Object -Unique){$Path=Join-Path $Repo $Relative;$Result[$Path]=(Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash}
 $Result["$Repo/Examples/DataTables/Items.json"]=(Get-FileHash "$Repo/Examples/DataTables/Items.json" -Algorithm SHA256).Hash
 $Result["$Repo/Tests/DataTables/probe.das"]=(Get-FileHash "$Repo/Tests/DataTables/probe.das" -Algorithm SHA256).Hash
 return $Result
}
function Assert-DataTablesSourceHashes($Expected) {
 $Actual=Read-DataTablesSourceHashes
 if($Expected.Count -ne $Actual.Count){throw 'Source file set changed during DataTables verification'}
 foreach($Path in $Expected.Keys){if($Expected[$Path] -ne $Actual[$Path]){throw "Source changed during DataTables verification: $Path"}}
}
$Sources=Read-DataTablesSourceHashes
if(!$SkipBuild){
 & "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=8 "-Log=$Repo/logs/data-tables-build.log" *> "$Repo/logs/data-tables-build-stdout.log"
 if($LASTEXITCODE){throw 'DataTables build failed'}
}
Assert-DataTablesSourceHashes $Sources
$Hashes=@{}
foreach($Name in @('DasRuntime','DasEditor')){$Path="$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$Name.dll";$Hashes[$Path]=(Get-FileHash $Path -Algorithm SHA256).Hash}
$Manifest="$Engine/Engine/Binaries/Win64/UnrealEditor.modules"
$BuildId=(Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId
$Log="$Repo/logs/data-tables-runtime.log"
try {
 & "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasDataTables "-Fixture=$Repo/Tests/DataTables/probe.das" "-ItemsJson=$Repo/Examples/DataTables/Items.json" -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$Log" *> "$Repo/logs/data-tables-runtime-stdout.log"
 $Code=$LASTEXITCODE;$Text=Get-Content $Log -Raw
 $Result=[regex]::Match($Text,'DAS_DATA_TABLES_RESULT=PASS checks=(\d+)')
 if($Code -or !$Result.Success){throw "DataTables runtime probe failed: $Log"}
 if($Text -match 'Ensure condition failed|Log[^:]+: Error:'){throw 'Engine error/ensure in DataTables probe'}
 foreach($Path in $Hashes.Keys){if((Get-FileHash $Path -Algorithm SHA256).Hash -ne $Hashes[$Path]){throw 'DLL changed during probe'}}
 Assert-DataTablesSourceHashes $Sources
 if((Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId -ne $BuildId){throw 'Engine BuildId changed during probe'}
 [ordered]@{state='PASS';runtime_checks=[int]$Result.Groups[1].Value;source_sha256=$Sources;dll_sha256=$Hashes;engine_build_id=$BuildId;runtime_log=$Log;build=$(if($SkipBuild){'existing'}else{'PASS'});PIE='not_run';packaged='not_run';physical_input='not_run';commit='not_created'}|ConvertTo-Json -Depth 8|Set-Content "$Repo/logs/data-tables-validation.json" -Encoding utf8
 Write-Output "DAS_DATA_TABLES_VALIDATION=PASS checks=$($Result.Groups[1].Value)"
} finally {}
