<#
Shared helpers for DasForUE2 test scripts.

    Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
    $Das = Get-DasContext -Engine $Engine
    Invoke-DasBuild $Das -Name 'my-feature'
    Invoke-DasCommandlet $Das -Run DasFoo -Name 'foo-create' -Marker 'DAS_FOO_RESULT=PASS' -Arguments @('-Phase=Create')

Every helper throws on failure, so scripts stay a list of steps.
#>

# Windows PowerShell exposes Get-FileHash as a module function, not a snap-in cmdlet.
Import-Module Microsoft.PowerShell.Utility -ErrorAction Stop

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
    param([Parameter(Mandatory)]$Das, [string]$Name = 'editor', [string]$Log, [int]$Parallel = 12,
          [string[]]$BuildArguments, [string[]]$Modules = @(), [switch]$NoEngineChanges)
    $Arguments = @('DasForUESmokeEditor', 'Win64', 'Development', "-Project=$($Das.Project)", '-WaitMutex', '-NoXGE',
                   "-MaxParallelActions=$Parallel")
    if ($Modules) { $Arguments += @($Modules | ForEach-Object { "-Module=$_" }) }
    if ($NoEngineChanges) { $Arguments += '-NoEngineChanges' }
    if ($Log) { $Arguments += "-Log=$Log" }
    if ($PSBoundParameters.ContainsKey("BuildArguments")) { $Arguments = $BuildArguments }
    # Test projects share plugin UHT outputs but select different conditional module directories.
    # Regenerate reflection when switching between clean and test-enabled targets.
    if ($Arguments -notcontains '-ForceHeaderGeneration') { $Arguments += '-ForceHeaderGeneration' }
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
          [string]$Marker, [int]$Count = 1, [string[]]$Extra = @(), [switch]$AtLeast, [switch]$Rendering)
    $Log = Join-Path $Das.Logs "$Name.log"
    $Report = Join-Path $Das.Logs "$Name-report"
    $Arguments = @(('"' + $Das.Project + '"'), '-unattended', '-nosplash', '-RenderOffscreen',
        ('-ExecCmds="Automation RunTests ' + $Test + '"'), '-TestExit="Automation Test Queue Empty"',
        ('-ReportExportPath="' + $Report + '"'), ('-abslog="' + $Log + '"')) + $Extra
    if (!$Rendering) { $Arguments += @('-NullRHI', '-nosound') }
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


# Keep native argument lists intact when a runner needs a game target, module-only build or receipt refresh.
# BuildArguments overrides the default editor arguments rather than appending a second target/project.

# Select-String compatibility for existing boolean gates (including -Quiet and regex matching).
function Test-DasResult {
    param([Parameter(Position=1)][string]$Path, [string]$LiteralPath,
          [Parameter(Position=0)][string]$Pattern, [switch]$SimpleMatch, [switch]$Quiet)
    Select-String @PSBoundParameters
}

# Extract a named PASS record once; callers can retain feature-specific fields from Groups.
function Get-DasResult {
    param([Parameter(Mandatory)][AllowEmptyString()][string]$Text,
          [Parameter(Mandatory)][string]$Pattern, [switch]$All)
    if ($All) { return [regex]::Matches($Text, $Pattern) }
    return [regex]::Match($Text, $Pattern)
}

# A checked result record, optionally with an exact check count. Return the record for extra fields.
function Assert-DasResult {
    param([Parameter(Mandatory)][string]$Log, [Parameter(Mandatory)][string]$Pattern,
          [int]$Checks = -1, [string]$Name = $Log)
    $Result = Get-DasResult -Text (Get-Content -LiteralPath $Log -Raw) -Pattern $Pattern
    if (!$Result.Success -or ($Checks -ge 0 -and [int]$Result.Groups[1].Value -ne $Checks)) {
        throw "$Name missing or incomplete result: $Log"
    }
    return $Result
}

# Deterministic source/binary freeze; file discovery stays with the runner so its scope is unchanged.
function Lock-SourceHashes {
    param([Parameter(Mandatory)][string[]]$Paths)
    $Result = [ordered]@{}
    foreach ($Path in ($Paths | Sort-Object)) {
        $Result[$Path] = (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash
    }
    return ($Result | ConvertTo-Json -Depth 8 -Compress)
}

function Write-DasValidation {
    param([Parameter(Mandatory)][string]$Path, [Parameter(Mandatory)]$Data, [int]$Depth = 20)
    $Data | ConvertTo-Json -Depth $Depth | Set-Content -LiteralPath $Path -Encoding utf8
}


# Common freeze scope used by narrow DSL commandlet runners; Extra retains each runner's fixtures.
function Get-DasValidationPaths {
    param([Parameter(Mandatory)]$Das, [string[]]$Extra = @())
    Get-ChildItem "$($Das.Repo)/Plugins/DasForUE/Source" -Recurse -File |
        Where-Object Extension -in '.h','.cpp','.inl','.cs' | ForEach-Object FullName
    Get-ChildItem "$($Das.Repo)/Plugins/DasForUE/Content/Das/Scripts" -File -Filter '*.das' |
        ForEach-Object FullName
    'DasRuntime','DasEditor' | ForEach-Object {
        "$($Das.Repo)/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$_.dll"
    }
    "$($Das.Engine)/Engine/Binaries/Win64/UnrealEditor.modules"
    Join-Path $PSScriptRoot 'DasTest.psm1'
    $Extra | ForEach-Object { "$($Das.Repo)/$_" }
}

# One scoped commandlet receipt. Re-enumerate paths after the probe to catch additions/removals too.
function Invoke-DasValidation {
    param([Parameter(Mandatory)]$Das, [Parameter(Mandatory)][string]$Run,
          [Parameter(Mandatory)][string]$Name, [Parameter(Mandatory)][string]$Marker,
          [Parameter(Mandatory)][int]$Checks, [Parameter(Mandatory)][scriptblock]$SourcePaths,
          [Parameter(Mandatory)][string]$Output, [Parameter(Mandatory)][string]$Mode,
          [Parameter(Mandatory)][string]$SuccessMarker, [string]$FailPattern,
          [string[]]$Arguments = @('-Multiprocess'), [string]$RequiredPattern)
    $Before = Lock-SourceHashes -Paths @(& $SourcePaths)
    $Log = Invoke-DasCommandlet $Das -Run $Run -Name $Name -Marker $Marker -NoZen `
        -Arguments $Arguments -FailPattern $FailPattern
    $Record = Assert-DasResult -Log $Log -Pattern ([regex]::Escape($Marker) + ' checks=(\d+)') -Checks $Checks
    if ($RequiredPattern -and (Get-Content -LiteralPath $Log -Raw) -notmatch $RequiredPattern) {
        throw "Missing required diagnostic $RequiredPattern in $Log"
    }
    if ((Lock-SourceHashes -Paths @(& $SourcePaths)) -cne $Before) { throw "Source or binary changed during $Name" }
    Write-DasValidation -Path (Join-Path $Das.Logs $Output) -Depth 8 -Data ([ordered]@{
        state='PASS'; checks=[int]$Record.Groups[1].Value; runtime_log=$Log; sha256=($Before|ConvertFrom-Json)
        mode=$Mode; PIE='not_run'; packaged='not_run'; full_suites='not_run'
    })
    Write-Output "$SuccessMarker checks=$Checks"
}

# Removes what earlier runs left behind: per-run asset folders (a GUID or hex id, PIE_<guid>) under
# Content/Das*/ of the smoke project and the generated method-values / savegame scripts. Keeps the ids that
# logs/*-probe-id.txt names (suites run with -NoDependencies reopen them) and the newest entry of each root (to
# look at after a failure); deletes only what git ignores. -DryRun lists the candidates instead.
function Clear-DasRunArtifacts {
    param($Das, [switch]$DryRun)
    $Id = '^(PIE_)?[0-9A-Fa-f]{12,32}$'
    $Keep = @{}
    foreach ($F in @(Get-ChildItem -LiteralPath $Das.Logs -Filter '*-probe-id.txt' -File -ErrorAction SilentlyContinue)) {
        $Text = (Get-Content -LiteralPath $F.FullName -Raw)
        if ($Text) { $Keep[$Text.Trim().ToLowerInvariant()] = $true }
    }
    # Groups of entries of one run: the newest group of a root stays.
    $Groups = @()
    $Content = Join-Path $Das.Repo 'Tests\DasForUESmoke\Content'
    foreach ($Root in @(Get-ChildItem -LiteralPath $Content -Directory -Filter 'Das*')) {
        $Runs = @(Get-ChildItem -LiteralPath $Root.FullName |
            Where-Object { [IO.Path]::GetFileNameWithoutExtension($_.Name) -match $Id })
        foreach ($R in $Runs) {
            $Groups += [pscustomobject]@{ Root = $Root.FullName; Key = [IO.Path]::GetFileNameWithoutExtension($R.Name)
                                          Items = @($R); Time = $R.LastWriteTimeUtc }
        }
    }
    $Scripts = Join-Path $Das.Repo 'Plugins\DasForUE\Content\Das\Scripts'
    $Generated = @(Get-ChildItem -LiteralPath $Scripts -File -Filter '*.das' |
        Where-Object { $_.Name -match '^(dsl_registered_methodvalues|savegame_compatibility)_([0-9A-Fa-f]{32})' })
    foreach ($G in ($Generated | Group-Object { $_.Name -replace '^(.+?_)([0-9A-Fa-f]{32}).*$', '$1$2' })) {
        $Groups += [pscustomobject]@{ Root = $Scripts + ':' + ($G.Name -replace '_[0-9A-Fa-f]{32}$', '')
                                      Key = ($G.Name -replace '^.*_', ''); Items = @($G.Group)
                                      Time = ($G.Group | Measure-Object LastWriteTimeUtc -Maximum).Maximum }
    }
    $Remove = @(); $Kept = 0
    foreach ($Root in ($Groups | Group-Object Root)) {
        $Newest = ($Root.Group | Sort-Object Time -Descending | Select-Object -First 1).Key
        foreach ($G in $Root.Group) {
            if ($G.Key -eq $Newest -or $Keep.ContainsKey(($G.Key -replace '^PIE_', '').ToLowerInvariant())) { ++$Kept }
            else { $Remove += $G.Items }
        }
    }
    # Only paths git ignores: a tracked file never matches, whatever its name.
    $Ignored = @{}
    if ($Remove) {
        $Previous = $ErrorActionPreference; $ErrorActionPreference = 'Continue'
        try {
            $Relative = @($Remove | ForEach-Object { $_.FullName.Substring($Das.Repo.Length + 1).Replace('\', '/') })
            # Paths as arguments, in batches under the command-line limit: Windows PowerShell pipes CRLF into
            # --stdin, and git then matches nothing.
            for ($I = 0; $I -lt $Relative.Count; $I += 100) {
                $Batch = $Relative[$I..([Math]::Min($I + 99, $Relative.Count - 1))]
                foreach ($Line in @(& git -C $Das.Repo check-ignore -- @Batch 2>$null)) { $Ignored[$Line] = $true }
            }
        } finally { $ErrorActionPreference = $Previous }
    }
    $Removed = 0
    foreach ($Item in $Remove) {
        if (!$Ignored.ContainsKey($Item.FullName.Substring($Das.Repo.Length + 1).Replace('\', '/'))) { ++$Kept; continue }
        if ($DryRun) { Write-Output "DAS_CLEANUP_CANDIDATE $($Item.FullName)" }
        else { Remove-Item -LiteralPath $Item.FullName -Recurse -Force }
        ++$Removed
    }
    Write-Output "DAS_CLEANUP removed=$Removed kept=$Kept"
}

# Windows PowerShell turns native stderr into ErrorRecord objects under Stop. unittest writes its
# normal progress/report there; only its exit code decides success. Restore the caller's policy.
function Invoke-DasPythonTests {
    param([Parameter(Mandatory)][string[]]$Path, [string]$Python='C:/Python312/python.exe', [string]$Log)
    foreach ($TestPath in $Path) {
        $Previous = $ErrorActionPreference
        $ErrorActionPreference = 'Continue'
        try {
            if ($Log) { & $Python $TestPath *> $Log }
            else { & $Python $TestPath 2>&1 | ForEach-Object { "$_" } }
            $Code = $LASTEXITCODE
        } finally { $ErrorActionPreference = $Previous }
        if ($Code) { throw "Python tests failed ($Code): $TestPath" }
    }
}

Export-ModuleMember -Function Get-DasContext, Invoke-DasBuild, Assert-DasLog, Invoke-DasCommandlet, Invoke-DasAutomation,
    Invoke-DasPackaged, Invoke-DasAssetPhases, Test-DasResult, Get-DasResult, Assert-DasResult,
    Lock-SourceHashes, Write-DasValidation, Get-DasValidationPaths, Invoke-DasValidation, Clear-DasRunArtifacts, Invoke-DasPythonTests -Variable DasEngineErrorPattern
