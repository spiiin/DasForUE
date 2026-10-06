param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
$Repo = Split-Path -Parent $PSScriptRoot
$Project = Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs = Join-Path $Repo 'logs'
& (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12
if ($LASTEXITCODE) { throw 'Authoring probe build failed' }
$ProbeId = [Guid]::NewGuid().ToString('N')
foreach ($Phase in @('Create','Reopen')) {
    $Log = Join-Path $Logs "dsl-$Phase.log"
    & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasAuthoring "-Phase=$Phase" "-ProbeId=$ProbeId" "-First=$Repo\Plugins\DasForUE\Content\Das\Scripts\dsl_counter_v1.das" "-Second=$Repo\Plugins\DasForUE\Content\Das\Scripts\dsl_counter_v2.das" -unattended -NullRHI -nosplash -stdout "-abslog=$Log"
    if ($LASTEXITCODE) { throw "Authoring $Phase failed" }
    if (!(Select-String -LiteralPath $Log -SimpleMatch 'DAS_AUTHORING_RESULT=PASS')) { throw "Authoring $Phase missing PASS marker" }
}
Write-Output 'DAS_DSL_PROBE=PASS'
