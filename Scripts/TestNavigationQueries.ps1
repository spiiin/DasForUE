param([string]$Engine='C:\src\unreal\UnrealEngine',[string]$Report,[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
if(!$SkipBuild){
 & "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=8 "-Log=$Repo/logs/navmesh-build.log" *> "$Repo/logs/navmesh-build-stdout.log"
 if($LASTEXITCODE){throw 'Navigation fixture build failed'}
}
if(!$Report){
 & "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile "$Repo/Bindings/Packages/PawnCharacterAINavigation.profile.json" -SkipBuild | Tee-Object -Variable PackageOutput
 if(!$?){throw 'Pawn AI package failed'}
 $Match=[regex]::Match(($PackageOutput -join "`n"),'DAS_API_PACKAGE=PASS[^\r\n]*report=(.+/report\.json)')
 if(!$Match.Success){throw 'Missing package report'}
 $Report=$Match.Groups[1].Value.Trim()
}
$Report=(Resolve-Path -LiteralPath $Report).Path
$Definition=Get-Content -LiteralPath $Report -Raw|ConvertFrom-Json
if($Definition.package -ne 'ue_api_pawn_ai' -or $Definition.state -ne 'ue_preflight_passed'){throw 'Expected Pawn AI preflight report'}
$Stage=Split-Path -Parent $Report
$Run=Join-Path $Stage ('navigation-'+[guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $Run|Out-Null
Copy-Item -LiteralPath (Join-Path $Stage ($Definition.staged_module+'.das')) -Destination $Run
(Get-Content -LiteralPath "$Repo/Tests/Navigation/probe.template.das" -Raw).Replace('@PAWN_AI@',$Definition.staged_module)|Set-Content -LiteralPath "$Run/probe.das" -Encoding utf8
function GetNavigationHashes {
 $Hashes=[ordered]@{}
 foreach($Name in @('Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-DasRuntime.dll','Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-DasEditor.dll','Plugins/DasForUE/Source/DasEditor/Private/DasNavigationCommandlet.cpp','Plugins/DasForUE/Source/DasEditor/Public/DasNavigationCommandlet.h','Tests/Navigation/probe.template.das')){
  $Hashes[$Name]=(Get-FileHash -LiteralPath "$Repo/$Name" -Algorithm SHA256).Hash
 }
 $Hashes['generated_module']=(Get-FileHash -LiteralPath (Join-Path $Run ($Definition.staged_module+'.das')) -Algorithm SHA256).Hash
 return ($Hashes|ConvertTo-Json -Compress)
}
$Before=GetNavigationHashes
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasNavigation "-Probe=$Run/probe.das" -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$Run/runtime.log" *> "$Run/stdout.log"
if($LASTEXITCODE){throw "Navigation runtime failed: $Run/runtime.log"}
$Text=Get-Content -LiteralPath "$Run/runtime.log" -Raw
$Checks=@('asset_compilation_navigation_unlock','recast_covers_start_goal_and_disconnected_island','native_route_detours_around_obstacle','generated_project_random_and_raycast','generated_synchronous_paths_cost_and_length','generated_disconnected_island_and_projection_failure')
foreach($Check in $Checks){if(!$Text.Contains("DAS_NAVIGATION_PASS $Check")){throw "Missing $Check"}}
if(!$Text.Contains('DAS_NAVIGATION_RUNTIME=PASS') -or $Text.Contains('Ensure condition failed') -or $Text -match 'Log[^\r\n]*: Error:'){throw "Navigation errors or missing success: $Run/runtime.log"}
if((GetNavigationHashes) -ne $Before){throw 'Navigation DLL, fixture or generated module changed during validation'}
[ordered]@{state='PASS';native_metrics=([regex]::Match($Text,'DAS_NAVIGATION_METRICS ([^\r\n]+)').Groups[1].Value);report=$Report;runtime_log="$Run/runtime.log";checks=$Checks;hashes=($Before|ConvertFrom-Json);scope='transient Recast NavMesh, static generated queries, Editor Development commandlet';minimum_successful_random_samples_per_query=8;maximum_random_attempts_per_query=64;scene_saved=$false;PIE='not_run';packaged='not_run';movement_ticks='not_run';network='not_run';commit='not_created'}|ConvertTo-Json -Depth 6|Set-Content -LiteralPath "$Run/result.json" -Encoding utf8
Copy-Item -LiteralPath "$Run/result.json" -Destination "$Repo/logs/navigation-validation.json" -Force
Write-Output "DAS_NAVIGATION_FOCUSED=PASS report=$Run/result.json"
