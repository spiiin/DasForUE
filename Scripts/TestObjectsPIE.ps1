param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
Invoke-DasBuild $Das -Name 'PIE probe' -Log (Join-Path $Das.Logs 'build-pie.log')
Invoke-DasAutomation $Das -Test 'DasForUE.Objects.PIE' -Name 'objects-pie' -Marker 'DAS_OBJECTS_RESULT=PASS'
Write-Output 'PIE object handles, native/Blueprint UFunction calls and Destroy/GC passed.'
