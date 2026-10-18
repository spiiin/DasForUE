param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
[ordered]@{state='pending';feature='CurveTable evaluation'}|ConvertTo-Json|Set-Content "$Repo/logs/curve-tables-validation.json" -Encoding utf8
function Read-CurveTablesSourceHashes {
 $Result=@{}
 foreach($File in Get-ChildItem "$Repo/Plugins/DasForUE/Source" -Recurse -File){$Result[$File.FullName]=(Get-FileHash -LiteralPath $File.FullName -Algorithm SHA256).Hash}
 # Hash versioned scripts and this feature's new modules; ignore unrelated ephemeral probes.
 $Scripts=@(& git -C $Repo ls-files -- 'Plugins/DasForUE/Content/Das/Scripts/*.das')+@('Plugins/DasForUE/Content/Das/Scripts/ue_curve_tables.das','Plugins/DasForUE/Content/Das/Scripts/curvetable_items.das')
 foreach($Relative in $Scripts|Select-Object -Unique){$Path=Join-Path $Repo $Relative;$Result[$Path]=(Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash}
 $Result["$Repo/Tests/CurveTables/probe.das"]=(Get-FileHash "$Repo/Tests/CurveTables/probe.das" -Algorithm SHA256).Hash
 return $Result
}
function Assert-CurveTablesSourceHashes($Expected) {
 $Actual=Read-CurveTablesSourceHashes
 if($Expected.Count -ne $Actual.Count){throw 'Source file set changed during CurveTables verification'}
 foreach($Path in $Expected.Keys){if($Expected[$Path] -ne $Actual[$Path]){throw "Source changed during CurveTables verification: $Path"}}
}
$Sources=Read-CurveTablesSourceHashes
if(!$SkipBuild){
 & "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=8 "-Log=$Repo/logs/curve-tables-build.log" *> "$Repo/logs/curve-tables-build-stdout.log"
 if($LASTEXITCODE){throw 'CurveTables build failed'}
}
Assert-CurveTablesSourceHashes $Sources
$Hashes=@{}
foreach($Name in @('DasRuntime','DasEditor')){$Path="$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$Name.dll";$Hashes[$Path]=(Get-FileHash $Path -Algorithm SHA256).Hash}
$Manifest="$Engine/Engine/Binaries/Win64/UnrealEditor.modules"
$BuildId=(Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId
$Log="$Repo/logs/curve-tables-runtime.log"
try {
 & "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasCurveTables "-Fixture=$Repo/Tests/CurveTables/probe.das" -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$Log" *> "$Repo/logs/curve-tables-runtime-stdout.log"
 $Code=$LASTEXITCODE;$Text=Get-Content $Log -Raw
 $Result=[regex]::Match($Text,'DAS_CURVE_TABLES_RESULT=PASS checks=(\d+)')
 if($Code -or !$Result.Success){throw "CurveTables runtime probe failed: $Log"}
 if($Text -match 'Ensure condition failed|Log[^:]+: Error:'){throw 'Engine error/ensure in CurveTables probe'}
 foreach($Path in $Hashes.Keys){if((Get-FileHash $Path -Algorithm SHA256).Hash -ne $Hashes[$Path]){throw 'DLL changed during probe'}}
 Assert-CurveTablesSourceHashes $Sources
 if((Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId -ne $BuildId){throw 'Engine BuildId changed during probe'}
 [ordered]@{state='PASS';runtime_checks=[int]$Result.Groups[1].Value;source_sha256=$Sources;dll_sha256=$Hashes;engine_build_id=$BuildId;runtime_log=$Log;build=$(if($SkipBuild){'existing'}else{'PASS'});PIE='not_run';packaged='not_run';physical_input='not_run';commit='not_created'}|ConvertTo-Json -Depth 8|Set-Content "$Repo/logs/curve-tables-validation.json" -Encoding utf8
 Write-Output "DAS_CURVE_TABLES_VALIDATION=PASS checks=$($Result.Groups[1].Value)"
} finally {}
