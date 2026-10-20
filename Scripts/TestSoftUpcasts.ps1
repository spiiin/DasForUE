param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild,[string]$DDC='NoZenLocalFallback')
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs=Join-Path $Repo 'logs'
$Run=[DateTime]::UtcNow.ToString('yyyyMMdd-HHmmss')
$Log=Join-Path $Logs "soft-upcasts-$Run.log"
$OldSkip=$env:UE_SKIP_UBT_SDK_SETUP
$OldTemp=$env:TMP
try {
 New-Item -ItemType Directory -Force -Path $Logs | Out-Null
 $Temp=Join-Path $Logs 'soft-upcasts-temp'
 New-Item -ItemType Directory -Force -Path $Temp | Out-Null
 $env:UE_SKIP_UBT_SDK_SETUP='1';$env:TMP=$Temp
 if(!$SkipBuild){
  Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=12', "-Log=$Logs\soft-upcasts-build-$Run.log")
  if($LASTEXITCODE){throw 'Soft upcast Editor build failed'}
 }
 & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasSoftReferences -UpcastsOnly "-DDC=$DDC" -unattended -NullRHI -nosplash "-abslog=$Log"
 if($LASTEXITCODE -or !(Test-DasResult -LiteralPath $Log -SimpleMatch 'DAS_SOFT_UPCASTS_RESULT=PASS')){throw "Soft upcasts failed: $Log"}
 Write-Output "DAS_SOFT_UPCASTS_FOCUSED=PASS log=$Log"
} finally { $env:UE_SKIP_UBT_SDK_SETUP=$OldSkip;$env:TMP=$OldTemp }
