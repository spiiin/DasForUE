param([string]$Engine='C:\src\unreal\UnrealEngine')
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs=Join-Path $Repo 'logs'
& (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12
if($LASTEXITCODE){throw 'Cooked gameplay editor build failed'}
$ProbeId=[Guid]::NewGuid().ToString('N')
$Map="/Game/DasCookedGameplay/$ProbeId/Map"
$ExportLog=Join-Path $Logs 'cooked-gameplay-export.log'
& (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasCookedGameplay "-ProbeId=$ProbeId" -unattended -NullRHI -nosplash -stdout "-abslog=$ExportLog"
if($LASTEXITCODE -or !(Select-String -LiteralPath $ExportLog -SimpleMatch 'DAS_COOKED_GAMEPLAY_EXPORT=PASS')){throw 'Cooked gameplay export failed'}
& (Join-Path $PSScriptRoot 'TestPackaged.ps1') -Engine $Engine -AdditionalMaps @($Map,"/Game/DasCookedGameplay/$ProbeId/PlayableMap")
$Exe=Join-Path $Repo 'Build\Packaged\Windows\DasForUESmoke\Binaries\Win64\DasForUESmoke.exe'
$Log=Join-Path $Logs 'cooked-gameplay-runtime.log'
$Process=Start-Process -FilePath $Exe -ArgumentList $Map,'-DasCookedGameplay','-nullrhi','-unattended','-nosplash',"-abslog=$Log" -WindowStyle Hidden -PassThru -Wait
if($Process.ExitCode -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_COOKED_GAMEPLAY_RESULT=PASS')){throw "Cooked gameplay failed ($($Process.ExitCode))"}
Write-Output "DAS_COOKED_GAMEPLAY_PROBE=PASS map=$Map"

$PlayableMap="/Game/DasCookedGameplay/$ProbeId/PlayableMap"
$PlayableLog=Join-Path $Logs 'playable-map-runtime.log'
$Demo=Start-Process -FilePath $Exe -ArgumentList $PlayableMap,'-DasPlayableMap','-nullrhi','-unattended','-nosplash',"-abslog=$PlayableLog" -WindowStyle Hidden -PassThru -Wait
if($Demo.ExitCode -or !(Select-String -LiteralPath $PlayableLog -SimpleMatch 'DAS_PLAYABLE_MAP_RESULT=PASS')){throw "Playable map failed ($($Demo.ExitCode))"}
Write-Output "DAS_PLAYABLE_MAP=PASS map=$PlayableMap"
