param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
$Logs=Join-Path $Repo 'logs';New-Item -ItemType Directory -Path $Logs -Force|Out-Null
if(!$SkipBuild){
 & "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12
 if($LASTEXITCODE){throw 'AI Editor build failed'}
}
$Id=[Guid]::NewGuid().ToString('N');$Log="$Logs/ai-example-export.log"
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasCookedGameplay -AIOnly "-ProbeId=$Id" -Multiprocess -unattended -NullRHI -nosplash -stdout "-abslog=$Log" -ddc=NoZenLocalFallback -NoZenAutoLaunch
if($LASTEXITCODE -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_AI_EXPORT=PASS')){throw "AI demo export failed: $Log"}
$Map="/Game/DasExamples/AI/$Id/PlayableMap"
[ordered]@{project=$Project;map=$Map;enemy_source="$Repo/Plugins/DasForUE/Content/Das/Scripts/dsl_enemy_character.das";export_log=$Log}|ConvertTo-Json|Set-Content "$Logs/ai-example.json" -Encoding utf8
Write-Output "Open $Map and press Play. Enemy navigates around the obstacle, pursues the player and attacks every second."
