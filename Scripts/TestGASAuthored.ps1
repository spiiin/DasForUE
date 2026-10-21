param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
New-Item -ItemType Directory -Path "$Repo/logs" -Force|Out-Null
[ordered]@{state='pending';revision=(& git -C $Repo rev-parse HEAD);scope='authored GAS focused runtime'}|ConvertTo-Json|Set-Content "$Repo/logs/gas-authored-validation.json" -Encoding utf8
if(!$SkipBuild){
 & "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=8 "-Log=$Repo/logs/gas-authored-build.log" *> "$Repo/logs/gas-authored-build-stdout.log"
 if($LASTEXITCODE){throw 'Authored GAS build failed'}
}
$Log="$Repo/logs/gas-authored-runtime.log"
# Windows PowerShell treats redirected native stderr as ErrorRecords under Stop.
# Preserve diagnostics, then enforce native exit code and the existing log assertions.
$PreviousErrorAction=$ErrorActionPreference
try { $ErrorActionPreference='Continue'
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasGASAuthored "-Fixture=$Repo/Tests/GASAuthored/probe.template.das" -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$Log" *> "$Repo/logs/gas-authored-runtime-stdout.log"
} finally { $ErrorActionPreference=$PreviousErrorAction }
$Code=$LASTEXITCODE
$Text=Get-Content -LiteralPath $Log -Raw
$Match=[regex]::Match($Text,'DAS_GAS_AUTHORED_RESULT=PASS checks=(\d+) assets=(\S+)')
if($Code -or !$Match.Success -or $Text -match 'Ensure condition failed|Log[^:]+: Error:'){throw "Authored GAS check failed: $Log"}
$Folder=$Match.Groups[2].Value
$ReopenLog="$Repo/logs/gas-authored-reopen.log"
# Windows PowerShell treats redirected native stderr as ErrorRecords under Stop.
# Preserve diagnostics, then enforce native exit code and the existing log assertions.
$PreviousErrorAction=$ErrorActionPreference
try { $ErrorActionPreference='Continue'
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasGASAuthored "-Fixture=$Repo/Tests/GASAuthored/probe.template.das" "-Reopen=$Folder" -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$ReopenLog" *> "$Repo/logs/gas-authored-reopen-stdout.log"
} finally { $ErrorActionPreference=$PreviousErrorAction }
$ReopenCode=$LASTEXITCODE
$ReopenText=Get-Content -LiteralPath $ReopenLog -Raw
$ReopenMatch=[regex]::Match($ReopenText,'DAS_GAS_AUTHORED_RESULT=PASS checks=(\d+) assets=(\S+)')
if($ReopenCode -or !$ReopenMatch.Success -or $ReopenMatch.Groups[1].Value -ne $Match.Groups[1].Value -or $ReopenMatch.Groups[2].Value -ne $Folder -or $ReopenText -match 'Ensure condition failed|Log[^:]+: Error:'){throw "Authored GAS reopen failed: $ReopenLog"}
[ordered]@{state='PASS';revision=(& git -C $Repo rev-parse HEAD);updated_utc=[DateTime]::UtcNow.ToString('o');checks=[int]$Match.Groups[1].Value;assets=$Match.Groups[2].Value;runtime_log=$Log;reopen_log=$ReopenLog;reopen='PASS';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json|Set-Content "$Repo/logs/gas-authored-validation.json" -Encoding utf8
Write-Output 'DAS_GAS_AUTHORED_FOCUSED=PASS'
