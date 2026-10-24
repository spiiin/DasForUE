param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
if (!$SkipBuild) {Invoke-DasBuild $Das -Name 'UE AOT generator'}
$Generated=Join-Path $Das.Repo 'Plugins\DasForUE\Source\DasRuntime\Private\Generated\UEGuardAot.cpp'
Invoke-DasCommandlet $Das -Run DasUEAot -Name 'ue-aot-generate' -Arguments @("-Output=$Generated") -Marker 'DAS_UE_AOT_GENERATED=PASS' | Out-Null
Invoke-DasBuild $Das -Name 'Link generated UE AOT' -Log (Join-Path $Das.Logs 'ue-aot-link.log')
& C:\Python312\python.exe (Join-Path $PSScriptRoot 'AuditUEExternGuards.py') --repo $Das.Repo --output (Join-Path $Das.Logs 'ue-aot-guard-coverage.json')
if ($LASTEXITCODE) {throw 'UE extern guard source audit failed'}
$Paths={Get-DasValidationPaths $Das -Extra @('Scripts/TestUEAot.ps1','Scripts/AuditUEExternGuards.py')}
Invoke-DasValidation $Das -Run DasUEAot -Name 'ue-aot-runtime' -Marker 'DAS_UE_AOT_RESULT=PASS' -Checks 19 `
    -SourcePaths $Paths -Output 'ue-aot-validation.json' -SuccessMarker 'DAS_UE_AOT_VALIDATION=PASS' `
    -Mode 'linked native UE AOT required; scalar ABI, role/thread/context/owner/property guards and recovery' `
    -RequiredPattern 'DAS_UE_AOT_ABI_ENTRIES=[1-9][0-9]*' `
    -FailPattern 'DAS_UE_AOT_FAIL|Fatal error:|Ensure condition failed'
