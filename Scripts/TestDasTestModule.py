"""Contract tests for shared runners; fake tools never start Unreal or UBT."""
import base64
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
POWERSHELL = shutil.which("powershell") or shutil.which("pwsh")


@unittest.skipUnless(POWERSHELL, "PowerShell is required for runner contracts")
class TestDasTestModule(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory(prefix="das runner ")
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)

    def ps(self, body, **environment):
        module = str(ROOT / "Scripts/DasTest.psm1").replace("'", "''")
        directory = str(self.root).replace("'", "''")
        script = (f"$ErrorActionPreference='Stop'; Import-Module '{module}' -Force; "
                  f"$Temp='{directory}'; " + body)
        environment = {**os.environ, **environment}
        # A PS7 parent module path must not select PS7 utility modules in Windows PowerShell.
        environment.pop("PSModulePath", None)
        result = subprocess.run(
            [POWERSHELL, "-NoProfile", "-NonInteractive", "-EncodedCommand",
             base64.b64encode(script.encode("utf-16-le")).decode()],
            capture_output=True, text=True, env=environment)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        return result.stdout

    @unittest.skipUnless(os.name == "nt", "Fake batch build uses Windows cmd")
    def test_exact_build_arguments_and_output(self):
        batch = self.root / "Engine/Build/BatchFiles/Build.bat"
        batch.parent.mkdir(parents=True)
        batch.write_text('@echo off\necho %* > "%DAS_TEST_ARGUMENTS%"\necho BUILD_OUTPUT\nexit /b 0\n')
        target = self.root / "arguments.txt"
        output = self.ps(r"""
$Das=[pscustomobject]@{Engine=$Temp;Project='unused.uproject'}
Invoke-DasBuild $Das -BuildArguments @('CustomTarget','Win64','Development',
    '-Project=C:/with spaces/probe.uproject','-Module=DasRuntime','-SkipBuild') *> "$Temp/output.log"
Get-Content "$Temp/output.log"
""", DAS_TEST_ARGUMENTS=str(target))
        arguments = target.read_text()
        self.assertIn('CustomTarget Win64 Development', arguments)
        self.assertIn('"-Project=C:/with spaces/probe.uproject"', arguments)
        self.assertIn('-Module=DasRuntime -SkipBuild', arguments)
        self.assertNotIn('DasForUESmokeEditor', arguments)
        self.assertNotIn('MaxParallelActions', arguments)
        self.assertIn('BUILD_OUTPUT', output)

    @unittest.skipUnless(os.name == "nt", "Fake batch build uses Windows cmd")
    def test_default_build_and_failure(self):
        batch = self.root / "Engine/Build/BatchFiles/Build.bat"
        batch.parent.mkdir(parents=True)
        batch.write_text('@echo off\necho %* > "%DAS_TEST_ARGUMENTS%"\nexit /b 3\n')
        target = self.root / "arguments.txt"
        self.ps(r"""
$Das=[pscustomobject]@{Engine=$Temp;Project='probe.uproject'}
try { Invoke-DasBuild $Das -Name probe -Parallel 8; throw 'failure was swallowed' }
catch { if($_.Exception.Message -ne 'probe build failed'){throw} }
if($LASTEXITCODE -ne 3){throw 'native exit code lost'}
exit 0
""", DAS_TEST_ARGUMENTS=str(target))
        self.assertIn('DasForUESmokeEditor Win64 Development', target.read_text())
        self.assertIn('-MaxParallelActions=8', target.read_text())

    def test_result_markers_regex_and_exact_count(self):
        self.ps(r"""
'DAS_PROBE_RESULT=PASS checks=12 detail=3' | Set-Content "$Temp/probe.log"
if(!(Test-DasResult -LiteralPath "$Temp/probe.log" -SimpleMatch 'RESULT=PASS' -Quiet)){throw 'marker'}
if(Test-DasResult -LiteralPath "$Temp/probe.log" -SimpleMatch 'MISSING' -Quiet){throw 'false marker'}
$M=Assert-DasResult -Log "$Temp/probe.log" -Pattern 'DAS_PROBE_RESULT=PASS checks=(\d+) detail=(\d+)' -Checks 12
if($M.Groups[2].Value -ne '3'){throw 'extra field lost'}
try { Assert-DasResult -Log "$Temp/probe.log" -Pattern 'checks=(\d+)' -Checks 13; throw 'bad count accepted' }
catch { if($_.Exception.Message -eq 'bad count accepted'){throw} }
if((Get-DasResult -Text 'value=ABC' -Pattern 'value=abc').Success){throw 'regex case changed'}
""")

    def test_hash_freeze_and_validation_payload(self):
        self.ps(r"""
'a' | Set-Content "$Temp/a"; 'b' | Set-Content "$Temp/b"
$Before=Lock-SourceHashes -Paths @("$Temp/b","$Temp/a")
if($Before -cne (Lock-SourceHashes -Paths @("$Temp/a","$Temp/b"))){throw 'ordering'}
'changed' | Set-Content "$Temp/a"
if($Before -ceq (Lock-SourceHashes -Paths @("$Temp/a","$Temp/b"))){throw 'mutation missed'}
Write-DasValidation -Path "$Temp/receipt.json" -Data ([ordered]@{state='PASS';checks=2;sha256=($Before|ConvertFrom-Json)})
$Data=Get-Content "$Temp/receipt.json" -Raw|ConvertFrom-Json
if($Data.state -ne 'PASS' -or $Data.checks -ne 2 -or @($Data.sha256.PSObject.Properties).Count -ne 2){throw 'payload changed'}
""")


if __name__ == "__main__":
    unittest.main()
