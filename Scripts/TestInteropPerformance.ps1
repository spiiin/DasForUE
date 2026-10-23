param([string]$Engine='C:\src\unreal\UnrealEngine',[ValidateRange(3,31)][int]$Samples=9,[switch]$SkipBuild,
      [switch]$SkipAotGeneration)
# Informational call-cost benchmark C++ <-> daScript (DasInteropPerformance commandlet). The AOT cases need native
# code for interop_aot.das linked in: the editor is built, generates Generated/InteropAot.cpp, and is relinked.
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
if (!$SkipAotGeneration) {
    if (!$SkipBuild) {Invoke-DasBuild $Das -Name 'Interop AOT generator'}
    $Generated=Join-Path $Das.Repo 'Plugins\DasForUE\Source\DasRuntime\Private\Generated\InteropAot.cpp'
    Invoke-DasCommandlet $Das -Run DasUEAot -Name 'interop-aot-generate' -Marker 'DAS_UE_AOT_GENERATED=PASS' `
        -Arguments @('-Script=interop_aot.das',"-Output=$Generated") | Out-Null
    Invoke-DasBuild $Das -Name 'Link interop AOT' -Log (Join-Path $Das.Logs 'interop-aot-link.log')
} elseif (!$SkipBuild) {Invoke-DasBuild $Das -Name 'Interop benchmark'}
$Output=Join-Path $Das.Logs 'interop-performance-editor.json'
if (Test-Path -LiteralPath $Output) {Remove-Item -LiteralPath $Output}
Invoke-DasCommandlet $Das -Run DasInteropPerformance -Name 'interop-performance-editor' -NoZen `
    -Marker 'DAS_INTEROP_PERFORMANCE_RESULT=PASS' -Arguments @("-PerfSamples=$Samples","-PerfOutput=$Output") | Out-Null
$Result=Get-Content -LiteralPath $Output -Raw|ConvertFrom-Json
if ($Result.result -ne 'PASS' -or $Result.sample_count -ne $Samples) {throw "Incomplete benchmark results: $($Result.error)"}
$Result.cases|Select-Object group,case,@{n='ns/call';e={[math]::Round($_.median_ns_per_call,1)}},
    @{n='p95';e={[math]::Round($_.p95_ns_per_call,1)}}|Format-Table -AutoSize
Write-Output "INTEROP PERFORMANCE editor PASS $Output"
