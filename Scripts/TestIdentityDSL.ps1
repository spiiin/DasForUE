param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){
    & "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$($Das.Project)" `
        -Module=DasRuntime -Module=DasEditor -Module=DasForUESmoke -NoEngineChanges -WaitMutex -NoXGE -MaxParallelActions=8
    if($LASTEXITCODE){throw 'Identity DSL module build failed'}
}
function Get-IdentityHashes {
    $Paths=@(Get-ChildItem "$($Das.Repo)/Plugins/DasForUE/Source" -Recurse -File |
        Where-Object Extension -in '.h','.cpp','.inl','.cs' | ForEach-Object FullName)
    $Paths+=@(Get-ChildItem "$($Das.Repo)/Plugins/DasForUE/Content/Das/Scripts" -File -Filter '*.das' |
        ForEach-Object FullName)
    $Paths+="$($Das.Repo)/Tests/IdentityDSL/probe.das"
    $Paths+="$($Das.Repo)/Tests/IdentityDSL/defaults.das"
    $Paths+=@('DasRuntime','DasEditor' | ForEach-Object {
        "$($Das.Repo)/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$_.dll"
    })
    $Paths+="$Engine/Engine/Binaries/Win64/UnrealEditor.modules"
    $Paths+="$($Das.Repo)/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor.modules"
    $Paths+="$($Das.Repo)/Bindings/FoundationExterns.txt"
    $Paths+="$($Das.Repo)/Scripts/TestIdentityDSL.ps1"
    $Result=[ordered]@{}
    foreach($Path in ($Paths | Sort-Object)){$Result[$Path]=(Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash}
    return ($Result | ConvertTo-Json -Compress)
}
$Before=Get-IdentityHashes
$Log=Invoke-DasCommandlet $Das -Run DasIdentityDSL -Name 'identity-dsl-runtime' -NoZen `
    -Arguments @('-Multiprocess') -Marker 'DAS_IDENTITY_DSL_RESULT=PASS' `
    -FailPattern 'DAS_IDENTITY_DSL_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:'
$Text=Get-Content -LiteralPath $Log -Raw
if($Text -notmatch 'DAS_DSL_PATH_LITERAL'){throw 'Missing path-literal lint warning'}
$Match=[regex]::Match($Text,'DAS_IDENTITY_DSL_RESULT=PASS checks=(\d+)')
if(!$Match.Success -or [int]$Match.Groups[1].Value -ne 42){throw 'Incomplete identity DSL probe'}
if((Get-IdentityHashes) -ne $Before){throw 'Source or binary changed during identity validation'}
[ordered]@{state='PASS';checks=[int]$Match.Groups[1].Value;runtime_log=$Log;sha256=($Before|ConvertFrom-Json);
    mode='headless Game World, Apply defaults, CDO/ref and scalar-state reload';PIE='not_run';packaged='not_run';full_suites='not_run'} |
    ConvertTo-Json -Depth 8 | Set-Content "$($Das.Logs)/identity-dsl-validation.json" -Encoding utf8
Write-Output "DAS_IDENTITY_DSL_VALIDATION=PASS checks=$($Match.Groups[1].Value)"
