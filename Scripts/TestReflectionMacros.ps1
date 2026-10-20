param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das -Parallel 8 -Modules @('DasRuntime','DasEditor','DasForUESmoke') -NoEngineChanges}
$Paths={Get-DasValidationPaths $Das -Extra @('Tests/ReflectionMacros/probe.das',
    'Tests/DasForUESmoke/Binaries/Win64/UnrealEditor.modules',
    'Scripts/GenerateBindings.py',
    'Scripts/TestReflectionMacros.ps1')}
Invoke-DasValidation $Das -Run DasReflectionMacros -Name 'reflection-macro-runtime' `
    -Marker 'DAS_REFLECTION_MACRO_RESULT=PASS' -Checks 28 -SourcePaths $Paths `
    -Output 'reflection-macro-validation.json' -SuccessMarker 'DAS_REFLECTION_MACRO_VALIDATION=PASS' `
    -Mode 'headless Game World, reflection dispatch and WorldContext' `
    -FailPattern 'DAS_REFLECTION_MACRO_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:'
