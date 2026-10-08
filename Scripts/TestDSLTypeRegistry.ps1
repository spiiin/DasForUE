param([string]$Engine = 'C:\src\unreal\UnrealEngine', [switch]$Cooked, [switch]$EnumAppend, [switch]$EnumGraphs, [switch]$EnumNodes, [switch]$EnumRename, [switch]$EnumValues, [switch]$StructFields, [switch]$StructDrop, [switch]$StructNumeric, [switch]$StructNested, [switch]$AuthoredNested, [switch]$StructArray, [switch]$AuthoredArray, [switch]$PrimitiveArray, [switch]$AuthoredMap, [switch]$StringMap, [switch]$AuthoredSet, [switch]$ScalarMap, [switch]$ContainerCalls, [switch]$OutRef, [switch]$Interfaces, [switch]$AuthoredInterface, [switch]$TypedObjects, [switch]$TypeSyntax)
$ErrorActionPreference = 'Stop'
$Repo = Split-Path -Parent $PSScriptRoot
$Project = Join-Path $Repo 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs = Join-Path $Repo 'logs'
New-Item -ItemType Directory -Path $Logs -Force | Out-Null
& (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmokeEditor Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12
if ($LASTEXITCODE) { throw 'Registry editor build failed' }
$ProbeId = [Guid]::NewGuid().ToString('N')
$ModeArgs = @(); if ($TypeSyntax) { $ModeArgs += '-TypeSyntax' } if ($TypedObjects) { $ModeArgs += '-TypedObjects' } if ($AuthoredInterface) { $ModeArgs += '-AuthoredInterface' }; if ($Interfaces) { $AuthoredArray = $true; $ModeArgs += '-Interfaces' }; if ($OutRef) { $AuthoredArray = $true; $ModeArgs += '-OutRef' }; if ($ContainerCalls) { $AuthoredArray = $true; $ModeArgs += '-ContainerCalls' }; if ($ScalarMap) { $StringMap = $true; $ModeArgs += '-ScalarMap' }; if ($AuthoredSet) { $AuthoredArray = $true; $ModeArgs += '-AuthoredSet' }; if ($StringMap) { $AuthoredMap = $true; $ModeArgs += '-StringMap' }; if ($AuthoredMap) { $AuthoredArray = $true; $ModeArgs += '-AuthoredMap' }; if ($PrimitiveArray) { $AuthoredArray = $true; $ModeArgs += '-PrimitiveArray' }; if ($AuthoredArray) { $AuthoredNested = $true; $ModeArgs += '-AuthoredArray' }; if ($StructArray) { $StructNested = $true; $ModeArgs += '-StructArray' }; if ($AuthoredNested) { $ModeArgs += '-AuthoredNested' }; if ($StructNested) { $StructFields = $true; $ModeArgs += '-StructNested' }; if ($StructNumeric) { if ($StructDrop) { throw 'Numeric conversion and drop are separate probes' }; $StructFields = $true; $ModeArgs += '-StructNumeric' }; if ($StructDrop) { $StructFields = $true; $ModeArgs += '-StructDrop' }; if ($EnumAppend) { $ModeArgs += '-EnumAppend' }; if ($EnumGraphs) { $ModeArgs += '-EnumGraphs' }; if ($EnumNodes) { $ModeArgs += '-EnumNodes' }; if ($EnumRename) { $ModeArgs += '-EnumRename' }; if ($EnumValues) { $ModeArgs += '-EnumValues' }; if ($StructFields) { $ModeArgs += '-StructFields' }
$Base = '/Game/DasFrameAssetProbe/Registered_' + $ProbeId
try {
    foreach ($Phase in @('Create','Reopen')) {
        $Log = Join-Path $Logs "type-registry-$Phase.log"
        & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasFrame -RegisteredTypes @ModeArgs "-Phase=$Phase" "-ProbeId=$ProbeId" -unattended -NullRHI -nosplash -stdout "-abslog=$Log"
        if ($LASTEXITCODE -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_REGISTERED_TYPES_RESULT=PASS')) { throw "Registry $Phase failed" }
    }
    if ($Cooked) {
        $Cache = Join-Path $Repo 'Tests\DasForUESmoke\Intermediate\Build\Win64\x64\DasForUESmoke\Development\Makefile.bin'
        if (Test-Path -LiteralPath $Cache) { Remove-Item -LiteralPath $Cache }
        & (Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat') DasForUESmoke Win64 Development "-Project=$Project" -WaitMutex -NoXGE -MaxParallelActions=12 -NoUBTMakefiles
        if ($LASTEXITCODE) { throw 'Registry game build failed' }
        $Receipt = Get-Content -LiteralPath (Join-Path $Repo 'Tests\DasForUESmoke\Binaries\Win64\DasForUESmoke.target') -Raw | ConvertFrom-Json
        $Source = Join-Path $Repo ('Plugins\DasForUE\Content\Das\Scripts\dsl_registered_' + $ProbeId + '.das')
        if (@($Receipt.RuntimeDependencies | ForEach-Object { $_.Path }) -notcontains $Source) { throw 'Registry source missing from game receipt' }
        $Archive = Join-Path $Repo 'Build\RegisteredTypesPackaged'
        & (Join-Path $Engine 'Engine\Build\BatchFiles\RunUAT.bat') BuildCookRun "-project=$Project" -noP4 -platform=Win64 -clientconfig=Development -cook -stage -pak -archive "-archivedirectory=$Archive" "-map=$Base/Map" -unattended -utf8output -nullrhi -nocompileeditor -skipbuildeditor -nodebuginfo
        if ($LASTEXITCODE) { throw 'Registry cook failed' }
        $Executable = Join-Path $Archive 'Windows\DasForUESmoke\Binaries\Win64\DasForUESmoke.exe'
        $Log = Join-Path $Logs 'type-registry-cooked.log'
        $GameArgs = @("$Base/Map",'-DasRegisteredCooked',"-DasRegistryProbeId=$ProbeId",'-nullrhi','-unattended','-nosplash',"-abslog=$Log") + $ModeArgs
        $Process = Start-Process -FilePath $Executable -ArgumentList $GameArgs -WindowStyle Hidden -PassThru -Wait
        if ($Process.ExitCode -or !(Select-String -LiteralPath $Log -SimpleMatch 'DAS_REGISTERED_COOKED_RESULT=PASS')) { throw 'Cooked registry game failed' }
    }
} finally {
    $InterfaceDefinition = Join-Path $Repo ('Plugins\DasForUE\Content\Das\Scripts\dsl_registered_' + $ProbeId + '_interface.das')
    if (Test-Path -LiteralPath $InterfaceDefinition) { Remove-Item -LiteralPath $InterfaceDefinition }
    $Source = Join-Path $Repo ('Plugins\DasForUE\Content\Das\Scripts\dsl_registered_' + $ProbeId + '.das')
    if (Test-Path -LiteralPath $Source) { Remove-Item -LiteralPath $Source }
    $Rules = Join-Path $Repo ('Plugins\DasForUE\Content\Das\Scripts\dsl_registered_' + $ProbeId + '_values.json')
    if (Test-Path -LiteralPath $Rules) { Remove-Item -LiteralPath $Rules }
}
Write-Output "DAS_TYPE_REGISTRY_PROBE=PASS cooked=$Cooked"
