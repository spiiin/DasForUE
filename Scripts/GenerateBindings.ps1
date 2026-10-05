param([string]$Engine = 'C:\src\unreal\UnrealEngine', [string]$Python = 'C:\Python312\python.exe', [switch]$Check)
$ErrorActionPreference = 'Stop'
$Repo = Split-Path -Parent $PSScriptRoot
$Project = Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Stage = Join-Path $Repo 'Build\Bindings'
New-Item -ItemType Directory -Path $Stage -Force | Out-Null
$IR = Join-Path $Stage 'Api.json'
$Module = Join-Path $Stage 'ue_generated.das'
& (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12
if ($LASTEXITCODE) { throw 'Binding exporter build failed' }
& (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasExport "-Selection=$Repo\Bindings\Selection.json" "-Output=$IR" -unattended -NullRHI -nosplash -stdout "-abslog=$Repo\logs\bindings-export.log"
if ($LASTEXITCODE) { throw 'Reflection export failed' }
& $Python (Join-Path $PSScriptRoot 'GenerateBindings.py') $IR $Module
if ($LASTEXITCODE) { throw 'Binding generation failed' }
$CommittedIR = Join-Path $Repo 'Bindings\Api.json'
$CommittedModule = Join-Path $Repo 'Plugins\DasForUE\Content\Das\Scripts\ue_generated.das'
if ($Check) {
    foreach ($Pair in @(@($IR,$CommittedIR),@($Module,$CommittedModule))) {
        if (!(Test-Path -LiteralPath $Pair[1]) -or [IO.File]::ReadAllText($Pair[0]).Replace("`r`n","`n") -cne [IO.File]::ReadAllText($Pair[1]).Replace("`r`n","`n")) {
            throw "Generated binding file is stale: $($Pair[1])"
        }
    }
} else {
    # Validate both staged artifacts before replacing either checked-in file.
    Copy-Item -LiteralPath $IR -Destination $CommittedIR
    Copy-Item -LiteralPath $Module -Destination $CommittedModule
}
Write-Output 'DAS_BINDINGS_RESULT=PASS'
