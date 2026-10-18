param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild,[switch]$Publish)
$ErrorActionPreference='Stop';$Repo=Split-Path -Parent $PSScriptRoot
$Python='C:/Python312/python.exe';$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
& $Python "$PSScriptRoot/TestGeometryGenerator.py";if($LASTEXITCODE){throw 'Geometry generator tests failed'}
if(!$SkipBuild){& "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=8 "-Log=$Repo/logs/geometry-build.log" *> "$Repo/logs/geometry-build-stdout.log";if($LASTEXITCODE){throw 'Geometry build failed'}}
$Hashes=@{};foreach($Name in @('DasRuntime','DasEditor')){$Path="$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$Name.dll";$Hashes[$Path]=(Get-FileHash $Path -Algorithm SHA256).Hash}
$Manifest="$Engine/Engine/Binaries/Win64/UnrealEditor.modules";$BuildId=(Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId
& "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile "$Repo/Bindings/Packages/UMGInput.profile.json" -SkipBuild -Publish:$Publish|Tee-Object -Variable PackageOutput
if(!$?){throw 'Geometry package generation failed'}
$Match=[regex]::Match(($PackageOutput -join "`n"),'DAS_API_PACKAGE=PASS[^\r\n]*report=(.+/report\.json)');if(!$Match.Success){throw 'Missing Geometry package report'}
$ReportPath=$Match.Groups[1].Value.Trim();$Stage=Split-Path -Parent $ReportPath;$Report=Get-Content $ReportPath -Raw|ConvertFrom-Json
$Contract=Get-Content "$Repo/Bindings/Geometry.contract.json" -Raw|ConvertFrom-Json
$Open=@($Report.members|Where-Object {$_.status -eq 'generation_ready'}|ForEach-Object {$_.class+'.'+$_.function})
if($Contract.functions.Count -ne 17){throw 'Geometry contract must attest exactly 17 functions'}
foreach($Name in $Contract.functions){if($Name -notin $Open){throw "Geometry function not ready: $Name"}}
if($Report.counts.generation_ready -lt 383){throw 'Unexpected UMG Geometry coverage'}
$Probe="$Stage/geometry.das";(Get-Content "$Repo/Tests/Geometry/probe.template.das" -Raw).Replace('_MODULE_',$(if($Publish){'ue_api_umg_input'}else{$Report.staged_module}))|Set-Content $Probe -Encoding utf8
$Editor="$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe";$Common=@($Project,'-Multiprocess','-unattended','-NullRHI','-nosplash','-stdout','-ddc=NoZenLocalFallback','-NoZenAutoLaunch')
$Log="$Stage/geometry.log"
& $Editor @Common -run=DasGeometry "-Script=$Probe" "-abslog=$Log" *> "$Stage/geometry-stdout.log"
$Result=[regex]::Match((Get-Content $Log -Raw),'DAS_GEOMETRY_RESULT=PASS checks=(\d+)');if($LASTEXITCODE -or !$Result.Success){throw "Geometry native/runtime probe failed: $Log"}
$CompileRejections=0
foreach($Case in @(
 @{name='geometry_integer_constructor';body='let value=UEGeometry(uint64(1));return 0'},
 @{name='geometry_numeric_cast';body='var value:UEGeometry;return int(uint64(value))'},
 @{name='geometry_token_field';body='var value:UEGeometry;return int(value.Token)'})){
 $Bad="$Stage/$($Case.name).das";$BadLog="$Stage/$($Case.name).log"
 "options gen2`nrequire ue_geometry`n[export]`ndef main(input:int):int { $($Case.body) }"|Set-Content $Bad -Encoding utf8
 & $Editor @Common -run=DasExport -Modules=Engine "-Probe=$Bad" "-abslog=$BadLog" *> "$BadLog.stdout"
 $BadText=Get-Content $BadLog -Raw;if(!$LASTEXITCODE -or !$BadText.Contains('error[') -or !$BadText.Contains('UEGeometry')){throw "Expected compile-time Geometry token rejection: $BadLog"};$CompileRejections++
}
foreach($Name in @('export.log','preflight.log','geometry.log')){if(Select-String (Join-Path $Stage $Name) -Pattern 'Ensure condition failed|Log[^:]+: Error:' -Quiet){throw "Engine error/ensure: $Name"}}
foreach($Path in $Hashes.Keys){if((Get-FileHash $Path -Algorithm SHA256).Hash -ne $Hashes[$Path]){throw 'DLL changed during Geometry tests'}}
if((Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId -ne $BuildId){throw 'Engine BuildId changed during Geometry tests'}
$Receipt=Get-Content "$Repo/Tests/DasForUESmoke/Binaries/Win64/DasForUESmokeEditor.target" -Raw|ConvertFrom-Json
if(!@($Receipt.RuntimeDependencies|Where-Object {$_.Path -like '*ue_geometry.das' -and $_.Type -eq 'UFS'}).Count){throw 'Geometry public module missing from UFS receipt'}
if($Publish){$Report|ConvertTo-Json -Depth 100|Set-Content "$Repo/Bindings/Packages/ue_api_umg_input.summary.json" -Encoding utf8}
[ordered]@{state='PASS';compile_rejections=$CompileRejections;generator_tests=8;runtime_checks=[int]$Result.Groups[1].Value;opened_functions=17;counts=$Report.counts;whole_module_compile='PASS';dll_sha256=$Hashes;engine_build_id=$BuildId;report=$ReportPath;runtime_log=$Log;public_module_ufs=$true;published=[bool]$Publish;viewport_mode='headless native fallback; visible window not tested';full_suites='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 12|Set-Content "$Repo/logs/geometry-validation.json" -Encoding utf8
Write-Output "DAS_GEOMETRY_VALIDATION=PASS stage=$Stage"
