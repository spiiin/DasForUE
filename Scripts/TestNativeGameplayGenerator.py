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
        cls=next(c for c in self.ir['classes'] if c['fields'])
        cls['fields'].append(copy.deepcopy(cls['fields'][0]))
        with self.assertRaises(ValueError): generate(self.ir)
    def test_reject_unsupported_type(self):
        next(c for c in self.ir['classes'] if c['fields'])['fields'][0]['kind']='raw_pointer'
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
    def add_soft(self,cls=False):
        import hashlib
        base='/Script/Engine.Actor' if cls else '/Script/CoreUObject.Object'
        field={'name':'SoftTest','kind':'struct','type_path':'/Script/DasRuntime.DasSoft'+('Class_' if cls else 'Object_')+hashlib.md5(base.encode()).hexdigest(),'soft_reference':{'base':base,'class_ref':cls},'signature':'fixture','declaring_class':self.ir['classes'][0]['path'],'readonly':False}
        self.ir['classes'][0]['fields'].append(field)
        return field
    def test_soft_reference_generation(self):
        field=self.add_soft();header,module=generate(self.ir)
        self.assertIn('DasNativeGameplaySoftTypes',header)
        self.assertIn(field['type_path'],header)
        self.assertRegex(module,r'\$UESoftObject<C_Object_\w+>')
        self.assertIn('require ue_soft_references public',module)
    def test_soft_class_generation(self):
        self.add_soft(True);header,module=generate(self.ir)
        self.assertRegex(module,r'\$UESoftClass<C_Actor_\w+>')
    def test_soft_array_and_map(self):
        field=self.add_soft();leaf={k:v for k,v in field.items() if k in ('kind','type_path','soft_reference')}
        field.update(kind='array',type_path='',element=leaf);del field['soft_reference']
        header,module=generate(self.ir)
        self.assertRegex(module,r'\$UESoftObject<C_Object_\w+>')
        field.update(kind='map',key={'kind':'string','type_path':''},value=leaf);del field['element']
        generate(self.ir)
    def test_reject_soft_wrong_carrier(self):
        self.add_soft()['type_path']+='bad'
        with self.assertRaises(ValueError):generate(self.ir)
    def test_reject_soft_missing_descriptor(self):
        del self.add_soft()['soft_reference']
        with self.assertRaises(ValueError):generate(self.ir)
    def test_reject_soft_bad_base(self):
        self.add_soft()['soft_reference']['base']='/Game/Example.Example_C'
        with self.assertRaises(ValueError):generate(self.ir)
    def test_reject_soft_wrong_kind(self):
        self.add_soft()['kind']='object'
        with self.assertRaises(ValueError):generate(self.ir)
    def test_reject_soft_invalid_boolean(self):
        self.add_soft()['soft_reference']['class_ref']=1
        with self.assertRaises(ValueError):generate(self.ir)
    def test_reject_soft_map_key(self):
        field=self.add_soft();leaf={k:v for k,v in field.items() if k in ('kind','type_path','soft_reference')}
        field.update(kind='map',type_path='',key=leaf,value={'kind':'int','type_path':''});del field['soft_reference']
        with self.assertRaises(ValueError):generate(self.ir)
    def test_requires_discovery(self):
        del self.ir['discovery']
        with self.assertRaisesRegex(ValueError,'discovery'):generate(self.ir)
    def test_stale_discovery_rejected(self):
        self.ir['discovery']['classes'][0]['name']='MissingView'
        with self.assertRaisesRegex(ValueError,'Stale native declaration'):generate(self.ir)
    def test_receiver_discovery_mismatch_rejected(self):
        self.ir['classes'].pop()
        with self.assertRaisesRegex(ValueError,'receiver/discovery mismatch'):generate(self.ir)
    def test_accessor_contract_in_cooked_header(self):
        field=next(c for c in self.ir['classes'] if c['fields'])['fields'][0]
        field['access']=dict(native_getter=True,native_setter=True,getter='ReadValue',setter='WriteValue',setter_unsafe=True)
        header,_=generate(self.ir)
        self.assertIn('NativeGetter,NativeSetter,GetterUnsafe,SetterUnsafe',header)
        self.assertIn('true,true,false,true,TEXT("ReadValue"),TEXT("WriteValue")',header)
    def test_accessor_function_name_rejected(self):
        next(c for c in self.ir['classes'] if c['fields'])['fields'][0]['access']={'getter':'unsafe.name'}
        with self.assertRaisesRegex(ValueError,'accessor function'):generate(self.ir)
    def test_unknown_accessor_key_rejected(self):
        next(c for c in self.ir['classes'] if c['fields'])['fields'][0]['access']={'native_getterr':True}
        with self.assertRaisesRegex(ValueError,'accessor contract'):generate(self.ir)
    def test_accessor_policy_needs_identity(self):
        next(c for c in self.ir['classes'] if c['fields'])['fields'][0]['access']={'setter_unsafe':True}
        with self.assertRaisesRegex(ValueError,'policy requires'):generate(self.ir)
    def test_construction_flag_cooked(self):
        method=next(c for c in self.ir['classes'] if c['methods'])['methods'][0];method['construction_unsafe']=True
        header,_=generate(self.ir)
        self.assertIn('bool ConstructionUnsafe;',header)
        self.assertIn('TEXT('+__import__('json').dumps(method['signature'])+'),true}',header)
    def test_construction_flag_must_be_boolean(self):
        next(c for c in self.ir['classes'] if c['methods'])['methods'][0]['construction_unsafe']=1
        with self.assertRaisesRegex(ValueError,'construction flag'):generate(self.ir)
if __name__=='__main__': unittest.main()
