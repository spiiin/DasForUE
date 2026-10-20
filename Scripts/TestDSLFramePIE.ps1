param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
Invoke-DasBuild $Das -Name 'PIE probe' -Log (Join-Path $Das.Logs 'build-pie.log')
Invoke-DasAutomation $Das -Test 'DasForUE.DSL.FramePIE' -Name 'dsl-frame-pie' -Marker 'DAS_FRAME_PIE_RESULT=PASS'
Write-Output 'Mixed frame DSL graphs passed in PIE.'
