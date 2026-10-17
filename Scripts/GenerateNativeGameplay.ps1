param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$Check,[switch]$SkipBuild,[string]$Sources,[string[]]$Entry=@(),[string[]]$ScriptDirectory=@())
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
if(!$Sources){$Sources=Join-Path $Repo 'Bindings/NativeGameplay.sources.json'}
New-Item -ItemType Directory -Path (Join-Path $Repo 'logs') -Force | Out-Null
$Stage=Join-Path $Repo 'Build\NativeGameplay';New-Item -ItemType Directory -Path $Stage -Force | Out-Null
function Build-NativeRegistry([string]$Phase){
    $BuildLog=Join-Path $Repo ('logs/native-gameplay-'+$Phase+'-'+[guid]::NewGuid().ToString('N')+'.log')
    & (Join-Path $Engine 'Engine/Build/BatchFiles/Build.bat') DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12 "-Log=$BuildLog"
    if($LASTEXITCODE){throw "Native gameplay $Phase build failed"}
}
if(!$SkipBuild){Build-NativeRegistry 'exporter'}
$Requests=Join-Path $Stage 'NativeGameplay.requests.json'
$DiscoverArgs=@((Join-Path $PSScriptRoot 'DiscoverNativeGameplay.py'),'--config',$Sources,'--output',$Requests)
foreach($Item in $Entry){$DiscoverArgs+=@('--entry',$Item)}
foreach($Item in $ScriptDirectory){$DiscoverArgs+=@('--scripts',$Item)}
& C:\Python312\python.exe @DiscoverArgs
if($LASTEXITCODE){throw 'Native gameplay discovery failed'}
$IR=Join-Path $Stage 'NativeGameplay.json';$Header=Join-Path $Stage 'DasNativeGameplay.generated.h';$Module=Join-Path $Stage 'ue_native_gameplay.das'
& (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasNativeGameplay "-Output=$IR" "-Requests=$Requests" -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$Repo\logs\native-gameplay-export.log"
if($LASTEXITCODE){throw 'Native gameplay export failed'}
$GenerateArgs=@((Join-Path $PSScriptRoot 'GenerateNativeGameplay.py'),$IR,$Header,$Module)
foreach($Item in $ScriptDirectory){$GenerateArgs+=@('--scripts',$Item)}
# Resolve declaration modules from custom source configurations as well.
$Config=Get-Content -LiteralPath $Sources -Raw | ConvertFrom-Json
foreach($Item in $Config.script_dirs){$GenerateArgs+=@('--scripts',([IO.Path]::GetFullPath((Join-Path (Split-Path -Parent ([IO.Path]::GetFullPath($Sources))) $Item))))}
& C:\Python312\python.exe @GenerateArgs
if($LASTEXITCODE){throw 'Native gameplay generation failed'}
$Pairs=@(@($IR,"$Repo\Bindings\NativeGameplay.json"),@($Header,"$Repo\Plugins\DasForUE\Source\DasRuntime\Private\DasNativeGameplay.generated.h"),@($Module,"$Repo\Plugins\DasForUE\Content\Das\Scripts\ue_native_gameplay.das"))
foreach($Pair in $Pairs){if($Check){if(!(Test-Path -LiteralPath $Pair[1]) -or [IO.File]::ReadAllText($Pair[0]).Replace("`r`n","`n") -cne [IO.File]::ReadAllText($Pair[1]).Replace("`r`n","`n")){throw "Stale native gameplay artifact: $($Pair[1])"}}else{Copy-Item -LiteralPath $Pair[0] -Destination $Pair[1] -Force}}
if(!$SkipBuild -and !$Check){Build-NativeRegistry 'registry'}
Write-Output 'DAS_NATIVE_GAMEPLAY_ARTIFACTS=PASS'
