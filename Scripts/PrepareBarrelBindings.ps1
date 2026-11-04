param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
& (Join-Path $PSScriptRoot 'PrepareCoinBindings.ps1') -Engine $Engine -SkipBuild:$SkipBuild
if(!$?) { throw 'Radial force bindings preparation failed' }
