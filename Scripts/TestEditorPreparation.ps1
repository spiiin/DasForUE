param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das}
$Sources=@(Get-ChildItem "$($Das.Repo)/Plugins/DasForUE/Source","$($Das.Repo)/Plugins/DasForUE/Tests/Editor" -Recurse -File|ForEach-Object FullName)
$Before=Lock-SourceHashes -Paths $Sources
Invoke-DasAutomation $Das -Test 'DasForUE.Runtime.EditorPreparation' -Name editor-preparation -Extra @('-ddc=NoZenLocalFallback','-NoZenAutoLaunch')
if((Lock-SourceHashes -Paths $Sources) -cne $Before){throw 'Sources changed during editor preparation verification'}
Write-DasValidation -Path (Join-Path $Das.Logs 'editor-preparation-validation.json') -Data @{
 state='PASS';implementation_sha256=($Before|ConvertFrom-Json);automation='DasForUE.Runtime.EditorPreparation';startup='all_project_and_selected_plugin_scripts';PIE='authorization_and_synthetic_world';packaged='not_run'
}
