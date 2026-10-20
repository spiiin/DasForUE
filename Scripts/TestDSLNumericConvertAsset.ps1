param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
$Repo = $Das.Repo
Invoke-DasBuild $Das -Name 'Authoring probe'
$ProbeId = [Guid]::NewGuid().ToString('N')
try {
foreach ($Phase in @('Create','Migrate','Reopen','NarrowFloat','ReopenFloat')) {
    Invoke-DasCommandlet $Das -Run DasFrame -Name "numeric-convert-asset-$Phase" -Marker 'DAS_NUMERIC_CONVERT_RESULT=PASS' -Arguments @('-NumericConvert',"-Phase=$Phase","-ProbeId=$ProbeId") | Out-Null
}
} finally {
    $ProbeSource = Join-Path $Repo ('Plugins\DasForUE\Content\Das\Scripts\dsl_convert_' + $ProbeId + '.das')
    if (Test-Path -LiteralPath $ProbeSource) { Remove-Item -LiteralPath $ProbeSource }
}
Write-Output 'DAS_NUMERIC_CONVERT_ASSET_PROBE=PASS'
