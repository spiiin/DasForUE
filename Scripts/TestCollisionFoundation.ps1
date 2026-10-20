param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
$Logs=Join-Path $Repo 'logs'
if(!$SkipBuild){
 Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=8', "-Log=$Logs/collision-foundation-build.log") *> "$Logs/collision-foundation-build-stdout.log"
 if($LASTEXITCODE){throw 'Collision foundation build failed'}
}
$RuntimeDll=Join-Path (Split-Path -Parent $Project) 'Binaries/Win64/UnrealEditor-DasRuntime.dll'
$RuntimeHash=(Get-FileHash -LiteralPath $RuntimeDll).Hash
& C:/Python312/python.exe "$PSScriptRoot/TestCollisionFoundationGenerator.py" *> "$Logs/collision-foundation-generator.log"
if($LASTEXITCODE){throw 'Weak metadata generator checks failed'}
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasExport -Modules=Engine "-Selection=$Repo/Bindings/CollisionFoundation.selection.json" "-Output=$Repo/Bindings/CollisionFoundation.json" -Multiprocess -unattended -NullRHI -nosplash -stdout "-abslog=$Logs/collision-foundation-export.log" -ddc=NoZenLocalFallback -NoZenAutoLaunch *> "$Logs/collision-foundation-export-stdout.log"
if($LASTEXITCODE){throw 'Weak fixture export failed'}
$IR=Get-Content -LiteralPath "$Repo/Bindings/CollisionFoundation.json" -Raw|ConvertFrom-Json
$Snapshot=@($IR.structs|Where-Object {$_.type_path -eq '/Script/DasRuntime.DasWeakCollisionSnapshot'})
$WeakLeaf=@($Snapshot.fields|Where-Object {$_.path -eq 'target'})
if($WeakLeaf.Count -ne 1 -or $WeakLeaf[0].weak_reference -ne $true){throw 'Native weak marker was not preserved in exported IR'}
& C:/Python312/python.exe "$PSScriptRoot/GenerateBindings.py" "$Repo/Bindings/CollisionFoundation.json" "$Repo/Plugins/DasForUE/Content/Das/Scripts/ue_collision_fixture.das" --module ue_collision_fixture
if($LASTEXITCODE){throw 'Weak fixture generation failed'}
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasCollisionFoundation -Multiprocess -unattended -NullRHI -nosplash -stdout "-abslog=$Logs/collision-foundation-runtime.log" -ddc=NoZenLocalFallback -NoZenAutoLaunch *> "$Logs/collision-foundation-runtime-stdout.log"
if($LASTEXITCODE -or !(Test-DasResult -LiteralPath "$Logs/collision-foundation-runtime.log" -SimpleMatch 'DAS_COLLISION_RESULT=PASS' -Quiet)){throw 'Collision foundation runtime checks failed'}
if((Get-FileHash -LiteralPath $RuntimeDll).Hash -ne $RuntimeHash){throw 'Runtime DLL changed during the focused check'}
[ordered]@{result='PASS';runtime_sha256=$RuntimeHash;exported_weak_marker=$true;scope='weak descriptor/value/GC and generated call-frame GC; real trace and overlap; checked lifetime';generator_tests=6;full_foundation='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json|Set-Content -LiteralPath "$Logs/collision-foundation-validation.json" -Encoding utf8
'DAS_COLLISION_FOUNDATION=PASS'
