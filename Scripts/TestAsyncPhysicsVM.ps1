param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
& C:\Python312\python.exe "$PSScriptRoot/AuditUEExternGuards.py" --repo $Repo --output "$Repo/logs/physics-vm-guard-coverage.json"
if($LASTEXITCODE){throw 'Guard inventory failed'}
if(!$SkipBuild){
 & "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=8 "-Log=$Repo/logs/physics-vm-build.log" *> "$Repo/logs/physics-vm-build-stdout.log"
 if($LASTEXITCODE){throw 'Physics VM build failed'}
}
$Dlls=@('DasRuntime','DasEditor')|ForEach-Object {Join-Path $Repo "Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$_.dll"}
$Hashes=@{};foreach($Dll in $Dlls){$Hashes[$Dll]=(Get-FileHash $Dll -Algorithm SHA256).Hash}
$Stage=Join-Path $Repo ('Build/AsyncPhysicsVM/'+[guid]::NewGuid().ToString('N'));New-Item -ItemType Directory -Path $Stage -Force|Out-Null
$Editor=Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$Common=@($Project,'-Multiprocess','-unattended','-NullRHI','-nosplash','-stdout','-ddc=NoZenLocalFallback','-NoZenAutoLaunch')
$Logs=@();$Total=0
foreach($Mode in @('sync','fixed-async')){
 $Probe="$Stage/$Mode.das";Copy-Item -LiteralPath "$Repo/Tests/AsyncPhysicsVM/physics.das" -Destination $Probe
 $Log="$Stage/$Mode.log";$Extra=@();if($Mode -eq 'fixed-async'){$Extra+='-AsyncFixed'}
 & $Editor @Common -run=DasAsyncPhysicsVM @Extra "-Probe=$Probe" "-abslog=$Log" *> "$Stage/$Mode-stdout.log"
 $Match=[regex]::Match((Get-Content -LiteralPath $Log -Raw),'DAS_PHYSICS_VM_RESULT=PASS checks=(\d+)')
 if($LASTEXITCODE -or !$Match.Success -or [int]$Match.Groups[1].Value -lt 25){throw "Physics VM $Mode failed: $Log"}
 if(Select-String -LiteralPath $Log -Pattern 'Ensure condition failed|Log[^:]+: Error:' -Quiet){throw "Engine error/ensure during physics VM $Mode"}
 $Total += [int]$Match.Groups[1].Value;$Logs+=$Log
}
# Existing native bridge + UE extern role matrix only (no package/full suite).
$RoleChecks=0
foreach($Mode in @('sync','fixed-async')){
 $Probe="$Stage/roles-$Mode.das";Copy-Item -LiteralPath "$Repo/Tests/ExecutionRoles/probe.das" -Destination $Probe
 $Log="$Stage/roles-$Mode.log";$Extra=@();if($Mode -eq 'fixed-async'){$Extra+='-AsyncFixed'}
 & $Editor @Common -run=DasExecutionRole @Extra "-Probe=$Probe" "-abslog=$Log" *> "$Stage/roles-$Mode-stdout.log"
 $Match=[regex]::Match((Get-Content -LiteralPath $Log -Raw),'DAS_EXECUTION_ROLES_RESULT=PASS checks=(\d+) externs=(\d+)')
 if($LASTEXITCODE -or !$Match.Success -or (Select-String -LiteralPath $Log -Pattern 'Ensure condition failed|Log[^:]+: Error:' -Quiet)){throw "Execution role regression $Mode failed: $Log"}
 $RoleChecks += [int]$Match.Groups[1].Value;$Logs+=$Log
}
foreach($Dll in $Dlls){if((Get-FileHash $Dll -Algorithm SHA256).Hash -ne $Hashes[$Dll]){throw 'DLL changed during tests; rerun after build'}}
[ordered]@{state='PASS';checks=$Total;execution_role_regression_checks=$RoleChecks;modes=@('sync','fixed_async_1_60');runtime_logs=$Logs;dll_sha256=$Hashes;mailbox_slots=16;mailbox_type='finite double';Foundation='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 20|Set-Content -LiteralPath "$Repo/logs/physics-vm-validation.json" -Encoding utf8
Write-Output "DAS_PHYSICS_VM_VALIDATION=PASS checks=$Total stage=$Stage"
