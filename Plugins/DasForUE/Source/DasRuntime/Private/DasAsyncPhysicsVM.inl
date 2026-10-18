namespace {
struct FPhysicsVMMailbox {
 FDasPhysicsPacket Input;
 FDasPhysicsOutput Output;
 uint64 InputSequence=0;
 bool HasOutput=false;
 FString Error;
 TWeakObjectPtr<UDasAsyncPhysicsBridge> Bridge;
 TWeakPtr<FDasPhysicsBodyBindings> Bodies;
 bool Ordinary()const{return IsInGameThread() && GetDasExecutionRole()==EDasExecutionRole::Gameplay && !IsDasFrozenPhysicsWorld(nullptr);}
 bool Live()const{auto* B=Bridge.Get();return B && B->IsLiveBinding();}
};
struct FIsolatedPhysicsVM {
 // The bridge exclusively owns this state, including the prepared interpreter.
 das::ModuleGroup Modules;
 das::FileAccessPtr Access;
 das::ProgramPtr Program;
 TUniquePtr<das::Context> Context;
 das::SimFunction* Tick=nullptr;
 TSharedRef<FPhysicsVMMailbox> Mailbox;
 TSharedRef<FDasPhysicsBodyBindings> Bodies;
 explicit FIsolatedPhysicsVM(TSharedRef<FPhysicsVMMailbox> In,UObject* Owner):Mailbox(In),Bodies(MakeShared<FDasPhysicsBodyBindings>(Owner)){Mailbox->Bodies=Bodies;}
 ~FIsolatedPhysicsVM(){
  // Context destruction runs DAS [shutdown] code too. Keep the same UE ban.
  TGuardValue<das::Context*> Restricted(RestrictedPhysicsContext,Context.Get());
  Context.Reset();
 }
 bool Evaluate(const FDasAsyncPhysicsStep& Step,FString& Error){
  if(!Step.IsCurrent() || ActivePhysicsVMFrame || RestrictedPhysicsContext){Error=TEXT("isolated physics VM reentry rejected");Mailbox->Error=Error;return false;}
  // Copy bounded input once and stage a complete replacement output. Exceptions
  // discard all writes; an untouched output slot is zero for this step.
  const FDasPhysicsPacket Input=Mailbox->Input;const uint64 InputSequence=Mailbox->InputSequence;
  FDasPhysicsPacket Output;
  FDasPhysicsBodyScope BodyScope(*Bodies,Step);
  FPhysicsVMFrame Frame{Context.Get(),&Step,&Input,&Output,false,&BodyScope};
  TGuardValue<FPhysicsVMFrame*> FrameScope(ActivePhysicsVMFrame,&Frame);
  TGuardValue<das::Context*> Restricted(RestrictedPhysicsContext,Context.Get());
  Context->restart();vec4f Args[]={das::cast<float>::from(Step.DeltaTime),das::cast<float>::from(Step.SimTime)};
  try{Context->evalWithCatch(Tick,Args);}catch(const std::exception& E){Error=UTF8_TO_TCHAR(E.what());}catch(...){Error=TEXT("isolated physics VM evaluation failed");}
  if(const auto* E=Context->getException())Error=UTF8_TO_TCHAR(E);
  if(Frame.Failed && Error.IsEmpty())Error=TEXT("physics data transaction rejected");
  if(Error.IsEmpty())BodyScope.Commit(Error);
  if(!Error.IsEmpty()){Mailbox->Error=Error;return false;}
  Mailbox->Output.Packet=Output;Mailbox->Output.InputSequence=InputSequence;++Mailbox->Output.StepSequence;Mailbox->HasOutput=true;return true;
 }
};
class FPhysicsVMHandle final:public IDasAsyncPhysicsVM {
 TSharedRef<FPhysicsVMMailbox> Mailbox;
public:
 explicit FPhysicsVMHandle(TSharedRef<FPhysicsVMMailbox> In):Mailbox(In){}
 virtual bool Publish(const FDasPhysicsPacket& Input,FString& Error)override{
  Error.Reset();if(!Mailbox->Ordinary() || !Mailbox->Live()){Error=TEXT("physics input requires ordinary game thread and live binding");return false;}
  for(double V:Input.Values)if(!FMath::IsFinite(V)){Error=TEXT("physics input must be finite");return false;}
  Mailbox->Input=Input;++Mailbox->InputSequence;return true;
 }
 virtual bool Read(FDasPhysicsOutput& Out)const override{if(!Mailbox->Ordinary() || !Mailbox->Live() || !Mailbox->HasOutput)return false;Out=Mailbox->Output;return true;}
 virtual bool BindBody(int32 Slot,UPrimitiveComponent* Body,FString& Error)override{
  if(!Mailbox->Ordinary() || !Mailbox->Live()){Error=TEXT("body binding requires ordinary game thread and live VM");return false;}
  auto B=Mailbox->Bodies.Pin();if(!B){Error=TEXT("physics body permissions expired");return false;}return B->Bind(Slot,Body,Error);
 }
 virtual bool ClearBody(int32 Slot,FString& Error)override{
  if(!Mailbox->Ordinary() || !Mailbox->Live()){Error=TEXT("body unbinding requires ordinary game thread and live VM");return false;}
  auto B=Mailbox->Bodies.Pin();if(!B){Error=TEXT("physics body permissions expired");return false;}return B->Clear(Slot,Error);
 }
 virtual uint64 BindingRevision()const override{if(!Mailbox->Ordinary())return 0;auto* B=Mailbox->Bridge.Get();return B && B->IsLiveBinding()?B->GetBindingRevision():0;}
 virtual bool IsLive()const override{return IsInGameThread() && Mailbox->Live();}
 virtual FString LastError()const override{if(!Mailbox->Ordinary())return TEXT("physics status requires ordinary game thread");if(!Mailbox->Error.IsEmpty())return Mailbox->Error;auto* B=Mailbox->Bridge.Get();return B?B->GetLastError():FString();}
 virtual void Revoke()override{if(Mailbox->Ordinary())if(auto* B=Mailbox->Bridge.Get())B->Revoke();}
};
struct FPreparedPhysicsVM final:IDasPreparedAsyncPhysicsVM {
 TSharedPtr<FIsolatedPhysicsVM> VM;TWeakObjectPtr<UObject> Owner;
 FPreparedPhysicsVM(TSharedPtr<FIsolatedPhysicsVM> In,UObject* O):VM(MoveTemp(In)),Owner(O){}
 virtual TSharedPtr<IDasAsyncPhysicsVM> Start(FString& Error)override{
  Error.Reset();if(!IsInGameThread() || GetDasExecutionRole()!=EDasExecutionRole::Gameplay || IsDasFrozenPhysicsWorld(Owner.Get()) || !VM){Error=TEXT("prepared physics VM start requires ordinary game thread and unused preparation");return nullptr;}
  auto State=MoveTemp(VM);auto Mailbox=State->Mailbox;
  auto* Bridge=UDasAsyncPhysicsBridge::Create(Owner.Get(),[State](const FDasAsyncPhysicsStep& S,FString& E){return State->Evaluate(S,E);},Error);
  if(!Bridge)return nullptr;Mailbox->Bridge=Bridge;return MakeShared<FPhysicsVMHandle>(Mailbox);
 }
};
TSharedPtr<IDasPreparedAsyncPhysicsVM> PreparePhysicsVMImpl(const FString& Path,UObject* Owner,FString& Error,FSourceSet* Sources){
 Error.Reset();if(!IsInGameThread() || GetDasExecutionRole()!=EDasExecutionRole::Gameplay || IsDasFrozenPhysicsWorld(Owner) || RestrictedPhysicsContext){Error=TEXT("physics VM preparation requires ordinary game thread");return nullptr;}
 auto Mailbox=MakeShared<FPhysicsVMMailbox>();auto VM=MakeShared<FIsolatedPhysicsVM>(Mailbox,Owner);
 try{
  das::TextWriter Output;auto Access=das::make_smart<FUnrealFileAccess>();Access->addFileSystem(new FUnrealFileSystem(Sources),true,false);VM->Access=Access;
  das::CodeOfPolicies Policies;Policies.aot=false;Policies.jit_enabled=false;Policies.no_unsafe=true;Policies.no_global_heap=true;
  Policies.stack=64*1024;Policies.max_static_variables_size=64*1024;Policies.max_heap_allocated=1024*1024;Policies.max_string_heap_allocated=1024*1024;
  const FTCHARToUTF8 File(*Path);VM->Program=DSLCompile(File.Get(),VM->Access,Output,VM->Modules,Policies);
  if(!VM->Program || VM->Program->failed()){Error=Errors(VM->Program,Output);return nullptr;}
  const int32 StackSize=VM->Program->getContextStackSize();
  if(StackSize<16*1024 || StackSize>64*1024){Error=TEXT("physics VM requires private stack 16..64 KiB");return nullptr;}
  // Source options can override lint policies. Validate the final used globals
  // ourselves before any initializer runs; shared and heap-backed globals are
  // not allowed in this value-only VM contract.
  bool GlobalsValid=true;
  VM->Program->library.foreach([&](das::Module* Module){
   Module->globals.foreach([&](auto V){if(VM->Program->isUsed(V) && (V->global_shared || !V->type->isNoHeapType()))GlobalsValid=false;});return true;
  },"*");
  if(!GlobalsValid){Error=TEXT("physics VM globals must be private no-heap values");return nullptr;}
  // Simulation reads source options before policy defaults. Pin these before
  // initialization as well as on the resulting allocators.
  for(auto& Option:VM->Program->options){
   if(Option.name=="heap_size_limit" || Option.name=="max_heap_allocated" || Option.name=="string_heap_size_limit" || Option.name=="max_string_heap_allocated")Option=das::AnnotationArgument(Option.name,uint64_t(1024*1024));
  }
  VM->Context=MakeUnique<das::Context>(VM->Program->getContextStackSize());
  TGuardValue<das::Context*> Restricted(RestrictedPhysicsContext,VM->Context.Get());
  if(!VM->Program->simulate(*VM->Context,Output)){Error=Errors(VM->Program,Output);if(auto* E=VM->Context->getException())Error=UTF8_TO_TCHAR(E);return nullptr;}
  // Host caps override script heap hints, including explicit source options.
  VM->Context->heap->setLimit(1024*1024);VM->Context->stringHeap->setLimit(1024*1024);
  VM->Tick=VM->Context->findFunction("physics_tick");
  if(!VM->Tick || !das::verifyCall<void,float,float>(VM->Tick->debugInfo,VM->Modules)){Error=TEXT("expected exported physics_tick(dt:float,time:float):void");return nullptr;}
 }catch(const std::exception& E){Error=UTF8_TO_TCHAR(E.what());return nullptr;}catch(...){Error=TEXT("physics VM preparation failed");return nullptr;}
 return MakeShared<FPreparedPhysicsVM>(VM,Owner);
}
}
TSharedPtr<IDasPreparedAsyncPhysicsVM> PrepareDasAsyncPhysicsVM(const FString& Path,UObject* Owner,FString& Error){return PreparePhysicsVMImpl(Path,Owner,Error,nullptr);}
TSharedPtr<IDasAsyncPhysicsVM> CreateDasAsyncPhysicsVM(const FString& Path,UObject* Owner,FString& Error){auto P=PrepareDasAsyncPhysicsVM(Path,Owner,Error);return P?P->Start(Error):nullptr;}
