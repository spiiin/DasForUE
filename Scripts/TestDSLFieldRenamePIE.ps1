param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
Invoke-DasBuild $Das -Name 'PIE probe' -Log (Join-Path $Das.Logs 'build-pie.log')
Invoke-DasAutomation $Das -Test 'DasForUE.DSL.FieldRenamePIE' -Name 'dsl-field-rename-pie' -Marker 'DAS_FIELD_RENAME_PIE_RESULT=PASS'
Write-Output 'Stable ID field rename passed in PIE.'
