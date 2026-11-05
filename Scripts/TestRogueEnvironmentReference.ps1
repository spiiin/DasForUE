param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Result='C:\src\ActionRoguelikeDas\Saved\Tests\original_environment/reference.json'
if(Test-Path -LiteralPath $Result){Remove-Item -LiteralPath $Result}
$Project='C:\src\ActionRoguelike\ActionRoguelikeComparison.uproject'
& (Join-Path $Engine 'Engine/Build/BatchFiles/Build.bat') ActionRoguelikeEditor Win64 Development "-Project=$Project" -EnablePlugin=PropertyAccessNode -WaitMutex -NoXGE -MaxParallelActions=12
if($LASTEXITCODE -ne 0){throw 'Original environment reference build failed'}
$Command="py __import__('runpy').run_path(r'C:/src/ActionRoguelikeDas/Scripts/export_original_environment.py')"
$Arguments=@(('"'+$Project+'"'),'/Engine/Maps/Entry',('-ExecCmds="'+$Command+'"'),'-EnablePlugins=PropertyAccessNode','-unattended','-NoSound','-NoZenAutoLaunch','-ddc=NoZenLocalFallback','-abslog="C:\src\ActionRoguelike\LocalComparison\EnvironmentReference.log"')
$Process=Start-Process -FilePath (Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor.exe') -ArgumentList $Arguments -WindowStyle Hidden -PassThru
if(!$Process.WaitForExit(240000)){Stop-Process -Id $Process.Id -Force;throw 'environment export timeout'}
if($Process.ExitCode -ne 0){throw 'environment reference editor failed'}
$Receipt=Get-Content -LiteralPath 'C:\src\ActionRoguelikeDas\Saved\Tests\original_environment/reference.json' -Raw | ConvertFrom-Json
if(!$Receipt.passed){throw $Receipt.error}
Write-Output 'DAS_ORIGINAL_ENVIRONMENT_REFERENCE=PASS'
