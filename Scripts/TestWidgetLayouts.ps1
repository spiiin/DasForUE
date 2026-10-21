param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das -Parallel 8}
$Paths={Get-DasValidationPaths $Das -Extra @()}
Invoke-DasValidation $Das -Run DasWidgetLayouts -Name 'widget-layout-runtime' `
    -Marker 'DAS_WIDGET_LAYOUT_RESULT=PASS' -Checks 30 -SourcePaths $Paths `
    -Output 'widget-layout-validation.json' -SuccessMarker 'DAS_WIDGET_LAYOUT_VALIDATION=PASS' `
    -Mode 'headless UMG with NullSlate' `
    -FailPattern 'DAS_WIDGET_LAYOUT_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:'
