import copy
import unittest
from GenerateApiPackage import closure, selection, stage


def profile():
    return dict(schema_version=1,module='ue_api_test',ue_modules=['Engine'],value_types='generated_transport',
                default_arguments='explicit',libraries=[{'class':'/Script/Engine.TestLibrary','prefix':'ue_test_','include':['*']}],exclusions=[],probe='[export]\ndef main(input:int):int { return 42 }')


def ir():
    field=dict(path='value',kind='int',type_path='',input=True,output=False)
    fn=dict(klass='',function='Add',alias='audit_x',signature='v1;',static=True,world_context='',fields=[field.copy()],parameters=[dict(field,default_text='not a literal')])
    fn.pop('klass');fn['class']='/Script/Engine.TestLibrary'
    return dict(schema_version=5,engine_version='test',functions=[fn],diagnostics=[],structs=[dict(type_path='/Script/Test.Broken',fields=[])],classes=[
        dict(type_path='/Script/Engine.TestLibrary',parent='/Script/Engine.BlueprintFunctionLibrary'),
        dict(type_path='/Script/Engine.BlueprintFunctionLibrary',parent='/Script/CoreUObject.Object'),
        dict(type_path='/Script/CoreUObject.Object',parent=''),dict(type_path='/Script/Test.Unrelated',parent='')])


class Packages(unittest.TestCase):
    def test_closure_removes_incomplete_unrelated_types(self):
        source=ir();value=closure(source,source['functions'])
        self.assertEqual(value['structs'],[])
        self.assertEqual(len(value['classes']),3)
    def test_explicit_default_policy_is_visible_and_does_not_mutate_input(self):
        source=ir();before=copy.deepcopy(source)
        data,public,staged,report=stage(profile(),source)
        self.assertEqual(source,before)
        self.assertNotIn('default_text',data['functions'][0]['parameters'][0])
        self.assertEqual(report['members'][0]['explicit_defaults'],[dict(parameter='value',value='not a literal')])
        self.assertIn('def ue_test_Add',public)
        self.assertEqual(report['state'],'awaiting_ue_preflight')
        self.assertNotEqual(public,staged)
    def test_blocks_are_partitioned_and_explained(self):
        source=ir();bad=copy.deepcopy(source['functions'][0]);bad['function']='Bad';bad['parameters'][0]['kind']='struct';bad['parameters'][0]['type_path']='/Script/Test.Broken'
        source['functions'].append(bad)
        source['diagnostics']=[{'class':'/Script/Engine.TestLibrary','function':'Latent','reason':'latent requires adapter'}]
        _,_,_,report=stage(profile(),source)
        self.assertEqual(report['candidate_count'],sum(report['counts'].values()))
        self.assertEqual(report['counts']['generation_blocked'],1)
        self.assertEqual(report['counts']['export_blocked'],1)
        self.assertTrue(all('reason' in r for r in report['members'] if r['status']!='generation_ready'))
    def test_policy_exclusion_is_not_lost(self):
        p=profile();p['exclusions']=[dict(klass='',function='Latent',reason='managed only')];p['exclusions'][0].pop('klass');p['exclusions'][0]['class']='/Script/Engine.TestLibrary'
        source=ir();source['diagnostics']=[{'class':'/Script/Engine.TestLibrary','function':'Latent','reason':'latent'}]
        report=stage(p,source)[3]
        self.assertEqual(report['counts']['policy_excluded'],1)
    def test_exclusion_keeps_original_export_diagnostic_and_alternative(self):
        p=profile();p['exclusions']=[dict(function='Latent',reason='unsafe raw capability',alternative='scoped DSL',**{'class':'/Script/Engine.TestLibrary'})]
        source=ir();source['diagnostics']=[dict(function='Latent',reason='requires physics context',**{'class':'/Script/Engine.TestLibrary'})]
        report=stage(p,source)[3]
        m=next(m for m in report['members'] if m['function']=='Latent')
        self.assertEqual(m['export_reason'],'requires physics context')
        self.assertEqual(m['alternative'],'scoped DSL')
        self.assertEqual(report['dependencies']['das_modules'],['ue_foundation'])

    def test_duplicate_identity_fails_package(self):
        source=ir();source['functions']*=2
        with self.assertRaisesRegex(ValueError,'Duplicate function'):stage(profile(),source)
    def test_unsupported_policy_is_rejected(self):
        p=profile();p['default_arguments']='silently_drop'
        with self.assertRaisesRegex(ValueError,'default_arguments'):selection(p)
    def test_undocumented_exclusions_rejected(self):
        p=profile();p['libraries'][0]['exclude']=['Danger*']
        with self.assertRaisesRegex(ValueError,'documented exclusions'):selection(p)
    def test_generation_is_deterministic(self):
        self.assertEqual(stage(profile(),ir()),stage(profile(),ir()))
    def test_missing_library_cannot_silently_reduce_package(self):
        p=profile();p['libraries'].append({'class':'/Script/Engine.MissingLibrary','prefix':'ue_missing_','include':['*']})
        with self.assertRaisesRegex(ValueError,'No candidates'):stage(p,ir())
    def test_cross_library_alias_collision_fails_whole_package(self):
        p=profile();p['libraries'].append({'class':'/Script/Engine.OtherLibrary','prefix':'ue_test_Foo_','include':['*']})
        source=ir();source['functions'][0]['function']='Foo_Bar'
        other=copy.deepcopy(source['functions'][0]);other.update({'class':'/Script/Engine.OtherLibrary','function':'Bar'})
        source['functions'].append(other)
        source['classes'].append(dict(type_path='/Script/Engine.OtherLibrary',parent='/Script/Engine.BlueprintFunctionLibrary'))
        with self.assertRaisesRegex(ValueError,'duplicate alias'):stage(p,source)


class ReceiverPackages(unittest.TestCase):
    def receiver(self):
        p=profile();p['receivers']=p.pop('libraries')
        source=ir();source['functions'][0]['static']=False
        return p,source
    def test_receiver_only_package_uses_instance_frames(self):
        p,source=self.receiver();result=stage(p,source)
        self.assertIn('target :',result[1]);self.assertIn('ue_begin_call(',result[1])
        self.assertEqual(result[3]['surfaces']['receiver']['generation_ready'],1)
    def test_receiver_package_includes_native_statics(self):
        p,source=self.receiver()
        valid=copy.deepcopy(source['functions'][0]);valid.update(function='Static',static=True)
        source['functions'].append(valid)
        result=stage(p,source)
        self.assertEqual(result[3]['counts']['generation_ready'],2)
        self.assertIn('ue_begin_static_call(',result[1])
        self.assertIn('ue_begin_call(',result[1])
    def test_library_still_excludes_instance_methods(self):
        source=ir();instance=copy.deepcopy(source['functions'][0]);instance.update(function='Instance',static=False)
        source['functions'].append(instance)
        self.assertEqual(stage(profile(),source)[3]['counts']['policy_excluded'],1)
    def test_inheritance_counts_receiver_occurrences_and_declarations(self):
        p,source=self.receiver();p['receivers'][0]['include_inherited']=True
        other=copy.deepcopy(p['receivers'][0]);other.update({'class':'/Script/Engine.Derived','prefix':'ue_derived_'})
        p['receivers'].append(other)
        original=source['functions'][0];original['declaring_class']=original['class']
        derived=copy.deepcopy(original);derived['class']=other['class'];source['functions'].append(derived)
        source['classes'].append(dict(type_path=other['class'],parent=original['class']))
        report=stage(p,source)[3]
        self.assertEqual(report['candidate_count'],2);self.assertEqual(report['unique_declaration_count'],1)
        self.assertTrue(selection(p)['class_rules'][0]['include_inherited'])
    def test_missing_declaring_class_in_inherited_diagnostic_is_counted_as_unknown(self):
        p,source=self.receiver();p['receivers'][0]['include_inherited']=True
        source['diagnostics']=[{'class':'/Script/Engine.TestLibrary','function':'Unknown','reason':'not callable'}]
        source['functions'][0]['declaring_class']='/Script/Engine.TestLibrary'
        report=stage(p,source)[3]
        self.assertEqual(report['unique_declaration_count'],1)
        self.assertEqual(report['unknown_declaration_occurrences'],1)
    def test_empty_struct_identity_is_reported_as_blocker(self):
        source=ir();fn=copy.deepcopy(source['functions'][0]);fn['function']='SoftRef'
        fn['parameters'][0].update(kind='struct',type_path='');source['functions'].append(fn)
        report=stage(profile(),source)[3]
        self.assertEqual(report['counts']['generation_blocked'],1)
        self.assertIn('Missing reflected struct identity',next(m['reason'] for m in report['members'] if m['status']=='generation_blocked'))
    def test_class_may_not_appear_in_both_surfaces(self):
        p=profile();p['receivers']=copy.deepcopy(p['libraries'])
        with self.assertRaisesRegex(ValueError,'unique'):selection(p)
    def test_world_probe_needs_live_world_setting(self):
        p=profile();p['world_probe']='source'
        with self.assertRaisesRegex(ValueError,'world_probe'):selection(p)


if __name__=='__main__':unittest.main()
