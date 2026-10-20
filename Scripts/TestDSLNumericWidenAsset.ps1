param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
$Repo = $Das.Repo
Invoke-DasBuild $Das -Name 'Authoring probe'
$ProbeId = [Guid]::NewGuid().ToString('N')
try {
foreach ($Phase in @('Create','Migrate','Reopen')) {
    Invoke-DasCommandlet $Das -Run DasFrame -Name "numeric-widen-asset-$Phase" -Marker 'DAS_NUMERIC_WIDEN_RESULT=PASS' -Arguments @('-NumericWiden',"-Phase=$Phase","-ProbeId=$ProbeId") | Out-Null
}
} finally {
    $ProbeSource = Join-Path $Repo ('Plugins\DasForUE\Content\Das\Scripts\dsl_widen_' + $ProbeId + '.das')
    if (Test-Path -LiteralPath $ProbeSource) { Remove-Item -LiteralPath $ProbeSource }
}
Write-Output 'DAS_NUMERIC_WIDEN_ASSET_PROBE=PASS'
