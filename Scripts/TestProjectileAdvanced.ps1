param([string]$Engine='C:\src\unreal\UnrealEngine',[string]$Report)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
if(!$Report){throw 'Pass the exact freshly generated Collision run report.json'}
$Report=(Resolve-Path -LiteralPath $Report).Path
$Stage=Split-Path -Parent $Report
$Definition=Get-Content -LiteralPath $Report -Raw|ConvertFrom-Json
if($Definition.state -ne 'ue_preflight_passed' -or $Definition.package -ne 'ue_api_collision'){throw 'Expected a verified Collision package'}
$Member=@($Definition.members|Where-Object {$_.class -eq '/Script/Engine.GameplayStatics' -and $_.function -eq 'Blueprint_PredictProjectilePath_Advanced'})
if($Member.Count -ne 1 -or $Member[0].status -ne 'generation_ready'){throw 'Advanced predictor remains blocked'}
$Validation=Join-Path $Stage ('projectile-advanced-'+[guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $Validation|Out-Null
Copy-Item -LiteralPath (Join-Path $Stage ($Definition.staged_module+'.das')) -Destination $Validation
$Python='C:\Python312\python.exe'
& $Python "$PSScriptRoot/CheckCollisionHitResultCoverage.py" $Report --output "$Validation/coverage.json"
if($LASTEXITCODE){throw 'HitResult declaration coverage incomplete'}
$Symbols=& $Python -c 'import sys; sys.path.insert(0,sys.argv[1]); from GenerateBindings import struct_name,enum_name,class_name; import json; print(json.dumps(dict(PARAMS=struct_name("/Script/Engine.PredictProjectilePathParams"), CHANNEL=enum_name("/Script/Engine.ECollisionChannel"), DEBUG=enum_name("/Script/Engine.EDrawDebugTrace"), ACTOR=class_name("/Script/Engine.Actor"))))' $PSScriptRoot
if($LASTEXITCODE){throw 'Could not resolve generated symbols'}
$Symbols=$Symbols|ConvertFrom-Json
$Source=Get-Content -LiteralPath "$Repo/Tests/ProjectileAdvanced/probe.template.das" -Raw
$Source=$Source.Replace('@COLLISION@',$Definition.staged_module)
foreach($Key in @('PARAMS','CHANNEL','DEBUG','ACTOR')){$Source=$Source.Replace('@'+$Key+'@',$Symbols.$Key)}
$Source|Set-Content -LiteralPath "$Validation/probe.das" -Encoding utf8
$Dlls=@('UnrealEditor-DasRuntime.dll','UnrealEditor-DasEditor.dll')
function GetDllHashes {
 $Result=[ordered]@{}
 foreach($Dll in $Dlls){$Result[$Dll]=(Get-FileHash -LiteralPath "$Repo/Tests/DasForUESmoke/Binaries/Win64/$Dll" -Algorithm SHA256).Hash}
 return ($Result|ConvertTo-Json -Compress)
}
$Before=GetDllHashes
$Editor="$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe"
& $Editor "$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject" -run=DasExport -Modules=Engine -GameWorld -CollisionWorld "-Probe=$Validation/probe.das" -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$Validation/runtime.log" *> "$Validation/stdout.log"
if($LASTEXITCODE){throw "Advanced projectile runtime failed: $Validation/runtime.log"}
if(!(Select-String -LiteralPath "$Validation/runtime.log" -SimpleMatch 'DAS_PACKAGE_PREFLIGHT=PASS value=42' -Quiet)){throw 'Missing runtime success marker'}
if(!(Select-String -LiteralPath "$Validation/stdout.log" -SimpleMatch 'DAS_PROJECTILE_ADVANCED_PASS' -Quiet)){throw 'Missing advanced prediction marker'}
if((GetDllHashes) -ne $Before){throw 'DLL changed during verification'}
[ordered]@{state='PASS';report=$Report;runtime_log="$Validation/runtime.log";coverage='33/33';scenarios=@('hit','nested_hitresult_and_pathdata','ignored_actor','clear_array','null_element','duplicates','spatial_miss');dll_sha256=($Before|ConvertFrom-Json);PIE='not_run';packaged='not_run';full_foundation='not_run';native_build='not_needed';commit='not_created'}|ConvertTo-Json -Depth 10|Set-Content -LiteralPath "$Validation/result.json" -Encoding utf8
Copy-Item -LiteralPath "$Validation/result.json" -Destination "$Repo/logs/projectile-advanced-validation.json" -Force
Write-Output "DAS_PROJECTILE_ADVANCED=PASS report=$Validation/result.json"
