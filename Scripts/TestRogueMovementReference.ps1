param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$PortRoot='C:\src\ActionRoguelikeDas'
$Result=Join-Path $PortRoot 'Saved/Tests/original_movement_reference.json'
if(Test-Path -LiteralPath $Result){Remove-Item -LiteralPath $Result}
$Log=Join-Path $PSScriptRoot '../logs/rogue-original-movement.log'
$Script=(Join-Path $PortRoot 'Scripts/export_original_movement.py').Replace('\','/')
$Command="py __import__('runpy').run_path(r'$Script')"
# Keep original Content, Config and game module; match the available editor plugin set.
$ReferenceProject='C:\src\ActionRoguelike\ActionRoguelikeMovementReference.uproject'
if(Test-Path -LiteralPath $ReferenceProject){throw 'Reference project file already exists'}
$Descriptor=Get-Content -LiteralPath 'C:\src\ActionRoguelike\ActionRoguelike.uproject' -Raw|ConvertFrom-Json
$Descriptor|Add-Member -NotePropertyName DisableEnginePluginsByDefault -NotePropertyValue $true -Force
$Descriptor.Plugins=@('EnhancedInput','Niagara','PythonScriptPlugin','PropertyAccessEditor','AnimationData','ControlRig','Metasound'|ForEach-Object {@{Name=$_;Enabled=$true}})
$Descriptor|ConvertTo-Json -Depth 20|Set-Content -LiteralPath $ReferenceProject -Encoding utf8
try {
    & (Join-Path $Engine 'Engine/Build/BatchFiles/Build.bat') ActionRoguelikeEditor Win64 Development ("-Project="+$ReferenceProject) -WaitMutex -NoXGE -MaxParallelActions=12
    if($LASTEXITCODE -ne 0){throw 'Original module build failed'}
    $Arguments=@(('"'+$ReferenceProject+'"'),'/Engine/Maps/Entry',('-ExecCmds="'+$Command+'"'),'-unattended','-NoSound','-NoZenAutoLaunch','-ddc=NoZenLocalFallback',('-abslog="'+$Log+'"'))
    $Process=Start-Process -FilePath (Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor.exe') -ArgumentList $Arguments -WindowStyle Hidden -PassThru
    if(!$Process.WaitForExit(180000)){Stop-Process -Id $Process.Id -Force;throw 'Original movement export timed out'}
    if($Process.ExitCode -ne 0){throw ('Original movement export exited '+$Process.ExitCode)}
    $Receipt=Get-Content -LiteralPath $Result -Raw|ConvertFrom-Json
    if(!$Receipt.passed){throw $Receipt.error}
    Write-Output 'DAS_ORIGINAL_MOVEMENT_REFERENCE=PASS'
} finally {
    Remove-Item -LiteralPath $ReferenceProject -Force
}
