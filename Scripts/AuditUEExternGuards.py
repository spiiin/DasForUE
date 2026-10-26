"""Fail closed when UE native binds bypass the common execution-role factory."""
import argparse,hashlib,json,re
from pathlib import Path
def main():
 p=argparse.ArgumentParser();p.add_argument('--repo',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
 root=a.repo/'Plugins/DasForUE/Source/DasRuntime';factory=root/'Private/DasUEExternGuard.inl';raw=[];sites=[];hashes={};physics_sites=0
 for f in root.rglob('*'):
  if f.suffix not in ('.h','.cpp','.inl'):continue
  text=f.read_text(encoding='utf-8-sig')
  if f!=factory and re.search(r'\b(?:addExtern|makeExtern|addInterop)\s*<',text):raw.append(str(f))
  physics_sites+=len(re.findall(r'\bAddPhysicsDataExtern\s*<',text))
  count=len(re.findall(r'\b(?:AddUEExtern|AddUEInterop)\s*<',text))
  if count:sites.append(dict(path=str(f.relative_to(a.repo)),registrations=count));hashes[str(f.relative_to(a.repo))]=hashlib.sha256(f.read_bytes()).hexdigest()
 if raw:raise ValueError('Unguarded native binding factory: '+', '.join(raw))
 text=factory.read_text(encoding='utf-8-sig')
 # Contract checks ignore whitespace so clang-format layout does not matter.
 flat=re.sub(r'\s+','',text);has=lambda needle:re.sub(r'\s+','',needle) in flat
 if not has('&TCheckedUEExtern<F>::template Invoke<Fn>') or not has('GuardUEExtern();return Fn(') or not has('Bound->noAot = !ConfigureUEAot<F, Fn>(Bound, Name)'):raise ValueError('Missing guarded pointer/AOT contract')
 if not has('reinterpret_cast<void*>(&TCheckedUEExtern<F>::template Invoke<Fn>)') or not has('if constexpr (dasue::aot::Signature<F>::Supported)'):raise ValueError('AOT bypasses checked pointer or signature gate')
 runtime=(root/'Private/DasRuntime.cpp').read_text(encoding='utf-8-sig')
 resolve=runtime.split('void* dasue::aot::Resolve(',1)[1].split('// Declared by DasTypeAdapters.cpp;',1)[0]
 if 'GuardUEExtern();' not in resolve or 'Context != ActiveContext' not in resolve or '!IsInGameThread()' not in resolve:raise ValueError('Missing AOT execution guard')
 if not has('&TCheckedPhysicsDataExtern<F>::template Invoke<Fn>') or not has('GuardPhysicsDataExtern();return Fn('):raise ValueError('Missing isolated physics data guard')
 if physics_sites!=4 or len(re.findall(r'\baddExtern\s*<',text))!=2:raise ValueError('Unexpected physics/native factory sites')
 if not has('das::addInterop<CheckedUEInterop<Fn>, Ret, Args...>') or not has('GuardUEExtern();GuardUEInteropContext(&C);return Fn(C, Node, Args);'):raise ValueError('Missing checked interop binding')
 if not has('Bound->result->getMangledName()') or not has('!std::is_same_v<Ret, das::Context*>'):raise ValueError('Interop ABI key omits result or exposes context return')
 if not has('reinterpret_cast<void*>(&CheckedUEInterop<Fn>)') or not has('Bound->noAot = !CanAot'):raise ValueError('Interop AOT bypasses checked pointer/signature gate')
 bridge=(root/'Public/DasUEAot.h').read_text(encoding='utf-8-sig')
 if 'Interop(das::Context& Context' not in bridge or 'Resolve(Id, &Context)' not in bridge:raise ValueError('Missing checked interop AOT bridge')
 count=sum(s['registrations'] for s in sites)
 if count<450:raise ValueError('Unexpectedly small UE extern coverage')
 result=dict(state='PASS',source_registration_sites=count,physics_data_registration_sites=physics_sites,sites=sites,hashes=hashes,factory_sha256=hashlib.sha256(factory.read_bytes()).hexdigest(),scope='DasRuntime C++/header/inl registration source. Runtime count includes loop-expanded overloads/aliases. Third-party daScript builtins and external plugins are outside this inventory.')
 a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8');print(f'DAS_UE_EXTERN_GUARDS=PASS source_sites={count}')
if __name__=='__main__':main()
