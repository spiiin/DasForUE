"""Focused reflection/generation check: new DAS-only class and invalid requests."""
import argparse,copy,json,os,subprocess,tempfile
from pathlib import Path
from DiscoverNativeGameplay import discover
from GenerateNativeGameplay import generate

ROOT=Path(__file__).resolve().parent.parent
def main():
 p=argparse.ArgumentParser();p.add_argument('--engine',type=Path,default=Path(r'C:\src\unreal\UnrealEngine'));a=p.parse_args()
 scripts=ROOT/'Plugins/DasForUE/Content/Das/Scripts';config=json.loads((ROOT/'Bindings/NativeGameplay.sources.json').read_text())
 logs=ROOT/'logs';logs.mkdir(exist_ok=True);results={}
 with tempfile.TemporaryDirectory(prefix='NativeDiscovery-',dir=ROOT/'Build') as temp:
  stage=Path(temp)
  (stage/'discovery_light.das').write_text('options gen2\nmodule discovery_light public\nrequire ue_object_types public\n[ue_native_object(path="/Script/Engine.PointLightComponent")]\nstruct DiscoveryLightRef { handle:UObjectHandle }\n',encoding='utf-8')
  request=discover(config['entries']+['discovery_light'],[scripts,stage])
  def export(label,value,success,reason=None):
   req=stage/(label+'.requests.json');out=stage/(label+'.json');log=logs/('native-discovery-'+label+'.log')
   req.write_text(json.dumps(value),encoding='utf-8')
   command=[str(a.engine/'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'),str(ROOT/'Tests/DasForUESmoke/DasForUESmoke.uproject'),'-run=DasNativeGameplay','-Requests='+str(req),'-Output='+str(out),'-Multiprocess','-unattended','-NullRHI','-nosplash','-stdout','-abslog='+str(log),'-ddc=NoZenLocalFallback','-NoZenAutoLaunch']
   env=os.environ.copy();env['UE_SKIP_UBT_SDK_SETUP']='1'
   with (logs/('native-discovery-'+label+'-stdout.log')).open('w',encoding='utf-8') as stream:
    run=subprocess.run(command,stdout=stream,stderr=subprocess.STDOUT,env=env,timeout=90)
   text=log.read_text(encoding='utf-8-sig')
   if success:
    assert run.returncode==0 and 'DAS_NATIVE_GAMEPLAY_EXPORT=PASS' in text,label
    return json.loads(out.read_text(encoding='utf-8-sig'))
   assert run.returncode!=0 and reason in text and not out.exists(),label
   results[label]='PASS'
  ir=export('new-class',request,True)
  receiver=next(c for c in ir['classes'] if c['path']=='/Script/Engine.PointLightComponent')
  assert 'SetIntensity' in {m['name'] for m in receiver['methods']}
  assert 'Intensity' in {m['name'] for m in receiver['fields']}
  header,module=generate(ir,[stage])
  assert 'discovery_light::DiscoveryLightRef' in header
  assert 'require discovery_light public' in module
  results['new_DAS_only_PointLightComponent']={'result':'PASS','fields':len(receiver['fields']),'methods':len(receiver['methods'])}
  assert generate(ir,[stage])==(header,module)
  results['new_class_generation_deterministic']='PASS'
  value=copy.deepcopy(request);value['classes'].append(value['classes'][0]);export('duplicate-class',value,False,'duplicate native receiver')
  value=copy.deepcopy(request);value['classes']=[{'path':'/Script/Engine.NoSuchNativeClass'}];export('missing-class',value,False,'Not a supported native UObject class')
  value=copy.deepcopy(request);value['classes']=[{'path':'/Script/NoSuchDasModule.NoClass'}];export('missing-module',value,False,'Cannot load native module')
 report={'result':'PASS','checks':results,'scope':'Native discovery, actual UE reflection and generated artifacts; no full suites'}
 (logs/'native-discovery-integration.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
 print('DAS_NATIVE_DISCOVERY_INTEGRATION=PASS')
if __name__=='__main__':main()
