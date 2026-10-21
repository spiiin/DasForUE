param([string]$Engine='C:\src\unreal\UnrealEngine')
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
Invoke-DasBuild (Get-DasContext -Engine $Engine) -BuildArguments @(
    'DasForUESmokeEditor', 'Win64', 'Development', "-Project=$Project", '-WaitMutex', '-NoXGE',
    '-MaxParallelActions=12')
if($LASTEXITCODE){throw 'Static library editor build failed'}
$ProbeId=[Guid]::NewGuid().ToString('N')
foreach($Phase in @('Create','Reopen')){
 $Log=Join-Path $Repo "logs\static-library-$Phase.log"
 & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasStaticLibrary "-ProbeId=$ProbeId" "-Phase=$Phase" -unattended -NullRHI -nosplash -stdout "-abslog=$Log"
 if($LASTEXITCODE -or !(Test-DasResult -LiteralPath $Log -SimpleMatch 'DAS_STATIC_LIBRARY_RESULT=PASS')){throw "Static library $Phase failed"}
}
Write-Output 'DAS_STATIC_LIBRARY_PROBE=PASS'
