param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
$Logs=Join-Path $Repo 'logs';New-Item -ItemType Directory -Path $Logs -Force|Out-Null
if(!$SkipBuild){
 Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=12', "-Log=$Logs/montage-build.log") *> "$Logs/montage-build-stdout.log"
 if($LASTEXITCODE){throw 'Montage Editor build failed'}
}
$Id=[Guid]::NewGuid().ToString('N');$Map="/Game/DasMontage/$Id/MontageMap"
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasCookedGameplay -MontageOnly "-ProbeId=$Id" -Multiprocess -unattended -NullRHI -nosplash -stdout "-abslog=$Logs/montage-export.log" -ddc=NoZenLocalFallback -NoZenAutoLaunch *> "$Logs/montage-export-stdout.log"
if($LASTEXITCODE -or !(Test-DasResult -LiteralPath "$Logs/montage-export.log" -SimpleMatch 'DAS_MONTAGE_EXPORT=PASS')){throw "Montage fixture export failed: $Logs/montage-export.log"}
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project $Map -game -DasMontage -Multiprocess -unattended -NullRHI -nosplash -stdout "-abslog=$Logs/montage-game.log" -ddc=NoZenLocalFallback -NoZenAutoLaunch *> "$Logs/montage-game-stdout.log"
if($LASTEXITCODE -or !(Test-DasResult -LiteralPath "$Logs/montage-game.log" -SimpleMatch 'DAS_MONTAGE_RESULT=PASS')){throw "Montage focused checks failed: $Logs/montage-game.log"}
if(!(Test-DasResult -LiteralPath "$Logs/montage-export.log" -SimpleMatch 'PASS montage_bad_callback_rejected_at_authoring')){throw 'Missing callback signature rejection check'}
$Checks=@('montage_terminal_callback_rearms_same_name','montage_other_group_playback_preserved','montage_typed_play_and_owned_subscription','montage_natural_completion_blend_and_real_notify_window','montage_stop_interrupted_once','montage_same_asset_replacement_isolates_instance_ids','montage_cancel_discards_pending_and_preserves_external_listener','montage_hot_reload_revokes_playback_and_queued_events','montage_rejected_reload_preserves_working_callback','montage_destroy_revokes_native_delegates_and_pending_events')
foreach($Check in $Checks){if(!(Test-DasResult -LiteralPath "$Logs/montage-game.log" -SimpleMatch "PASS $Check")){throw "Missing focused check: $Check"}}
[ordered]@{result='PASS';map=$Map;checks=$Checks;authoring_checks=@('montage_bad_callback_rejected_at_authoring');scope='Editor Development game process; real Montage instances advanced with engine APIs and real Montage Notify Window; injected notifications only for negative/cancellation/queued-reload checks; temporary unique reload source removed';not_run=@('Foundation suite','full PIE suite','packaged build','rendered animation/root motion/physical devices');commit_created=$false}|ConvertTo-Json -Depth 4|Set-Content -LiteralPath "$Logs/montage-validation.json" -Encoding utf8
Write-Output "DAS_MONTAGE_FOCUSED=PASS map=$Map"
