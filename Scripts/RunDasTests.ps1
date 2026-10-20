<#
Runs regression suites described in Tests/suites.json.

    .\Scripts\RunDasTests.ps1 -Preset quick          # build, Python, Foundation, automation without MethodValues
    .\Scripts\RunDasTests.ps1 -Preset full           # everything TestCombined.ps1 runs
    .\Scripts\RunDasTests.ps1 -Suite foundation,automation-quick -SkipBuild
    .\Scripts\RunDasTests.ps1 -List

Prerequisites listed in a suite's 'requires' are added automatically; with
-NoDependencies their probe ids are read from the previous run's logs instead.
Prints DAS_SUITE_<NAME>=PASS per suite and DAS_SUITES=PASS at the end.
#>
param([string]$Engine = 'C:\src\unreal\UnrealEngine', [string]$Python = 'C:\Python312\python.exe',
      [string]$Preset, [string[]]$Suite = @(), [switch]$SkipBuild, [switch]$NoDependencies, [switch]$List,
      [string]$Manifest)
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
if (!$Manifest) { $Manifest = Join-Path $Das.Repo 'Tests\suites.json' }
$Data = Get-Content -LiteralPath $Manifest -Raw | ConvertFrom-Json
$All = @($Data.suites)
$ByName = @{}
foreach ($S in $All) { $ByName[$S.name] = $S }

if ($List) {
    foreach ($S in $All) { Write-Output ("{0,-20} {1}" -f $S.name, $S.kind) }
    foreach ($P in $Data.presets.PSObject.Properties) { Write-Output ("preset {0}: {1}" -f $P.Name, ($P.Value -join ', ')) }
    return
}

# Selection: preset and/or explicit suites (comma-separated values are accepted from the build agent).
$Wanted = @()
if ($Preset) {
    $P = $Data.presets.PSObject.Properties[$Preset]
    if (!$P) { throw "Unknown preset '$Preset'" }
    $Wanted += @($P.Value)
}
$Wanted += @($Suite | ForEach-Object { $_ -split ',' } | Where-Object { $_ })
if (!$Wanted) { throw 'Pass -Preset or -Suite (see -List)' }
$Selected = @{}
function Add-DasSuite([string]$Name) {
    if (!$ByName.ContainsKey($Name)) { throw "Unknown suite '$Name'" }
    if ($Selected.ContainsKey($Name)) { return }
    $Selected[$Name] = $true
    if (!$NoDependencies) { foreach ($R in @($ByName[$Name].requires | Where-Object { $_ })) { Add-DasSuite $R } }
}
foreach ($Name in $Wanted) { Add-DasSuite $Name }
if ($SkipBuild) { $Selected.Remove('build') }
$Plan = @($All | Where-Object { $Selected.ContainsKey($_.name) })
Write-Output ('DAS_SUITES_PLAN=' + (($Plan | ForEach-Object { $_.name }) -join ','))

# Probe ids produced by earlier suites; falls back to the probe file of a previous run.
$Probes = @{}
function Get-DasProbe([string]$Name) {
    if ($Probes.ContainsKey($Name)) { return $Probes[$Name] }
    $File = Join-Path $Das.Logs $ByName[$Name].probe
    if (!(Test-Path -LiteralPath $File)) { throw "Suite '$Name' has not run and $File does not exist" }
    return (Get-Content -LiteralPath $File -Raw).Trim()
}

$Steps = @{
    build      = { param($S) Invoke-DasBuild $Das -Name 'Editor' -Log (Join-Path $Das.Logs "$($S.log).log") }
    python     = { param($S)
        Push-Location $Das.Repo
        # Windows PowerShell turns redirected native stderr (unittest's report) into terminating errors under Stop.
        $Previous = $ErrorActionPreference; $ErrorActionPreference = 'Continue'
        try { & $Python -m unittest discover -s Scripts -p 'Test*.py' -v *> (Join-Path $Das.Logs "$($S.log).log"); $Code = $LASTEXITCODE }
        finally { $ErrorActionPreference = $Previous; Pop-Location }
        if ($Code) { throw "Generator tests failed; see $($S.log).log" }
    }
    commandlet = { param($S) Invoke-DasCommandlet $Das -Run $S.run -Name $S.log -Marker $S.marker | Out-Null }
    phases     = { param($S)
        $Id = Invoke-DasAssetPhases $Das -Run $S.run -Name $S.log -Marker $S.marker -Phases @($S.phases)
        Set-Content -LiteralPath (Join-Path $Das.Logs $S.probe) -Value $Id
        $Probes[$S.name] = $Id
    }
    automation = { param($S)
        $Extra = @('-ini:Engine:[ConsoleVariables]:HomeScreen.EnableHomeScreen=0')
        if ($S.probeArgs) {
            foreach ($A in $S.probeArgs.PSObject.Properties) { $Extra += "-$($A.Name)=$(Get-DasProbe $A.Value)" }
        }
        Invoke-DasAutomation $Das -Test $S.test -Name $S.log -Count $S.minimum -AtLeast -Extra $Extra
    }
    script     = { param($S)
        & (Join-Path $PSScriptRoot $S.script) -Engine $Engine -SkipBuild
        $Probes[$S.name] = (Get-Content -LiteralPath (Join-Path $Das.Logs $S.probe) -Raw).Trim()
    }
    packaged   = { param($S)
        $Ids = @{ export = [Guid]::NewGuid().ToString('N') }
        foreach ($R in @($S.requires)) { $Ids[$R] = Get-DasProbe $R }
        $Maps = @($S.maps | ForEach-Object {
            $Map = $_.map
            foreach ($K in $Ids.Keys) { $Map = $Map.Replace("{$K}", $Ids[$K]) }
            [pscustomobject]@{ Name = $_.name; Map = $Map; Switch = $_.switch; Marker = $_.marker }
        })
        Invoke-DasCommandlet $Das -Run DasCookedGameplay -Name $S.log -Marker 'DAS_COOKED_GAMEPLAY_EXPORT=PASS' `
            -Arguments @("-ProbeId=$($Ids.export)") | Out-Null
        & (Join-Path $PSScriptRoot 'TestPackaged.ps1') -Engine $Engine -AdditionalMaps @($Maps | ForEach-Object { $_.Map }) -SkipZenStore
        foreach ($M in $Maps) {
            Invoke-DasPackaged $Das -Name "combined-packaged-$($M.Name)" -Marker $M.Marker -Arguments @($M.Map, $M.Switch) | Out-Null
            Write-Output ("DAS_COMBINED_PACKAGED_" + $M.Name.ToUpper() + "=PASS map=" + $M.Map)
        }
    }
}

$PreviousSdkSetup = $env:UE_SKIP_UBT_SDK_SETUP
try {
    $env:UE_SKIP_UBT_SDK_SETUP = '1'
    foreach ($S in $Plan) {
        $Started = Get-Date
        & $Steps[$S.kind] $S
        Write-Output ("DAS_SUITE_{0}=PASS seconds={1:0}" -f $S.name.ToUpper().Replace('-', '_'), ((Get-Date) - $Started).TotalSeconds)
    }
    Write-Output 'DAS_SUITES=PASS'
} finally { $env:UE_SKIP_UBT_SDK_SETUP = $PreviousSdkSetup }
