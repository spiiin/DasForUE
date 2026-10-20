param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
$Logs=Join-Path $Repo 'logs';New-Item -ItemType Directory -Path $Logs -Force|Out-Null
if(!$SkipBuild){
 Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=12', "-Log=$Logs/effects-build.log") *> "$Logs/effects-build-stdout.log"
 if($LASTEXITCODE){throw 'Effects Editor build failed'}
}
& "$PSScriptRoot/CreatePlayableCharacter.ps1" -Engine $Engine -SkipBuild *> "$Logs/effects-create.log"
if(!$?){throw 'Effects example export failed'}
$Example=Get-Content -LiteralPath "$Logs/playable-example.json" -Raw|ConvertFrom-Json
$Log="$Logs/effects-game.log"
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project $Example.map -game -DasEffects -Multiprocess -unattended -RenderOffscreen -windowed -ResX=640 -ResY=360 -nosplash -stdout "-abslog=$Log" -ddc=NoZenLocalFallback -NoZenAutoLaunch *> "$Logs/effects-game-stdout.log"
if($LASTEXITCODE -or !(Test-DasResult -LiteralPath $Log -SimpleMatch 'DAS_EFFECTS_RESULT=PASS')){throw "Effects targeted game checks failed: $Log"}
$Checks=@('effects_pickup_score_and_damage_spawn_owned_audio_niagara','effects_natural_audio_and_niagara_completion_once_and_release','effects_replace_stop_drop_queued_and_preserve_unrelated_component','effects_sound_only_niagara_only_and_stop_all','effects_hot_reload_revokes_components_and_pending_callback','effects_rejected_reload_preserves_effect_and_callback','effects_destroy_cleans_pickup_damage_and_late_callbacks')
foreach($Check in $Checks){if(!(Test-DasResult -LiteralPath $Log -SimpleMatch "PASS $Check")){throw "Missing focused check: $Check"}}
[ordered]@{result='PASS';map=$Example.map;checks=$Checks;scope='Editor Development, fresh saved demo game process, real AudioComponent playback and CPU Niagara simulation; synthetic completion only for cancellation/reload/Destroy boundaries; unique temporary reload probe removed';not_run=@('Foundation suite','full PIE suite','packaged build','audible quality/rendered particles','network');commit_created=$false}|ConvertTo-Json -Depth 4|Set-Content -LiteralPath "$Logs/effects-validation.json" -Encoding utf8
Write-Output "DAS_EFFECTS_FOCUSED=PASS map=$($Example.map)"
