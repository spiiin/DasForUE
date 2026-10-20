param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild,[switch]$SkipPreparation)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
if(!$SkipBuild){
 Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=8', "-Log=$Repo/logs/input-ui-build.log") *> "$Repo/logs/input-ui-build-stdout.log"
 if($LASTEXITCODE){throw 'Input/UI build failed'}
}
function GetInputUIHashes {
 $Hashes=[ordered]@{}
 foreach($Name in @(
  'Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-DasRuntime.dll',
  'Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-DasEditor.dll',
  'Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-DasForUESmoke.dll',
  'Scripts/GenerateBindings.py',
  'Plugins/DasForUE/Source/DasRuntime/Private/DasRuntime.cpp',
  'Plugins/DasForUE/Source/DasRuntime/Private/DasDSL.inl',
  'Plugins/DasForUE/Source/DasRuntime/Private/DasDirectTypes.inl',
  'Plugins/DasForUE/Source/DasRuntime/Private/DasInputActionValue.inl',
  'Plugins/DasForUE/Source/DasRuntime/Private/DasDelegateValues.inl',
  'Plugins/DasForUE/Source/DasRuntime/Private/DasEnhancedInputProxy.h',
  'Plugins/DasForUE/Source/DasRuntime/Private/DasEnhancedInputProxy.cpp',
  'Plugins/DasForUE/Source/DasRuntime/Private/DasCallbackTypes.cpp',
  'Plugins/DasForUE/Source/DasRuntime/Private/DasBindingExport.inl',
  'Plugins/DasForUE/Source/DasRuntime/Private/DasMarshalling.inl',
  'Plugins/DasForUE/Content/Das/Scripts/ue_input_value.das',
  'Plugins/DasForUE/Content/Das/Scripts/ue_enhanced_input.das',
  'Plugins/DasForUE/Content/Das/Scripts/ue_native_delegate.das',
  'Plugins/DasForUE/Content/Das/Scripts/input_ui_probe.das',
  'Plugins/DasForUE/Content/Das/Scripts/dsl_playable_character.das',
  'Tests/DasForUESmoke/Source/DasForUESmoke/DasInputUI.cpp',
  'Tests/DasForUESmoke/Source/DasForUESmoke/DasInputUIProbe.h'
 )) { $Hashes[$Name]=(Get-FileHash -LiteralPath "$Repo/$Name" -Algorithm SHA256).Hash }
 return ($Hashes|ConvertTo-Json -Compress)
}
$Before=GetInputUIHashes
if(!$SkipPreparation){
& "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile "$Repo/Bindings/Packages/UMGInput.profile.json" -SkipBuild -Publish *> "$Repo/logs/input-ui-package.log"
if(!$?){throw 'Input/UI package preflight failed'}
& "$PSScriptRoot/CreatePlayableCharacter.ps1" -Engine $Engine -SkipBuild *> "$Repo/logs/input-ui-create.log"
if(!$?){throw 'Input/UI example refresh failed'}
}
$Example=Get-Content "$Repo/logs/playable-example.json" -Raw|ConvertFrom-Json
$Log="$Repo/logs/input-ui-game.log"
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project $Example.map -game -DasInputUI -Multiprocess -unattended -NullRHI -nosplash -stdout "-abslog=$Log" -ddc=NoZenLocalFallback -NoZenAutoLaunch *> "$Repo/logs/input-ui-game-stdout.log"
if($LASTEXITCODE -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_INPUT_UI=PASS' -Quiet)){throw "Input/UI runtime failed: $Log"}
$Text=Get-Content $Log -Raw
if($Text.Contains('Ensure condition failed') -or $Text -match 'Log[^\r\n]*: Error:'){throw 'Engine error/ensure in Input/UI run'}
if((GetInputUIHashes) -ne $Before){throw 'Input/UI source or DLL changed during validation'}
[ordered]@{state='PASS';hashes=($Before|ConvertFrom-Json);scope='one short combined game scenario';typed_input='four shapes via native proxy; owned array copies';authored_input='real local-player action injection';UMG='generated navigation delegate and wrong-signature rejection; managed HUD click; UI/game cursor';cleanup='EnhancedInput handles removed on ReleaseObject';not_run=@('full PIE suite','packaged','physical input','rendering','reload suite');commit='not_created'}|ConvertTo-Json -Depth 5|Set-Content "$Repo/logs/input-ui-validation.json" -Encoding utf8
Write-Output 'DAS_INPUT_UI=PASS'
