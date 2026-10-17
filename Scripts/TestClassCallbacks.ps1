param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs=Join-Path $Repo 'logs'
New-Item -ItemType Directory -Path $Logs -Force | Out-Null
$Previous=$env:UE_SKIP_UBT_SDK_SETUP
try {
 $env:UE_SKIP_UBT_SDK_SETUP='1'
 if(!$SkipBuild){
  & (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12 "-Log=$Logs\class-callbacks-editor-build.log"
  if($LASTEXITCODE){throw 'Class callbacks build failed'}
 }
 $Editor=Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
 $Foundation=Join-Path $Logs 'class-callbacks-foundation.log'
 & $Editor $Project -run=DasSmoke -unattended -NullRHI -nosplash "-abslog=$Foundation"
 $FoundationSucceeded=($LASTEXITCODE -eq 0 -and [bool](Select-String -LiteralPath $Foundation -SimpleMatch 'DAS_FOUNDATION_RESULT=PASS failures=0'))
 $Log=Join-Path $Logs 'class-callbacks-pie.log'
 $Report=Join-Path $Logs 'class-callbacks-pie-report'
 $Arguments=@(('"'+$Project+'"'),'-unattended','-NullRHI','-nosplash','-nosound','-RenderOffscreen',
  '-ini:Engine:[ConsoleVariables]:HomeScreen.EnableHomeScreen=0',
  '-ExecCmds="Automation RunTests DasForUE.Callbacks.ClassReferencesPIE"',
  '-TestExit="Automation Test Queue Empty"',('-ReportExportPath="'+$Report+'"'),('-abslog="'+$Log+'"'))
 $Process=Start-Process -FilePath $Editor -ArgumentList $Arguments -WindowStyle Hidden -PassThru
 $Process.WaitForExit()
 if($Process.ExitCode){throw 'Class callback PIE process failed'}
 $Result=Get-Content -LiteralPath (Join-Path $Report 'index.json') -Raw | ConvertFrom-Json
 if($Result.succeeded -ne 1 -or $Result.failed -ne 0 -or $Result.succeededWithWarnings -ne 0 -or $Result.notRun -ne 0){throw 'Class callback PIE failed'}
 if(!$FoundationSucceeded){throw 'Foundation failed; see class-callbacks-foundation.log (PIE was checked independently)'}
 Write-Output 'CLASS_CALLBACKS_RESULT=PASS'
} finally { $env:UE_SKIP_UBT_SDK_SETUP=$Previous }
