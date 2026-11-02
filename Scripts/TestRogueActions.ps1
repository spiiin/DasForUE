param([string]$Engine='C:\src\unreal\UnrealEngine', [string]$PortRoot='C:\src\ActionRoguelikeDas', [switch]$SkipBuild, [switch]$Attack, [switch]$Presentation, [switch]$Effects)
$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das=Get-DasContext -Engine $Engine
if(!$SkipBuild) { Invoke-DasBuild $Das -Name 'Rogue actions' }
$Source=Join-Path (Split-Path -Parent $Das.Project) 'Binaries/Win64'
$Target=Join-Path $PortRoot 'Binaries/Win64'
$Modules=Get-Content -LiteralPath (Join-Path $Source 'UnrealEditor.modules') -Raw | ConvertFrom-Json
$EngineModules=Get-Content -LiteralPath (Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor.modules') -Raw | ConvertFrom-Json
if($Modules.BuildId -ne $EngineModules.BuildId) { throw 'Tested modules and actual engine have different BuildIds' }
Copy-Item -LiteralPath (Join-Path $Source 'UnrealEditor-DasRuntime.dll'),(Join-Path $Source 'UnrealEditor-DasEditor.dll') -Destination $Target -Force
@{BuildId=$Modules.BuildId;Modules=@{DasRuntime='UnrealEditor-DasRuntime.dll';DasEditor='UnrealEditor-DasEditor.dll'}} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $Target 'UnrealEditor.modules') -Encoding utf8
$Case=if($Effects){'effects'}elseif($Presentation){'presentation'}elseif($Attack){'projectile_attack'}else{'actions'}
$Result=Join-Path $PortRoot ('Saved/Tests/rogue_'+$Case+'_pie.json')
if(Test-Path -LiteralPath $Result) { Remove-Item -LiteralPath $Result }
$Bootstrap=(Join-Path $PortRoot ('Scripts/run_'+$Case+'_test.py')).Replace('\','/')
if($Bootstrap.Contains("'") -or $Bootstrap.Contains('"')) { throw 'Bootstrap path contains unsupported quotes' }
$Command="py __import__('runpy').run_path(r'$Bootstrap')"
$Log=Join-Path $Das.Logs ('rogue-'+$Case+'.log')
$Arguments=@(('"'+(Join-Path $PortRoot 'ActionRoguelikeDas.uproject')+'"'),'/Game/DasRogue/Maps/ChestTest','-Multiprocess','-NoZenAutoLaunch','-ddc=NoZenLocalFallback','-nosplash','-NoSound','-benchmark','-fps=60',('-abslog="'+$Log+'"'),('-ExecCmds="'+$Command+'"'))
$StrictSetup=$Presentation -or $Effects
# Do not interpret the previous run's errors while the new editor is starting.
if($StrictSetup -and (Test-Path -LiteralPath $Log)) { Move-Item -LiteralPath $Log -Destination ($Log+'.previous') -Force }
$Process=Start-Process -FilePath (Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor.exe') -ArgumentList $Arguments -WindowStyle Hidden -PassThru
$Deadline=[DateTime]::UtcNow.AddSeconds($(if($Presentation){600}else{240}))
while(!$Process.WaitForExit(1000)) {
    if($StrictSetup -and (Test-Path -LiteralPath $Log)) {
        $Failure=Select-String -LiteralPath $Log -Pattern 'Incompatible or missing module:|DAS_DSL_APPLY rejected:|DAS_DSL_AUTO_REIMPORT rejected|Play-In-Editor canceled by plugin:|appError called:' | Select-Object -First 1
        if($Failure) {
            Stop-Process -Id $Process.Id -Force
            throw ('Rogue setup failed before PIE: '+$Failure.Line)
        }
    }
    if([DateTime]::UtcNow -gt $Deadline) {
        Stop-Process -Id $Process.Id -Force
        throw 'Rogue editor timed out; only its own process was terminated'
    }
}
$Process.Refresh()
if($Process.ExitCode) { throw "Rogue actions editor failed (exit $($Process.ExitCode)); see $Log" }
if(!(Test-Path -LiteralPath $Result)) { throw "Rogue actions produced no result; see $Log" }
$Receipt=Get-Content -LiteralPath $Result -Raw | ConvertFrom-Json
if(!$Receipt.passed) { throw "Rogue actions failed: $($Receipt.error)" }
Write-Output ('DAS_ROGUE_'+$Case.ToUpper()+'=PASS checks='+$Receipt.checks.Count)
