# Full regression: the 'full' preset in Tests/suites.json (editor suites, defaults probes, cooked export and
# packaged maps). -SkipEditor reuses the method-values probe of the previous editor run.
param([string]$Engine = 'C:\src\unreal\UnrealEngine', [string]$Python = 'C:\Python312\python.exe', [switch]$SkipEditor)
$ErrorActionPreference = 'Stop'
$Run = Join-Path $PSScriptRoot 'RunDasTests.ps1'
if ($SkipEditor) {
    & $Run -Engine $Engine -Python $Python -Suite native-defaults, computed-defaults, packaged -NoDependencies
} else {
    & $Run -Engine $Engine -Python $Python -Preset full
}
Write-Output 'DAS_COMBINED_PACKAGE=PASS'
