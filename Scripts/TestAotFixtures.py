"""Catch silent fixture-mode collisions when AOT scenarios from branches are combined."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / 'Plugins/DasForUE/Content/Das/Scripts/aot_ue_guard.das'
PROBE = ROOT / 'Plugins/DasForUE/Tests/Runtime/Private/DasUEAotProbe.inl'

class AotFixtureTest(unittest.TestCase):
    def modes(self):
        return [int(x) for x in re.findall(r'\binput\s*==\s*(\d+)', SCRIPT.read_text(encoding='utf-8-sig'))]

    def test_scenario_modes_are_unique(self):
        modes = self.modes()
        self.assertEqual(len(modes), len(set(modes)), 'An earlier branch shadows another AOT scenario')
        self.assertGreaterEqual(len(modes), 18)

    def test_probe_modes_have_script_scenarios(self):
        probe = PROBE.read_text(encoding='utf-8-sig')
        called = {int(x) for x in re.findall(r'\bRun\((\d+)\)', probe)}
        called.update(int(x) for x in re.findall(r'RunFile\(Path,\s*(\d+)', probe))
        self.assertTrue(called)
        self.assertFalse(called - set(self.modes()), 'Probe refers to a missing AOT scenario')

if __name__ == '__main__':
    unittest.main()
