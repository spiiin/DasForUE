param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipEditorBuild,[string]$DDC='')
$ErrorActionPreference='Stop'
$DDCArgs=@();if($DDC){$DDCArgs=@("-DDC=$DDC")}
$Root=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Root 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs=Join-Path $Root 'logs'
New-Item -ItemType Directory -Path $Logs -Force | Out-Null
$Previous=$env:UE_SKIP_UBT_SDK_SETUP
try {
 $env:UE_SKIP_UBT_SDK_SETUP='1'
 if(!$SkipEditorBuild){
  & (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12 "-Log=$Logs\soft-references-package-editor-build.log"
  if($LASTEXITCODE){throw 'Soft reference exporter build failed'}
 }
 $Id=[Guid]::NewGuid().ToString('N')
 Set-Content -LiteralPath (Join-Path $Logs 'soft-references-packaged-probe-id.txt') -Value $Id
 $Map="/Game/DasSoftReferences/$Id/Map"
 $ExportLog=Join-Path $Logs 'soft-references-package-export.log'
 & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasSoftReferences "-ProbeId=$Id" -unattended -NullRHI -nosplash "-abslog=$ExportLog" @DDCArgs
 if($LASTEXITCODE -or !(Select-String -LiteralPath $ExportLog -SimpleMatch 'DAS_SOFT_REFERENCES_EXPORT=PASS')){throw 'Soft references export failed'}
 & (Join-Path $Root 'Scripts\TestPackaged.ps1') -Engine $Engine -AdditionalMaps @($Map) -SkipZenStore -DDC $DDC
 $Exe=Join-Path $Root 'Build\Packaged\Windows\DasForUESmoke\Binaries\Win64\DasForUESmoke.exe'
 $Log=Join-Path $Logs 'soft-references-packaged.log'
 $P=Start-Process -FilePath $Exe -ArgumentList $Map,'-DasSoftReferences','-nullrhi','-unattended','-nosplash',"-abslog=$Log" -WindowStyle Hidden -PassThru
 $P.WaitForExit()
 if($P.ExitCode -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_SOFT_REFERENCES_PACKAGED_RESULT=PASS')){throw "Packaged soft references failed; exit=$($P.ExitCode)"}
 Write-Output "DAS_SOFT_REFERENCES_DEVELOPMENT_PACKAGE=PASS map=$Map"
} finally { $env:UE_SKIP_UBT_SDK_SETUP=$Previous }
