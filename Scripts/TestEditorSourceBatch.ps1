param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das}
$Sources=@(Get-ChildItem "$($Das.Repo)/Plugins/DasForUE/Source","$($Das.Repo)/Plugins/DasForUE/Tests/Editor" -Recurse -File|ForEach-Object FullName)
$Before=Lock-SourceHashes -Paths $Sources
Invoke-DasAutomation $Das -Test 'DasForUE.Runtime.EditorSourceBatch' -Name editor-source-batch -Extra @('-ddc=NoZenLocalFallback','-NoZenAutoLaunch')
if((Lock-SourceHashes -Paths $Sources) -cne $Before){throw 'Sources changed during source batch verification'}
Write-DasValidation -Path (Join-Path $Das.Logs 'editor-source-batch-validation.json') -Data @{
 state='PASS';implementation_sha256=($Before|ConvertFrom-Json);automation='DasForUE.Runtime.EditorSourceBatch';PIE='not_run';packaged='not_run'
}
