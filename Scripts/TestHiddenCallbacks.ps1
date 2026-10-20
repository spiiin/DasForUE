param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){
    Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$($Das.Project)", '-Module=DasRuntime',
    '-Module=DasEditor', '-Module=DasForUESmoke', '-NoEngineChanges', '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=8')
    if($LASTEXITCODE){throw 'Hidden callbacks module build failed'}
}
function Get-HiddenHashes {
    $Paths=@(Get-ChildItem "$($Das.Repo)/Plugins/DasForUE/Source" -Recurse -File |
        Where-Object Extension -in '.h','.cpp','.inl','.cs' | ForEach-Object FullName)
    $Paths+=@(Get-ChildItem "$($Das.Repo)/Plugins/DasForUE/Content/Das/Scripts" -File -Filter '*.das' |
        ForEach-Object FullName)
    $Paths+="$($Das.Repo)/Tests/HiddenCallbacks/probe.das"
    $Paths+=@('DasRuntime','DasEditor' | ForEach-Object {
        "$($Das.Repo)/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$_.dll"
    })
    $Paths+="$Engine/Engine/Binaries/Win64/UnrealEditor.modules"
    $Paths+="$($Das.Repo)/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor.modules"
    $Paths+="$($Das.Repo)/Bindings/FoundationExterns.txt"
    $Paths+="$($Das.Repo)/Scripts/TestHiddenCallbacks.ps1"
    $Result=[ordered]@{}
    foreach($Path in ($Paths | Sort-Object)){$Result[$Path]=(Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash}
    return ($Result | ConvertTo-Json -Compress)
}
$Before=Get-HiddenHashes
$Log=Invoke-DasCommandlet $Das -Run DasHiddenCallbacks -Name 'hidden-callback-runtime' -NoZen `
    -Arguments @('-Multiprocess') -Marker 'DAS_HIDDEN_CALLBACK_RESULT=PASS' `
    -FailPattern 'DAS_HIDDEN_CALLBACK_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:'
$Text=Get-Content -LiteralPath $Log -Raw
$Match=(Get-DasResult -Text $Text -Pattern 'DAS_HIDDEN_CALLBACK_RESULT=PASS checks=(\d+)')
if(!$Match.Success -or [int]$Match.Groups[1].Value -ne 25){throw 'Incomplete hidden callback probe'}
if((Get-HiddenHashes) -ne $Before){throw 'Source or binary changed during hidden callback validation'}
[ordered]@{state='PASS';checks=[int]$Match.Groups[1].Value;runtime_log=$Log;sha256=($Before|ConvertFrom-Json);
    mode='headless Game World, Apply, palette filtering, timers and Enhanced Input reload';PIE='not_run';packaged='not_run';full_suites='not_run'} |
    ConvertTo-Json -Depth 8 | Set-Content "$($Das.Logs)/hidden-callback-validation.json" -Encoding utf8
Write-Output "DAS_HIDDEN_CALLBACK_VALIDATION=PASS checks=$($Match.Groups[1].Value)"
