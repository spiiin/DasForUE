param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
foreach($Test in @('TestBindingsGenerator.py','TestApiPackage.py')){
 & C:\Python312\python.exe "$PSScriptRoot/$Test"
 if($LASTEXITCODE){throw "$Test failed"}
}
if(!$SkipBuild){
 & "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=8 "-Log=$Repo/logs/collision-profile-adapter-build.log" *> "$Repo/logs/collision-profile-adapter-build-stdout.log"
 if($LASTEXITCODE){throw 'Collision profile adapter build failed'}
}
$Dlls=@('DasRuntime','DasEditor')|ForEach-Object {Join-Path $Repo "Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$_.dll"}
$Hashes=@{};foreach($Dll in $Dlls){$Hashes[$Dll]=(Get-FileHash $Dll -Algorithm SHA256).Hash}
& "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile "$Repo/Bindings/Packages/CollisionPhysics.profile.json" -SkipBuild | Tee-Object -Variable PackageOutput
if(!$?){throw 'Collision package failed'}
$Match=[regex]::Match(($PackageOutput -join "`n"),'DAS_API_PACKAGE=PASS[^\r\n]*report=(.+/report\.json)')
if(!$Match.Success){throw 'Missing successful package report path'}
$ReportPath=$Match.Groups[1].Value.Trim();$Stage=Split-Path -Parent $ReportPath
$Report=Get-Content -LiteralPath $ReportPath -Raw|ConvertFrom-Json
$IR=Get-Content -LiteralPath "$Stage/reflection.json" -Raw|ConvertFrom-Json
$Function=@($IR.functions|Where-Object {$_.class -eq '/Script/Engine.KismetSystemLibrary' -and $_.function -eq 'SetCollisionProfileNameProperty'})
if($Function.Count -ne 1 -or $Function[0].call_adapter -ne 'collision_profile_property_v1' -or !$Function[0].metadata.UnsafeDuringActorConstruction){throw 'Missing checked CustomThunk adapter export contract'}
$Editor=Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$Common=@($Project,'-Multiprocess','-unattended','-NullRHI','-nosplash','-stdout','-ddc=NoZenLocalFallback','-NoZenAutoLaunch')
$ObjectView=& C:\Python312\python.exe -c "import sys;sys.path.insert(0,sys.argv[1]);from GenerateBindings import class_name;print(class_name('/Script/CoreUObject.Object'))" $PSScriptRoot
$ProfileType=& C:\Python312\python.exe -c "import sys;sys.path.insert(0,sys.argv[1]);from GenerateBindings import struct_name;print(struct_name('/Script/Engine.CollisionProfileName'))" $PSScriptRoot
foreach($Case in @('probe','bind')){
 $Source=(Get-Content -LiteralPath "$Repo/Tests/CollisionProfileAdapter/collision_profile_$Case.template.das" -Raw).Replace('@COLLISION@',$Report.staged_module).Replace('@OBJECT@',$ObjectView.Trim()).Replace('@PROFILE@',$ProfileType.Trim())
 $Source|Set-Content -LiteralPath "$Stage/collision-profile-$Case.das" -Encoding utf8
}
$Log="$Stage/collision-profile-runtime.log"
& $Editor @Common -run=DasCollisionProfile "-Probe=$Stage/collision-profile-probe.das" "-BindProbe=$Stage/collision-profile-bind.das" "-abslog=$Log" *> "$Stage/collision-profile-runtime-stdout.log"
if($LASTEXITCODE -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_COLLISION_PROFILE_RESULT=PASS checks=22' -Quiet)){throw "Collision profile runtime checks failed: $Log"}
'{"schema_version":1,"modules":["Engine"],"subsystems":[{"id":"collision_physics","modules":["Engine"],"roots":[]}]}'|Set-Content -LiteralPath "$Stage/inventory-profile.json" -Encoding utf8
& $Editor @Common -run=DasNativeGameplay -Coverage "-Profile=$Stage/inventory-profile.json" "-Output=$Stage/inventory.json" "-abslog=$Stage/inventory.log" *> "$Stage/inventory-stdout.log"
if($LASTEXITCODE){throw 'Fresh collision inventory failed'}
& C:\Python312\python.exe "$PSScriptRoot/ReportApiPackage.py" --report $ReportPath --inventory "$Stage/inventory.json" --baseline "$Repo/Build/ApiPackages/ue_api_world/latest-report.json" --baseline "$Repo/Build/ApiPackages/ue_api_math/latest-report.json" --output "$Repo/Bindings/Packages/ue_api_collision.summary.json" --markdown "$Repo/Docs/Dev/Api/Blockers/CollisionPhysicsBlockers.md"
if($LASTEXITCODE){throw 'Collision blocker report failed'}
foreach($Dll in $Dlls){if((Get-FileHash $Dll -Algorithm SHA256).Hash -ne $Hashes[$Dll]){throw 'DLL changed during validation; rerun against completed build'}}
[ordered]@{state='PASS';python_tests=113;runtime_checks=22;whole_collision_module='PASS';generated_inherited_property='PASS';shared_property_setter='PASS';invalid_type_visibility_array_name='PASS';world_null_template_destroyed='PASS';construction_EndPlay_without_thunk_metadata='PASS';bind_events='PASS';other_custom_thunk_rejected='PASS';report=$ReportPath;runtime_log=$Log;counts=$Report.counts;dll_sha256=$Hashes;published=$false;Foundation='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 20|Set-Content -LiteralPath "$Repo/logs/collision-profile-adapter-validation.json" -Encoding utf8
Write-Output "DAS_COLLISION_PROFILE_ADAPTER=PASS report=$ReportPath"
