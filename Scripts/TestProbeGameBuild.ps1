param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
Invoke-DasBuild $Das -Name 'opt-in game probes' -BuildArguments @('DasForUESmoke','Win64','Development',"-Project=$($Das.Project)",'-WaitMutex','-NoXGE','-MaxParallelActions=12')
$Root=Join-Path $Das.Repo 'Tests/DasForUESmoke/Intermediate/Build/Win64'
$Manifests=@(Get-ChildItem $Root -Filter '*.uhtmanifest' -Recurse | Where-Object { $_.FullName -notmatch 'DasForUESmokeEditor' })
if(!$Manifests -or !($Manifests | Where-Object { (Get-Content $_.FullName -Raw) -match 'DasObjectFixture.h' })){
    throw 'Opt-in game target lost reflected test fixtures'
}
Write-Output 'DAS_PROBE_GAME_BUILD=PASS (non-editor opt-in game linked; cook/run not included)'
