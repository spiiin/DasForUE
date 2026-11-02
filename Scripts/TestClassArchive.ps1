param([string]$Engine='C:\src\unreal\UnrealEngine', [switch]$SkipBuild)
$ErrorActionPreference='Stop'
& (Join-Path $PSScriptRoot 'TestDSLArchiveFieldsAsset.ps1') -Engine $Engine -SkipBuild:$SkipBuild
& (Join-Path $PSScriptRoot 'TestDSLArchiveFieldsPIE.ps1') -Engine $Engine -SkipBuild
