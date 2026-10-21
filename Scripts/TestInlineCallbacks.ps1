param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das -Parallel 8 -Modules @('DasRuntime','DasEditor')}
$Paths={Get-DasValidationPaths $Das -Extra @('Tests/InlineCallbacks/probe.template.das')}
Invoke-DasValidation $Das -Run DasInlineCallbacks -Name 'inline-callback-runtime' `
    -Marker 'DAS_INLINE_CALLBACK_RESULT=PASS' -Checks 26 -SourcePaths $Paths `
    -Output 'inline-callback-validation.json' -SuccessMarker 'DAS_INLINE_CALLBACK_VALIDATION=PASS' `
    -Mode 'headless Game World, real timers and EnhancedPlayerInput' `
    -FailPattern 'DAS_INLINE_CALLBACK_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:'
