param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Result='C:\src\ActionRoguelikeDas\Saved\Tests\original_ai_reference.json'
if(Test-Path -LiteralPath $Result){Remove-Item -LiteralPath $Result}
$Project='C:\src\ActionRoguelike\ActionRoguelikeComparison.uproject'
& (Join-Path $Engine 'Engine/Build/BatchFiles/Build.bat') ActionRoguelikeEditor Win64 Development "-Project=$Project" -EnablePlugin=PropertyAccessNode -WaitMutex -NoXGE -MaxParallelActions=12
if($LASTEXITCODE -ne 0){throw 'Original AI reference build failed'}
$Command="py __import__('runpy').run_path(r'C:/src/ActionRoguelikeDas/Scripts/export_original_ai.py')"
$Arguments=@(('"'+$Project+'"'),'/Engine/Maps/Entry',('-ExecCmds="'+$Command+'"'),'-EnablePlugins=PropertyAccessNode','-unattended','-NoSound','-NoZenAutoLaunch','-ddc=NoZenLocalFallback','-abslog="C:\src\ActionRoguelike\LocalComparison\AIReference.log"')
$Process=Start-Process -FilePath (Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor.exe') -ArgumentList $Arguments -WindowStyle Hidden -PassThru
if(!$Process.WaitForExit(240000)){Stop-Process -Id $Process.Id -Force;throw 'AI export timeout'}
if($Process.ExitCode -ne 0){throw 'AI reference editor failed'}
$Receipt=Get-Content -LiteralPath 'C:\src\ActionRoguelikeDas\Saved\Tests\original_ai_reference.json' -Raw | ConvertFrom-Json
if(!$Receipt.passed){throw $Receipt.error}
Write-Output 'DAS_ORIGINAL_AI_REFERENCE=PASS'
