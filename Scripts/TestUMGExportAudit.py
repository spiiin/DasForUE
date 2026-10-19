"""Mutation checks for coverage accounting and evidence gates; no Unreal launch."""
import copy
from pathlib import Path
import tempfile
import unittest
from AuditUMGExports import read, partition, compare_members, source_anchor, check_widget_routes

ROOT=Path(__file__).resolve().parents[1]

class AuditTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.report=read(ROOT/'Bindings/Packages/ue_api_umg_input.summary.json')
        cls.rules=read(ROOT/'Bindings/UMGExportAudit.rules.json')

    def setUp(self):
        self.r=copy.deepcopy(self.report);self.d=copy.deepcopy(self.rules)

    def refused(self, operation):
        with self.assertRaises(ValueError):operation()

    def test_complete_partition_preserves_raw_refusals(self):
        rows=partition(self.r,self.d)
        self.assertEqual(len(rows),108)
        self.assertTrue(all(x['raw_status']=='export_blocked' for x in rows))
        self.assertEqual({x['category'] for x in rows},{'implemented_adapter','policy_exclusion','technical_blocker'})

    def test_missing_decision(self):
        self.d['members'].pop();self.refused(lambda:partition(self.r,self.d))

    def test_extra_decision(self):
        row=copy.deepcopy(self.d['members'][0]);row['function']='UnknownNewAPI';self.d['members'].append(row);self.refused(lambda:partition(self.r,self.d))

    def test_duplicate_decision(self):
        self.d['members'].append(copy.deepcopy(self.d['members'][0]));self.refused(lambda:partition(self.r,self.d))

    def test_duplicate_report_member(self):
        self.r['members'].append(copy.deepcopy(self.r['members'][0]));self.refused(lambda:partition(self.r,self.d))

    def test_changed_reason(self):
        self.d['members'][0]['expected_reason']='changed_policy';self.refused(lambda:partition(self.r,self.d))

    def test_new_refusal_requires_review(self):
        m=next(m for m in self.r['members'] if m['status']=='generation_ready');m['status']='export_blocked';m['reason']='new';self.r['counts']['export_blocked']+=1;self.refused(lambda:partition(self.r,self.d))

    def test_unknown_category(self):
        self.d['members'][0]['category']='ready';self.refused(lambda:partition(self.r,self.d))

    def test_signature_is_not_callable_adapter(self):
        m=next(x for x in self.d['members'] if x['function'].endswith('__DelegateSignature'));m['category']='implemented_adapter';m['adapter']='input_reply';self.refused(lambda:partition(self.r,self.d))

    def test_deprecated_stays_policy(self):
        m=next(x for x in self.d['members'] if x['expected_reason']=='deprecated_function');m['category']='technical_blocker';self.refused(lambda:partition(self.r,self.d))

    def test_missing_adapter_contract(self):
        m=next(x for x in self.d['members'] if x['category']=='implemented_adapter');m['adapter']='unimplemented';self.refused(lambda:partition(self.r,self.d))

    def test_missing_adapter_evidence(self):
        self.d['adapters']['input_reply']['evidence']=[];self.refused(lambda:partition(self.r,self.d))

    def test_missing_next_action(self):
        self.d['members'][0]['next_action']='';self.refused(lambda:partition(self.r,self.d))

    def test_fresh_status_drift(self):
        changed=copy.deepcopy(self.r);changed['members'][0]['status']='generation_blocked';self.refused(lambda:compare_members(self.r,changed))

    def test_fresh_signature_drift(self):
        changed=copy.deepcopy(self.r);changed['members'][0]['signature']='changed ABI';self.refused(lambda:compare_members(self.r,changed))

    def test_source_anchor_hash_and_missing_marker(self):
        with tempfile.TemporaryDirectory() as directory:
            Path(directory,'source.cpp').write_text('line1\nNativeRoute();\n',encoding='utf-8')
            spec=dict(path='source.cpp',marker='NativeRoute(')
            evidence=source_anchor(directory,spec);self.assertEqual(evidence['line'],2);self.assertEqual(len(evidence['sha256']),64)
            Path(directory,'source.cpp').write_text('removed',encoding='utf-8');self.refused(lambda:source_anchor(directory,spec))

    def test_evidence_cannot_escape_root(self):
        self.refused(lambda:source_anchor(ROOT,dict(path='../outside.cpp',marker='anything')))

    def test_missing_native_input_route(self):
        rows=partition(self.r,self.d);self.refused(lambda:check_widget_routes(rows,''))

    def test_new_route_invalidates_missing_callback_claim(self):
        rows=partition(self.r,self.d)
        source=(ROOT/'Plugins/DasForUE/Source/DasRuntime/Private/DasScriptUserWidget.cpp').read_text(encoding='utf-8-sig')
        check_widget_routes(rows,source)
        self.refused(lambda:check_widget_routes(rows,source+'\nDAS_WIDGET_INPUT(OnMotionDetected,FMotionEvent)'))

    def test_unrelated_existing_adapter_cannot_claim_technical_blocker(self):
        row=next(x for x in self.d['members'] if x['category']=='technical_blocker' and x['class_path']!='/Script/UMG.WidgetBlueprintLibrary');row['category']='implemented_adapter';row['adapter']='widget_factory'
        rows=partition(self.r,self.d)
        source=(ROOT/'Plugins/DasForUE/Source/DasRuntime/Private/DasScriptUserWidget.cpp').read_text(encoding='utf-8-sig')
        self.refused(lambda:check_widget_routes(rows,source))

if __name__=='__main__':unittest.main()
