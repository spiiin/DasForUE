param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
if (!$SkipBuild) { & (Join-Path $PSScriptRoot 'ExportTypeSupport.ps1') -Engine $Engine }
& C:\Python312\python.exe (Join-Path $PSScriptRoot 'GenerateTypeSupport.py') --manifest (Join-Path $Das.Logs 'type-support-live.json') --check
if ($LASTEXITCODE) { throw 'Type support export, baseline or documentation drifted' }
