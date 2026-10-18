"""Create a new DAS starter project; never overwrite an existing destination."""
import argparse,json,os,shutil
from pathlib import Path
ROOT=Path(__file__).resolve().parent.parent
def main():
 p=argparse.ArgumentParser();p.add_argument('destination',type=Path);p.add_argument('--engine',type=Path,default=Path(r'C:\src\unreal\UnrealEngine'));a=p.parse_args()
 destination=a.destination.resolve()
 if destination.exists():raise SystemExit('Destination already exists: '+str(destination))
 if not (ROOT/'ThirdParty/daScript/include/daScript/daScript.h').exists():raise SystemExit('Prepare the plugin daScript dependency first')
 shutil.copytree(ROOT/'Examples/Standalone/Template',destination)
 project=destination/'DasStarter.uproject';config=json.loads(project.read_text())
 config['AdditionalPluginDirectories']=[str(ROOT/'Plugins')]
 # Reuse an existing registration, without installing or changing the user's engine.
 try:
  import winreg
  with winreg.OpenKey(winreg.HKEY_CURRENT_USER,r'Software\Epic Games\Unreal Engine\Builds') as key:
   for index in range(winreg.QueryInfoKey(key)[1]):
    name,value,_=winreg.EnumValue(key,index)
    if Path(value).resolve()==a.engine.resolve():config['EngineAssociation']=name;break
 except OSError:pass
 project.write_text(json.dumps(config,indent=2)+'\n',encoding='utf-8')
 bindings=destination/'Bindings';bindings.mkdir()
 sources={'entries':['starter_character','starter_pickup'],'script_dirs':['../Content/Das/Scripts',os.path.relpath(ROOT/'Plugins/DasForUE/Content/Das/Scripts',bindings).replace('\\','/')],'external_modules':['ue_foundation']}
 (bindings/'NativeGameplay.sources.json').write_text(json.dumps(sources,indent=2)+'\n',encoding='utf-8')
 (destination/'.gitignore').write_text('/Binaries/\n/Intermediate/\n/Saved/\n/DerivedDataCache/\n/Build/\n*.sln\n.vs/\n',encoding='utf-8')
 print('DAS_STARTER_CREATE=PASS project='+str(project))
if __name__=='__main__':main()
