param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das}
$Probe=[Guid]::NewGuid().ToString('N')
$Output=Join-Path $Das.Logs 'savegame-compatibility-runtime.json'
Invoke-DasCommandlet $Das -Run DasSaveGameCompatibility -Name 'savegame-compatibility-prepare' `
 -Arguments @('-Phase=Prepare',"-Probe=$Probe","-Output=$Output",'-Multiprocess') `
 -Marker 'DAS_SAVEGAME_COMPAT_PREPARE=PASS' -NoZen `
 -FailPattern 'DAS_SAVEGAME_COMPAT_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:' | Out-Null
$Info=Get-Content -LiteralPath $Output -Raw|ConvertFrom-Json
if($Info.state -ne 'PASS'){throw 'SaveGame compatibility runtime failed'}
Write-Output "DAS_SAVEGAME_COMPAT_RUNTIME=PASS checks=$($Info.checks)"
