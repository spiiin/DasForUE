param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
& (Join-Path $PSScriptRoot 'TestRogueActions.ps1') -Engine $Engine -SkipBuild:$SkipBuild -Unattended -Audio -Scenario course_effects
if($LASTEXITCODE){exit $LASTEXITCODE}
