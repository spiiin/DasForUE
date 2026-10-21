param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
$Logs=Join-Path $Repo 'logs';New-Item -ItemType Directory -Path $Logs -Force|Out-Null
& "$PSScriptRoot/GenerateNativeGameplay.ps1" -Engine $Engine -Check -SkipBuild:$SkipBuild *> "$Logs/native-discovery-reproduce.log"
if(!$?){throw 'Native discovery artifact reproduction failed'}
foreach($Test in @('TestNativeGameplayDiscovery.py','TestNativeGameplayGenerator.py')){
 & C:/Python312/python.exe "$PSScriptRoot/$Test" *> "$Logs/$Test.log"
 if($LASTEXITCODE){throw "$Test failed"}
}
& C:/Python312/python.exe "$PSScriptRoot/TestNativeGameplayDiscoveryIntegration.py" --engine $Engine *> "$Logs/native-discovery-integration-stdout.log"
if($LASTEXITCODE){throw 'Native discovery reflection integration failed'}
$Log=Join-Path $Logs 'native-discovery-runtime.log'
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasNativeGameplay -Probe -Multiprocess -unattended -NullRHI -nosplash -stdout "-abslog=$Log" -ddc=NoZenLocalFallback -NoZenAutoLaunch *> "$Logs/native-discovery-runtime-stdout.log"
if($LASTEXITCODE -or !(Test-DasResult -LiteralPath $Log -SimpleMatch 'DAS_NATIVE_DISCOVERY_RUNTIME=PASS')){throw 'Discovered CapsuleComponent runtime probe failed'}
& "$PSScriptRoot/TestPickup.ps1" -Engine $Engine -SkipBuild *> "$Logs/native-discovery-pickup.log"
if(!$?){throw 'Pickup regression failed'}
[ordered]@{result='PASS';checks=@('discovery lexer/import graph','native generator','DAS-only PointLightComponent reflection and generation','invalid receiver/module requests rejected','artifact reproduction','discovered CapsuleComponent runtime methods','saved-map pickup scenarios');not_run=@('Foundation suite','full PIE suite','packaged build');commit_created=$false}|ConvertTo-Json -Depth 4|Set-Content -LiteralPath "$Logs/native-discovery-validation.json" -Encoding utf8
Write-Output 'DAS_NATIVE_DISCOVERY_FOCUSED=PASS'
