param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild,[string]$Profile)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
if(!$Profile){$Profile=Join-Path $Repo 'Bindings/ApiCoverage.profile.json'}
$Stage=Join-Path $Repo 'Build/ApiCoverage';$Logs=Join-Path $Repo 'logs'
New-Item -ItemType Directory -Path $Stage,$Logs -Force|Out-Null
if(!$SkipBuild){
 & "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12 "-Log=$Logs/api-coverage-build.log"
 if($LASTEXITCODE){throw 'API coverage exporter build failed'}
}
$Reflection=Join-Path $Stage 'Reflection.json'
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasNativeGameplay -Coverage "-Profile=$Profile" "-Output=$Reflection" -Multiprocess -unattended -NullRHI -nosplash -stdout "-abslog=$Logs/api-coverage-export.log" -ddc=NoZenLocalFallback -NoZenAutoLaunch *> "$Logs/api-coverage-export-stdout.log"
if($LASTEXITCODE){throw "API coverage export failed: $Logs/api-coverage-export.log"}
# Verify that sharing the audit policy did not change the live registry or views.
& "$PSScriptRoot/GenerateNativeGameplay.ps1" -Engine $Engine -Check -SkipBuild *> "$Logs/api-coverage-native-check.log"
if(!$?){throw 'Native gameplay artifacts are stale; inspect api-coverage-native-check.log'}
& C:\Python312\python.exe "$PSScriptRoot/TestApiCoverage.py"
if($LASTEXITCODE){throw 'API coverage report tests failed'}
& C:\Python312\python.exe "$PSScriptRoot/TestNativeGameplayGenerator.py"
if($LASTEXITCODE){throw 'Native gameplay generator regression tests failed'}
& C:\Python312\python.exe "$PSScriptRoot/AuditApiCoverage.py" --inventory $Reflection --libraries "$Reflection.libraries.json" --output "$Stage/report.json" --summary "$Repo/Bindings/ApiCoverage.summary.json" --markdown "$Repo/Docs/Dev/Audits/ApiCoverage.md" --csv "$Stage/members.csv"
if($LASTEXITCODE){throw 'API coverage report generation failed'}
Write-Output "DAS_API_COVERAGE=PASS report=$Stage/report.json summary=$Repo/Docs/Dev/Audits/ApiCoverage.md"
