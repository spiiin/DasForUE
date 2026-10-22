param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das}
$Paths={Get-DasValidationPaths $Das -Extra @('Scripts/TestBuiltinValues.ps1')}
Invoke-DasValidation $Das -Run DasBuiltinValues -Name 'builtin-values-runtime' `
 -Marker 'DAS_BUILTIN_VALUES_RESULT=PASS' -Checks 33 -SourcePaths $Paths `
 -Output 'builtin-values-validation.json' -SuccessMarker 'DAS_BUILTIN_VALUES_VALIDATION=PASS' `
 -Mode 'native Unreal math parity, explicit operators, Name identity and compile/runtime refusals' `
 -FailPattern 'DAS_BUILTIN_VALUES_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:'
