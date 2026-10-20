param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
Invoke-DasBuild $Das -Name 'PIE probe' -Log (Join-Path $Das.Logs 'build-pie.log')
Invoke-DasAutomation $Das -Test 'DasForUE.DSL.ObjectFieldsPIE' -Name 'dsl-object-fields-pie' -Marker 'DAS_OBJECT_FIELDS_PIE_RESULT=PASS'
Write-Output 'Typed object fields, GC, reference migration and rollback passed in PIE.'
