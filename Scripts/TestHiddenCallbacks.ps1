param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das -Parallel 8 -Modules @('DasRuntime','DasEditor','DasForUESmoke') -NoEngineChanges}
$Paths={Get-DasValidationPaths $Das -Extra @('Tests/HiddenCallbacks/probe.das',
    'Tests/DasForUESmoke/Binaries/Win64/UnrealEditor.modules',
    'Bindings/FoundationExterns.txt',
    'Scripts/TestHiddenCallbacks.ps1')}
Invoke-DasValidation $Das -Run DasHiddenCallbacks -Name 'hidden-callback-runtime' `
    -Marker 'DAS_HIDDEN_CALLBACK_RESULT=PASS' -Checks 25 -SourcePaths $Paths `
    -Output 'hidden-callback-validation.json' -SuccessMarker 'DAS_HIDDEN_CALLBACK_VALIDATION=PASS' `
    -Mode 'headless Game World, Apply, palette filtering, timers and Enhanced Input reload' `
    -FailPattern 'DAS_HIDDEN_CALLBACK_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:'
