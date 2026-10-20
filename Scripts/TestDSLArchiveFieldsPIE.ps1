param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
Invoke-DasBuild $Das -Name 'PIE probe' -Log (Join-Path $Das.Logs 'build-pie.log')
Invoke-DasAutomation $Das -Test 'DasForUE.DSL.ArchivePIE' -Name 'dsl-archive-pie' -Marker 'DAS_ARCHIVE_PIE_RESULT=PASS'
Write-Output 'Field retirement, restoration and rollback passed in PIE.'
