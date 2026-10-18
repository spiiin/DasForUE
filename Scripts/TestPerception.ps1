param([string]$Engine='C:\src\unreal\UnrealEngine',[string]$Report,[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
if(!$SkipBuild){
 & "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=8 "-Log=$Repo/logs/perception-build.log" *> "$Repo/logs/perception-build-stdout.log"
 if($LASTEXITCODE){throw 'Perception fixture build failed'}
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
$Run=Join-Path $Stage ('perception-'+[guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $Run|Out-Null
Copy-Item -LiteralPath (Join-Path $Stage ($Definition.staged_module+'.das')) -Destination $Run
(Get-Content -LiteralPath "$Repo/Tests/Perception/probe.template.das" -Raw).Replace('@PAWN_AI@',$Definition.staged_module)|Set-Content -LiteralPath "$Run/probe.das" -Encoding utf8
function GetPerceptionHashes {
 $Hashes=[ordered]@{}
 foreach($Name in @('Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-DasRuntime.dll','Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-DasEditor.dll','Plugins/DasForUE/Source/DasEditor/Private/DasPerceptionCommandlet.cpp','Plugins/DasForUE/Source/DasEditor/Public/DasPerceptionCommandlet.h','Tests/Perception/probe.template.das','Plugins/DasForUE/Content/Das/Scripts/ue_perception.das','Plugins/DasForUE/Source/DasRuntime/Public/DasPerceptionTypes.h','Plugins/DasForUE/Source/DasRuntime/Private/DasPerceptionTypes.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasCallbackFrames.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasAdapterValueTypes.inl')){
  $Hashes[$Name]=(Get-FileHash -LiteralPath "$Repo/$Name" -Algorithm SHA256).Hash
 }
 $Hashes['generated_module']=(Get-FileHash -LiteralPath (Join-Path $Run ($Definition.staged_module+'.das')) -Algorithm SHA256).Hash
 return ($Hashes|ConvertTo-Json -Compress)
}
$Before=GetPerceptionHashes
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasPerception "-Probe=$Run/probe.das" -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$Run/runtime.log" *> "$Run/stdout.log"
if($LASTEXITCODE){throw "Perception runtime failed: $Run/runtime.log"}
$Text=Get-Content -LiteralPath "$Run/runtime.log" -Raw
$Checks=@('real_perception_system','invalid_native_stimulus_is_not_sight','other_native_sense_is_not_sight_or_hearing','raw_stimulus_callback_remains_compatible','unrelated_callback_struct_is_rejected','actor_stimulus_callback_bound','native_sight_detects_source','native_sight_loses_out_of_range_source','native_hearing_receives_real_noise','generated_queries_and_expired_hearing','out_of_range_noise_not_delivered','released_context_unsubscribed','native_stimuli_continue_without_released_das_callback','fresh_context_has_no_stale_callback_state','destroyed_listener_has_no_subscription')
foreach($Check in $Checks){if(!$Text.Contains("DAS_PERCEPTION_PASS $Check")){throw "Missing $Check"}}
if(!$Text.Contains('DAS_PERCEPTION_RUNTIME=PASS') -or $Text.Contains('Ensure condition failed') -or $Text -match 'Log[^\r\n]*: Error:'){throw "Perception errors or missing success: $Run/runtime.log"}
if((GetPerceptionHashes) -ne $Before){throw 'Perception DLL, fixture or generated module changed during validation'}
[ordered]@{state='PASS';native_counters=([regex]::Match($Text,'DAS_PERCEPTION_RUNTIME=PASS ([^\r\n]+)').Groups[1].Value);report=$Report;runtime_log="$Run/runtime.log";checks=$Checks;hashes=($Before|ConvertFrom-Json);scope='real sight/hearing senses and native OnTargetPerceptionUpdated to typed AIStimulus via ue_event; transient Game world';callback='generated ue_event binder and owned AIStimulus projection';stimuli='engine sense processing; no manual RegisterStimulus or delegate Broadcast';scene_saved=$false;PIE='not_run';packaged='not_run';network='not_run';commit='not_created'}|ConvertTo-Json -Depth 6|Set-Content -LiteralPath "$Run/result.json" -Encoding utf8
Copy-Item -LiteralPath "$Run/result.json" -Destination "$Repo/logs/perception-validation.json" -Force
Write-Output "DAS_PERCEPTION_FOCUSED=PASS report=$Run/result.json"
