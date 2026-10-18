param([string]$Engine='C:/src/unreal/UnrealEngine')
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Python='C:/Python312/python.exe'
& $Python "$PSScriptRoot/TestUMGInputAudit.py"
if($LASTEXITCODE){throw 'UMG input audit unit tests failed'}
$Stage="$Repo/Build/UMGInputAudit/$([guid]::NewGuid().ToString('N'))"
New-Item -ItemType Directory -Path $Stage -Force|Out-Null
# Freeze evidence, without replacing the installed package or coverage summary.
Copy-Item "$Repo/Build/ApiPackages/ue_api_umg_input/latest-report.json" "$Stage/report.json"
Copy-Item "$Repo/Bindings/Packages/ue_api_umg_input.summary.json" "$Stage/summary.json"
Copy-Item "$Repo/Bindings/Packages/UMGInput.profile.json" "$Stage/profile.json"
$EngineManifest="$Engine/Engine/Binaries/Win64/UnrealEditor.modules"
$BuildId=(Get-Content $EngineManifest -Raw|ConvertFrom-Json).BuildId
$DllHashes=@{}
foreach($Name in @('DasRuntime','DasEditor')){
 $Path="$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$Name.dll"
 $DllHashes[$Path]=(Get-FileHash $Path -Algorithm SHA256).Hash
}
& $Python "$PSScriptRoot/GenerateApiPackage.py" --profile "$Stage/profile.json" --selection "$Stage/selection.json"
if($LASTEXITCODE){throw 'UMG audit selection failed'}
$Modules=((Get-Content "$Stage/profile.json" -Raw|ConvertFrom-Json).ue_modules -join ',')
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" "$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject" -run=DasExport "-Modules=$Modules" "-Selection=$Stage/selection.json" "-Output=$Stage/reflection.json" -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$Stage/export.log" *> "$Stage/export-stdout.log"
if($LASTEXITCODE -or (Select-String "$Stage/export.log" -Pattern 'Ensure condition failed|Log[^:]+: Error:' -Quiet)){throw "Fresh UMG reflection failed: $Stage/export.log"}
& $Python "$PSScriptRoot/AuditUMGInput.py" --profile "$Stage/profile.json" --report "$Stage/report.json" --summary "$Stage/summary.json" --reflection "$Stage/reflection.json" --engine $Engine --contract "$Repo/Bindings/InputEvents.contract.json" --output "$Stage/audit.json" --markdown "$Stage/audit.md"
if($LASTEXITCODE){throw 'UMG source/type/contract audit failed'}
$Audit=Get-Content "$Stage/audit.json" -Raw|ConvertFrom-Json
if($Audit.runtime_transport_implemented){if($Audit.generation_failures -ne 37 -or $Audit.scoped_rejections.Count -ne 4){throw 'Implemented UMG snapshot scope changed'}}
elseif($Audit.generation_failures -ne 74 -or $Audit.input_scope.detached_snapshot_candidates -ne 33 -or $Audit.input_scope.navigation_context_candidates -ne 3){throw 'UMG 74-failure snapshot has changed; review the scope'}
foreach($Path in $DllHashes.Keys){if((Get-FileHash $Path -Algorithm SHA256).Hash -ne $DllHashes[$Path]){throw 'DLL changed during audit'}}
if((Get-Content $EngineManifest -Raw|ConvertFrom-Json).BuildId -ne $BuildId){throw 'Engine BuildId changed during audit'}
Copy-Item "$Stage/audit.json" "$Repo/Bindings/Packages/ue_api_umg_input.audit.json" -Force
Copy-Item "$Stage/audit.md" "$Repo/Docs/UMGInputBlockers.md" -Force
[ordered]@{state='PASS';unit_tests=16;fresh_reflection=$true;real_generator_recheck='PASS';native_source_anchors='PASS';contract=(Get-Content "$Repo/Bindings/InputEvents.contract.json" -Raw|ConvertFrom-Json).status;coverage_changed=$false;generation_failures=$Audit.generation_failures;partition=$Audit.partition;input_scope=$Audit.input_scope;dll_sha256=$DllHashes;engine_build_id=$BuildId;stage=$Stage;build='not_run';PIE='not_run';packaged='not_run';physical_input='not_run';runtime_transport=$Audit.runtime_transport_implemented;commit='not_created'}|ConvertTo-Json -Depth 12|Set-Content "$Repo/logs/umg-input-audit-validation.json" -Encoding utf8
Write-Output "DAS_UMG_INPUT_AUDIT_VALIDATION=PASS stage=$Stage"
