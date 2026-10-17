param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipEditorBuild)
$ErrorActionPreference='Stop'
$Root=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Root 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs=Join-Path $Root 'logs'
New-Item -ItemType Directory -Path $Logs -Force | Out-Null
$Previous=$env:UE_SKIP_UBT_SDK_SETUP
try {
 $env:UE_SKIP_UBT_SDK_SETUP='1'
 if(!$SkipEditorBuild){
  & (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12 "-Log=$Logs\class-callbacks-package-editor-build.log"
  if($LASTEXITCODE){throw 'Class callback exporter build failed'}
 }
 $Id=[Guid]::NewGuid().ToString('N')
 Set-Content -LiteralPath (Join-Path $Logs 'class-callbacks-packaged-probe-id.txt') -Value $Id
 $Map="/Game/DasClassCallbacks/$Id/Map"
 $ExportLog=Join-Path $Logs 'class-callbacks-package-export.log'
 & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasClassCallbacks "-ProbeId=$Id" -unattended -NullRHI -nosplash "-abslog=$ExportLog"
 if($LASTEXITCODE -or !(Select-String -LiteralPath $ExportLog -SimpleMatch 'DAS_CLASS_CALLBACKS_EXPORT=PASS')){throw 'Class callbacks export failed'}
 & (Join-Path $Root 'Scripts\TestPackaged.ps1') -Engine $Engine -AdditionalMaps @($Map) -SkipZenStore
 $Exe=Join-Path $Root 'Build\Packaged\Windows\DasForUESmoke\Binaries\Win64\DasForUESmoke.exe'
 $Log=Join-Path $Logs 'class-callbacks-packaged.log'
 $P=Start-Process -FilePath $Exe -ArgumentList $Map,'-DasClassCallbacks','-nullrhi','-unattended','-nosplash',"-abslog=$Log" -WindowStyle Hidden -PassThru
 $P.WaitForExit()
 if($P.ExitCode -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_CLASS_CALLBACKS_PACKAGED_RESULT=PASS')){throw "Packaged class callbacks failed; exit=$($P.ExitCode)"}
 Write-Output "DAS_CLASS_CALLBACKS_DEVELOPMENT_PACKAGE=PASS map=$Map"
} finally { $env:UE_SKIP_UBT_SDK_SETUP=$Previous }
