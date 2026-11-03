param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild) { Invoke-DasBuild $Das -Name 'World component and subsystem bridge' }
& (Join-Path $PSScriptRoot 'GenerateNativeGameplay.ps1') -Engine $Engine -SkipBuild
if(!$?) { throw 'Native instance registry generation failed' }
Invoke-DasBuild $Das -Name 'Instance binding registry'
Write-Output 'DAS_COIN_BINDINGS_PREPARE=PASS'
