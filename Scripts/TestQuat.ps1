param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das}
$Paths={Get-DasValidationPaths $Das -Extra @('Scripts/TestQuat.ps1')}
Invoke-DasValidation $Das -Run DasBuiltinValues -Name 'quat-runtime' -Arguments @('-Multiprocess','-QuatOnly') `
 -Marker 'DAS_QUAT_RESULT=PASS' -Checks 1 -SourcePaths $Paths `
 -Output 'quat-validation.json' -SuccessMarker 'DAS_QUAT_VALIDATION=PASS' `
 -Mode 'native FQuat explicit Rotator conversions, singularities, sign equivalence and invalid inputs' `
 -FailPattern 'DAS_BUILTIN_VALUES_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:'
