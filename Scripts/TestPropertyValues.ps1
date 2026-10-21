param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs=Join-Path $Repo 'logs'
New-Item -ItemType Directory -Path $Logs -Force | Out-Null
if(!$SkipBuild){
 Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=12', '-NoUBTMakefiles', '-ForceRulesCompile')
 if($LASTEXITCODE){throw 'Property values editor build failed'}
}
$Log=Join-Path $Logs 'property-values-PropertyValues.log'
$Report=Join-Path $Logs 'property-values-PropertyValues-report'
& (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -unattended -NullRHI -nosplash -nosound -RenderOffscreen '-ExecCmds=Automation RunTests DasForUE.Types.PropertyValues' '-TestExit=Automation Test Queue Empty' "-ReportExportPath=$Report" "-abslog=$Log"
if($LASTEXITCODE){throw 'Property values editor process failed'}
$Result=Get-Content -LiteralPath (Join-Path $Report 'index.json') -Raw | ConvertFrom-Json
if($Result.failed -ne 0 -or $Result.notRun -ne 0 -or $Result.succeeded -ne 1 -or $Result.succeededWithWarnings -ne 0 -or !(Test-DasResult -LiteralPath $Log -SimpleMatch 'DAS_PROPERTY_VALUES_RESULT=PASS')){throw 'Property values automation did not pass cleanly'}
Write-Output 'DAS_PROPERTY_VALUES=PASS'
