param([string]$Engine='C:\src\unreal\UnrealEngine',[string]$PortRoot='C:\src\ActionRoguelikeDas',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild) { Invoke-DasBuild $Das -Name 'Rogue patrol point preparation' }
$Source=Join-Path (Split-Path -Parent $Das.Project) 'Binaries/Win64'
$Target=Join-Path $PortRoot 'Binaries/Win64'
$Modules=Get-Content -LiteralPath (Join-Path $Source 'UnrealEditor.modules') -Raw | ConvertFrom-Json
$EngineModules=Get-Content -LiteralPath (Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor.modules') -Raw | ConvertFrom-Json
if($Modules.BuildId -ne $EngineModules.BuildId) { throw 'Editor modules have different BuildIds' }
Copy-Item -LiteralPath (Join-Path $Source 'UnrealEditor-DasRuntime.dll'),(Join-Path $Source 'UnrealEditor-DasEditor.dll') -Destination $Target -Force
@{BuildId=$Modules.BuildId;Modules=@{DasRuntime='UnrealEditor-DasRuntime.dll';DasEditor='UnrealEditor-DasEditor.dll'}} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $Target 'UnrealEditor.modules') -Encoding utf8
$Probe=Join-Path $PortRoot 'Content/Das/Scripts/roguetest/patrol_point_compile.das'
Copy-Item -LiteralPath (Join-Path (Split-Path -Parent $PSScriptRoot) 'Tests/Navigation/patrol_point_compile.das') -Destination $Probe -Force
try {
    $Log=Join-Path $Das.Logs 'rogue-patrol-points-compile.log'
    $Arguments=@((Join-Path $PortRoot 'ActionRoguelikeDas.uproject'),'-run=DasExport',('-Probe='+$Probe),'-Multiprocess','-unattended','-NullRHI','-nosplash','-NoZenAutoLaunch','-ddc=NoZenLocalFallback',('-abslog='+$Log))
    & (Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe') @Arguments *> ($Log+'.stdout')
    if($LASTEXITCODE) { throw ('Patrol point compilation failed; see '+$Log) }
    if(!(Select-String -LiteralPath $Log -SimpleMatch 'DAS_PACKAGE_PREFLIGHT=PASS value=42' -Quiet)) { throw 'Missing patrol point compilation marker' }
} finally {
    Remove-Item -LiteralPath $Probe -Force -ErrorAction SilentlyContinue
}
Write-Output 'DAS_ROGUE_PATROL_POINTS=PASS source_compile_and_reference'
