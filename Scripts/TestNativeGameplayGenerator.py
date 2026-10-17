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
    def test_enum_layout(self):
        header,module=generate(self.ir)
        self.assertIn('enum EMovementMode {',module)
        self.assertIn('MOVE_Walking=1',module)
        self.assertIn('ue_native_gameplay::EMovementMode',header)
    def test_missing_enum(self):
        self.ir['enums']=[]
        with self.assertRaises(ValueError): generate(self.ir)
    def test_duplicate_enum(self):
        self.ir['enums'].append(copy.deepcopy(self.ir['enums'][0]))
        with self.assertRaises(ValueError): generate(self.ir)
    def test_bad_enum_value(self):
        self.ir['enums'][0]['entries'][0]['value']=255
        with self.assertRaises(ValueError): generate(self.ir)
    def test_enum_alias(self):
        self.ir['enums'][0]['entries'][1]['value']=self.ir['enums'][0]['entries'][0]['value']
        with self.assertRaises(ValueError): generate(self.ir)
    def test_byte_and_text_exported(self):
        kinds={v['kind'] for c in self.ir['classes'] for v in c['fields']}
        self.assertTrue({'byte','text'}<=kinds)
        header,module=generate(self.ir)
        self.assertIn('TEXT("SetMovementMode")',header)
        self.assertIn('ue_text::TextRenderComponentRef',header)
        self.assertIn('require ue_text public',module)
    def test_structs_and_arrays(self):
        header,module=generate(self.ir)
        self.assertIn('struct StreamingSourceShape {',module)
        self.assertIn('struct NavAvoidanceMask {',module)
        self.assertIn('DasNativeGameplayStructs',header)
        arrays=[v for c in self.ir['classes'] for v in c['fields'] if v['kind']=='array']
        self.assertTrue(any(v['name']=='Tags' and v['element']['kind']=='name' for v in arrays))
        self.assertTrue(any(v['element']['kind']=='struct' for v in arrays))
    def test_missing_struct(self):
        self.ir['structs']=[]
        with self.assertRaises(ValueError):generate(self.ir)
    def test_bad_array_element(self):
        field=next(v for c in self.ir['classes'] for v in c['fields'] if v['kind']=='array')
        del field['element']
        with self.assertRaises(ValueError):generate(self.ir)
    def test_duplicate_struct_field(self):
        self.ir['structs'][0]['fields'].append(copy.deepcopy(self.ir['structs'][0]['fields'][0]))
        with self.assertRaises(ValueError):generate(self.ir)
    def test_hash_container_generation(self):
        header,module=generate(self.ir)
        self.assertIn('Labels:table<int;string>',module)
        self.assertIn('Names:table<string>',module)
        self.assertIn('TEXT("EchoActors")',header)
        fixture=next(c for c in self.ir['classes'] if c.get('fixture'))
        self.assertEqual(fixture['path'],'/Script/DasRuntime.DasNativeHashProbeComponent')
        self.assertTrue({'map','set'} <= {v['kind'] for v in fixture['fields']})
    def hash_field(self,kind):
        return next(v for c in self.ir['classes'] for v in c['fields'] if v['kind']==kind)
    def test_reject_float_hash_key(self):
        self.hash_field('map')['key']={'kind':'float','type_path':''}
        with self.assertRaises(ValueError):generate(self.ir)
    def test_reject_nested_hash_value(self):
        self.hash_field('map')['value']={'kind':'set','type_path':'','element':{'kind':'int','type_path':''}}
        with self.assertRaises(ValueError):generate(self.ir)
    def test_reject_missing_map_value(self):
        del self.hash_field('map')['value']
        with self.assertRaises(ValueError):generate(self.ir)
    def test_reject_struct_set(self):
        self.hash_field('set')['element']={'kind':'struct','type_path':'/Script/CoreUObject.Vector'}
        with self.assertRaises(ValueError):generate(self.ir)
    def test_enum_default_preserved(self):
        header,_=generate(self.ir)
        self.assertIn('TEXT("TransformSpace"),TEXT("RTS_World")',header)
if __name__=='__main__': unittest.main()
