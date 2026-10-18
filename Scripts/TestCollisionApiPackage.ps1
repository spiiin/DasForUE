param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$EngineManifest=Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor.modules'
$EngineBuildId=(Get-Content -LiteralPath $EngineManifest -Raw|ConvertFrom-Json).BuildId
foreach($Test in @('TestApiPackage.py','TestApiPackageReport.py')){
 & C:\Python312\python.exe "$PSScriptRoot/$Test"
 if($LASTEXITCODE){throw "$Test failed"}
}
& "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile "$Repo/Bindings/Packages/CollisionPhysics.profile.json" -SkipBuild:$SkipBuild | Tee-Object -Variable PackageOutput
if(!$?){throw 'Collision package failed'}
$Match=[regex]::Match(($PackageOutput -join "`n"),'DAS_API_PACKAGE=PASS[^\r\n]*report=(.+/report\.json)')
if(!$Match.Success){throw 'Missing successful package report path'}
$ReportPath=$Match.Groups[1].Value.Trim()
$Stage=Split-Path -Parent $ReportPath
$Report=Get-Content -LiteralPath $ReportPath -Raw|ConvertFrom-Json
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
$Editor=Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$Common=@($Project,'-Multiprocess','-unattended','-NullRHI','-nosplash','-stdout','-ddc=NoZenLocalFallback','-NoZenAutoLaunch')
# Fresh native-policy evidence supplements, rather than replaces, generic export diagnostics.
'{"schema_version":1,"modules":["Engine"],"subsystems":[{"id":"collision_physics","modules":["Engine"],"roots":[]}]}' | Set-Content -LiteralPath "$Stage/inventory-profile.json" -Encoding utf8
& $Editor @Common -run=DasNativeGameplay -Coverage "-Profile=$Stage/inventory-profile.json" "-Output=$Stage/inventory.json" "-abslog=$Stage/inventory.log" *> "$Stage/inventory-stdout.log"
if($LASTEXITCODE){throw "Fresh type inventory failed: $Stage/inventory.log"}
$Checks=@(
 @{name='null_receiver';world=$true;error='object handle is null, expired, or belongs to another context';body='let damping=ue_body_GetLinearDamping(C_PrimitiveComponent_5efdd477_null()); if(damping<0.0) { return -1 }'},
 @{name='missing_world';world=$false;error='self is missing or expired';body='var types:array<E_EObjectTypeQuery_e6df5ae8>; var ignored:array<C_Actor_adf65d5b>; let filter=__ue_class_loaded("/Script/Engine.Actor","/Script/CoreUObject.Object",""); let result <- ue_collision_SphereOverlapActors(ue_vector(0.0lf,0.0lf,0.0lf),100.0,types,filter,ignored); if(result.out_returnvalue) { return -1 }'},
 @{name='wrong_receiver';world=$true;error='object does not match requested class';body='let wrong=C_PrimitiveComponent_5efdd477_from(ue_self_object()); if(C_PrimitiveComponent_5efdd477_valid(wrong)) { return -1 }'}
)
foreach($Check in $Checks){
 $Probe=Join-Path $Stage ($Check.name+'.das');$Log=Join-Path $Stage ($Check.name+'.log')
 "options gen2`nrequire $($Report.staged_module)`n[export]`ndef main(input:int):int {`n$($Check.body)`nreturn 42`n}"|Set-Content -LiteralPath $Probe -Encoding utf8
 $Extra=@();if($Check.world){$Extra+='-GameWorld'}
 & $Editor @Common -run=DasExport -Modules=Engine @Extra "-Probe=$Probe" "-abslog=$Log" *> "$Log.stdout"
 if(!$LASTEXITCODE -or !(Select-String -LiteralPath $Log -SimpleMatch $Check.error -Quiet)){throw "Expected rejection $($Check.name) missing: $Log"}
 Write-Output "DAS_COLLISION_API_REJECTION=PASS $($Check.name)"
}
# Compile the entire staged module alongside installed World/Math using explicit namespaces.
$Probe="$Stage/combined.das";$Log="$Stage/combined.log"
"options gen2`nrequire $($Report.staged_module)`nrequire ue_api_world`nrequire ue_api_math`n[export]`ndef main(input:int):int {`nlet actor=ue_api_world::C_Actor_adf65d5b_from(ue_self_object())`nlet scale=ue_api_world::ue_actor_GetActorScale3D(actor)`nif(scale.x!=1.0lf) { return -1 }`nreturn 42`n}"|Set-Content -LiteralPath $Probe -Encoding utf8
& $Editor @Common -run=DasExport -Modules=Engine -GameWorld "-Probe=$Probe" "-abslog=$Log" *> "$Log.stdout"
if($LASTEXITCODE -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_PACKAGE_PREFLIGHT=PASS value=42' -Quiet)){throw "Combined imports failed: $Log"}
& C:\Python312\python.exe "$PSScriptRoot/ReportApiPackage.py" --report $ReportPath --inventory "$Stage/inventory.json" --baseline "$Repo/Build/ApiPackages/ue_api_world/latest-report.json" --baseline "$Repo/Build/ApiPackages/ue_api_math/latest-report.json" --output "$Repo/Bindings/Packages/ue_api_collision.summary.json" --markdown "$Repo/Docs/CollisionPhysicsBlockers.md"
if($LASTEXITCODE){throw 'Collision blocker report failed'}
$Summary=Get-Content -LiteralPath "$Repo/Bindings/Packages/ue_api_collision.summary.json" -Raw|ConvertFrom-Json
if($Summary.blocker_groups.type_or_abi -le 0 -or $Summary.construction_guarded_count -le 0){throw 'Expected type blockers or construction contracts missing'}
if(!($Summary.blockers|Where-Object {$_.function -eq 'LineTraceSingle' -and $_.status -eq 'export_blocked'})){throw 'Missing LineTraceSingle diagnostic'}
$Trace=$Summary.blockers|Where-Object {$_.function -eq 'LineTraceSingle'}
if(!$Trace.native_policy_evidence.type_issues -and !$Trace.native_policy_evidence.marshalling_error){throw 'Missing trace type/marshalling evidence'}
if((Get-Content -LiteralPath $EngineManifest -Raw|ConvertFrom-Json).BuildId -ne $EngineBuildId){throw 'Engine BuildId changed during validation; rerun against the completed build'}
[ordered]@{state='PASS';engine_build_id=$EngineBuildId;python_tests=23;whole_module='PASS';damping_gravity_collision_settings='PASS';overlap_actor_hit_miss_ignore='PASS';overlap_component_result_identity='PASS';combined_world_math='PASS';negative_checks=@($Checks|ForEach-Object {$_.name});counts=$Report.counts;source_sha256=$Report.source_sha256;report=$ReportPath;published=$false;trace_sweep='blocked_not_tested';physics_simulation='not_run';full_foundation='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 20|Set-Content -LiteralPath "$Repo/logs/collision-api-package-validation.json" -Encoding utf8
Write-Output "DAS_COLLISION_API_PACKAGE=PASS report=$ReportPath"
exit 0
