import unittest
from GenerateBindings import generate, class_name

OBJECT='/Script/CoreUObject.Object'
HOLDER='/Script/Test.ReferenceArrayHolder'

def fixture(kind='object', marker=None):
    marker=marker or ('ObjectProperty' if kind=='object' else 'ClassProperty')
    element=dict(kind=kind,type_path=OBJECT)
    member=dict(path='targets',kind='array',type_path=marker,element=element)
    return dict(schema_version=5,classes=[dict(type_path=OBJECT,parent='')],
      structs=[dict(type_path=HOLDER,fields=[member])],
      functions=[dict(**{'class':OBJECT},function='Echo',alias='fixture_Echo',signature='v1;',
        parameters=[dict(path='value',kind='struct',type_path=HOLDER,input=True,output=True)],
        fields=[dict(path='value.targets',kind='array',type_path=marker,input=True,output=True),
                dict(path='value.targets.*',kind=kind,type_path=OBJECT,input=True,output=True)])])

class StructReferenceArrays(unittest.TestCase):
    def test_object_arrays_use_checked_object_input_and_nullable_output(self):
        text=generate(fixture())
        self.assertIn('array<'+class_name(OBJECT)+'>',text)
        self.assertIn('ue_arg_object(call, "value.targets.{index}"',text)
        self.assertIn('ue_result_object(call, "value.targets.{output_index_0}"',text)
        self.assertIn('ue_cast_nullable_object',text)
    def test_class_arrays_use_class_bridges(self):
        text=generate(fixture('class'))
        self.assertIn('array<UEClassHandle>',text)
        self.assertIn('ue_arg_class(call, "value.targets.{index}"',text)
        self.assertIn('ue_result_class(call, "value.targets.{output_index_0}"',text)
    def test_missing_object_class_rejected(self):
        data=fixture();data['structs'][0]['fields'][0]['element']['type_path']='/Script/Test.Missing'
        with self.assertRaisesRegex(ValueError,'object class'):generate(data)
    def test_inconsistent_storage_marker_rejected(self):
        for kind,marker in [('object','ClassProperty'),('class','ObjectProperty'),('object','WeakObjectProperty')]:
            with self.subTest(kind=kind,marker=marker):
                with self.assertRaisesRegex(ValueError,'struct array member'):generate(fixture(kind,marker))
    def test_array_inside_nested_struct_preserves_paths(self):
        data=fixture();outer='/Script/Test.Outer'
        data['structs'].append(dict(type_path=outer,fields=[dict(path='inner',kind='struct',type_path=HOLDER)]))
        data['functions'][0]['parameters'][0]['type_path']=outer
        for field in data['functions'][0]['fields']:field['path']=field['path'].replace('value.','value.inner.')
        text=generate(data)
        self.assertIn('"value.inner.targets.{index}"',text)
        self.assertIn('"value.inner.targets.{output_index_0}"',text)

if __name__=='__main__':unittest.main()
