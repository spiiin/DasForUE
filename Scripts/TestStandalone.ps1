param([Parameter(Mandatory=$true)][string]$Project,[string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=[IO.Path]::GetFullPath($Project)
$ProjectRoot=Split-Path -Parent $Project
$Logs=Join-Path $ProjectRoot 'Saved/DasValidation';New-Item -ItemType Directory -Path $Logs -Force|Out-Null
if(!$SkipBuild){
 & "$PSScriptRoot/GenerateNativeGameplay.ps1" -Project $Project -Engine $Engine *> "$Logs/generation.log"
 if(!$?){throw 'Starter generation/build failed'}
}
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasStarter -Multiprocess -unattended -NullRHI -nosplash -stdout "-abslog=$Logs/export.log" -ddc=NoZenLocalFallback -NoZenAutoLaunch *> "$Logs/export-stdout.log"
if($LASTEXITCODE -or !(Test-DasResult -LiteralPath "$Logs/export.log" -SimpleMatch 'DAS_STARTER_EXPORT=PASS')){throw "Starter export failed: $Logs/export.log"}
$File=Join-Path $ProjectRoot 'Content/Das/Scripts/starter_values.das';$Before=(Get-FileHash -LiteralPath $File).Hash
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project /Game/Starter/StarterMap -game -DasStarterProbe -Multiprocess -unattended -NullRHI -nosplash -stdout "-abslog=$Logs/game.log" -ddc=NoZenLocalFallback -NoZenAutoLaunch *> "$Logs/game-stdout.log"
if($LASTEXITCODE -or !(Test-DasResult -LiteralPath "$Logs/game.log" -SimpleMatch 'DAS_STARTER_RESULT=PASS')){throw "Starter game failed: $Logs/game.log"}
if((Get-FileHash -LiteralPath $File).Hash -ne $Before){throw 'Reload check did not restore project helper'}
& "$PSScriptRoot/GenerateNativeGameplay.ps1" -Project $Project -Engine $Engine -Check -SkipBuild *> "$Logs/reproduce.log"
if(!$?){throw 'Project native bindings are not reproducible'}
$Checks=@('starter_project_scripts_component_and_possession','starter_native_collision_and_authored_score','starter_respawn_and_project_import','starter_import_hot_reload_preserves_state_and_restores_source')
foreach($Check in $Checks){if(!(Test-DasResult -LiteralPath "$Logs/game.log" -SimpleMatch "PASS $Check")){throw "Missing focused check: $Check"}}
[ordered]@{result='PASS';project=$Project;map='/Game/Starter/StarterMap';checks=$Checks;native_generation='project-local reproducible registry';scope='Separate UE project/target; project-local DAS files, same live game process reload; NullRHI';not_run=@('Foundation suite','full PIE suite','packaged build','visual/physical input validation');commit_created=$false}|ConvertTo-Json -Depth 4|Set-Content -LiteralPath "$Logs/result.json" -Encoding utf8
Write-Output "DAS_STARTER_FOCUSED=PASS project=$Project"
