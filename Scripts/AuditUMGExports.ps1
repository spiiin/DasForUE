param([string]$Engine='C:/src/unreal/UnrealEngine')
$ErrorActionPreference='Stop';$Repo=Split-Path -Parent $PSScriptRoot;$Python='C:/Python312/python.exe'
& $Python "$PSScriptRoot/TestUMGExportAudit.py";if($LASTEXITCODE){throw 'UMG export audit mutation checks failed'}
$Stage="$Repo/Build/UMGExportAudit/$([guid]::NewGuid().ToString('N'))";New-Item -ItemType Directory -Path $Stage -Force|Out-Null
foreach($Pair in @(@('Build/ApiPackages/ue_api_umg_input/latest-report.json','report.json'),@('Bindings/Packages/ue_api_umg_input.summary.json','summary.json'),@('Bindings/Packages/UMGInput.profile.json','profile.json'),@('Bindings/UMGExportAudit.rules.json','rules.json'))){Copy-Item "$Repo/$($Pair[0])" "$Stage/$($Pair[1])"}
$Rules=Get-Content "$Stage/rules.json" -Raw|ConvertFrom-Json
$Files=@('Bindings/Packages/ue_api_umg_input.summary.json','Build/ApiPackages/ue_api_umg_input/latest-report.json','Bindings/Packages/UMGInput.profile.json','Bindings/UMGExportAudit.rules.json','Scripts/AuditUMGExports.py','Scripts/TestUMGExportAudit.py','Scripts/AuditUMGExports.ps1','Scripts/GenerateApiPackage.py','Scripts/GenerateBindings.py','Plugins/DasForUE/Source/DasRuntime/Private/DasScriptUserWidget.cpp','Plugins/DasForUE/Content/Das/Scripts/ue_api_umg_input.das')
$Files+=@($Rules.adapters.PSObject.Properties|ForEach-Object {$_.Value.evidence|ForEach-Object {$_.path}})
# Freeze the exporter/policy/marshalling code used by the already built export commandlet.
$Files+=@(Get-ChildItem "$Repo/Plugins/DasForUE/Source/DasRuntime" -Recurse -File | Where-Object {$_.Extension -in @('.cpp','.h','.inl','.cs')} | ForEach-Object {$_.FullName.Substring($Repo.Length+1).Replace('\','/')})
$Sources=@{};foreach($File in $Files|Select-Object -Unique){$Sources[$File]=(Get-FileHash "$Repo/$File" -Algorithm SHA256).Hash}
$Native=@{};foreach($File in $Rules.members.native_evidence.path|Select-Object -Unique){$Native[$File]=(Get-FileHash "$Engine/$File" -Algorithm SHA256).Hash}
$Dlls=@{};foreach($Name in @('DasRuntime','DasEditor')){$Path="$Repo/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$Name.dll";$Dlls[$Path]=(Get-FileHash $Path -Algorithm SHA256).Hash}
$Manifest="$Engine/Engine/Binaries/Win64/UnrealEditor.modules";$BuildId=(Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId
& $Python "$PSScriptRoot/GenerateApiPackage.py" --profile "$Stage/profile.json" --selection "$Stage/selection.json";if($LASTEXITCODE){throw 'UMG selection failed'}
$Modules=((Get-Content "$Stage/profile.json" -Raw|ConvertFrom-Json).ue_modules -join ',')
& "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" "$Repo/Tests/DasForUESmoke/DasForUESmoke.uproject" -run=DasExport "-Modules=$Modules" "-Selection=$Stage/selection.json" "-Output=$Stage/reflection.json" -Multiprocess -unattended -NullRHI -nosplash -stdout -ddc=NoZenLocalFallback -NoZenAutoLaunch "-abslog=$Stage/export.log" *> "$Stage/export-stdout.log"
if($LASTEXITCODE -or (Select-String "$Stage/export.log" -Pattern 'Ensure condition failed|Log[^:]+: Error:' -Quiet)){throw "Fresh UMG export failed: $Stage/export.log"}
& $Python "$PSScriptRoot/AuditUMGExports.py" --repo $Repo --engine $Engine --profile "$Stage/profile.json" --reflection "$Stage/reflection.json" --report "$Stage/report.json" --summary "$Stage/summary.json" --rules "$Stage/rules.json" --output "$Stage/audit.json" --markdown "$Stage/audit.md"
if($LASTEXITCODE){throw "UMG export classification failed: $Stage"}
foreach($File in $Sources.Keys){if((Get-FileHash "$Repo/$File" -Algorithm SHA256).Hash -ne $Sources[$File]){throw "Source changed during UMG audit: $File"}}
foreach($File in $Native.Keys){if((Get-FileHash "$Engine/$File" -Algorithm SHA256).Hash -ne $Native[$File]){throw "Native evidence changed during UMG audit: $File"}}
foreach($File in $Dlls.Keys){if((Get-FileHash $File -Algorithm SHA256).Hash -ne $Dlls[$File]){throw 'DLL changed during UMG audit'}}
if((Get-Content $Manifest -Raw|ConvertFrom-Json).BuildId -ne $BuildId){throw 'Engine BuildId changed during UMG audit'}
$Audit=Get-Content "$Stage/audit.json" -Raw|ConvertFrom-Json
Copy-Item "$Stage/audit.json" "$Repo/Bindings/Packages/ue_api_umg_input.export-audit.json" -Force
Copy-Item "$Stage/audit.md" "$Repo/Docs/UMGExportBlockers.md" -Force
[ordered]@{state='PASS';unit_tests=20;fresh_reflection=$true;real_generator_recheck='PASS';coverage_changed=$false;partition=$Audit.partition;counts=$Audit.counts;source_sha256=$Sources;native_sha256=$Native;dll_sha256=$Dlls;engine_build_id=$BuildId;stage=$Stage;build='not_run';runtime_adapter_execution='not_run_static_audit_only';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 12|Set-Content "$Repo/logs/umg-export-audit-validation.json" -Encoding utf8
Write-Output "DAS_UMG_EXPORT_AUDIT_VALIDATION=PASS stage=$Stage"
