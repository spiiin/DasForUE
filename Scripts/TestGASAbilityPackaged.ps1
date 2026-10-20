param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild) { Invoke-DasBuild $Das -Name 'GAS packaged exporter' }
# The same factory as PIE regenerates only the dedicated demo assets/map.
Invoke-DasCommandlet $Das -Run DasGASPlaygroundMap -Name 'gas-ability-package-export' -Marker 'DAS_GAS_MAP_EXPORT=PASS' | Out-Null
$Map='/Game/DasExamples/GASCues/GASAbilityMap'
# Cue scripts hold soft paths. Explicit cook roots also include cue Blueprints found by GAS discovery.
$CookDirs=@((Join-Path $Das.Repo 'Tests\DasForUESmoke\Content\DasExamples\GASCues'),
            (Join-Path $Engine 'Engine\Content\EngineSounds'))
& (Join-Path $PSScriptRoot 'TestPackaged.ps1') -Engine $Engine -AdditionalMaps @($Map) `
    -AdditionalCookDirectories $CookDirs -SkipZenStore -SkipSmoke
$Log=Join-Path $Das.Logs 'gas-ability-packaged.log'
$Screenshot=Join-Path $Das.Logs 'gas-ability-packaged.png'
foreach($File in @($Log,$Screenshot)) { if(Test-Path -LiteralPath $File) { Remove-Item -LiteralPath $File } }
$Arguments=@($Map,'-DasGASAbilityPackaged','-unattended','-nosplash','-RenderOffscreen','-windowed','-ResX=960','-ResY=600',
             ('-abslog="'+$Log+'"'),('-DasGASScreenshot="'+$Screenshot+'"'))
# Run the archived executable from its own directory, with real rendering and audio.
$Process=Start-Process -FilePath $Das.Packaged -WorkingDirectory (Split-Path -Parent $Das.Packaged) `
    -ArgumentList $Arguments -WindowStyle Hidden -PassThru
if(!$Process.WaitForExit(600000)) {
    Stop-Process -Id $Process.Id -Force
    throw 'Packaged GAS process exceeded the 10-minute startup/runtime deadline'
}
$Process.Refresh()
if($Process.ExitCode) { throw "Packaged GAS failed: exit $($Process.ExitCode)" }
Assert-DasLog -Log $Log -Name 'GAS packaged' -Marker 'DAS_GAS_ABILITY_PACKAGED_RESULT=PASS' `
    -FailPattern 'DAS_GAS_ABILITY_PACKAGED_FAIL|LogDasRuntime:\s+Error:|Fatal error:'
if(!(Test-Path -LiteralPath $Screenshot -PathType Leaf)) { throw 'Packaged GAS viewport screenshot is missing' }
Write-Output "DAS_GAS_ABILITY_DEVELOPMENT_PACKAGE=PASS map=$Map"
