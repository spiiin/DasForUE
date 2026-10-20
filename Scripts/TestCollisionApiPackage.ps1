param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild,[switch]$Publish)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$EngineManifest=Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor.modules'
$EngineBuildId=(Get-Content -LiteralPath $EngineManifest -Raw|ConvertFrom-Json).BuildId
foreach($Test in @('TestApiPackage.py','TestApiPackageReport.py')){
 & C:\Python312\python.exe "$PSScriptRoot/$Test"
 if($LASTEXITCODE){throw "$Test failed"}
}
& "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile "$Repo/Bindings/Packages/CollisionPhysics.profile.json" -SkipBuild:$SkipBuild -Publish:$Publish | Tee-Object -Variable PackageOutput
if(!$?){throw 'Collision package failed'}
$Match=(Get-DasResult -Text ($PackageOutput -join "`n") -Pattern 'DAS_API_PACKAGE=PASS[^\r\n]*report=(.+/report\.json)')
if(!$Match.Success){throw 'Missing successful package report path'}
$ReportPath=$Match.Groups[1].Value.Trim()
$Stage=Split-Path -Parent $ReportPath
$Report=Get-Content -LiteralPath $ReportPath -Raw|ConvertFrom-Json
$ReceiptState='not_checked'
if($Publish){
 if(!$SkipBuild){
 Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject",
    '-WaitMutex', '-SkipBuild', "-Log=$Stage/receipt.log") *> "$Stage/receipt-stdout.log"
 if($LASTEXITCODE){throw 'Receipt metadata refresh failed'}
 }
 $Receipt=Get-Content "$Repo/Tests/DasForUESmoke/Binaries/Win64/DasForUESmokeEditor.target" -Raw|ConvertFrom-Json
 $Expected=[IO.Path]::GetFullPath("$Repo/Plugins/DasForUE/Content/Das/Scripts/ue_api_collision.das")
 if(!@($Receipt.RuntimeDependencies|Where-Object {$_.Type -eq 'UFS' -and [IO.Path]::GetFullPath($_.Path) -eq $Expected}).Count){throw 'Collision script missing from UFS receipt'}
 $ReceiptState='UFS_verified'
}

# UBT can update the shared BuildId while writing our own metadata.
# Freeze the expected ID after those authorized build steps, before runtime probes.
$EngineBuildId=(Get-Content -LiteralPath $EngineManifest -Raw|ConvertFrom-Json).BuildId
$Dlls=@('DasRuntime','DasEditor')|ForEach-Object {"$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$_.dll"};$DllHashes=@{};foreach($Dll in $Dlls){$DllHashes[$Dll]=(Get-FileHash -LiteralPath $Dll -Algorithm SHA256).Hash}
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
 if(!$LASTEXITCODE -or !(Test-DasResult -LiteralPath $Log -SimpleMatch $Check.error -Quiet)){throw "Expected rejection $($Check.name) missing: $Log"}
 Write-Output "DAS_COLLISION_API_REJECTION=PASS $($Check.name)"
}
# Compile the entire staged module alongside installed World/Math using explicit namespaces.
$Probe="$Stage/combined.das";$Log="$Stage/combined.log"
"options gen2`nrequire $($Report.staged_module)`nrequire ue_api_world`nrequire ue_api_math`n[export]`ndef main(input:int):int {`nlet actor=ue_api_world::C_Actor_adf65d5b_from(ue_self_object())`nlet scale=ue_api_world::ue_actor_GetActorScale3D(actor)`nif(scale.x!=1.0lf) { return -1 }`nreturn 42`n}"|Set-Content -LiteralPath $Probe -Encoding utf8
& $Editor @Common -run=DasExport -Modules=Engine -GameWorld "-Probe=$Probe" "-abslog=$Log" *> "$Log.stdout"
if($LASTEXITCODE -or !(Test-DasResult -LiteralPath $Log -SimpleMatch 'DAS_PACKAGE_PREFLIGHT=PASS value=42' -Quiet)){throw "Combined imports failed: $Log"}
# Public imports resolve through the normal runtime plugin module search, not stage aliases.
$PublicChecks=@()
if($Publish){
 $Installed="$Repo/Plugins/DasForUE/Content/Das/Scripts/ue_api_collision.das"
 if((Get-FileHash -LiteralPath $Installed -Algorithm SHA256).Hash.ToLower() -ne $Report.source_sha256){throw 'Installed source differs from verified package'}
 $ProbeRoot=Join-Path $Stage 'public-probes';New-Item -ItemType Directory -Path $ProbeRoot -Force|Out-Null
 $Definition=Get-Content "$Repo/Bindings/Packages/CollisionPhysics.profile.json" -Raw|ConvertFrom-Json
 $Combined=(Get-Content -LiteralPath "$Stage/combined.das" -Raw).Replace($Report.staged_module,'ue_api_collision')
 foreach($Case in @(@{name='public_standalone';text="options gen2`nrequire ue_api_collision`n$($Definition.probe)"},@{name='public_world_math';text=$Combined})){
  $Probe=Join-Path $ProbeRoot ($Case.name+'.das');$Log=Join-Path $ProbeRoot ($Case.name+'.log');$Case.text|Set-Content -LiteralPath $Probe -Encoding utf8
  & $Editor @Common -run=DasExport -Modules=Engine -GameWorld "-Probe=$Probe" "-abslog=$Log" *> "$Log.stdout"
  if($LASTEXITCODE -or !(Test-DasResult -LiteralPath $Log -SimpleMatch 'DAS_PACKAGE_PREFLIGHT=PASS value=42' -Quiet) -or (Test-DasResult -LiteralPath $Log -Pattern 'Ensure condition failed|Log[^:]+: Error:' -Quiet)){throw "Public require failed: $Log"}
  $PublicChecks+=$Case.name
 }
}
& C:\Python312\python.exe "$PSScriptRoot/ReportApiPackage.py" --report $ReportPath --inventory "$Stage/inventory.json" --baseline "$Repo/Build/ApiPackages/ue_api_world/latest-report.json" --baseline "$Repo/Build/ApiPackages/ue_api_math/latest-report.json" --output "$Repo/Bindings/Packages/ue_api_collision.summary.json" --markdown "$Repo/Docs/Dev/Api/Blockers/CollisionPhysicsBlockers.md"
if($LASTEXITCODE){throw 'Collision blocker report failed'}
$Summary=Get-Content -LiteralPath "$Repo/Bindings/Packages/ue_api_collision.summary.json" -Raw|ConvertFrom-Json
if($Summary.construction_guarded_count -le 0){throw 'Expected construction contracts missing'}
if($Summary.counts.generation_ready -ne 234 -or $Summary.blockers.Count -ne 0 -or $Summary.exclusions.Count -ne 7){throw 'Expected 234 generated functions, zero technical blockers and seven explicit policy exclusions'}
if(@($Summary.exclusions|Where-Object {!$_.alternative -or !$_.export_reason}).Count){throw 'Exclusion evidence or alternatives missing'}
if(($Summary.dependencies.das_modules -join ',') -ne 'ue_foundation' -or ($Summary.dependencies.ue_modules -join ',') -ne 'Engine'){throw 'Unexpected Collision dependencies'}
$ConstraintGetter=@($Summary.members|Where-Object {$_.class -eq '/Script/Engine.PhysicsConstraintComponent' -and $_.function -eq 'GetConstraint'})
if($ConstraintGetter.Count -ne 1 -or $ConstraintGetter[0].status -ne 'generation_ready'){throw 'Expected checked GetConstraint transport'}
$TraceFunctions=@($Summary.members|Where-Object {$_.class -eq '/Script/Engine.KismetSystemLibrary' -and $_.function -match '^(Line|Sphere|Box|Capsule)Trace'})
if($TraceFunctions.Count -ne 24 -or @($TraceFunctions|Where-Object {$_.status -ne 'generation_ready'}).Count){throw 'Expected all 24 generated trace/sweep declarations'}
if((Get-Content -LiteralPath $EngineManifest -Raw|ConvertFrom-Json).BuildId -ne $EngineBuildId){throw 'Engine BuildId changed during validation; rerun against the completed build'}
foreach($Dll in $Dlls){if((Get-FileHash -LiteralPath $Dll -Algorithm SHA256).Hash -ne $DllHashes[$Dll]){throw 'DLL changed during Collision checks'}}
foreach($LogName in @('preflight.log','combined.log','inventory.log')){if(Test-DasResult -LiteralPath (Join-Path $Stage $LogName) -Pattern 'Ensure condition failed|Log[^:]+: Error:' -Quiet){throw "Unexpected engine error: $LogName"}}
[ordered]@{state='PASS';build_receipt=$ReceiptState;dll_sha256=$DllHashes;public_require=$PublicChecks;dependencies=$Summary.dependencies;technical_blockers=$Summary.blockers.Count;policy_exclusions=$Summary.exclusions.Count;engine_build_id=$EngineBuildId;python_tests=27;whole_module='PASS';damping_gravity_collision_settings='PASS';overlap_actor_hit_miss_ignore='PASS';overlap_component_result_identity='PASS';combined_world_math='PASS';negative_checks=@($Checks|ForEach-Object {$_.name});counts=$Report.counts;source_sha256=$Report.source_sha256;report=$ReportPath;published=[bool]$Publish;trace_sweep='compiled_runtime_checked_by_TestHitResultTransport';physics_simulation='not_run';full_foundation='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 20|Set-Content -LiteralPath "$Repo/logs/collision-api-package-validation.json" -Encoding utf8
Write-Output "DAS_COLLISION_API_PACKAGE=PASS report=$ReportPath"
exit 0
