param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das -Name 'subsystems packaged exporter'}
$Config=Join-Path $Das.Repo 'Tests\DasForUESmoke\Config\DefaultGame.ini'
$Original=[IO.File]::ReadAllBytes($Config)
$Files=@(Get-ChildItem (Join-Path $Das.Repo 'Plugins\DasForUE\Source'),(Join-Path $Das.Repo 'Tests\DasForUESmoke\Source') -Recurse -File |
    Where-Object Extension -in '.cpp','.h','.inl','.cs' | ForEach-Object FullName)
$Files+=@(Get-ChildItem (Join-Path $Das.Repo 'Plugins\DasForUE\Content\Das\Scripts') -Filter '*.das' -File | ForEach-Object FullName)
$Files+=@($PSCommandPath,$Config,(Join-Path $PSScriptRoot 'TestPackaged.ps1'),(Join-Path $PSScriptRoot 'DasTest.psm1'))
$Before=Lock-SourceHashes $Files
Invoke-DasCommandlet $Das -Run DasSubsystemsPackage -Name 'subsystems-package-export' -Marker 'DAS_SUBSYSTEMS_EXPORT=PASS' -NoZen | Out-Null
$Classes=@('/Game/DasSubsystems/Packaged/BP_Missions.BP_Missions_C','/Game/DasSubsystems/Packaged/BP_Session.BP_Session_C')
try {
    $Addition="`n[/Script/DasRuntime.DasSubsystems]`n!Classes=ClearArray`n"+ (($Classes | ForEach-Object { "+Classes=$_" }) -join "`n")+"`n"
    [IO.File]::AppendAllText($Config,$Addition,[Text.UTF8Encoding]::new($false))
    & (Join-Path $PSScriptRoot 'TestPackaged.ps1') -Engine $Engine -SkipZenStore -SkipSmoke `
        -AdditionalMaps @('/Game/DasSubsystems/Packaged/Map') `
        -AdditionalCookDirectories @((Join-Path $Das.Repo 'Tests\DasForUESmoke\Content\DasSubsystems\Packaged'))
} finally { [IO.File]::WriteAllBytes($Config,$Original) }
$Log=Join-Path $Das.Logs 'subsystems-packaged.log'
if(Test-Path -LiteralPath $Log){Remove-Item -LiteralPath $Log}
$BinaryHash=(Get-FileHash -LiteralPath $Das.Packaged -Algorithm SHA256).Hash
$Arguments=@('/Game/DasSubsystems/Packaged/Map','-DasSubsystemsPackaged','-nullrhi','-unattended','-nosplash',('-abslog="'+$Log+'"'))
$Process=Start-Process -FilePath $Das.Packaged -WorkingDirectory (Split-Path -Parent $Das.Packaged) `
    -ArgumentList $Arguments -WindowStyle Hidden -PassThru
if(!$Process.WaitForExit(180000)){Stop-Process -Id $Process.Id -Force;throw 'Subsystem packaged process exceeded 3 minutes'}
$Process.Refresh()
if($Process.ExitCode){throw "Subsystem packaged process failed: exit $($Process.ExitCode)"}
Assert-DasLog -Log $Log -Marker 'DAS_SUBSYSTEMS_PACKAGED_RESULT=PASS checks=12' `
    -FailPattern 'DAS_SUBSYSTEMS_PACKAGED_FAIL|:\s+Error:|Fatal error:' -Name 'authored subsystems packaged'
if((Lock-SourceHashes $Files) -cne $Before){throw 'Subsystem packaged sources/config changed during validation'}
if((Get-FileHash -LiteralPath $Das.Packaged -Algorithm SHA256).Hash -ne $BinaryHash){throw 'Packaged binary changed during validation'}
Write-DasValidation -Path (Join-Path $Das.Logs 'authored-subsystems-packaged-validation.json') -Data ([ordered]@{
    state='PASS';checks=12;mode='Win64 packaged Development; config-only classes, explicit cook, native startup and normal engine shutdown'
    updated_utc=[DateTime]::UtcNow.ToString('o');runtime_log=$Log;classes=$Classes;source_hashes=($Before | ConvertFrom-Json)
    executable=$Das.Packaged;executable_sha256=$BinaryHash;config_restored=$true;shipping='not_run';pie='not_run'
})
Write-Output 'DAS_SUBSYSTEMS_DEVELOPMENT_PACKAGE=PASS checks=12'
