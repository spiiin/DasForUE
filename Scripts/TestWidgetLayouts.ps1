param([string]$Engine='C:/src/unreal/UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das -Name 'widget-layout' -Parallel 8}
# Freeze the source tree and binaries used by this narrowly scoped commandlet.
function Get-LayoutHashes {
    $Paths=@(Get-ChildItem "$($Das.Repo)/Plugins/DasForUE/Source" -Recurse -File |
        Where-Object Extension -in '.h','.cpp','.inl','.cs' | ForEach-Object FullName)
    $Paths+=@(Get-ChildItem "$($Das.Repo)/Plugins/DasForUE/Content/Das/Scripts" -File -Filter '*.das' |
        ForEach-Object FullName)
    $Paths+=@('DasRuntime','DasEditor' | ForEach-Object {
        "$($Das.Repo)/Tests/DasForUESmoke/Binaries/Win64/UnrealEditor-$_.dll"
    })
    $Paths+="$Engine/Engine/Binaries/Win64/UnrealEditor.modules"
    $Result=[ordered]@{}
    foreach($Path in ($Paths | Sort-Object)){$Result[$Path]=(Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash}
    return ($Result | ConvertTo-Json -Compress)
}
$Before=Get-LayoutHashes
$Log=Invoke-DasCommandlet $Das -Run DasWidgetLayouts -Name 'widget-layout-runtime' -NoZen `
    -Arguments @('-Multiprocess') -Marker 'DAS_WIDGET_LAYOUT_RESULT=PASS' `
    -FailPattern 'DAS_WIDGET_LAYOUT_FAIL|Ensure condition failed|Fatal error:|Log[^:]+: Error:'
$Text=Get-Content -LiteralPath $Log -Raw
$Match=[regex]::Match($Text,'DAS_WIDGET_LAYOUT_RESULT=PASS checks=(\d+)')
if(!$Match.Success -or [int]$Match.Groups[1].Value -ne 30){throw 'Incomplete widget-layout probe'}
if((Get-LayoutHashes) -ne $Before){throw 'Source or binary changed during widget-layout validation'}
[ordered]@{state='PASS';checks=[int]$Match.Groups[1].Value;runtime_log=$Log;sha256=($Before|ConvertFrom-Json);
    mode='headless UMG with NullSlate';PIE='not_run';packaged='not_run';full_suites='not_run'} |
    ConvertTo-Json -Depth 8 | Set-Content "$($Das.Logs)/widget-layout-validation.json" -Encoding utf8
Write-Output "DAS_WIDGET_LAYOUT_VALIDATION=PASS checks=$($Match.Groups[1].Value)"
