param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
$Logs=Join-Path $Repo 'logs';New-Item -ItemType Directory -Path $Logs -Force|Out-Null
if(!$SkipBuild){
 Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=12', "-Log=$Logs/ai-build.log") *> "$Logs/ai-build-stdout.log"
 if($LASTEXITCODE){throw 'AI Editor build failed'}
}
& "$PSScriptRoot/CreateAIEnemy.ps1" -Engine $Engine -SkipBuild *> "$Logs/ai-create.log"
if(!$?){throw 'AI example export failed'}
$Example=Get-Content "$Logs/ai-example.json" -Raw|ConvertFrom-Json
$Log="$Logs/ai-game.log"
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project $Example.map -game -DasAI -Multiprocess -unattended -NullRHI -nosplash -stdout "-abslog=$Log" -ddc=NoZenLocalFallback -NoZenAutoLaunch *> "$Logs/ai-game-stdout.log"
if($LASTEXITCODE -or !(Test-DasResult -LiteralPath $Log -SimpleMatch 'DAS_AI_RESULT=PASS')){throw "AI targeted game check failed: $Log"}
$Checks=@('ai_navmesh_pursuit_obstacle_and_damage','ai_location_move_natural_success_once','ai_already_at_goal_and_failed_request_results','ai_replace_cancel_pending_preserves_external_request_and_listener','ai_no_handler_move_releases_subscription','ai_reload_cancels_active_request_and_pending_callback','ai_rejected_reload_preserves_request_and_callback','ai_destroy_aborts_owned_request_and_removes_delegate')
foreach($Check in $Checks){if(!(Test-DasResult -LiteralPath $Log -SimpleMatch "PASS $Check")){throw "Missing check: $Check"}}
[ordered]@{result='PASS';map=$Example.map;checks=$Checks;scope='Editor Development game process; saved NavMesh, real path following and damage; synthetic completion only for pending callback cancellation';not_run=@('Foundation suite','full PIE suite','packaged build','network','rendering/audio quality');commit_created=$false}|ConvertTo-Json -Depth 4|Set-Content "$Logs/ai-validation.json" -Encoding utf8
Write-Output "DAS_AI_FOCUSED=PASS map=$($Example.map)"
