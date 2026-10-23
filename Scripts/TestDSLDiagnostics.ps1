param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das}
$Paths={Get-DasValidationPaths $Das -Extra @('Scripts/TestDSLDiagnostics.ps1','Tests/suites.json')}
Invoke-DasValidation $Das -Run DasDSLDiagnostics -Name 'dsl-diagnostics-runtime' -Arguments @('-Multiprocess') `
 -Marker 'DAS_DSL_DIAGNOSTICS_RESULT=PASS' -Checks 9 -SourcePaths $Paths `
 -Output 'dsl-diagnostics-validation.json' -SuccessMarker 'DAS_DSL_DIAGNOSTICS_VALIDATION=PASS' `
 -Mode 'DSL compile/import/runtime source locations, retained reload maps and editor navigation token' `
 -FailPattern 'DAS_DSL_DIAGNOSTICS_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:'
