param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$Package)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=12')
if($LASTEXITCODE){throw 'Inherited defaults editor build failed'}
$Id=[Guid]::NewGuid().ToString('N')
foreach($Phase in @('Create','Reopen','Recompile')){
 $Log=Join-Path $Repo "logs\inherited-defaults-$Phase.log"
 & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasInheritedDefaults "-ProbeId=$Id" "-Phase=$Phase" -unattended -NullRHI -nosplash -stdout "-abslog=$Log"
 if($LASTEXITCODE -or (Select-String -LiteralPath $Log -Pattern ':\s+Error:|Fatal error:|DAS_INHERITED_DEFAULTS FAIL') -or !(Test-DasResult -LiteralPath $Log -SimpleMatch 'DAS_INHERITED_DEFAULTS_RESULT=PASS')){throw "Inherited defaults $Phase failed"}
}
if($Package){
 $Map="/Game/DasInheritedDefaults/$Id/Map"
 & (Join-Path $PSScriptRoot 'TestPackaged.ps1') -Engine $Engine -AdditionalMaps @($Map) -SkipZenStore
 $Exe=Join-Path $Repo 'Build\Packaged\Windows\DasForUESmoke\Binaries\Win64\DasForUESmoke.exe'
 $Log=Join-Path $Repo 'logs\inherited-defaults-packaged.log'
 $Process=Start-Process -FilePath $Exe -ArgumentList $Map,'-DasInheritedDefaults','-nullrhi','-unattended','-nosplash',"-abslog=$Log" -WindowStyle Hidden -PassThru -Wait
 if($Process.ExitCode -or (Select-String -LiteralPath $Log -Pattern ':\s+Error:|Fatal error:') -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_INHERITED_DEFAULTS_PACKAGED=PASS')){throw 'Packaged inherited defaults failed'}
}
Write-Output "DAS_INHERITED_DEFAULTS_PROBE=PASS map=/Game/DasInheritedDefaults/$Id/Map"
