param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das -Parallel 8 -Modules @('DasRuntime','DasEditor','DasForUESmoke') -NoEngineChanges}
$Paths={Get-DasValidationPaths $Das -Extra @('Tests/HandlerReferences/authored.das',
    'Tests/HandlerReferences/free.das',
    'Tests/DasForUESmoke/Config/DefaultEngine.ini',
    'Tests/DasForUESmoke/Binaries/Win64/UnrealEditor.modules',
    'Bindings/FoundationExterns.txt',
    'Scripts/TestHandlerReferences.ps1')}
Invoke-DasValidation $Das -Run DasHandlerReferences -Name 'handler-references-runtime' `
    -Marker 'DAS_HANDLER_REF_RESULT=PASS' -Checks 34 -SourcePaths $Paths `
    -Output 'handler-references-validation.json' -SuccessMarker 'DAS_HANDLER_REF_VALIDATION=PASS' `
    -Mode 'headless Game World, typed method/free references, delegate ABI, Apply, GC and reload leases' `
    -FailPattern 'DAS_HANDLER_REF_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:'
