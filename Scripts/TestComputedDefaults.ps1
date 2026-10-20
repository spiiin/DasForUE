param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$Package,[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das -Name 'Computed defaults editor'}
$Id=Invoke-DasAssetPhases $Das -Run DasComputedDefaults -Name 'computed-defaults' -Marker 'DAS_COMPUTED_DEFAULTS_RESULT=PASS' `
    -Phases @('Create','Reopen','Recompile') -FailPattern "$DasEngineErrorPattern|DAS_COMPUTED_DEFAULTS FAIL" -NoZen
Set-Content -LiteralPath (Join-Path $Das.Logs 'computed-defaults-probe-id.txt') -Value $Id
$Map="/Game/DasComputedDefaults/$Id/Map"
if($Package){
 & (Join-Path $PSScriptRoot 'TestPackaged.ps1') -Engine $Engine -AdditionalMaps @($Map) -SkipZenStore
 Invoke-DasPackaged $Das -Name 'computed-defaults-packaged' -Marker 'DAS_COMPUTED_DEFAULTS_PACKAGED=PASS' `
    -Arguments @($Map,'-DasComputedDefaults') -FailPattern $DasEngineErrorPattern | Out-Null
}
Write-Output "DAS_COMPUTED_DEFAULTS_PROBE=PASS map=$Map"
