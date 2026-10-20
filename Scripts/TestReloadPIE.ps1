param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
Invoke-DasBuild $Das -Name 'PIE probe' -Log (Join-Path $Das.Logs 'build-pie.log')
Invoke-DasAutomation $Das -Test 'DasForUE.Reload.PIE' -Name 'reload-pie' -Marker 'DAS_PIE_RESULT=PASS'
Write-Output 'PIE structural reload passed, including separate editor/play state and EndPIE.'
