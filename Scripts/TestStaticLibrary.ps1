param([string]$Engine='C:\src\unreal\UnrealEngine')
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
& (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12
if($LASTEXITCODE){throw 'Static library editor build failed'}
$ProbeId=[Guid]::NewGuid().ToString('N')
foreach($Phase in @('Create','Reopen')){
 $Log=Join-Path $Repo "logs\static-library-$Phase.log"
 & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasStaticLibrary "-ProbeId=$ProbeId" "-Phase=$Phase" -unattended -NullRHI -nosplash -stdout "-abslog=$Log"
 if($LASTEXITCODE -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_STATIC_LIBRARY_RESULT=PASS')){throw "Static library $Phase failed"}
}
Write-Output 'DAS_STATIC_LIBRARY_PROBE=PASS'
