import copy
import re
import json
import unittest
import subprocess
import sys
import tempfile
from unittest import mock
from pathlib import Path
from GenerateBindings import generate, default_expr, enum_name, class_name, native_type_names, dsl_base_names, \
    skipped_aliases, PLUGIN_SCRIPTS, \
    NATIVE_BINDS_SOURCE

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
    def static_fixture(self):
        fn = self.ir['functions'][0]
        library = '/Script/Test.Library'
        self.ir['classes'].extend([dict(type_path='/Script/Engine.BlueprintFunctionLibrary', parent='/Script/CoreUObject.Object'), dict(type_path=library, parent='/Script/Engine.BlueprintFunctionLibrary')])
        fn.update({'static': True, 'world_context': '', 'class': library})
        return fn
    def test_static_reflection_macro_descriptor(self):
        fn = self.static_fixture()
        text = generate(self.ir)
        expected = '[ue_reflected_call(owner="' + fn['class'] + '", method="' + fn['function'] + '", signature="' + fn['signature'].replace('\\', '\\\\').replace('"', '\\"').replace('{', '\\{').replace('}', '\\}') + '")]'
        self.assertIn(expected + '\ndef ' + fn['alias'] + '(', text)

    def test_static_library_has_no_receiver(self):
        fn = self.static_fixture()
        body = generate(self.ir).split('def ' + fn['alias'] + '(', 1)[1].split('\n}\n', 1)[0]
        self.assertNotIn('target :', body)
        self.assertIn('ue_begin_static_call(', body)
        self.assertIn('ue_expect_signature(', body)
    def test_world_context_is_hidden_from_wrapper_inputs(self):
        fn = self.static_fixture()
        field = dict(path='world', kind='object', type_path='/Script/CoreUObject.Object', input=True, output=False)
        fn['parameters'].insert(0, field.copy())
        fn['fields'].insert(0, field.copy())
        fn['world_context'] = 'world'
        body = generate(self.ir).split('def ' + fn['alias'] + '(', 1)[1].split('\n}\n', 1)[0]
        self.assertNotIn('arg_world', body)
        self.assertIn('"world")', body)
    def test_world_context_requires_input_only_object(self):
        fn = self.static_fixture()
        field = dict(path='world', kind='object', type_path='/Script/CoreUObject.Object', input=True, output=True)
        fn['parameters'].insert(0, field.copy())
        fn['fields'].insert(0, field.copy())
        fn['world_context'] = 'world'
        with self.assertRaisesRegex(ValueError, 'WorldContext'): generate(self.ir)
    def test_class_parameters_and_output_use_handles(self):
        fn = self.static_fixture()
        fn['parameters'] = [dict(path='value',kind='class',type_path='/Script/Engine.Actor',input=True,output=False),dict(path='returnvalue',kind='class',type_path='/Script/Engine.Actor',input=False,output=True)]
        fn['fields'] = copy.deepcopy(fn['parameters'])
        body = generate(self.ir).split('def ' + fn['alias'] + '(', 1)[1].split('\n}\n', 1)[0]
        self.assertIn('arg_value : UEClassHandle', body)
        self.assertIn('ue_arg_class(', body)
        self.assertIn('ue_result_class(', body)
    def test_class_requires_metaclass_identity(self):
        fn = self.static_fixture()
        fn['fields'][0].update(kind='class',type_path='/Script/Missing.Base')
        with self.assertRaisesRegex(ValueError,'MetaClass'): generate(self.ir)
    def test_null_class_default(self):
        self.assertEqual(default_expr(dict(kind='class',default_text='None',input=True,output=False),{}),'ue_class_null()')
    def test_native_static_on_nonlibrary_has_no_receiver(self):
        fn = self.ir['functions'][0]
        fn['static'] = True
        body = generate(self.ir).split('def ' + fn['alias'] + '(', 1)[1].split('\n}\n', 1)[0]
        self.assertNotIn('target :', body)
        self.assertIn('ue_begin_static_call(', body)
        self.assertIn('ue_expect_signature(', body)
    def test_authored_static_rejected(self):
        fn = self.static_fixture()
        fn['class'] = '/Game/Generated.Test'
        self.ir['classes'].append(dict(type_path=fn['class'],parent='/Script/CoreUObject.Object'))
        with self.assertRaisesRegex(ValueError, 'native /Script/'): generate(self.ir)
    def test_invalid_world_context_rejected(self):
        fn = self.static_fixture()
        fn['world_context'] = 'missing'
        with self.assertRaisesRegex(ValueError, 'WorldContext'): generate(self.ir)
    def test_static_marker_requires_boolean(self):
        self.ir['functions'][0]['static'] = 'yes'
        with self.assertRaisesRegex(ValueError, 'static marker'): generate(self.ir)
    def test_native_object_metadata_and_short_alias(self):
        text = generate(self.ir, native_names=frozenset(), dsl_bases={})
        self.assertIn('[ue_native_object(path="/Script/Engine.Actor")]', text)
        self.assertIn('typedef Actor = ' + class_name('/Script/Engine.Actor'), text)
    def test_native_bind_type_name_has_no_short_alias(self):
        # A native bind type named Actor: a generated alias would make the name ambiguous.
        actor = class_name('/Script/Engine.Actor')
        self.ir['classes'].append(dict(type_path='/Script/Engine.Light', parent='/Script/Engine.Actor'))
        text = generate(self.ir, native_names=frozenset({'Actor'}), dsl_bases={})
        self.assertNotIn('typedef Actor = ', text)
        self.assertIn('// Actor: no short alias, the name belongs to the native bind type Actor (DasNativeBinds.h); '
                      'use ' + actor, text)
        self.assertIn('struct ' + actor + ' { handle : UObjectHandle }', text)
        self.assertIn('def ' + actor + '_from(value : UObjectHandle)', text)
        self.assertIn('typedef Light = ' + class_name('/Script/Engine.Light'), text)
        self.assertEqual(skipped_aliases(text), ['Actor'])
    def test_dsl_base_class_has_no_short_alias(self):
        # ue_dsl declares `class Actor` as a DSL base: `var a : Actor` would be ambiguous next to an alias.
        actor = class_name('/Script/Engine.Actor')
        text = generate(self.ir, native_names=frozenset(), dsl_bases={'Actor': 'ue_dsl'})
        self.assertNotIn('typedef Actor = ', text)
        self.assertIn('// Actor: no short alias, the name belongs to the DSL base class ue_dsl::Actor; use ' + actor, text)
        self.assertEqual(skipped_aliases(text), ['Actor'])
    def test_plugin_reserved_names(self):
        self.assertEqual(sorted(native_type_names([NATIVE_BINDS_SOURCE])),
                         ['AActor', 'APawn', 'UActorComponent', 'UGameInstance', 'USceneComponent', 'UWorld'])
        self.assertEqual(dsl_base_names(PLUGIN_SCRIPTS),
                         dict(ProbeActor='ue_probe_actor', **{name: 'ue_dsl' for name in ('Actor', 'Pawn', 'Character', 'ActorComponent', 'SaveGame',
                                                      'DataAsset', 'PrimaryDataAsset', 'WorldSubsystem',
                                                      'GameInstanceSubsystem')}))
        text = generate(self.ir)
        self.assertEqual(skipped_aliases(text), ['Actor'])
        self.assertIn('the name belongs to the DSL base class ue_dsl::Actor', text)
    def test_checked_in_modules_have_no_reserved_aliases(self):
        # Generated modules whose IR comes from an editor export are checked here instead of by regeneration.
        reserved = set(native_type_names([NATIVE_BINDS_SOURCE])) | set(dsl_base_names(PLUGIN_SCRIPTS))
        offenders = []
        for path in sorted(PLUGIN_SCRIPTS.glob('*.das')):
            text = path.read_text(encoding='utf-8', errors='replace')
            if '// Generated by Scripts/GenerateBindings.py' not in text:
                continue
            offenders += [path.name + ': ' + name for name in
                          re.findall(r'^typedef ([A-Za-z_][A-Za-z0-9_]*) = ', text, re.M) if name in reserved]
        self.assertEqual(offenders, [])
    def test_dsl_base_names_scan(self):
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            (root / 'bases.das').write_text('options gen2\nmodule my_bases public\n'
                                            '[ue_native_base(path="/Script/Engine.Actor")]\nclass Actor {}\n'
                                            '[ue_native_base(path="/Script/Engine.Pawn"), ue_class(Blueprintable)]\n'
                                            'class Mixed {}\nclass Plain {}\n')
            (root / 'test.das').write_text('options gen2\n[ue_native_base(path="/Script/Engine.Actor")]\nclass Bad {}\n')
            self.assertEqual(dsl_base_names(root), {'Actor': 'my_bases'})
    def test_native_type_names_scan(self):
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            (root / 'Binds.cpp').write_text('// Class<AOld>(TEXT("Commented"))\nDAS_NATIVE_BINDS(Game, "game")\n{\n'
                                            '    Binds.Class<AFoo>(TEXT("Foo")).Method<&Get>(TEXT("get"));\n'
                                            '    Binds.Class<TMyTemplate<int>>( TEXT("Templated"));\n'
                                            '    Binds.Class<AActor>().Method<&Extra>(TEXT("extra"));\n}\n')
            (root / 'Other.cpp').write_text('auto X = Registry.Class<ABar>(TEXT("Bar"));\n')
            (root / 'Notes.txt').write_text('DAS_NATIVE_BINDS(X, "x") Class<ABaz>(TEXT("Baz"))\n')
            self.assertEqual(sorted(native_type_names([root])), ['Foo', 'Templated'])
    def test_ambiguous_native_short_name_is_not_exported(self):
        self.ir['classes'].append(dict(type_path='/Script/Other.Actor', parent='/Script/CoreUObject.Object'))
        text = generate(self.ir, native_names=frozenset(), dsl_bases={})
        self.assertNotIn('typedef Actor = ', text)
        self.assertIn('struct ' + class_name('/Script/Other.Actor'), text)
        self.assertIn('struct ' + class_name('/Script/Engine.Actor'), text)
    def test_native_short_alias_does_not_hide_function(self):
        alias = self.ir['functions'][0]['alias']
        self.ir['classes'].append(dict(type_path='/Script/Other.' + alias, parent='/Script/CoreUObject.Object'))
        text = generate(self.ir)
        self.assertNotIn('typedef ' + alias + ' = ', text)
        self.assertIn('def ' + alias + '(', text)
    def test_interface_receiver_uses_membership_and_declared_signature(self):
        text = generate(self.ir)
        self.assertIn('[ue_native_interface(path="/Script/DasRuntime.DasProbeInterface")]', text)
        self.assertIn('def DasProbeInterface_from(value : UObjectHandle)', text)
        body = text.split('def ue_probe_Query(', 1)[1].split('\n}\n', 1)[0]
        self.assertIn('ue_begin_interface_call(', body)
        self.assertIn('ue_expect_interface_signature(', body)
        self.assertNotIn('ue_cast_object(', body)
    def test_interface_marker_must_be_boolean(self):
        self.ir['classes'][0]['interface'] = 'true'
        with self.assertRaisesRegex(ValueError, 'interface class marker'): generate(self.ir)
    def test_ambiguous_interface_alias_does_not_emit_helpers(self):
        self.ir['classes'].append(dict(type_path='/Script/Other.DasProbeInterface', parent='/Script/CoreUObject.Interface', interface=True))
        text = generate(self.ir)
        self.assertNotIn('typedef DasProbeInterface = ', text)
        self.assertNotIn('def DasProbeInterface_from(', text)
    def test_interface_helpers_do_not_hide_selected_function(self):
        self.ir['functions'][0]['alias'] = 'ue_DasProbeInterface_from'
        self.ir['classes'].append(dict(type_path='/Script/Other.ue_DasProbeInterface', parent='/Script/CoreUObject.Interface', interface=True))
        text = generate(self.ir)
        self.assertEqual(text.count('def ue_DasProbeInterface_from('), 1)
        self.assertNotIn('def ue_DasProbeInterface_raw(', text)
    def authored_ir(self, identity=True):
        old = '/Script/DasRuntime.DasProbeInterface'
        path = '/Game/Contracts/Probe.Probe_C'
        for item in self.ir['classes']:
            if item['type_path'] == old:
                item['type_path'] = path
                if identity: item['interface_id'] = 'contracts.Probe'
        for fn in self.ir['functions']:
            if fn['class'] == old: fn['class'] = path
        return path
    def test_authored_interface_identity_metadata(self):
        path = self.authored_ir()
        text = generate(self.ir)
        self.assertIn('[ue_interface_ref(path="' + path + '", id="contracts.Probe")]', text)
        self.assertIn('typedef Probe = ', text)
        self.assertIn('def Probe_from(', text)
    def test_authored_interface_missing_identity_refused(self):
        self.authored_ir(False)
        with self.assertRaisesRegex(ValueError, 'stable identity'): generate(self.ir)
    def test_authored_interface_bad_identity_refused(self):
        path = self.authored_ir()
        next(c for c in self.ir['classes'] if c['type_path'] == path)['interface_id'] = 'bad identity'
        with self.assertRaisesRegex(ValueError, 'stable identity'): generate(self.ir)
    def test_custom_binding_module(self):
        self.assertIn('module ue_contracts public', generate(self.ir, 'ue_contracts'))
    def test_binding_module_injection_refused(self):
        with self.assertRaisesRegex(ValueError, 'module name'): generate(self.ir, 'bad; panic()')
    def test_native_interface_must_not_have_authored_identity(self):
        next(c for c in self.ir['classes'] if c.get('interface'))['interface_id'] = 'contracts.Probe'
        with self.assertRaisesRegex(ValueError, 'interface identity'): generate(self.ir)
    def test_interface_method_facade_and_copyback(self):
        text = generate(self.ir)
        self.assertIn('def Query(target : ' + class_name('/Script/DasRuntime.DasProbeInterface'), text)
        stats = text.split('def Stats(', 1)[1].split('\n}\n', 1)[0]
        self.assertIn('var arg_counter : int&', stats)
        self.assertIn('var arg_total : int&', stats)
        self.assertIn('let result = ue_probe_Stats(target, arg_counter)', stats)
        self.assertIn('arg_counter = result.out_counter', stats)
        self.assertIn('arg_total = result.out_total', stats)
    def test_interface_container_facade_moves_owned_result(self):
        body = generate(self.ir).split('def Weights(', 1)[1].split('\n}\n', 1)[0]
        self.assertIn('var result <- ue_probe_Weights', body)
        self.assertIn('return <- result', body)
    def test_duplicate_interface_alias_does_not_duplicate_method(self):
        duplicate = copy.deepcopy(next(f for f in self.ir['functions'] if f['alias'] == 'ue_probe_Query'))
        duplicate['alias'] = 'ue_extra_Query'
        self.ir['functions'].append(duplicate)
        self.assertEqual(generate(self.ir).count('def Query('), 1)
    def test_typed_interface_factory_is_specific(self):
        text = generate(self.ir)
        self.assertIn('def ue_interface(requested : type<' + class_name('/Script/DasRuntime.DasProbeInterface') + '>', text)
        self.assertNotIn('type<' + class_name('/Script/Engine.Actor') + '>', text)
    def test_source_manifest_needs_explicit_registry(self):
        manifest = dict(module='ue_values',registry='',types=[])
        with self.assertRaisesRegex(ValueError,'registry'): generate(self.ir, source_types=manifest)
    def test_source_manifest_module_injection_refused(self):
        manifest = dict(module='bad; code',registry='/Game/Test.Registry',types=[])
        with self.assertRaisesRegex(ValueError,'module'): generate(self.ir, source_types=manifest)
    def test_source_manifest_unknown_enum_refused(self):
        manifest = dict(module='ue_values',registry='/Game/Test.Registry',types=[dict(kind='enum',path='/Game/Types/Missing.Missing',name='State',id='types.State',members={'Active':42})])
        with self.assertRaisesRegex(ValueError,'enum values'): generate(self.ir, source_types=manifest)
    def nested_source_fixture(self):
        child = '/Game/Test/Child.Child'
        parent = '/Game/Test/Parent.Parent'
        self.ir['structs'].extend([
            dict(type_path=child,cpp_type='Child',fields=[dict(path='value_native',kind='int',type_path='')]),
            dict(type_path=parent,cpp_type='Parent',fields=[dict(path='nested_native',kind='struct',type_path=child),dict(path='items_native',kind='array',type_path='StructProperty',element=dict(kind='struct',path='items_native',type_path=child))])])
        manifest = dict(module='ue_values',registry='/Game/Test/Registry.Registry',types=[
            dict(path=parent,kind='struct',name='Parent',id='test.Parent',members={'nested_native':'Nested','items_native':'Items'}),
            dict(path=child,kind='struct',name='Child',id='test.Child',members={'value_native':'Value'})])
        return manifest
    def test_nested_source_conversion_copies_owned_array_elements(self):
        text = generate(self.ir,source_types=self.nested_source_fixture())
        self.assertIn('resize(result.f_items_native, length(value.Items))',text)
        self.assertIn('source_index_0',text)
        self.assertIn('result.Items[source_index_0]',text)
        self.assertIn('return <- result',text)
    def test_nested_source_mapping_requires_child_identity(self):
        manifest = self.nested_source_fixture()
        manifest['types'].pop()
        with self.assertRaisesRegex(ValueError,'explicit matching mapping'): generate(self.ir,source_types=manifest)
    def test_source_struct_direct_nested_array_refused(self):
        manifest = self.nested_source_fixture()
        field=self.ir['structs'][-1]['fields'][-1]
        field['element'] = dict(kind='array',path='items_native',type_path='IntProperty',element=dict(kind='int',type_path=''))
        field['type_path']='ArrayProperty'
        with self.assertRaisesRegex(ValueError,'struct array member'): generate(self.ir,source_types=manifest)
    def test_source_name_and_object_conversion(self):
        manifest=self.nested_source_fixture()
        self.ir['structs'][-2]['fields'].extend([dict(path='key_native',kind='name',type_path=''),dict(path='actor_native',kind='object',type_path='/Script/Engine.Actor')])
        manifest['types'][-1]['members'].update(key_native='Key',actor_native='Target')
        text=generate(self.ir,source_types=manifest)
        self.assertIn('ue_name(ue_name_text(value.Key))',text)
        self.assertIn('result.Key = ue_name_value(value.f_key_native.value)',text)
        self.assertIn(class_name('/Script/Engine.Actor')+'_from(value.Target.handle)',text)
        self.assertIn('result.Target.handle = ue_generated::'+class_name('/Script/Engine.Actor')+'_raw(value.f_actor_native)',text)
    def test_source_bindings_qualify_internal_symbols(self):
        manifest=self.nested_source_fixture()
        text=generate(self.ir,'ue_custom',manifest,native_names=frozenset(),dsl_bases={})
        actor=class_name('/Script/Engine.Actor')
        self.assertIn('typedef Actor = ue_custom::'+actor,text)
        self.assertIn('def '+actor+'_from(value : UObjectHandle) : ue_custom::'+actor,text)
        self.assertIn('return ue_custom::'+actor+'(handle = ',text)
        self.assertIn('struct '+actor+' { handle : UObjectHandle }',text)
        self.assertIn('[ue_native_object(path="/Script/Engine.Actor")]',text)
    def test_source_qualification_preserves_shadowing_local(self):
        manifest=self.nested_source_fixture()
        self.ir['classes'].append(dict(type_path='/Script/Other.result',parent='/Script/CoreUObject.Object'))
        text=generate(self.ir,'ue_custom',manifest)
        self.assertIn('typedef result = ue_custom::',text)
        self.assertIn('let result = ue_custom::ue_probe_Query(target, arg_input)',text)
        self.assertIn('return result',text)
        self.assertNotIn('let ue_custom::result',text)
    def test_source_struct_interface_reference_conversion(self):
        manifest=self.nested_source_fixture()
        field=self.ir['structs'][-2]['fields'][0]
        field.update(kind='object',type_path='/Script/DasRuntime.DasProbeInterface')
        text=generate(self.ir,source_types=manifest)
        self.assertIn(class_name('/Script/DasRuntime.DasProbeInterface')+'_from(value.Value.handle)',text)
    def source_hash_fixture(self):
        manifest=self.nested_source_fixture()
        fields=self.ir['structs'][-2]['fields']
        fields.extend([dict(path='weights_native',kind='map',type_path='StringProperty;DoubleProperty',key=dict(kind='string',type_path=''),value=dict(kind='double',type_path='')),dict(path='tags_native',kind='set',type_path='StrProperty',element=dict(kind='string',type_path=''))])
        fields[-2]['type_path']='StrProperty;DoubleProperty'
        manifest['types'][-1]['members'].update(weights_native='Weights',tags_native='Tags')
        return manifest
    def test_source_nested_hash_conversion(self):
        text=generate(self.ir,source_types=self.source_hash_fixture())
        self.assertIn('keys(value.Weights)',text)
        self.assertIn('values(value.Weights)',text)
        self.assertNotIn('value.Weights[source_key_0]',text)
        self.assertIn('insert(result.Tags,',text)
        self.assertIn('duplicate source hash key/element',text)
        self.assertLess(text.index('struct H_Map_string_double_'),text.index('struct S_Child_'))
    def test_nested_hash_invalid_marker_rejected(self):
        manifest=self.source_hash_fixture()
        self.ir['structs'][-2]['fields'][-2]['type_path']='IntProperty;DoubleProperty'
        with self.assertRaisesRegex(ValueError,'hash container member'):generate(self.ir,source_types=manifest)
    def test_source_hash_object_value_rejected(self):
        manifest=self.source_hash_fixture()
        field=self.ir['structs'][-2]['fields'][-2]
        field['type_path']='StrProperty;ObjectProperty';field['value'].update(kind='object',type_path='/Script/Engine.Actor')
        with self.assertRaisesRegex(ValueError,'hash container member'):generate(self.ir,source_types=manifest)
    def test_source_hash_name_key_rejected(self):
        manifest=self.source_hash_fixture()
        field=self.ir['structs'][-2]['fields'][-2]
        field['type_path']='NameProperty;DoubleProperty';field['key'].update(kind='name',type_path='')
        with self.assertRaisesRegex(ValueError,'int/string'):generate(self.ir,source_types=manifest)
    def test_source_struct_array_method_facade(self):
        manifest=self.nested_source_fixture()
        child=self.ir['structs'][-2]['type_path']
        fields=self.ir['structs'][-2]['fields']
        fn=copy.deepcopy(next(f for f in self.ir['functions'] if f['alias']=='ue_probe_Query'))
        fn.update(function='EchoDetails',alias='ue_probe_EchoDetails')
        fn['parameters']=[dict(path='values',kind='array',type_path='StructProperty',element=dict(kind='struct',type_path=child),input=True,output=False),dict(path='returnvalue',kind='array',type_path='StructProperty',element=dict(kind='struct',type_path=child),input=False,output=True)]
        fn['fields']=[]
        for p in fn['parameters']:
            fn['fields'].append({k:p[k] for k in ('path','kind','type_path','input','output')})
            for f in fields:fn['fields'].append(dict(path=p['path']+'.*.'+f['path'],kind=f['kind'],type_path=f['type_path'],input=p['input'],output=p['output']))
        self.ir['functions'].append(fn)
        text=generate(self.ir,'ue_custom',manifest)
        self.assertIn('arg_values : array<ue_values::Child>',text)
        self.assertIn('resize(native_arg_values, length(arg_values))',text)
        self.assertIn('source_out_returnvalue : array<ue_values::Child>',text)
        self.assertIn('return <- source_out_returnvalue',text)
    def test_source_struct_array_ref_copyback_uses_converted_output(self):
        manifest=self.nested_source_fixture();child=self.ir['structs'][-2]['type_path']
        fn=copy.deepcopy(next(f for f in self.ir['functions'] if f['alias']=='ue_probe_Query'))
        fn.update(function='BumpDetails',alias='ue_probe_BumpDetails')
        fn['parameters']=[dict(path='values',kind='array',type_path='StructProperty',element=dict(kind='struct',type_path=child),input=True,output=True)]
        fn['fields']=[dict(path='values',kind='array',type_path='StructProperty',input=True,output=True),dict(path='values.*.value_native',kind='int',type_path='',input=True,output=True)]
        self.ir['functions'].append(fn)
        body=generate(self.ir,'ue_custom',manifest).split('def BumpDetails(',1)[1].split('\n}\n',1)[0]
        self.assertIn('var arg_values : array<ue_values::Child>&',body)
        self.assertIn('arg_values <- source_out_values',body)
        self.assertLess(body.index('resize(source_out_values'),body.index('arg_values <- source_out_values'))
    def test_interface_byte_text_ref_out_and_arrays(self):
        fn=copy.deepcopy(next(f for f in self.ir['functions'] if f['alias']=='ue_probe_Query'))
        fn.update(function='TextByteValues',alias='ue_probe_TextByteValues')
        fn['parameters']=[dict(path='number',kind='byte',type_path='',input=True,output=True),dict(path='title',kind='text',type_path='',input=True,output=True),dict(path='ReturnValue',kind='text',type_path='',input=False,output=True),dict(path='messages',kind='array',type_path='TextProperty',element=dict(kind='text',type_path=''),input=True,output=True)]
        fn['fields']=[]
        for p in fn['parameters']:
            fn['fields'].append({k:p[k] for k in ('path','kind','type_path','input','output')})
            if p['kind']=='array':fn['fields'].append(dict(path=p['path']+'.*',kind='text',type_path='',input=True,output=True))
        self.ir['functions'].append(fn)
        body=generate(self.ir)
        self.assertIn('var arg_number : uint8&',body)
        self.assertIn('var arg_title : UETextValue&',body)
        self.assertIn('ue_arg_byte(call, "number", int(arg_number))',body)
        self.assertIn('__ue_method_arg_text(call, "title", arg_title)',body)
        self.assertIn('uint8(ue_result_byte(call, "number"))',body)
        self.assertIn('__ue_method_result_text(call, "ReturnValue")',body)
        self.assertIn('array<UETextValue>',body)

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
    def test_object_struct_missing_class_rejected(self):
        self.ir['structs'][0]['fields'][0]['kind'] = 'object'
        with self.assertRaisesRegex(ValueError, 'struct object class'): generate(self.ir)
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

    def test_safe_function_carries_explicit_cooked_context_policy(self):
        text=generate(self.ir)
        self.assertIn('ue_require_call_context(call, false)',text)
    def test_unsafe_function_keeps_construction_guard(self):
        fn=self.ir['functions'][0];fn.setdefault('metadata',{})['UnsafeDuringActorConstruction']=''
        body=generate(self.ir).split('def '+fn['alias']+'(',1)[1].split('\n}',1)[0]
        self.assertIn('ue_require_call_context(call, true)',body)
        self.assertLess(body.index('ue_require_call_context'),body.index('ue_invoke_call'))

if __name__ == '__main__': unittest.main()
