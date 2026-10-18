import copy
import unittest
from TestInputEventsGenerator import event_ir
from GenerateBindings import generate
from GenerateApiPackage import stage
from TestApiPackage import profile
PATH='/Script/UMG.PaintContext';TRANSPORT='scoped_paint_context_v1'
def paint_ir():
 i=event_ir('KeyEvent');f=i['functions'][0];f['call_adapter']=TRANSPORT
 for v in [*f['fields'],*f['parameters']]:v.update(type_path=PATH,transport=TRANSPORT,path='context',input=True,output=True)
 i['structs']=[dict(type_path=PATH,transport=TRANSPORT,fields=[])]
 return i
class PaintGeneratorTests(unittest.TestCase):
 def test_opaque_type_and_public_module(self):
  s=generate(paint_ir(),'ue_test');self.assertIn('UEPaintContext',s);self.assertIn('require ue_paint public',s);self.assertIn('__ue_arg_paint_context',s);self.assertIn('__ue_result_paint_context',s);self.assertNotIn('struct S_PaintContext',s)
 def test_not_a_native_result(self):
  i=paint_ir()
  for v in [*i['functions'][0]['fields'],*i['functions'][0]['parameters']]:v.update(path='returnvalue',input=False)
  with self.assertRaisesRegex(ValueError,'scalar context parameter'):generate(i,'ue_test')
 def test_not_a_native_array(self):
  i=paint_ir();f=i['functions'][0]['parameters'][0];i['functions'][0]['parameters']=[dict(path='contexts',kind='array',input=True,output=False,element=f)]
  with self.assertRaisesRegex(ValueError,'scalar context parameter'):generate(i,'ue_test')
 def test_requires_function_attestation(self):
  i=paint_ir();i['functions'][0].pop('call_adapter')
  with self.assertRaisesRegex(ValueError,'synchronous function adapter'):generate(i,'ue_test')
 def test_requires_field_marker(self):
  i=paint_ir();i['functions'][0]['fields'][0].pop('transport')
  with self.assertRaisesRegex(ValueError,'transport'):generate(i,'ue_test')
 def test_no_borrowed_native_fields(self):
  i=paint_ir();i['structs'][0]['fields']=[dict(path='layer',kind='int',type_path='')]
  with self.assertRaisesRegex(ValueError,'opaque native transport'):generate(i,'ue_test')
 def test_foreign_same_name_rejected(self):
  i=paint_ir()
  for v in [*i['functions'][0]['fields'],*i['functions'][0]['parameters'],*i['structs']]:v['type_path']='/Script/Game.PaintContext'
  with self.assertRaisesRegex(ValueError,'transport'):generate(i,'ue_test')
 def test_staging_attestation_dependency_and_source(self):
  i=paint_ir();old=copy.deepcopy(i);v,_,_,r=stage(profile(),i)
  self.assertEqual(v['functions'][0]['call_adapter'],TRANSPORT);self.assertEqual(r['counts']['generation_ready'],1);self.assertIn('ue_paint',r['dependencies']['das_modules']);self.assertEqual(i,old)
if __name__=='__main__':unittest.main()
