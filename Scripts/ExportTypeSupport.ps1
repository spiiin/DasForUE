param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
if (!$SkipBuild) { Invoke-DasBuild $Das -Name 'Type support export' }
$Output=Join-Path $Das.Logs 'type-support-live.json'
Invoke-DasCommandlet $Das -Run DasExport -Name 'type-support-export' -Arguments @("-TypeSupport=$Output") -Marker 'DAS_TYPE_SUPPORT_EXPORT=PASS' | Out-Null
