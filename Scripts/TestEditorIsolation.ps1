param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
$Root=Join-Path $Das.Repo 'Tests/DasEditorIsolation'
$Project=Join-Path $Root 'DasEditorIsolation.uproject'
Invoke-DasBuild $Das -Name 'clean editor' -BuildArguments @('DasEditorIsolationEditor','Win64','Development',"-Project=$Project",'-WaitMutex','-NoXGE','-MaxParallelActions=12')
$Manifests=@(Get-ChildItem (Join-Path $Root 'Intermediate/Build') -Filter '*.uhtmanifest' -Recurse)
if(!$Manifests){throw 'Missing clean-editor UHT manifest'}
foreach($File in $Manifests){if((Get-Content $File.FullName -Raw) -match 'Tests[\\/]+(Editor|Runtime)[\\/]'){throw "Test reflection leaked: $File"}}
$Responses=@(Get-ChildItem (Join-Path $Root 'Intermediate/Build') -Filter '*.rsp' -Recurse)
if(!$Responses){throw 'Missing clean-editor compiler responses'}
foreach($File in $Responses){if((Get-Content $File.FullName -Raw) -match 'Tests[\\/]+(Editor|Runtime)[\\/]'){throw "Test sources leaked: $File"}}
$Log=Join-Path $Das.Logs 'editor-isolation.log'
if(Test-Path -LiteralPath $Log){Remove-Item -LiteralPath $Log}
$P=Start-Process -FilePath $Das.Editor -ArgumentList $Project,'-DasEditorIsolation','-nullrhi','-unattended','-nosplash',"-abslog=$Log" -WindowStyle Hidden -PassThru
if(!$P.WaitForExit(120000)){Stop-Process -Id $P.Id -Force;throw 'Clean editor startup timeout'}
$P.Refresh()
if($P.ExitCode){throw "Clean editor exited $($P.ExitCode)"}
Assert-DasLog -Log $Log -Marker 'DAS_EDITOR_ISOLATION=PASS' -FailPattern 'DAS_EDITOR_ISOLATION=FAIL|Fatal error:'
