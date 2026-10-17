param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild,[string]$DDC='')
$ErrorActionPreference='Stop'
$DDCArgs=@();if($DDC){$DDCArgs=@("-DDC=$DDC")}
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs=Join-Path $Repo 'logs'
New-Item -ItemType Directory -Path $Logs -Force | Out-Null
$Previous=$env:UE_SKIP_UBT_SDK_SETUP
try {
 $env:UE_SKIP_UBT_SDK_SETUP='1'
 if(!$SkipBuild){
  & (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12 "-Log=$Logs\soft-references-editor-build.log"
  if($LASTEXITCODE){throw 'Soft references build failed'}
 }
 $Editor=Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
 $Foundation=Join-Path $Logs 'soft-references-foundation.log'
 & $Editor $Project -run=DasSmoke -unattended -NullRHI -nosplash "-abslog=$Foundation" @DDCArgs
 $FoundationSucceeded=($LASTEXITCODE -eq 0 -and [bool](Select-String -LiteralPath $Foundation -SimpleMatch 'DAS_FOUNDATION_RESULT=PASS failures=0'))
 $Log=Join-Path $Logs 'soft-references-pie.log'
 $Report=Join-Path $Logs 'soft-references-pie-report'
 $Arguments=@(('"'+$Project+'"'),'-unattended','-NullRHI','-nosplash','-nosound','-RenderOffscreen',
  '-ini:Engine:[ConsoleVariables]:HomeScreen.EnableHomeScreen=0',
  '-ExecCmds="Automation RunTests DasForUE.SoftReferences.RuntimePIE"',
  '-TestExit="Automation Test Queue Empty"',('-ReportExportPath="'+$Report+'"'),('-abslog="'+$Log+'"'))
 $Arguments+= $DDCArgs
 $PIEStartedUtc=[DateTime]::UtcNow
 $Process=Start-Process -FilePath $Editor -ArgumentList $Arguments -WindowStyle Hidden -PassThru
 $Process.WaitForExit()
 if($Process.ExitCode){throw 'Soft reference PIE process failed'}
 $ReportFile=Join-Path $Report 'index.json'
 if(!(Test-Path -LiteralPath $ReportFile) -or (Get-Item -LiteralPath $ReportFile).LastWriteTimeUtc -lt $PIEStartedUtc){throw 'PIE did not produce a fresh report'}
 $Result=Get-Content -LiteralPath $ReportFile -Raw | ConvertFrom-Json
 if(@($Result.tests).Count -ne 1 -or $Result.tests[0].fullTestPath -ne 'DasForUE.SoftReferences.RuntimePIE'){throw 'Unexpected PIE test report'}
 if($Result.succeeded -ne 1 -or $Result.failed -ne 0 -or $Result.succeededWithWarnings -ne 0 -or $Result.notRun -ne 0){throw 'Soft reference PIE failed'}
 if(!$FoundationSucceeded){throw 'Foundation failed; see soft-references-foundation.log (PIE was checked independently)'}
 Write-Output 'SOFT_REFERENCES_RESULT=PASS'
} finally { $env:UE_SKIP_UBT_SDK_SETUP=$Previous }
