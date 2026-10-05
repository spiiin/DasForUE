param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
$Repo = Split-Path -Parent $PSScriptRoot
$Project = Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs = Join-Path $Repo 'logs'
$Archive = Join-Path $Repo 'Build\Packaged'
New-Item -ItemType Directory -Path $Logs -Force | Out-Null
& (Join-Path $Engine 'Engine\Build\BatchFiles\RunUAT.bat') BuildCookRun "-project=$Project" `
    -noP4 -platform=Win64 -clientconfig=Development -build -cook -stage -pak -archive `
    "-archivedirectory=$Archive" -map=/Engine/Maps/Entry -unattended -utf8output -nullrhi `
    -nocompileeditor -skipbuildeditor -nodebuginfo '-UbtArgs=-NoXGE -MaxParallelActions=12'
if ($LASTEXITCODE) { throw 'Packaging failed' }
$Executable = Join-Path $Archive 'Windows\DasForUESmoke\Binaries\Win64\DasForUESmoke.exe'
if (!(Test-Path -LiteralPath $Executable)) { throw "Packaged executable missing: $Executable" }
$Log = Join-Path $Logs 'smoke-packaged.log'
$Process = Start-Process -FilePath $Executable -ArgumentList '-DasSmoke','-nullrhi','-unattended','-nosplash',"-abslog=$Log" `
    -WindowStyle Hidden -PassThru -Wait
if ($Process.ExitCode) { throw "Packaged smoke failed: $($Process.ExitCode)" }
if (!(Select-String -LiteralPath $Log -SimpleMatch 'DAS_FOUNDATION_RESULT=PASS')) {
    throw 'Packaged game exited without a PASS marker'
}
