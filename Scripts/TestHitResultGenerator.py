import copy
import unittest
from GenerateBindings import generate
from GenerateApiPackage import closure

HIT='/Script/Engine.HitResult'
def value(path='hit',**kw):
    return dict(path=path,kind='struct',type_path=HIT,transport='hit_result_v1',**kw)
def ir():
    field=value(input=True,output=True)
    return dict(schema_version=5,classes=[dict(type_path='/Script/Test.Fixture',parent='')],
        structs=[dict(type_path=HIT,transport='hit_result_v1',fields=[])],
        functions=[dict(klass='',function='Echo',alias='fixture_Echo',signature='v1;Echo;',parameters=[copy.deepcopy(field)],fields=[field])])
def source():
    result=ir();f=result['functions'][0];f['class']='/Script/Test.Fixture';del f['klass'];return result

class HitTransport(unittest.TestCase):
    def test_opaque_scalar_and_ref_return_use_native_bridges(self):
        text=generate(source())
        self.assertIn('arg_hit : UEHitResult',text)
        self.assertIn('__ue_arg_hit_result(call, "hit", arg_hit)',text)
        self.assertIn('__ue_result_hit_result(call, "hit")',text)
        self.assertNotIn('ActorInstanceHandle',text)
        self.assertNotIn('struct S_HitResult',text)
    def test_missing_marker_cannot_enable_flattening(self):
        data=source();del data['functions'][0]['parameters'][0]['transport']
        with self.assertRaisesRegex(ValueError,'transport'):generate(data)
    def test_other_struct_cannot_claim_transport(self):
        data=source();data['functions'][0]['fields'][0]['type_path']='/Script/Test.Lookalike'
        with self.assertRaisesRegex(ValueError,'transport'):generate(data)
    def test_private_hit_fields_are_rejected(self):
        data=source();data['structs'][0]['fields']=[dict(path='private',kind='int',type_path='')]
        with self.assertRaisesRegex(ValueError,'opaque native'):generate(data)
    def test_definition_version_is_required(self):
        data=source();data['structs'][0]['transport']='future'
        with self.assertRaisesRegex(ValueError,'opaque native'):generate(data)
    def test_ordinary_struct_cannot_be_an_opaque_leaf(self):
        data=source();field=data['functions'][0]['fields'][0];field.pop('transport');field['type_path']='/Script/Test.Other'
        with self.assertRaisesRegex(ValueError,'opaque call struct'):generate(data)
    def test_array_hit_inputs_outputs_use_element_bridge(self):
        data=source();f=data['functions'][0]
        p=dict(path='hits',kind='array',type_path='StructProperty',element=value(),input=True,output=True)
        f['parameters']=[p]
        f['fields']=[dict(path='hits',kind='array',type_path='StructProperty',input=True,output=True),value(path='hits.*',input=True,output=True)]
        text=generate(data)
        self.assertIn('array<UEHitResult>',text)
        self.assertIn('__ue_arg_hit_result(call, "hits.{index}"',text)
        self.assertIn('__ue_result_hit_result(call, "hits.{index}"',text)
    def test_nested_struct_and_array_hit_preserve_dependency_closure(self):
        data=source();f=data['functions'][0]
        data['structs'].append(dict(type_path='/Script/Test.Envelope',fields=[value()]))
        f['parameters']=[dict(path='envelope',kind='struct',type_path='/Script/Test.Envelope',input=True,output=True)]
        f['fields']=[value(path='envelope.hit',input=True,output=True)]
        text=generate(closure(data,data['functions']))
        self.assertIn('f_hit : UEHitResult',text)
        self.assertIn('__ue_arg_hit_result(call, "envelope.hit"',text)

if __name__=='__main__': unittest.main()
