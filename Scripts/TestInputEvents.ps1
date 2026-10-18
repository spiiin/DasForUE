param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild,[switch]$Publish)
$ErrorActionPreference='Stop';$Repo=Split-Path -Parent $PSScriptRoot
$Python='C:/Python312/python.exe';$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
& $Python "$PSScriptRoot/TestInputEventsGenerator.py";if($LASTEXITCODE){throw 'Input event generator tests failed'}
if(!$SkipBuild){& "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=8 "-Log=$Repo/logs/input-events-build.log" *> "$Repo/logs/input-events-build-stdout.log";if($LASTEXITCODE){throw 'Input events build failed'}}
$Hashes=@{};foreach($Name in @('DasRuntime','DasEditor')){$Path="$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$Name.dll";$Hashes[$Path]=(Get-FileHash $Path -Algorithm SHA256).Hash}
$Manifest="$Engine/Engine/Binaries/Win64/UnrealEditor.modules";$BuildId=(Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId
& "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile "$Repo/Bindings/Packages/UMGInput.profile.json" -SkipBuild -Publish:$Publish|Tee-Object -Variable PackageOutput
if(!$?){throw 'Input events package generation failed'}
$Match=[regex]::Match(($PackageOutput -join "`n"),'DAS_API_PACKAGE=PASS[^\r\n]*report=(.+/report\.json)');if(!$Match.Success){throw 'Missing package preflight report'}
$ReportPath=$Match.Groups[1].Value.Trim();$Stage=Split-Path -Parent $ReportPath;$Report=Get-Content $ReportPath -Raw|ConvertFrom-Json
$Contract=Get-Content "$Repo/Bindings/InputEvents.contract.json" -Raw|ConvertFrom-Json
$Open=@($Report.members|Where-Object {$_.status -eq 'generation_ready'}|ForEach-Object {$_.class+'.'+$_.function})
foreach($Name in $Contract.snapshot_function_allowlist){if($Name -notin $Open){throw "Contract function not opened: $Name"}}
foreach($Name in $Contract.deferred_navigation_functions){if($Name -in $Open){throw "Navigation policy bypass: $Name"}}
if($Report.counts.generation_ready -ne 383 -or $Report.counts.export_blocked -ne 114 -or $Report.counts.generation_blocked -ne 20){throw 'Unexpected UMG input-event coverage'}
$Probe="$Stage/input-events.das";(Get-Content "$Repo/Tests/InputEvents/probe.template.das" -Raw).Replace('_MODULE_',$(if($Publish){'ue_api_umg_input'}else{$Report.staged_module}))|Set-Content $Probe -Encoding utf8
$Log="$Stage/input-events.log"
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasInputEvents "-Script=$Probe" -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$Log" *> "$Stage/input-events-stdout.log"
$Result=[regex]::Match((Get-Content $Log -Raw),'DAS_INPUT_EVENTS_RESULT=PASS checks=(\d+)');if($LASTEXITCODE -or !$Result.Success){throw "Input event native/runtime probe failed: $Log"}
$CompileRejections=0
foreach($Case in @(
 @{name='integer_constructor';body='let value=UEKeyEvent(uint64(1));return 0'},
 @{name='numeric_cast';body='var value:UEKeyEvent;return int(uint64(value))'},
 @{name='token_field';body='var value:UEKeyEvent;return int(value.Token)'})){
 $Bad="$Stage/$($Case.name).das";$BadLog="$Stage/$($Case.name).log"
 "options gen2`nrequire ue_input_events`n[export]`ndef main(input:int):int { $($Case.body) }"|Set-Content $Bad -Encoding utf8
 & "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasExport -Modules=Engine "-Probe=$Bad" -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$BadLog" *> "$BadLog.stdout"
 $BadText=Get-Content $BadLog -Raw;if(!$LASTEXITCODE -or !$BadText.Contains('error[') -or !$BadText.Contains('UEKeyEvent')){throw "Expected compile-time token rejection: $BadLog"};$CompileRejections++
}
foreach($Name in @('export.log','preflight.log','input-events.log')){if(Select-String (Join-Path $Stage $Name) -Pattern 'Ensure condition failed|Log[^:]+: Error:' -Quiet){throw "Engine error/ensure: $Name"}}
foreach($Path in $Hashes.Keys){if((Get-FileHash $Path -Algorithm SHA256).Hash -ne $Hashes[$Path]){throw 'DLL changed during tests'}}
if((Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId -ne $BuildId){throw 'Engine BuildId changed during tests'}
if($Publish){$Report|ConvertTo-Json -Depth 100|Set-Content "$Repo/Bindings/Packages/ue_api_umg_input.summary.json" -Encoding utf8}
[ordered]@{state='PASS';compile_rejections=$CompileRejections;generator_tests=9;runtime_checks=[int]$Result.Groups[1].Value;opened_functions=33;navigation_functions_still_closed=3;counts=$Report.counts;whole_module_compile='PASS';dll_sha256=$Hashes;engine_build_id=$BuildId;report=$ReportPath;runtime_log=$Log;published=[bool]$Publish;full_suites='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 12|Set-Content "$Repo/logs/input-events-validation.json" -Encoding utf8
Write-Output "DAS_INPUT_EVENTS_VALIDATION=PASS stage=$Stage"
