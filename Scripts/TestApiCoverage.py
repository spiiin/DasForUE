import copy,unittest
from AuditApiCoverage import audit,library_wrappers,markdown

def member(ident,domain='method',eligible=True,reasons=(),issues=()):
    owner,name=ident.rsplit('.',1)
    return {'id':domain+':'+ident,'domain':domain,'name':name,'declaring_class':owner,'signature':'shape' if eligible else '', 'eligible':eligible,'primary_reason':next(iter(reasons),''),'reasons':list(reasons),'type_issues':list(issues)}

class ApiCoverageTests(unittest.TestCase):
    def setUp(self):
        self.base=member('/Script/Engine.Base.Run')
        self.blocked=member('/Script/Engine.Base.Delay',eligible=False,reasons=['metadata:Latent','unsupported_parameter_type'])
        self.field=member('/Script/Engine.Base.Label',domain='field',eligible=False,reasons=['unsupported_property_type'],issues=[{'property_kind':'TextProperty','cpp_type':'FText','path':'Label'}])
        self.inv={'schema_version':1,'engine_version':'fixture','scope':'test','profile':{'modules':['Engine']},'excluded_classes':[],
          'members':[self.base,self.blocked,self.field], 'classes':[
            {'path':'/Script/Engine.Base','subsystem':'world','function_library':False,'fields':[self.field['id']],'methods':[self.base['id'],self.blocked['id']]},
            {'path':'/Script/Engine.Child','subsystem':'world','function_library':False,'fields':[self.field['id']],'methods':[self.base['id'],self.blocked['id']]}]}
        self.native={'schema_version':5,'classes':[{'path':'/Script/Engine.Base','fields':[],'methods':[{'name':'Run','signature':'shape'}]}]}
        self.libs={'schema_version':5,'functions':[],'diagnostics':[]}
    def run_audit(self):return audit(self.inv,self.native,self.libs,{})
    def test_inherited_occurrences_and_unique_counts_separate(self):
        r,rows=self.run_audit();self.assertEqual(r['summary']['unique_declarations'],3);self.assertEqual(len(rows),6)
        self.assertEqual(r['summary']['states']['native_generated'],1);self.assertEqual(r['summary']['states']['native_eligible_unselected'],1)
    def test_reasons_overlap_without_inherited_double_count(self):
        r,_=self.run_audit();reasons={x['reason']:x['unique_declarations'] for x in r['blockers']};self.assertEqual(reasons['metadata:Latent'],1);self.assertEqual(reasons['unsupported_parameter_type'],1)
    def test_type_only_opportunities_exclude_adapter_members(self):
        r,_=self.run_audit();self.assertEqual(len(r['type_only_opportunities']),1);self.assertEqual(r['type_only_opportunities'][0]['single_shape_candidates'],1)
    def test_stale_registration(self):
        self.native['classes'][0]['methods'][0]['signature']='stale';r,_=self.run_audit();self.assertEqual(r['summary']['states']['native_registration_stale'],1)
    def test_registered_member_missing_is_error(self):
        self.native['classes'][0]['methods'].append({'name':'Gone','signature':'shape'})
        with self.assertRaisesRegex(ValueError,'missing from inventory'):self.run_audit()
    def test_missing_definition_is_error(self):
        self.inv['members'].pop()
        with self.assertRaisesRegex(ValueError,'Missing member'):self.run_audit()
    def test_duplicate_class_is_error(self):
        self.inv['classes'].append(copy.deepcopy(self.inv['classes'][0]))
        with self.assertRaisesRegex(ValueError,'Duplicate reflected classes'):self.run_audit()
    def test_reordered_inventory_is_deterministic(self):
        first=self.run_audit();self.inv['classes'].reverse();self.inv['members'].reverse();self.assertEqual(first,self.run_audit())
    def add_library(self):
        m=member('/Script/Engine.Lib.Time',eligible=False,reasons=['static','metadata:WorldContext']);self.inv['members'].append(m)
        self.inv['classes'].append({'path':'/Script/Engine.Lib','subsystem':'library','function_library':True,'fields':[],'methods':[m['id']]})
        self.libs['functions']=[{'class':'/Script/Engine.Lib','function':'Time','signature':'libshape'}]
    def test_library_route_not_counted_as_adapter_gap(self):
        self.add_library();r,_=self.run_audit();self.assertEqual(r['summary']['states']['library_export_eligible'],1)
        self.assertNotIn('static',{x['reason'] for x in r['blockers']})
    def test_library_wrapper_signature_checked(self):
        self.add_library();r,_=audit(self.inv,self.native,self.libs,{('/Script/Engine.Lib','Time'):'libshape'});self.assertEqual(r['summary']['states']['library_generated'],1)
        r,_=audit(self.inv,self.native,self.libs,{('/Script/Engine.Lib','Time'):'old'});self.assertEqual(r['summary']['states']['library_wrapper_stale'],1)
    def test_wrapper_lexer_skips_phantom_calls(self):
        source='// ue_begin_static_call("fake","x","")\ndef actual() { let call=ue_begin_static_call("/Script/Engine.Lib","Time","");ue_expect_signature(call,"sig") }'
        self.assertEqual(library_wrappers(source),{('/Script/Engine.Lib','Time'):'sig'})
    def test_partition_and_documented_denominators(self):
        r,rows=self.run_audit();self.assertEqual(sum(r['summary']['states'].values()),len(rows));text=markdown(r);self.assertIn('не процент готовности',text);self.assertIn('runtime-валидация',text)
    def test_fixture_receivers_outside_scope_explicit(self):
        self.native['classes'].append({'path':'/Script/DasRuntime.Fixture','fields':[],'methods':[]});r,_=self.run_audit();self.assertEqual(r['summary']['selected_receivers_outside_scope'],['/Script/DasRuntime.Fixture'])
    def test_fixed_array_not_reported_as_unsupported_scalar(self):
        self.field['type_issues']=[{'property_kind':'FloatProperty','cpp_type':'float','array_dim':4,'path':'Density'}]
        r,_=self.run_audit();self.assertEqual(r['type_only_opportunities'][0]['array_dim'],4);self.assertIn('float [fixed dim 4]',markdown(r))
if __name__=='__main__':unittest.main()
