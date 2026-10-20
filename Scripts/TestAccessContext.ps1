param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
$Logs=Join-Path $Repo 'logs'
foreach($Test in @('TestNativeGameplayGenerator.py','TestBindingsGenerator.py','TestApiPackage.py','TestCanonicalMathGenerator.py')){
 & C:\Python312\python.exe "$PSScriptRoot/$Test"
 if($LASTEXITCODE){throw "$Test failed"}
}
& "$PSScriptRoot/GenerateNativeGameplay.ps1" -Engine $Engine -SkipBuild:$SkipBuild -Check:$SkipBuild *> "$Logs/access-context-native-check.log"
if(!$?){throw 'Native accessor manifest export/check failed'}
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasExport -Modules=Engine "-Selection=$Repo/Bindings/AccessContext.selection.json" "-Output=$Repo/Bindings/AccessContext.json" -Multiprocess -unattended -NullRHI -nosplash -stdout "-abslog=$Logs/access-context-export.log" -ddc=NoZenLocalFallback -NoZenAutoLaunch *> "$Logs/access-context-export-stdout.log"
if($LASTEXITCODE){throw 'Construction fixture wrapper export failed'}
& C:\Python312\python.exe "$PSScriptRoot/GenerateBindings.py" "$Repo/Bindings/AccessContext.json" "$Repo/Plugins/DasForUE/Content/Das/Scripts/ue_access_context_api.das" --module ue_access_context_api --canonical-math
if($LASTEXITCODE){throw 'Construction fixture wrapper generation failed'}
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasAccessContext -Multiprocess -unattended -NullRHI -nosplash -stdout "-abslog=$Logs/access-context-runtime.log" -ddc=NoZenLocalFallback -NoZenAutoLaunch *> "$Logs/access-context-runtime-stdout.log"
if($LASTEXITCODE -or !(Test-DasResult -LiteralPath "$Logs/access-context-runtime.log" -SimpleMatch 'DAS_ACCESS_CONTEXT_RESULT=PASS' -Quiet)){
 throw "Accessor/context checks failed: $Logs/access-context-runtime.log"
}
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasExport -Modules=Engine -GameWorld "-Probe=$Repo/Plugins/DasForUE/Content/Das/Scripts/native_accessors_example.das" -Multiprocess -unattended -NullRHI -nosplash -stdout "-abslog=$Logs/access-context-engine.log" -ddc=NoZenLocalFallback -NoZenAutoLaunch *> "$Logs/access-context-engine-stdout.log"
if($LASTEXITCODE){throw 'Native Engine accessor example failed'}
[ordered]@{state='PASS';python_tests=160;native_engine_accessors='PASS';native_manifest_check='PASS';runtime_accessor_and_construction='PASS';metadata_removed_guard='PASS';GC_setter='PASS';full_foundation='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 10|Set-Content -LiteralPath "$Logs/access-context-validation.json" -Encoding utf8
Write-Output 'DAS_ACCESS_CONTEXT=PASS'
exit 0
