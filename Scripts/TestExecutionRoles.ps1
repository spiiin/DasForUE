param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
& C:\Python312\python.exe "$PSScriptRoot/AuditUEExternGuards.py" --repo $Repo --output "$Repo/logs/execution-role-coverage.json"
if($LASTEXITCODE){throw 'Unguarded UE extern registration found'}
if(!$SkipBuild){
 & "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=8 "-Log=$Repo/logs/execution-role-build.log" *> "$Repo/logs/execution-role-build-stdout.log"
 if($LASTEXITCODE){throw 'Execution roles build failed'}
}
$Dlls=@('DasRuntime','DasEditor')|ForEach-Object {Join-Path $Repo "Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$_.dll"}
$Hashes=@{};foreach($Dll in $Dlls){$Hashes[$Dll]=(Get-FileHash $Dll -Algorithm SHA256).Hash}
$Stage=Join-Path $Repo ('Build/ExecutionRoles/'+[guid]::NewGuid().ToString('N'));New-Item -ItemType Directory -Path $Stage -Force|Out-Null
$Editor=Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$Common=@($Project,'-Multiprocess','-unattended','-NullRHI','-nosplash','-stdout','-ddc=NoZenLocalFallback','-NoZenAutoLaunch')
$Logs=@();$Total=0;$Registrations=0
foreach($Mode in @('sync','fixed-async')){
 $Probe="$Stage/$Mode.das";Copy-Item -LiteralPath "$Repo/Tests/ExecutionRoles/probe.das" -Destination $Probe
 $Log="$Stage/$Mode.log";$Extra=@();if($Mode -eq 'fixed-async'){$Extra+='-AsyncFixed'}
 & $Editor @Common -run=DasExecutionRole @Extra "-Probe=$Probe" "-abslog=$Log" *> "$Stage/$Mode-stdout.log"
 $Match=[regex]::Match((Get-Content -LiteralPath $Log -Raw),'DAS_EXECUTION_ROLES_RESULT=PASS checks=(\d+) externs=(\d+)')
 if($LASTEXITCODE -or !$Match.Success -or [int]$Match.Groups[1].Value -lt 45){throw "Execution role $Mode checks failed: $Log"}
 if(Select-String -LiteralPath $Log -Pattern 'Ensure condition failed|Log[^:]+: Error:' -Quiet){throw "Engine error/ensure during $Mode role checks"}
 $Total += [int]$Match.Groups[1].Value;$Registrations=[int]$Match.Groups[2].Value;$Logs+=$Log
}
# One module preflight exercises the normal reflection/struct/container ABI after
# changing the common registration factory; no broad Foundation suite is run.
& "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile "$Repo/Bindings/Packages/CollisionPhysics.profile.json" -SkipBuild | Tee-Object -Variable PackageOutput
if(!$?){throw 'Ordinary Collision module regression failed'}
$PackageMatch=[regex]::Match(($PackageOutput -join "`n"),'DAS_API_PACKAGE=PASS[^\r\n]*report=(.+/report\.json)')
if(!$PackageMatch.Success){throw 'Missing Collision preflight report'}
foreach($Dll in $Dlls){if((Get-FileHash $Dll -Algorithm SHA256).Hash -ne $Hashes[$Dll]){throw 'DLL changed during execution-role checks; rerun against completed build'}}
[ordered]@{state='PASS';runtime_checks=$Total;guarded_extern_registrations=$Registrations;coverage="$Repo/logs/execution-role-coverage.json";modes=@('sync','fixed_async_1_60');role_and_epoch='PASS';scalar_CMRES_low_level_UE_families='PASS';ordinary_gameplay_UE_ABI='PASS';unscoped_foreign_no_self_bypass='rejected';deferred_cancel_peer_and_error='PASS';reload_reject_preserved_accept_revoked='PASS';ReleaseObject_unregister_Destroy_GC_world_cleanup='PASS';runtime_logs=$Logs;collision_report=$PackageMatch.Groups[1].Value.Trim();dll_sha256=$Hashes;DAS_async_callback_syntax='not_exposed';body_handle_AddForce_transport='not_implemented';Foundation='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 20|Set-Content -LiteralPath "$Repo/logs/execution-role-validation.json" -Encoding utf8
Write-Output "DAS_EXECUTION_ROLES_VALIDATION=PASS checks=$Total registrations=$Registrations stage=$Stage"
