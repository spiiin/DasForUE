param([string]$Engine='C:\src\unreal\UnrealEngine')
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs=Join-Path $Repo 'logs'
New-Item -ItemType Directory -Path $Logs -Force | Out-Null
& (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12
if($LASTEXITCODE){throw 'Delegate editor build failed'}
$ProbeId=[Guid]::NewGuid().ToString('N')
foreach($Phase in @('Create','Reopen')){
 $Log=Join-Path $Logs "authored-delegates-$Phase.log"
 & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasDelegate "-ProbeId=$ProbeId" "-Phase=$Phase" -unattended -NullRHI -nosplash -stdout "-abslog=$Log"
 if($LASTEXITCODE -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_AUTHORED_DELEGATES_RESULT=PASS')){throw "Delegate $Phase failed"}
}
Write-Output 'DAS_AUTHORED_DELEGATES_PROBE=PASS'
