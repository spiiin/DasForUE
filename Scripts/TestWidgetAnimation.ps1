param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild,[switch]$Publish)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop';$Repo=Split-Path -Parent $PSScriptRoot
$Python='C:/Python312/python.exe';$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
& $Python "$PSScriptRoot/TestWidgetAnimationGenerator.py";if($LASTEXITCODE){throw 'WidgetAnimation generator tests failed'}
if(!$SkipBuild){Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=8', "-Log=$Repo/logs/widget-animation-build.log") *> "$Repo/logs/widget-animation-build-stdout.log";if($LASTEXITCODE){throw 'WidgetAnimation build failed'}}
$Sources=@{};foreach($File in @('Plugins/DasForUE/Source/DasRuntime/Private/DasRuntime.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasWidgetAnimationRuntime.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasWidgetAnimationTransport.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasWidgetAnimationTransport.h','Plugins/DasForUE/Source/DasRuntime/Private/DasMarshalling.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasBindingExport.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasStructValidation.cpp','Plugins/DasForUE/Source/DasEditor/Private/DasWidgetAnimationCommandlet.cpp','Scripts/GenerateBindings.py','Bindings/WidgetAnimation.contract.json','Tests/WidgetAnimation/probe.template.das')){$Sources[$File]=(Get-FileHash "$Repo/$File" -Algorithm SHA256).Hash}
$Hashes=@{};foreach($Name in @('DasRuntime','DasEditor')){$Path="$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$Name.dll";$Hashes[$Path]=(Get-FileHash $Path -Algorithm SHA256).Hash}
$Manifest="$Engine/Engine/Binaries/Win64/UnrealEditor.modules";$BuildId=(Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId
& "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile "$Repo/Bindings/Packages/UMGInput.profile.json" -SkipBuild -Publish:$Publish|Tee-Object -Variable PackageOutput
if(!$?){throw 'WidgetAnimation package generation failed'}
$Match=[regex]::Match(($PackageOutput -join "`n"),'DAS_API_PACKAGE=PASS[^\r\n]*report=(.+/report\.json)');if(!$Match.Success){throw 'Missing WidgetAnimation package report'}
$ReportPath=$Match.Groups[1].Value.Trim();$Stage=Split-Path -Parent $ReportPath;$Report=Get-Content $ReportPath -Raw|ConvertFrom-Json
$Contract=Get-Content "$Repo/Bindings/WidgetAnimation.contract.json" -Raw|ConvertFrom-Json
$Open=@($Report.members|Where-Object {$_.status -eq 'generation_ready'}|ForEach-Object {$_.class+'.'+$_.function})
if($Contract.functions.Count -ne 4){throw 'WidgetAnimation contract must attest exactly 4 functions'}
foreach($Name in $Contract.functions){if($Name -notin $Open){throw "WidgetAnimation function not ready: $Name"}}
if($Report.counts.generation_ready -lt 398){throw 'Unexpected UMG WidgetAnimation coverage'}
$Probe="$Stage/widget-animation.das";(Get-Content "$Repo/Tests/WidgetAnimation/probe.template.das" -Raw).Replace('_MODULE_',$(if($Publish){'ue_api_umg_input'}else{$Report.staged_module}))|Set-Content $Probe -Encoding utf8
$Editor="$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe";$Common=@($Project,'-Multiprocess','-unattended','-NullRHI','-nosplash','-stdout','-ddc=NoZenLocalFallback','-NoZenAutoLaunch')
$Log="$Stage/widget-animation.log"
& $Editor @Common -run=DasWidgetAnimation "-Script=$Probe" "-abslog=$Log" *> "$Stage/widget-animation-stdout.log"
$Result=(Get-DasResult -Text (Get-Content $Log -Raw) -Pattern 'DAS_WIDGET_ANIMATION_RESULT=PASS checks=(\d+)');if($LASTEXITCODE -or !$Result.Success){throw "WidgetAnimation native/runtime probe failed: $Log"}
$CompileRejections=0
foreach($Case in @(
 @{name='animation_integer_constructor';body='let value=UEWidgetAnimationHandle(uint64(1));return 0'},
 @{name='animation_numeric_cast';body='var value:UEWidgetAnimationHandle;return int(uint64(value))'},
 @{name='animation_token_field';body='var value:UEWidgetAnimationHandle;return int(value.Token)'})){
 $Bad="$Stage/$($Case.name).das";$BadLog="$Stage/$($Case.name).log"
 "options gen2`nrequire ue_widget_animation`n[export]`ndef main(input:int):int { $($Case.body) }"|Set-Content $Bad -Encoding utf8
 & $Editor @Common -run=DasExport -Modules=Engine "-Probe=$Bad" "-abslog=$BadLog" *> "$BadLog.stdout"
 $BadText=Get-Content $BadLog -Raw;if(!$LASTEXITCODE -or !$BadText.Contains('error[') -or !$BadText.Contains('UEWidgetAnimationHandle')){throw "Expected compile-time WidgetAnimation token rejection: $BadLog"};$CompileRejections++
}
foreach($Name in @('export.log','preflight.log','widget-animation.log')){if(Select-String (Join-Path $Stage $Name) -Pattern 'Ensure condition failed|Log[^:]+: Error:' -Quiet){throw "Engine error/ensure: $Name"}}
foreach($File in $Sources.Keys){if((Get-FileHash "$Repo/$File" -Algorithm SHA256).Hash -ne $Sources[$File]){throw "Source changed during WidgetAnimation tests: $File"}}
foreach($Path in $Hashes.Keys){if((Get-FileHash $Path -Algorithm SHA256).Hash -ne $Hashes[$Path]){throw 'DLL changed during WidgetAnimation tests'}}
if((Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId -ne $BuildId){throw 'Engine BuildId changed during WidgetAnimation tests'}
$Receipt=Get-Content "$Repo/Tests/DasForUESmoke/Binaries/Win64/DasForUESmokeEditor.target" -Raw|ConvertFrom-Json
if(!@($Receipt.RuntimeDependencies|Where-Object {$_.Path -like '*ue_widget_animation.das' -and $_.Type -eq 'UFS'}).Count){throw 'WidgetAnimation public module missing from UFS receipt'}
if($Publish){$Report|ConvertTo-Json -Depth 100|Set-Content "$Repo/Bindings/Packages/ue_api_umg_input.summary.json" -Encoding utf8}
[ordered]@{state='PASS';compile_rejections=$CompileRejections;generator_tests=9;runtime_checks=[int]$Result.Groups[1].Value;opened_functions=4;counts=$Report.counts;whole_module_compile='PASS';source_sha256=$Sources;dll_sha256=$Hashes;engine_build_id=$BuildId;report=$ReportPath;runtime_log=$Log;public_module_ufs=$true;published=[bool]$Publish;viewport_mode='headless real UMG animation and NullSlate; visible window not tested';full_suites='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 12|Set-Content "$Repo/logs/widget-animation-validation.json" -Encoding utf8
Write-Output "DAS_WIDGET_ANIMATION_VALIDATION=PASS stage=$Stage"
