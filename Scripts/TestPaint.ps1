param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild,[switch]$Publish)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop';$Repo=Split-Path -Parent $PSScriptRoot
$Python='C:/Python312/python.exe';$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
& $Python "$PSScriptRoot/TestPaintGenerator.py";if($LASTEXITCODE){throw 'Paint generator tests failed'}
if(!$SkipBuild){Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=8', "-Log=$Repo/logs/paint-build.log") *> "$Repo/logs/paint-build-stdout.log";if($LASTEXITCODE){throw 'Paint build failed'}}
$Sources=@{};foreach($File in @('Plugins/DasForUE/Source/DasRuntime/Private/DasRuntime.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasPaintRuntime.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasPaintTransport.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasPaintTransport.h','Plugins/DasForUE/Source/DasRuntime/Private/DasScriptUserWidget.cpp','Plugins/DasForUE/Source/DasRuntime/Public/DasScriptUserWidget.h','Plugins/DasForUE/Source/DasRuntime/Public/DasRuntime.h','Plugins/DasForUE/Source/DasRuntime/Private/DasMarshalling.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasBindingExport.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasStructValidation.cpp','Plugins/DasForUE/Source/DasEditor/Private/DasPaintCommandlet.cpp','Scripts/GenerateBindings.py','Bindings/PaintContext.contract.json','Tests/Paint/probe.template.das','Plugins/DasForUE/Content/Das/Scripts/ue_paint.das','Plugins/DasForUE/Source/DasEditor/Public/DasPaintCommandlet.h')){$Sources[$File]=(Get-FileHash "$Repo/$File" -Algorithm SHA256).Hash}
$Hashes=@{};foreach($Name in @('DasRuntime','DasEditor')){$Path="$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$Name.dll";$Hashes[$Path]=(Get-FileHash $Path -Algorithm SHA256).Hash}
$Manifest="$Engine/Engine/Binaries/Win64/UnrealEditor.modules";$BuildId=(Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId
& "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile "$Repo/Bindings/Packages/UMGInput.profile.json" -SkipBuild -Publish:$Publish|Tee-Object -Variable PackageOutput
if(!$?){throw 'Paint package generation failed'}
$Match=[regex]::Match(($PackageOutput -join "`n"),'DAS_API_PACKAGE=PASS[^\r\n]*report=(.+/report\.json)');if(!$Match.Success){throw 'Missing Paint package report'}
$ReportPath=$Match.Groups[1].Value.Trim();$Stage=Split-Path -Parent $ReportPath;$Report=Get-Content $ReportPath -Raw|ConvertFrom-Json
$Contract=Get-Content "$Repo/Bindings/PaintContext.contract.json" -Raw|ConvertFrom-Json
$Open=@($Report.members|Where-Object {$_.status -eq 'generation_ready'}|ForEach-Object {$_.class+'.'+$_.function})
if($Contract.functions.Count -ne 5){throw 'Paint contract must attest exactly 5 functions'}
foreach($Name in $Contract.functions){if($Name -notin $Open){throw "Paint function not ready: $Name"}}
if($Report.counts.generation_ready -lt 403 -or $Report.counts.generation_blocked -ne 0){throw 'Unexpected UMG Paint coverage'}
$Name='paint_probe_'+[guid]::NewGuid().ToString('N')+'.das';$Probe="$Repo/Plugins/DasForUE/Content/Das/Scripts/$Name"
$Source=Get-Content "$Repo/Tests/Paint/probe.template.das" -Raw
if(!$Publish){$Source=$Source.Replace('require ue_api_umg_input','require '+$Report.staged_module)}
$Editor="$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe";$Common=@($Project,'-Multiprocess','-unattended','-NullRHI','-nosplash','-stdout','-ddc=NoZenLocalFallback','-NoZenAutoLaunch')
$Log="$Stage/paint.log";$TemporaryModule=$null
if(!$Publish){
 $ModulePath="$Repo/Plugins/DasForUE/Content/Das/Scripts/$($Report.staged_module).das"
 if(!(Test-Path -LiteralPath $ModulePath)){Copy-Item "$Stage/$($Report.staged_module).das" $ModulePath;$TemporaryModule=$ModulePath}
 elseif((Get-FileHash $ModulePath).Hash -ne (Get-FileHash "$Stage/$($Report.staged_module).das").Hash){throw 'Conflicting staged module'}
}
try {
 $Source|Set-Content $Probe -Encoding utf8
 & $Editor @Common -run=DasPaint "-Script=$Name" "-abslog=$Log" *> "$Stage/paint-stdout.log"
 $Result=(Get-DasResult -Text (Get-Content $Log -Raw) -Pattern 'DAS_PAINT_RESULT=PASS checks=(\d+)');if($LASTEXITCODE -or !$Result.Success){throw "Paint native/runtime probe failed: $Log"}
}finally{Remove-Item -LiteralPath $Probe -ErrorAction SilentlyContinue;if($TemporaryModule){Remove-Item -LiteralPath $TemporaryModule}}
$CompileRejections=0
foreach($Case in @(
 @{name='paint_integer_constructor';body='let value=UEPaintContext(uint64(1));return 0'},
 @{name='paint_numeric_cast';body='var value:UEPaintContext;return int(uint64(value))'},
 @{name='paint_token_field';body='var value:UEPaintContext;return int(value.Token)'})){
 $Bad="$Stage/$($Case.name).das";$BadLog="$Stage/$($Case.name).log"
 "options gen2`nrequire ue_paint`n[export]`ndef main(input:int):int { $($Case.body) }"|Set-Content $Bad -Encoding utf8
 & $Editor @Common -run=DasExport -Modules=Engine "-Probe=$Bad" "-abslog=$BadLog" *> "$BadLog.stdout"
 $BadText=Get-Content $BadLog -Raw;if(!$LASTEXITCODE -or !$BadText.Contains('error[') -or !$BadText.Contains('UEPaintContext')){throw "Expected compile-time Paint token rejection: $BadLog"};$CompileRejections++
}
foreach($Name in @('export.log','preflight.log','paint.log')){if(Select-String (Join-Path $Stage $Name) -Pattern 'Ensure condition failed|Log[^:]+: Error:' -Quiet){throw "Engine error/ensure: $Name"}}
foreach($File in $Sources.Keys){if((Get-FileHash "$Repo/$File" -Algorithm SHA256).Hash -ne $Sources[$File]){throw "Source changed during Paint tests: $File"}}
foreach($Path in $Hashes.Keys){if((Get-FileHash $Path -Algorithm SHA256).Hash -ne $Hashes[$Path]){throw 'DLL changed during Paint tests'}}
if((Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId -ne $BuildId){throw 'Engine BuildId changed during Paint tests'}
$Receipt=Get-Content "$Repo/Tests/DasForUESmoke/Binaries/Win64/DasForUESmokeEditor.target" -Raw|ConvertFrom-Json
if(!@($Receipt.RuntimeDependencies|Where-Object {$_.Path -like '*ue_paint.das' -and $_.Type -eq 'UFS'}).Count){throw 'Paint public module missing from UFS receipt'}
if($Publish){$Report|ConvertTo-Json -Depth 100|Set-Content "$Repo/Bindings/Packages/ue_api_umg_input.summary.json" -Encoding utf8}
[ordered]@{state='PASS';compile_rejections=$CompileRejections;generator_tests=8;runtime_checks=[int]$Result.Groups[1].Value;opened_functions=5;counts=$Report.counts;whole_module_compile='PASS';source_sha256=$Sources;dll_sha256=$Hashes;engine_build_id=$BuildId;report=$ReportPath;runtime_log=$Log;public_module_ufs=$true;published=[bool]$Publish;viewport_mode='SObjectWidget.Paint with NullSlate renderer; draw elements inspected, no visible window';full_suites='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 12|Set-Content "$Repo/logs/paint-validation.json" -Encoding utf8
Write-Output "DAS_PAINT_VALIDATION=PASS stage=$Stage"
