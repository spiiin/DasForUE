param([string]$Engine='C:\src\unreal\UnrealEngine', [string]$PortRoot='C:\src\ActionRoguelikeDas', [switch]$SkipBuild, [switch]$InputLifecycle, [switch]$Attack, [switch]$Attributes, [switch]$Combat, [switch]$Presentation, [switch]$Effects, [switch]$Overwhelm, [switch]$PointImpact, [switch]$AI, [switch]$AIPresentation, [switch]$Patrol, [switch]$Pickup, [switch]$Coins, [string]$Scenario, [switch]$Unattended, [switch]$Audio)
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
$Case=if($Attributes){'attributes'}elseif($InputLifecycle){'input'}elseif($Coins){'coins'}elseif($Pickup){'pickup'}elseif($Patrol){'patrol'}elseif($AIPresentation){'ai_presentation'}elseif($AI){'ai'}elseif($PointImpact){'point_impact'}elseif($Overwhelm){'overwhelm'}elseif($Effects){'effects'}elseif($Presentation){'presentation'}elseif($Combat){'character_combat'}elseif($Attack){'projectile_attack'}else{'actions'}
if($Scenario) {
    if($Scenario -notmatch '^[a-z_]+$') { throw 'Invalid scenario name' }
    $Case=$Scenario
}
$Result=Join-Path $PortRoot ('Saved/Tests/rogue_'+$Case+'_pie.json')
if(Test-Path -LiteralPath $Result) { Remove-Item -LiteralPath $Result }
$Bootstrap=(Join-Path $PortRoot ('Scripts/run_'+$Case+'_test.py')).Replace('\','/')
if($Bootstrap.Contains("'") -or $Bootstrap.Contains('"')) { throw 'Bootstrap path contains unsupported quotes' }
$Command="py __import__('runpy').run_path(r'$Bootstrap')"
$Log=Join-Path $Das.Logs ('rogue-'+$Case+'.log')
$Map=if($Case -eq 'director'){'/Game/DasRogue/Maps/DirectorTest'}elseif(($Patrol -or $Coins) -and (Test-Path -LiteralPath (Join-Path $PortRoot 'Content/DasRogue/Maps/PatrolTest.umap'))){'/Game/DasRogue/Maps/PatrolTest'}else{'/Game/DasRogue/Maps/ChestTest'}
$Arguments=@(('"'+(Join-Path $PortRoot 'ActionRoguelikeDas.uproject')+'"'),$Map,'-Multiprocess','-NoZenAutoLaunch','-ddc=NoZenLocalFallback','-nosplash','-NoSound','-benchmark','-fps=60',('-abslog="'+$Log+'"'),('-ExecCmds="'+$Command+'"'))
# Benchmark mode disables the audio engine even without -NoSound.
if($Audio){$Arguments=@($Arguments|Where-Object {$_ -notin @('-NoSound','-benchmark','-fps=60')})}
if($Unattended){$Arguments+='-unattended'}
$StrictSetup=$Scenario -or $Attributes -or $Attack -or $Combat -or $InputLifecycle -or $Coins -or $Pickup -or $Patrol -or $AIPresentation -or $AI -or $Presentation -or $Effects -or $Overwhelm -or $PointImpact
# Do not interpret the previous run's errors while the new editor is starting.
if($StrictSetup -and (Test-Path -LiteralPath $Log)) { Move-Item -LiteralPath $Log -Destination ($Log+'.previous') -Force }
$Process=Start-Process -FilePath (Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor.exe') -ArgumentList $Arguments -WindowStyle Hidden -PassThru
$Deadline=[DateTime]::UtcNow.AddSeconds($(if($Scenario -or $Presentation -or $AIPresentation -or $Patrol -or $Pickup -or $Coins){600}else{360}))
while(!$Process.WaitForExit(1000)) {
    if($StrictSetup -and (Test-Path -LiteralPath $Log)) {
        $Failure=Test-DasResult -LiteralPath $Log -Pattern 'Incompatible or missing module:|DAS_DSL_APPLY rejected:|DAS_DSL_REIMPORT rejected:|DAS_DSL_AUTO_REIMPORT rejected|Play-In-Editor canceled by plugin:|appError called:' | Select-Object -First 1
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
$RuntimePattern=if($Scenario -or $AI -or $AIPresentation -or $Patrol -or $Pickup -or $Coins){'daScript runtime:|Script timer .* failed:|DAS object call rejected:|Blueprint Runtime Error:|Invalid authored object owner|Invalid authored lifecycle owner'}else{'PIE: Error: daScript runtime:|Script timer .* failed:|DAS object call rejected:'}
$RuntimeFailure=Test-DasResult -LiteralPath $Log -Pattern $RuntimePattern | Select-Object -First 1
if($RuntimeFailure) { throw ('Rogue runtime/teardown failed: '+$RuntimeFailure.Line) }
if($Scenario -or $Patrol -or $Pickup -or $Coins) {
    $EngineFailure=Test-DasResult -LiteralPath $Log -Pattern 'Ensure condition failed:|Unhandled Exception:|Fatal error:' | Select-Object -First 1
    if($EngineFailure) { throw ('Rogue engine diagnostic failed: '+$EngineFailure.Line) }
}

if($AIPresentation) {
    $MaterialFailure=Test-DasResult -LiteralPath $Log -Pattern 'Failed to compile Material|missing usage flag|LoadErrors:.*(/Game/ParagonMinions/|/Game/ExampleContent/Textures/T_White)' | Select-Object -First 1
    if($MaterialFailure) { throw ('Minion resource load/compile failed: '+$MaterialFailure.Line) }
}

Write-Output ('DAS_ROGUE_'+$Case.ToUpper()+'=PASS checks='+$Receipt.checks.Count)
