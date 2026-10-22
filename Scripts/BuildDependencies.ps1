param([string]$VisualStudio = 'C:\BuildTools\2022')
$ErrorActionPreference = 'Stop'
$Repo = Split-Path -Parent $PSScriptRoot
$Lock = Get-Content -Raw (Join-Path $Repo 'dependencies.json') | ConvertFrom-Json
# git and CMake report progress on stderr; Windows PowerShell turns redirected stderr into errors, so native
# tools run with 'Continue' and fail on their exit code only.
function Invoke-Native([string]$Failure, [scriptblock]$Command) {
    $ErrorActionPreference = 'Continue'
    & $Command
    if ($LASTEXITCODE) { throw $Failure }
}
$Source = Join-Path $Repo 'ThirdParty\daScript'
if (!(Test-Path -LiteralPath $Source)) {
    # Clone next to the target and rename only after the pinned checkout, so an interrupted clone is retried.
    $Partial = "$Source.partial"
    if (Test-Path -LiteralPath $Partial) { Remove-Item -LiteralPath $Partial -Recurse -Force }
    Invoke-Native 'daScript clone failed' { git clone --no-checkout $Lock.dascript.url $Partial }
    Invoke-Native 'Pinned checkout failed' { git -C $Partial checkout --detach $Lock.dascript.commit }
    Rename-Item -LiteralPath $Partial -NewName (Split-Path -Leaf $Source)
}
$Revision = & git -C $Source rev-parse HEAD
if ($Revision -ne $Lock.dascript.commit) { throw 'daScript revision differs from dependencies.json; refusing to overwrite it' }
$Build = Join-Path $Repo 'Build\daScript'
$Disabled = Select-String -LiteralPath (Join-Path $Source 'CMakeLists.txt') -Pattern '^option\((DAS_[A-Z0-9_]+_DISABLED)' |
    ForEach-Object { '-D' + $_.Matches[0].Groups[1].Value + '=ON' }
Invoke-Native 'CMake configure failed' {
    cmake -S $Source -B $Build -G 'Visual Studio 17 2022' -A x64 "-DCMAKE_GENERATOR_INSTANCE=$VisualStudio" `
        "-DCMAKE_SYSTEM_VERSION=$($Lock.windowsSdk)" '-DDAS_USE_STATIC_STD_LIBS=OFF' `
        '-DCMAKE_CXX_FLAGS=/DDAS_NO_GLOBAL_NEW_AND_DELETE=1' @Disabled '-DDAS_TOOLS_DISABLED=OFF'
}
Invoke-Native 'daScript build failed' { cmake --build $Build --config Release --target libDaScript daslang_static --parallel 12 }
$Content = Join-Path $Repo 'Plugins\DasForUE\Content\Das'
New-Item -ItemType Directory -Path (Join-Path $Content 'daslib') -Force | Out-Null
Copy-Item -Path (Join-Path $Source 'daslib\*') -Destination (Join-Path $Content 'daslib') -Recurse -Force
$Generated = Join-Path $Repo 'Plugins\DasForUE\Source\DasRuntime\Private\Generated'
New-Item -ItemType Directory -Path $Generated -Force | Out-Null
Push-Location $Source
try {
    Invoke-Native 'AOT generation failed' {
        & (Join-Path $Source 'bin\Release\daslang_static.exe') 'utils/aot/main.das' -- -aot `
            (Join-Path $Content 'Scripts\aot_smoke.das') (Join-Path $Generated 'SmokeAot.cpp')
    }
} finally { Pop-Location }
