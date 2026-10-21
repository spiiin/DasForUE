param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild) { Invoke-DasBuild $Das -Name 'GAS merged regression' }
# Reuse the MethodValues/component fixtures already verified against this runtime.
# The standard runner rejects missing probe ids; strict warning checks remain enabled.
# This is the targeted rerun after smoke-project cue-path configuration changes.
& (Join-Path $PSScriptRoot 'RunDasTests.ps1') -Engine $Engine -SkipBuild -NoDependencies `
    -Suite gas-ability-shipping,gas-ability-packaged,foundation,automation,dsl-inherited-handlers
Write-Output 'DAS_GAS_MERGE_REGRESSION=PASS'
