param([string]$Engine = 'C:\src\unreal\UnrealEngine')
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference = 'Stop'
$Repo = Split-Path -Parent $PSScriptRoot
$Project = Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs = Join-Path $Repo 'logs'
New-Item -ItemType Directory -Path $Logs -Force | Out-Null
Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=12', "-Log=$Logs\build-reload.log")
if ($LASTEXITCODE) { throw 'Reload probe build failed' }
$Editor = Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
$Manifest = Join-Path $Logs 'reload-root.txt'
$CreateLog = Join-Path $Logs 'reload-create.log'
& $Editor $Project -run=DasReload "-Manifest=$Manifest" -unattended -NullRHI -nosplash -stdout -FullStdOutLogOutput "-abslog=$CreateLog"
if ($LASTEXITCODE) { throw 'Structural reload probe failed' }
if (!(Test-DasResult -LiteralPath $CreateLog -SimpleMatch 'DAS_RELOAD_RESULT=PASS failures=0 reopen=0')) {
    throw 'Create process did not report PASS'
}
$ProbeRoot = (Get-Content -LiteralPath $Manifest -Raw).Trim()
if ($ProbeRoot -notmatch '^/Game/DasReloadProbe/Run_[A-Fa-f0-9]{32}$') { throw 'Invalid probe manifest' }
$ReopenLog = Join-Path $Logs 'reload-reopen.log'
& $Editor $Project -run=DasReload "-Reopen=$ProbeRoot" -unattended -NullRHI -nosplash -stdout -FullStdOutLogOutput "-abslog=$ReopenLog"
if ($LASTEXITCODE) { throw 'Reloaded assets failed validation in the second process' }
if (!(Test-DasResult -LiteralPath $ReopenLog -SimpleMatch 'DAS_RELOAD_RESULT=PASS failures=0 reopen=1')) {
    throw 'Reopen process did not report PASS'
}
Write-Output "Structural reload and fresh-process reload passed: $ProbeRoot"
