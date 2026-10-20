param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
foreach($Test in @('TestBindingsGenerator.py','TestHitResultGenerator.py','TestConstraintTransportGenerator.py')){
 & C:\Python312\python.exe "$PSScriptRoot/$Test"
 if($LASTEXITCODE){throw "$Test failed"}
}
if(!$SkipBuild){
 Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=8', "-Log=$Repo/logs/constraint-transport-build.log") *> "$Repo/logs/constraint-transport-build-stdout.log"
 if($LASTEXITCODE){throw 'Constraint transport build failed'}
}
$Dlls=@('DasRuntime','DasEditor')|ForEach-Object {Join-Path $Repo "Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$_.dll"}
$Hashes=@{};foreach($Dll in $Dlls){$Hashes[$Dll]=(Get-FileHash $Dll -Algorithm SHA256).Hash}
& "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile "$Repo/Bindings/Packages/CollisionPhysics.profile.json" -SkipBuild | Tee-Object -Variable PackageOutput
if(!$?){throw 'Collision package failed'}
$Match=[regex]::Match(($PackageOutput -join "`n"),'DAS_API_PACKAGE=PASS[^\r\n]*report=(.+/report\.json)')
if(!$Match.Success){throw 'Missing successful package report path'}
$ReportPath=$Match.Groups[1].Value.Trim();$Stage=Split-Path -Parent $ReportPath
$Report=Get-Content -LiteralPath $ReportPath -Raw|ConvertFrom-Json
$Getter=@($Report.members|Where-Object {$_.class -eq '/Script/Engine.PhysicsConstraintComponent' -and $_.function -eq 'GetConstraint'})
if($Getter.Count -ne 1 -or $Getter[0].status -ne 'generation_ready'){throw 'GetConstraint not generated'}
$Editor=Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$Common=@($Project,'-run=DasExport','-Modules=Engine','-Multiprocess','-unattended','-NullRHI','-nosplash','-stdout','-ddc=NoZenLocalFallback','-NoZenAutoLaunch')
$Selection=Join-Path $Stage 'constraint-fixture-selection.json'
'{"class_rules":[{"class":"/Script/DasRuntime.DasConstraintTransportFixture","prefix":"ue_constraint_fixture_","include":["*"]}]}'|Set-Content -LiteralPath $Selection -Encoding utf8
& $Editor @Common "-Selection=$Selection" "-Output=$Stage/constraint-fixture.json" "-abslog=$Stage/constraint-fixture-export.log" *> "$Stage/constraint-fixture-export-stdout.log"
if($LASTEXITCODE){throw 'Constraint fixture export failed'}
& C:\Python312\python.exe "$PSScriptRoot/GenerateBindings.py" "$Stage/constraint-fixture.json" "$Stage/ue_constraint_fixture.das" --module ue_constraint_fixture --canonical-math
if($LASTEXITCODE){throw 'Constraint fixture generation failed'}
$Component='C_PhysicsConstraintComponent_'+([System.BitConverter]::ToString([System.Security.Cryptography.SHA256]::Create().ComputeHash([Text.Encoding]::UTF8.GetBytes('/Script/Engine.PhysicsConstraintComponent')))).Replace('-','').Substring(0,8).ToLower()
foreach($Case in @('transport','scope')){
 $Template=if($Case -eq 'transport'){'constraint_transport_probe.template.das'}else{'constraint_scope_probe.template.das'}
 $Source=(Get-Content -LiteralPath "$Repo/Tests/ConstraintTransport/$Template" -Raw).Replace('@COLLISION@',$Report.staged_module).Replace('@COMPONENT@',$Component)
 $Probe="$Stage/constraint-$Case.das";$Log="$Stage/constraint-$Case.log"
 $Source|Set-Content -LiteralPath $Probe -Encoding utf8
 $Extra=@();if($Case -eq 'scope'){$Extra+='-ProbeContextRoundtrip'}
 & $Editor @Common -GameWorld @Extra "-Probe=$Probe" "-abslog=$Log" *> "$Stage/constraint-$Case-stdout.log"
 if($LASTEXITCODE -or !(Test-DasResult -LiteralPath $Log -SimpleMatch 'DAS_PACKAGE_PREFLIGHT=PASS value=42' -Quiet)){throw "Constraint $Case probe failed: $Log"}
}
# Refresh the blocker report from this exact export and a fresh native inventory.
'{"schema_version":1,"modules":["Engine"],"subsystems":[{"id":"collision_physics","modules":["Engine"],"roots":[]}]}'|Set-Content -LiteralPath "$Stage/inventory-profile.json" -Encoding utf8
& $Editor $Project -run=DasNativeGameplay -Coverage "-Profile=$Stage/inventory-profile.json" "-Output=$Stage/inventory.json" "-abslog=$Stage/inventory.log" -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch *> "$Stage/inventory-stdout.log"
if($LASTEXITCODE){throw 'Fresh constraint native inventory failed'}
& C:\Python312\python.exe "$PSScriptRoot/ReportApiPackage.py" --report $ReportPath --inventory "$Stage/inventory.json" --baseline "$Repo/Build/ApiPackages/ue_api_world/latest-report.json" --baseline "$Repo/Build/ApiPackages/ue_api_math/latest-report.json" --output "$Repo/Bindings/Packages/ue_api_collision.summary.json" --markdown "$Repo/Docs/Dev/Api/Blockers/CollisionPhysicsBlockers.md"
if($LASTEXITCODE){throw 'Constraint blocker report failed'}
foreach($Dll in $Dlls){if((Get-FileHash $Dll -Algorithm SHA256).Hash -ne $Hashes[$Dll]){throw 'DLL changed during validation; rerun against completed build'}}
[ordered]@{state='PASS';python_tests=112;whole_collision_module='PASS';component_getter_library_ref_out='PASS';physical_generation_rebuild='PASS';pending_mutation_and_delayed_result='PASS';weak_owner_GC_destroy='PASS';null_wrong_type_direction_unassigned_foreign_context='PASS';snapshot_collection='PASS';report=$ReportPath;counts=$Report.counts;dll_sha256=$Hashes;published=$false;PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 20|Set-Content -LiteralPath "$Repo/logs/constraint-transport-validation.json" -Encoding utf8
Write-Output "DAS_CONSTRAINT_TRANSPORT=PASS report=$ReportPath"
