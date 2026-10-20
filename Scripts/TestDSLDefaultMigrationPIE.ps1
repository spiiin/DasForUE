param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
Invoke-DasBuild $Das -Name 'PIE probe' -Log (Join-Path $Das.Logs 'build-pie.log')
Invoke-DasAutomation $Das -Test 'DasForUE.DSL.DefaultMigrationPIE' -Name 'dsl-default-migration-pie' -Marker 'DAS_DEFAULT_MIGRATION_PIE_RESULT=PASS'
Write-Output 'Explicit default migration passed in PIE.'
