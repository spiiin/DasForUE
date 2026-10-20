param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
& C:\Python312\python.exe "$PSScriptRoot/AuditUEExternGuards.py" --repo $Repo --output "$Repo/logs/scoped-body-guard-coverage.json"
if($LASTEXITCODE){throw 'Guard inventory failed'}
if(!$SkipBuild){
 Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=8', "-Log=$Repo/logs/scoped-body-build.log") *> "$Repo/logs/scoped-body-build-stdout.log"
 if($LASTEXITCODE){throw 'Scoped body build failed'}
}
$Dlls=@('DasRuntime','DasEditor')|ForEach-Object {Join-Path $Repo "Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$_.dll"}
$Hashes=@{};foreach($Dll in $Dlls){$Hashes[$Dll]=(Get-FileHash $Dll -Algorithm SHA256).Hash}
$Stage=Join-Path $Repo ('Build/ScopedPhysicsBody/'+[guid]::NewGuid().ToString('N'));New-Item -ItemType Directory -Path $Stage -Force|Out-Null
$Editor=Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$Common=@($Project,'-Multiprocess','-unattended','-NullRHI','-nosplash','-stdout','-ddc=NoZenLocalFallback','-NoZenAutoLaunch')
$Logs=@();$Total=0
foreach($Mode in @('sync','fixed-async')){
 $Probe="$Stage/$Mode.das";Copy-Item -LiteralPath "$Repo/Tests/ScopedPhysicsBody/force.das" -Destination $Probe
 $Log="$Stage/$Mode.log";$Extra=@();if($Mode -eq 'fixed-async'){$Extra+='-AsyncFixed'}
 & $Editor @Common -run=DasScopedPhysicsBody @Extra "-Probe=$Probe" "-abslog=$Log" *> "$Stage/$Mode-stdout.log"
 $Match=(Get-DasResult -Text (Get-Content -LiteralPath $Log -Raw) -Pattern 'DAS_SCOPED_BODY_RESULT=PASS checks=(\d+)')
 if($LASTEXITCODE -or !$Match.Success -or [int]$Match.Groups[1].Value -lt 35){throw "Scoped body $Mode failed: $Log"}
 if(Test-DasResult -LiteralPath $Log -Pattern 'Ensure condition failed|Log[^:]+: Error:' -Quiet){throw "Engine error/ensure during scoped body $Mode"}
 $Total += [int]$Match.Groups[1].Value;$Logs+=$Log
}
# Isolated VM/mailbox and existing role/lifecycle regression only.
& "$PSScriptRoot/TestAsyncPhysicsVM.ps1" -Engine $Engine -SkipBuild
if(!$?){throw 'Physics VM regression failed'}
foreach($Dll in $Dlls){if((Get-FileHash $Dll -Algorithm SHA256).Hash -ne $Hashes[$Dll]){throw 'DLL changed during tests; rerun after build'}}
[ordered]@{state='PASS';body_checks=$Total;regression_report='logs/physics-vm-validation.json';modes=@('sync','fixed_async_1_60');runtime_logs=$Logs;dll_sha256=$Hashes;body_slots=8;force_calls_per_step=64;force_commit='same_presim_step_after_successful_DAS';Foundation='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 20|Set-Content -LiteralPath "$Repo/logs/scoped-body-validation.json" -Encoding utf8
Write-Output "DAS_SCOPED_BODY_VALIDATION=PASS checks=$Total stage=$Stage"
