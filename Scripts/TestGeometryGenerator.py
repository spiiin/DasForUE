import copy
import unittest
from TestInputEventsGenerator import event_ir
from GenerateBindings import generate
from GenerateApiPackage import stage
from TestApiPackage import profile
PATH='/Script/SlateCore.Geometry'
TRANSPORT='slate_geometry_snapshot_v1'
def geometry_ir():
 i=event_ir('KeyEvent');f=i['functions'][0];f['call_adapter']=TRANSPORT
 for v in [*f['fields'],*f['parameters'],*i['structs']]:
  v['type_path']=PATH;v['transport']=TRANSPORT
 return i
class GeometryGeneratorTests(unittest.TestCase):
 def test_scalar_native_identity_and_public_dependency(self):
  s=generate(geometry_ir(),'ue_test');self.assertIn('UEGeometry',s);self.assertIn('__ue_arg_geometry',s);self.assertIn('require ue_geometry public',s);self.assertNotIn('require ue_input_events',s);self.assertNotIn('struct S_Geometry',s)
 def test_owned_result_adapter(self):
  i=geometry_ir()
  for v in [*i['functions'][0]['fields'],*i['functions'][0]['parameters']]:v.update(path='returnvalue',input=False,output=True)
  self.assertIn('__ue_result_geometry',generate(i,'ue_test'))
 def test_no_function_attestation(self):
  i=geometry_ir();i['functions'][0].pop('call_adapter')
  with self.assertRaisesRegex(ValueError,'function adapter'):generate(i,'ue_test')
 def test_no_field_attestation(self):
  i=geometry_ir();i['functions'][0]['parameters'][0].pop('transport')
  with self.assertRaisesRegex(ValueError,'transport'):generate(i,'ue_test')
 def test_no_native_layout_fields(self):
  i=geometry_ir();i['structs'][0]['fields']=[dict(path='x',kind='int',type_path='')]
  with self.assertRaisesRegex(ValueError,'opaque native transport'):generate(i,'ue_test')
 def test_not_a_native_array_contract(self):
  i=geometry_ir();f=i['functions'][0]['parameters'][0];i['functions'][0]['parameters']=[dict(path='a',kind='array',input=True,output=False,element=f)]
  with self.assertRaisesRegex(ValueError,'scalar'):generate(i,'ue_test')
 def test_same_name_foreign_identity(self):
  i=geometry_ir()
  for v in [*i['functions'][0]['fields'],*i['functions'][0]['parameters'],*i['structs']]:v['type_path']='/Script/Game.Geometry'
  with self.assertRaisesRegex(ValueError,'transport'):generate(i,'ue_test')
 def test_staging_dependency_and_no_source_mutation(self):
  i=geometry_ir();before=copy.deepcopy(i);_,_,_,r=stage(profile(),i)
  self.assertEqual(r['counts']['generation_ready'],1);self.assertIn('ue_geometry',r['dependencies']['das_modules']);self.assertEqual(i,before)
if __name__=='__main__':unittest.main()
