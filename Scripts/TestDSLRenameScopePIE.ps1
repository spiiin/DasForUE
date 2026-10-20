param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
Invoke-DasBuild $Das -Name 'PIE probe' -Log (Join-Path $Das.Logs 'build-pie.log')
Invoke-DasAutomation $Das -Test 'DasForUE.DSL.RenameScopePIE' -Name 'dsl-rename-scope-pie' -Marker 'DAS_RENAME_SCOPE_PIE_RESULT=PASS'
Write-Output 'Dependent Blueprint rename scope passed in PIE.'
