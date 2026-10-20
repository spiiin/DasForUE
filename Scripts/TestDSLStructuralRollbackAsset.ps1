param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
$Repo = $Das.Repo
Invoke-DasBuild $Das -Name 'Authoring probe'
$ProbeId = [Guid]::NewGuid().ToString('N')
try {
foreach ($Phase in @('Create','Rollback','Reopen')) {
    Invoke-DasCommandlet $Das -Run DasFrame -Name "structural-rollback-asset-$Phase" -Marker 'DAS_STRUCTURAL_ASSET_RESULT=PASS' -Arguments @('-StructuralRollback',"-Phase=$Phase","-ProbeId=$ProbeId") | Out-Null
}
} finally {
    $ProbeSource = Join-Path $Repo ('Plugins\DasForUE\Content\Das\Scripts\dsl_renamescope_' + $ProbeId + '.das')
    if (Test-Path -LiteralPath $ProbeSource) { Remove-Item -LiteralPath $ProbeSource }
}
Write-Output 'DAS_STRUCTURAL_ROLLBACK_ASSET_PROBE=PASS'
