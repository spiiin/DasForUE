"""Regression contract for production/test reflection separation."""
import json
import re
import unittest
from pathlib import Path
from GenerateRuntimeBindings import ROOT, generate, references
from DiscoverNativeGameplay import declarations

class RuntimeBindingsTests(unittest.TestCase):
    def test_generated_runtime_has_no_fixture_dependencies(self):
        outputs = generate()
        excluded = set(json.loads((ROOT/'Plugins/DasForUE/Tests/Runtime/ReflectedTypes.json').read_text()))
        for file, source in outputs.items():
            self.assertEqual(file.read_text(encoding='utf-8'), source, str(file))
            self.assertFalse(references(source, excluded), str(file))
        content = ROOT/'Plugins/DasForUE/Content/Das'
        files = [content/line for line in map(str.strip, (content/'RuntimeScripts.txt').read_text().splitlines())
                 if line and not line.startswith('#')]
        modules = {file.stem for file in files}
        for file in files:
            source = file.read_text(encoding='utf-8')
            self.assertFalse(references(source, excluded), str(file))
            for dependency in declarations(source)[1]:
                if (content/'Scripts'/(dependency+'.das')).exists():
                    self.assertIn(dependency, modules, str(file))

    def test_reflection_inventory_matches_test_headers(self):
        root = ROOT/'Plugins/DasForUE/Tests/Runtime'
        found = set()
        pattern = r'U(?:CLASS|STRUCT|INTERFACE|ENUM)\([^\n]*\)\s*(?:enum\s+(?:class\s+)?|class\s+|struct\s+)(?:DASRUNTIME_API\s+)?(\w+)'
        for file in root.rglob('*.h'):
            for name in re.findall(pattern, file.read_text(encoding='utf-8')):
                found.add('/Script/DasRuntime.'+(name if name.startswith('E') else name[1:]))
        self.assertEqual(set(json.loads((root/'ReflectedTypes.json').read_text())), found)

    def test_standalone_probes_stay_outside_production_sources(self):
        source = ROOT/'Plugins/DasForUE/Source/DasRuntime'
        offenders = []
        for file in source.rglob('*'):
            if not file.is_file(): continue
            if (file.name.endswith(('Probe.cpp', 'Tests.cpp', 'Tests.inl'))
                    or 'Fixture' in file.name
                    or file.name in ('SmokeAot.cpp', 'UEGuardAot.cpp', 'DasGASPlaygroundValidation.cpp')):
                offenders.append(str(file.relative_to(source)))
        self.assertEqual(offenders, [], 'Move new test sources to Tests/Runtime instead of extending the runtime')

    def test_reference_matching_respects_identity_boundaries(self):
        fixtures = {'/Script/DasRuntime.Fixture'}
        self.assertTrue(references({'signature':'ObjectProperty:/Script/DasRuntime.Fixture;'}, fixtures))
        self.assertFalse(references('/Script/DasRuntime.FixtureProduction', fixtures))
        self.assertFalse(references({'metadata':{'ToolTip':'Uses /Script/DasRuntime.Fixture in tests'}}, fixtures))

if __name__ == '__main__': unittest.main()
