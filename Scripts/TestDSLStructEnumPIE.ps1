param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
Invoke-DasBuild $Das -Name 'PIE probe' -Log (Join-Path $Das.Logs 'build-pie.log')
Invoke-DasAutomation $Das -Test 'DasForUE.DSL.StructEnumPIE' -Name 'dsl-struct-enum-pie' -Marker 'DAS_STRUCT_ENUM_PIE_RESULT=PASS'
Write-Output 'Enum leaves in generic struct DSL passed in PIE.'
