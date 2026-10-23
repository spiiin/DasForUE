param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das}
$Paths={Get-DasValidationPaths $Das -Extra @('Scripts/TestCompleteBuiltinTypes.ps1','Scripts/GenerateBindings.py','Scripts/GenerateNativeGameplay.py','Bindings/NativeGameplay.json','Bindings/Packages/CoreMath.profile.json','Tests/suites.json')}
Invoke-DasValidation $Das -Run DasBuiltinValues -Name 'complete-builtins-runtime' -Arguments @('-Multiprocess','-Complete') `
 -Marker 'DAS_COMPLETE_BUILTINS_RESULT=PASS' -Checks 5 -SourcePaths $Paths `
 -Output 'complete-builtins-validation.json' -SuccessMarker 'DAS_COMPLETE_BUILTINS_VALIDATION=PASS' `
 -Mode 'native built-in math/color parity, generated calls and authored fields/containers/methods' `
 -FailPattern 'DAS_COMPLETE_BUILTINS_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:'
