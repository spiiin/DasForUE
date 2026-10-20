param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
Invoke-DasBuild $Das -Name 'PIE probe' -Log (Join-Path $Das.Logs 'build-pie.log')
Invoke-DasAutomation $Das -Test 'DasForUE.Delegates.PIE' -Name 'delegates-pie' -Marker 'DAS_DELEGATES_RESULT=PASS'
Write-Output 'PIE delegates, hot reload, nested events, Destroy and EndPIE passed.'
