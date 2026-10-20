param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
Invoke-DasBuild $Das -Name 'PIE probe' -Log (Join-Path $Das.Logs 'build-pie.log')
Invoke-DasAutomation $Das -Test 'DasForUE.MethodDispatch.PIE' -Name 'method-dispatch-pie' -Marker 'DAS_METHOD_DISPATCH_PIE_RESULT=PASS'
Write-Output 'DAS_METHOD_DISPATCH_PIE=PASS'
