import copy,unittest
from GenerateBindings import generate
class WeakMarkerTests(unittest.TestCase):
 def fixture(self):
  return dict(schema_version=5,classes=[dict(type_path='/Script/CoreUObject.Object',parent='')],functions=[],structs=[dict(type_path='/Script/Test.WeakValue',cpp_type='FWeakValue',fields=[dict(path='target',kind='object',type_path='/Script/CoreUObject.Object',weak_reference=True)])])
 def test_weak_view_uses_checked_object_transport(self):
  text=generate(self.fixture(),module='ue_weak_test');self.assertIn('f_target : C_Object_',text);self.assertIn('ue_cast_nullable_object',text)
 def test_marker_requires_object_kind(self):
  value=self.fixture();value['structs'][0]['fields'][0].update(kind='int',type_path='')
  with self.assertRaisesRegex(ValueError,'weak reference'):generate(value)
 def test_marker_must_be_boolean(self):
  value=self.fixture();value['structs'][0]['fields'][0]['weak_reference']='true'
  with self.assertRaisesRegex(ValueError,'weak reference'):generate(value)
class WeakNativeMarkerTests(unittest.TestCase):
 def fixture(self):
  path='/Script/CoreUObject.Object'
  return dict(schema_version=5,classes=[dict(path=path,fields=[dict(name='Weak',kind='object',type_path=path,signature='Weak:WeakObjectProperty',declaring_class=path,readonly=False,weak_reference=True)],methods=[])],structs=[],enums=[],discovery=dict(schema_version=1,modules=['ue_generated'],classes=[dict(path=path,module='ue_generated',name='C_Object_55c3753a')]))
 def test_native_weak_descriptor(self):
  from GenerateNativeGameplay import generate as native_generate
  header,module=native_generate(self.fixture());self.assertIn('Weak:WeakObjectProperty',header)
 def test_native_weak_marker_type(self):
  from GenerateNativeGameplay import generate as native_generate
  value=self.fixture();value['classes'][0]['fields'][0]['weak_reference']='true'
  with self.assertRaisesRegex(ValueError,'weak reference'):native_generate(value)
 def test_native_weak_marker_kind(self):
  from GenerateNativeGameplay import generate as native_generate
  value=self.fixture();value['classes'][0]['fields'][0].update(kind='int',type_path='')
  with self.assertRaisesRegex(ValueError,'weak reference'):native_generate(value)
if __name__=='__main__':unittest.main()
