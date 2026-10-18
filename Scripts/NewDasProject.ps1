param([Parameter(Mandatory=$true)][string]$Destination,[string]$Engine='C:\src\unreal\UnrealEngine')
$ErrorActionPreference='Stop'
& C:/Python312/python.exe "$PSScriptRoot/CreateStandalone.py" $Destination --engine $Engine
if($LASTEXITCODE){throw 'Starter creation failed'}
Write-Output "Next: ./Scripts/GenerateNativeGameplay.ps1 -Project $Destination/DasStarter.uproject -Engine $Engine"
