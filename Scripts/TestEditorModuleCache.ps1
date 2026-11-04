param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das}
$Sources=@(Get-ChildItem "$($Das.Repo)/Plugins/DasForUE/Source","$($Das.Repo)/Plugins/DasForUE/Tests/Editor","$($Das.Repo)/Plugins/DasForUE/Tests/Runtime" -Recurse -File|ForEach-Object FullName)
$Sources+=@('ThirdParty/daScript/lib/Release/libDaScript.lib','ThirdParty/daScript/lib/Release/libDaScript_runtime.lib','Scripts/ApplyDaScriptPatches.ps1','Scripts/Dependencies/daScript-module-cache-patches.json','Scripts/Dependencies/daScript-module-cache-safety.patch')|ForEach-Object{Join-Path $Das.Repo $_}
$Before=Lock-SourceHashes -Paths $Sources
Invoke-DasAutomation $Das -Test 'DasForUE.Runtime.ModuleCacheSafety' -Name 'module-cache-safety' | Out-Null
$Probe=[guid]::NewGuid().ToString('N')
try {
Invoke-DasCommandlet $Das -Run DasEditorModuleCache -Name editor-cache-cold -Marker 'DAS_EDITOR_MODULE_CACHE PASS phase=cold' -Arguments @("-ProbeId=$Probe",'-Cold') -NoZen
Invoke-DasCommandlet $Das -Run DasEditorModuleCache -Name editor-cache-warm -Marker 'DAS_EDITOR_MODULE_CACHE PASS phase=warm' -Arguments @("-ProbeId=$Probe") -NoZen
Invoke-DasCommandlet $Das -Run DasEditorModuleCache -Name editor-cache-reopen -Marker 'DAS_EDITOR_MODULE_CACHE PASS phase=reopen' -Arguments @("-ProbeId=$Probe",'-Reopen') -NoZen
} finally {
 $FixtureRoot=Join-Path (Split-Path $Das.Project) 'Content/Das/Scripts'
 foreach($Suffix in @('.das','_dep.das','_unused.das')) {
  Remove-Item -LiteralPath (Join-Path $FixtureRoot "module_cache_$Probe$Suffix") -Force -ErrorAction SilentlyContinue
 }
 $SavedRoot=Join-Path (Split-Path $Das.Project) 'Saved/Das'
 $CacheList=Join-Path $SavedRoot "CacheProbe/$Probe/files.txt"
 if(Test-Path -LiteralPath $CacheList) {
  foreach($File in Get-Content -LiteralPath $CacheList) {
   if($File -cmatch '^[0-9a-fA-F]{32}\.dasm$') {
    Remove-Item -LiteralPath (Join-Path $SavedRoot "ModuleCache/$File") -Force -ErrorAction SilentlyContinue
   }
  }
 }
}
if((Lock-SourceHashes -Paths $Sources) -cne $Before){throw 'Sources changed during editor module cache verification'}
Write-DasValidation -Path (Join-Path $Das.Logs 'editor-module-cache-validation.json') -Data @{
 state='PASS';implementation_sha256=($Before|ConvertFrom-Json);fresh_process='cold_warm_save_reopen';PIE='not_run';packaged='not_run'
}
