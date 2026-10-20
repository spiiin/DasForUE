param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$Package,[switch]$PIE)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=12')
if($LASTEXITCODE){throw 'Defaults editor build failed'}
$Id=[Guid]::NewGuid().ToString('N')
foreach($Phase in @('Create','Reopen')){
 $Log=Join-Path $Repo "logs\extended-defaults-$Phase.log"
 & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasExtendedDefaults "-ProbeId=$Id" "-Phase=$Phase" -unattended -NullRHI -nosplash -stdout "-abslog=$Log"
 if($LASTEXITCODE -or !(Test-DasResult -LiteralPath $Log -SimpleMatch 'DAS_EXTENDED_DEFAULTS_RESULT=PASS')){throw "Defaults $Phase failed"}
}
if($PIE){
 $Log=Join-Path $Repo 'logs\extended-defaults-pie.log'
 $Report=Join-Path $Repo 'logs\extended-defaults-pie-report'
 $Arguments=@(('"'+$Project+'"'),'-unattended','-NullRHI','-nosplash','-nosound','-RenderOffscreen',
  '-ExecCmds="Automation RunTests DasForUE.Defaults.RuntimePIE"','-TestExit="Automation Test Queue Empty"',
  ('-ReportExportPath="'+$Report+'"'),('-abslog="'+$Log+'"'))
 $Process=Start-Process -FilePath (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') -ArgumentList $Arguments -WindowStyle Hidden -PassThru
 $Process.WaitForExit()
 if($Process.ExitCode -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_EXTENDED_DEFAULTS_PIE=PASS')){throw 'Defaults PIE failed'}
 $Result=Get-Content -LiteralPath (Join-Path $Report 'index.json') -Raw | ConvertFrom-Json
 if($Result.failed -ne 0 -or $Result.notRun -ne 0 -or $Result.succeeded -ne 1 -or $Result.succeededWithWarnings -ne 0){throw 'Defaults PIE automation did not pass cleanly'}
}
if($Package){
 $Map="/Game/DasExtendedDefaults/$Id/Map"
 & (Join-Path $PSScriptRoot 'TestPackaged.ps1') -Engine $Engine -AdditionalMaps @($Map) -SkipZenStore
 $Exe=Join-Path $Repo 'Build\Packaged\Windows\DasForUESmoke\Binaries\Win64\DasForUESmoke.exe'
 $Log=Join-Path $Repo 'logs\extended-defaults-packaged.log'
 $Process=Start-Process -FilePath $Exe -ArgumentList $Map,'-DasExtendedDefaults','-nullrhi','-unattended','-nosplash',"-abslog=$Log" -WindowStyle Hidden -PassThru -Wait
 if($Process.ExitCode -or (Select-String -LiteralPath $Log -Pattern ':\s+Error:|Fatal error:') -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_EXTENDED_DEFAULTS_PACKAGED=PASS')){throw 'Packaged defaults failed'}
}
Write-Output "DAS_EXTENDED_DEFAULTS_PROBE=PASS map=/Game/DasExtendedDefaults/$Id/Map"
