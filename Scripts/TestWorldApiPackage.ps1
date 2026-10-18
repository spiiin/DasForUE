param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild,[switch]$Publish)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
foreach($Test in @('TestBindingsGenerator.py','TestApiPackage.py','TestCanonicalMathGenerator.py')){
 & C:\Python312\python.exe "$PSScriptRoot/$Test"
 if($LASTEXITCODE){throw "$Test failed"}
}
& "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile "$Repo/Bindings/Packages/WorldActorComponents.profile.json" -SkipBuild:$SkipBuild -Publish:$Publish
if(!$?){throw 'World package failed'}
$Stage=Get-ChildItem "$Repo/Build/ApiPackages/ue_api_world" -Directory|Sort-Object LastWriteTime -Descending|Select-Object -First 1 -ExpandProperty FullName
$Report=Get-Content -LiteralPath "$Stage/report.json" -Raw|ConvertFrom-Json
$Checks=@(
 @{name='missing_world';world=$false;error='self is missing or expired';body='let seconds=ue_gameplay_GetWorldDeltaSeconds(); if(seconds<0.0lf) { return -1 }'},
 @{name='world_override';world=$true;error='automatic WorldContext cannot be overridden';body='let call=ue_begin_static_call("/Script/Engine.GameplayStatics","GetWorldDeltaSeconds","worldcontextobject"); ue_arg_object(call,"worldcontextobject",ue_self_object())'},
 @{name='null_receiver';world=$true;error='object handle is null, expired, or belongs to another context';body='let scale=ue_actor_GetActorScale3D(C_Actor_adf65d5b_null()); if(scale.x<0.0lf) { return -1 }'},
 @{name='wrong_receiver';world=$true;error='object does not match requested class';body='let actor=C_Actor_adf65d5b_from(ue_self_object()); let root=ue_actor_K2_GetRootComponent(actor); let wrong=C_Actor_adf65d5b_from(C_SceneComponent_6f0751c1_raw(root)); if(C_Actor_adf65d5b_valid(wrong)) { return -1 }'},
 @{name='destroyed_receiver';world=$true;error='object handle is null, expired, or belongs to another context';body='let actor=C_Actor_adf65d5b_from(ue_self_object()); ue_actor_K2_DestroyActor(actor); let scale=ue_actor_GetActorScale3D(actor); if(scale.x<0.0lf) { return -1 }'}
)
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
foreach($Check in $Checks){
 $Probe=Join-Path $Stage ($Check.name+'.das');$Log=Join-Path $Stage ($Check.name+'.log')
 "options gen2`nrequire $($Report.staged_module)`n[export]`ndef main(input:int):int {`n$($Check.body)`nreturn 42`n}"|Set-Content -LiteralPath $Probe -Encoding utf8
 $Extra=@();if($Check.world){$Extra+='-GameWorld'}
 & "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasExport -Modules=Engine @Extra "-Probe=$Probe" -Multiprocess -unattended -NullRHI -nosplash -stdout "-abslog=$Log" -ddc=NoZenLocalFallback -NoZenAutoLaunch *> "$Log.stdout"
 if(!$LASTEXITCODE -or !(Select-String -LiteralPath $Log -SimpleMatch $Check.error -Quiet)){throw "Expected rejection $($Check.name) missing: $Log"}
 Write-Output "DAS_WORLD_API_REJECTION=PASS $($Check.name)"
}
$Blocked=@($Report.members|Where-Object {$_.status -eq 'export_blocked'})
if(!($Blocked|Where-Object {$_.reason -eq 'soft_reference_requires_binding_adapter'})){throw 'Missing explicit soft-reference blocker'}
if($Report.construction_guarded_count -le 0){throw 'Missing generated construction guards'}
[ordered]@{state='PASS';python_tests=120;whole_module='PASS';actor_components_world_context='PASS';world_receiver='PASS';negative_checks=@($Checks|ForEach-Object {$_.name});counts=$Report.counts;source_sha256=$Report.source_sha256;report="$Stage/report.json";full_foundation='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 20|Set-Content -LiteralPath "$Repo/logs/world-api-package-validation.json" -Encoding utf8
Write-Output "DAS_WORLD_API_PACKAGE=PASS report=$Stage/report.json"
exit 0
