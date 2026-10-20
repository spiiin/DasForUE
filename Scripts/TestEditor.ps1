param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
Invoke-DasBuild $Das -Name 'Editor' -Log (Join-Path $Das.Logs 'build-editor.log')
Invoke-DasCommandlet $Das -Run DasSmoke -Name 'smoke-editor' -Marker 'DAS_FOUNDATION_RESULT=PASS' -FullStdOut | Out-Null
