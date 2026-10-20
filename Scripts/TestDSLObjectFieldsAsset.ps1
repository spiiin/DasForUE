param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
$Repo = $Das.Repo
Invoke-DasBuild $Das -Name 'Authoring probe'
$ProbeId = [Guid]::NewGuid().ToString('N')
try {
foreach ($Phase in @('Create','Reopen','Rename','ReopenRenamed','Retire','ReopenRetired','Restore','ReopenRestored')) {
    Invoke-DasCommandlet $Das -Run DasFrame -Name "object-fields-asset-$Phase" -Marker 'DAS_OBJECT_FIELDS_ASSET_RESULT=PASS' -Arguments @('-ObjectFields',"-Phase=$Phase","-ProbeId=$ProbeId") | Out-Null
}
} finally {
    $ProbeSource = Join-Path $Repo ('Plugins\DasForUE\Content\Das\Scripts\dsl_object_fields_asset_' + $ProbeId + '.das')
    if (Test-Path -LiteralPath $ProbeSource) { Remove-Item -LiteralPath $ProbeSource }
}
Write-Output 'DAS_OBJECT_FIELDS_ASSET_PROBE=PASS'
