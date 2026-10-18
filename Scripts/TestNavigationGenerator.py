import copy
import unittest
from TestInputEventsGenerator import event_ir
from GenerateBindings import generate
from GenerateApiPackage import stage
from TestApiPackage import profile

NAMES=('Key_GetNavigationActionFromKey','Key_GetNavigationDirectionFromKey','Key_GetNavigationDirectionFromAnalog')
def nav_ir(name=NAMES[0]):
    analog=name.endswith('Analog');i=event_ir('AnalogInputEvent' if analog else 'KeyEvent');f=i['functions'][0]
    f.update({'class':'/Script/Engine.KismetInputLibrary','function':name,'call_adapter':'live_input_navigation_v1'})
    i['classes'][0]['type_path']=f['class']
    for p in [*f['parameters'],*f['fields']]:p['path']='inanalogevent' if analog else 'inkeyevent'
    return i

class NavigationTests(unittest.TestCase):
    def test_three_exact_scoped_adapters(self):
        for name in NAMES:self.assertIn('__ue_arg_'+('analoginputevent' if name.endswith('Analog') else 'keyevent'),generate(nav_ir(name),'ue_test'))
    def test_wrong_class_rejected(self):
        i=nav_ir();i['functions'][0]['class']='/Script/Game.KismetInputLibrary'
        with self.assertRaisesRegex(ValueError,'Input event requires'):generate(i,'ue_test')
    def test_wrong_function_rejected(self):
        i=nav_ir();i['functions'][0]['function']='Other'
        with self.assertRaisesRegex(ValueError,'Input event requires'):generate(i,'ue_test')
    def test_wrong_event_type_rejected(self):
        i=nav_ir(NAMES[2]);i['functions'][0]['function']=NAMES[0]
        with self.assertRaisesRegex(ValueError,'Input event requires'):generate(i,'ue_test')
    def test_wrong_path_or_direction_rejected(self):
        for change in (dict(path='other'),dict(input=False,output=True)):
            i=nav_ir()
            for p in [*i['functions'][0]['fields'],*i['functions'][0]['parameters']]:p.update(change)
            with self.assertRaisesRegex(ValueError,'Input event requires'):generate(i,'ue_test')
    def test_missing_adapter_rejected(self):
        i=nav_ir();i['functions'][0].pop('call_adapter')
        with self.assertRaisesRegex(ValueError,'Input event requires'):generate(i,'ue_test')
    def test_nested_native_container_not_enabled(self):
        i=nav_ir();f=i['functions'][0];f['parameters'][0]=dict(path='items',kind='array',input=True,output=False,element=f['parameters'][0])
        with self.assertRaisesRegex(ValueError,'scalar'):generate(i,'ue_test')
    def test_stage_preserves_attestation_and_source(self):
        i=nav_ir();before=copy.deepcopy(i);p=profile();p['libraries'][0]['class']=i['functions'][0]['class'];v,_,_,report=stage(p,i)
        self.assertEqual(i,before);self.assertEqual(report['counts']['generation_ready'],1);self.assertEqual(v['functions'][0]['call_adapter'],'live_input_navigation_v1')
if __name__=='__main__':unittest.main()
