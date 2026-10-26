"""Registry export schema, generation, boundaries and drift detection."""
import copy
import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

import GenerateTypeSupport as gen

class TypeSupportTest(unittest.TestCase):
    def setUp(self):
        self.data = gen.load(gen.BASELINE)

    def test_document_is_current(self):
        doc = gen.DOCUMENT.read_text(encoding='utf-8')
        self.assertEqual(doc, gen.replace_section(doc, gen.render(self.data)))

    def test_every_adapter_and_sample_is_rendered(self):
        text = gen.render(self.data)
        for adapter in self.data['adapters']:
            self.assertIn(gen.code(adapter['id']), text)
            self.assertTrue(adapter['samples'], adapter['id'])
            for sample in adapter['samples']:
                self.assertIn(gen.code(gen.descriptor(sample['type'])), text)

    def test_container_nesting_is_not_promised(self):
        containers = [a for a in self.data['adapters'] if a['container_slots']]
        self.assertEqual({a['kind'] for a in containers}, {'array', 'set', 'map'})
        for a in containers:
            self.assertFalse(a['container_value_policy'])
            for sample in a['samples']:
                for key in ('array_element', 'map_value', 'map_key', 'set_element'):
                    self.assertEqual(sample['capabilities'][key]['state'], 'unsupported')

    def test_weak_storage_and_interface_keys_stay_separate(self):
        samples = [s for a in self.data['adapters'] if a['kind'] == 'object' for s in a['samples']]
        self.assertTrue(any(s['type']['weak'] for s in samples))
        for s in samples:
            if s['type']['weak']:
                for key in ('array_element', 'map_value', 'map_key', 'set_element'):
                    self.assertEqual(s['capabilities'][key]['state'], 'unsupported')
        self.assertTrue(any(not s['type']['weak'] and s['capabilities']['map_key']['state'] == 'unsupported' for s in samples))

    def test_no_sample_means_unreported_not_supported(self):
        self.data['adapters'][0]['samples'] = []
        self.assertIn(' | ' + ' | '.join(['Unreported'] * len(gen.CAPS)) + ' |', gen.render(self.data))

    def test_future_adapter_does_not_need_a_generator_case(self):
        new = copy.deepcopy(self.data['adapters'][0])
        new['id'] = new['kind'] = 'future_kind'
        for s in new['samples']:
            s['type']['kind'] = 'future_kind'
        self.data['adapters'].append(new)
        self.data['adapters'].sort(key=lambda a: a['id'])
        self.assertIn('future_kind', gen.render(self.data))

    def test_duplicate_json_keys_are_rejected(self):
        with self.assertRaisesRegex(ValueError, 'Duplicate JSON key'):
            json.loads('{"state":"supported","state":"unsupported"}', object_pairs_hook=gen.unique_object)

    def test_new_capability_field_requires_schema_update(self):
        self.data['adapters'][0]['future_support'] = True
        with self.assertRaisesRegex(ValueError, 'fields'):
            gen.validate(self.data)

    def test_unsupported_schema_is_rejected(self):
        self.data['schema_version'] = 2
        with self.assertRaisesRegex(ValueError, 'schema'):
            gen.validate(self.data)

    def test_reason_must_agree_with_support_state(self):
        cell = self.data['adapters'][0]['samples'][0]['capabilities']['value_property']
        cell.update(state='unsupported', reason='')
        with self.assertRaisesRegex(ValueError, 'reason'):
            gen.validate(self.data)

    def test_missing_context_is_rejected(self):
        del self.data['adapters'][0]['samples'][0]['capabilities']['map_key']
        with self.assertRaisesRegex(ValueError, 'contexts'):
            gen.validate(self.data)

    def test_duplicate_adapter_identity_is_rejected(self):
        self.data['adapters'].append(self.data['adapters'][0])
        with self.assertRaisesRegex(ValueError, 'identities'):
            gen.validate(self.data)

    def test_scope_and_per_binding_aot_are_explicit(self):
        text = gen.render(self.data)
        self.assertIn('property ABI usages', text)
        self.assertIn('per binding', text)
        self.assertIn('Not inferred', text)
        self.assertIn('not acceptance by every authored DSL', text)

    def test_markdown_data_is_escaped(self):
        self.assertEqual(gen.code('a|b<c>'), '<code>a&#124;b&lt;c&gt;</code>')

    def test_marker_errors_fail_closed(self):
        with self.assertRaisesRegex(ValueError, 'exactly one'):
            gen.replace_section('no markers', gen.render(self.data))

    def test_check_detects_live_drift_without_writing(self):
        before_doc = gen.DOCUMENT.read_bytes()
        before_baseline = gen.BASELINE.read_bytes()
        self.data['adapters'][0]['leaf'] = not self.data['adapters'][0]['leaf']
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / 'live.json'
            path.write_text(json.dumps(self.data), encoding='utf-8')
            p = subprocess.run([sys.executable, str(Path(gen.__file__)), '--manifest', str(path), '--check'], capture_output=True)
        self.assertNotEqual(p.returncode, 0)
        self.assertIn(b'Live registry differs', p.stderr)
        self.assertEqual(before_doc, gen.DOCUMENT.read_bytes())
        self.assertEqual(before_baseline, gen.BASELINE.read_bytes())

if __name__ == '__main__':
    unittest.main()
