param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
& (Join-Path $PSScriptRoot 'TestRogueActions.ps1') -Engine $Engine -SkipBuild:$SkipBuild -Unattended -Scenario environment
if($LASTEXITCODE){exit $LASTEXITCODE}
