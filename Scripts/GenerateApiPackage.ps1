param(
 [string]$Engine='C:\src\unreal\UnrealEngine',
 [string]$Profile,
 [switch]$SkipBuild,
 [switch]$Publish
)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
if(!$Profile){$Profile=Join-Path $Repo 'Bindings/Packages/CoreMath.profile.json'}
$Definition=Get-Content -LiteralPath $Profile -Raw|ConvertFrom-Json
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
$Stage=Join-Path $Repo ("Build/ApiPackages/"+$Definition.module+'/'+[guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $Stage -Force|Out-Null
$Python='C:\Python312\python.exe'
$Editor=Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
& $Python "$PSScriptRoot/GenerateApiPackage.py" --profile $Profile --selection "$Stage/selection.json"
if($LASTEXITCODE){throw 'Invalid API package profile'}
if(!$SkipBuild){
 & "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12 "-Log=$Stage/build.log" *> "$Stage/build-stdout.log"
 if($LASTEXITCODE){throw "API package commandlet build failed: $Stage/build.log"}
}
$Modules=$Definition.ue_modules -join ','
$Common=@($Project,'-run=DasExport',"-Modules=$Modules",'-Multiprocess','-unattended','-NullRHI','-nosplash','-stdout','-ddc=NoZenLocalFallback','-NoZenAutoLaunch')
& $Editor @Common "-Selection=$Stage/selection.json" "-Output=$Stage/reflection.json" "-abslog=$Stage/export.log" *> "$Stage/export-stdout.log"
if($LASTEXITCODE){throw "API package export failed: $Stage/export.log"}
& $Python "$PSScriptRoot/GenerateApiPackage.py" --profile $Profile --ir "$Stage/reflection.json" --stage $Stage
if($LASTEXITCODE){throw "API package generation failed: $Stage"}
# The hash-suffixed staged module cannot be shadowed by an older installed package.
$ProbeArgs=@();if($Definition.probe_world){$ProbeArgs+='-GameWorld'}
& $Editor @Common @ProbeArgs "-Probe=$Stage/probe.das" "-abslog=$Stage/preflight.log" *> "$Stage/preflight-stdout.log"
if($LASTEXITCODE -or !(Select-String -LiteralPath "$Stage/preflight.log" -SimpleMatch 'DAS_PACKAGE_PREFLIGHT=PASS value=42' -Quiet)){
 throw "API package preflight failed; package not published: $Stage/preflight.log"
}
if($Definition.world_probe){
 & $Editor @Common -GameWorld -WorldOwner "-Probe=$Stage/world-probe.das" "-abslog=$Stage/world-preflight.log" *> "$Stage/world-preflight-stdout.log"
 if($LASTEXITCODE -or !(Select-String -LiteralPath "$Stage/world-preflight.log" -SimpleMatch 'DAS_PACKAGE_PREFLIGHT=PASS value=42' -Quiet)){
  throw "World receiver probe failed; package not published: $Stage/world-preflight.log"
 }
}
$Report=Get-Content -LiteralPath "$Stage/report.json" -Raw|ConvertFrom-Json
$Report.state='ue_preflight_passed'
$Report|Add-Member -NotePropertyName preflight -NotePropertyValue @{compile='passed';runtime_probe='passed';build_mode='Editor NullRHI';game_world=[bool]$Definition.probe_world;packaged='not_run';profile_sha256=(Get-FileHash -LiteralPath $Profile -Algorithm SHA256).Hash.ToLower()}
if($Publish){
 $Destination=Join-Path $Repo ('Plugins/DasForUE/Content/Das/Scripts/'+$Definition.module+'.das')
 $Source=Join-Path $Stage ($Definition.module+'.das')
 if((Get-FileHash -LiteralPath $Source -Algorithm SHA256).Hash.ToLower() -ne $Report.source_sha256){throw 'Generated source changed after preflight'}
 # Copy to a sibling, then replace only after successful UE verification.
 $Pending=$Destination+'.'+[guid]::NewGuid().ToString('N')+'.pending'
 Copy-Item -LiteralPath $Source -Destination $Pending -Force
 Move-Item -LiteralPath $Pending -Destination $Destination -Force
 # New script names are not in an existing UBT makefile's enumerated dependencies.
 # Invalidate the rules cache so the next build regenerates its UFS receipt.
 (Get-Item -LiteralPath (Join-Path $Repo 'Plugins/DasForUE/Source/DasRuntime/DasRuntime.Build.cs')).LastWriteTimeUtc=[DateTime]::UtcNow
 $Report.state='published'
 $Summary=[ordered]@{
  schema_version=1;package=$Report.package;engine_version=$Report.engine_version
  source_sha256=$Report.source_sha256;state=$Report.state;preflight=$Report.preflight
  value_types=$Report.value_types;default_arguments=$Report.default_arguments;dependencies=$Report.dependencies
  candidate_count=$Report.candidate_count;construction_guarded_count=$Report.construction_guarded_count;unique_declaration_count=$Report.unique_declaration_count;unknown_declaration_occurrences=$Report.unknown_declaration_occurrences;surfaces=$Report.surfaces;counts=$Report.counts
  blockers=@($Report.members|Where-Object {$_.status -ne 'generation_ready'})
 }
 $Summary|ConvertTo-Json -Depth 100|Set-Content -LiteralPath (Join-Path $Repo ('Bindings/Packages/'+$Definition.module+'.summary.json')) -Encoding utf8
}
$Report|ConvertTo-Json -Depth 100|Set-Content -LiteralPath "$Stage/report.json" -Encoding utf8
Copy-Item -LiteralPath "$Stage/report.json" -Destination (Join-Path (Split-Path $Stage -Parent) 'latest-report.json') -Force
Write-Output "DAS_API_PACKAGE=PASS state=$($Report.state) functions=$($Report.counts.generation_ready) report=$Stage/report.json"
