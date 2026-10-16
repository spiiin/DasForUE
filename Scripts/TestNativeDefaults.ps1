param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$Package,[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
if(!$SkipBuild){
 & (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12
 if($LASTEXITCODE){throw 'Native defaults editor build failed'}
}
$Id=[Guid]::NewGuid().ToString('N')
foreach($Phase in @('Create','Reopen','Recompile')){
 $Log=Join-Path $Repo "logs\native-defaults-$Phase.log"
 & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasNativeDefaults "-ProbeId=$Id" "-Phase=$Phase" -unattended -NullRHI -nosplash -stdout "-abslog=$Log" -ddc=NoZenLocalFallback -NoZenAutoLaunch
 if($LASTEXITCODE -or (Select-String -LiteralPath $Log -Pattern ':\s+Error:|Fatal error:|DAS_NATIVE_DEFAULTS FAIL') -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_NATIVE_DEFAULTS_RESULT=PASS')){throw "Native defaults $Phase failed"}
}
Set-Content -LiteralPath (Join-Path $Repo 'logs\native-defaults-probe-id.txt') -Value $Id
if($Package){
 $Map="/Game/DasNativeDefaults/$Id/Map"
 & (Join-Path $PSScriptRoot 'TestPackaged.ps1') -Engine $Engine -AdditionalMaps @($Map) -SkipZenStore
 $Exe=Join-Path $Repo 'Build\Packaged\Windows\DasForUESmoke\Binaries\Win64\DasForUESmoke.exe'
 $Log=Join-Path $Repo 'logs\native-defaults-packaged.log'
 $Process=Start-Process -FilePath $Exe -ArgumentList $Map,'-DasNativeDefaults','-nullrhi','-unattended','-nosplash',"-abslog=$Log" -WindowStyle Hidden -PassThru -Wait
 if($Process.ExitCode -or (Select-String -LiteralPath $Log -Pattern ':\s+Error:|Fatal error:') -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_NATIVE_DEFAULTS_PACKAGED=PASS')){throw 'Packaged native defaults failed'}
}
Write-Output "DAS_NATIVE_DEFAULTS_PROBE=PASS map=/Game/DasNativeDefaults/$Id/Map"
