param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das}
$Sources=@(Get-ChildItem "$($Das.Repo)/Plugins/DasForUE/Source","$($Das.Repo)/Plugins/DasForUE/Tests/Editor" -Recurse -File|ForEach-Object FullName)
$Before=Lock-SourceHashes -Paths $Sources
Invoke-DasAutomation $Das -Test 'DasForUE.Runtime.PreparedPrograms' -Name 'prepared-programs' -Extra @('-ddc=NoZenLocalFallback','-NoZenAutoLaunch')
Invoke-DasAutomation $Das -Test 'DasForUE.Runtime.ProgramCache' -Name 'program-cache' -Extra @('-ddc=NoZenLocalFallback','-NoZenAutoLaunch')
# A fresh process must read the artifact without relying on modules left by its compiler.
$Fixture="prepared_fresh_$([guid]::NewGuid().ToString('N')).das"
$FixtureDirectory=Join-Path (Split-Path $Das.Project) 'Content/Das/Scripts'
$PreparedDirectory=Join-Path (Split-Path $Das.Project) 'Content/Das/Prepared'
New-Item -ItemType Directory -Path $FixtureDirectory -Force|Out-Null
$Existing=@(Get-ChildItem $PreparedDirectory -Filter '*.dasp' -ErrorAction SilentlyContinue|ForEach-Object Name)
$FixturePath=Join-Path $FixtureDirectory $Fixture
try {
 @'
options gen2
require ue_dsl
[ue_class(Blueprintable)]
class FreshPreparedActor : Actor {
    @ue_property=(BlueprintReadWrite)
    Score:int=3
    [ue_function(BlueprintCallable)]
    def Add(value:int):int { self.Score+=value; return self.Score }
}
[export]
def main(input:int):int { return input+1 }
'@ | Set-Content -LiteralPath $FixturePath -Encoding utf8
 Invoke-DasCommandlet $Das -Run DasPreparePrograms -Name prepared-export-fresh -Marker 'DAS_PREPARE_EXPORT PASS' -Arguments @("-Scripts=$Fixture") -NoZen
 Invoke-DasCommandlet $Das -Run DasPreparePrograms -Name prepared-load-fresh -Marker 'DAS_PREPARE_VERIFY PASS' -Arguments @("-Scripts=$Fixture",'-Verify') -NoZen
} finally {
 Remove-Item -LiteralPath $FixturePath -Force -ErrorAction SilentlyContinue
 Get-ChildItem $PreparedDirectory -Filter '*.dasp' -ErrorAction SilentlyContinue|Where-Object {$_.Name -notin $Existing}|Remove-Item -Force
}
if((Lock-SourceHashes -Paths $Sources) -cne $Before){throw 'Source changed during program cache verification'}
Write-DasValidation -Path (Join-Path $Das.Logs 'program-cache-validation.json') -Data @{
 state='PASS';implementation_sha256=($Before|ConvertFrom-Json);automation=@('DasForUE.Runtime.ProgramCache','DasForUE.Runtime.PreparedPrograms');fresh_process='PASS';PIE='not_run';packaged='not_run'
}
