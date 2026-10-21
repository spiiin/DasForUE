param([string]$Engine='C:\src\unreal\UnrealEngine',[switch]$SkipBuild,[switch]$Publish)
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force

$ErrorActionPreference='Stop'
$Repo=Split-Path -Parent $PSScriptRoot
$Python='C:\Python312\python.exe'
foreach($Test in @('TestBindingsGenerator.py','TestApiPackage.py','TestCanonicalMathGenerator.py')){
 & $Python "$PSScriptRoot/$Test"
 if($LASTEXITCODE){throw "$Test failed"}
}
& "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -SkipBuild:$SkipBuild -Publish:$Publish
if(!$?){throw 'Canonical Math package failed'}
& "$PSScriptRoot/GenerateApiPackage.ps1" -Engine $Engine -SkipBuild -Profile "$Repo/Bindings/Packages/Tests/CanonicalMath.profile.json"
if(!$?){throw 'Nested canonical Math fixture failed'}
$Stage=Join-Path $Repo ('Build/ApiPackages/CanonicalMathChecks/'+[guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $Stage -Force|Out-Null
$VectorCall='let call=ue_begin_static_call("/Script/Engine.KismetMathLibrary","Add_VectorVector","")'
$ArrayCall='let call=ue_begin_static_call("/Script/Engine.KismetMathLibrary","GetVectorArrayAverage","")'
$Vector='ue_vector(1.0lf,2.0lf,3.0lf)'
$Checks=@(
 @{name='wrong_direction';body="$VectorCall`n__ue_method_arg_vector(call,`"returnvalue`",$Vector)";error='unknown parameter or wrong parameter direction'},
 @{name='wrong_type';body="$VectorCall`n__ue_method_arg_transform(call,`"a`",ue_transform_identity())";error='method math input type/value mismatch'},
 @{name='uninitialized_array';body="$ArrayCall`n__ue_method_arg_vector(call,`"vectors.0`",$Vector)";error='initialize array before assigning elements'},
 @{name='out_of_bounds';body="$ArrayCall`nue_arg_array(call,`"vectors`",1)`n__ue_method_arg_vector(call,`"vectors.1`",$Vector)";error='array index out of bounds'},
 @{name='missing_element';body="$ArrayCall`nue_arg_array(call,`"vectors`",2)`n__ue_method_arg_vector(call,`"vectors.0`",$Vector)`nue_invoke_call(call)";error='missing array element or struct field'},
 @{name='released_frame';body="$VectorCall`nue_release_call(call)`n__ue_method_arg_vector(call,`"a`",$Vector)";error='call handle is released or belongs to another context'},
 @{name='nonfinite_vector';body="$VectorCall`nvar vector=$Vector`nvector.x=1.0lf/double(input)`n__ue_method_arg_vector(call,`"a`",vector)";error='method math input type/value mismatch'}
)
$Project=Join-Path $Repo 'Tests/DasForUESmoke/DasForUESmoke.uproject'
foreach($Check in $Checks){
 $Probe=Join-Path $Stage ($Check.name+'.das')
 $Log=Join-Path $Stage ($Check.name+'.log')
 "options gen2`nrequire ue_foundation`n[export]`ndef main(input:int):int {`n$($Check.body)`nreturn 42`n}"|Set-Content -LiteralPath $Probe -Encoding utf8
 & "$Engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" $Project -run=DasExport -Modules=Engine "-Probe=$Probe" -Multiprocess -unattended -NullRHI -nosplash -stdout "-abslog=$Log" -ddc=NoZenLocalFallback -NoZenAutoLaunch *> "$Log.stdout"
 if(!$LASTEXITCODE -or !(Test-DasResult -LiteralPath $Log -SimpleMatch $Check.error -Quiet)){
  throw "Expected rejection $($Check.name) missing; inspect $Log"
 }
 Write-Output "DAS_CANONICAL_MATH_REJECTION=PASS $($Check.name)"
}
[ordered]@{state='PASS';python_tests=120;whole_math_module='PASS';nested_struct_and_array_fixture='PASS';negative_checks=@($Checks|ForEach-Object {$_.name});full_foundation='not_run';PIE='not_run';packaged='not_run';commit='not_created'}|ConvertTo-Json -Depth 10|Set-Content -LiteralPath "$Repo/logs/canonical-math-validation.json" -Encoding utf8
Write-Output "DAS_CANONICAL_MATH=PASS negative_logs=$Stage"
exit 0
