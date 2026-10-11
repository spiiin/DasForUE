param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
$Repo = Split-Path -Parent $PSScriptRoot
$Project = Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs = Join-Path $Repo 'logs'
New-Item -ItemType Directory -Path $Logs -Force | Out-Null
& (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development `
    "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12 "-Log=$Logs\function-libraries-build.log"
if ($LASTEXITCODE) { throw 'Native components test build failed' }
$Stage = Join-Path $Repo 'Build\FunctionLibraries'
New-Item -ItemType Directory -Path $Stage -Force | Out-Null
$IR = Join-Path $Stage 'Api.json'
& (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasExport "-Selection=$Repo\Bindings\FunctionLibraries.json" "-Output=$IR" -unattended -NullRHI -nosplash "-abslog=$Logs\function-libraries-export.log"
if ($LASTEXITCODE) { throw 'Library reflection export failed' }
& 'C:\Python312\python.exe' (Join-Path $Repo 'Scripts\GenerateBindings.py') $IR (Join-Path $Repo 'Plugins\DasForUE\Content\Das\Scripts\ue_library_generated.das') --module ue_library_generated
if ($LASTEXITCODE) { throw 'Library wrapper generation failed' }
$Log = Join-Path $Logs 'function-libraries.log'
$Report = Join-Path $Logs 'function-libraries-report'
$Arguments = @(('"' + $Project + '"'), '-unattended', '-NullRHI', '-nosplash', '-nosound', '-RenderOffscreen',
    '-ExecCmds="Automation RunTests DasForUE.FunctionLibraries.PIE"', '-TestExit="Automation Test Queue Empty"',
    ('-ReportExportPath="' + $Report + '"'), ('-abslog="' + $Log + '"'))
$Process = Start-Process -FilePath (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') `
    -ArgumentList $Arguments -WindowStyle Hidden -PassThru -Wait
if ($Process.ExitCode) { throw "Test editor exited with $($Process.ExitCode)" }
$Result = Get-Content -LiteralPath (Join-Path $Report 'index.json') -Raw | ConvertFrom-Json
if ($Result.failed -ne 0 -or $Result.notRun -ne 0 -or $Result.succeeded -ne 1 -or $Result.succeededWithWarnings -ne 0) {
    throw 'Unreal Automation did not report exactly one successful test without warnings'
}
Write-Output 'DAS_FUNCTION_LIBRARIES=PASS'
