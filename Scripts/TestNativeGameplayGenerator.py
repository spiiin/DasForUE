import copy,json,unittest
from pathlib import Path
from GenerateNativeGameplay import generate

class NativeGameplayGeneratorTests(unittest.TestCase):
    def setUp(self): self.ir=json.loads((Path(__file__).resolve().parent.parent/'Bindings/NativeGameplay.json').read_text(encoding='utf-8-sig'))
    def test_deterministic(self): self.assertEqual(generate(self.ir),generate(copy.deepcopy(self.ir)))
    def test_reuses_canonical_character(self):
        header,module=generate(self.ir)
        self.assertIn('ue_character_control::CharacterRef',header)
        self.assertNotIn('[ue_native_object(path="/Script/Engine.Character")]',module)
        self.assertIn('_from(value:UObjectHandle)',module)
    def test_reject_schema(self):
        self.ir['schema_version']=99
        with self.assertRaises(ValueError): generate(self.ir)
    def test_reject_duplicate_class(self):
        self.ir['classes'].append(copy.deepcopy(self.ir['classes'][0]))
        with self.assertRaises(ValueError): generate(self.ir)
    def test_reject_duplicate_member(self):
        self.ir['classes'][0]['fields'].append(copy.deepcopy(self.ir['classes'][0]['fields'][0]))
        with self.assertRaises(ValueError): generate(self.ir)
    def test_reject_unsupported_type(self):
        self.ir['classes'][0]['fields'][0]['kind']='raw_pointer'
        with self.assertRaises(ValueError): generate(self.ir)
if __name__=='__main__': unittest.main()
