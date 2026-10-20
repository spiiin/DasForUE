param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild) { Invoke-DasBuild $Das -Name 'GAS Shipping exporter' }
Invoke-DasCommandlet $Das -Run DasGASPlaygroundMap -Name 'gas-ability-shipping-export' -Marker 'DAS_GAS_MAP_EXPORT=PASS' | Out-Null
$Map='/Game/DasExamples/GASCues/GASAbilityMap'
$CookDirs=@((Join-Path $Das.Repo 'Tests\DasForUESmoke\Content\DasExamples\GASCues'),
            (Join-Path $Engine 'Engine\Content\EngineSounds'))
$Archive=Join-Path $Das.Repo 'Build\PackagedShipping'
$Previous=$env:DAS_GAS_SHIPPING_VALIDATION
# Shipping intentionally ignores the positional startup-map override. Stage a real
# default map in this isolated smoke project, restoring exact source bytes/time.
$Config=Join-Path $Das.Repo 'Tests\DasForUESmoke\Config\DefaultEngine.ini'
$Original=[IO.File]::ReadAllBytes($Config)
$OriginalTime=[IO.File]::GetLastWriteTimeUtc($Config)
$Text=[Text.Encoding]::UTF8.GetString($Original)
if($Text -notmatch '(?m)^GameDefaultMap=.+$') { throw 'Smoke default-map entry is missing' }
try {
    $Text=[regex]::Replace($Text,'(?m)^GameDefaultMap=.+$',"GameDefaultMap=$Map")
    [IO.File]::WriteAllText($Config,$Text,[Text.UTF8Encoding]::new($false))
    # Module-local opt-in; does not enable engine Shipping logging or alter Shipping optimization.
    $env:DAS_GAS_SHIPPING_VALIDATION='1'
    & (Join-Path $PSScriptRoot 'TestPackaged.ps1') -Engine $Engine -Configuration Shipping `
        -ArchiveDirectory $Archive -AdditionalMaps @($Map) -AdditionalCookDirectories $CookDirs -SkipZenStore -SkipSmoke
} finally {
    $env:DAS_GAS_SHIPPING_VALIDATION=$Previous
    [IO.File]::WriteAllBytes($Config,$Original)
    [IO.File]::SetLastWriteTimeUtc($Config,$OriginalTime)
}
$Report=Join-Path $Das.Logs 'gas-ability-shipping.txt'
if(Test-Path -LiteralPath $Report) { Remove-Item -LiteralPath $Report }
$Executable=Join-Path $Archive 'Windows\DasForUESmoke\Binaries\Win64\DasForUESmoke-Win64-Shipping.exe'
$Arguments=@($Map,'-DasGASAbilityPackaged','-unattended','-nosplash','-RenderOffscreen',
             '-windowed','-ResX=960','-ResY=600',('-DasGASReport="'+$Report+'"'))
$Process=Start-Process -FilePath $Executable -WorkingDirectory (Split-Path -Parent $Executable) `
    -ArgumentList $Arguments -WindowStyle Hidden -PassThru
if(!$Process.WaitForExit(600000)) {
    Stop-Process -Id $Process.Id -Force
    throw 'Shipping GAS process exceeded the 10-minute startup/runtime deadline'
}
$Process.Refresh()
if(Test-Path -LiteralPath $Report) { Get-Content -LiteralPath $Report | Write-Output }
if($Process.ExitCode) { throw "Shipping GAS failed: exit $($Process.ExitCode)" }
Assert-DasLog -Log $Report -Name 'Shipping GAS' -Marker 'DAS_GAS_ABILITY_PACKAGED_RESULT=PASS' `
    -FailPattern 'DAS_GAS_ABILITY_PACKAGED_FAIL|DAS_GAS_AOT_ERROR='
foreach($Marker in @('DAS_GAS_PACKAGE_CONFIGURATION=Shipping',
                     'DAS_GAS_ABILITY_PACKAGED_PASS linked pure DAS AOT executes without VM fallback',
                     'DAS_GAS_AOT_SCOPE=pure-smoke; GAS hooks remain VM')) {
    Assert-DasLog -Log $Report -Name 'Shipping GAS identity/AOT' -Marker $Marker
}
Write-Output "DAS_GAS_ABILITY_SHIPPING_PACKAGE=PASS map=$Map gas=VM pure_aot=PASS"
