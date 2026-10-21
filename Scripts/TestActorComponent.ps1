param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$Cooked,[switch]$SkipBuild)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs=Join-Path $Repo 'logs'
New-Item -ItemType Directory -Path $Logs -Force | Out-Null
if(!$SkipBuild){
 Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=12')
 if($LASTEXITCODE){throw 'Actor/Component editor build failed'}
}
$ProbeId=[Guid]::NewGuid().ToString('N')
$Base='/Game/DasActorComponent/'+$ProbeId
foreach($Phase in @('Create','Reopen')){
 $Log=Join-Path $Logs ("actor-component-$Phase.log")
 & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasActorComponent "-ProbeId=$ProbeId" "-Phase=$Phase" -unattended -NullRHI -nosplash -stdout "-abslog=$Log"
 if($LASTEXITCODE -or !(Test-DasResult -LiteralPath $Log -SimpleMatch 'DAS_ACTOR_COMPONENT_RESULT=PASS')){throw "Actor/Component $Phase failed"}
}
if($Cooked){
 Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmoke', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=12', '-NoUBTMakefiles', '-ForceRulesCompile')
 if($LASTEXITCODE){throw 'Actor/Component game build failed'}
 $Receipt=Get-Content -LiteralPath (Join-Path $Repo 'Tests\DasForUESmoke\Binaries\Win64\DasForUESmoke.target') -Raw | ConvertFrom-Json
 if(!($Receipt.RuntimeDependencies | Where-Object { $_.Path.EndsWith('dsl_component_behavior.das') })){throw 'Game receipt omits the authored component script'}
 $Archive=Join-Path $Repo 'Build\ActorComponentPackaged'
 & (Join-Path $Engine 'Engine\Build\BatchFiles\RunUAT.bat') BuildCookRun "-project=$Project" -noP4 -platform=Win64 -clientconfig=Development -cook -stage -pak -archive "-archivedirectory=$Archive" "-map=$Base/Map" -unattended -utf8output -nullrhi -nocompileeditor -skipbuildeditor -nodebuginfo
 if($LASTEXITCODE){throw 'Actor/Component cook failed'}
 $Log=Join-Path $Logs 'actor-component-cooked.log'
 $Process=Start-Process -FilePath (Join-Path $Archive 'Windows\DasForUESmoke\Binaries\Win64\DasForUESmoke.exe') -ArgumentList @("$Base/Map",'-DasActorComponent','-nullrhi','-unattended','-nosplash',"-abslog=$Log") -WindowStyle Hidden -PassThru -Wait
 if($Process.ExitCode -or !(Test-DasResult -LiteralPath $Log -SimpleMatch 'DAS_ACTOR_COMPONENT_RESULT=PASS')){throw 'Actor/Component packaged run failed'}
}
Write-Output "DAS_ACTOR_COMPONENT_PROBE=PASS cooked=$Cooked"
