param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
& C:\Python312\python.exe "$PSScriptRoot/TestApiPackageReport.py"
if($LASTEXITCODE){throw 'API execution-context report tests failed'}
& C:\Python312\python.exe "$PSScriptRoot/AuditAsyncPhysics.py" --engine $Engine --output "$Repo/logs/async-physics-source-audit.json"
if($LASTEXITCODE){throw 'Async physics source audit failed'}
if(!$SkipBuild){
 & "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=8 "-Log=$Repo/logs/async-physics-audit-build.log" *> "$Repo/logs/async-physics-audit-build-stdout.log"
 if($LASTEXITCODE){throw 'Async physics audit build failed'}
}
$Dlls=@('DasRuntime','DasEditor')|ForEach-Object {Join-Path $Repo "Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$_.dll"}
$Hashes=@{};foreach($Dll in $Dlls){$Hashes[$Dll]=(Get-FileHash $Dll -Algorithm SHA256).Hash}
$Editor=Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$Common=@($Project,'-Multiprocess','-unattended','-NullRHI','-nosplash','-stdout','-ddc=NoZenLocalFallback','-NoZenAutoLaunch')
$Logs=@()
foreach($Mode in @('sync','fixed-async')){
 $Log="$Repo/logs/async-physics-audit-$Mode.log";$Extra=@();if($Mode -eq 'fixed-async'){$Extra+='-AsyncFixed'}
 & $Editor @Common -run=DasAsyncPhysicsAudit @Extra "-Probe=$Repo/Tests/AsyncPhysicsAudit/guard_probe.das" "-abslog=$Log" *> "$Repo/logs/async-physics-audit-$Mode-stdout.log"
 if($LASTEXITCODE -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_ASYNC_PHYSICS_AUDIT=PASS checks=12' -Quiet)){throw "Async physics $Mode audit failed: $Log"}
 if(Select-String -LiteralPath $Log -Pattern 'Ensure condition failed|Log[^:]+: Error:' -Quiet){throw "Engine error/ensure during $Mode audit"}
 $Logs+=$Log
}
& "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile "$Repo/Bindings/Packages/CollisionPhysics.profile.json" -SkipBuild | Tee-Object -Variable PackageOutput
if(!$?){throw 'Collision guarded package failed'}
$Match=[regex]::Match(($PackageOutput -join "`n"),'DAS_API_PACKAGE=PASS[^\r\n]*report=(.+/report\.json)')
if(!$Match.Success){throw 'Missing successful package report path'}
$ReportPath=$Match.Groups[1].Value.Trim();$Stage=Split-Path -Parent $ReportPath
$Report=Get-Content -LiteralPath $ReportPath -Raw|ConvertFrom-Json
$Getter=@($Report.members|Where-Object {$_.class -eq '/Script/Engine.PrimitiveComponent' -and $_.function -eq 'GetBodyInstanceAsyncPhysicsTickHandle'})
if($Getter.Count -ne 1 -or $Getter[0].status -ne 'export_blocked' -or $Getter[0].reason -notlike '*scoped frozen-game-thread physics callback adapter*'){throw 'Missing explicit async physics execution policy'}
'{"schema_version":1,"modules":["Engine"],"subsystems":[{"id":"collision_physics","modules":["Engine"],"roots":[]}]}'|Set-Content -LiteralPath "$Stage/inventory-profile.json" -Encoding utf8
& $Editor @Common -run=DasNativeGameplay -Coverage "-Profile=$Stage/inventory-profile.json" "-Output=$Stage/inventory.json" "-abslog=$Stage/inventory.log" *> "$Stage/inventory-stdout.log"
if($LASTEXITCODE){throw 'Fresh collision inventory failed'}
& C:\Python312\python.exe "$PSScriptRoot/ReportApiPackage.py" --report $ReportPath --inventory "$Stage/inventory.json" --baseline "$Repo/Build/ApiPackages/ue_api_world/latest-report.json" --baseline "$Repo/Build/ApiPackages/ue_api_math/latest-report.json" --output "$Repo/Bindings/Packages/ue_api_collision.summary.json" --markdown "$Repo/Docs/CollisionPhysicsBlockers.md"
if($LASTEXITCODE){throw 'Async physics blocker report failed'}
foreach($Dll in $Dlls){if((Get-FileHash $Dll -Algorithm SHA256).Hash -ne $Hashes[$Dll]){throw 'DLL changed during audit; rerun against completed build'}}
[ordered]@{state='PASS';python_tests=7;native_runtime_checks=24;modes=@('sync','fixed_async_1_60');source_audit="$Repo/logs/async-physics-source-audit.json";reflection_surface='one_getter_one_AddForce';native_callback_phase='game_thread_internal_physics_context';force_accel_change='measured';recreate_and_destroy_generation='PASS';DAS_getter_consumer='rejected_before_frame_allocation';proposed_DAS_scoped_callback='not_implemented';runtime_logs=$Logs;report=$ReportPath;counts=$Report.counts;dll_sha256=$Hashes;published=$false;Foundation='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 20|Set-Content -LiteralPath "$Repo/logs/async-physics-audit-validation.json" -Encoding utf8
Write-Output "DAS_ASYNC_PHYSICS_AUDIT_VALIDATION=PASS report=$ReportPath"
