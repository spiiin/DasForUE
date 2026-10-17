param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Logs=Join-Path $Repo 'logs'
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
$Content=Join-Path (Split-Path -Parent $Project) 'Content'
$Playable=Join-Path $Content 'DasExamples/Playable'
$Shared=Join-Path $Content 'DasExamples/Shared/Interactable.uasset'
New-Item -ItemType Directory -Path $Logs -Force|Out-Null
function Existing-InterfaceCopies {
    $Copies=@{}
    if(Test-Path -LiteralPath $Playable){
        Get-ChildItem -LiteralPath $Playable -Filter Interactable.uasset -File -Recurse | ForEach-Object {
            $Copies[$_.FullName]=(Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash
        }
    }
    return $Copies
}
$Before=Existing-InterfaceCopies
& "$PSScriptRoot/TestPickup.ps1" -Engine $Engine -SkipBuild:$SkipBuild -Project $Project *> "$Logs/shared-interface-pickup.log"
if(!$?){throw 'Main-project pickup validation failed'}
$Pickup=Get-Content -LiteralPath "$Logs/pickup-validation.json" -Raw|ConvertFrom-Json
if($Pickup.result -ne 'PASS' -or $Pickup.project -ne $Project){throw 'Pickup did not validate the main project'}
if(!(Test-Path -LiteralPath $Shared)){throw 'Canonical shared interface asset missing'}
# This creates another fresh map, reuses the interface and exercises real interface dispatch.
& "$PSScriptRoot/TestGameplayFeatures.ps1" -Engine $Engine -SkipBuild *> "$Logs/shared-interface-gameplay.log"
if(!$?){throw 'Repeated export/interface-dispatch validation failed'}
$Gameplay=Get-Content -LiteralPath "$Logs/gameplay-features-validation.json" -Raw|ConvertFrom-Json
if($Gameplay.result -ne 'PASS' -or $Gameplay.map -eq $Pickup.map){throw 'Expected a second independently exported map'}
$After=Existing-InterfaceCopies
if($Before.Count -ne $After.Count){throw 'Export created another per-map interface copy'}
foreach($File in $Before.Keys){if(!$After.ContainsKey($File) -or $After[$File] -ne $Before[$File]){throw "Old interface asset was modified: $File"}}
foreach($Map in @($Pickup.map,$Gameplay.map)){
    $Folder=Join-Path $Content ($Map -replace '^/Game/','' -replace '/PlayableMap$','')
    if(Get-ChildItem -LiteralPath $Folder -Filter Interactable.uasset -Recurse -File){throw "Map has its own interface copy: $Map"}
}
[ordered]@{result='PASS';project=$Project;canonical_interface='/Game/DasExamples/Shared/Interactable.Interactable_C';maps=@($Pickup.map,$Gameplay.map);old_interface_copies_preserved=$Before.Count;old_interface_sha256=$Before;checks=@('main-project async pickup scenarios','repeat export reuses shared interface','Enhanced Input trace -> interface dispatch -> pickup score/HUD','no new per-map Interactable assets','old copies preserved byte-for-byte');not_run=@('Foundation suite','full PIE suite','packaged build');commit_created=$false}|ConvertTo-Json -Depth 5|Set-Content -LiteralPath "$Logs/shared-interface-validation.json" -Encoding utf8
Write-Output "DAS_SHARED_INTERFACE_FOCUSED=PASS old_copies=$($Before.Count)"
