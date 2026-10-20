param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das -Parallel 8 -Modules @('DasRuntime','DasEditor','DasForUESmoke') -NoEngineChanges}
$Paths={Get-DasValidationPaths $Das -Extra @('Tests/OperationKeys/probe.das',
    'Tests/DasForUESmoke/Binaries/Win64/UnrealEditor.modules',
    'Bindings/FoundationExterns.txt',
    'Scripts/TestOperationKeys.ps1')}
Invoke-DasValidation $Das -Run DasHiddenCallbacks -Name 'operation-key-runtime' `
    -Marker 'DAS_HIDDEN_CALLBACK_RESULT=PASS' -Checks 26 -SourcePaths $Paths `
    -Output 'operation-key-validation.json' -SuccessMarker 'DAS_HIDDEN_CALLBACK_VALIDATION=PASS' `
    -Mode 'headless Game World, keyless operations and timer/latent family isolation, Apply/reload' `
    -FailPattern 'DAS_HIDDEN_CALLBACK_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:' `
    -Arguments @('-Multiprocess','-OperationKeys') `
    -RequiredPattern 'DAS_DSL_UNKNOWN_OPERATION_KEY'
