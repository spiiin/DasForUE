import copy
import json
import unittest
import subprocess
import sys
import tempfile
from unittest import mock
from pathlib import Path
from GenerateBindings import generate, default_expr, enum_name, class_name

ROOT = Path(__file__).resolve().parents[1]
class GeneratorTests(unittest.TestCase):
    def setUp(self):
        self.ir = json.loads((ROOT / 'Bindings/Api.json').read_text(encoding='utf-8-sig'))
    def test_deterministic_and_current(self):
        expected = (ROOT / 'Plugins/DasForUE/Content/Das/Scripts/ue_generated.das').read_text()
        self.assertEqual(generate(self.ir), expected)
        self.ir['functions'].reverse()
        self.ir['structs'].reverse()
        self.ir['classes'].reverse()
        self.assertEqual(generate(self.ir), expected)
    def test_native_object_metadata_and_short_alias(self):
        text = generate(self.ir)
        self.assertIn('[ue_native_object(path="/Script/Engine.Actor")]', text)
        self.assertIn('typedef Actor = ' + class_name('/Script/Engine.Actor'), text)
    def test_ambiguous_native_short_name_is_not_exported(self):
        self.ir['classes'].append(dict(type_path='/Script/Other.Actor', parent='/Script/CoreUObject.Object'))
        text = generate(self.ir)
        self.assertNotIn('typedef Actor = ', text)
        self.assertIn('struct ' + class_name('/Script/Other.Actor'), text)
        self.assertIn('struct ' + class_name('/Script/Engine.Actor'), text)
    def test_native_short_alias_does_not_hide_function(self):
        alias = self.ir['functions'][0]['alias']
        self.ir['classes'].append(dict(type_path='/Script/Other.' + alias, parent='/Script/CoreUObject.Object'))
        text = generate(self.ir)
        self.assertNotIn('typedef ' + alias + ' = ', text)
        self.assertIn('def ' + alias + '(', text)
    def test_schema_version(self):
        self.ir['schema_version'] = 9
        with self.assertRaises(ValueError): generate(self.ir)
    def test_unsupported_field(self):
        self.ir['functions'][0]['fields'][0]['kind'] = 'array'
        with self.assertRaises(ValueError): generate(self.ir)
    def test_alias_injection(self):
        self.ir['functions'][0]['alias'] = 'actor_x()'
        with self.assertRaises(ValueError): generate(self.ir)
    def test_duplicate_alias(self):
        self.ir['functions'].append(copy.deepcopy(self.ir['functions'][0]))
        with self.assertRaises(ValueError): generate(self.ir)
    def test_field_collision(self):
        fields = next(f['fields'] for f in self.ir['functions'] if f['function'] == 'MutateValues')
        fields[0]['path'] = 'a.b'
        fields[1]['path'] = 'a_b'
        with self.assertRaises(ValueError): generate(self.ir)
    def test_enum_conflict(self):
        fields = [p for f in self.ir['functions'] for p in f['fields'] if p['kind'] == 'enum' and 'EDasCallMode' in p['type_path']]
        fields[-1]['members'][0]['value'] = '999'
        with self.assertRaises(ValueError): generate(self.ir)
    def test_generated_type_collision(self):
        self.ir['functions'][0]['alias'] = 'fixture_MutateValues_Result'
        with self.assertRaises(ValueError): generate(self.ir)

    def test_missing_struct(self):
        self.ir['structs'].pop()
        with self.assertRaisesRegex(ValueError, 'Missing struct'): generate(self.ir)
    def test_recursive_struct(self):
        item = self.ir['structs'][0]
        item['fields'][0].update(kind='struct', type_path=item['type_path'])
        with self.assertRaisesRegex(ValueError, 'Recursive value struct'): generate(self.ir)
    def test_duplicate_struct(self):
        self.ir['structs'].append(copy.deepcopy(self.ir['structs'][0]))
        with self.assertRaisesRegex(ValueError, 'duplicate struct'): generate(self.ir)
    def test_tree_leaf_mismatch(self):
        self.ir['structs'][0]['fields'][0]['kind'] = 'bool'
        with self.assertRaisesRegex(ValueError, 'does not match'): generate(self.ir)
    def test_struct_field_collision(self):
        fields = self.ir['structs'][0]['fields']
        fields[0]['path'] = 'a-b'
        fields[1]['path'] = 'a_b'
        with self.assertRaisesRegex(ValueError, 'colliding struct field'): generate(self.ir)
    def test_object_struct_field_rejected(self):
        self.ir['structs'][0]['fields'][0]['kind'] = 'object'
        with self.assertRaisesRegex(ValueError, 'Unsupported struct leaf'): generate(self.ir)
    def test_parameter_direction_type(self):
        self.ir['functions'][0]['parameters'][0]['input'] = 1
        with self.assertRaisesRegex(ValueError, 'parameter direction'): generate(self.ir)


    def test_missing_parent_class(self):
        self.ir['classes'][0]['parent'] = '/Script/Missing.Class'
        with self.assertRaisesRegex(ValueError, 'Missing parent'): generate(self.ir)
    def test_duplicate_class(self):
        self.ir['classes'].append(copy.deepcopy(self.ir['classes'][0]))
        with self.assertRaisesRegex(ValueError, 'duplicate class'): generate(self.ir)
    def test_class_cycle(self):
        self.ir['classes'][0]['parent'] = self.ir['classes'][0]['type_path']
        with self.assertRaisesRegex(ValueError, 'Cyclic class'): generate(self.ir)
    def test_unknown_receiver(self):
        self.ir['functions'][0]['class'] = '/Script/Missing.Class'
        with self.assertRaisesRegex(ValueError, 'Missing receiver'): generate(self.ir)
    def test_unknown_object_type(self):
        field = next(p for f in self.ir['functions'] for p in f['fields'] if p['kind'] == 'object')
        field['type_path'] = '/Script/Missing.Class'
        with self.assertRaisesRegex(ValueError, 'Missing object'): generate(self.ir)

    def test_metadata_does_not_change_execution(self):
        expected = generate(self.ir)
        for fn in self.ir['functions']:
            fn['metadata'] = {'DisplayName': 'ignored {text}', 'ToolTip': 'documentation'}
        self.assertEqual(expected, generate(self.ir))

class DefaultTests(unittest.TestCase):
    def field(self, kind, text, **extra):
        return dict(kind=kind, default_text=text, input=True, output=False, type_path=extra.pop('type_path', ''), **extra)
    def test_int_bounds(self):
        for text, expected in [('7','7'), ('+007','7'), ('-2147483648','(-2147483647 - 1)'), ('2147483647','2147483647')]:
            self.assertEqual(default_expr(self.field('int',text), {}), expected)
        for text in ['2147483648', '-2147483649', '1; panic("x")', '0x10', '7.0']:
            with self.subTest(text=text), self.assertRaises(ValueError): default_expr(self.field('int',text), {})
    def test_numeric_literals(self):
        self.assertEqual(default_expr(self.field('double','1.23456789012345'), {}), '1.23456789012345lf')
        self.assertEqual(default_expr(self.field('float','1.5'), {}), '1.5')
        self.assertEqual(default_expr(self.field('double','-0.0'), {}), '-0.0lf')
        for kind,text in [('float','1e99'), ('float','1e-99'), ('double','1e999'), ('double','1e-999'), ('double','nan'), ('float','1.5f')]:
            with self.subTest(kind=kind,text=text), self.assertRaises(ValueError): default_expr(self.field(kind,text), {})
    def test_bool_is_strict(self):
        self.assertEqual(default_expr(self.field('bool','false'), {}), 'false')
        for text in ['1','False','yes']:
            with self.assertRaises(ValueError): default_expr(self.field('bool',text), {})
    def test_string_is_data(self):
        self.assertEqual(default_expr(self.field('string','{x}'), {}), '"' + chr(92) + '{x' + chr(92) + '}"')
        self.assertEqual(default_expr(self.field('string',''), {}), '""')
        with self.assertRaises(ValueError): default_expr(self.field('string',chr(0)), {})
    def test_name_default(self):
        self.assertEqual(default_expr(self.field('name','None'), {}), 'ue_name("None")')
        with self.assertRaises(ValueError): default_expr(self.field('name','x'*1024), {})
    def test_enum_default(self):
        path='/Script/Test.Mode'
        enums={path:[dict(name='Active',value='7')]}
        for text in ['Active','Mode::Active']:
            self.assertEqual(default_expr(self.field('enum',text,type_path=path), enums), enum_name(path)+'.v_Active')
        for text in ['7','Other::Active','Unknown']:
            with self.assertRaises(ValueError): default_expr(self.field('enum',text,type_path=path), enums)
    def test_object_null_only(self):
        path='/Script/Engine.Actor'
        self.assertEqual(default_expr(self.field('object','None',type_path=path), {}), class_name(path)+'_null()')
        with self.assertRaises(ValueError): default_expr(self.field('object','/Game/Asset',type_path=path), {})
    def test_unsupported_struct_and_direction(self):
        with self.assertRaises(ValueError): default_expr(self.field('struct','(X=0)'), {})
        p=self.field('int','7');p['output']=True
        with self.assertRaises(ValueError): default_expr(p,{})
    def test_default_context_in_error(self):
        ir=json.loads((ROOT/'Bindings/Api.json').read_text())
        fn=next(f for f in ir['functions'] if f['function']=='AddHealth')
        fn['parameters'][0]['default_text']='arbitrary_cpp()'
        with self.assertRaisesRegex(ValueError,'fixture_AddHealth.value'): generate(ir)
    def test_failed_default_preserves_existing_module(self):
        ir=json.loads((ROOT/'Bindings/Api.json').read_text())
        fn=next(f for f in ir['functions'] if f['function']=='AddHealth')
        fn['parameters'][0]['default_text']='arbitrary_cpp()'
        with tempfile.TemporaryDirectory() as directory:
            source=Path(directory)/'api.json'; output=Path(directory)/'module.das'
            source.write_text(json.dumps(ir)); output.write_text('previous module')
            result=subprocess.run([sys.executable,str(ROOT/'Scripts/GenerateBindings.py'),str(source),str(output)],capture_output=True,text=True)
            self.assertNotEqual(result.returncode,0)
            self.assertIn('fixture_AddHealth.value',result.stderr)
            self.assertEqual(output.read_text(),'previous module')
    def test_changed_default_changes_module(self):
        ir=json.loads((ROOT/'Bindings/Api.json').read_text())
        before=generate(ir)
        fn=next(f for f in ir['functions'] if f['function']=='PolicyDefault')
        fn['parameters'][0]['default_text']='9'
        self.assertNotEqual(generate(ir),before)
        self.assertIn('arg_value : int = 9',generate(ir))
    def test_required_after_optional_rejected(self):
        ir=json.loads((ROOT/'Bindings/Api.json').read_text())
        fn=next(f for f in ir['functions'] if f['function']=='TransformPayload')
        next(p for p in fn['parameters'] if p['path']=='delta')['default_text']='1'
        with self.assertRaisesRegex(ValueError,'Required input after default'): generate(ir)

class ArrayTests(unittest.TestCase):
    def setUp(self):
        self.ir=json.loads((ROOT/'Bindings/Api.json').read_text())
        self.param=next(f for f in self.ir['functions'] if f['function']=='TransformInts')['parameters'][0]
    def test_element_kind_mismatch(self):
        self.param['element']['kind']='float'
        with self.assertRaisesRegex(ValueError,'array element'): generate(self.ir)
    def test_missing_element(self):
        self.param.pop('element')
        with self.assertRaisesRegex(ValueError,'array element'): generate(self.ir)
    def test_object_element_marker_mismatch(self):
        self.param['element'].update(kind='object',type_path='/Script/Engine.Actor')
        with self.assertRaisesRegex(ValueError,'array element'): generate(self.ir)
    def test_nested_arrays_rejected(self):
        self.param['element']['kind']='array'
        with self.assertRaisesRegex(ValueError,'array element'): generate(self.ir)
    def test_array_defaults_rejected(self):
        self.param['default_text']='()'
        with self.assertRaisesRegex(ValueError,'Unsupported default'): generate(self.ir)

class ComplexArrayTests(unittest.TestCase):
    def setUp(self):
        self.ir=json.loads((ROOT/'Bindings/Api.json').read_text())
    def param(self, function):
        return next(f for f in self.ir['functions'] if f['function']==function)['parameters'][0]
    def test_missing_object_array_class(self):
        self.param('EchoFixtureObjects')['element']['type_path']='/Script/Missing.Class'
        with self.assertRaisesRegex(ValueError,'Missing object class'): generate(self.ir)
    def test_missing_struct_array_type(self):
        self.param('TransformPayloads')['element']['type_path']='/Script/Missing.Struct'
        with self.assertRaisesRegex(ValueError,'Missing struct definition'): generate(self.ir)
    def test_missing_enum_array_type(self):
        self.param('EchoModes')['element']['type_path']='/Script/Missing.Enum'
        with self.assertRaisesRegex(ValueError,'Missing enum definition'): generate(self.ir)
    def test_array_struct_leaf_mismatch(self):
        fn=next(f for f in self.ir['functions'] if f['function']=='TransformPayloads')
        next(f for f in fn['fields'] if f['path']=='values.*.count')['kind']='bool'
        with self.assertRaisesRegex(ValueError,'does not match'): generate(self.ir)
    def test_array_object_leaf_identity_mismatch(self):
        fn=next(f for f in self.ir['functions'] if f['function']=='EchoFixtureObjects')
        next(f for f in fn['fields'] if f['path']=='values.*')['type_path']='/Script/Engine.Actor'
        with self.assertRaisesRegex(ValueError,'does not match'): generate(self.ir)
    def test_array_wildcard_required(self):
        fn=next(f for f in self.ir['functions'] if f['function']=='TransformInts')
        fn['fields']=[f for f in fn['fields'] if '.*' not in f['path']]
        with self.assertRaisesRegex(ValueError,'does not match'): generate(self.ir)

class HashTests(unittest.TestCase):
    def setUp(self): self.ir=json.loads((ROOT/'Bindings/Api.json').read_text())
    def param(self, function): return next(f for f in self.ir['functions'] if f['function']==function)['parameters'][0]
    def test_map_marker_mismatch(self):
        self.param('TransformMap')['value']['kind']='int'
        with self.assertRaisesRegex(ValueError,'hash container member'): generate(self.ir)
    def test_missing_map_member(self):
        self.param('TransformMap').pop('key')
        with self.assertRaisesRegex(ValueError,'hash container member'): generate(self.ir)
    def test_object_map_value_rejected(self):
        p=self.param('TransformMap');p['type_path']='IntProperty;ObjectProperty';p['value'].update(kind='object',type_path='/Script/Engine.Actor')
        with self.assertRaisesRegex(ValueError,'hash container member'): generate(self.ir)
    def test_nested_set_member_rejected(self):
        self.param('TransformSet')['element']['kind']='array'
        with self.assertRaisesRegex(ValueError,'hash container member'): generate(self.ir)
    def test_hash_leaf_mismatch(self):
        f=next(f for f in self.ir['functions'] if f['function']=='TransformMap')
        next(p for p in f['fields'] if p['path']=='values.*.key')['kind']='string'
        with self.assertRaisesRegex(ValueError,'does not match'): generate(self.ir)
    def test_hash_default_rejected(self):
        self.param('TransformSet')['default_text']='()'
        with self.assertRaisesRegex(ValueError,'Unsupported default'): generate(self.ir)
    def test_hash_symbol_collision(self):
        with mock.patch('GenerateBindings.hash_container_name',return_value='H_Collision'):
            with self.assertRaisesRegex(ValueError,'symbol collision'): generate(self.ir)

if __name__ == '__main__': unittest.main()
