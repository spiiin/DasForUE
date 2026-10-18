param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Profile="$Repo/Bindings/Packages/AnimationAudioVFX.profile.json"
$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
if(!$SkipBuild){
 & "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=8 "-Log=$Repo/logs/animation-audio-vfx-build.log" *> "$Repo/logs/animation-audio-vfx-build-stdout.log"
 if($LASTEXITCODE){throw 'Animation / Audio / VFX Editor build failed'}
}
function GetPackageHashes {
 $Hashes=[ordered]@{}
 foreach($Name in @('Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-DasRuntime.dll','Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-DasEditor.dll','Bindings/Packages/AnimationAudioVFX.profile.json','Scripts/GenerateBindings.py','Scripts/GenerateApiPackage.py')){
  $Hashes[$Name]=(Get-FileHash -LiteralPath "$Repo/$Name" -Algorithm SHA256).Hash
 }
 return ($Hashes|ConvertTo-Json -Compress)
}
$Before=GetPackageHashes
# One runtime probe, embedded in the profile; no extra commandlets or full suites.
& "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile $Profile -SkipBuild | Tee-Object -Variable PackageOutput
if(!$?){throw 'Animation / Audio / VFX package failed'}
$Match=[regex]::Match(($PackageOutput -join "`n"),'DAS_API_PACKAGE=PASS[^\r\n]*report=(.+/report\.json)')
if(!$Match.Success){throw 'Missing successful package report'}
$Report=(Resolve-Path -LiteralPath $Match.Groups[1].Value.Trim()).Path
$Definition=Get-Content -LiteralPath $Report -Raw|ConvertFrom-Json
if($Definition.package -ne 'ue_api_animation_audio_vfx' -or $Definition.state -ne 'ue_preflight_passed'){throw 'Wrong package or unsuccessful preflight'}
$Run=Split-Path -Parent $Report
foreach($Log in @('export.log','preflight.log')){
 $Text=Get-Content -LiteralPath "$Run/$Log" -Raw
 if($Text.Contains('Ensure condition failed') -or $Text -match 'Log[^\r\n]*: Error:'){throw "Engine error/ensure: $Run/$Log"}
}
if((GetPackageHashes) -ne $Before){throw 'DLL, profile or generator changed during validation'}
$Summary=[ordered]@{schema_version=1;package=$Definition.package;engine_version=$Definition.engine_version;state=$Definition.state;value_types=$Definition.value_types;default_arguments=$Definition.default_arguments;candidate_count=$Definition.candidate_count;counts=$Definition.counts;construction_guarded_count=$Definition.construction_guarded_count;source_sha256=$Definition.source_sha256;surfaces=$Definition.surfaces;blockers=@($Definition.members|Where-Object {$_.status -ne 'generation_ready'})}
$Summary|ConvertTo-Json -Depth 100|Set-Content -LiteralPath "$Repo/Bindings/Packages/ue_api_animation_audio_vfx.summary.json" -Encoding utf8
[ordered]@{state='PASS';report=$Report;runtime_probe='KismetAnimationLibrary.CalculateDirection: rightward velocity gives 90 degrees';runtime_probe_count=1;whole_module_compile='PASS';counts=$Definition.counts;candidate_count=$Definition.candidate_count;hashes=($Before|ConvertFrom-Json);published=$false;animation_playback='not_run';audio_playback='not_run';niagara_simulation='not_run';PIE='not_run';packaged='not_run';full_suites='not_run';commit='not_created'}|ConvertTo-Json -Depth 6|Set-Content -LiteralPath "$Run/validation.json" -Encoding utf8
Copy-Item -LiteralPath "$Run/validation.json" -Destination "$Repo/logs/animation-audio-vfx-validation.json" -Force
Write-Output "DAS_ANIMATION_AUDIO_VFX_PACKAGE=PASS report=$Run/validation.json"
