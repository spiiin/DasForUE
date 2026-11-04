param([Parameter(Mandatory)][string]$Source)
$ErrorActionPreference='Stop'
$Root=[IO.Path]::GetFullPath($Source).TrimEnd('\','/')+[IO.Path]::DirectorySeparatorChar
$Spec=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'Dependencies/daScript-module-cache-patches.json') -Raw | ConvertFrom-Json
$Pending=@()
foreach($Patch in $Spec.patches) {
 $File=[IO.Path]::GetFullPath((Join-Path $Root $Patch.source))
 if(!$File.StartsWith($Root,[StringComparison]::OrdinalIgnoreCase)){throw 'Dependency patch path escapes source root'}
 $Original=[IO.File]::ReadAllText($File)
 $Text=$Original
 $Newline=if($Text.Contains("`r`n")){"`r`n"}else{"`n"}
 foreach($Change in $Patch.replacements) {
  $Before=$Change.before -replace "`r?`n",$Newline
  $After=$Change.after -replace "`r?`n",$Newline
  if($Text.Contains($After)){continue}
  if(!$Text.Contains($Before) -or $Text.IndexOf($Before) -ne $Text.LastIndexOf($Before)) {
   throw "daScript cache patch context differs: $($Patch.source); dependency files were not modified"
  }
  $Text=$Text.Replace($Before,$After)
 }
 if($Text -cne $Original){$Pending+=@{File=$File;Text=$Text}}
}
# Validate every context before writing any source file; preserve unrelated edits.
foreach($Change in $Pending) {
 [IO.File]::WriteAllText($Change.File,$Change.Text,[Text.UTF8Encoding]::new($false))
}
Write-Host "DAS_DEPENDENCY_CACHE_PATCHES=PASS changed_files=$($Pending.Count)"
