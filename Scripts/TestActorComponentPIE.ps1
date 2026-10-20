param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
Invoke-DasBuild $Das -Name 'PIE probe' -Log (Join-Path $Das.Logs 'build-pie.log')
Invoke-DasAutomation $Das -Test 'DasForUE.ActorComponent.PIE' -Name 'actor-component-pie' -Marker 'DAS_ACTOR_COMPONENT_PIE_RESULT=PASS'
Write-Output 'Actor/Component PIE, real ticks, owner delegate, Destroy and EndPIE passed.'
