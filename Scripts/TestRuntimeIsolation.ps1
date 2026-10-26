param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
& C:\Python312\python.exe (Join-Path $PSScriptRoot 'GenerateRuntimeBindings.py') --check
if($LASTEXITCODE){throw 'Runtime bindings stale or reference test fixtures'}
$Project=Join-Path $Das.Repo 'Tests/DasRuntimeIsolation/DasRuntimeIsolation.uproject'
# Refresh script glob dependencies when project sources were added since the last build.
$Makefile=Join-Path (Split-Path $Project) 'Intermediate/Build/Win64/x64/DasRuntimeIsolation/Development/Makefile.bin'
if(Test-Path -LiteralPath $Makefile){Remove-Item -LiteralPath $Makefile}
# Run only through the shared build agent. Always build this target: SkipBuild applies to the smoke Editor.
Invoke-DasBuild $Das -Name 'runtime without probes' -BuildArguments @('DasRuntimeIsolation','Win64','Development',"-Project=$Project",'-WaitMutex','-NoXGE','-MaxParallelActions=12','-NoUBTMakefiles','-ForceRulesCompile')
$Root=Split-Path $Project
$Manifests=@(Get-ChildItem (Join-Path $Root 'Intermediate/Build') -Filter 'DasRuntimeIsolation.uhtmanifest' -Recurse)
if(!$Manifests){throw 'Missing UHT manifest for isolation target'}
foreach($Manifest in $Manifests){
    $Runtime=(Get-Content $Manifest.FullName -Raw | ConvertFrom-Json).Modules | Where-Object Name -eq 'DasRuntime'
    $Headers=@($Runtime.PublicHeaders)+@($Runtime.PrivateHeaders)
    if(!($Headers | Where-Object { [IO.Path]::GetFileName($_) -eq 'DasActor.h' })){
        throw 'Production actor host is missing from the game reflection manifest'
    }
    if($Headers | Where-Object { [IO.Path]::GetFileName($_) -eq 'DasProbeActor.h' }){
        throw 'Test actor host leaked into the game reflection manifest'
    }
    if((Get-Content $Manifest.FullName -Raw) -match 'Tests[\\/]+Runtime[\\/]'){
        throw "Test reflection leaked into production UHT manifest: $($Manifest.FullName)"
    }
}
$Responses=@(Get-ChildItem (Join-Path $Root 'Intermediate/Build') -Filter '*.rsp' -Recurse | Where-Object { $_.FullName -notmatch '[\\/]UnrealEditor[\\/]' })
if(!$Responses){throw 'Missing build response files for isolation target'}
$ProbeNames=@(Get-ChildItem (Join-Path $Das.Repo 'Plugins/DasForUE/Tests/Runtime') -Filter '*.cpp' -Recurse | ForEach-Object { [regex]::Escape($_.Name) })
$ProbePattern='Tests[\\/]+Runtime[\\/]|'+($ProbeNames -join '|')
foreach($Response in $Responses){
    if((Get-Content $Response.FullName -Raw) -match $ProbePattern){
        throw "Test sources leaked into production build: $($Response.FullName)"
    }
}
Write-Output 'DAS_RUNTIME_ISOLATION=PASS (game linked; no test source directories or UHT fixtures)'
