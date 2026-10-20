param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
Invoke-DasBuild $Das -Name 'PIE probe' -Log (Join-Path $Das.Logs 'build-pie.log')
Invoke-DasAutomation $Das -Test 'DasForUE.DSL.NumericWidenPIE' -Name 'dsl-numeric-widen-pie' -Marker 'DAS_NUMERIC_WIDEN_PIE_RESULT=PASS'
Write-Output 'Exact numeric widening passed in PIE.'
