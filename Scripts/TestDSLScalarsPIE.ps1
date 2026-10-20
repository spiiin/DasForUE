param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
Invoke-DasBuild $Das -Name 'PIE probe' -Log (Join-Path $Das.Logs 'build-pie.log')
Invoke-DasAutomation $Das -Test 'DasForUE.DSL.ScalarsPIE' -Name 'dsl-scalars-pie' -Marker 'DAS_SCALAR_PIE_RESULT=PASS'
Write-Output 'Typed scalar/void DSL graphs and bool properties passed in PIE.'
