param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
$Zip=Join-Path $Das.Repo 'Build/Distribution/DasForUE-source.zip'
$Extract=Join-Path $Das.Repo 'Build/DistributionVerify'
& C:\Python312\python.exe (Join-Path $PSScriptRoot 'BuildSourceDistribution.py') --output $Zip --extract $Extract
if($LASTEXITCODE){throw 'Source distribution audit failed'}
$Root=Join-Path $Extract 'Examples/CleanGameplay'
$Project=Join-Path $Root 'CleanGameplay.uproject'
foreach($Name in @('DasIncludeTestFixtures.txt','DasIncludeEditorTests.txt')){if(Test-Path (Join-Path $Root "Config/$Name")){throw 'Example opted into tests'}}
Invoke-DasBuild $Das -Name 'distributed example editor' -BuildArguments @('CleanGameplayEditor','Win64','Development',"-Project=$Project",'-WaitMutex','-NoXGE','-MaxParallelActions=12')
function Run-Example([string]$Exe,[string[]]$Arguments,[string]$Name,[string]$Marker){
    $Log=Join-Path $Das.Logs "$Name.log"
    if(Test-Path -LiteralPath $Log){Remove-Item -LiteralPath $Log}
    $P=Start-Process -FilePath $Exe -WorkingDirectory (Split-Path $Exe) -ArgumentList ($Arguments+@('-unattended','-nosplash',"-abslog=$Log")) -WindowStyle Hidden -PassThru
    if(!$P.WaitForExit(180000)){Stop-Process -Id $P.Id -Force;throw "$Name exceeded timeout"}
    $P.Refresh()
    if($P.ExitCode){throw "$Name exited $($P.ExitCode)"}
    Assert-DasLog -Log $Log -Marker $Marker -FailPattern 'DAS_CLEAN_GAMEPLAY=FAIL|DAS_CLEAN_GAMEPLAY_PREPARE=FAIL|Fatal error:'
}
Run-Example $Das.Editor @($Project,'/Engine/Maps/Entry','-PrepareCleanGameplay','-nullrhi','-ini:Engine:[ConsoleVariables]:HomeScreen.EnableHomeScreen=0') 'clean-gameplay-prepare' 'DAS_CLEAN_GAMEPLAY_PREPARE=PASS'
Run-Example $Das.Editor @($Project,'/Game/CleanGameplay/Arena','-game','-CleanGameplayVerify','-nullrhi') 'clean-gameplay-editor-game' 'DAS_CLEAN_GAMEPLAY=PASS'
$Makefile=Join-Path $Root 'Intermediate/Build/Win64/x64/CleanGameplay/Development/Makefile.bin'
if(Test-Path -LiteralPath $Makefile){Remove-Item -LiteralPath $Makefile}
Invoke-DasBuild $Das -Name 'distributed example game' -BuildArguments @('CleanGameplay','Win64','Development',"-Project=$Project",'-WaitMutex','-NoXGE','-MaxParallelActions=12','-NoUBTMakefiles','-ForceRulesCompile')
$Receipt=Get-Content (Join-Path $Root 'Binaries/Win64/CleanGameplay.target') -Raw | ConvertFrom-Json
$Files=@($Receipt.RuntimeDependencies | ForEach-Object {$_.Path.Replace('\','/')})
if(!($Files | Where-Object {$_ -like '*/Content/Das/Scripts/collectible.das'})){throw 'Gameplay script missing from receipt'}
if($Files | Where-Object {$_ -match '/Tests/(Editor|Runtime)/|/Scripts/(dsl_|ue_probe_actor)'}){throw 'Test dependency in example receipt'}
$Archive=Join-Path $Das.Repo 'Build/CleanGameplayPackaged'
& (Join-Path $Engine 'Engine/Build/BatchFiles/RunUAT.bat') -WaitForUATMutex BuildCookRun "-project=$Project" -noP4 -platform=Win64 -clientconfig=Development -cook -stage -pak -archive "-archivedirectory=$Archive" -map=/Game/CleanGameplay/Arena "-CookDir=$Engine/Engine/Content/BasicShapes" -unattended -utf8output -nullrhi -nocompileeditor -skipbuildeditor -nodebuginfo -AdditionalCookerOptions=-SkipZenStore
if($LASTEXITCODE){throw 'Clean gameplay packaging failed'}
$Manifest=Join-Path $Root 'Saved/StagedBuilds/Windows/Manifest_UFSFiles_Win64.txt'
$Staged=Get-Content $Manifest
if(!($Staged -match 'Content/Das/Scripts/collectible.das')){throw 'Gameplay script not staged'}
foreach($ManifestName in @('Manifest_UFSFiles_Win64.txt','Manifest_NonUFSFiles_Win64.txt')) {
    $Entries=Get-Content (Join-Path $Root "Saved/StagedBuilds/Windows/$ManifestName")
    if($Entries -match '(?i)/Tests/|/Logs/|/Scripts/(dsl_|ue_probe_actor)|\.pdb\s|\.log\s'){throw "Test or diagnostic artifact staged: $ManifestName"}
}
$Exe=Join-Path $Archive 'Windows/CleanGameplay/Binaries/Win64/CleanGameplay.exe'
$Screenshot=Join-Path $Das.Logs 'clean-gameplay.png'
if(Test-Path -LiteralPath $Screenshot){Remove-Item -LiteralPath $Screenshot}
Run-Example $Exe @('/Game/CleanGameplay/Arena','-CleanGameplayVerify','-RenderOffscreen','-windowed','-ResX=960','-ResY=600',"-CleanGameplayScreenshot=$Screenshot") 'clean-gameplay-packaged' 'DAS_CLEAN_GAMEPLAY=PASS'
if(!(Test-Path -LiteralPath $Screenshot)){throw 'Packaged screenshot missing'}
Write-Output "DAS_CLEAN_DISTRIBUTION=PASS archive=$Zip game=$Exe"
