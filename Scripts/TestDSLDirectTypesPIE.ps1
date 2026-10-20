param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
Invoke-DasBuild $Das -Name 'PIE probe' -Log (Join-Path $Das.Logs 'build-pie.log')
Invoke-DasAutomation $Das -Test 'DasForUE.DSL.DirectTypesPIE' -Name 'dsl-direct-types-pie' -Marker 'DAS_AUTHORED_VALUES_PIE_RESULT=PASS'
Write-Output 'Direct enum/struct fields, typed frames and rollback passed in PIE.'
