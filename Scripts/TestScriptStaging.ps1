param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Output=Join-Path $Repo 'Build/ScriptStagingTest'
New-Item -ItemType Directory -Path $Output -Force | Out-Null
$Rules=Get-Content (Join-Path $Repo 'Plugins/DasForUE/Source/DasRuntime/DasRuntime.Build.cs') -Raw
$Start=$Rules.IndexOf('public static class DasScriptStaging')
if($Start -lt 0){throw 'Staging selector missing'}
[IO.File]::WriteAllText((Join-Path $Output 'Selector.cs'),"using System.IO;`n"+$Rules.Substring($Start))
Copy-Item (Join-Path $Repo 'Tests/ScriptStaging/Program.cs') (Join-Path $Output 'Program.cs') -Force
[IO.File]::WriteAllText((Join-Path $Output 'Staging.csproj'),'<Project Sdk="Microsoft.NET.Sdk"><PropertyGroup><OutputType>Exe</OutputType><TargetFramework>net10.0</TargetFramework><EnableDefaultCompileItems>true</EnableDefaultCompileItems></PropertyGroup></Project>')
[IO.File]::WriteAllText((Join-Path $Output 'NuGet.Config'),'<configuration><packageSources><clear /></packageSources></configuration>')
$DotNet=Join-Path $Engine 'Engine/Binaries/ThirdParty/DotNet/10.0/win-x64/dotnet.exe'
& $DotNet run --project (Join-Path $Output 'Staging.csproj') -- $Repo
if($LASTEXITCODE){throw 'Script staging contract failed'}

$ReceiptPath=Join-Path $Repo 'Tests/DasForUESmoke/Binaries/Win64/DasForUESmokeEditor.target'
if(Test-Path -LiteralPath $ReceiptPath) {
    $Receipt=Get-Content -LiteralPath $ReceiptPath -Raw | ConvertFrom-Json
    $Expected=@(Get-Content (Join-Path $Repo 'logs/staging-smoke-files.txt') | ForEach-Object { [IO.Path]::GetFullPath($_) })
    $ProjectRoot=Join-Path $Repo 'Tests/DasForUESmoke'
    $PluginRoot=Join-Path $Repo 'Plugins/DasForUE'
    $Prefix=[IO.Path]::GetFullPath((Join-Path $PluginRoot 'Content/Das'))+[IO.Path]::DirectorySeparatorChar
    $Actual=@($Receipt.RuntimeDependencies | ForEach-Object {
        $Path=$_.Path.Replace('$(ProjectDir)',$ProjectRoot).Replace('$(EngineDir)',(Join-Path $Engine 'Engine')).Replace('$(PluginDir)',$PluginRoot)
        if($Path -notmatch '\$\(') { [IO.Path]::GetFullPath($Path) }
    } | Where-Object { $_.StartsWith($Prefix,[StringComparison]::OrdinalIgnoreCase) })
    if(Compare-Object ($Expected | Sort-Object -Unique) ($Actual | Sort-Object -Unique)) {throw 'UBT receipt differs from staging selector'}
    Write-Output "DAS_STAGING_RECEIPT=PASS files=$($Actual.Count)"
} else { Write-Output 'DAS_STAGING_RECEIPT=NOT_RUN (build the Editor target first)' }
