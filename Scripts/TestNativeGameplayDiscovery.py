import json,tempfile,unittest
from pathlib import Path
from DiscoverNativeGameplay import declarations,discover

class DiscoveryTests(unittest.TestCase):
 def setUp(self):
  self.temp=tempfile.TemporaryDirectory();self.addCleanup(self.temp.cleanup);self.root=Path(self.temp.name)
 def write(self,name,source):
  (self.root/(name+'.das')).write_text(source,encoding='utf-8')
 def wrapper(self,name='LightRef',path='/Script/Engine.PointLightComponent'):
  return '[ue_native_object(path="'+path+'") ]\nstruct '+name+' { handle:UObjectHandle }\n'
 def run_discovery(self,entries=('main',)):
  return discover(entries,[self.root])
 def test_transitive_imports_and_full_receivers(self):
  self.write('main','require types\ndef unrelated():void {}')
  self.write('types','module types public\nrequire base public\n'+self.wrapper())
  self.write('base','module base public\n'+self.wrapper('ObjectRef','/Script/CoreUObject.Object'))
  result=self.run_discovery()
  self.assertEqual([v['path'] for v in result['classes']],['/Script/CoreUObject.Object','/Script/Engine.PointLightComponent'])
  self.assertEqual(result['modules'],['base','types'])
 def test_generated_declarations_do_not_feed_back(self):
  self.write('main','require ue_native_gameplay')
  self.write('ue_native_gameplay','module ue_native_gameplay public\n'+self.wrapper('Wrong','/Script/Engine.Wrong'))
  self.write('ue_native_gameplay_sources','require types')
  self.write('types',self.wrapper())
  self.assertEqual(len(self.run_discovery()['classes']),1)
  (self.root/'ue_native_gameplay.das').unlink()
  self.assertEqual(len(self.run_discovery()['classes']),1)
 def test_comments_and_strings_are_not_declarations(self):
  source='// require missing\n/* '+self.wrapper()+' /* nested */ */\nlet text="require missing"\n'+self.wrapper()
  self.assertEqual(declarations(source)[1],[])
  self.assertEqual(len(declarations(source)[2]),1)
 def test_multiple_annotations_and_multiline(self):
  source='[other, ue_native_object(\npath = "/Script/Engine.PointLightComponent"\n)]\nstruct LightRef {}'
  self.assertEqual(declarations(source)[2],[('/Script/Engine.PointLightComponent','LightRef')])
 def test_duplicate_paths_rejected(self):
  self.write('main','require a\nrequire b')
  self.write('a',self.wrapper());self.write('b',self.wrapper('Other'))
  with self.assertRaisesRegex(ValueError,'Ambiguous native'):self.run_discovery()
 def test_generated_dependency_cycle_rejected(self):
  self.write('main','require ue_native_gameplay\n'+self.wrapper())
  self.write('ue_native_gameplay_sources','')
  with self.assertRaisesRegex(ValueError,'depends on generated'):self.run_discovery()
 def test_missing_import_rejected(self):
  self.write('main','require missing\n'+self.wrapper())
  with self.assertRaisesRegex(ValueError,'Missing DAS module'):self.run_discovery()
 def test_external_module_allowed(self):
  self.write('main','require ue_foundation\n'+self.wrapper())
  self.assertEqual(len(self.run_discovery()['classes']),1)
 def test_invalid_path_rejected(self):
  with self.assertRaisesRegex(ValueError,'Invalid native path'):declarations(self.wrapper(path='/Game/BP.BP_C'))
 def test_computed_path_rejected(self):
  with self.assertRaisesRegex(ValueError,'literal path'):declarations('[ue_native_object(path=SomeConstant)] struct Ref {}')
 def test_annotation_requires_struct(self):
  with self.assertRaisesRegex(ValueError,'annotate a struct'):declarations('[ue_native_object(path="/Script/Engine.Actor")] def wrong() {}')
 def test_module_mismatch_rejected(self):
  self.write('main','module another\n'+self.wrapper())
  with self.assertRaisesRegex(ValueError,'module/file mismatch'):self.run_discovery()
 def test_import_order_deterministic(self):
  self.write('main','require a\nrequire b');self.write('a',self.wrapper())
  self.write('b',self.wrapper('ObjectRef','/Script/CoreUObject.Object'))
  before=self.run_discovery();self.write('main','require b\nrequire a')
  self.assertEqual(before,self.run_discovery())
 def test_ambiguous_search_path_rejected(self):
  self.write('main',self.wrapper())
  with tempfile.TemporaryDirectory() as second:
   (Path(second)/'main.das').write_text(self.wrapper())
   with self.assertRaisesRegex(ValueError,'Ambiguous DAS module'):discover(['main'],[self.root,second])
 def test_no_receivers_rejected(self):
  self.write('main','')
  with self.assertRaisesRegex(ValueError,'No native object'):self.run_discovery()
 def test_unterminated_lexemes_rejected(self):
  for source in ['/* comment','"string']:
   with self.assertRaisesRegex(ValueError,'Unterminated'):declarations(source)
 def test_native_enum_discovered(self):
  self.write('main',self.wrapper()+'[ue_native_enum(path="/Script/Engine.ECollisionChannel")] enum Channel { A }')
  self.assertEqual(self.run_discovery()['enums'][0]['name'],'Channel')
if __name__=='__main__':unittest.main()
