import copy
import tempfile
import unittest
from pathlib import Path
from AuditUMGInput import audit, source_evidence, EVIDENCE, markdown, validate_contract, INPUT_TYPES
from GenerateApiPackage import stage
from TestApiPackage import profile, ir

def fixture():
    p=profile();i=ir();i['structs']=[]
    fn=copy.deepcopy(i['functions'][0]);fn['function']='Inspect'
    f=dict(path='event',kind='struct',type_path='/Script/SlateCore.PointerEvent',input=True,output=False)
    fn['parameters']=[f];fn['fields']=[];i['functions'].append(fn)
    i['structs']=[dict(type_path=f['type_path'],fields=[])]
    r=stage(p,i)[3];r['state']='ue_preflight_passed'
    return p,i,r,copy.deepcopy(r)

class AuditTests(unittest.TestCase):
    def setUp(self):
        # These fixtures exercise the historical contract-only audit. The new
        # implemented codec has independent TestInputEventsGenerator coverage.
        from GenerateBindings import OPAQUE_TRANSPORTS
        self.transports=dict(OPAQUE_TRANSPORTS)
        for path in INPUT_TYPES: OPAQUE_TRANSPORTS.pop(path,None)
    def tearDown(self):
        from GenerateBindings import OPAQUE_TRANSPORTS
        OPAQUE_TRANSPORTS.clear();OPAQUE_TRANSPORTS.update(self.transports)

    def contract(self):
        a=audit(*fixture())
        c=dict(schema_version=1,status='design_only_not_implemented',native_types=sorted(INPUT_TYPES),
            snapshot_function_allowlist=[m['class']+'.'+m['function'] for m in a['members']],deferred_navigation_functions=[],
            ownership=dict(event_path='always_null_in_detached_native_copy',memcpy_native_layout=False),
            navigation=dict(ordinary_snapshot_dispatch=False),ui_callbacks=dict(queue_reply_callbacks=False),implementation_changes_coverage=False)
        return c,a
    def test_valid_design_contract(self):
        validate_contract(*self.contract())
    def test_contract_cannot_claim_implementation(self):
        c,a=self.contract();c['status']='implemented'
        with self.assertRaisesRegex(ValueError,'planned'):validate_contract(c,a)
    def test_contract_cannot_expand_allowlist_without_audit(self):
        c,a=self.contract();c['snapshot_function_allowlist'].append('/Script/Engine.KismetInputLibrary.Key_GetNavigationDirectionFromAnalog')
        with self.assertRaisesRegex(ValueError,'allowlist'):validate_contract(c,a)
    def test_contract_native_identity_is_exact(self):
        c,a=self.contract();c['native_types'][0]='/Script/Game.InputEvent'
        with self.assertRaisesRegex(ValueError,'identities'):validate_contract(c,a)
    def test_contract_forbids_borrowed_path_or_memcpy(self):
        for field,value in [('event_path','borrowed'),('memcpy_native_layout',True)]:
            c,a=self.contract();c['ownership'][field]=value
            with self.assertRaisesRegex(ValueError,'ownership'):validate_contract(c,a)
    def test_contract_cannot_queue_replies_or_dispatch_stored_navigation(self):
        for section,field in [('navigation','ordinary_snapshot_dispatch'),('ui_callbacks','queue_reply_callbacks')]:
            c,a=self.contract();c[section][field]=True
            with self.assertRaisesRegex(ValueError,'navigation or reply'):validate_contract(c,a)
    def test_partition_and_paths_preserve_no_coverage_claim(self):
        a=audit(*fixture())
        self.assertEqual(a['partition'],{'input_events':1})
        self.assertEqual(a['input_scope']['detached_snapshot_candidates'],1)
        self.assertEqual(a['members'][0]['fields'][0]['path'],'event')
        self.assertFalse(a['runtime_transport_implemented'])
        self.assertFalse(a['coverage_changed'])
    def test_stale_summary_hash_rejected(self):
        p,i,r,s=fixture();s['source_sha256']='stale'
        with self.assertRaisesRegex(ValueError,'mismatched'):audit(p,i,r,s)
    def test_stale_member_reason_rejected(self):
        p,i,r,s=fixture();r['members'][-1]['reason']='obsolete'
        with self.assertRaisesRegex(ValueError,'status/reason'):audit(p,i,r,s)
    def test_missing_summary_member_rejected(self):
        p,i,r,s=fixture();s['members']=[m for m in s['members'] if m['status']!='generation_blocked']
        with self.assertRaisesRegex(ValueError,'missing or extra'):audit(p,i,r,s)
    def test_unverified_report_rejected(self):
        p,i,r,s=fixture();r['state']='awaiting_ue_preflight'
        with self.assertRaisesRegex(ValueError,'accepted preflight'):audit(p,i,r,s)
    def test_duplicate_report_rejected(self):
        p,i,r,s=fixture();r['members'].append(r['members'][0])
        with self.assertRaisesRegex(ValueError,'Duplicate report'):audit(p,i,r,s)
    def test_unknown_opaque_type_cannot_hide_behind_known_type(self):
        p,i,r,s=fixture();fn=i['functions'][-1]
        f=dict(path='other',kind='struct',type_path='/Script/Test.Unknown',input=True,output=False)
        fn['parameters'].append(f);i['structs'].append(dict(type_path=f['type_path'],fields=[]))
        r=stage(p,i)[3];r['state']='ue_preflight_passed';s=copy.deepcopy(r)
        with self.assertRaisesRegex(ValueError,'Unclassified opaque'):audit(p,i,r,s)
    def test_cross_group_function_counted_once_and_type_occurrences_overlap(self):
        p,i,r,s=fixture();fn=i['functions'][-1]
        f=dict(path='reply',kind='struct',type_path='/Script/UMG.EventReply',input=True,output=True)
        fn['parameters'].append(f);i['structs'].append(dict(type_path=f['type_path'],fields=[]))
        r=stage(p,i)[3];r['state']='ue_preflight_passed';s=copy.deepcopy(r)
        a=audit(p,i,r,s)
        self.assertEqual(a['partition'],{'event_reply':1})
        self.assertEqual(len(a['opaque_types']),2)
        self.assertEqual(len(a['input_scope']['cross_group_functions']),1)
        self.assertIn('inout',markdown(a))
    def test_navigation_is_exact_native_identity_not_short_name(self):
        p,i,r,s=fixture();fn=i['functions'][-1];fn['function']='Key_GetNavigationDirectionFromAnalog'
        r=stage(p,i)[3];r['state']='ue_preflight_passed';s=copy.deepcopy(r)
        self.assertEqual(audit(p,i,r,s)['input_scope']['navigation_context_candidates'],0)
    def test_native_source_anchors_and_hashes_are_fail_closed(self):
        with tempfile.TemporaryDirectory() as d:
            for file,markers in EVIDENCE.values():
                p=Path(d)/file;p.parent.mkdir(parents=True,exist_ok=True);p.write_text('\n'.join(markers),encoding='utf-8')
            a=source_evidence(d)
            self.assertEqual(len(a),len(EVIDENCE))
            file=next(iter(EVIDENCE.values()))[0];(Path(d)/file).write_text('',encoding='utf-8')
            with self.assertRaisesRegex(ValueError,'Missing native'):source_evidence(d)

if __name__=='__main__':unittest.main()
