param([string]$Engine = 'C:\src\unreal\UnrealEngine', [switch]$SkipBuild)
$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'TestDSLTypeRegistry.ps1') -Engine $Engine -AuthoredInterface
