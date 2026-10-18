import copy
import unittest
from ReportApiPackage import explain


def inputs():
    members = [dict(klass='Base', declaring_class='Base', function='Ready', status='generation_ready'),
        dict(klass='Base', declaring_class='Base', function='Trace', status='export_blocked', reason='unsupported UFunction or struct field type\n'),
        dict(klass='Derived', declaring_class='Base', function='Trace', status='export_blocked', reason='unsupported UFunction or struct field type\n')]
    for m in members:
        m['class'] = m.pop('klass')
    report = dict(engine_version='test', candidate_count=3, members=members,
        counts=dict(generation_ready=1, export_blocked=2, generation_blocked=0, policy_excluded=0))
    inventory = dict(engine_version='test', members=[dict(domain='method', declaring_class='Base',name='Trace',reasons=['unsupported_parameter_type'],type_issues=[
        dict(path='Hit.Component', property_kind='WeakObjectProperty', cpp_type='TWeakObjectPtr<UObject>'),
        dict(path='Hit.Other', property_kind='WeakObjectProperty', cpp_type='TWeakObjectPtr<UObject>')])])
    return report, inventory


class Reports(unittest.TestCase):
    def test_fields_and_inherited_occurrences_do_not_inflate_type_counts(self):
        r, i = inputs()
        out = explain(r, i)
        self.assertEqual(out['blocker_groups'], {'type_or_abi': 1})
        self.assertEqual(out['type_issue_counts'][0]['unique_declarations'], 1)
        self.assertEqual(len(out['blockers']), 2)

    def test_unsafe_async_handle_is_execution_context_not_missing_value_codec(self):
        r,i=inputs()
        for m in r['members'][1:]:m['reason']='async physics handles require a scoped frozen-game-thread physics callback adapter'
        out=explain(r,i)
        self.assertEqual(out['blocker_groups'],{'execution_context':1})
        self.assertTrue(all(m['blocker_group']=='execution_context' for m in out['blockers']))

    def test_documented_exclusion_is_not_a_technical_blocker(self):
        r,i=inputs()
        for m in r['members'][1:]: m.update(status='policy_excluded',reason='raw_async_handle_forbidden',alternative='scoped DSL',export_reason='async execution required')
        r['counts'].update(export_blocked=0,policy_excluded=2)
        out=explain(r,i)
        self.assertEqual(out['blockers'],[])
        self.assertEqual(len(out['exclusions']),2)
        self.assertEqual(out['blocker_groups'],{})
        self.assertEqual(out['exclusion_groups'],{'documented_policy_exclusion':1})
        self.assertEqual(out['exclusions'][0]['export_reason'],'async execution required')

    def test_overlap_uses_declaration_identity_and_ready_baselines_only(self):
        r, i = inputs()
        baseline = dict(engine_version='test', package='world', source_sha256='hash',state='ue_preflight_passed', members=copy.deepcopy(r['members']))
        baseline['members'][0]['class'] = 'OtherReceiver'
        out = explain(r, i, [baseline, baseline])
        self.assertEqual(out['shared_generated_declarations'], 1)
        self.assertEqual(out['additional_generated_declarations'], 0)

    def test_stale_inventory_version_is_rejected(self):
        r, i = inputs(); i['engine_version'] = 'old'
        with self.assertRaisesRegex(ValueError, 'engine version'): explain(r, i)

    def test_missing_blocker_evidence_is_rejected(self):
        r, i = inputs(); i['members'] = []
        with self.assertRaisesRegex(ValueError, 'missing from inventory'): explain(r, i)

    def test_invalid_partition_is_rejected(self):
        r, i = inputs(); r['counts']['export_blocked'] = 1
        with self.assertRaisesRegex(ValueError, 'partition'): explain(r, i)

    def test_unverified_baseline_is_rejected(self):
        r, i = inputs()
        with self.assertRaisesRegex(ValueError, 'preflight'):
            explain(r, i, [dict(engine_version='test', state='awaiting_ue_preflight')])


if __name__ == '__main__': unittest.main()
