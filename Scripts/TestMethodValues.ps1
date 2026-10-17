param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$Cooked,[switch]$SkipBuild,[switch]$SkipZenStore)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
if(!$SkipBuild){
& (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12
if($LASTEXITCODE){throw 'Method value editor build failed'}
}
$Id=[Guid]::NewGuid().ToString('N')
Set-Content -LiteralPath (Join-Path $Repo 'logs\method-values-probe-id.txt') -Value $Id
$Base='/Game/DasMethodValues/'+$Id
foreach($Phase in @('Create','Reopen')){
 $Log=Join-Path $Repo ("logs\method-values-$Phase.log")
 & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasMethodValues "-ProbeId=$Id" "-Phase=$Phase" -unattended -NullRHI -nosplash -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$Log"
 if($LASTEXITCODE -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_METHOD_VALUES_RESULT=PASS')){throw "Method values $Phase failed"}
}
$Log=Join-Path $Repo 'logs\method-values-pie.log'
$Report=Join-Path $Repo 'logs\method-values-pie-report'
$Args=@(('"'+$Project+'"'),'-unattended','-NullRHI','-nosplash','-nosound','-ddc=NoZenLocalFallback','-NoZenAutoLaunch','-RenderOffscreen',"-DasMethodValuesProbeId=$Id",'-ExecCmds="Automation RunTests DasForUE.MethodValues.PIE"','-TestExit="Automation Test Queue Empty"',('-ReportExportPath="'+$Report+'"'),('-abslog="'+$Log+'"'))
$Editor=Start-Process -FilePath (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') -ArgumentList $Args -WindowStyle Hidden -PassThru -Wait
if($Editor.ExitCode -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_METHOD_VALUES_PIE_RESULT=PASS')){throw 'Natural method PIE failed'}
$Result=Get-Content -LiteralPath (Join-Path $Report 'index.json') -Raw | ConvertFrom-Json
if($Result.succeeded -ne 1 -or $Result.succeededWithWarnings -ne 0 -or $Result.failed -ne 0 -or $Result.notRun -ne 0){throw 'Natural method automation report failed'}
if($Cooked){
 # The generated source list must be re-evaluated, including the new GUID files.
 $Cache=Join-Path $Repo 'Tests\DasForUESmoke\Intermediate\Build\Win64\x64\DasForUESmoke\Development\Makefile.bin'
 if(Test-Path -LiteralPath $Cache){Remove-Item -LiteralPath $Cache}

 & (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmoke Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12 -NoUBTMakefiles -ForceRulesCompile
 if($LASTEXITCODE){throw 'Method value game build failed'}
 $Receipt=Get-Content -LiteralPath (Join-Path $Repo 'Tests\DasForUESmoke\Binaries\Win64\DasForUESmoke.target') -Raw | ConvertFrom-Json
 foreach($Suffix in @('_target.das','_driver.das','_refs.das')){if(!($Receipt.RuntimeDependencies | Where-Object { $_.Path.EndsWith('dsl_registered_methodvalues_'+$Id+$Suffix) })){throw 'Generated method source missing from fresh game receipt'}}
 $Archive=Join-Path $Repo 'Build\MethodValuesPackaged'
 $CookerArgs=@();if($SkipZenStore){$CookerArgs=@('-AdditionalCookerOptions=-SkipZenStore')}
 & (Join-Path $Engine 'Engine\Build\BatchFiles\RunUAT.bat') -WaitForUATMutex BuildCookRun "-project=$Project" -noP4 -platform=Win64 -clientconfig=Development -cook -stage -pak -archive "-archivedirectory=$Archive" "-map=$Base/Map" -unattended -utf8output -nullrhi -nocompileeditor -skipbuildeditor -nodebuginfo @CookerArgs
 if($LASTEXITCODE){throw 'Method value cook failed'}
 $Log=Join-Path $Repo 'logs\method-values-cooked.log'
 $Process=Start-Process -FilePath (Join-Path $Archive 'Windows\DasForUESmoke\Binaries\Win64\DasForUESmoke.exe') -ArgumentList @("$Base/Map",'-DasMethodValues','-nullrhi','-unattended','-nosplash',"-abslog=$Log") -WindowStyle Hidden -PassThru -Wait
 if($Process.ExitCode -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_METHOD_VALUES_RESULT=PASS')){throw 'Packaged method values failed'}
}
Write-Output "DAS_METHOD_VALUES_PROBE=PASS cooked=$Cooked"
