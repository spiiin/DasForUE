param([string]$Engine = 'C:\src\unreal\UnrealEngine')
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference = 'Stop'
$Repo = Split-Path -Parent $PSScriptRoot
$Project = Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs = Join-Path $Repo 'logs'
New-Item -ItemType Directory -Path $Logs -Force | Out-Null
Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=12', "-Log=$Logs\actor-spawn-pie-build.log")
if ($LASTEXITCODE) { throw 'Native components test build failed' }
$Log = Join-Path $Logs 'actor-spawn-pie.log'
$Report = Join-Path $Logs 'actor-spawn-pie-report'
$Arguments = @(('"' + $Project + '"'), '-stdout', '-FullStdOutLogOutput', '-unattended', '-NullRHI', '-nosplash', '-nosound', '-RenderOffscreen',
    '-ExecCmds="Automation RunTests DasForUE.Actors.SpawnPIE"', '-TestExit="Automation Test Queue Empty"',
    ('-ReportExportPath="' + $Report + '"'), ('-abslog="' + $Log + '"'))
$Process = Start-Process -FilePath (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') `
    -ArgumentList $Arguments -WindowStyle Hidden -PassThru -Wait -RedirectStandardOutput (Join-Path $Logs 'actor-spawn-console.log') -RedirectStandardError (Join-Path $Logs 'actor-spawn-stderr.log')
if ($Process.ExitCode) { throw "Test editor exited with $($Process.ExitCode)" }
$Result = Get-Content -LiteralPath (Join-Path $Report 'index.json') -Raw | ConvertFrom-Json
if ($Result.failed -ne 0 -or $Result.notRun -ne 0 -or $Result.succeeded -ne 1 -or $Result.succeededWithWarnings -ne 0) {
    throw 'Unreal Automation did not report exactly one successful test without warnings'
}
Write-Output 'DAS_ACTOR_SPAWN_PIE=PASS'

