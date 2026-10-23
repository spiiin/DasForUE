import copy
import unittest
from GenerateBindings import generate, CANONICAL_MATH, struct_name
from GenerateApiPackage import stage


def fixture(path='/Script/CoreUObject.Vector', array=False):
    structures=[]
    for identity,(_,_,layout) in CANONICAL_MATH.items():
        fields=[dict(path=name,kind='struct' if kind.startswith('/') else kind,type_path=kind if kind.startswith('/') else '') for name,kind in layout.items()]
        structures.append(dict(type_path=identity,fields=fields))
    def parameter(name,output):
        value=dict(path=name,kind='struct',type_path=path,input=not output,output=output)
        if array:value.update(kind='array',type_path='StructProperty',element=dict(kind='struct',type_path=path))
        return value
    params=[parameter('value',False),parameter('returnvalue',True)]
    def flatten(field,name):
        if field['kind']=='struct':
            for child in next(s for s in structures if s['type_path']==field['type_path'])['fields']:
                yield from flatten(child,name+'.'+child['path'])
        elif field['kind']=='array':
            yield dict(path=name,kind='array',type_path=field['type_path'])
            yield from flatten(field['element'],name+'.*')
        else:yield dict(path=name,kind=field['kind'],type_path=field['type_path'])
    flattened=[]
    for param in params:
        flattened.extend(dict(f,input=param['input'],output=param['output']) for f in flatten(param,param['path']))
    return dict(schema_version=5,functions=[{'class':'/Script/Test.Library','function':'Echo','alias':'ue_test_Echo','signature':'v1;', 'static':True,'parameters':params,'fields':flattened}],diagnostics=[],structs=structures,classes=[
        dict(type_path='/Script/Test.Library',parent='/Script/Engine.BlueprintFunctionLibrary'),
        dict(type_path='/Script/Engine.BlueprintFunctionLibrary',parent='/Script/CoreUObject.Object'),
        dict(type_path='/Script/CoreUObject.Object',parent='')])


class CanonicalMath(unittest.TestCase):
    def test_all_math_identities_reuse_foundation_types(self):
        for path,(name,bridge,_) in CANONICAL_MATH.items():
            with self.subTest(path=path):
                text=generate(fixture(path),canonical_math=True)
                self.assertIn('require ue_foundation public',text)
                self.assertIn('arg_value : '+name,text)
                self.assertIn(') : '+name+' {',text)
                self.assertIn('__ue_method_arg_'+bridge+'(call, "value", arg_value)',text)
                self.assertIn('__ue_method_result_'+bridge+'(call, "returnvalue")',text)
                for identity in CANONICAL_MATH:self.assertNotIn('struct '+struct_name(identity)+' {',text)
    def test_arrays_use_native_element_type_and_concrete_call_paths(self):
        text=generate(fixture('/Script/CoreUObject.Transform',True),canonical_math=True)
        self.assertIn('array<UETransform>',text)
        self.assertIn('__ue_method_arg_transform(call, "value.{index}", arg_value[index])',text)
        self.assertIn('__ue_method_result_transform(call, "returnvalue.{index}")',text)
    def test_nested_values_in_container_owning_struct_are_atomic_math_reads(self):
        source=fixture('/Script/CoreUObject.Transform',True)
        child=copy.deepcopy(source['functions'][0]['parameters'][0]);child['path']='poses'
        source['structs'].append(dict(type_path='/Script/Test.Payload',fields=[child]))
        fn=source['functions'][0]
        fn['parameters']=[dict(path='returnvalue',kind='struct',type_path='/Script/Test.Payload',input=False,output=True)]
        fn['fields']=[dict(f,path=f['path'].replace('returnvalue','returnvalue.poses',1)) for f in fn['fields'] if f['output']]
        text=generate(source,canonical_math=True)
        self.assertIn('__ue_method_result_transform(call, "returnvalue.poses.{output_index_0}")',text)
        self.assertNotIn('.f_rotation',text)
    def test_layout_drift_is_a_generation_error(self):
        source=fixture();source['structs'][0]['fields'][0]['kind']='float'
        with self.assertRaisesRegex(ValueError,'Canonical math layout changed'):generate(source,canonical_math=True)
    def test_same_layout_with_different_identity_is_not_native_math(self):
        source=fixture()
        custom='/Script/Test.Vector'
        vector=next(s for s in source['structs'] if s['type_path']=='/Script/CoreUObject.Vector')
        source['structs'].append(dict(vector,type_path=custom))
        for p in source['functions'][0]['parameters']:p['type_path']=custom
        text=generate(source,canonical_math=True)
        self.assertIn('arg_value : '+struct_name(custom),text)
        self.assertIn('arg_value.f_x',text)
    def test_transport_mode_remains_explicitly_available(self):
        text=generate(fixture())
        self.assertIn('arg_value : '+struct_name('/Script/CoreUObject.Vector'),text)
        self.assertNotIn('__ue_method_arg_vector(call',text)
    def test_package_profile_selects_math_representation(self):
        profile=dict(schema_version=1,module='ue_api_test',ue_modules=['Test'],value_types='canonical_math',default_arguments='explicit',libraries=[{'class':'/Script/Test.Library','prefix':'ue_test_','include':['*']}],probe='probe')
        result=stage(profile,fixture())
        self.assertIn('arg_value : UEVector',result[1])
        self.assertEqual(result[3]['value_types'],'canonical_math')


if __name__=='__main__':unittest.main()
