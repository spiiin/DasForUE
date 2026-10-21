param([string]$Engine = 'C:\src\unreal\UnrealEngine',[string[]]$AdditionalMaps=@(),[switch]$SkipZenStore,[string]$DDC='',[string[]]$AdditionalCookDirectories=@(),[switch]$SkipSmoke,[ValidateSet('Development','Shipping')][string]$Configuration='Development',[string]$ArchiveDirectory='')
$ErrorActionPreference = 'Stop'
$DDCArgs=@();if($DDC){$DDCArgs=@("-DDC=$DDC")}
$Repo = Split-Path -Parent $PSScriptRoot
$Project = Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs = Join-Path $Repo 'logs'
$Archive = if($ArchiveDirectory){$ArchiveDirectory}else{Join-Path $Repo 'Build\Packaged'}
New-Item -ItemType Directory -Path $Logs -Force | Out-Null
# UE 5.8.3 BuildMode's synchronous fallback still loads Makefile.bin with -NoUBTMakefiles.
# Rebuild only this disposable game-target cache so RuntimeDependencies globs include new scripts.
$TargetMakefile = Join-Path $Repo "Tests\DasForUESmoke\Intermediate\Build\Win64\x64\DasForUESmoke\$Configuration\Makefile.bin"
if (Test-Path -LiteralPath $TargetMakefile -PathType Leaf) {
    Remove-Item -LiteralPath $TargetMakefile
}
$BuildLog=if($Configuration -eq 'Development'){Join-Path $Logs 'build-packaged.log'}else{Join-Path $Logs "build-packaged-$Configuration.log"}
# BuildConfiguration switches must be top-level UBT arguments, not nested UAT -Target arguments.
& (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmoke Win64 $Configuration `
    "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12 -NoUBTMakefiles -ForceRulesCompile "-Log=$BuildLog"
if ($LASTEXITCODE) { throw 'Packaged target build failed' }
$Binary = if($Configuration -eq 'Shipping'){'DasForUESmoke-Win64-Shipping'}else{'DasForUESmoke'}
$Receipt = Get-Content -LiteralPath (Join-Path $Repo "Tests\DasForUESmoke\Binaries\Win64\$Binary.target") -Raw | ConvertFrom-Json
$DeclaredFiles = @($Receipt.RuntimeDependencies | ForEach-Object { $_.Path })
foreach ($Script in Get-ChildItem -LiteralPath (Join-Path $Repo 'Plugins\DasForUE\Content\Das\Scripts') -Filter '*.das' -File -Recurse) {
    if ($DeclaredFiles -notcontains $Script.FullName) { throw "Script missing from build receipt: $($Script.FullName)" }
}
# The game is already built above; cook/stage/archive must consume that verified receipt.
$Maps=(@('/Engine/Maps/Entry')+$AdditionalMaps) -join '+'
$CookDirArgs=@();if($AdditionalCookDirectories.Count){$CookDirArgs=@('-CookDir=' + ($AdditionalCookDirectories -join '+'))}
$CookerArgs=@();if($SkipZenStore){$CookerArgs=@('-AdditionalCookerOptions=-SkipZenStore')}
& (Join-Path $Engine 'Engine\Build\BatchFiles\RunUAT.bat') -WaitForUATMutex BuildCookRun "-project=$Project" `
    -noP4 -platform=Win64 "-clientconfig=$Configuration" -cook -stage -pak -archive `
    "-archivedirectory=$Archive" "-map=$Maps" -unattended -utf8output -nullrhi `
    -nocompileeditor -skipbuildeditor -nodebuginfo @CookerArgs @DDCArgs @CookDirArgs
if ($LASTEXITCODE) { throw 'Packaging failed' }
$Executable = Join-Path $Archive "Windows\DasForUESmoke\Binaries\Win64\$Binary.exe"
if (!(Test-Path -LiteralPath $Executable)) { throw "Packaged executable missing: $Executable" }
if ($SkipSmoke) { return }
$Log = Join-Path $Logs 'smoke-packaged.log'
$Process = Start-Process -FilePath $Executable -ArgumentList '-DasSmoke','-nullrhi','-unattended','-nosplash',"-abslog=$Log" `
    -WindowStyle Hidden -PassThru -Wait
if ($Process.ExitCode) { throw "Packaged smoke failed: $($Process.ExitCode)" }
if (!(Select-String -LiteralPath $Log -SimpleMatch 'DAS_FOUNDATION_RESULT=PASS')) {
    throw 'Packaged game exited without a PASS marker'
}
