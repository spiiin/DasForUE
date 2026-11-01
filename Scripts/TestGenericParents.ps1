param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das}
$Sources=@(Get-ChildItem "$($Das.Repo)/Plugins/DasForUE/Source","$($Das.Repo)/Plugins/DasForUE/Tests/Editor" -File -Recurse|ForEach-Object FullName)
$Before=Lock-SourceHashes -Paths $Sources
Invoke-DasAutomation $Das -Test 'DasForUE.DSL.GenericParents' -Name 'generic-parents' -Extra @('-ddc=NoZenLocalFallback','-NoZenAutoLaunch')
if((Lock-SourceHashes -Paths $Sources) -cne $Before){throw 'Sources changed during the generic-parent runtime check'}
Write-DasValidation -Path (Join-Path $Das.Logs 'generic-parents-validation.json') -Data @{
 state='PASS';automation='DasForUE.DSL.GenericParents';implementation_sha256=($Before|ConvertFrom-Json);
 PIE='not_run';packaged='not_run';full_suites='not_run'
}
