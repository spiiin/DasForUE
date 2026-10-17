param([string]$Engine='C:\src\unreal\UnrealEngine',[string]$Python='C:\Python312\python.exe',[switch]$SkipEditor)
$ErrorActionPreference='Stop'
$PreviousSdkSetup=$env:UE_SKIP_UBT_SDK_SETUP
try {
$env:UE_SKIP_UBT_SDK_SETUP='1'
$Root=Split-Path -Parent $PSScriptRoot
$Project=Join-Path $Root 'Tests\DasForUESmoke\DasForUESmoke.uproject'
$Logs=Join-Path $Root 'logs'
if(!$SkipEditor){& (Join-Path $Root 'Scripts\TestCombinedEditor.ps1') -Engine $Engine -Python $Python}
& (Join-Path $Root 'Scripts\TestNativeDefaults.ps1') -Engine $Engine -SkipBuild
& (Join-Path $Root 'Scripts\TestComputedDefaults.ps1') -Engine $Engine -SkipBuild
$NativeId=(Get-Content (Join-Path $Logs 'native-defaults-probe-id.txt') -Raw).Trim()
$ComputedId=(Get-Content (Join-Path $Logs 'computed-defaults-probe-id.txt') -Raw).Trim()
$MethodId=(Get-Content (Join-Path $Logs 'combined-method-values-probe-id.txt') -Raw).Trim()
$ProbeId=[Guid]::NewGuid().ToString('N')
$Map="/Game/DasCookedGameplay/$ProbeId/Map"
$Playable="/Game/DasCookedGameplay/$ProbeId/PlayableMap"
$Native="/Game/DasNativeDefaults/$NativeId/Map"
$Computed="/Game/DasComputedDefaults/$ComputedId/Map"
$Method="/Game/DasMethodValues/$MethodId/Map"
$ExportLog=Join-Path $Logs 'combined-export.log'
& (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $Project -run=DasCookedGameplay "-ProbeId=$ProbeId" -unattended -NullRHI -nosplash -stdout "-abslog=$ExportLog"
if($LASTEXITCODE -or !(Select-String -LiteralPath $ExportLog -SimpleMatch 'DAS_COOKED_GAMEPLAY_EXPORT=PASS')){throw 'Combined gameplay export failed'}
& (Join-Path $Root 'Scripts\TestPackaged.ps1') -Engine $Engine -AdditionalMaps @($Map,$Playable,$Native,$Computed,$Method) -SkipZenStore
$Exe=Join-Path $Root 'Build\Packaged\Windows\DasForUESmoke\Binaries\Win64\DasForUESmoke.exe'
$Checks=@(
 @{Map=$Map;Switch='-DasCookedGameplay';Name='gameplay';Marker='DAS_COOKED_GAMEPLAY_RESULT=PASS'},
 @{Map=$Playable;Switch='-DasPlayableMap';Name='playable';Marker='DAS_PLAYABLE_MAP_RESULT=PASS'},
 @{Map=$Native;Switch='-DasNativeDefaults';Name='native';Marker='DAS_NATIVE_DEFAULTS_PACKAGED=PASS'},
 @{Map=$Computed;Switch='-DasComputedDefaults';Name='computed';Marker='DAS_COMPUTED_DEFAULTS_PACKAGED=PASS'},
 @{Map=$Method;Switch='-DasMethodValues';Name='method-values';Marker='DAS_METHOD_VALUES_RESULT=PASS'}
)
foreach($Check in $Checks){
 $Log=Join-Path $Logs ("combined-packaged-"+$Check.Name+'.log')
 $P=Start-Process -FilePath $Exe -ArgumentList $Check.Map,$Check.Switch,'-nullrhi','-unattended','-nosplash',"-abslog=$Log" -WindowStyle Hidden -PassThru
 $P.WaitForExit()
 if($P.ExitCode -or !(Select-String -LiteralPath $Log -SimpleMatch $Check.Marker)){throw ("Packaged "+$Check.Name+" failed; exit="+$P.ExitCode)}
 Write-Output ("DAS_COMBINED_PACKAGED_"+$Check.Name.ToUpper()+"=PASS map="+$Check.Map)
}
Write-Output 'DAS_COMBINED_PACKAGE=PASS'

} finally { $env:UE_SKIP_UBT_SDK_SETUP=$PreviousSdkSetup }
