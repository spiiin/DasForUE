param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$Check,[switch]$SkipBuild,[string]$Sources,[string[]]$Entry=@(),[string[]]$ScriptDirectory=@(),[string]$Project,[string]$Target)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$ProjectMode=!!$Project
if(!$Project){$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'}
$Project=[IO.Path]::GetFullPath($Project)
$ProjectRoot=Split-Path -Parent $Project
if(!$Target){$Target=([IO.Path]::GetFileNameWithoutExtension($Project))+'Editor'}
if(!$Sources){$Sources=if($ProjectMode){Join-Path $ProjectRoot 'Bindings/NativeGameplay.sources.json'}else{Join-Path $Repo 'Bindings/NativeGameplay.sources.json'}}
New-Item -ItemType Directory -Path (Join-Path $Repo 'logs') -Force | Out-Null
$OutputRoot=if($ProjectMode){$ProjectRoot}else{$Repo}
$Stage=Join-Path $OutputRoot 'Build/NativeGameplay';New-Item -ItemType Directory -Path $Stage -Force | Out-Null
function Build-NativeRegistry([string]$Phase){
    $BuildLog=Join-Path $Repo ('logs/native-gameplay-'+$Phase+'-'+[guid]::NewGuid().ToString('N')+'.log')
    # A new project header changes ModuleRules; do not reuse the bootstrap makefile.
    $RuleArgs=@();if($ProjectMode -and $Phase -eq 'registry'){$RuleArgs=@('-NoUBTMakefiles')}
    & (Join-Path $Engine 'Engine/Build/BatchFiles/Build.bat') $Target Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12 "-Log=$BuildLog" @RuleArgs
    if($LASTEXITCODE){throw "Native gameplay $Phase build failed"}
}
# Bootstrap a project-specific compiled header after a generator ABI change.
# Its saved IR is sufficient; the fresh exporter below still rebuilds the full contract.
if($ProjectMode -and !$SkipBuild){
 $ExistingIR=Join-Path $ProjectRoot 'Bindings/NativeGameplay.json'
 $ExistingHeader=Join-Path $ProjectRoot 'Source/DasGenerated/DasProjectNativeGameplay.generated.h'
 if((Test-Path -LiteralPath $ExistingIR) -and (Test-Path -LiteralPath $ExistingHeader)){
  $Bootstrap=@((Join-Path $PSScriptRoot 'GenerateNativeGameplay.py'),$ExistingIR,(Join-Path $Stage 'bootstrap.h'),(Join-Path $Stage 'bootstrap.das'),'--scripts',(Join-Path $ProjectRoot 'Content/Das/Scripts'))
  foreach($Directory in $ScriptDirectory){$Bootstrap+=@('--scripts',$Directory)}
  if(Test-Path -LiteralPath $Sources){$SavedConfig=Get-Content -LiteralPath $Sources -Raw|ConvertFrom-Json;foreach($Directory in $SavedConfig.script_dirs){$Bootstrap+=@('--scripts',([IO.Path]::GetFullPath((Join-Path (Split-Path -Parent ([IO.Path]::GetFullPath($Sources))) $Directory))))}}
  & C:\Python312\python.exe @Bootstrap
  if($LASTEXITCODE){throw 'Project native header ABI bootstrap failed'}
  if([IO.File]::ReadAllText((Join-Path $Stage 'bootstrap.h')) -cne [IO.File]::ReadAllText($ExistingHeader)){Copy-Item -LiteralPath (Join-Path $Stage 'bootstrap.h') -Destination $ExistingHeader -Force}
 }
}
if(!$SkipBuild){Build-NativeRegistry 'exporter'}
$Requests=Join-Path $Stage 'NativeGameplay.requests.json'
$DiscoverArgs=@((Join-Path $PSScriptRoot 'DiscoverNativeGameplay.py'),'--config',$Sources,'--output',$Requests)
foreach($Item in $Entry){$DiscoverArgs+=@('--entry',$Item)}
foreach($Item in $ScriptDirectory){$DiscoverArgs+=@('--scripts',$Item)}
& C:\Python312\python.exe @DiscoverArgs
if($LASTEXITCODE){throw 'Native gameplay discovery failed'}
$IR=Join-Path $Stage 'NativeGameplay.json';$Header=Join-Path $Stage 'DasNativeGameplay.generated.h';$Module=Join-Path $Stage 'ue_native_gameplay.das'
& (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasNativeGameplay "-Output=$IR" "-Requests=$Requests" -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$Stage/native-gameplay-export.log"
if($LASTEXITCODE){throw 'Native gameplay export failed'}
$GenerateArgs=@((Join-Path $PSScriptRoot 'GenerateNativeGameplay.py'),$IR,$Header,$Module)
foreach($Item in $ScriptDirectory){$GenerateArgs+=@('--scripts',$Item)}
# Resolve declaration modules from custom source configurations as well.
$Config=Get-Content -LiteralPath $Sources -Raw | ConvertFrom-Json
foreach($Item in $Config.script_dirs){$GenerateArgs+=@('--scripts',([IO.Path]::GetFullPath((Join-Path (Split-Path -Parent ([IO.Path]::GetFullPath($Sources))) $Item))))}
& C:\Python312\python.exe @GenerateArgs
if($LASTEXITCODE){throw 'Native gameplay generation failed'}
if($ProjectMode){
    $HeaderOutput=Join-Path $ProjectRoot 'Source/DasGenerated/DasProjectNativeGameplay.generated.h'
    $ModuleOutput=Join-Path $ProjectRoot 'Content/Das/Scripts/ue_native_gameplay.das'
    $IROutput=Join-Path $ProjectRoot 'Bindings/NativeGameplay.json'
    foreach($File in @($HeaderOutput,$ModuleOutput,$IROutput)){New-Item -ItemType Directory -Path (Split-Path -Parent $File) -Force|Out-Null}
    $Pairs=@(@($IR,$IROutput),@($Header,$HeaderOutput),@($Module,$ModuleOutput))
}else{
$Pairs=@(@($IR,"$Repo\Bindings\NativeGameplay.json"),@($Header,"$Repo\Plugins\DasForUE\Source\DasRuntime\Private\Generated\DasNativeGameplay.generated.h"),@($Module,"$Repo\Plugins\DasForUE\Content\Das\Scripts\ue_native_gameplay.das"))
}
foreach($Pair in $Pairs){if($Check){if(!(Test-Path -LiteralPath $Pair[1]) -or [IO.File]::ReadAllText($Pair[0]).Replace("`r`n","`n") -cne [IO.File]::ReadAllText($Pair[1]).Replace("`r`n","`n")){throw "Stale native gameplay artifact: $($Pair[1])"}}elseif(!(Test-Path -LiteralPath $Pair[1]) -or [IO.File]::ReadAllText($Pair[0]) -cne [IO.File]::ReadAllText($Pair[1])){Copy-Item -LiteralPath $Pair[0] -Destination $Pair[1] -Force}}
if(!$SkipBuild -and !$Check){Build-NativeRegistry 'registry'}
Write-Output 'DAS_NATIVE_GAMEPLAY_ARTIFACTS=PASS'
