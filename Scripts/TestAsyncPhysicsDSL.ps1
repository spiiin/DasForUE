param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild,[switch]$SkipRegression,[switch]$SyncOnly)
$ErrorActionPreference='Stop';$Repo=Split-Path -Parent $PSScriptRoot;$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
if(!$SkipBuild){& "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=8 "-Log=$Repo/logs/async-dsl-build.log" *> "$Repo/logs/async-dsl-build-stdout.log";if($LASTEXITCODE){throw 'Async DSL build failed'}}
$Dlls=@('DasRuntime','DasEditor')|ForEach-Object {"$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$_.dll"};$Hashes=@{};foreach($Dll in $Dlls){$Hashes[$Dll]=(Get-FileHash $Dll -Algorithm SHA256).Hash}
$Stage="$Repo/Build/AsyncPhysicsDSL/$([guid]::NewGuid().ToString('N'))";New-Item -ItemType Directory -Path $Stage -Force|Out-Null
$Scripts="$Repo/Tests/DasForUESmoke/Content/Das/Scripts";New-Item -ItemType Directory -Path $Scripts -Force|Out-Null
$Logs=@();$Checks=0
$Modes=if($SyncOnly){@('sync')}else{@('sync','fixed-async')};foreach($Mode in $Modes){
 $Id=[guid]::NewGuid().ToString('N');$Game="async_dsl_game_$Id.das";$Physics="async_dsl_physics_$Id.das";$Files=@("$Scripts/$Game","$Scripts/$Physics","$Scripts/$([IO.Path]::GetFileNameWithoutExtension($Game))_derived.das","$Scripts/$([IO.Path]::GetFileNameWithoutExtension($Game))_component.das","$Scripts/$Game.mailbox_probe.das")
 try{
  (Get-Content "$Repo/Plugins/DasForUE/Content/Das/Scripts/async_physics_thruster.das" -Raw).Replace('async_physics_thruster_tick.das',$Physics)|Set-Content -LiteralPath $Files[0] -Encoding utf8
  Copy-Item "$Repo/Plugins/DasForUE/Content/Das/Scripts/async_physics_thruster_tick.das" -Destination $Files[1]
  $Artifact="$Repo/Tests/DasForUESmoke/Content/DasTests/AsyncPhysicsDSL_$Id";if(Test-Path -LiteralPath $Artifact){throw "Preexisting test asset directory: $Artifact"}
  $Extra=@();if($Mode -eq 'fixed-async'){$Extra+='-AsyncFixed'};$Log="$Stage/$Mode.log"
  & "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasAsyncPhysicsDSL "-Script=$Game" "-Physics=$Physics" "-Package=/Game/DasTests/AsyncPhysicsDSL_$Id/BP_Thruster" @Extra -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$Log" *> "$Stage/$Mode-stdout.log"
  $Match=[regex]::Match((Get-Content $Log -Raw),'DAS_ASYNC_DSL_RESULT=PASS checks=(\d+)');if($LASTEXITCODE -or !$Match.Success){throw "Async DSL $Mode failed: $Log"}
  if(Select-String -LiteralPath $Log -Pattern 'Ensure condition failed|Log[^:]+: Error:' -Quiet){throw "Engine error/ensure: $Log"};$Checks += [int]$Match.Groups[1].Value;$Logs+=$Log
 }finally{
  if($Artifact -and (Test-Path -LiteralPath $Artifact)){$AssetRoot=[IO.Path]::GetFullPath("$Repo/Tests/DasForUESmoke/Content/DasTests")+[IO.Path]::DirectorySeparatorChar;$Resolved=[IO.Path]::GetFullPath($Artifact);if(!$Resolved.StartsWith($AssetRoot,[StringComparison]::OrdinalIgnoreCase) -or [IO.Path]::GetFileName($Resolved) -ne "AsyncPhysicsDSL_$Id"){throw 'Unexpected test asset cleanup path'};Remove-Item -LiteralPath $Resolved -Recurse}
  foreach($File in $Files){$Full=[IO.Path]::GetFullPath($File);if(!$Full.StartsWith([IO.Path]::GetFullPath($Scripts)+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){throw 'Unexpected cleanup path'};if(Test-Path -LiteralPath $Full){Remove-Item -LiteralPath $Full}}}
}
if(!$SkipRegression){& "$PSScriptRoot/TestScopedPhysicsBody.ps1" -Engine $Engine -SkipBuild;if(!$?){throw 'Scoped body/VM regression failed'}}
foreach($Dll in $Dlls){if((Get-FileHash $Dll -Algorithm SHA256).Hash -ne $Hashes[$Dll]){throw 'DLL changed during tests'}}
$BodyChecks=0;$VMChecks=0;$RoleChecks=0
if(!$SkipRegression){$BodyReport=Get-Content "$Repo/logs/scoped-body-validation.json" -Raw|ConvertFrom-Json;$VMReport=Get-Content "$Repo/logs/physics-vm-validation.json" -Raw|ConvertFrom-Json;$BodyChecks=$BodyReport.body_checks;$VMChecks=$VMReport.checks;$RoleChecks=$VMReport.execution_role_regression_checks}
[ordered]@{state='PASS';DSL_checks=$Checks;body_checks=$BodyChecks;VM_checks=$VMChecks;execution_role_regression_checks=$RoleChecks;total_checks=($Checks+$BodyChecks+$VMChecks+$RoleChecks);regression_skipped=[bool]$SkipRegression;sync_only=[bool]$SyncOnly;regression_report='logs/scoped-body-validation.json';runtime_logs=$Logs;dll_sha256=$Hashes;native_registration_in_example='none';Foundation='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 20|Set-Content "$Repo/logs/async-dsl-validation.json" -Encoding utf8
Write-Output "DAS_ASYNC_DSL_VALIDATION=PASS checks=$Checks stage=$Stage"
