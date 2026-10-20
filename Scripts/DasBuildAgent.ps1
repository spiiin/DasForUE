<#
Local build agent for remote-assisted development.

Run it once in a PowerShell window from the repository root:
    powershell -ExecutionPolicy Bypass -File Scripts\DasBuildAgent.ps1

It polls logs\agent\request.txt. A request is a single line naming one of the
whitelisted jobs below, optionally followed by a request id and, for test-suite,
comma-separated suite names from Tests\suites.json:
    test-quick q1
    test-suite s1 foundation,method-values
Arbitrary commands are never executed. Output goes to logs\agent\<id>.log, and logs\agent\<id>.done
contains the exit code. Stop the agent with Ctrl+C.
#>
param(
    [string]$Engine = 'C:\src\unreal\UnrealEngine',
    [int]$PollSeconds = 3
)
$ErrorActionPreference = 'Continue'
$Repo = Split-Path -Parent $PSScriptRoot
$Agent = Join-Path $Repo 'logs\agent'
New-Item -ItemType Directory -Path $Agent -Force | Out-Null
$Request = Join-Path $Agent 'request.txt'
$Project = Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'

$Jobs = @{
    'build-editor'  = { & (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12 }
    'test-editor'   = { & (Join-Path $Repo 'Scripts\TestEditor.ps1') -Engine $Engine }
    'test-packaged' = { & (Join-Path $Repo 'Scripts\TestPackaged.ps1') -Engine $Engine }
    'test-combined' = { & (Join-Path $Repo 'Scripts\TestCombined.ps1') -Engine $Engine }
    'test-quick'    = { & (Join-Path $Repo 'Scripts\RunDasTests.ps1') -Engine $Engine -Preset quick }
    'test-suite'    = { param($Suites) & (Join-Path $Repo 'Scripts\RunDasTests.ps1') -Engine $Engine -Suite $Suites }
    'check-format'  = { & 'C:\Python312\python.exe' (Join-Path $Repo 'Scripts\CheckFormat.py') }
    'check-format-fix' = { & 'C:\Python312\python.exe' (Join-Path $Repo 'Scripts\CheckFormat.py') --fix }
    'check-bindings' = { & (Join-Path $Repo 'Scripts\GenerateBindings.ps1') -Engine $Engine -Check }
}

Write-Host "DasBuildAgent watching $Request (jobs: $($Jobs.Keys -join ', '))"
while ($true) {
    if (Test-Path $Request) {
        $line = (Get-Content -LiteralPath $Request -Raw).Trim()
        Remove-Item -LiteralPath $Request -Force
        $parts = $line -split '\s+'
        $job = $parts[0]
        $id = if ($parts.Count -gt 1 -and $parts[1] -match '^[A-Za-z0-9_-]+$') { $parts[1] } else { Get-Date -Format 'yyyyMMdd-HHmmss' }
        # Optional third word: suite names for test-suite (validated against Tests/suites.json by the runner).
        $arg = if ($parts.Count -gt 2 -and $parts[2] -match '^[a-z0-9,-]+$') { $parts[2] } else { '' }
        $log = Join-Path $Agent "$id.log"
        $done = Join-Path $Agent "$id.done"
        if (-not $Jobs.ContainsKey($job)) {
            "Unknown job '$job'. Allowed: $($Jobs.Keys -join ', ')" | Set-Content -LiteralPath $log
            '2' | Set-Content -LiteralPath $done
            continue
        }
        Write-Host "$(Get-Date -Format T) running $job ($id)"
        $global:LASTEXITCODE = 0
        $code = 0
        try {
            & $Jobs[$job] $arg *>&1 | ForEach-Object { "$_" } | Tee-Object -FilePath $log | Out-Null
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
