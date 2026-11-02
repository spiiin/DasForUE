param([string]$Engine='C:/src/unreal/UnrealEngine',[string[]]$Scripts=@(),[switch]$SkipBuild)
$ErrorActionPreference='Stop'
Import-Module "$PSScriptRoot/DasTest.psm1" -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild){Invoke-DasBuild $Das}
$Arguments=@()
if($Scripts.Count){$Arguments+="-Scripts=$($Scripts -join ',')"}
Invoke-DasCommandlet $Das -Run DasPreparePrograms -Name prepare-programs -Marker 'DAS_PREPARE_EXPORT PASS' -Arguments $Arguments -NoZen
