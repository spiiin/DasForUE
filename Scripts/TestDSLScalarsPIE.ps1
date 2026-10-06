param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
$Repo = Split-Path -Parent $PSScriptRoot
$Project = Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs = Join-Path $Repo 'logs'
New-Item -ItemType Directory -Path $Logs -Force | Out-Null
& (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development `
    "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12 "-Log=$Logs\build-pie.log"
if ($LASTEXITCODE) { throw 'PIE probe build failed' }
$Log = Join-Path $Logs 'dsl-scalars-pie.log'
$Report = Join-Path $Logs 'dsl-scalars-pie-report'
$Arguments = @(('"' + $Project + '"'), '-unattended', '-NullRHI', '-nosplash', '-nosound', '-RenderOffscreen',
    '-ExecCmds="Automation RunTests DasForUE.DSL.ScalarsPIE"', '-TestExit="Automation Test Queue Empty"',
    ('-ReportExportPath="' + $Report + '"'), ('-abslog="' + $Log + '"'))
$Process = Start-Process -FilePath (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') `
    -ArgumentList $Arguments -WindowStyle Hidden -PassThru -Wait
if ($Process.ExitCode) { throw "PIE editor exited with $($Process.ExitCode)" }
if (!(Select-String -LiteralPath $Log -SimpleMatch 'DAS_SCALAR_PIE_RESULT=PASS')) { throw 'PIE probe did not report PASS' }
$Result = Get-Content -LiteralPath (Join-Path $Report 'index.json') -Raw | ConvertFrom-Json
if ($Result.failed -ne 0 -or $Result.notRun -ne 0 -or $Result.succeeded -ne 1 -or $Result.succeededWithWarnings -ne 0) {
    throw 'Unreal Automation did not report exactly one successful test without warnings'
}
Write-Output 'Typed scalar/void DSL graphs and bool properties passed in PIE.'
