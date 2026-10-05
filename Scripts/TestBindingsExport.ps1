param([string]$Engine = 'C:\src\unreal\UnrealEngine')
$ErrorActionPreference = 'Stop'
$Repo = Split-Path -Parent $PSScriptRoot
$Stage = Join-Path $Repo 'Build\Bindings'
New-Item -ItemType Directory -Path $Stage -Force | Out-Null
$Selection = Join-Path $Stage 'Unsupported.json'
$Output = Join-Path $Stage 'Rejected.json'
@{ functions = @(@{class='/Script/DasRuntime.DasObjectFixture';function='AddHealth';alias='fixture_AddHealth'},@{class='/Script/Engine.Actor';function='K2_GetComponentsByClass';alias='actor_GetComponentsByClass'}) } | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $Selection -Encoding utf8
[IO.File]::WriteAllText($Output,'previous validated output')
& (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') "$Repo\Tests\DasForUESmoke\DasForUESmoke.uproject" -run=DasExport "-Selection=$Selection" "-Output=$Output" -unattended -NullRHI -nosplash -stdout "-abslog=$Repo\logs\bindings-export-rejected.log"
if ($LASTEXITCODE -ne 1) { throw "Expected exporter failure 1, got $LASTEXITCODE" }
if ([IO.File]::ReadAllText($Output) -cne 'previous validated output') { throw 'Failed export modified output' }
if (!(Select-String -LiteralPath "$Repo\logs\bindings-export-rejected.log" -SimpleMatch 'unsupported UFunction or struct field type')) { throw 'Expected unsupported signature diagnostic' }
Write-Output 'DAS_EXPORT_NEGATIVE_RESULT=PASS'

$global:LASTEXITCODE = 0
