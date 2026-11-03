param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
foreach($pass in 1..2) {
    & "$PSScriptRoot/TestRogueActions.ps1" -Engine $Engine -InputLifecycle -SkipBuild:($SkipBuild -or $pass -gt 1)
}
Write-Output 'DAS_ROGUE_INPUT_RESTART=PASS runs=2'
