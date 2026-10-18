param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
function Read-WidgetSourceHashes {
 $Result=@{}
 foreach($File in Get-ChildItem "$Repo/Plugins/DasForUE/Source" -Recurse -File){$Result[$File.FullName]=(Get-FileHash -LiteralPath $File.FullName -Algorithm SHA256).Hash}
 foreach($File in Get-ChildItem "$Repo/Plugins/DasForUE/Content/Das/Scripts" -File -Filter '*.das' | Where-Object {$_.Name -notlike '*_probe_*'}){$Result[$File.FullName]=(Get-FileHash -LiteralPath $File.FullName -Algorithm SHA256).Hash}
 $Result["$Repo/Tests/WidgetEvents/probe.das"]=(Get-FileHash "$Repo/Tests/WidgetEvents/probe.das" -Algorithm SHA256).Hash
 return $Result
}
function Assert-WidgetSourceHashes($Expected) {
 $Actual=Read-WidgetSourceHashes
 if($Expected.Count -ne $Actual.Count){throw 'Source file set changed during widget verification'}
 foreach($Path in $Expected.Keys){if($Expected[$Path] -ne $Actual[$Path]){throw "Source changed during widget verification: $Path"}}
}
$Sources=Read-WidgetSourceHashes
if(!$SkipBuild){
 & "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=8 "-Log=$Repo/logs/widget-events-build.log" *> "$Repo/logs/widget-events-build-stdout.log"
 if($LASTEXITCODE){throw 'Widget events build failed'}
}
Assert-WidgetSourceHashes $Sources
$Hashes=@{}
foreach($Name in @('DasRuntime','DasEditor')){$Path="$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$Name.dll";$Hashes[$Path]=(Get-FileHash $Path -Algorithm SHA256).Hash}
$Manifest="$Engine/Engine/Binaries/Win64/UnrealEditor.modules"
$BuildId=(Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId
$Name='widget_events_probe_'+[guid]::NewGuid().ToString('N')+'.das'
$Probe="$Repo/Plugins/DasForUE/Content/Das/Scripts/$Name"
$Log="$Repo/logs/widget-events-runtime.log"
Copy-Item "$Repo/Tests/WidgetEvents/probe.das" $Probe
try {
 & "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasWidgetEvents "-Script=$Name" -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$Log" *> "$Repo/logs/widget-events-runtime-stdout.log"
 $Code=$LASTEXITCODE;$Text=Get-Content $Log -Raw
 $Result=[regex]::Match($Text,'DAS_WIDGET_EVENTS_RESULT=PASS checks=(\d+)')
 if($Code -or !$Result.Success){throw "Widget input runtime probe failed: $Log"}
 if($Text -match 'Ensure condition failed|Log[^:]+: Error:'){throw 'Engine error/ensure in widget input probe'}
 foreach($Path in $Hashes.Keys){if((Get-FileHash $Path -Algorithm SHA256).Hash -ne $Hashes[$Path]){throw 'DLL changed during probe'}}
 Assert-WidgetSourceHashes $Sources
 if((Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId -ne $BuildId){throw 'Engine BuildId changed during probe'}
 [ordered]@{state='PASS';runtime_checks=[int]$Result.Groups[1].Value;source_sha256=$Sources;dll_sha256=$Hashes;engine_build_id=$BuildId;runtime_log=$Log;build=$(if($SkipBuild){'existing'}else{'PASS'});PIE='not_run';packaged='not_run';physical_input='not_run';commit='not_created'}|ConvertTo-Json -Depth 8|Set-Content "$Repo/logs/widget-events-validation.json" -Encoding utf8
 Write-Output "DAS_WIDGET_EVENTS_VALIDATION=PASS checks=$($Result.Groups[1].Value)"
} finally {Remove-Item -LiteralPath $Probe -ErrorAction SilentlyContinue}
