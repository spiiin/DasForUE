param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild) { Invoke-DasBuild $Das -Name 'Navigation binding exporter' }
& (Join-Path $PSScriptRoot 'GenerateNativeGameplay.ps1') -Engine $Engine -SkipBuild
if(!$?) { throw 'Native navigation registry generation failed' }
Invoke-DasBuild $Das -Name 'Navigation binding registry'
Write-Output 'DAS_NAVIGATION_BINDINGS_PREPARE=PASS'
