param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das -Parallel 8 -Modules @('DasRuntime','DasEditor','DasForUESmoke') -NoEngineChanges}
$Paths={Get-DasValidationPaths $Das -Extra @('Tests/InheritedHandlers/base.das',
    'Tests/InheritedHandlers/middle.das',
    'Tests/InheritedHandlers/leaf.das',
    'Tests/DasForUESmoke/Config/DefaultEngine.ini',
    'Tests/DasForUESmoke/Binaries/Win64/UnrealEditor.modules',
    'Bindings/FoundationExterns.txt',
    'Scripts/TestInheritedHandlers.ps1')}
Invoke-DasValidation $Das -Run DasInheritedHandlers -Name 'inherited-handlers-runtime' `
    -Marker 'DAS_INHERITED_HANDLER_RESULT=PASS' -Checks 34 -SourcePaths $Paths `
    -Output 'inherited-handlers-validation.json' -SuccessMarker 'DAS_INHERITED_HANDLER_VALIDATION=PASS' `
    -Mode 'headless Game World, inherited method references, virtual callbacks, Apply, imported hierarchy and reload' `
    -FailPattern 'DAS_INHERITED_HANDLER_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:'
