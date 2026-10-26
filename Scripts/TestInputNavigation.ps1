param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild,[switch]$Publish)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop';$Repo=Split-Path -Parent $PSScriptRoot;$Python='C:/Python312/python.exe';$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
Invoke-DasPythonTests -Python $Python -Path "$PSScriptRoot/TestNavigationGenerator.py";if($LASTEXITCODE){throw 'Navigation generator tests failed'}
if(!$SkipBuild){Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=8', "-Log=$Repo/logs/navigation-build.log") *> "$Repo/logs/navigation-build-stdout.log";if($LASTEXITCODE){throw 'Navigation build failed'}}
$Sources=@{};foreach($File in @('Plugins/DasForUE/Source/DasRuntime/Private/DasRuntime.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasInputNavigation.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasInputEvents.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasInputEventsRuntime.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasWidgetEventsRuntime.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasEventReplyCalls.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasMarshalling.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasBindingExport.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasScriptUserWidget.cpp','Plugins/DasForUE/Source/DasRuntime/Public/DasInputEvents.h','Plugins/DasForUE/Tests/Editor/Private/DasInputNavigationCommandlet.cpp','Plugins/DasForUE/Tests/Editor/Public/DasInputNavigationCommandlet.h','Scripts/GenerateBindings.py','Bindings/InputNavigation.contract.json','Tests/InputNavigation/probe.das')){$Sources[$File]=(Get-FileHash "$Repo/$File" -Algorithm SHA256).Hash}
$Dlls=@{};foreach($Name in @('DasRuntime','DasEditor')){$Path="$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$Name.dll";$Dlls[$Path]=(Get-FileHash $Path -Algorithm SHA256).Hash}
$Manifest="$Engine/Engine/Binaries/Win64/UnrealEditor.modules";$BuildId=(Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId
& "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile "$Repo/Bindings/Packages/UMGInput.profile.json" -SkipBuild -Publish:$Publish|Tee-Object -Variable Output
if(!$?){throw 'Navigation API package generation failed'}
$Match=(Get-DasResult -Text ($Output -join "`n") -Pattern 'DAS_API_PACKAGE=PASS[^\r\n]*report=(.+/report\.json)');if(!$Match.Success){throw 'Missing Navigation package report'}
$ReportPath=$Match.Groups[1].Value.Trim();$Stage=Split-Path -Parent $ReportPath;$Report=Get-Content $ReportPath -Raw|ConvertFrom-Json
$Contract=Get-Content "$Repo/Bindings/InputNavigation.contract.json" -Raw|ConvertFrom-Json
$Opened=@($Report.members|Where-Object {$_.status -eq 'generation_ready'}|ForEach-Object {$_.class+'.'+$_.function})
foreach($Function in $Contract.functions){if($Function -notin $Opened){throw "Navigation function not opened: $Function"}}
$CoverageBaseline=Get-Content "$Repo/Bindings/Packages/ue_api_umg_input.summary.json" -Raw|ConvertFrom-Json
if($CoverageBaseline.package -ne $Report.package -or $CoverageBaseline.engine_version -ne $Report.engine_version){throw 'Navigation coverage baseline targets a different package/engine'}
foreach($Counter in @('generation_ready','export_blocked','generation_blocked','policy_excluded')) {
 if($Report.counts.$Counter -ne $CoverageBaseline.counts.$Counter){throw "Unexpected Navigation coverage: $Counter"}
}
$Scripts="$Repo/Plugins/DasForUE/Content/Das/Scripts";$Name='navigation_probe_'+[guid]::NewGuid().ToString('N')+'.das';$Probe="$Scripts/$Name";$Source=Get-Content "$Repo/Tests/InputNavigation/probe.das" -Raw;$TemporaryModule=$null
if(!$Publish){
 $Source=$Source.Replace('require ue_api_umg_input','require '+$Report.staged_module);$ModulePath="$Scripts/$($Report.staged_module).das"
 if(!(Test-Path -LiteralPath $ModulePath)){Copy-Item "$Stage/$($Report.staged_module).das" $ModulePath;$TemporaryModule=$ModulePath}
 elseif((Get-FileHash $ModulePath).Hash -ne (Get-FileHash "$Stage/$($Report.staged_module).das").Hash){throw 'Conflicting staged module'}
}
$Editor="$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe";$Common=@($Project,'-Multiprocess','-unattended','-NullRHI','-nosplash','-stdout','-ddc=NoZenLocalFallback','-NoZenAutoLaunch');$Log="$Stage/navigation.log"
try {
 $Source|Set-Content $Probe -Encoding utf8
 & $Editor @Common -run=DasInputNavigation "-Script=$Name" "-abslog=$Log" *> "$Stage/navigation-stdout.log"
 $Result=(Get-DasResult -Text (Get-Content $Log -Raw) -Pattern 'DAS_NAVIGATION_RESULT=PASS checks=(\d+)');if($LASTEXITCODE -or !$Result.Success){throw "Navigation runtime probe failed: $Log"}
}finally{Remove-Item -LiteralPath $Probe -ErrorAction SilentlyContinue;if($TemporaryModule){Remove-Item -LiteralPath $TemporaryModule}}
foreach($File in @('export.log','preflight.log','navigation.log')){if(Test-DasResult "$Stage/$File" -Pattern 'Ensure condition failed|Log[^:]+: Error:' -Quiet){throw "Engine error/ensure: $File"}}
foreach($File in $Sources.Keys){if((Get-FileHash "$Repo/$File" -Algorithm SHA256).Hash -ne $Sources[$File]){throw "Source changed during Navigation verification: $File"}}
foreach($Path in $Dlls.Keys){if((Get-FileHash $Path -Algorithm SHA256).Hash -ne $Dlls[$Path]){throw 'DLL changed during Navigation verification'}}
if((Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId -ne $BuildId){throw 'Engine BuildId changed during Navigation verification'}
# No new script module; navigation imports existing ue_widget_events.
$Receipt=Get-Content "$Repo/Tests/DasForUESmoke/Binaries/Win64/DasForUESmokeEditor.target" -Raw|ConvertFrom-Json
if($Publish){$Report|ConvertTo-Json -Depth 100|Set-Content "$Repo/Bindings/Packages/ue_api_umg_input.summary.json" -Encoding utf8}
[ordered]@{state='PASS';runtime_checks=[int]$Result.Groups[1].Value;generator_tests=8;opened_functions=3;counts=$Report.counts;whole_module_compile='PASS';source_sha256=$Sources;dll_sha256=$Dlls;engine_build_id=$BuildId;report=$ReportPath;runtime_log=$Log;published=[bool]$Publish;physical_input='not_run';full_suites='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 12|Set-Content "$Repo/logs/navigation-validation.json" -Encoding utf8
Write-Output "DAS_NAVIGATION_VALIDATION=PASS stage=$Stage"
