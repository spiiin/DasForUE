param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
[ordered]@{state='pending';feature='GameplayTag and GameplayTagContainer DSL'}|ConvertTo-Json|Set-Content "$Repo/logs/gameplay-tags-validation.json" -Encoding utf8
function Read-GameplayTagsSourceHashes {
 $Result=@{}
 foreach($File in Get-ChildItem "$Repo/Plugins/DasForUE/Source" -Recurse -File){$Result[$File.FullName]=(Get-FileHash -LiteralPath $File.FullName -Algorithm SHA256).Hash}
 # Hash versioned scripts and this feature's new modules; ignore unrelated ephemeral probes.
 $Scripts=@(& git -C $Repo ls-files -- 'Plugins/DasForUE/Content/Das/Scripts/*.das')+@('Plugins/DasForUE/Content/Das/Scripts/ue_gameplay_tags.das','Plugins/DasForUE/Content/Das/Scripts/gameplaytag_items.das','Plugins/DasForUE/Content/Das/Scripts/gameplaytag_item_types.das')
 foreach($Relative in $Scripts|Select-Object -Unique){$Path=Join-Path $Repo $Relative;$Result[$Path]=(Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash}
 $Result["$Repo/Tests/DasForUESmoke/Config/DefaultGameplayTags.ini"]=(Get-FileHash "$Repo/Tests/DasForUESmoke/Config/DefaultGameplayTags.ini" -Algorithm SHA256).Hash
 $Result["$Repo/Tests/GameplayTags/probe.das"]=(Get-FileHash "$Repo/Tests/GameplayTags/probe.das" -Algorithm SHA256).Hash
 return $Result
}
function Assert-GameplayTagsSourceHashes($Expected) {
 $Actual=Read-GameplayTagsSourceHashes
 if($Expected.Count -ne $Actual.Count){throw 'Source file set changed during GameplayTags verification'}
 foreach($Path in $Expected.Keys){if($Expected[$Path] -ne $Actual[$Path]){throw "Source changed during GameplayTags verification: $Path"}}
}
$Sources=Read-GameplayTagsSourceHashes
if(!$SkipBuild){
 Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=8', "-Log=$Repo/logs/gameplay-tags-build.log") *> "$Repo/logs/gameplay-tags-build-stdout.log"
 if($LASTEXITCODE){throw 'GameplayTags build failed'}
}
Assert-GameplayTagsSourceHashes $Sources
$Hashes=@{}
foreach($Name in @('DasRuntime','DasEditor')){$Path="$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$Name.dll";$Hashes[$Path]=(Get-FileHash $Path -Algorithm SHA256).Hash}
$Manifest="$Engine/Engine/Binaries/Win64/UnrealEditor.modules"
$BuildId=(Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId
$Log="$Repo/logs/gameplay-tags-runtime.log"
try {
 & "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasGameplayTags "-Fixture=$Repo/Tests/GameplayTags/probe.das" -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$Log" *> "$Repo/logs/gameplay-tags-runtime-stdout.log"
 $Code=$LASTEXITCODE;$Text=Get-Content $Log -Raw
 $Result=(Get-DasResult -Text $Text -Pattern 'DAS_GAMEPLAY_TAGS_RESULT=PASS checks=(\d+)')
 if($Code -or !$Result.Success){throw "GameplayTags runtime probe failed: $Log"}
 if($Text -match 'Ensure condition failed|Log[^:]+: Error:'){throw 'Engine error/ensure in GameplayTags probe'}
 foreach($Path in $Hashes.Keys){if((Get-FileHash $Path -Algorithm SHA256).Hash -ne $Hashes[$Path]){throw 'DLL changed during probe'}}
 Assert-GameplayTagsSourceHashes $Sources
 if((Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId -ne $BuildId){throw 'Engine BuildId changed during probe'}
 [ordered]@{state='PASS';runtime_checks=[int]$Result.Groups[1].Value;source_sha256=$Sources;dll_sha256=$Hashes;engine_build_id=$BuildId;runtime_log=$Log;build=$(if($SkipBuild){'existing'}else{'PASS'});PIE='not_run';packaged='not_run';physical_input='not_run';commit='not_created'}|ConvertTo-Json -Depth 8|Set-Content "$Repo/logs/gameplay-tags-validation.json" -Encoding utf8
 Write-Output "DAS_GAMEPLAY_TAGS_VALIDATION=PASS checks=$($Result.Groups[1].Value)"
} finally {}
