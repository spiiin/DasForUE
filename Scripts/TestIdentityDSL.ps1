param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das -Parallel 8 -Modules @('DasRuntime','DasEditor','DasForUESmoke') -NoEngineChanges}
$Paths={Get-DasValidationPaths $Das -Extra @('Tests/IdentityDSL/probe.das',
    'Tests/IdentityDSL/defaults.das',
    'Tests/DasForUESmoke/Binaries/Win64/UnrealEditor.modules',
    'Bindings/FoundationExterns.txt',
    'Scripts/TestIdentityDSL.ps1')}
Invoke-DasValidation $Das -Run DasIdentityDSL -Name 'identity-dsl-runtime' `
    -Marker 'DAS_IDENTITY_DSL_RESULT=PASS' -Checks 42 -SourcePaths $Paths `
    -Output 'identity-dsl-validation.json' -SuccessMarker 'DAS_IDENTITY_DSL_VALIDATION=PASS' `
    -Mode 'headless Game World, Apply defaults, CDO/ref and scalar-state reload' `
    -FailPattern 'DAS_IDENTITY_DSL_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:' `
    -RequiredPattern 'DAS_DSL_PATH_LITERAL'
