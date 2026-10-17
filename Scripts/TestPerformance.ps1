param(
    [string]$Engine='C:\src\unreal\UnrealEngine',
    [ValidateRange(8,256)][int]$Iterations=64,
    [ValidateRange(3,31)][int]$Samples=9,
    [switch]$SkipBuild,
    [switch]$TraceMemory,
    [string]$PackagedExe='',
    [string]$LogPrefix='performance'
)
$ErrorActionPreference='Stop'
if($LogPrefix -notmatch '^[a-zA-Z0-9_-]+$'){throw 'Invalid log prefix'}
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
$Logs=Join-Path $Repo 'logs';New-Item -ItemType Directory -Path $Logs -Force|Out-Null
$Mode=if($PackagedExe){'packaged'}else{'editor'}
$Output=Join-Path $Logs "$LogPrefix-$Mode.json";$Log=Join-Path $Logs "$LogPrefix-$Mode.log"
$Trace=Join-Path $Logs "$LogPrefix-$Mode.utrace"
$TraceArgs=if($TraceMemory){@('-trace=default,memory',"-tracefile=$Trace",'-DasPerformanceTrace','-tracefiletrunc')}else{@()}
if(Test-Path -LiteralPath $Output){Remove-Item -LiteralPath $Output}
if($PackagedExe){
    if(!(Test-Path -LiteralPath $PackagedExe)){throw 'Packaged executable missing; build/cook/stage it first'}
    $Arguments=@('/Engine/Maps/Entry','-DasPerformance',"-PerfIterations=$Iterations","-PerfSamples=$Samples","-PerfOutput=`"$Output`"",'-unattended','-nullrhi','-nosplash',"-abslog=`"$Log`"")+$TraceArgs
    $Process=Start-Process -FilePath $PackagedExe -ArgumentList $Arguments -WindowStyle Hidden -PassThru -Wait
    if($Process.ExitCode){throw 'Packaged benchmark failed'}
}else{
    if(!$SkipBuild){
        & "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12
        if($LASTEXITCODE){throw 'Editor benchmark build failed'}
    }
    & "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasPerformance "-PerfIterations=$Iterations" "-PerfSamples=$Samples" "-PerfOutput=$Output" -Multiprocess -unattended -NullRHI -nosplash -stdout "-abslog=$Log" -ddc=NoZenLocalFallback -NoZenAutoLaunch @TraceArgs *> "$Logs/$LogPrefix-$Mode-stdout.log"
    if($LASTEXITCODE){throw 'Editor benchmark failed'}
}
if(!(Test-Path -LiteralPath $Output) -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_PERFORMANCE_RESULT=PASS')){throw 'Missing benchmark report or PASS marker'}
$Result=Get-Content -LiteralPath $Output -Raw|ConvertFrom-Json
if($Result.result -ne 'PASS' -or $Result.cases.Count -ne 33 -or $Result.sample_count -ne $Samples){throw 'Incomplete benchmark results'}
if($TraceMemory -and (!(Test-Path -LiteralPath $Trace) -or (Get-Item -LiteralPath $Trace).Length -eq 0)){throw 'Memory trace file missing or empty'}
$Result.cases|Select-Object case,container_size,median_ms,p95_ms|Format-Table -AutoSize
Write-Output "PERFORMANCE $Mode PASS $Output"
