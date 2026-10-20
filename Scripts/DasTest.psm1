<#
Shared helpers for DasForUE2 test scripts.

    Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
    $Das = Get-DasContext -Engine $Engine
    Invoke-DasBuild $Das -Name 'my-feature'
    Invoke-DasCommandlet $Das -Run DasFoo -Name 'foo-create' -Marker 'DAS_FOO_RESULT=PASS' -Arguments @('-Phase=Create')

Every helper throws on failure, so scripts stay a list of steps.
#>

function Get-DasContext {
    param([string]$Engine = 'C:\src\unreal\UnrealEngine')
    $Repo = Split-Path -Parent $PSScriptRoot
    $Logs = Join-Path $Repo 'logs'
    New-Item -ItemType Directory -Path $Logs -Force | Out-Null
    [pscustomobject]@{
        Engine  = $Engine
        Repo    = $Repo
        Logs    = $Logs
        Project = Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
        Editor  = Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
        Packaged = Join-Path $Repo 'Build\Packaged\Windows\DasForUESmoke\Binaries\Win64\DasForUESmoke.exe'
    }
}

# Builds the editor target of the smoke project.
function Invoke-DasBuild {
    param([Parameter(Mandatory)]$Das, [string]$Name = 'editor', [string]$Log, [int]$Parallel = 12)
    $Arguments = @('DasForUESmokeEditor', 'Win64', 'Development', "-Project=$($Das.Project)", '-WaitMutex', '-NoXGE',
                   "-MaxParallelActions=$Parallel")
    if ($Log) { $Arguments += "-Log=$Log" }
    & (Join-Path $Das.Engine 'Engine\Build\BatchFiles\Build.bat') @Arguments | ForEach-Object { Write-Host $_ }
    if ($LASTEXITCODE) { throw "$Name build failed" }
}

# Throws unless the log contains the marker and, with -FailPattern, none of the failure patterns.
function Assert-DasLog {
    param([Parameter(Mandatory)][string]$Log, [Parameter(Mandatory)][string]$Marker, [string]$FailPattern,
          [string]$Name = $Log)
    if (!(Test-Path -LiteralPath $Log)) { throw "$Name produced no log: $Log" }
    if ($FailPattern -and (Select-String -LiteralPath $Log -Pattern $FailPattern)) { throw "$Name failed: $Log" }
    if (!(Select-String -LiteralPath $Log -SimpleMatch $Marker)) { throw "$Name missing $Marker" }
}

# Runs an editor commandlet and checks its exit code and PASS marker. Returns the log path.
# Tool output goes to the information stream so callers can capture it (*>) without it becoming a return value.
function Invoke-DasCommandlet {
    param([Parameter(Mandatory)]$Das, [Parameter(Mandatory)][string]$Run, [Parameter(Mandatory)][string]$Name,
          [Parameter(Mandatory)][string]$Marker, [string[]]$Arguments = @(), [string]$FailPattern,
          [switch]$NoZen, [switch]$FullStdOut, [switch]$Rendering)
    $Log = Join-Path $Das.Logs "$Name.log"
    $All = @($Das.Project, "-run=$Run") + $Arguments + @('-unattended', '-nosplash', '-stdout')
    if ($Rendering) { $All += @('-AllowCommandletRendering', '-AllowCommandletAudio', '-RenderOffscreen') }
    else { $All += '-NullRHI' }
    if ($FullStdOut) { $All += '-FullStdOutLogOutput' }
    $All += "-abslog=$Log"
    if ($NoZen) { $All += @('-ddc=NoZenLocalFallback', '-NoZenAutoLaunch') }
    & $Das.Editor @All | ForEach-Object { Write-Host $_ }
    if ($LASTEXITCODE) { throw "$Name failed (exit $LASTEXITCODE)" }
    Assert-DasLog -Log $Log -Marker $Marker -FailPattern $FailPattern -Name $Name
    return $Log
}

# Runs the packaged smoke game with a map and switch and checks its marker. Returns the log path.
function Invoke-DasPackaged {
    param([Parameter(Mandatory)]$Das, [Parameter(Mandatory)][string]$Name, [Parameter(Mandatory)][string]$Marker,
          [string[]]$Arguments = @(), [string]$FailPattern)
    $Log = Join-Path $Das.Logs "$Name.log"
    $Process = Start-Process -FilePath $Das.Packaged @DasWindowStyle -PassThru -Wait `
        -ArgumentList ($Arguments + @('-nullrhi', '-unattended', '-nosplash', "-abslog=$Log"))
    if ($Process.ExitCode) { throw "$Name failed (exit $($Process.ExitCode))" }
    Assert-DasLog -Log $Log -Marker $Marker -FailPattern $FailPattern -Name $Name
    return $Log
}

# Hidden windows on Windows; the parameter does not exist on other platforms (used by script self-tests).
$DasWindowStyle = if ($PSVersionTable.PSEdition -eq 'Desktop' -or $IsWindows) { @{ WindowStyle = 'Hidden' } } else { @{} }

# Engine errors that fail editor probes.
$DasEngineErrorPattern = ':\s+Error:|Fatal error:'

# Create/Reopen/... phases of one asset probe commandlet with a shared probe id; returns the id.
function Invoke-DasAssetPhases {
    param([Parameter(Mandatory)]$Das, [Parameter(Mandatory)][string]$Run, [Parameter(Mandatory)][string]$Name,
          [Parameter(Mandatory)][string]$Marker, [string[]]$Phases = @('Create', 'Reopen'), [string]$FailPattern,
          [string[]]$Arguments = @(), [switch]$NoZen)
    $Id = [Guid]::NewGuid().ToString('N')
    foreach ($Phase in $Phases) {
        Invoke-DasCommandlet $Das -Run $Run -Name "$Name-$Phase" -Marker $Marker -FailPattern $FailPattern `
            -Arguments (@("-ProbeId=$Id", "-Phase=$Phase") + $Arguments) -NoZen:$NoZen | Out-Null
    }
    return $Id
}

# Runs Unreal Automation tests in the editor and requires exactly Count (with -AtLeast: at least Count)
# successes without warnings.
function Invoke-DasAutomation {
    param([Parameter(Mandatory)]$Das, [Parameter(Mandatory)][string]$Test, [Parameter(Mandatory)][string]$Name,
          [string]$Marker, [int]$Count = 1, [string[]]$Extra = @(), [switch]$AtLeast)
    $Log = Join-Path $Das.Logs "$Name.log"
    $Report = Join-Path $Das.Logs "$Name-report"
    $Arguments = @(('"' + $Das.Project + '"'), '-unattended', '-NullRHI', '-nosplash', '-nosound', '-RenderOffscreen',
        ('-ExecCmds="Automation RunTests ' + $Test + '"'), '-TestExit="Automation Test Queue Empty"',
        ('-ReportExportPath="' + $Report + '"'), ('-abslog="' + $Log + '"')) + $Extra
    $Process = Start-Process -FilePath $Das.Editor -ArgumentList $Arguments @DasWindowStyle -PassThru -Wait
    if ($Process.ExitCode) { throw "$Name editor exited with $($Process.ExitCode)" }
    if ($Marker) { Assert-DasLog -Log $Log -Marker $Marker -Name $Name }
    $Result = Get-Content -LiteralPath (Join-Path $Report 'index.json') -Raw | ConvertFrom-Json
    Write-Host ($Result | Select-Object succeeded, succeededWithWarnings, failed, notRun | ConvertTo-Json -Compress)
    $CountOk = if ($AtLeast) { $Result.succeeded -ge $Count } else { $Result.succeeded -eq $Count }
    if ($Result.failed -ne 0 -or $Result.notRun -ne 0 -or !$CountOk -or $Result.succeededWithWarnings -ne 0) {
        $Expected = if ($AtLeast) { "at least $Count" } else { "exactly $Count" }
        throw "$Name`: Unreal Automation did not report $Expected successful test(s) without warnings"
    }
}

Export-ModuleMember -Function Get-DasContext, Invoke-DasBuild, Assert-DasLog, Invoke-DasCommandlet, Invoke-DasAutomation,
    Invoke-DasPackaged, Invoke-DasAssetPhases -Variable DasEngineErrorPattern
