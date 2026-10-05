param([string]$VisualStudio = 'C:\BuildTools\2022')
$ErrorActionPreference = 'Stop'
$Repo = Split-Path -Parent $PSScriptRoot
$Lock = Get-Content -Raw (Join-Path $Repo 'dependencies.json') | ConvertFrom-Json
$Source = Join-Path $Repo 'ThirdParty\daScript'
if (!(Test-Path -LiteralPath $Source)) {
    & git clone $Lock.dascript.url $Source
    if ($LASTEXITCODE) { throw 'daScript clone failed' }
    & git -C $Source checkout --detach $Lock.dascript.commit
    if ($LASTEXITCODE) { throw 'Pinned checkout failed' }
}
$Revision = & git -C $Source rev-parse HEAD
if ($Revision -ne $Lock.dascript.commit) { throw 'daScript revision differs from dependencies.json; refusing to overwrite it' }
$Build = Join-Path $Repo 'Build\daScript'
$Disabled = Select-String -LiteralPath (Join-Path $Source 'CMakeLists.txt') -Pattern '^option\((DAS_[A-Z0-9_]+_DISABLED)' |
    ForEach-Object { '-D' + $_.Matches[0].Groups[1].Value + '=ON' }
& cmake -S $Source -B $Build -G 'Visual Studio 17 2022' -A x64 "-DCMAKE_GENERATOR_INSTANCE=$VisualStudio" `
    "-DCMAKE_SYSTEM_VERSION=$($Lock.windowsSdk)" '-DDAS_USE_STATIC_STD_LIBS=OFF' `
    '-DCMAKE_CXX_FLAGS=/DDAS_NO_GLOBAL_NEW_AND_DELETE=1' @Disabled '-DDAS_TOOLS_DISABLED=OFF'
if ($LASTEXITCODE) { throw 'CMake configure failed' }
& cmake --build $Build --config Release --target libDaScript daslang_static --parallel 12
if ($LASTEXITCODE) { throw 'daScript build failed' }
$Content = Join-Path $Repo 'Plugins\DasForUE\Content\Das'
New-Item -ItemType Directory -Path (Join-Path $Content 'daslib') -Force | Out-Null
Copy-Item -Path (Join-Path $Source 'daslib\*') -Destination (Join-Path $Content 'daslib') -Recurse -Force
$Generated = Join-Path $Repo 'Plugins\DasForUE\Source\DasRuntime\Private\Generated'
New-Item -ItemType Directory -Path $Generated -Force | Out-Null
Push-Location $Source
try {
    & (Join-Path $Source 'bin\Release\daslang_static.exe') 'utils/aot/main.das' -- -aot `
        (Join-Path $Content 'Scripts\aot_smoke.das') (Join-Path $Generated 'SmokeAot.cpp')
    if ($LASTEXITCODE) { throw 'AOT generation failed' }
} finally { Pop-Location }
