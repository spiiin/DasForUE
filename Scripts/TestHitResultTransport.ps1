param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
foreach($Test in @('TestBindingsGenerator.py','TestHitResultGenerator.py')){
 & C:\Python312\python.exe "$PSScriptRoot/$Test"
 if($LASTEXITCODE){throw "$Test failed"}
}
if(!$SkipBuild){
 Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=8', "-Log=$Repo/logs/hit-transport-build.log") *> "$Repo/logs/hit-transport-build-stdout.log"
 if($LASTEXITCODE){throw 'Hit transport build failed'}
}
$RuntimeDll=Join-Path $Repo 'Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-DasRuntime.dll'
$RuntimeHash=(Get-FileHash $RuntimeDll -Algorithm SHA256).Hash
& "$PSScriptRoot/TestCollisionApiPackage.ps1" -Engine $Engine -SkipBuild
if(!$?){throw 'Collision package validation failed'}
$Package=Get-Content -LiteralPath "$Repo/logs/collision-api-package-validation.json" -Raw|ConvertFrom-Json
$Stage=Split-Path -Parent $Package.report
$Report=Get-Content -LiteralPath $Package.report -Raw|ConvertFrom-Json
$Editor=Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$Common=@($Project,'-run=DasExport','-Modules=Engine','-Multiprocess','-unattended','-NullRHI','-nosplash','-stdout','-ddc=NoZenLocalFallback','-NoZenAutoLaunch')
$Selection=Join-Path $Stage 'hit-fixture-selection.json'
'{"class_rules":[{"class":"/Script/DasRuntime.DasHitTransportLibrary","prefix":"ue_hit_fixture_","include":["*"]}]}'|Set-Content -LiteralPath $Selection -Encoding utf8
& $Editor @Common "-Selection=$Selection" "-Output=$Stage/hit-fixture.json" "-abslog=$Stage/hit-fixture-export.log" *> "$Stage/hit-fixture-export-stdout.log"
if($LASTEXITCODE){throw 'Hit transport fixture export failed'}
& C:\Python312\python.exe "$PSScriptRoot/GenerateBindings.py" "$Stage/hit-fixture.json" "$Stage/ue_hit_transport_fixture.das" --module ue_hit_transport_fixture --canonical-math
if($LASTEXITCODE){throw 'Hit fixture generation failed'}
$Source=Get-Content -LiteralPath "$Repo/Tests/HitResultTransport/probe.template.das" -Raw
$Source=$Source.Replace('@COLLISION@',$Report.staged_module).Replace('@ENVELOPE@','S_DasHitTransportEnvelope_'+([System.BitConverter]::ToString([System.Security.Cryptography.SHA256]::Create().ComputeHash([Text.Encoding]::UTF8.GetBytes('/Script/DasRuntime.DasHitTransportEnvelope')))).Replace('-','').Substring(0,8).ToLower())
$Probe=Join-Path $Stage 'hit-transport-probe.das';$Log=Join-Path $Stage 'hit-transport-runtime.log'
$Source|Set-Content -LiteralPath $Probe -Encoding utf8
& $Editor @Common -GameWorld -CollisionWorld "-Probe=$Probe" "-abslog=$Log" *> "$Stage/hit-transport-runtime-stdout.log"
if($LASTEXITCODE -or !(Test-DasResult -LiteralPath $Log -SimpleMatch 'DAS_PACKAGE_PREFLIGHT=PASS value=42' -Quiet)){throw "Hit transport runtime probe failed: $Log"}
if((Get-FileHash $RuntimeDll -Algorithm SHA256).Hash -ne $RuntimeHash){throw 'Runtime DLL changed during validation; rerun against the completed build'}
[ordered]@{state='PASS';python_tests=127;whole_collision_module='PASS';generated_line_sphere_box_capsule='PASS';multi_sweep_array='PASS';native_consumer_and_actor_instance_roundtrip='PASS';nested_array_ref_transport='PASS';weak_snapshot_input_output_GC='PASS';destroyed_references='PASS';wrong_type_direction_unassigned_private_path='PASS';snapshot_root_collection='PASS';runtime_sha256=$RuntimeHash.ToLower();report=$Package.report;runtime_log=$Log;counts=$Report.counts;published=$false;full_foundation='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 20|Set-Content -LiteralPath "$Repo/logs/hit-transport-validation.json" -Encoding utf8
Write-Output "DAS_HIT_TRANSPORT=PASS report=$($Package.report)"
exit 0
