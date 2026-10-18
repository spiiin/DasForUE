param(
 [string]$Engine='C:\src\unreal\UnrealEngine',
 [string]$Report,
 [switch]$Regenerate
)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Python='C:\Python312\python.exe'
if($Regenerate){
 & "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -Profile "$Repo/Bindings/Packages/CollisionPhysics.profile.json" -SkipBuild | Tee-Object -Variable PackageOutput
 if(!$?){throw 'Collision regeneration failed'}
 $PackageMatch=[regex]::Match(($PackageOutput -join "`n"),'DAS_API_PACKAGE=PASS[^\r\n]*report=(.+/report\.json)')
 if(!$PackageMatch.Success){throw 'Missing regenerated package report'}
 $Report=$PackageMatch.Groups[1].Value.Trim()
}
if(!$Report){$Report="$Repo/Build/ApiPackages/ue_api_collision/latest-report.json"}
$Report=(Resolve-Path -LiteralPath $Report).Path
$Stage=Split-Path -Parent $Report
# latest-report is a pointer copy, while reflection/source live in the run folder.
if(!(Test-Path -LiteralPath "$Stage/reflection.json")){
 $ReportObject=Get-Content -LiteralPath $Report -Raw|ConvertFrom-Json
 $Candidates=@(Get-ChildItem -LiteralPath $Stage -Directory|Where-Object {
  $CandidateReport=Join-Path $_.FullName 'report.json'
  (Test-Path -LiteralPath $CandidateReport) -and ((Get-Content -LiteralPath $CandidateReport -Raw|ConvertFrom-Json).staged_module -eq $ReportObject.staged_module)
 })
 if($Candidates.Count -ne 1){throw 'Pass the exact fresh run report.json path'}
 $Stage=$Candidates[0].FullName;$Report=Join-Path $Stage 'report.json'
}
$Validation=Join-Path $Stage ('trace-matrix-'+[guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $Validation|Out-Null
$ReportObject=Get-Content -LiteralPath $Report -Raw|ConvertFrom-Json
Copy-Item -LiteralPath (Join-Path $Stage ($ReportObject.staged_module+'.das')) -Destination $Validation
$Success=$false
$Dlls=@("$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-DasRuntime.dll","$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-DasEditor.dll")
function GetValidationHashes {
 $Result=[ordered]@{}
 foreach($Path in $Dlls){$Result[$Path]=(Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash}
 foreach($Folder in @('Plugins/DasForUE/Source','Plugins/DasForUE/Content/Das/Scripts')){
  foreach($File in Get-ChildItem -LiteralPath "$Repo/$Folder" -File -Recurse|Sort-Object FullName){
   $Result[$File.FullName]=(Get-FileHash -LiteralPath $File.FullName -Algorithm SHA256).Hash
  }
 }
 return ($Result|ConvertTo-Json -Depth 5 -Compress)
}
$Before=GetValidationHashes
try {
 & $Python "$PSScriptRoot/CheckCollisionHitResultCoverage.py" $Report --traces-only --output "$Validation/coverage.json"
 if($LASTEXITCODE){throw 'HitResult coverage incomplete; runtime matrix not started'}
 & $Python "$PSScriptRoot/GenerateCollisionTraceMatrix.py" $Report --output "$Validation/probe.das"
 if($LASTEXITCODE){throw 'Trace matrix generation failed'}
 $Project="$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject"
 $Editor="$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe"
 & $Editor $Project -run=DasExport -Modules=Engine -GameWorld "-Probe=$Validation/probe.das" -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$Validation/runtime.log" *> "$Validation/stdout.log"
 if($LASTEXITCODE){throw "Trace matrix failed: $Validation/runtime.log"}
 $Text=Get-Content -LiteralPath "$Validation/runtime.log" -Raw
 if(!$Text.Contains('DAS_PACKAGE_PREFLIGHT=PASS value=42')){throw 'Missing runtime success marker'}
 $TraceOutput=Get-Content -LiteralPath "$Validation/stdout.log" -Raw
 $Passed=@([regex]::Matches($TraceOutput,'DAS_TRACE_MATRIX_PASS ([A-Za-z0-9_]+)')|ForEach-Object {$_.Groups[1].Value}|Sort-Object -Unique)
 if($Passed.Count -ne 24){throw "Expected 24 unique variants, got $($Passed.Count)"}
 if((GetValidationHashes) -ne $Before){throw 'Sources or DLL changed during validation; rerun on a stable build'}
 $Success=$true
 [ordered]@{state='PASS';report=$Report;runtime_variants=$Passed;queries=96;break_hit_result_roundtrips=24;mode='transient Game world / Editor NullRHI';source_and_dll_hashes=($Before|ConvertFrom-Json);PIE='not_run';packaged='not_run';full_foundation='not_run';multi_object_ordering='not_assessed';GC='separate_adapter_tests';commit='not_created'}|ConvertTo-Json -Depth 20|Set-Content -LiteralPath "$Validation/result.json" -Encoding utf8
 Copy-Item -LiteralPath "$Validation/result.json" -Destination "$Repo/logs/collision-trace-matrix-validation.json" -Force
 Write-Output "DAS_COLLISION_TRACE_MATRIX=PASS variants=24 queries=96 report=$Validation/result.json"
} finally {
 if(!$Success){Write-Output "DAS_COLLISION_TRACE_MATRIX=FAIL evidence=$Validation"}
}
