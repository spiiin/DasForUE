"""Record local UE source evidence; no proxy is dereferenced by this source audit."""
import argparse,hashlib,json,subprocess
from pathlib import Path

def main():
 p=argparse.ArgumentParser();p.add_argument('--engine',required=True,type=Path);p.add_argument('--output',required=True,type=Path);a=p.parse_args()
 pattern=r'FBodyInstanceAsyncPhysicsTickHandle|GetBodyInstanceAsyncPhysicsTickHandle|UPhysicsThreadLibrary'
 cmd=['rg','--json','-n','-g','*.h','-g','*.cpp','-g','*.inl',pattern,str(a.engine/'Engine/Source'),str(a.engine/'Engine/Plugins')]
 result=subprocess.run(cmd,capture_output=True,text=True,encoding='utf-8',timeout=180)
 if result.returncode not in (0,1):raise RuntimeError(result.stderr)
 matches=[]
 for line in result.stdout.splitlines():
  item=json.loads(line)
  if item['type']=='match':
   d=item['data'];matches.append(dict(path=str(Path(d['path']['text']).relative_to(a.engine)).replace('\\','/'),line=d['line_number'],text=d['lines']['text'].strip()))
 evidence={}
 anchors={
 'Engine/Source/Runtime/Engine/Classes/PhysicsEngine/BodyInstance.h':['struct FBodyInstanceAsyncPhysicsTickHandle','FPhysicsActorHandle Proxy'],
 'Engine/Source/Runtime/Engine/Private/PhysicsEngine/BodyInstance.cpp':['FBodyInstanceAsyncPhysicsTickHandle::IsValid','return Proxy &&'],
 'Engine/Source/Runtime/Engine/Private/PrimitiveComponentPhysics.cpp':['FBodyInstanceAsyncPhysicsTickHandle UPrimitiveComponent::GetBodyInstanceAsyncPhysicsTickHandle'],
 'Engine/Source/Runtime/Engine/Public/Physics/Experimental/PhysicsThreadLibrary.h':['static void AddForce'],
 'Engine/Source/Runtime/Engine/Private/PhysicsEngine/Experimental/PhysScene_Chaos.cpp':['class FAsyncPhysicsTickCallback','Chaos::ESimCallbackOptions::Presimulate | Chaos::ESimCallbackOptions::RunOnFrozenGameThread','Component->AsyncPhysicsTickComponent','Actor->AsyncPhysicsTickActor'],
 'Engine/Source/Runtime/Experimental/Chaos/Private/Chaos/Framework/PhysicsSolverBase.cpp':['void FPhysicsSolverFrozenGTPreSimCallbacks::GTPreSimCallbacks','FFrozenGameThreadContextScope FrozenScope'],
 'Engine/Source/Runtime/Experimental/Chaos/Public/PhysicsProxy/SingleParticlePhysicsProxy.h':['FRigidBodyHandle_Internal* GetPhysicsThreadAPI','void AddForce(const FVec3& InForce','void VerifyContext() const'],
 'Engine/Source/Runtime/Experimental/Chaos/Public/Framework/Threading.h':['Debug helper to ensure threading mistakes are caught. Do not use for ship','&& !bFrozenGameThread'],
 'Engine/Source/Runtime/Experimental/Chaos/Public/Chaos/Framework/PhysicsProxyBase.h':['void MarkDeleted()','GetSyncTimestamp() const'],
 'Engine/Source/Runtime/Engine/Private/SkeletalMeshComponentPhysics.cpp':['FBodyInstance* USkeletalMeshComponent::GetBodyInstance'],
 'Engine/Source/Runtime/Engine/Private/InstancedStaticMesh.cpp':['FBodyInstance* UInstancedStaticMeshComponent::GetBodyInstance'],
 }
 for rel,needles in anchors.items():
  raw=(a.engine/rel).read_bytes();lines=raw.decode('utf-8-sig').splitlines();hits=[]
  for needle in needles:
   found=[dict(line=n+1,text=line.strip()) for n,line in enumerate(lines) if needle in line]
   if not found:raise ValueError(f'Missing audit evidence {rel}: {needle}')
   hits.extend(found)
  evidence[rel]=dict(sha256=hashlib.sha256(raw).hexdigest(),anchors=hits)
 output=dict(schema_version=1,state='source_audit_complete',engine=str(a.engine),engine_build=json.loads((a.engine/'Engine/Build/Build.version').read_text(encoding='utf-8-sig')),scan_scope=['Engine/Source','Engine/Plugins'],source_extensions=['h','cpp','inl'],source_matches=matches,evidence=evidence,limits='Text-source inventory only; binary Blueprint assets/external projects are not scanned. Runtime audit enumerates loaded reflection separately. Proposed DAS callback/capability transport is not implemented.')
 a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(output,indent=2,ensure_ascii=False)+'\n',encoding='utf-8');print(f'DAS_ASYNC_PHYSICS_SOURCE_AUDIT=PASS matches={len(matches)} files={len(evidence)}')
if __name__=='__main__':main()
