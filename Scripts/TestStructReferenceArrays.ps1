param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
foreach($Test in @('TestStructReferenceArraysGenerator.py','TestBindingsGenerator.py','TestHitResultGenerator.py')){
 & C:\Python312\python.exe "$PSScriptRoot/$Test"
 if($LASTEXITCODE){throw "$Test failed"}
}
if(!$SkipBuild){
 Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=8', "-Log=$Repo/logs/struct-reference-arrays-build.log") *> "$Repo/logs/struct-reference-arrays-build-stdout.log"
 if($LASTEXITCODE){throw 'Reference array fixture build failed'}
}
function GetValidationDlls {
 $Hashes=[ordered]@{}
 foreach($Dll in @('UnrealEditor-DasRuntime.dll','UnrealEditor-DasEditor.dll')){$Hashes[$Dll]=(Get-FileHash -LiteralPath "$Repo/Tests/DasForUESmoke/Binaries/Win64/$Dll" -Algorithm SHA256).Hash}
 return ($Hashes|ConvertTo-Json -Compress)
}
$Before=GetValidationDlls
& "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile "$Repo/Bindings/Packages/CollisionPhysics.profile.json" -SkipBuild | Tee-Object -Variable PackageOutput
if(!$?){throw 'Collision preflight failed'}
$Match=[regex]::Match(($PackageOutput -join "`n"),'DAS_API_PACKAGE=PASS[^\r\n]*report=(.+/report\.json)')
if(!$Match.Success){throw 'Missing verified package report'}
$ReportPath=$Match.Groups[1].Value.Trim();$Stage=Split-Path -Parent $ReportPath
$Report=Get-Content -LiteralPath $ReportPath -Raw|ConvertFrom-Json
& "$PSScriptRoot/TestProjectileAdvanced.ps1" -Engine $Engine -Report $ReportPath
if(!$?){throw 'Advanced projectile probe failed'}
$Selection=Join-Path $Stage 'reference-array-selection.json'
'{"class_rules":[{"class":"/Script/DasRuntime.DasHitTransportLibrary","prefix":"ue_reference_arrays_","include":["EchoReferences"]}]}'|Set-Content -LiteralPath $Selection -Encoding utf8
$Editor=Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$Common=@($Project,'-run=DasExport','-Modules=Engine','-Multiprocess','-unattended','-NullRHI','-nosplash','-stdout','-ddc=NoZenLocalFallback','-NoZenAutoLaunch')
& $Editor @Common "-Selection=$Selection" "-Output=$Stage/reference-arrays.ir.json" "-abslog=$Stage/reference-arrays-export.log" *> "$Stage/reference-arrays-export-stdout.log"
if($LASTEXITCODE){throw 'Reference array fixture export failed'}
& C:\Python312\python.exe "$PSScriptRoot/GenerateBindings.py" "$Stage/reference-arrays.ir.json" "$Stage/ue_reference_arrays_fixture.das" --module ue_reference_arrays_fixture --canonical-math
if($LASTEXITCODE){throw 'Reference array fixture generation failed'}
$Symbols=& C:\Python312\python.exe -c 'import sys,json;sys.path.insert(0,sys.argv[1]);from GenerateBindings import class_name,struct_name;print(json.dumps(dict(ACTOR=class_name("/Script/Engine.Actor"),ENVELOPE=struct_name("/Script/DasRuntime.DasReferenceArrayEnvelope"))))' $PSScriptRoot
if($LASTEXITCODE){throw 'Could not resolve fixture symbols'}
$Symbols=$Symbols|ConvertFrom-Json
$Source=Get-Content -LiteralPath "$Repo/Tests/StructReferenceArrays/probe.template.das" -Raw
foreach($Key in @('ACTOR','ENVELOPE')){$Source=$Source.Replace('@'+$Key+'@',$Symbols.$Key)}
$Source|Set-Content -LiteralPath "$Stage/reference-arrays-probe.das" -Encoding utf8
$Log=Join-Path $Stage 'reference-arrays-runtime.log'
& $Editor @Common -GameWorld "-Probe=$Stage/reference-arrays-probe.das" "-abslog=$Log" *> "$Stage/reference-arrays-runtime-stdout.log"
if($LASTEXITCODE -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_PACKAGE_PREFLIGHT=PASS value=42' -Quiet)){throw "Reference array runtime probe failed: $Log"}
if((GetValidationDlls) -ne $Before){throw 'DLL changed during validation; rerun against the completed build'}
[ordered]@{state='PASS';python_tests=113;report=$ReportPath;runtime_log=$Log;generated_module='PASS';advanced_projectile='PASS';nested_object_class_arrays='PASS';nested_struct_arrays='PASS';null_duplicates_empty_resize='PASS';detached_result='PASS';wrong_object_metaclass_missing_element='PASS';dll_sha256=($Before|ConvertFrom-Json);counts=$Report.counts;published=$false;full_foundation='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 15|Set-Content -LiteralPath "$Repo/logs/struct-reference-arrays-validation.json" -Encoding utf8
Write-Output "DAS_STRUCT_REFERENCE_ARRAYS=PASS report=$ReportPath"
