param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop';$Repo=Split-Path -Parent $PSScriptRoot;$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
if(!$SkipBuild){Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=8', "-Log=$Repo/logs/focus-build.log") *> "$Repo/logs/focus-build-stdout.log";if($LASTEXITCODE){throw 'Focus build failed'}}
$Sources=@{};foreach($File in @('Plugins/DasForUE/Source/DasRuntime/Private/DasRuntime.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasInputEvents.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasInputEventsRuntime.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasWidgetEventsRuntime.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasEventReplyCalls.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasScriptUserWidget.cpp','Plugins/DasForUE/Source/DasRuntime/Public/DasRuntime.h','Plugins/DasForUE/Source/DasRuntime/Public/DasScriptUserWidget.h','Plugins/DasForUE/Tests/Editor/Private/DasFocusEventsCommandlet.cpp','Plugins/DasForUE/Tests/Editor/Public/DasFocusEventsCommandlet.h','Plugins/DasForUE/Content/Das/Scripts/ue_widget_events.das','Tests/FocusEvents/probe.das')){$Sources[$File]=(Get-FileHash "$Repo/$File" -Algorithm SHA256).Hash}
$Sources['Tests/EventReply/probe.das']=(Get-FileHash "$Repo/Tests/EventReply/probe.das" -Algorithm SHA256).Hash
$Sources['Plugins/DasForUE/Content/Das/Scripts/ue_api_umg_input.das']=(Get-FileHash "$Repo/Plugins/DasForUE/Content/Das/Scripts/ue_api_umg_input.das" -Algorithm SHA256).Hash
$Dlls=@{};foreach($Name in @('DasRuntime','DasEditor')){$Path="$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$Name.dll";$Dlls[$Path]=(Get-FileHash $Path -Algorithm SHA256).Hash}
$Manifest="$Engine/Engine/Binaries/Win64/UnrealEditor.modules";$BuildId=(Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId
$Stage="$Repo/logs/focus_"+[guid]::NewGuid().ToString('N');New-Item -ItemType Directory $Stage|Out-Null
$Name='focus_probe_'+[guid]::NewGuid().ToString('N')+'.das';$Probe="$Repo/Plugins/DasForUE/Content/Das/Scripts/$Name";$Log="$Stage/focus.log"
try {
 Copy-Item "$Repo/Tests/FocusEvents/probe.das" $Probe
 & "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch -run=DasFocusEvents "-Script=$Name" "-abslog=$Log" *> "$Stage/focus-stdout.log"
 $Result=(Get-DasResult -Text (Get-Content $Log -Raw) -Pattern 'DAS_FOCUS_RESULT=PASS checks=(\d+)');if($LASTEXITCODE -or !$Result.Success){throw "Focus runtime probe failed: $Log"}
}finally{Remove-Item -LiteralPath $Probe -ErrorAction SilentlyContinue}
if(Test-DasResult $Log -Pattern 'Ensure condition failed|Log[^:]+: Error:' -Quiet){throw "Engine error/ensure: $Log"}
$ReplyName='focus_reply_probe_'+[guid]::NewGuid().ToString('N')+'.das';$ReplyProbe="$Repo/Plugins/DasForUE/Content/Das/Scripts/$ReplyName";$ReplyLog="$Stage/related-event-reply.log"
try {
 Copy-Item "$Repo/Tests/EventReply/probe.das" $ReplyProbe
 & "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch -run=DasWidgetEvents -GeneratedReply "-Script=$ReplyName" "-abslog=$ReplyLog" *> "$Stage/related-event-reply-stdout.log"
 $ReplyResult=(Get-DasResult -Text (Get-Content $ReplyLog -Raw) -Pattern 'DAS_WIDGET_EVENTS_RESULT=PASS checks=(\d+)');if($LASTEXITCODE -or !$ReplyResult.Success){throw "Related EventReply regression failed: $ReplyLog"}
}finally{Remove-Item -LiteralPath $ReplyProbe -ErrorAction SilentlyContinue}
if(Test-DasResult $ReplyLog -Pattern 'Ensure condition failed|Log[^:]+: Error:' -Quiet){throw "Engine error/ensure: $ReplyLog"}
foreach($File in $Sources.Keys){if((Get-FileHash "$Repo/$File" -Algorithm SHA256).Hash -ne $Sources[$File]){throw "Source changed during Focus verification: $File"}}
foreach($Path in $Dlls.Keys){if((Get-FileHash $Path -Algorithm SHA256).Hash -ne $Dlls[$Path]){throw 'DLL changed during Focus verification'}}
if((Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId -ne $BuildId){throw 'Engine BuildId changed during Focus verification'}
[ordered]@{state='PASS';runtime_checks=[int]$Result.Groups[1].Value;related_event_reply_checks=[int]$ReplyResult.Groups[1].Value;source_sha256=$Sources;dll_sha256=$Dlls;engine_build_id=$BuildId;runtime_log=$Log;related_event_reply_log=$ReplyLog;routing='SObjectWidget native focus callbacks with NullSlate';physical_input='not_run';full_suites='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 12|Set-Content "$Repo/logs/focus-validation.json" -Encoding utf8
Write-Output "DAS_FOCUS_VALIDATION=PASS checks=$($Result.Groups[1].Value) log=$Log"
