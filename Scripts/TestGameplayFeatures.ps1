param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Logs=Join-Path $Repo 'logs';New-Item -ItemType Directory -Path $Logs -Force|Out-Null
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
if(!$SkipBuild){
 Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=12', "-Log=$Logs/gameplay-features-build.log") *> "$Logs/gameplay-features-build-stdout.log"
 if($LASTEXITCODE){throw 'Gameplay features build failed'}
}
& "$PSScriptRoot/CreatePlayableCharacter.ps1" -Engine $Engine -SkipBuild *> "$Logs/gameplay-features-create.log"
if(!$?){throw 'Gameplay example export failed'}
$Example=Get-Content -LiteralPath "$Logs/playable-example.json" -Raw|ConvertFrom-Json
$Log=Join-Path $Logs 'gameplay-features-game.log'
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project $Example.map -game -DasGameplayFeatures -Multiprocess -unattended -NullRHI -nosplash -stdout "-abslog=$Log" -ddc=NoZenLocalFallback -NoZenAutoLaunch *> "$Logs/gameplay-features-game-stdout.log"
if($LASTEXITCODE -or !(Test-DasResult -LiteralPath $Log -SimpleMatch 'DAS_GAMEPLAY_FEATURES_RESULT=PASS')){throw "Gameplay features check failed: $Log"}
$Checks=@('gameplay_widget_remove_revokes_and_invalid_bindings_refused','gameplay_hud_local_player_text_health_component','gameplay_line_sphere_ignore_math_and_rejected_inputs','gameplay_enhanced_E_trace_interface_pickup_score_hud','gameplay_H_native_damage_component_signal_hud','gameplay_umg_button_deferred_damage','gameplay_health_zero_and_maximum_clamp','gameplay_trace_weak_references_after_destroy','gameplay_endplay_removes_hud_callbacks_input_and_pending_click')
foreach($Check in $Checks){if(!(Select-String -LiteralPath $Log -SimpleMatch "PASS $Check")){throw "Missing focused check: $Check"}}
[ordered]@{result='PASS';map=$Example.map;checks=$Checks;scope='Editor Development; saved demo in fresh -game process; NullRHI; real Enhanced Input injection, collision queries, UMG widgets/delegate, native damage event and teardown';not_run=@('Foundation suite','full PIE suite','packaged build','physical devices/rendering');commit_created=$false}|ConvertTo-Json -Depth 4|Set-Content -LiteralPath "$Logs/gameplay-features-validation.json" -Encoding utf8
Write-Output "DAS_GAMEPLAY_FEATURES_FOCUSED=PASS map=$($Example.map)"
