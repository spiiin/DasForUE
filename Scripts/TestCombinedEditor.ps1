# Editor half of the full regression; the suite list is the 'editor' preset in Tests/suites.json.
param([string]$Engine = 'C:\src\unreal\UnrealEngine', [string]$Python = 'C:\Python312\python.exe', [switch]$SkipBuild)
$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'RunDasTests.ps1') -Engine $Engine -Python $Python -Preset editor -SkipBuild:$SkipBuild
Write-Output 'DAS_COMBINED_EDITOR=PASS'
