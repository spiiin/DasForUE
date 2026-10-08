param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
$Repo = Split-Path -Parent $PSScriptRoot
$Project = Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs = Join-Path $Repo 'logs'
& (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12
if ($LASTEXITCODE) { throw 'Authoring probe build failed' }
$ProbeId = [Guid]::NewGuid().ToString('N')
try {
foreach ($Phase in @('Create','Reopen')) {
    $Log = Join-Path $Logs "authored-values-asset-$Phase.log"
    & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasFrame -AuthoredValues "-Phase=$Phase" "-ProbeId=$ProbeId" -unattended -NullRHI -nosplash -stdout "-abslog=$Log"
    if ($LASTEXITCODE) { throw "Authoring $Phase failed" }
    if (!(Select-String -LiteralPath $Log -SimpleMatch 'DAS_AUTHORED_VALUES_RESULT=PASS')) { throw "Authoring $Phase missing PASS marker" }
}
} finally {
    $ProbeSource = Join-Path $Repo ('Plugins\DasForUE\Content\Das\Scripts\dsl_authored_values_' + $ProbeId + '.das')
    if (Test-Path -LiteralPath $ProbeSource) { Remove-Item -LiteralPath $ProbeSource }
}
Write-Output 'DAS_AUTHORED_VALUES_ASSET_PROBE=PASS'
