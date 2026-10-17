param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs=Join-Path $Repo 'logs'
New-Item -ItemType Directory -Path $Logs -Force | Out-Null
if(!$SkipBuild){
 & (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12 -NoUBTMakefiles -ForceRulesCompile
 if($LASTEXITCODE){throw 'Pin adapter editor build failed'}
}
$Log=Join-Path $Logs 'type-pins-matrix.log'
$Report=Join-Path $Logs 'type-pins-matrix-report'
& (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -unattended -NullRHI -nosplash -nosound -RenderOffscreen '-ExecCmds=Automation RunTests DasForUE.Types.PinAdapter' '-TestExit=Automation Test Queue Empty' "-ReportExportPath=$Report" "-abslog=$Log"
if($LASTEXITCODE){throw 'Pin adapter editor process failed'}
$Result=Get-Content -LiteralPath (Join-Path $Report 'index.json') -Raw | ConvertFrom-Json
if($Result.failed -ne 0 -or $Result.notRun -ne 0 -or $Result.succeeded -ne 1 -or $Result.succeededWithWarnings -ne 0 -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_TYPE_PINS_RESULT=PASS')){throw 'Pin adapter automation did not pass cleanly'}
Write-Output 'DAS_TYPE_PINS=PASS'
