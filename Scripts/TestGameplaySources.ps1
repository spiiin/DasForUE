param([string]$Engine='C:\src\unreal\UnrealEngine')
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Sources=Join-Path $Repo 'Plugins\DasForUE\Content\Das\Scripts'
$Token=[Guid]::NewGuid().ToString('N')
$Module="ue_validation_$Token"
$Output="$Module.json"
$Failures=@()
# Compiler/model checks only. Gameplay execution is a separate PIE test.
$Cases=@('dsl_managed_timers','dsl_timer_arguments','dsl_timer_array_arguments','dsl_next_tick_timers',
 'dsl_scalar_events','dsl_value_events','dsl_math_events','dsl_array_events',
 'dsl_interface_object_events','dsl_authored_object_events','dsl_legacy_input',
 'dsl_enhanced_input','dsl_enhanced_contexts','dsl_vector2d','dsl_scene_components',
 'dsl_latent_delay','dsl_async_asset','dsl_gameplay_actor','dsl_gameplay_behavior')
try {
 foreach($Case in $Cases){
  $Log=Join-Path $Repo "logs\source-$Case.log"; $FixtureArgs=@(); if($Case -eq 'dsl_authored_object_events'){$FixtureArgs=@('-StructMapFixtures')}
  & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasGameplaySourceTest @FixtureArgs "-Scripts=$Case.das" "-Output=$Output" "-Module=$Module" -unattended -NullRHI -nosplash "-abslog=$Log" *> (Join-Path $Repo "logs\source-$Case-console.log")
  if($LASTEXITCODE -or !(Test-DasResult -LiteralPath $Log -SimpleMatch 'DAS_SOURCE_CATALOG PASS')){$Failures+=$Case;Write-Output "FAIL $Case"}else{Write-Output "PASS $Case"}
 }
}finally{
 # Only the two unique outputs created by this test; never remove source fixtures.
 foreach($Name in @($Output,"$Module.das")){
  $Path=[IO.Path]::GetFullPath((Join-Path $Sources $Name))
  if(!$Path.StartsWith([IO.Path]::GetFullPath($Sources)+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Invalid cleanup path'}
  if(Test-Path -LiteralPath $Path){Remove-Item -LiteralPath $Path}
 }
}
if($Failures.Count){throw "Gameplay source checks failed: $($Failures -join ', ')"}
Write-Output "DAS_GAMEPLAY_SOURCES_RESULT=PASS count=$($Cases.Count)"
