param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
New-Item -ItemType Directory -Path "$Repo/logs" -Force|Out-Null
function Get-GASSourceFingerprint {
 $Rows=foreach($File in Get-ChildItem "$Repo/Plugins/DasForUE/Source","$Repo/Plugins/DasForUE/Content/Das/Scripts" -Recurse -File|Where-Object {$_.Extension -in '.cpp','.h','.inl','.cs','.das'}) {
  $File.FullName+':'+(Get-FileHash -LiteralPath $File.FullName -Algorithm SHA256).Hash
 }
 return ($Rows|Sort-Object) -join "`n"
}
$Sources=Get-GASSourceFingerprint
[ordered]@{state='pending';scope='GAS focused runtime'}|ConvertTo-Json|Set-Content "$Repo/logs/gas-validation.json" -Encoding utf8
if(!$SkipBuild){
 Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @('DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE', '-MaxParallelActions=8', "-Log=$Repo/logs/gas-build.log") *> "$Repo/logs/gas-build-stdout.log"
 if($LASTEXITCODE){throw 'GAS build failed'}
}
if((Get-GASSourceFingerprint) -cne $Sources){throw 'Source changed during GAS build'}
$Dlls=@{}
foreach($Name in 'DasRuntime','DasEditor'){
 $Path="$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$Name.dll"
 $Dlls[$Path]=(Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash
}
$Log="$Repo/logs/gas-runtime.log"
# Windows PowerShell treats redirected native stderr as ErrorRecords under Stop.
# Preserve diagnostics, then enforce native exit code and the existing log assertions.
$PreviousErrorAction=$ErrorActionPreference
try { $ErrorActionPreference='Continue'
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasGAS -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$Log" *> "$Repo/logs/gas-runtime-stdout.log"
} finally { $ErrorActionPreference=$PreviousErrorAction }
$Exit=$LASTEXITCODE
$Text=Get-Content -LiteralPath $Log -Raw
if($Exit -or !(Get-DasResult -Text $Text -Pattern 'DAS_GAS_RESULT=PASS checks=(\d+)').Success -or $Text -match 'Ensure condition failed|Log[^:]+: Error:'){throw "GAS focused runtime failed: $Log"}
if((Get-GASSourceFingerprint) -cne $Sources){throw 'Source changed during GAS runtime check'}
foreach($Path in $Dlls.Keys){if((Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash -ne $Dlls[$Path]){throw 'DLL changed during GAS runtime check'}}
[ordered]@{state='PASS';dll_sha256=$Dlls;runtime_log=$Log;scope='native GAS health, instant effects, queued owned snapshots, release';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json|Set-Content "$Repo/logs/gas-validation.json" -Encoding utf8
Write-Output 'DAS_GAS_FOCUSED=PASS'
