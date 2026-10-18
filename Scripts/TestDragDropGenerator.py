import copy
import unittest
from GenerateBindings import generate
from GenerateApiPackage import stage
from TestApiPackage import profile
from TestInputEventsGenerator import event_ir

def drag_ir():
    i=event_ir('PointerEvent');f=i['functions'][0]
    f.update({'class':'/Script/UMG.WidgetBlueprintLibrary','function':'DetectDragIfPressed','call_adapter':'scoped_event_reply_v1'})
    for v in [*f['parameters'],*f['fields']]:v['path']='pointerevent'
    reply=dict(path='returnvalue',kind='struct',type_path='/Script/UMG.EventReply',transport='scoped_event_reply_v1',input=False,output=True)
    f['parameters'].append(reply.copy());f['fields'].append(reply.copy())
    i['classes'][0]['type_path']='/Script/UMG.WidgetBlueprintLibrary'
    i['structs'].append(dict(type_path=reply['type_path'],transport=reply['transport'],fields=[]))
    return i

class DragGeneratorTests(unittest.TestCase):
    def test_mixed_input_reply_exact_adapter(self):
        s=generate(drag_ir(),'ue_test')
        for marker in ('UEPointerEvent','UEEventReply','__ue_arg_pointerevent','__ue_result_event_reply','require ue_widget_events public','require ue_input_events public'):self.assertIn(marker,s)
    def test_pointer_snapshot_adapter_does_not_attest_reply(self):
        i=drag_ir();i['functions'][0]['call_adapter']='slate_input_event_snapshot_v1'
        with self.assertRaisesRegex(ValueError,'synchronous function adapter'):generate(i,'ue_test')
    def test_other_function_cannot_mix_transports(self):
        i=drag_ir();i['functions'][0]['function']='Other'
        with self.assertRaisesRegex(ValueError,'Input event requires'):generate(i,'ue_test')
    def test_other_class_cannot_impersonate_adapter(self):
        i=drag_ir();i['functions'][0]['class']='/Script/Game.WidgetBlueprintLibrary'
        with self.assertRaisesRegex(ValueError,'Input event requires'):generate(i,'ue_test')
    def test_other_event_type_cannot_impersonate_pointer(self):
        i=drag_ir()
        for p in [*i['functions'][0]['parameters'],*i['functions'][0]['fields'],i['structs'][0]]:
            if p.get('type_path')=='/Script/SlateCore.PointerEvent':p['type_path']='/Script/SlateCore.KeyEvent'
        with self.assertRaisesRegex(ValueError,'Input event requires'):generate(i,'ue_test')
    def test_pointer_must_be_input_scalar(self):
        for updates in (dict(input=False,output=True),dict(path='wrong')):
            i=drag_ir()
            for p in [i['functions'][0]['parameters'][0],i['functions'][0]['fields'][0]]:p.update(updates)
            with self.assertRaisesRegex(ValueError,'Input event requires'):generate(i,'ue_test')
    def test_container_not_enabled(self):
        i=drag_ir();f=i['functions'][0];p=f['parameters'][0];f['parameters'][0]=dict(path='events',kind='array',input=True,output=False,element=p)
        with self.assertRaisesRegex(ValueError,'scalar'):generate(i,'ue_test')
    def test_stage_preserves_contract_and_closure(self):
        i=drag_ir();before=copy.deepcopy(i);p=profile();p['libraries'][0]['class']='/Script/UMG.WidgetBlueprintLibrary'
        _,_,_,report=stage(p,i)
        self.assertEqual(i,before);self.assertEqual(report['counts']['generation_ready'],1)
        self.assertIn('ue_widget_events',report['dependencies']['das_modules']);self.assertIn('ue_input_events',report['dependencies']['das_modules'])
if __name__=='__main__':unittest.main()
