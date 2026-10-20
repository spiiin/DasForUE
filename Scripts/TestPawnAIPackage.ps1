param([string]$Engine='C:\src\unreal\UnrealEngine',[string]$Report,[switch]$SkipBuild)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
$Python='C:\Python312\python.exe'
if(!$SkipBuild){
 Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=8', "-Log=$Repo/logs/pawn-ai-build.log") *> "$Repo/logs/pawn-ai-build-stdout.log"
 if($LASTEXITCODE){throw 'Pawn AI fixture build failed'}
}
if(!$Report){
 & "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile "$Repo/Bindings/Packages/PawnCharacterAINavigation.profile.json" -SkipBuild | Tee-Object -Variable PackageOutput
 if(!$?){throw 'Pawn AI package failed'}
 $Match=[regex]::Match(($PackageOutput -join "`n"),'DAS_API_PACKAGE=PASS[^\r\n]*report=(.+/report\.json)')
 if(!$Match.Success){throw 'Missing successful package report'}
 $Report=$Match.Groups[1].Value.Trim()
}
$Report=(Resolve-Path -LiteralPath $Report).Path
$Definition=Get-Content -LiteralPath $Report -Raw|ConvertFrom-Json
if($Definition.package -ne 'ue_api_pawn_ai' -or $Definition.state -ne 'ue_preflight_passed'){throw 'Expected successful Pawn AI package preflight'}
$Stage=Split-Path -Parent $Report
$Validation=Join-Path $Stage ('runtime-'+[guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $Validation|Out-Null
Copy-Item -LiteralPath (Join-Path $Stage ($Definition.staged_module+'.das')) -Destination $Validation
$Source=(Get-Content -LiteralPath "$Repo/Tests/PawnAI/probe.template.das" -Raw).Replace('@PAWN_AI@',$Definition.staged_module)
$Source|Set-Content -LiteralPath "$Validation/probe.das" -Encoding utf8
function GetPawnAIHashes {
 $Result=[ordered]@{}
 foreach($Name in @('UnrealEditor-DasRuntime.dll','UnrealEditor-DasEditor.dll')){
  $Result[$Name]=(Get-FileHash -LiteralPath "$Repo/Tests/DasForUESmoke/Binaries/Win64/$Name" -Algorithm SHA256).Hash
 }
 foreach($Name in @('Plugins/DasForUE/Source/DasEditor/Private/DasPawnAICommandlet.cpp','Plugins/DasForUE/Source/DasEditor/Public/DasPawnAICommandlet.h','Tests/PawnAI/probe.template.das','Bindings/Packages/PawnCharacterAINavigation.profile.json')){
  $Result[$Name]=(Get-FileHash -LiteralPath "$Repo/$Name" -Algorithm SHA256).Hash
 }
 $Result['generated_module']=(Get-FileHash -LiteralPath (Join-Path $Validation ($Definition.staged_module+'.das')) -Algorithm SHA256).Hash
 return ($Result|ConvertTo-Json -Depth 5 -Compress)
}
$Before=GetPawnAIHashes
$Editor="$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe"
$Common=@($Project,'-Multiprocess','-unattended','-NullRHI','-nosplash','-stdout','-ddc=NoZenLocalFallback','-NoZenAutoLaunch')
& $Editor @Common -run=DasPawnAI "-Probe=$Validation/probe.das" "-abslog=$Validation/runtime.log" *> "$Validation/stdout.log"
if($LASTEXITCODE){throw "Pawn AI runtime failed: $Validation/runtime.log"}
$Text=Get-Content -LiteralPath "$Validation/runtime.log" -Raw
if(!$Text.Contains('DAS_PAWN_AI_RUNTIME=PASS')){throw 'Missing Pawn AI runtime success marker'}
$Passed=@([regex]::Matches($Text,'DAS_PAWN_AI_PASS ([A-Za-z0-9_]+)')|ForEach-Object {$_.Groups[1].Value}|Sort-Object -Unique)
if($Passed.Count -ne 7){throw "Expected seven native runtime groups, got $($Passed.Count)"}
if($Text.Contains('Ensure condition failed') -or $Text -match 'Log[^\r\n]*: Error:'){throw 'Engine error/ensure in Pawn AI runtime'}
# Compile this package together with installed Math/World using explicit namespaces.
$Combined="options gen2`nrequire $($Definition.staged_module)`nrequire ue_api_world`nrequire ue_api_math`n[export]`ndef main(input:int):int {`nlet pawn=$($Definition.staged_module)::C_Pawn_ba3732bb_null()`nif($($Definition.staged_module)::C_Pawn_ba3732bb_valid(pawn)) { return -1 }`nlet actor=ue_api_world::C_Actor_adf65d5b_null()`nif(ue_api_world::C_Actor_adf65d5b_valid(actor)) { return -2 }`nreturn 42`n}"
$Combined|Set-Content -LiteralPath "$Validation/combined.das" -Encoding utf8
& $Editor @Common -run=DasExport "-Modules=Engine,AIModule,NavigationSystem" "-Probe=$Validation/combined.das" "-abslog=$Validation/combined.log" *> "$Validation/combined-stdout.log"
if($LASTEXITCODE -or !(Select-String -LiteralPath "$Validation/combined.log" -SimpleMatch 'DAS_PACKAGE_PREFLIGHT=PASS value=42' -Quiet)){throw 'Combined Pawn AI / World / Math imports failed'}
# Independent native policy evidence explains every rejected declaration.
'{"schema_version":1,"modules":["Engine","AIModule","NavigationSystem"],"subsystems":[{"id":"pawn_ai_navigation","modules":["Engine","AIModule","NavigationSystem"],"roots":[]}]}'|Set-Content -LiteralPath "$Validation/inventory-profile.json" -Encoding utf8
& $Editor @Common -run=DasNativeGameplay -Coverage "-Profile=$Validation/inventory-profile.json" "-Output=$Validation/inventory.json" "-abslog=$Validation/inventory.log" *> "$Validation/inventory-stdout.log"
if($LASTEXITCODE){throw 'Pawn AI native inventory failed'}
& $Python "$PSScriptRoot/ReportApiPackage.py" --report $Report --inventory "$Validation/inventory.json" --baseline "$Repo/Build/ApiPackages/ue_api_world/latest-report.json" --baseline "$Repo/Build/ApiPackages/ue_api_math/latest-report.json" --baseline "$Repo/Build/ApiPackages/ue_api_collision/latest-report.json" --output "$Repo/Bindings/Packages/ue_api_pawn_ai.summary.json" --markdown "$Repo/Docs/Dev/Api/Blockers/PawnAINavigationBlockers.md"
if($LASTEXITCODE){throw 'Pawn AI blocker report failed'}
if((GetPawnAIHashes) -ne $Before){throw 'Pawn AI DLL, fixture, profile or generated module changed during validation'}
[ordered]@{state='PASS';report=$Report;runtime_log="$Validation/runtime.log";groups=$Passed;whole_module='PASS';combined_world_math='PASS';null_wrong_receiver_and_sense_class='PASS';counts=$Definition.counts;candidate_count=$Definition.candidate_count;hashes=($Before|ConvertFrom-Json);perception_stimuli='not_run';real_navmesh_pathfinding='not_run';character_movement_ticks='not_run';managed_move_to='existing_separate_adapter_not_rerun';full_foundation='not_run';PIE='not_run';packaged='not_run';published=$false;commit='not_created'}|ConvertTo-Json -Depth 12|Set-Content -LiteralPath "$Validation/result.json" -Encoding utf8
Copy-Item -LiteralPath "$Validation/result.json" -Destination "$Repo/logs/pawn-ai-package-validation.json" -Force
Write-Output "DAS_PAWN_AI_PACKAGE=PASS report=$Validation/result.json"
