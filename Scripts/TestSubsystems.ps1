param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das -Name 'authored subsystems'}
Invoke-DasValidation $Das -Run DasSubsystems -Name 'authored-subsystems-runtime' `
    -Marker 'DAS_SUBSYSTEMS_RESULT=PASS' -Checks 20 `
    -SourcePaths { Get-DasValidationPaths $Das -Extra @('Scripts/TestSubsystems.ps1','Bindings/FoundationExterns.txt') } `
    -Output 'authored-subsystems-validation.json' -Mode 'native subsystem collections, reload and teardown' `
    -SuccessMarker 'DAS_SUBSYSTEMS_VALIDATION=PASS'
