param([string]$Engine='C:\src\unreal\UnrealEngine',[string]$Report,[switch]$SkipBuild)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
if(!$SkipBuild){
 Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=8', "-Log=$Repo/logs/physics-simulation-build.log") *> "$Repo/logs/physics-simulation-build-stdout.log"
 if($LASTEXITCODE){throw 'Physics simulation commandlet build failed'}
}
if(!$Report){
 & "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile "$Repo/Bindings/Packages/CollisionPhysics.profile.json" -SkipBuild | Tee-Object -Variable PackageOutput
 if(!$?){throw 'Collision API preflight failed'}
 $Match=[regex]::Match(($PackageOutput -join "`n"),'DAS_API_PACKAGE=PASS[^\r\n]*report=(.+/report\.json)')
 if(!$Match.Success){throw 'Missing package report'}
 $Report=$Match.Groups[1].Value.Trim()
}
$Report=(Resolve-Path -LiteralPath $Report).Path
$Definition=Get-Content -LiteralPath $Report -Raw|ConvertFrom-Json
if($Definition.package -ne 'ue_api_collision' -or $Definition.state -ne 'ue_preflight_passed'){throw 'Expected collision preflight report'}
$Stage=Split-Path -Parent $Report
$Validation=Join-Path $Stage ('physics-simulation-'+[guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $Validation|Out-Null
Copy-Item -LiteralPath (Join-Path $Stage ($Definition.staged_module+'.das')) -Destination $Validation
$Source=(Get-Content -LiteralPath "$Repo/Tests/PhysicsSimulation/probe.template.das" -Raw).Replace('@COLLISION@',$Definition.staged_module)
$Source|Set-Content -LiteralPath "$Validation/probe.das" -Encoding utf8
function GetPhysicsHashes {
 $Result=[ordered]@{}
 foreach($Name in @('UnrealEditor-DasRuntime.dll','UnrealEditor-DasEditor.dll')){
  $Result[$Name]=(Get-FileHash -LiteralPath "$Repo/Tests/DasForUESmoke/Binaries/Win64/$Name" -Algorithm SHA256).Hash
 }
 foreach($Name in @('Plugins/DasForUE/Source/DasEditor/Private/DasPhysicsSimulationCommandlet.cpp','Plugins/DasForUE/Source/DasEditor/Public/DasPhysicsSimulationCommandlet.h','Tests/PhysicsSimulation/probe.template.das')){
  $Result[$Name]=(Get-FileHash -LiteralPath "$Repo/$Name" -Algorithm SHA256).Hash
 }
 $Result['generated_module']=(Get-FileHash -LiteralPath (Join-Path $Validation ($Definition.staged_module+'.das')) -Algorithm SHA256).Hash
 return ($Result|ConvertTo-Json -Depth 5 -Compress)
}
$Before=GetPhysicsHashes
$Editor="$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe"
& $Editor $Project -run=DasPhysicsSimulation "-Probe=$Validation/probe.das" -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$Validation/runtime.log" *> "$Validation/stdout.log"
if($LASTEXITCODE){throw "Physics simulation failed: $Validation/runtime.log"}
$Text=Get-Content -LiteralPath "$Validation/runtime.log" -Raw
if(!$Text.Contains('DAS_PHYSICS_SIMULATION=PASS')){throw 'Missing simulation success marker'}
$Passed=@([regex]::Matches($Text,'DAS_PHYSICS_PASS ([A-Za-z0-9_]+)')|ForEach-Object {$_.Groups[1].Value}|Sort-Object -Unique)
if($Passed.Count -ne 8){throw "Expected eight independent native measurements, got $($Passed.Count)"}
if($Text.Contains('Ensure condition failed') -or $Text -match 'Log[^\r\n]*: Error:'){throw "Engine error/ensure during simulation: $Validation/runtime.log"}
if((GetPhysicsHashes) -ne $Before){throw 'Test source, generated module or DLL changed during simulation'}
[ordered]@{state='PASS';report=$Report;runtime_log="$Validation/runtime.log";measurements=$Passed;null_receiver='PASS';fixed_dt_seconds=1.0/60.0;physics_steps=450;simulated_seconds=7.5;simulation='Chaos / transient Game world / synchronous World ticks';hashes=($Before|ConvertFrom-Json);full_foundation='not_run';PIE='not_run';packaged='not_run';published=$false;commit='not_created'}|ConvertTo-Json -Depth 10|Set-Content -LiteralPath "$Validation/result.json" -Encoding utf8
Copy-Item -LiteralPath "$Validation/result.json" -Destination "$Repo/logs/physics-simulation-validation.json" -Force
Write-Output "DAS_PHYSICS_SIMULATION_TEST=PASS report=$Validation/result.json"
