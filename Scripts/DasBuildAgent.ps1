<#
Local build agent for remote-assisted development.

Run it once in a PowerShell window from the repository root:
    powershell -ExecutionPolicy Bypass -File Scripts\DasBuildAgent.ps1

All Unreal builds and test runs on this machine should go through one agent: every editor build
rewrites the shared engine's module BuildId, so a build from another checkout in the middle of a test
run makes that run fail to load its modules. The agent runs one job at a time.

Requests are single lines in logs\agent\queue\<anything>.txt (processed oldest first) or in
logs\agent\request.txt:
    <job> [<id>] [<suites>] [repo=<worktree path>]
    test-quick q1
    test-suite s1 foundation,method-values
    test-suite ui1 automation-quick repo=C:\src\DasForUE2-ui
<job> is one of the whitelisted jobs below; <suites> are names from Tests\suites.json (test-suite
always builds first); repo= selects a git worktree of this repository (default: this checkout).
Arbitrary commands are never executed. Output goes to logs\agent\<id>.log of this checkout, and
logs\agent\<id>.done contains the exit code. Stop the agent with Ctrl+C.
#>
param(
    [string]$Engine = 'C:\src\unreal\UnrealEngine',
    [int]$PollSeconds = 3
)
$ErrorActionPreference = 'Continue'
$Repo = Split-Path -Parent $PSScriptRoot
$Agent = Join-Path $Repo 'logs\agent'
$Queue = Join-Path $Agent 'queue'
New-Item -ItemType Directory -Path $Queue -Force | Out-Null
$Request = Join-Path $Agent 'request.txt'

# Jobs see $R (repository root of the request) and $P (its smoke project).
$Jobs = @{
    'build-editor'  = { & (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development "-Project=$P" -WaitMutex -NoXGE -MaxParallelActions=12 }
    'test-editor'   = { & (Join-Path $R 'Scripts\TestEditor.ps1') -Engine $Engine }
    'test-packaged' = { & (Join-Path $R 'Scripts\TestPackaged.ps1') -Engine $Engine }
    'test-combined' = { & (Join-Path $R 'Scripts\TestCombined.ps1') -Engine $Engine }
    'test-quick'    = { & (Join-Path $R 'Scripts\RunDasTests.ps1') -Engine $Engine -Preset quick }
    'test-suite'    = { param($Suites) & (Join-Path $R 'Scripts\RunDasTests.ps1') -Engine $Engine -Suite "build,$Suites" }
    'check-format'  = { & 'C:\Python312\python.exe' (Join-Path $R 'Scripts\CheckFormat.py') }
    'check-format-fix' = { & 'C:\Python312\python.exe' (Join-Path $R 'Scripts\CheckFormat.py') --fix }
    'check-bindings' = { & (Join-Path $R 'Scripts\GenerateBindings.ps1') -Engine $Engine -Check }
}

# A repo= path is accepted only if it is this checkout or one of its git worktrees.
function Resolve-DasWorktree([string]$Path) {
    if (!$Path) { return $Repo }
    $Full = [IO.Path]::GetFullPath($Path).TrimEnd('\', '/')
    if ($Full -eq $Repo.TrimEnd('\', '/')) { return $Repo }
    $GitFile = Join-Path $Full '.git'
    if (!(Test-Path -LiteralPath $GitFile -PathType Leaf)) { throw "repo=$Path is not a git worktree" }
    $Line = (Get-Content -LiteralPath $GitFile -TotalCount 1).Trim()
    if ($Line -notmatch '^gitdir:\s*(.+)$') { throw "repo=$Path has an unexpected .git file" }
    $GitDir = [IO.Path]::GetFullPath($Matches[1].Trim())
    $Worktrees = [IO.Path]::GetFullPath([IO.Path]::Combine($Repo, '.git', 'worktrees')) + [IO.Path]::DirectorySeparatorChar
    if (!$GitDir.StartsWith($Worktrees, [StringComparison]::OrdinalIgnoreCase)) {
        throw "repo=$Path is not a worktree of $Repo"
    }
    return $Full
}

function Get-DasNextRequest {
    $Next = Get-ChildItem -LiteralPath $Queue -Filter '*.txt' -File | Sort-Object LastWriteTime, Name | Select-Object -First 1
    $File = if ($Next) { $Next.FullName } elseif (Test-Path -LiteralPath $Request) { $Request } else { $null }
    if (!$File) { return $null }
    $Line = (Get-Content -LiteralPath $File -Raw)
    Remove-Item -LiteralPath $File -Force
    return "$Line".Trim()
}

Write-Host "DasBuildAgent watching $Queue and $Request (jobs: $($Jobs.Keys -join ', '))"
while ($true) {
    $line = Get-DasNextRequest
    if ($line) {
        $parts = @($line -split '\s+' | Where-Object { $_ })
        $job = $parts[0]
        $repoArg = ($parts | Where-Object { $_ -like 'repo=*' } | Select-Object -First 1)
        $rest = @($parts | Select-Object -Skip 1 | Where-Object { $_ -notlike 'repo=*' })
        $id = if ($rest.Count -gt 0 -and $rest[0] -match '^[A-Za-z0-9_-]+$') { $rest[0] } else { Get-Date -Format 'yyyyMMdd-HHmmss' }
        # Optional suite names for test-suite (validated against Tests/suites.json by the runner).
        $arg = if ($rest.Count -gt 1 -and $rest[1] -match '^[a-z0-9,-]+$') { $rest[1] } else { '' }
        $log = Join-Path $Agent "$id.log"
        $done = Join-Path $Agent "$id.done"
        if (-not $Jobs.ContainsKey($job)) {
            "Unknown job '$job'. Allowed: $($Jobs.Keys -join ', ')" | Set-Content -LiteralPath $log
            '2' | Set-Content -LiteralPath $done
            continue
        }
        try { $R = Resolve-DasWorktree ($(if ($repoArg) { $repoArg.Substring(5) } else { '' })) }
        catch {
            "$_" | Set-Content -LiteralPath $log
            '2' | Set-Content -LiteralPath $done
            continue
        }
        $P = Join-Path $R 'Tests\DasForUESmoke\DasForUESmoke.uproject'
        Write-Host "$(Get-Date -Format T) running $job ($id) in $R"
        $global:LASTEXITCODE = 0
        $code = 0
        try {
            "DAS_AGENT_JOB=$job id=$id repo=$R" | Tee-Object -FilePath $log | Out-Null
            & $Jobs[$job] $arg *>&1 | ForEach-Object { "$_" } | Tee-Object -FilePath $log -Append | Out-Null
            if ($LASTEXITCODE) { $code = $LASTEXITCODE }
        } catch {
            "EXCEPTION: $_" | Add-Content -LiteralPath $log
            $code = 1
        }
        "$code" | Set-Content -LiteralPath $done
        Write-Host "$(Get-Date -Format T) finished $job ($id) exit=$code"
    }
    Start-Sleep -Seconds $PollSeconds
}
