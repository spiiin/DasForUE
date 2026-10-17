param(
    [string]$Engine='C:\src\unreal\UnrealEngine',
    [switch]$SkipBuild,
    [string]$Project
)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
if(!$Project){$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'}
if(!$SkipBuild){
    & (Join-Path $Engine 'Engine/Build/BatchFiles/Build.bat') DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12
    if($LASTEXITCODE){throw 'Playable example build failed'}
}
$Id=[Guid]::NewGuid().ToString('N')
$Logs=Join-Path $Repo 'logs';New-Item -ItemType Directory -Path $Logs -Force|Out-Null
$Log=Join-Path $Logs 'playable-example-export.log'
& (Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe') $Project -run=DasCookedGameplay -PlayableOnly "-ProbeId=$Id" -Multiprocess -unattended -NullRHI -nosplash -stdout "-abslog=$Log" -ddc=NoZenLocalFallback -NoZenAutoLaunch
if($LASTEXITCODE -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_PLAYABLE_EXAMPLE_EXPORT=PASS')){throw 'Playable example export failed'}
$MapMatch=Select-String -LiteralPath $Log -Pattern 'DAS_PLAYABLE_CHARACTER_MAP=(/Game/[^\s]+)'|Select-Object -Last 1
if(!$MapMatch){throw 'Playable example map path missing'}
$Map=$MapMatch.Matches[0].Groups[1].Value
$Manifest=Join-Path $Logs 'playable-example.json'
[ordered]@{project=$Project;map=$Map;source=(Join-Path $Repo 'Plugins/DasForUE/Content/Das/Scripts/dsl_playable_character.das');pickup_source=(Join-Path $Repo 'Plugins/DasForUE/Content/Das/Scripts/dsl_playable_pickup.das');export_log=$Log;mode='playable-only';created_utc=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json|Set-Content -LiteralPath $Manifest -Encoding utf8
Write-Output "Open $Map and press Play. WASD: move; mouse: look; Space: jump; E: interact; H: damage; Tab: mouse/UI."
Write-Output "After its mesh loads, walk into the cube to collect points; it respawns after its configured delay."
Write-Output "Example manifest: $Manifest"
