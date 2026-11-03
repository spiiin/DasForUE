param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das -Name 'authored int64'}
Invoke-DasValidation $Das -Run DasInt64 -Name 'authored-int64-runtime' `
    -Marker 'DAS_INT64_RESULT=PASS' -Checks 17 `
    -SourcePaths { Get-DasValidationPaths $Das -Extra @('Scripts/TestInt64.ps1') } `
    -Output 'authored-int64-validation.json' -Mode 'exact reflected int64, Blueprint frames, ref/out and containers' `
    -SuccessMarker 'DAS_INT64_VALIDATION=PASS'
