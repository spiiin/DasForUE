param([string]$Engine='C:\src\unreal\UnrealEngine', [string]$PortRoot='C:\src\ActionRoguelikeDas', [switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
$Destination=Join-Path (Split-Path -Parent $Das.Project) 'Content/Das/Scripts'
New-Item -ItemType Directory -Path $Destination -Force | Out-Null
foreach($File in @('rogue_interaction_interface.das','rogue_interaction.das','rogue_item_chest.das')) {
    Copy-Item -LiteralPath (Join-Path $PortRoot "Content/Das/Scripts/rogue/$File") -Destination (Join-Path $Destination $File)
}
Copy-Item -LiteralPath (Join-Path $PortRoot 'Tests/Das/roguetest/rogue_item_chest_observer.das') -Destination (Join-Path $Destination 'rogue_item_chest_observer.das')
if(!$SkipBuild) { Invoke-DasBuild $Das -Name 'Rogue chest' }
$Config=Join-Path (Split-Path -Parent $Das.Project) 'Config/DefaultEngine.ini'
$Before=[IO.File]::ReadAllBytes($Config)
try {
    $PortConfig=Get-Content -LiteralPath (Join-Path $PortRoot 'Config/DefaultEngine.ini') -Raw
    $Collision=[regex]::Match($PortConfig,'(?ms)^\[/Script/Engine\.CollisionProfile\].*?(?=^\[|\z)').Value
    if(!$Collision) { throw 'Port collision profile section is missing' }
    Add-Content -LiteralPath $Config -Value ("`n"+$Collision) -Encoding utf8
    Invoke-DasAutomation $Das -Test 'DasForUE.Ports.RogueChest' -Name 'rogue-chest' -Extra @('-DasRoguePort','-ddc=NoZenLocalFallback','-NoZenAutoLaunch','-ini:Engine:[ConsoleVariables]:HomeScreen.EnableHomeScreen=0')
} finally {
    [IO.File]::WriteAllBytes($Config,$Before)
}
