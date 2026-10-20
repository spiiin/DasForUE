param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){
    & "$Engine/Engine/Build/BatchFiles/Build.bat" DasForUESmokeEditor Win64 Development "-Project=$($Das.Project)" `
        -Module=DasRuntime -Module=DasEditor -Module=DasForUESmoke -NoEngineChanges -WaitMutex -NoXGE -MaxParallelActions=8
    if($LASTEXITCODE){throw 'Handler references module build failed'}
}
function Get-HandlerReferenceHashes {
    $Paths=@(Get-ChildItem "$($Das.Repo)/Plugins/DasForUE/Source" -Recurse -File |
        Where-Object Extension -in '.h','.cpp','.inl','.cs' | ForEach-Object FullName)
    $Paths+=@(Get-ChildItem "$($Das.Repo)/Plugins/DasForUE/Content/Das/Scripts" -File -Filter '*.das' |
        ForEach-Object FullName)
    $Paths+="$($Das.Repo)/Tests/HandlerReferences/authored.das"
    $Paths+="$($Das.Repo)/Tests/HandlerReferences/free.das"
    $Paths+="$($Das.Repo)/Tests/DasForUESmoke/Config/DefaultEngine.ini"
    $Paths+=@('DasRuntime','DasEditor' | ForEach-Object {
        "$($Das.Repo)/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$_.dll"
    })
    $Paths+="$Engine/Engine/Binaries/Win64/UnrealEditor.modules"
    $Paths+="$($Das.Repo)/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor.modules"
    $Paths+="$($Das.Repo)/Bindings/FoundationExterns.txt"
    $Paths+="$($Das.Repo)/Scripts/TestHandlerReferences.ps1"
    $Result=[ordered]@{}
    foreach($Path in ($Paths | Sort-Object)){$Result[$Path]=(Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash}
    return ($Result | ConvertTo-Json -Compress)
}
$Before=Get-HandlerReferenceHashes
$Log=Invoke-DasCommandlet $Das -Run DasHandlerReferences -Name 'handler-references-runtime' -NoZen `
    -Arguments @('-Multiprocess') -Marker 'DAS_HANDLER_REF_RESULT=PASS' `
    -FailPattern 'DAS_HANDLER_REF_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:'
$Text=Get-Content -LiteralPath $Log -Raw
$Match=[regex]::Match($Text,'DAS_HANDLER_REF_RESULT=PASS checks=(\d+)')
if(!$Match.Success -or [int]$Match.Groups[1].Value -ne 34){throw 'Incomplete handler references probe'}
if((Get-HandlerReferenceHashes) -ne $Before){throw 'Source or binary changed during handler references validation'}
[ordered]@{state='PASS';checks=[int]$Match.Groups[1].Value;runtime_log=$Log;sha256=($Before|ConvertFrom-Json);
    mode='headless Game World, typed method/free references, delegate ABI, Apply, GC and reload leases';PIE='not_run';packaged='not_run';full_suites='not_run'} |
    ConvertTo-Json -Depth 8 | Set-Content "$($Das.Logs)/handler-references-validation.json" -Encoding utf8
Write-Output "DAS_HANDLER_REF_VALIDATION=PASS checks=$($Match.Groups[1].Value)"
