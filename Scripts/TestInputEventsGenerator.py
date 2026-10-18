import copy
import unittest
from GenerateBindings import generate, OPAQUE_TRANSPORTS
from GenerateApiPackage import stage
from TestApiPackage import ir, profile

TYPES=('InputEvent','KeyEvent','AnalogInputEvent','CharacterEvent','PointerEvent','NavigationEvent')
def event_ir(name):
    i=ir();p='/Script/SlateCore.'+name
    f=dict(path='event',kind='struct',type_path=p,transport='slate_input_event_snapshot_v1',input=True,output=False)
    fn=i['functions'][0];fn['alias']='ue_test_event';fn['fields']=[f.copy()];fn['parameters']=[f.copy()];fn['call_adapter']='slate_input_event_snapshot_v1'
    i['structs']=[dict(type_path=p,transport='slate_input_event_snapshot_v1',fields=[])]
    return i
class InputEventGeneratorTests(unittest.TestCase):
    def test_six_exact_types_use_common_transport_and_canonical_module(self):
        for n in TYPES:
            s=generate(event_ir(n),'ue_test')
            self.assertIn('require ue_input_events public',s)
            self.assertIn('UE'+n,s)
            self.assertIn('__ue_arg_'+n.lower(),s)
            self.assertNotIn('struct S_'+n,s)
    def test_outputs_use_owned_snapshot_result_adapter(self):
        for n in TYPES:
            i=event_ir(n)
            for f in [*i['functions'][0]['fields'],*i['functions'][0]['parameters']]:f.update(path='ReturnValue',input=False,output=True)
            self.assertIn('__ue_result_'+n.lower(),generate(i,'ue_test'))
    def test_missing_field_attestation_fails(self):
        i=event_ir('KeyEvent');i['functions'][0]['parameters'][0].pop('transport')
        with self.assertRaisesRegex(ValueError,'transport'):generate(i,'ue_test')
    def test_no_function_attestation_fails(self):
        i=event_ir('KeyEvent');i['functions'][0].pop('call_adapter')
        with self.assertRaisesRegex(ValueError,'function adapter'):generate(i,'ue_test')
    def test_opaque_definition_cannot_expose_fields(self):
        i=event_ir('KeyEvent');i['structs'][0]['fields']=[dict(path='x',kind='int',type_path='')]
        with self.assertRaisesRegex(ValueError,'opaque native transport'):generate(i,'ue_test')
    def test_unknown_transport_cannot_impersonate_keyevent(self):
        i=event_ir('KeyEvent');i['structs'][0]['transport']='fake'
        with self.assertRaisesRegex(ValueError,'opaque native transport'):generate(i,'ue_test')
    def test_array_event_is_not_a_scalar_contract(self):
        i=event_ir('KeyEvent');f=i['functions'][0]['parameters'][0];i['functions'][0]['parameters']=[dict(path='events',kind='array',input=True,output=False,element=f)]
        with self.assertRaisesRegex(ValueError,'scalar'):generate(i,'ue_test')
    def test_fake_same_name_struct_not_accepted(self):
        i=event_ir('KeyEvent');p='/Script/Game.KeyEvent'
        for f in [*i['functions'][0]['fields'],*i['functions'][0]['parameters']]:f['type_path']=p
        i['structs'][0]['type_path']=p
        with self.assertRaisesRegex(ValueError,'transport'):generate(i,'ue_test')
    def test_staging_keeps_function_contract_and_dependency(self):
        i=event_ir('PointerEvent');before=copy.deepcopy(i)
        value,_,_,report=stage(profile(),i)
        self.assertEqual(value['functions'][0]['call_adapter'],'slate_input_event_snapshot_v1')
        self.assertEqual(report['counts']['generation_ready'],1)
        self.assertIn('ue_input_events',report['dependencies']['das_modules'])
        self.assertEqual(i,before)
if __name__=='__main__':unittest.main()
