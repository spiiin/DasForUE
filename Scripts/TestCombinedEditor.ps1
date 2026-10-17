param([string]$Engine='C:\src\unreal\UnrealEngine',[string]$Python='C:\Python312\python.exe',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs=Join-Path $Repo 'logs'
New-Item -ItemType Directory -Path $Logs -Force | Out-Null
$Editor=Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
$PreviousSdkSetup=$env:UE_SKIP_UBT_SDK_SETUP
try {
 $env:UE_SKIP_UBT_SDK_SETUP='1'
 if(!$SkipBuild){
  & (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12 "-Log=$Logs\combined-editor-build.log"
  if($LASTEXITCODE){throw 'Combined Editor build failed'}
 }
 Push-Location $Repo
 try { & $Python -m unittest discover -s Scripts -p 'Test*.py' -v *> (Join-Path $Logs 'combined-python-tests.log');if($LASTEXITCODE){throw 'Generator tests failed'} }
 finally { Pop-Location }
 function Invoke-CombinedAutomation([string]$Filter,[string]$Name,[int]$MinimumCount,[string[]]$Extra=@()) {
  $Log=Join-Path $Logs ($Name+'.log');$Report=Join-Path $Logs ($Name+'-report')
  $Arguments=@(('"'+$Project+'"'),'-unattended','-NullRHI','-nosplash','-nosound','-RenderOffscreen',
   '-ini:Engine:[ConsoleVariables]:HomeScreen.EnableHomeScreen=0',
   ('-ExecCmds="Automation RunTests '+$Filter+'"'),'-TestExit="Automation Test Queue Empty"',
   ('-ReportExportPath="'+$Report+'"'),('-abslog="'+$Log+'"'))+$Extra
  $P=Start-Process -FilePath $Editor -ArgumentList $Arguments -WindowStyle Hidden -PassThru
  $P.WaitForExit()
  if($P.ExitCode){throw "Automation process failed: $Filter ($($P.ExitCode))"}
  $Result=Get-Content -LiteralPath (Join-Path $Report 'index.json') -Raw | ConvertFrom-Json
  Write-Output ($Result | Select-Object succeeded,succeededWithWarnings,failed,notRun | ConvertTo-Json -Compress)
  if($Result.succeeded -lt $MinimumCount -or $Result.failed -ne 0 -or $Result.succeededWithWarnings -ne 0 -or $Result.notRun -ne 0){throw "Automation failed: $Filter"}
 }
 $FoundationLog=Join-Path $Logs 'combined-editor-foundation.log'
 & $Editor $Project -run=DasSmoke -unattended -NullRHI -nosplash -stdout "-abslog=$FoundationLog"
 if($LASTEXITCODE -or !(Select-String -LiteralPath $FoundationLog -SimpleMatch 'DAS_FOUNDATION_RESULT=PASS failures=0')){throw 'Combined Foundation failed'}
 # ClassReload deliberately loads assets produced by DynamicPIE in an earlier
 # process; alphabetical aggregate test ordering alone cannot prepare them.
 Invoke-CombinedAutomation 'DasForUE.Components.DynamicPIE' 'combined-components-prepare' 1
 # MethodValues requires saved generated sources/assets and the probe ID.
 $MethodId=[Guid]::NewGuid().ToString('N')
 foreach($Phase in @('Create','Reopen')){
  $Log=Join-Path $Logs ("combined-method-values-$Phase.log")
  & $Editor $Project -run=DasMethodValues "-ProbeId=$MethodId" "-Phase=$Phase" -unattended -NullRHI -nosplash "-abslog=$Log"
  if($LASTEXITCODE -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_METHOD_VALUES_RESULT=PASS')){throw "MethodValues $Phase failed"}
 }
 Set-Content -LiteralPath (Join-Path $Logs 'combined-method-values-probe-id.txt') -Value $MethodId
 Invoke-CombinedAutomation 'DasForUE' 'combined-all-automation' 35 @("-DasMethodValuesProbeId=$MethodId")
 Write-Output 'DAS_COMBINED_EDITOR=PASS'
} finally { $env:UE_SKIP_UBT_SDK_SETUP=$PreviousSdkSetup }
