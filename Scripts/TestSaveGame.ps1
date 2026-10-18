param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
[ordered]@{state='pending';feature='Authored SaveGame and managed async slots'}|ConvertTo-Json|Set-Content "$Repo/logs/savegame-validation.json" -Encoding utf8
function Read-SaveGameSourceHashes {
 $Result=@{}
 foreach($File in Get-ChildItem "$Repo/Plugins/DasForUE/Source" -Recurse -File){$Result[$File.FullName]=(Get-FileHash -LiteralPath $File.FullName -Algorithm SHA256).Hash}
 foreach($File in Get-ChildItem "$Repo/Plugins/DasForUE/Content/Das/Scripts" -File -Filter '*.das' | Where-Object {$_.Name -notlike '*_probe_*'}){$Result[$File.FullName]=(Get-FileHash -LiteralPath $File.FullName -Algorithm SHA256).Hash}
 $Result["$Repo/Tests/SaveGame/probe.das"]=(Get-FileHash "$Repo/Tests/SaveGame/probe.das" -Algorithm SHA256).Hash
 return $Result
}
function Assert-SaveGameSourceHashes($Expected) {
 $Actual=Read-SaveGameSourceHashes
 if($Expected.Count -ne $Actual.Count){throw 'Source file set changed during SaveGame verification'}
 foreach($Path in $Expected.Keys){if($Expected[$Path] -ne $Actual[$Path]){throw "Source changed during SaveGame verification: $Path"}}
}
$Sources=Read-SaveGameSourceHashes
if(!$SkipBuild){
 & "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=8 "-Log=$Repo/logs/savegame-build.log" *> "$Repo/logs/savegame-build-stdout.log"
 if($LASTEXITCODE){throw 'SaveGame build failed'}
}
Assert-SaveGameSourceHashes $Sources
$Hashes=@{}
foreach($Name in @('DasRuntime','DasEditor')){$Path="$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$Name.dll";$Hashes[$Path]=(Get-FileHash $Path -Algorithm SHA256).Hash}
$Manifest="$Engine/Engine/Binaries/Win64/UnrealEditor.modules"
$BuildId=(Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId
$Log="$Repo/logs/savegame-runtime.log"
try {
 & "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasSaveGame "-Fixture=$Repo/Tests/SaveGame/probe.das" -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$Log" *> "$Repo/logs/savegame-runtime-stdout.log"
 $Code=$LASTEXITCODE;$Text=Get-Content $Log -Raw
 $Result=[regex]::Match($Text,'DAS_SAVEGAME_RESULT=PASS checks=(\d+)')
 if($Code -or !$Result.Success){throw "SaveGame runtime probe failed: $Log"}
 if($Text -match 'Ensure condition failed|Log[^:]+: Error:'){throw 'Engine error/ensure in SaveGame probe'}
 foreach($Path in $Hashes.Keys){if((Get-FileHash $Path -Algorithm SHA256).Hash -ne $Hashes[$Path]){throw 'DLL changed during probe'}}
 Assert-SaveGameSourceHashes $Sources
 if((Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId -ne $BuildId){throw 'Engine BuildId changed during probe'}
 [ordered]@{state='PASS';runtime_checks=[int]$Result.Groups[1].Value;callbacks='save(bool), load(UObjectHandle)';source_sha256=$Sources;dll_sha256=$Hashes;engine_build_id=$BuildId;runtime_log=$Log;build=$(if($SkipBuild){'existing'}else{'PASS'});PIE='not_run';packaged='not_run';physical_input='not_run';commit='not_created'}|ConvertTo-Json -Depth 8|Set-Content "$Repo/logs/savegame-validation.json" -Encoding utf8
 Write-Output "DAS_SAVEGAME_VALIDATION=PASS checks=$($Result.Groups[1].Value)"
} finally {}
