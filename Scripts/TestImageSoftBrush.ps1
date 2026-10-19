param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild,[switch]$Publish)
$ErrorActionPreference='Stop';$Repo=Split-Path -Parent $PSScriptRoot
$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
[ordered]@{state='pending';feature='Image managed soft brush'}|ConvertTo-Json|Set-Content "$Repo/logs/image-soft-brush-validation.json" -Encoding utf8
if(!$SkipBuild){& "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=8 "-Log=$Repo/logs/image-soft-brush-build.log" *> "$Repo/logs/image-soft-brush-build-stdout.log";if($LASTEXITCODE){throw 'ImageSoftBrush build failed'}}
$Sources=@{};foreach($File in @('Plugins/DasForUE/Source/DasRuntime/Private/DasImageSoftBrush.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasImageSoftBrush.h','Plugins/DasForUE/Source/DasRuntime/Private/DasImageSoftBrushRuntime.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasMarshalling.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasRuntime.cpp','Plugins/DasForUE/Source/DasEditor/Private/DasImageSoftBrushCommandlet.cpp','Plugins/DasForUE/Content/Das/Scripts/ue_image_soft_brush.das','Scripts/GenerateBindings.py','Tests/ImageSoftBrush/probe.template.das')){$Sources[$File]=(Get-FileHash "$Repo/$File" -Algorithm SHA256).Hash}
$Hashes=@{};foreach($Name in @('DasRuntime','DasEditor')){$Path="$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$Name.dll";$Hashes[$Path]=(Get-FileHash $Path -Algorithm SHA256).Hash}
$Manifest="$Engine/Engine/Binaries/Win64/UnrealEditor.modules";$BuildId=(Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId
& "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile "$Repo/Bindings/Packages/UMGInput.profile.json" -SkipBuild -Publish:$Publish|Tee-Object -Variable PackageOutput
if(!$?){throw 'ImageSoftBrush package generation failed'}
$Match=[regex]::Match(($PackageOutput -join "`n"),'DAS_API_PACKAGE=PASS[^\r\n]*report=(.+/report\.json)');if(!$Match.Success){throw 'Missing UMG package report'}
$ReportPath=$Match.Groups[1].Value.Trim();$Stage=Split-Path -Parent $ReportPath;$Report=Get-Content $ReportPath -Raw|ConvertFrom-Json
$Open=@($Report.members|Where-Object {$_.status -eq 'generation_ready'}|ForEach-Object {$_.class+'.'+$_.function})
foreach($Name in @('/Script/UMG.Image.SetBrushFromSoftTexture','/Script/UMG.Image.SetBrushFromSoftMaterial')){if($Name -notin $Open){throw "Soft brush function not ready: $Name"}}
$Probe="$Stage/image-soft-brush.das"
$Text=(Get-Content "$Repo/Tests/ImageSoftBrush/probe.template.das" -Raw).Replace('_MODULE_',$(if($Publish){'ue_api_umg_input'}else{$Report.staged_module}))
[IO.File]::WriteAllText($Probe,$Text,(New-Object Text.UTF8Encoding $false))
$Log="$Stage/image-soft-brush.log"
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasImageSoftBrush "-Script=$Probe" -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$Log" *> "$Stage/image-soft-brush-stdout.log"
$Result=[regex]::Match((Get-Content $Log -Raw),'DAS_IMAGE_SOFT_BRUSH_RESULT=PASS checks=(\d+)');if($LASTEXITCODE -or !$Result.Success){throw "ImageSoftBrush runtime probe failed: $Log"}
foreach($Name in @('preflight.log','image-soft-brush.log')){if(Select-String (Join-Path $Stage $Name) -Pattern 'Ensure condition failed|Log[^:]+: Error:' -Quiet){throw "Engine error/ensure: $Name"}}
foreach($File in $Sources.Keys){if((Get-FileHash "$Repo/$File" -Algorithm SHA256).Hash -ne $Sources[$File]){throw "Source changed during ImageSoftBrush tests: $File"}}
foreach($Path in $Hashes.Keys){if((Get-FileHash $Path -Algorithm SHA256).Hash -ne $Hashes[$Path]){throw 'DLL changed during ImageSoftBrush tests'}}
if((Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId -ne $BuildId){throw 'Engine BuildId changed during ImageSoftBrush tests'}
if($Publish){$Summary="$Repo/Bindings/Packages/ue_api_umg_input.summary.json";$Report|ConvertTo-Json -Depth 100|Set-Content $Summary -Encoding utf8;& C:/Python312/python.exe "$PSScriptRoot/NormalizeJson.py" $Summary;if($LASTEXITCODE){throw 'Summary normalization failed'}}
[ordered]@{state='PASS';runtime_checks=[int]$Result.Groups[1].Value;counts=$Report.counts;whole_module_compile='PASS';source_sha256=$Sources;dll_sha256=$Hashes;engine_build_id=$BuildId;report=$ReportPath;runtime_log=$Log;published=[bool]$Publish;viewport_mode='headless UMG widget with NullSlate; visible rendering not tested';full_suites='not_run';PIE='not_run';packaged='not_run'}|ConvertTo-Json -Depth 12|Set-Content "$Repo/logs/image-soft-brush-validation.json" -Encoding utf8
Write-Output "DAS_IMAGE_SOFT_BRUSH_VALIDATION=PASS checks=$($Result.Groups[1].Value) stage=$Stage"
