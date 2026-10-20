param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$Package,[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das -Name 'Native defaults editor'}
$Id=Invoke-DasAssetPhases $Das -Run DasNativeDefaults -Name 'native-defaults' -Marker 'DAS_NATIVE_DEFAULTS_RESULT=PASS' `
    -Phases @('Create','Reopen','Recompile') -FailPattern "$DasEngineErrorPattern|DAS_NATIVE_DEFAULTS FAIL" -NoZen
Set-Content -LiteralPath (Join-Path $Das.Logs 'native-defaults-probe-id.txt') -Value $Id
$Map="/Game/DasNativeDefaults/$Id/Map"
if($Package){
 & (Join-Path $PSScriptRoot 'TestPackaged.ps1') -Engine $Engine -AdditionalMaps @($Map) -SkipZenStore
 Invoke-DasPackaged $Das -Name 'native-defaults-packaged' -Marker 'DAS_NATIVE_DEFAULTS_PACKAGED=PASS' `
    -Arguments @($Map,'-DasNativeDefaults') -FailPattern $DasEngineErrorPattern | Out-Null
}
Write-Output "DAS_NATIVE_DEFAULTS_PROBE=PASS map=$Map"
