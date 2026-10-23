param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das}
$Probe=[Guid]::NewGuid().ToString('N')
$Root=Join-Path $Das.Repo "Build/SaveGameCompatibility/$Probe"
New-Item -ItemType Directory -Path $Root -Force|Out-Null
$Prepared=Join-Path $Das.Logs 'savegame-compatibility-runtime.json'
$Upgraded=Join-Path $Das.Logs 'savegame-compatibility-upgrade.json'
$Sources=@(Get-ChildItem "$($Das.Repo)/Plugins/DasForUE/Source","$($Das.Repo)/Tests/DasForUESmoke/Source" -File -Recurse|ForEach-Object FullName)
$Sources+=@("$PSScriptRoot/TestSaveGameCompatibility.ps1","$PSScriptRoot/TestSaveGameCompatibilityRuntime.ps1","$PSScriptRoot/TestPackaged.ps1","$PSScriptRoot/DasTest.psm1","$($Das.Repo)/Tests/suites.json")
$Before=Lock-SourceHashes -Paths $Sources
$ScriptPath=$null
$ResultFile=Join-Path $Das.Logs 'savegame-compatibility-validation.json'
Write-DasValidation -Path $ResultFile -Data @{state='pending';probe=$Probe}
try {
 Invoke-DasCommandlet $Das -Run DasSaveGameCompatibility -Name 'savegame-compatibility-prepare' `
  -Arguments @('-Phase=Prepare',"-Probe=$Probe","-Output=$Prepared",'-Multiprocess') `
  -Marker 'DAS_SAVEGAME_COMPAT_PREPARE=PASS' -NoZen `
  -FailPattern 'DAS_SAVEGAME_COMPAT_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:'|Out-Null
 $Info=Get-Content -LiteralPath $Prepared -Raw|ConvertFrom-Json
 $ScriptPath=Join-Path $Das.Repo "Plugins/DasForUE/Content/Das/Scripts/$($Info.script)"
 $OriginalSource=(Get-FileHash -LiteralPath $ScriptPath -Algorithm SHA256).Hash
 $DataFile=Join-Path $Root 'original.sav'
 function Run-CompatibilityPackage([string]$Archive,[string]$Phase) {
  $Exe=Join-Path $Archive 'Windows/DasForUESmoke/Binaries/Win64/DasForUESmoke.exe'
  $Log=Join-Path $Das.Logs "savegame-compatibility-packaged-$Phase.log"
  $Observation=Join-Path $Das.Logs "savegame-compatibility-packaged-$Phase.json"
  $Arguments=@('/Engine/Maps/Entry','-DasSaveGameCompatibility',"-CompatibilityPhase=$Phase",("-CompatibilityClass="+$Info.class),
   ('-CompatibilityFile="'+$DataFile+'"'),("-CompatibilityScript="+$Info.script),('-CompatibilityObservation="'+$Observation+'"'),
   '-nullrhi','-unattended','-nosplash',('-abslog="'+$Log+'"'))
  $Process=Start-Process -FilePath $Exe -WorkingDirectory (Split-Path -Parent $Exe) -ArgumentList $Arguments -WindowStyle Hidden -PassThru
  if(!$Process.WaitForExit(180000)){Stop-Process -Id $Process.Id -Force;throw "Packaged $Phase timed out"}
  $Process.Refresh();if($Process.ExitCode){throw "Packaged $Phase failed with exit $($Process.ExitCode)"}
  Assert-DasLog -Log $Log -Marker 'DAS_SAVEGAME_COMPAT_PACKAGED_RESULT=PASS' -Name "Packaged $Phase" `
   -FailPattern 'DAS_SAVEGAME_COMPAT_PACKAGED_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:'
  return Get-Content -LiteralPath $Observation -Raw|ConvertFrom-Json
 }
 $Old=Join-Path $Root 'Original'
 & "$PSScriptRoot/TestPackaged.ps1" -Engine $Engine -SkipZenStore -DDC 'NoZenLocalFallback' -SkipSmoke `
  -AdditionalCookDirectories @($Info.cook_directory) -ArchiveDirectory $Old
 $Written=Run-CompatibilityPackage $Old Write
 if(!(Test-Path -LiteralPath $DataFile -PathType Leaf)){throw 'Original packaged version did not produce a save'}
 $NativeBytes=[IO.File]::ReadAllBytes($DataFile)
 if($NativeBytes.Length -lt 4 -or [Text.Encoding]::ASCII.GetString($NativeBytes,0,4) -ne 'GVAS'){throw 'Save is not the standard Unreal GVAS format'}
 $SaveHash=(Get-FileHash -LiteralPath $DataFile -Algorithm SHA256).Hash
 Invoke-DasCommandlet $Das -Run DasSaveGameCompatibility -Name 'savegame-compatibility-upgrade' `
  -Arguments @('-Phase=Upgrade',"-Probe=$Probe","-Output=$Upgraded",'-Multiprocess') `
  -Marker 'DAS_SAVEGAME_COMPAT_UPGRADE=PASS' -NoZen `
  -FailPattern 'DAS_SAVEGAME_COMPAT_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:'|Out-Null
 $NewInfo=Get-Content -LiteralPath $Upgraded -Raw|ConvertFrom-Json
 if($Info.class -ne $NewInfo.class -or $Info.variable_guid -ne $NewInfo.variable_guid){throw 'Class path or variable GUID changed'}
 $RenamedSource=(Get-FileHash -LiteralPath $ScriptPath -Algorithm SHA256).Hash
 if($OriginalSource -eq $RenamedSource){throw 'Renamed DAS fixture was not updated'}
 $New=Join-Path $Root 'Renamed'
 & "$PSScriptRoot/TestPackaged.ps1" -Engine $Engine -SkipZenStore -DDC 'NoZenLocalFallback' -SkipSmoke `
  -AdditionalCookDirectories @($Info.cook_directory) -ArchiveDirectory $New
 $Loaded=Run-CompatibilityPackage $New Read
 if((Get-FileHash -LiteralPath $DataFile -Algorithm SHA256).Hash -ne $SaveHash){throw 'Original save changed during loading'}
 if((Lock-SourceHashes -Paths $Sources) -cne $Before){throw 'Implementation sources changed during compatibility verification'}
 Write-DasValidation -Path $ResultFile -Data ([ordered]@{state='PASS';probe=$Probe;runtime=$Info;editor_rename=$NewInfo;
  packaged_old=$Written;packaged_renamed=$Loaded;rename_outcome='stock cooked load defaults renamed field';
  native_save_format='GVAS';original_save_sha256=$SaveHash;original_source_sha256=$OriginalSource;renamed_source_sha256=$RenamedSource;
  implementation_sha256=($Before|ConvertFrom-Json);original_archive=$Old;renamed_archive=$New;PIE='not_run';full_suites='not_run'})
 Write-Output 'DAS_SAVEGAME_COMPATIBILITY=PASS runtime=PASS packaged_two_versions=PASS'
} finally {
 if($ScriptPath -and (Test-Path -LiteralPath $ScriptPath -PathType Leaf)){Remove-Item -LiteralPath $ScriptPath}
}
