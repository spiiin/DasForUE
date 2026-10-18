namespace {
struct FExecutionGuardProbe final:IDasExecutionGuardProbe {
 FCompiledContext Live;
 virtual int32 ExternRegistrationCount()const override{return UEExternRegistrationCount;}
 virtual FDasExecutionResult Evaluate(int32 Mode)override{
  FDasExecutionResult R;auto& C=*Live.Context;
  if(!IsInGameThread() || C.ExecutionDepth){R.Error=TEXT("prepared probe reentry/thread rejected");return R;}
  C.restart();TGuardValue<int32> Depth(C.ExecutionDepth,1);FActiveContextScope Scope(C);
  vec4f Args[]={das::cast<int32>::from(Mode)};const auto Value=C.evalWithCatch(Live.Function,Args);
  if(auto* E=C.getException())R.Error=UTF8_TO_TCHAR(E);else{R.bSucceeded=true;R.Value=das::cast<int32>::to(Value);}return R;
 }
};
}
TUniquePtr<IDasExecutionGuardProbe> PrepareDasExecutionGuardProbeForTests(const FString& Path,UObject* Owner,FString& Error){
 Error.Reset();if(!IsInGameThread() || GetDasExecutionRole()!=EDasExecutionRole::Gameplay || IsDasFrozenPhysicsWorld(Owner)){Error=TEXT("prepare requires ordinary gameplay role");return nullptr;}
 auto P=MakeUnique<FExecutionGuardProbe>();das::TextWriter Output;
 auto Access=das::make_smart<FUnrealFileAccess>();Access->addFileSystem(new FUnrealFileSystem(),true,false);P->Live.Access=Access;
 const FTCHARToUTF8 File(*Path);das::CodeOfPolicies Policies;Policies.aot=false;
 P->Live.Program=DSLCompile(File.Get(),P->Live.Access,Output,P->Live.Modules,Policies);
 if(!P->Live.Program || P->Live.Program->failed()){Error=Errors(P->Live.Program,Output);return nullptr;}
 P->Live.Context=MakeUnique<FUnrealDasContext>(P->Live.Program->getContextStackSize());auto& C=*P->Live.Context;C.Self=Owner;FActiveContextScope Scope(C);
 if(!P->Live.Program->simulate(C,Output)){Error=Errors(P->Live.Program,Output);return nullptr;}
 P->Live.Function=C.findFunction("main");if(!P->Live.Function || !das::verifyCall<int32,int32>(P->Live.Function->debugInfo,P->Live.Modules)){Error=TEXT("expected exported main(input:int):int");return nullptr;}
 return P;
}
