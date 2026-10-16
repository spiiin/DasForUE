param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$Package,[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
if(!$SkipBuild){
 & (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12
 if($LASTEXITCODE){throw 'Computed defaults editor build failed'}
}
$Id=[Guid]::NewGuid().ToString('N')
foreach($Phase in @('Create','Reopen','Recompile')){
 $Log=Join-Path $Repo "logs\computed-defaults-$Phase.log"
 & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasComputedDefaults "-ProbeId=$Id" "-Phase=$Phase" -unattended -NullRHI -nosplash -stdout "-abslog=$Log" -ddc=NoZenLocalFallback -NoZenAutoLaunch
 if($LASTEXITCODE -or (Select-String -LiteralPath $Log -Pattern ':\s+Error:|Fatal error:|DAS_COMPUTED_DEFAULTS FAIL') -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_COMPUTED_DEFAULTS_RESULT=PASS')){throw "Computed defaults $Phase failed"}
}
Set-Content -LiteralPath (Join-Path $Repo 'logs\computed-defaults-probe-id.txt') -Value $Id
if($Package){
 $Map="/Game/DasComputedDefaults/$Id/Map"
 & (Join-Path $PSScriptRoot 'TestPackaged.ps1') -Engine $Engine -AdditionalMaps @($Map) -SkipZenStore
 $Exe=Join-Path $Repo 'Build\Packaged\Windows\DasForUESmoke\Binaries\Win64\DasForUESmoke.exe'
 $Log=Join-Path $Repo 'logs\computed-defaults-packaged.log'
 $Process=Start-Process -FilePath $Exe -ArgumentList $Map,'-DasComputedDefaults','-nullrhi','-unattended','-nosplash',"-abslog=$Log" -WindowStyle Hidden -PassThru -Wait
 if($Process.ExitCode -or (Select-String -LiteralPath $Log -Pattern ':\s+Error:|Fatal error:') -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_COMPUTED_DEFAULTS_PACKAGED=PASS')){throw 'Packaged computed defaults failed'}
}
Write-Output "DAS_COMPUTED_DEFAULTS_PROBE=PASS map=/Game/DasComputedDefaults/$Id/Map"
