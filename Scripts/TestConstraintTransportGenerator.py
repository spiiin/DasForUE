import copy
import unittest
from GenerateBindings import generate,CONSTRAINT_PATH

def scalar(path='returnvalue',input=False,output=True):
    return dict(path=path,kind='struct',type_path=CONSTRAINT_PATH,transport='constraint_component_v1',input=input,output=output)
def fixture(consumer=False):
    base='/Script/Engine.BlueprintFunctionLibrary';obj='/Script/CoreUObject.Object'
    owner='/Script/Engine.ConstraintInstanceBlueprintLibrary' if consumer else '/Script/Engine.PhysicsConstraintComponent'
    p=scalar('accessor' if consumer else 'returnvalue',input=consumer)
    fn=dict(**{'class':owner},function='SetDisableCollision' if consumer else 'GetConstraint',alias='ue_fixture_constraint',signature='v1;',parameters=[copy.deepcopy(p)],fields=[p],static=consumer,world_context='')
    return dict(schema_version=5,classes=[dict(type_path=obj,parent=''),dict(type_path=base,parent=obj),dict(type_path=owner,parent=base if consumer else obj)],structs=[dict(type_path=CONSTRAINT_PATH,transport='constraint_component_v1',fields=[])],functions=[fn])

class ConstraintTransport(unittest.TestCase):
    def test_component_getter_returns_opaque_handle(self):
        t=generate(fixture());self.assertIn(': UEConstraintHandle',t);self.assertIn('__ue_result_constraint(call, "returnvalue")',t)
        self.assertNotIn('f_owner',t);self.assertNotIn('f_index',t)
    def test_library_ref_accessor_uses_checked_bridges(self):
        t=generate(fixture(True));self.assertIn('__ue_arg_constraint(call, "accessor", arg_accessor)',t);self.assertIn('__ue_result_constraint(call, "accessor")',t)
    def test_missing_marker_rejected(self):
        d=fixture();d['functions'][0]['parameters'][0].pop('transport')
        with self.assertRaisesRegex(ValueError,'transport'):generate(d)
    def test_private_reflected_fields_cannot_replace_transport(self):
        d=fixture();d['structs'][0]['fields']=[dict(path='index',kind='int',type_path='')]
        with self.assertRaisesRegex(ValueError,'opaque native transport'):generate(d)
    def test_other_getter_cannot_claim_component_capability(self):
        d=fixture();d['functions'][0]['function']='GetSkeletalConstraint'
        with self.assertRaisesRegex(ValueError,'component getter/library contract'):generate(d)
    def test_array_of_accessors_rejected_by_provenance_policy(self):
        d=fixture(True);d['functions'][0]['fields'][0]['path']='accessors.*'
        with self.assertRaisesRegex(ValueError,'component getter/library contract'):generate(d)
    def test_instance_library_claim_is_rejected(self):
        d=fixture(True);d['functions'][0]['static']=False
        with self.assertRaisesRegex(ValueError,'component getter/library contract'):generate(d)
    def test_library_cannot_mint_new_accessor_return(self):
        d=fixture(True);d['functions'][0]['fields'][0].update(input=False,path='returnvalue')
        with self.assertRaisesRegex(ValueError,'component getter/library contract'):generate(d)

if __name__=='__main__':unittest.main()
