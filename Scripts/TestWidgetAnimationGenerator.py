import copy
import unittest
from TestInputEventsGenerator import event_ir
from GenerateBindings import generate
from GenerateApiPackage import stage
from TestApiPackage import profile
PATH='/Script/UMG.WidgetAnimationHandle';TRANSPORT='weak_widget_animation_v1'
def animation_ir():
 i=event_ir('KeyEvent');f=i['functions'][0];f['call_adapter']=TRANSPORT
 for v in [*f['fields'],*f['parameters']]:v.update(type_path=PATH,transport=TRANSPORT,path='returnvalue',input=False,output=True)
 i['structs']=[dict(type_path=PATH,transport=TRANSPORT,fields=[])]
 return i
class WidgetAnimationGeneratorTests(unittest.TestCase):
 def test_native_opaque_result_and_public_module(self):
  s=generate(animation_ir(),'ue_test');self.assertIn('UEWidgetAnimationHandle',s);self.assertIn('require ue_widget_animation public',s);self.assertIn('__ue_result_widget_animation',s);self.assertNotIn('struct S_WidgetAnimationHandle',s)
 def test_no_native_input(self):
  i=animation_ir()
  for v in [*i['functions'][0]['fields'],*i['functions'][0]['parameters']]:v.update(input=True,output=False)
  with self.assertRaisesRegex(ValueError,'producer result'):generate(i,'ue_test')
 def test_no_inout(self):
  i=animation_ir();i['functions'][0]['parameters'][0]['input']=True
  with self.assertRaisesRegex(ValueError,'producer result'):generate(i,'ue_test')
 def test_no_array(self):
  i=animation_ir();f=i['functions'][0]['parameters'][0];i['functions'][0]['parameters']=[dict(path='returnvalue',kind='array',input=False,output=True,element=f)]
  with self.assertRaisesRegex(ValueError,'producer result'):generate(i,'ue_test')
 def test_no_function_attestation(self):
  i=animation_ir();i['functions'][0].pop('call_adapter')
  with self.assertRaisesRegex(ValueError,'function adapter'):generate(i,'ue_test')
 def test_no_field_attestation(self):
  i=animation_ir();i['functions'][0]['fields'][0].pop('transport')
  with self.assertRaisesRegex(ValueError,'transport'):generate(i,'ue_test')
 def test_no_native_layout(self):
  i=animation_ir();i['structs'][0]['fields']=[dict(path='x',kind='int',type_path='')]
  with self.assertRaisesRegex(ValueError,'opaque native transport'):generate(i,'ue_test')
 def test_no_foreign_identity(self):
  i=animation_ir()
  for v in [*i['functions'][0]['fields'],*i['functions'][0]['parameters'],*i['structs']]:v['type_path']='/Script/Game.WidgetAnimationHandle'
  with self.assertRaisesRegex(ValueError,'transport'):generate(i,'ue_test')
 def test_dependency_and_immutable_input_ir(self):
  i=animation_ir();old=copy.deepcopy(i);_,_,_,r=stage(profile(),i)
  self.assertEqual(r['counts']['generation_ready'],1);self.assertIn('ue_widget_animation',r['dependencies']['das_modules']);self.assertEqual(i,old)
if __name__=='__main__':unittest.main()
