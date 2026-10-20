param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$Package)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=12')
if($LASTEXITCODE){throw 'Defaults merge editor build failed'}
$Id=[Guid]::NewGuid().ToString('N')
foreach($Phase in @('Create','Reopen','Recompile','Unopened')){
 $Log=Join-Path $Repo "logs\defaults-merge-$Phase.log"
 & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasDefaultsMerge "-ProbeId=$Id" "-Phase=$Phase" -unattended -NullRHI -nosplash -stdout "-abslog=$Log"
 if($LASTEXITCODE -or (Select-String -LiteralPath $Log -Pattern ':\s+Error:|Fatal error:|DAS_DEFAULTS_TEST FAIL') -or !(Test-DasResult -LiteralPath $Log -SimpleMatch 'DAS_DEFAULTS_MERGE_RESULT=PASS')){throw "Defaults merge $Phase failed"}
}
if($Package){
 $Map="/Game/DasDefaultsMerge/$Id/Map"
 & (Join-Path $PSScriptRoot 'TestPackaged.ps1') -Engine $Engine -AdditionalMaps @($Map) -SkipZenStore
 $Exe=Join-Path $Repo 'Build\Packaged\Windows\DasForUESmoke\Binaries\Win64\DasForUESmoke.exe'
 $Log=Join-Path $Repo 'logs\defaults-merge-packaged.log'
 $Process=Start-Process -FilePath $Exe -ArgumentList $Map,'-DasDefaultsMerge','-nullrhi','-unattended','-nosplash',"-abslog=$Log" -WindowStyle Hidden -PassThru -Wait
 if($Process.ExitCode -or (Select-String -LiteralPath $Log -Pattern ':\s+Error:|Fatal error:') -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_DEFAULTS_MERGE_PACKAGED=PASS')){throw 'Packaged defaults merge failed'}
}
Write-Output "DAS_DEFAULTS_MERGE_PROBE=PASS map=/Game/DasDefaultsMerge/$Id/Map"
