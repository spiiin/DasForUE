"""Keep the production editor independent of its optional test extension."""
from pathlib import Path
import re
import unittest

PLUGIN = Path(__file__).resolve().parents[1] / "Plugins/DasForUE"

class EditorBoundaryTest(unittest.TestCase):
    def test_production_editor_does_not_import_editor_test_headers(self):
        test_headers = {p.name for p in (PLUGIN / "Tests/Editor").rglob("*.h")}
        offenders = []
        for p in (PLUGIN / "Source/DasEditor").rglob("*"):
            if p.suffix not in (".h", ".cpp", ".inl"):
                continue
            for header in re.findall(r'#include "([^"\n]+)"', p.read_text(encoding="utf-8-sig")):
                if header in test_headers:
                    offenders.append(f"{p.name}: {header}")
        self.assertEqual(offenders, [])

    def test_automation_registration_is_outside_production_sources(self):
        offenders = [str(p.relative_to(PLUGIN)) for p in (PLUGIN / "Source/DasEditor").rglob("*.cpp")
                     if re.search(r"IMPLEMENT_\w*AUTOMATION_TEST", p.read_text(encoding="utf-8-sig"))]
        self.assertEqual(offenders, [])

if __name__ == "__main__":
    unittest.main()
