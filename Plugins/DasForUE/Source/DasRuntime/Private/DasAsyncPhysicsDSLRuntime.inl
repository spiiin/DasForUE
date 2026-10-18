struct FDSLAsyncPhysicsState {
 FDasDeclaredAsyncPhysics Spec;
 TSharedPtr<IDasPreparedAsyncPhysicsVM> Prepared;
 TSharedPtr<IDasAsyncPhysicsVM> VM;
 FDasPhysicsPacket Input;
 bool Activated=false;
 FString ActivationError;
};
TMap<TWeakObjectPtr<UObject>,TWeakPtr<FDSLAsyncPhysicsState>> DSLPhysicsOwners;
bool DSLPhysicsOwnerReady(UObject* O){auto* C=Cast<UActorComponent>(O);auto* A=C?C->GetOwner():Cast<AActor>(O);return IsValid(A) && A->HasActorBegunPlay() && (!C || C->HasBegunPlay());}
bool ActivateDSLPhysics(FDSLAsyncPhysicsState& State,UObject* Owner,FString& Error){
 if(State.Activated){Error=State.ActivationError;return Error.IsEmpty();}State.Activated=true;
 auto VM=State.Prepared?State.Prepared->Start(Error):nullptr;
 if(!VM){State.ActivationError=Error;return false;}
 for(const auto& Pair:State.Spec.Bodies){
  UObject* Target=Owner;FString Name=Pair.Value;
  if(Name.StartsWith(TEXT("owner."))){auto* C=Cast<UActorComponent>(Owner);Target=C?C->GetOwner():nullptr;Name.RightChopInline(6);}
  auto* P=Target?FindFProperty<FObjectPropertyBase>(Target->GetClass(),*Name):nullptr;
  auto* Body=P?Cast<UPrimitiveComponent>(P->GetObjectPropertyValue_InContainer(Target)):nullptr;
  if(!VM->BindBody(Pair.Key,Body,Error)){VM->Revoke();Error=TEXT("Async Physics body ")+Pair.Value+TEXT(": ")+Error;State.ActivationError=Error;return false;}
 }
 if(!VM->Publish(State.Input,Error)){VM->Revoke();State.ActivationError=Error;return false;}State.VM=MoveTemp(VM);State.Prepared.Reset();return true;
}
bool CompileDSLPhysics(FCompiledContext& Candidate,const FCompiledContext* Previous,const FString& FullPath,UObject* Owner,FSourceSet& Sources,bool Parent,bool Activate,FString& Error){
 if(Parent || Candidate.DSLDeclaration.IsEmpty())return true;
 FDasAuthoringModule Model;if(!FDasAuthoringModule::FromJson(Candidate.DSLDeclaration,Model,Error))return false;
 const FString Id=DasHostIdentity(Owner);const auto* Declared=Model.Classes.FindByPredicate([&](const auto& C){return C.Id==Id;});
 if(!Declared){Candidate.bPhysicsForeignSession=true;return true;}Candidate.bPhysicsOwner=true;
 FDasDeclaredClass C;if(!Model.ResolveClass(Id,C,Error))return false;if(!C.AsyncPhysics)return true;
 auto State=MakeShared<FDSLAsyncPhysicsState>();State->Spec=*C.AsyncPhysics;
 if(Previous && Previous->Physics)State->Input=Previous->Physics->Input;
 const FString Path=FPaths::Combine(FPaths::GetPath(FullPath),State->Spec.ScriptFile);
 Sources.Add(Path,FSourceSnapshot::Read(Path));
 State->Prepared=PreparePhysicsVMImpl(Path,Owner,Error,&Sources);if(!State->Prepared){Error=TEXT("Async Physics preparation: ")+Error;return false;}
 if(Activate && !ActivateDSLPhysics(*State,Owner,Error))return false;
 Candidate.Physics=MoveTemp(State);return true;
}
TSharedPtr<FDSLAsyncPhysicsState> DSLPhysicsContext(das::Context* C){
 if(!ActiveContext || C!=ActiveContext || !ActiveContext->Self.IsValid() || ActiveContext->bEnding || ConstructingObject)C->throw_error("async physics mailbox requires live gameplay self outside construction/EndPlay");
 auto* Ref=DSLPhysicsOwners.Find(ActiveContext->Self);auto State=Ref?Ref->Pin():nullptr;
 if(!State)C->throw_error("declare ue_async_physics on the owning class before accessing its mailbox");return State;
}
void AsyncPhysicsSetInput(int32 Slot,double Value,das::Context* C){
 auto State=DSLPhysicsContext(C);if(Slot<0 || Slot>=16 || !FMath::IsFinite(Value))C->throw_error("async physics input requires slot 0..15 and finite value");
 auto Input=State->Input;Input.Values[Slot]=Value;FString Error;
 if(State->VM && !State->VM->Publish(Input,Error))C->throw_error(TCHAR_TO_UTF8(*Error));State->Input=Input;
}
bool AsyncPhysicsHasOutput(das::Context* C){auto State=DSLPhysicsContext(C);FDasPhysicsOutput Out;return State->VM && State->VM->Read(Out);}
double AsyncPhysicsOutput(int32 Slot,das::Context* C){auto State=DSLPhysicsContext(C);FDasPhysicsOutput Out;if(Slot<0 || Slot>=16 || !State->VM || !State->VM->Read(Out))C->throw_error("async physics output is unavailable or slot invalid");return Out.Packet.Values[Slot];}
bool AsyncPhysicsRunning(das::Context* C){auto State=DSLPhysicsContext(C);return State->VM && State->VM->IsLive();}
char* AsyncPhysicsError(das::Context* C){auto State=DSLPhysicsContext(C);return CopyDSLString(State->VM?State->VM->LastError():State->ActivationError,C);}
