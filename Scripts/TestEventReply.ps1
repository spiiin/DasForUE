param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild,[switch]$Publish)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop';$Repo=Split-Path -Parent $PSScriptRoot;$Python='C:/Python312/python.exe';$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
& $Python "$PSScriptRoot/TestEventReplyGenerator.py";if($LASTEXITCODE){throw 'EventReply generator tests failed'}
if(!$SkipBuild){Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=8', "-Log=$Repo/logs/event-reply-build.log") *> "$Repo/logs/event-reply-build-stdout.log";if($LASTEXITCODE){throw 'EventReply build failed'}}
$Sources=@{};foreach($File in @('Plugins/DasForUE/Source/DasRuntime/Private/DasRuntime.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasWidgetEventsRuntime.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasEventReplyCalls.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasEventReplyTransport.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasBindingExport.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasMarshalling.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasStructValidation.cpp','Plugins/DasForUE/Source/DasEditor/Private/DasWidgetEventsCommandlet.cpp','Scripts/GenerateBindings.py','Bindings/EventReply.contract.json','Tests/EventReply/probe.das')){$Sources[$File]=(Get-FileHash "$Repo/$File" -Algorithm SHA256).Hash}
$Hashes=@{};foreach($Name in @('DasRuntime','DasEditor')){$Path="$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$Name.dll";$Hashes[$Path]=(Get-FileHash $Path -Algorithm SHA256).Hash}
$Manifest="$Engine/Engine/Binaries/Win64/UnrealEditor.modules";$BuildId=(Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId
& "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile "$Repo/Bindings/Packages/UMGInput.profile.json" -SkipBuild -Publish:$Publish|Tee-Object -Variable PackageOutput
if(!$?){throw 'EventReply package generation failed'}
$Match=[regex]::Match(($PackageOutput -join "`n"),'DAS_API_PACKAGE=PASS[^\r\n]*report=(.+/report\.json)');if(!$Match.Success){throw 'Missing EventReply package report'}
$ReportPath=$Match.Groups[1].Value.Trim();$Stage=Split-Path -Parent $ReportPath;$Report=Get-Content $ReportPath -Raw|ConvertFrom-Json
$Contract=Get-Content "$Repo/Bindings/EventReply.contract.json" -Raw|ConvertFrom-Json
$Open=@($Report.members|Where-Object {$_.status -eq 'generation_ready'}|ForEach-Object {$_.class+'.'+$_.function})
if($Contract.functions.Count -ne 12){throw 'Expected exactly 12 EventReply functions'}
foreach($Name in $Contract.functions){if($Name -notin $Open){throw "EventReply function not ready: $Name"}}
if($Report.counts.generation_ready -lt 394){throw 'Unexpected UMG EventReply coverage'}
$Name='event_reply_probe_'+[guid]::NewGuid().ToString('N')+'.das';$Probe="$Repo/Plugins/DasForUE/Content/Das/Scripts/$Name"
$Source=Get-Content "$Repo/Tests/EventReply/probe.das" -Raw
if(!$Publish){$Source=$Source.Replace('require ue_api_umg_input','require '+$Report.staged_module)}
$Editor="$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe";$Common=@($Project,'-Multiprocess','-unattended','-NullRHI','-nosplash','-stdout','-ddc=NoZenLocalFallback','-NoZenAutoLaunch')
$Log="$Stage/event-reply.log";$Result=$null;$TemporaryModule=$null
if(!$Publish){
 $ModulePath="$Repo/Plugins/DasForUE/Content/Das/Scripts/$($Report.staged_module).das"
 if(!(Test-Path -LiteralPath $ModulePath)){Copy-Item "$Stage/$($Report.staged_module).das" $ModulePath;$TemporaryModule=$ModulePath}
 elseif((Get-FileHash $ModulePath).Hash -ne (Get-FileHash "$Stage/$($Report.staged_module).das").Hash){throw 'Conflicting staged module'}
}
try {
 $Source|Set-Content $Probe -Encoding utf8
 & $Editor @Common -run=DasWidgetEvents -GeneratedReply "-Script=$Name" "-abslog=$Log" *> "$Stage/event-reply-stdout.log"
 $Result=(Get-DasResult -Text (Get-Content $Log -Raw) -Pattern 'DAS_WIDGET_EVENTS_RESULT=PASS checks=(\d+)');if($LASTEXITCODE -or !$Result.Success){throw "Generated EventReply runtime failed: $Log"}
}finally{Remove-Item -LiteralPath $Probe -ErrorAction SilentlyContinue;if($TemporaryModule){Remove-Item -LiteralPath $TemporaryModule}}
$Rejections=0
foreach($Case in @(
 @{name='reply_integer_constructor';body='let value=UEEventReply(uint64(1));return 0'},
 @{name='reply_numeric_cast';body='var value:UEEventReply;return int(uint64(value))'},
 @{name='reply_token_field';body='var value:UEEventReply;return int(value.Token)'})){
 $Bad="$Stage/$($Case.name).das";$BadLog="$Stage/$($Case.name).log"
 "options gen2`nrequire ue_widget_events`n[export]`ndef main(input:int):int { $($Case.body) }"|Set-Content $Bad -Encoding utf8
 & $Editor @Common -run=DasExport "-Modules=Engine,UMG" "-Probe=$Bad" "-abslog=$BadLog" *> "$BadLog.stdout"
 $Text=Get-Content $BadLog -Raw;if(!$LASTEXITCODE -or !$Text.Contains('error[') -or !$Text.Contains('UEEventReply')){throw "Expected EventReply compile rejection: $BadLog"};$Rejections++
}
foreach($File in @('export.log','preflight.log','event-reply.log')){if(Test-DasResult "$Stage/$File" -Pattern 'Ensure condition failed|Log[^:]+: Error:' -Quiet){throw "Engine error/ensure: $File"}}
foreach($File in $Sources.Keys){if((Get-FileHash "$Repo/$File" -Algorithm SHA256).Hash -ne $Sources[$File]){throw "Source changed during EventReply verification: $File"}}
foreach($Path in $Hashes.Keys){if((Get-FileHash $Path -Algorithm SHA256).Hash -ne $Hashes[$Path]){throw 'DLL changed during EventReply verification'}}
if((Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId -ne $BuildId){throw 'Engine BuildId changed during EventReply verification'}
if($Publish){$Report|ConvertTo-Json -Depth 100|Set-Content "$Repo/Bindings/Packages/ue_api_umg_input.summary.json" -Encoding utf8}
[ordered]@{state='PASS';runtime_checks=[int]$Result.Groups[1].Value;compile_rejections=$Rejections;generator_tests=8;opened_functions=12;counts=$Report.counts;whole_module_compile='PASS';source_sha256=$Sources;published=[bool]$Publish;dll_sha256=$Hashes;engine_build_id=$BuildId;report=$ReportPath;runtime_log=$Log;physical_input='not_run';full_suites='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 10|Set-Content "$Repo/logs/event-reply-validation.json" -Encoding utf8
Write-Output "DAS_EVENT_REPLY_VALIDATION=PASS stage=$Stage"
