param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das}
$Log=Join-Path $Das.Logs 'result-try.log'
$PythonScript=(Join-Path $Das.Repo 'Tests/ResultTry/run.py').Replace('\','/')
$Receipt=Join-Path $Das.Logs 'result-try-validation.json'
if(Test-Path -LiteralPath $Receipt){Remove-Item -LiteralPath $Receipt}
$Arguments=@($Das.Project,'-run=pythonscript',('-script='+$PythonScript),'-EnablePlugins=PythonScriptPlugin','-unattended','-NullRHI','-nosplash','-stdout','-Multiprocess','-ddc=NoZenLocalFallback','-NoZenAutoLaunch',('-abslog='+$Log))
& (Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe') @Arguments *> (Join-Path $Das.Logs 'result-try-stdout.log')
# Negative compile cases intentionally log errors. The full receipt and marker,
# not an unrestricted log-error whitelist, determine whether all cases ran.
if($LASTEXITCODE -notin @(0,1,-1)){throw "Result try editor failed: $LASTEXITCODE"}
Assert-DasLog -Log $Log -Marker 'DAS_RESULT_TRY=PASS checks=8' -FailPattern 'Fatal error:|Assertion failed:|Traceback \(most recent call last\)'
if(!(Test-Path -LiteralPath $Receipt)){throw 'Missing result-try receipt'}
$Result=Get-Content -LiteralPath $Receipt -Raw | ConvertFrom-Json
if(!$Result.passed -or $Result.checks.Count -ne 8){throw 'Incomplete result-try checks'}
Write-Output 'DAS_RESULT_TRY_VALIDATION=PASS checks=8'
