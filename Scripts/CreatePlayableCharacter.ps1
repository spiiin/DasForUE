param([string]$Engine='C:\src\unreal\UnrealEngine')
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
& (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12
if($LASTEXITCODE){throw 'Playable demo build failed'}
$Id=[Guid]::NewGuid().ToString('N')
$Log=Join-Path $Repo 'logs\playable-character-export.log'
& (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasCookedGameplay "-ProbeId=$Id" -unattended -NullRHI -nosplash -stdout "-abslog=$Log"
if($LASTEXITCODE -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_PLAYABLE_CHARACTER_MAP=')){throw 'Playable demo export failed'}
Write-Output "Open /Game/DasCookedGameplay/$Id/PlayableMap and press Play. WASD: move; mouse: look; Space: jump."
