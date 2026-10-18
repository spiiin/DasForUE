import copy
import unittest
from TestInputEventsGenerator import event_ir
from GenerateBindings import generate
from GenerateApiPackage import stage
from TestApiPackage import profile
PATH='/Script/UMG.EventReply'
TRANSPORT='scoped_event_reply_v1'
def reply_ir():
 i=event_ir('KeyEvent');f=i['functions'][0];f['call_adapter']=TRANSPORT
 for v in [*f['fields'],*f['parameters'],*i['structs']]:v['type_path']=PATH;v['transport']=TRANSPORT
 return i
class ReplyGeneratorTests(unittest.TestCase):
 def test_type_and_public_module(self):
  s=generate(reply_ir(),'ue_test');self.assertIn('UEEventReply',s);self.assertIn('require ue_widget_events public',s);self.assertIn('__ue_arg_event_reply',s);self.assertNotIn('struct S_EventReply',s)
 def test_inout_and_return_extracted_through_scoped_adapter(self):
  i=reply_ir();fn=i['functions'][0]
  for v in [*fn['fields'],*fn['parameters']]:v.update(output=True)
  for field in ('fields','parameters'):fn[field].append(dict(path='returnvalue',kind='struct',type_path=PATH,transport=TRANSPORT,input=False,output=True))
  s=generate(i,'ue_test');self.assertIn('out_event',s);self.assertIn('out_returnvalue',s);self.assertEqual(s.count('__ue_result_event_reply(call,'),2)
 def test_no_attestation_rejected(self):
  i=reply_ir();i['functions'][0].pop('call_adapter')
  with self.assertRaisesRegex(ValueError,'synchronous function adapter'):generate(i,'ue_test')
 def test_no_parameter_marker_rejected(self):
  i=reply_ir();i['functions'][0]['parameters'][0].pop('transport')
  with self.assertRaisesRegex(ValueError,'transport'):generate(i,'ue_test')
 def test_native_layout_not_exposed(self):
  i=reply_ir();i['structs'][0]['fields']=[dict(path='x',kind='int',type_path='')]
  with self.assertRaisesRegex(ValueError,'opaque native transport'):generate(i,'ue_test')
 def test_native_array_not_supported(self):
  i=reply_ir();f=i['functions'][0]['parameters'][0];i['functions'][0]['parameters']=[dict(path='items',kind='array',input=True,output=False,element=f)]
  with self.assertRaisesRegex(ValueError,'scalar'):generate(i,'ue_test')
 def test_foreign_same_name_rejected(self):
  i=reply_ir()
  for v in [*i['functions'][0]['fields'],*i['functions'][0]['parameters'],*i['structs']]:v['type_path']='/Script/Game.EventReply'
  with self.assertRaisesRegex(ValueError,'transport'):generate(i,'ue_test')
 def test_staging_preserves_attestation_dependency_and_source(self):
  i=reply_ir();before=copy.deepcopy(i);v,_,_,r=stage(profile(),i)
  self.assertEqual(v['functions'][0]['call_adapter'],TRANSPORT);self.assertEqual(r['counts']['generation_ready'],1);self.assertIn('ue_widget_events',r['dependencies']['das_modules']);self.assertEqual(i,before)
if __name__=='__main__':unittest.main()
