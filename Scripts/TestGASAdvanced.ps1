param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
[ordered]@{state='pending';scope='GAS tasks, targets and calculations; one focused process'}|ConvertTo-Json|Set-Content "$Repo/logs/gas-advanced-validation.json" -Encoding utf8
if(!$SkipBuild){
 & "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -Module=DasRuntime+DasEditor -NoEngineChanges -WaitMutex -NoXGE -MaxParallelActions=8 "-Log=$Repo/logs/gas-advanced-build.log" *> "$Repo/logs/gas-advanced-build-stdout.log"
 if($LASTEXITCODE){throw 'Advanced GAS build failed'}
}
$Log="$Repo/logs/gas-advanced-runtime.log"
# Windows PowerShell treats redirected native stderr as ErrorRecords under Stop.
# Preserve diagnostics, then enforce native exit code and the existing log assertions.
$PreviousErrorAction=$ErrorActionPreference
try { $ErrorActionPreference='Continue'
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasGASAdvanced "-Fixture=$Repo/Tests/GASAuthored/advanced.template.das" -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$Log" *> "$Repo/logs/gas-advanced-runtime-stdout.log"
} finally { $ErrorActionPreference=$PreviousErrorAction }
$Code=$LASTEXITCODE;$Text=Get-Content -LiteralPath $Log -Raw;$Match=[regex]::Match($Text,'DAS_GAS_ADVANCED_RESULT=PASS checks=(\d+)')
if($Code -or !$Match.Success -or $Text -match 'Ensure condition failed|Log[^:]+: Error:|DAS_GAS_ABILITY_HOOK_FAILED|DAS_GAS_CALCULATION_FAILED'){throw "Advanced GAS check failed: $Log"}
$Hashes=[ordered]@{};$Files=@(& git -C $Repo diff --name-only HEAD)+@(& git -C $Repo ls-files --others --exclude-standard)
foreach($Name in ($Files|Sort-Object -Unique)){$Path=Join-Path $Repo $Name;if(Test-Path -LiteralPath $Path -PathType Leaf){$Hashes[$Name]=(Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash}}
$Dlls=[ordered]@{};foreach($Name in @('DasRuntime','DasEditor')){$Dlls[$Name]=(Get-FileHash -LiteralPath "$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$Name.dll" -Algorithm SHA256).Hash}
[ordered]@{state='PASS';base_revision=(& git -C $Repo rev-parse HEAD);updated_utc=[DateTime]::UtcNow.ToString('o');checks=[int]$Match.Groups[1].Value;runtime_log=$Log;processes=1;PIE='not_run';packaged='not_run';commit='not_created';source_sha256=$Hashes;dll_sha256=$Dlls}|ConvertTo-Json -Depth 5|Set-Content "$Repo/logs/gas-advanced-validation.json" -Encoding utf8
Write-Output 'DAS_GAS_ADVANCED_FOCUSED=PASS'
