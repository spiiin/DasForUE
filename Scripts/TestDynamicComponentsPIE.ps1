param([string]$Engine = 'C:\src\unreal\UnrealEngine')
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference = 'Stop'
$Repo = Split-Path -Parent $PSScriptRoot
$Project = Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs = Join-Path $Repo 'logs'
New-Item -ItemType Directory -Path $Logs -Force | Out-Null
Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=12', "-Log=$Logs\dynamic-components-pie-build.log")
if ($LASTEXITCODE) { throw 'Native components test build failed' }
$Log = Join-Path $Logs 'dynamic-components-pie.log'
$Report = Join-Path $Logs 'dynamic-components-pie-report'
$Arguments = @(('"' + $Project + '"'), '-unattended', '-NullRHI', '-nosplash', '-nosound', '-RenderOffscreen',
    '-ExecCmds="Automation RunTests DasForUE.Components.DynamicPIE"', '-TestExit="Automation Test Queue Empty"',
    ('-ReportExportPath="' + $Report + '"'), ('-abslog="' + $Log + '"'))
$Process = Start-Process -FilePath (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') `
    -ArgumentList $Arguments -WindowStyle Hidden -PassThru -Wait
if ($Process.ExitCode) { throw "Test editor exited with $($Process.ExitCode)" }
$Result = Get-Content -LiteralPath (Join-Path $Report 'index.json') -Raw | ConvertFrom-Json
if ($Result.failed -ne 0 -or $Result.notRun -ne 0 -or $Result.succeeded -ne 1 -or $Result.succeededWithWarnings -ne 0) {
    throw 'Unreal Automation did not report exactly one successful test without warnings'
}
# Verify serialized class dependency and PIE in a fresh process.
$ReloadReport = Join-Path $Logs 'class-property-reload-report'
$ReloadLog = Join-Path $Logs 'class-property-reload.log'
$ReloadArgs = @(('"' + $Project + '"'), '-unattended', '-NullRHI', '-nosplash', '-nosound', '-RenderOffscreen',
    '-ExecCmds="Automation RunTests DasForUE.Components.ClassReload"', '-TestExit="Automation Test Queue Empty"',
    ('-ReportExportPath="' + $ReloadReport + '"'), ('-abslog="' + $ReloadLog + '"'))
$ReloadProcess = Start-Process -FilePath (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') -ArgumentList $ReloadArgs -WindowStyle Hidden -PassThru -Wait
if ($ReloadProcess.ExitCode) { throw 'Class-property reload editor failed' }
$ReloadResult = Get-Content -LiteralPath (Join-Path $ReloadReport 'index.json') -Raw | ConvertFrom-Json
if ($ReloadResult.failed -ne 0 -or $ReloadResult.notRun -ne 0 -or $ReloadResult.succeeded -ne 1 -or $ReloadResult.succeededWithWarnings -ne 0) { throw 'Class-property reload test failed' }
# Remove only the two assets created by this run, after both editors have exited.
$Manifest = Join-Path $Repo 'Tests\DasForUESmoke\Saved\DasClassPropertyReload.txt'
$SavedRoot = (Get-Content -LiteralPath $Manifest -Raw).Trim()
if ($SavedRoot -notmatch '^/Game/DasClassPropertyTest/([0-9A-Fa-f]{32})$') { throw 'Unexpected test package root' }
$TestGuid = $Matches[1]
$ExpectedParent = [IO.Path]::GetFullPath((Join-Path $Repo 'Tests\DasForUESmoke\Content\DasClassPropertyTest'))
$AssetDirectory = [IO.Path]::GetFullPath((Join-Path $ExpectedParent $TestGuid))
if ([IO.Path]::GetDirectoryName($AssetDirectory) -ne $ExpectedParent) { throw 'Test cleanup path escaped its directory' }
foreach ($AssetName in @('BP_Owner.uasset','BP_Component.uasset')) {
    $AssetFile = Join-Path $AssetDirectory $AssetName
    if (Test-Path -LiteralPath $AssetFile) { Remove-Item -LiteralPath $AssetFile }
}
if ((Test-Path -LiteralPath $AssetDirectory) -and @(Get-ChildItem -LiteralPath $AssetDirectory -Force).Count -eq 0) { Remove-Item -LiteralPath $AssetDirectory }
Remove-Item -LiteralPath $Manifest
Write-Output 'DAS_DYNAMIC_COMPONENTS_PIE=PASS'
