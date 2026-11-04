param([string]$Engine='C:\src\unreal\UnrealEngine', [string]$PortRoot='C:\src\ActionRoguelikeDas', [switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
$Destination=Join-Path (Split-Path -Parent $Das.Project) 'Content/Das/Scripts'
New-Item -ItemType Directory -Path $Destination -Force | Out-Null
# Stage the port dependency closure, including typed action and attribute references.
Get-ChildItem -LiteralPath (Join-Path $PortRoot 'Content/Das/Scripts') -Filter '*.das' -Recurse -File | ForEach-Object {
    Copy-Item -LiteralPath $_.FullName -Destination (Join-Path $Destination $_.Name)
}
# These regressions intentionally exercise the base 30-second projectile.
$AttackFile=Join-Path $Destination 'rogue_action_projectile_attack.das'
$AttackSource=[IO.File]::ReadAllText($AttackFile).Replace('/Game/DasRogue/BP_MagicProjectileDemo.BP_MagicProjectileDemo_C','/Game/DasRogue/BP_ProjectileDemo.BP_ProjectileDemo_C')
# DefaultPawn fixture has no elevated muzzle; keep its zero-pitch physics probe.
$AttackSource=$AttackSource -replace '(var\s+MuzzleHeight\s*:\s*float\s*=\s*)40\.0f', '${1}0.0f'
# This suite probes projectile physics. Default windup/cooldown have a separate real-PIE suite.
$AttackSource=$AttackSource -replace '(var\s+AttackDelay\s*:\s*float\s*=\s*)0\.2f', '${1}0.01f'
$AttackSource=$AttackSource -replace '(self\.CooldownTime\s*=\s*)0\.5f', '${1}0.0f'
[IO.File]::WriteAllText($AttackFile,$AttackSource,[Text.UTF8Encoding]::new($false))
Copy-Item -LiteralPath (Join-Path $PortRoot 'Tests/Das/roguetest/rogue_item_chest_observer.das') -Destination (Join-Path $Destination 'rogue_item_chest_observer.das')
if(!$SkipBuild) { Invoke-DasBuild $Das -Name 'Rogue projectile' }
$Config=Join-Path (Split-Path -Parent $Das.Project) 'Config/DefaultEngine.ini'
$Before=[IO.File]::ReadAllBytes($Config)
$InputConfig=Join-Path (Split-Path -Parent $Das.Project) 'Config/DefaultInput.ini'
$InputBefore=[IO.File]::ReadAllBytes($InputConfig)
$TagsConfig=Join-Path (Split-Path -Parent $Das.Project) 'Config/DefaultGameplayTags.ini'
$TagsBefore=if(Test-Path -LiteralPath $TagsConfig){[IO.File]::ReadAllBytes($TagsConfig)}else{$null}
try {
    Copy-Item -LiteralPath (Join-Path $PortRoot 'Config/DefaultGameplayTags.ini') -Destination $TagsConfig -Force
    $PortConfig=Get-Content -LiteralPath (Join-Path $PortRoot 'Config/DefaultEngine.ini') -Raw
    $Collision=[regex]::Match($PortConfig,'(?ms)^\[/Script/Engine\.CollisionProfile\].*?(?=^\[|\z)').Value
    if(!$Collision) { throw 'Port collision profile section is missing' }
    Add-Content -LiteralPath $Config -Value ("`n"+$Collision) -Encoding utf8
    Add-Content -LiteralPath $InputConfig -Value ("`n"+(Get-Content -LiteralPath (Join-Path $PortRoot 'Config/DefaultInput.ini') -Raw)) -Encoding utf8
    Invoke-DasAutomation $Das -Test 'DasForUE.Ports.RogueProjectilePIE' -Name 'rogue-projectile' -Rendering -Extra @('-DasRoguePort','-ddc=NoZenLocalFallback','-NoZenAutoLaunch','-ini:Engine:[ConsoleVariables]:HomeScreen.EnableHomeScreen=0')
    $RunLog=Join-Path $Das.Logs 'rogue-projectile.log'
    if(Test-DasResult -LiteralPath $RunLog -Pattern 'daScript lifecycle .*:|daScript did not run') { throw 'Projectile emitted a runtime lifecycle error' }
} finally {
    if($null -ne $TagsBefore){[IO.File]::WriteAllBytes($TagsConfig,$TagsBefore)}else{Remove-Item -LiteralPath $TagsConfig -ErrorAction SilentlyContinue}
    [IO.File]::WriteAllBytes($Config,$Before)
    [IO.File]::WriteAllBytes($InputConfig,$InputBefore)
}
