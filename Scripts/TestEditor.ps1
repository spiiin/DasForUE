param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
$Repo = Split-Path -Parent $PSScriptRoot
$Project = Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs = Join-Path $Repo 'logs'
New-Item -ItemType Directory -Path $Logs -Force | Out-Null
& (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development `
    "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12 "-Log=$Logs\build-editor.log"
if ($LASTEXITCODE) { throw 'Editor build failed' }
& (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasSmoke `
    -unattended -NullRHI -nosplash -stdout -FullStdOutLogOutput "-abslog=$Logs\smoke-editor.log"
if ($LASTEXITCODE) { throw 'Editor smoke test failed' }
if (!(Select-String -LiteralPath "$Logs\smoke-editor.log" -SimpleMatch 'DAS_FOUNDATION_RESULT=PASS')) {
    throw 'Smoke test exited without a PASS marker'
}
