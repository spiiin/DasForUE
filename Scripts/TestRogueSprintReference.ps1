param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Result='C:\src\ActionRoguelikeDas\Saved\Tests\original_sprint_profile.json'
if(Test-Path -LiteralPath $Result){Remove-Item -LiteralPath $Result}
$Project='C:\src\ActionRoguelike\ActionRoguelikeComparison.uproject'
& (Join-Path $Engine 'Engine/Build/BatchFiles/Build.bat') ActionRoguelikeEditor Win64 Development "-Project=$Project" -EnablePlugin=PropertyAccessNode -WaitMutex -NoXGE -MaxParallelActions=12
if($LASTEXITCODE -ne 0){throw 'Original world reference build failed'}
$Command="py __import__('runpy').run_path(r'C:/src/ActionRoguelikeDas/Scripts/read_original_sprint_profile.py')"
$Arguments=@(('"'+$Project+'"'),'/Game/ActionRoguelike/Maps/P_OpenWorldTestMap',('-ExecCmds="'+$Command+'"'),'-EnablePlugins=PropertyAccessNode','-unattended','-NoSound','-NoZenAutoLaunch','-ddc=NoZenLocalFallback','-abslog="C:\src\ActionRoguelike\LocalComparison\SprintReference.log"')
$Process=Start-Process -FilePath (Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor.exe') -ArgumentList $Arguments -WindowStyle Hidden -PassThru
if(!$Process.WaitForExit(360000)){Stop-Process -Id $Process.Id -Force;throw 'world export timeout'}
if($Process.ExitCode -ne 0){throw 'world reference editor failed'}
$Receipt=Get-Content -LiteralPath 'C:\src\ActionRoguelikeDas\Saved\Tests\original_sprint_profile.json' -Raw | ConvertFrom-Json
if(!$Receipt.passed){throw $Receipt.error}
Write-Output 'DAS_ORIGINAL_SPRINT_PROFILE_REFERENCE=PASS'
