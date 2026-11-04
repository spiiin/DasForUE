param([string]$Engine='C:\src\unreal\UnrealEngine', [string]$PortRoot='C:\src\ActionRoguelikeDas', [string]$SourceRoot='C:\src\ActionRoguelike', [switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
# This whole script runs inside the shared agent job. No other job may change
# engine BuildIds between source export, restoring the wrapper build, and PIE.
& (Join-Path $Engine 'Engine/Build/BatchFiles/Build.bat') ActionRoguelikeEditor Win64 Development "-Project=$SourceRoot\ActionRoguelikeComparison.uproject" -EnablePlugin=PropertyAccessNode -WaitMutex -NoXGE -MaxParallelActions=12
if($LASTEXITCODE) { throw 'Original editor build failed' }
$Temporary=Join-Path $SourceRoot 'Content/DasRogueExternal/ABP_CoursePlayer.uasset'
# Remove only the known generated staging asset left by an interrupted export.
if(Test-Path -LiteralPath $Temporary) { Remove-Item -LiteralPath $Temporary -Force }
$Receipt=Join-Path $PortRoot 'Saved/ExternalAssets/course_animation.json'
if(Test-Path -LiteralPath $Receipt) { Remove-Item -LiteralPath $Receipt }
$Bootstrap=(Join-Path $PortRoot 'Scripts/run_course_animation_export.py').Replace('\','/')
$Log=Join-Path $Das.Logs 'rogue-course-animation-export.log'
$Command="py __import__('runpy').run_path(r'$Bootstrap')"
$EditorArguments=@(('"'+(Join-Path $SourceRoot 'ActionRoguelikeComparison.uproject')+'"'),'/Game/ActionRoguelike/Maps/P_OpenWorldTestMap','-EnablePlugins=PropertyAccessNode','-Multiprocess','-unattended','-NoZenAutoLaunch','-ddc=NoZenLocalFallback',('-abslog="'+$Log+'"'),('-ExecCmds="'+$Command+'"'))
$Process=Start-Process -FilePath (Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor.exe') -ArgumentList $EditorArguments -WindowStyle Hidden -PassThru
$Deadline=[DateTime]::UtcNow.AddSeconds(300)
while(!$Process.WaitForExit(1000)) {
    if([DateTime]::UtcNow -gt $Deadline) { Stop-Process -Id $Process.Id -Force; throw 'Source animation export timed out' }
}
if(!(Test-Path -LiteralPath $Receipt)) { throw "No exported animation receipt; see $Log" }
if(!(Select-String -LiteralPath $Log -SimpleMatch 'DAS_COURSE_ANIMATION_EXPORT_PASS')) { throw "Animation export failed; see $Log" }
if(Select-String -LiteralPath $Log -Pattern 'Ensure condition failed:|Unhandled Exception:|Fatal error:') { throw "Source export diagnostic; see $Log" }
if(Test-Path -LiteralPath $Temporary) { throw 'Source-project staging asset was not cleaned up' }
Invoke-DasBuild $Das -Name 'Restore wrapper after original animation export'
& (Join-Path $PSScriptRoot 'TestRogueActions.ps1') -Engine $Engine -PortRoot $PortRoot -SkipBuild -Scenario course_animation
