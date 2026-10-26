param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild,[switch]$Publish)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop';$Repo=Split-Path -Parent $PSScriptRoot;$Python='C:/Python312/python.exe';$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
& $Python "$PSScriptRoot/TestDragDropGenerator.py";if($LASTEXITCODE){throw 'Drag generator tests failed'}
if(!$SkipBuild){Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=8', "-Log=$Repo/logs/drag-build.log") *> "$Repo/logs/drag-build-stdout.log";if($LASTEXITCODE){throw 'Drag build failed'}}
$Sources=@{};foreach($File in @('Plugins/DasForUE/Source/DasRuntime/Private/DasRuntime.cpp','Plugins/DasForUE/Source/DasRuntime/Public/DasRuntime.h','Plugins/DasForUE/Source/DasRuntime/Private/DasDragRuntime.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasDragDropOperation.cpp','Plugins/DasForUE/Source/DasRuntime/Public/DasDragDropOperation.h','Plugins/DasForUE/Source/DasRuntime/Private/DasScriptUserWidget.cpp','Plugins/DasForUE/Source/DasRuntime/Public/DasScriptUserWidget.h','Plugins/DasForUE/Source/DasRuntime/Private/DasInputEvents.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasInputEventsRuntime.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasEventReplyTransport.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasEventReplyCalls.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasWidgetEventsRuntime.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasBindingExport.inl','Plugins/DasForUE/Tests/Editor/Private/DasDragCommandlet.cpp','Plugins/DasForUE/Tests/Editor/Public/DasDragCommandlet.h','Plugins/DasForUE/Content/Das/Scripts/ue_drag_drop.das','Scripts/GenerateBindings.py','Tests/DragDrop/probe.das','Bindings/DragDrop.contract.json','Bindings/EventReply.contract.json')){$Sources[$File]=(Get-FileHash "$Repo/$File" -Algorithm SHA256).Hash}
$Dlls=@{};foreach($Name in @('DasRuntime','DasEditor')){$Path="$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$Name.dll";$Dlls[$Path]=(Get-FileHash $Path -Algorithm SHA256).Hash}
$Manifest="$Engine/Engine/Binaries/Win64/UnrealEditor.modules";$BuildId=(Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId
& "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile "$Repo/Bindings/Packages/UMGInput.profile.json" -SkipBuild -Publish:$Publish|Tee-Object -Variable Output
if(!$?){throw 'Drag API package generation failed'}
$Match=(Get-DasResult -Text ($Output -join "`n") -Pattern 'DAS_API_PACKAGE=PASS[^\r\n]*report=(.+/report\.json)');if(!$Match.Success){throw 'Missing Drag package report'}
$ReportPath=$Match.Groups[1].Value.Trim();$Stage=Split-Path -Parent $ReportPath;$Report=Get-Content $ReportPath -Raw|ConvertFrom-Json
$Member=@($Report.members|Where-Object {$_.class -eq '/Script/UMG.WidgetBlueprintLibrary' -and $_.function -eq 'DetectDragIfPressed'})
if($Member.Count -ne 1 -or $Member[0].status -ne 'generation_ready' -or $Report.counts.generation_ready -lt 404 -or $Report.counts.generation_blocked -ne 0){throw 'Unexpected Drag coverage'}
$Scripts="$Repo/Plugins/DasForUE/Content/Das/Scripts";$Name='drag_probe_'+[guid]::NewGuid().ToString('N')+'.das';$Probe="$Scripts/$Name";$Source=Get-Content "$Repo/Tests/DragDrop/probe.das" -Raw;$TemporaryModule=$null
if(!$Publish){
 $Source=$Source.Replace('require ue_api_umg_input','require '+$Report.staged_module);$ModulePath="$Scripts/$($Report.staged_module).das"
 if(!(Test-Path -LiteralPath $ModulePath)){Copy-Item "$Stage/$($Report.staged_module).das" $ModulePath;$TemporaryModule=$ModulePath}
 elseif((Get-FileHash $ModulePath).Hash -ne (Get-FileHash "$Stage/$($Report.staged_module).das").Hash){throw 'Conflicting staged module'}
}
$Editor="$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe";$Common=@($Project,'-Multiprocess','-unattended','-NullRHI','-nosplash','-stdout','-ddc=NoZenLocalFallback','-NoZenAutoLaunch');$Log="$Stage/drag.log"
try {
 $Source|Set-Content $Probe -Encoding utf8
 & $Editor @Common -run=DasDrag "-Script=$Name" "-abslog=$Log" *> "$Stage/drag-stdout.log"
 $Result=(Get-DasResult -Text (Get-Content $Log -Raw) -Pattern 'DAS_DRAG_RESULT=PASS checks=(\d+)');if($LASTEXITCODE -or !$Result.Success){throw "Drag runtime probe failed: $Log"}
}finally{Remove-Item -LiteralPath $Probe -ErrorAction SilentlyContinue;if($TemporaryModule){Remove-Item -LiteralPath $TemporaryModule}}
foreach($File in @('export.log','preflight.log','drag.log')){if(Test-DasResult "$Stage/$File" -Pattern 'Ensure condition failed|Log[^:]+: Error:' -Quiet){throw "Engine error/ensure: $File"}}
foreach($File in $Sources.Keys){if((Get-FileHash "$Repo/$File" -Algorithm SHA256).Hash -ne $Sources[$File]){throw "Source changed during Drag verification: $File"}}
foreach($Path in $Dlls.Keys){if((Get-FileHash $Path -Algorithm SHA256).Hash -ne $Dlls[$Path]){throw 'DLL changed during Drag verification'}}
if((Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId -ne $BuildId){throw 'Engine BuildId changed during Drag verification'}
$Receipt=Get-Content "$Repo/Tests/DasForUESmoke/Binaries/Win64/DasForUESmokeEditor.target" -Raw|ConvertFrom-Json
if(!@($Receipt.RuntimeDependencies|Where-Object {$_.Path -like '*ue_drag_drop.das' -and $_.Type -eq 'UFS'}).Count){throw 'Drag public module missing from UFS receipt'}
if($Publish){$Report|ConvertTo-Json -Depth 100|Set-Content "$Repo/Bindings/Packages/ue_api_umg_input.summary.json" -Encoding utf8}
[ordered]@{state='PASS';runtime_checks=[int]$Result.Groups[1].Value;generator_tests=8;opened_functions=1;native_drag_events=6;counts=$Report.counts;whole_module_compile='PASS';source_sha256=$Sources;dll_sha256=$Dlls;engine_build_id=$BuildId;report=$ReportPath;runtime_log=$Log;public_module_ufs=$true;published=[bool]$Publish;physical_drag='not_run';full_suites='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 12|Set-Content "$Repo/logs/drag-validation.json" -Encoding utf8
Write-Output "DAS_DRAG_VALIDATION=PASS stage=$Stage"
