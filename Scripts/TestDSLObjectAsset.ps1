param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
$Repo = $Das.Repo
Invoke-DasBuild $Das -Name 'Authoring probe'
$ProbeId = [Guid]::NewGuid().ToString('N')
foreach ($Phase in @('Create','Reopen')) {
    Invoke-DasCommandlet $Das -Run DasFrame -Name "object-asset-$Phase" -Marker 'DAS_FRAME_ASSET_RESULT=PASS' -Arguments @('-Objects',"-Phase=$Phase","-ProbeId=$ProbeId") | Out-Null
}
Write-Output 'DAS_FRAME_ASSET_PROBE=PASS'
