param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){
    & "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$($Das.Project)" `
        -Module=DasRuntime -Module=DasEditor -WaitMutex -NoXGE -MaxParallelActions=8
    if($LASTEXITCODE){throw 'Inline callback module build failed'}
}
function Get-CallbackHashes {
    $Paths=@(Get-ChildItem "$($Das.Repo)/Plugins/DasForUE/Source" -Recurse -File |
        Where-Object Extension -in '.h','.cpp','.inl','.cs' | ForEach-Object FullName)
    $Paths+=@(Get-ChildItem "$($Das.Repo)/Plugins/DasForUE/Content/Das/Scripts" -File -Filter '*.das' |
        ForEach-Object FullName)
    $Paths+="$($Das.Repo)/Tests/InlineCallbacks/probe.template.das"
    $Paths+=@('DasRuntime','DasEditor' | ForEach-Object {
        "$($Das.Repo)/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$_.dll"
    })
    $Paths+="$Engine/Engine/Binaries/Win64/UnrealEditor.modules"
    $Result=[ordered]@{}
    foreach($Path in ($Paths | Sort-Object)){$Result[$Path]=(Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash}
    return ($Result | ConvertTo-Json -Compress)
}
$Before=Get-CallbackHashes
$Log=Invoke-DasCommandlet $Das -Run DasInlineCallbacks -Name 'inline-callback-runtime' -NoZen `
    -Arguments @('-Multiprocess') -Marker 'DAS_INLINE_CALLBACK_RESULT=PASS' `
    -FailPattern 'DAS_INLINE_CALLBACK_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:'
$Text=Get-Content -LiteralPath $Log -Raw
$Match=[regex]::Match($Text,'DAS_INLINE_CALLBACK_RESULT=PASS checks=(\d+)')
if(!$Match.Success -or [int]$Match.Groups[1].Value -ne 26){throw 'Incomplete inline callback probe'}
if((Get-CallbackHashes) -ne $Before){throw 'Source or binary changed during callback validation'}
[ordered]@{state='PASS';checks=[int]$Match.Groups[1].Value;runtime_log=$Log;sha256=($Before|ConvertFrom-Json);
    mode='headless Game World, real timers and EnhancedPlayerInput';PIE='not_run';packaged='not_run';full_suites='not_run'} |
    ConvertTo-Json -Depth 8 | Set-Content "$($Das.Logs)/inline-callback-validation.json" -Encoding utf8
Write-Output "DAS_INLINE_CALLBACK_VALIDATION=PASS checks=$($Match.Groups[1].Value)"
