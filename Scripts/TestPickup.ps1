param(
    [string]$Engine='C:\src\unreal\UnrealEngine',
    [switch]$SkipBuild,
    [string]$Project
)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Logs=Join-Path $Repo 'logs';New-Item -ItemType Directory -Path $Logs -Force|Out-Null
if(!$Project){$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'}
& C:/Python312/python.exe "$PSScriptRoot/TestNativeGameplayGenerator.py" *> "$Logs/pickup-generator.log"
if($LASTEXITCODE){throw 'Native gameplay generator checks failed'}
if(!$SkipBuild){
    & "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12 "-Log=$Logs/pickup-editor-build.log" *> "$Logs/pickup-editor-build-stdout.log"
    if($LASTEXITCODE){throw 'Pickup Editor build failed'}
}
& "$PSScriptRoot/CreatePlayableCharacter.ps1" -Engine $Engine -SkipBuild -Project $Project *> "$Logs/pickup-create.log"
if(!$?){throw 'Pickup example export failed'}
$Example=Get-Content -LiteralPath "$Logs/playable-example.json" -Raw|ConvertFrom-Json
$Log=Join-Path $Logs 'pickup-game.log'
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project $Example.map -game -DasPickup -Multiprocess -unattended -NullRHI -nosplash -stdout "-abslog=$Log" -ddc=NoZenLocalFallback -NoZenAutoLaunch *> "$Logs/pickup-game-stdout.log"
if($LASTEXITCODE -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_PICKUP_RESULT=PASS')){throw "Pickup game check failed: $Log"}
$Checks=@('pickup_real_non_recipient_overlap_ignored','pickup_real_overlap_score_mesh_collision_timer','pickup_duplicate_callback_guard','pickup_timer_restores_mesh_collision_availability','pickup_reenter_awards_again','pickup_occupied_respawn_reentrant_overlap','pickup_destroy_revokes_timer_and_subscription','pickup_async_cold_mesh_loaded','pickup_async_pending_gate_and_replacement','pickup_async_empty_mesh_unavailable','pickup_async_missing_mesh_unavailable','pickup_async_destroy_suppresses_late_completion')
foreach($Check in $Checks){if(!(Select-String -LiteralPath $Log -SimpleMatch "PASS $Check")){throw "Missing pickup check: $Check"}}
$Folder=Join-Path (Join-Path (Split-Path -Parent $Project) 'Content') ($Example.map -replace '^/Game/','' -replace '/PlayableMap$','')
$Assets=@(Get-ChildItem -LiteralPath $Folder -File|ForEach-Object Name|Sort-Object)
$Expected=@('BP_HealthComponent.uasset','BP_PlayableCharacter.uasset','BP_PlayablePickup.uasset','WBP_HUD.uasset','IA_Interact.uasset','IA_Hurt.uasset','IA_UI.uasset','IA_Jump.uasset','IA_Look.uasset','IA_Move.uasset','IMC_Character.uasset','PickupMesh.uasset','NS_Pickup.uasset','NS_Hurt.uasset','PlayableMap.umap')|Sort-Object
foreach($Asset in $Expected){if($Asset -notin $Assets){throw "Missing standalone pickup asset: $Asset"}}
[ordered]@{result='PASS';project=$Project;map=$Example.map;assets=$Assets;checks=$Checks;generator_checks='TestNativeGameplayGenerator.py PASS';scope='Editor Development; saved map in a fresh -game process; NullRHI, cold async mesh load, empty/missing/replaced/destroyed requests, real overlaps, accelerated respawn timer';not_run=@('Foundation suite','full PIE suite','packaged build','physical devices/rendering');commit_created=$false}|ConvertTo-Json -Depth 4|Set-Content -LiteralPath "$Logs/pickup-validation.json" -Encoding utf8
Write-Output "DAS_PICKUP_FOCUSED=PASS map=$($Example.map)"
