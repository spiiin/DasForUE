param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das -Parallel 8 -Modules @('DasRuntime','DasEditor','DasForUESmoke') -NoEngineChanges}
$Paths={Get-DasValidationPaths $Das -Extra @('Tests/EventChecks/probe.das',
    'Tests/EventChecks/dynamic.das',
    'Tests/DasForUESmoke/Binaries/Win64/UnrealEditor.modules',
    'Bindings/FoundationExterns.txt',
    'Scripts/TestEventChecks.ps1')}
Invoke-DasValidation $Das -Run DasEventChecks -Name 'event-check-runtime' `
    -Marker 'DAS_EVENT_CHECK_RESULT=PASS' -Checks 51 -SourcePaths $Paths `
    -Output 'event-check-validation.json' -SuccessMarker 'DAS_EVENT_CHECK_VALIDATION=PASS' `
    -Mode 'headless Game World, native/authored event signatures, Apply and managed reload' `
    -FailPattern 'DAS_EVENT_CHECK_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:'
