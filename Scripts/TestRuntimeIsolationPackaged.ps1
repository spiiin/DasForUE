param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
$Root=Join-Path $Das.Repo 'Tests/DasRuntimeIsolation'
$Project=Join-Path $Root 'DasRuntimeIsolation.uproject'
$Das.Project=$Project
Invoke-DasBuild $Das -Name 'isolation editor' -BuildArguments @('DasRuntimeIsolationEditor','Win64','Development',"-Project=$Project",'-WaitMutex','-NoXGE','-MaxParallelActions=12')
Invoke-DasAutomation $Das -Test 'DasForUE.Hosts.ExportIsolation' -Name 'isolation-export' -Count 1
& (Join-Path $PSScriptRoot 'TestRuntimeIsolation.ps1') -Engine $Engine
$Receipt=Get-Content -LiteralPath (Join-Path $Root 'Binaries/Win64/DasRuntimeIsolation.target') -Raw | ConvertFrom-Json
$Files=@($Receipt.RuntimeDependencies | ForEach-Object { $_.Path.Replace('\','/') })
if(!($Files | Where-Object { $_ -like '*/Content/Das/Scripts/isolation_actor.das' })){throw 'Project script missing from game receipt'}
if($Files | Where-Object { $_ -match '/Tests/Runtime/|/Scripts/ue_probe_actor\.das$|/Scripts/dsl_' }){throw 'Test scripts leaked into game receipt'}
$Archive=Join-Path $Das.Repo 'Build/IsolationPackaged'
& (Join-Path $Engine 'Engine/Build/BatchFiles/RunUAT.bat') -WaitForUATMutex BuildCookRun "-project=$Project" -noP4 -platform=Win64 -clientconfig=Development -cook -stage -pak -archive "-archivedirectory=$Archive" -map=/Engine/Maps/Entry "-CookDir=$Root/Content/RuntimeIsolation" -unattended -utf8output -nullrhi -nocompileeditor -skipbuildeditor -nodebuginfo -AdditionalCookerOptions=-SkipZenStore
if($LASTEXITCODE){throw 'Isolation packaging failed'}
$Exe=Join-Path $Archive 'Windows/DasRuntimeIsolation/Binaries/Win64/DasRuntimeIsolation.exe'
$Log=Join-Path $Das.Logs 'isolation-packaged.log'
if(Test-Path -LiteralPath $Log){Remove-Item -LiteralPath $Log}
$Process=Start-Process -FilePath $Exe -WorkingDirectory (Split-Path $Exe) -ArgumentList '/Engine/Maps/Entry','-DasIsolation','-nullrhi','-unattended','-nosplash',"-abslog=$Log" -WindowStyle Hidden -PassThru
if(!$Process.WaitForExit(120000)){Stop-Process -Id $Process.Id -Force;throw 'Isolation game timeout'}
$Process.Refresh()
if($Process.ExitCode){throw "Isolation game failed: $($Process.ExitCode)"}
Assert-DasLog -Log $Log -Marker 'DAS_ISOLATION_PACKAGE=PASS' -FailPattern 'DAS_ISOLATION_PACKAGE=FAIL|Fatal error:'
