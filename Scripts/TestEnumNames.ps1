param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das -Parallel 8 -Modules @('DasRuntime','DasEditor','DasForUESmoke') -NoEngineChanges}
$Paths={Get-DasValidationPaths $Das -Extra @('Tests/EnumNames/probe.das',
    'Tests/DasForUESmoke/Config/DefaultEngine.ini',
    'Tests/DasForUESmoke/Binaries/Win64/UnrealEditor.modules',
    'Bindings/FoundationExterns.txt',
    'Scripts/TestEnumNames.ps1')}
Invoke-DasValidation $Das -Run DasEnumNames -Name 'enum-names-runtime' `
    -Marker 'DAS_ENUM_NAMES_RESULT=PASS' -Checks 15 -SourcePaths $Paths `
    -Output 'enum-names-validation.json' -SuccessMarker 'DAS_ENUM_NAMES_VALIDATION=PASS' `
    -Mode 'headless Game World, typed collision profiles, native FName roundtrip, string trigger lint' `
    -FailPattern 'DAS_ENUM_NAMES_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:' `
    -RequiredPattern 'DAS_DSL_STRING_TRIGGER'
