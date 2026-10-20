param([string]$Engine = 'C:\src\unreal\UnrealEngine',[string[]]$AdditionalMaps=@(),[switch]$SkipZenStore,[string]$DDC='')
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference = 'Stop'
$DDCArgs=@();if($DDC){$DDCArgs=@("-DDC=$DDC")}
$Repo = Split-Path -Parent $PSScriptRoot
$Project = Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs = Join-Path $Repo 'logs'
$Archive = Join-Path $Repo 'Build\Packaged'
New-Item -ItemType Directory -Path $Logs -Force | Out-Null
# UE 5.8.3 BuildMode's synchronous fallback still loads Makefile.bin with -NoUBTMakefiles.
# Rebuild only this disposable game-target cache so RuntimeDependencies globs include new scripts.
$TargetMakefile = Join-Path $Repo 'Tests\DasForUESmoke\Intermediate\Build\Win64\x64\DasForUESmoke\Development\Makefile.bin'
if (Test-Path -LiteralPath $TargetMakefile -PathType Leaf) {
    Remove-Item -LiteralPath $TargetMakefile
}
# BuildConfiguration switches must be top-level UBT arguments, not nested UAT -Target arguments.
Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmoke', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=12', '-NoUBTMakefiles', '-ForceRulesCompile', "-Log=$Logs\build-packaged.log")
if ($LASTEXITCODE) { throw 'Packaged target build failed' }
$Receipt = Get-Content -LiteralPath (Join-Path $Repo 'Tests\DasForUESmoke\Binaries\Win64\DasForUESmoke.target') -Raw | ConvertFrom-Json
$DeclaredFiles = @($Receipt.RuntimeDependencies | ForEach-Object { $_.Path })
foreach ($Script in Get-ChildItem -LiteralPath (Join-Path $Repo 'Plugins\DasForUE\Content\Das\Scripts') -Filter '*.das' -File -Recurse) {
    if ($DeclaredFiles -notcontains $Script.FullName) { throw "Script missing from build receipt: $($Script.FullName)" }
}
# The game is already built above; cook/stage/archive must consume that verified receipt.
$Maps=(@('/Engine/Maps/Entry')+$AdditionalMaps) -join '+'
$CookerArgs=@();if($SkipZenStore){$CookerArgs=@('-AdditionalCookerOptions=-SkipZenStore')}
& (Join-Path $Engine 'Engine\Build\BatchFiles\RunUAT.bat') -WaitForUATMutex BuildCookRun "-project=$Project" `
    -noP4 -platform=Win64 -clientconfig=Development -cook -stage -pak -archive `
    "-archivedirectory=$Archive" "-map=$Maps" -unattended -utf8output -nullrhi `
    -nocompileeditor -skipbuildeditor -nodebuginfo @CookerArgs @DDCArgs
if ($LASTEXITCODE) { throw 'Packaging failed' }
$Executable = Join-Path $Archive 'Windows\DasForUESmoke\Binaries\Win64\DasForUESmoke.exe'
if (!(Test-Path -LiteralPath $Executable)) { throw "Packaged executable missing: $Executable" }
$Log = Join-Path $Logs 'smoke-packaged.log'
$Process = Start-Process -FilePath $Executable -ArgumentList '-DasSmoke','-nullrhi','-unattended','-nosplash',"-abslog=$Log" `
    -WindowStyle Hidden -PassThru -Wait
if ($Process.ExitCode) { throw "Packaged smoke failed: $($Process.ExitCode)" }
if (!(Test-DasResult -LiteralPath $Log -SimpleMatch 'DAS_FOUNDATION_RESULT=PASS')) {
    throw 'Packaged game exited without a PASS marker'
}
