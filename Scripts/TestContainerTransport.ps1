param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das}
$Sources=@(Get-ChildItem "$($Das.Repo)/Plugins/DasForUE/Source","$($Das.Repo)/Plugins/DasForUE/Tests/Editor" -Recurse -File|ForEach-Object FullName)
$Before=Lock-SourceHashes -Paths $Sources
Invoke-DasAutomation $Das -Test 'DasForUE.Runtime.ContainerTransport' -Name container-transport -Extra @('-ddc=NoZenLocalFallback','-NoZenAutoLaunch')
if((Lock-SourceHashes -Paths $Sources) -cne $Before){throw 'Sources changed during container transport verification'}
Copy-Item -LiteralPath (Join-Path $Das.Logs 'container-transport-externs.txt') -Destination (Join-Path $Das.Repo 'Bindings/FoundationExterns.txt') -Force
Write-DasValidation -Path (Join-Path $Das.Logs 'container-transport-validation.json') -Data @{
 state='PASS';implementation_sha256=($Before|ConvertFrom-Json);automation='DasForUE.Runtime.ContainerTransport';packaged='not_run';PIE='not_run'
}
