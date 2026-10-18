UPhysicsConstraintComponent* TryConstraintOwner(const FDasConstraintTransportRecord& R,das::Context* C){
 if(!ActiveContext || C!=ActiveContext || !IsInGameThread() || ConstructingObject || ActiveContext->bCollectingSubscriptions || ActiveContext->bEnding || (LibraryInvocationContract && !ActiveContext->bLibraryScope))return nullptr;
 auto* Owner=R.Owner.Get();auto* Actor=Owner?Owner->GetOwner():nullptr;
 auto* Self=ActiveContext->bLibraryScope?ActiveContext->LibraryWorld.Get():ActiveContext->Self.Get();
 if(!IsValid(Self) || Self->IsTemplate() || (Cast<AActor>(Self) && CastChecked<AActor>(Self)->IsActorBeingDestroyed()) || (Cast<UActorComponent>(Self) && CastChecked<UActorComponent>(Self)->IsBeingDestroyed()))return nullptr;
 auto* World=Self->GetWorld();
 if(!IsValid(Owner) || Owner->IsBeingDestroyed() || !Owner->IsRegistered() || !IsValid(Actor) || Actor->IsActorBeingDestroyed() || !World || !World->IsGameWorld() || World->bIsTearingDown || Owner->GetWorld()!=World)return nullptr;
 return Owner;
}
UPhysicsConstraintComponent* ConstraintOwner(const FDasConstraintTransportRecord& R,das::Context* C){
 auto* Owner=TryConstraintOwner(R,C);if(!Owner)C->throw_error("constraint owner/context expired, unregistered, or belongs to another world");return Owner;
}
TSharedPtr<FProxyTimestampBase,ESPMode::ThreadSafe> ConstraintGeneration(UPhysicsConstraintComponent* Owner){
 TSharedPtr<FProxyTimestampBase,ESPMode::ThreadSafe> Result;
 auto& Handle=Owner->ConstraintInstance.ConstraintHandle;
 if(Handle.IsValid())FPhysicsCommand::ExecuteRead(Handle,[&](const FPhysicsConstraintHandle& Current){
  if(auto* Proxy=Current.Constraint->GetProxy())Result=Proxy->GetSyncTimestamp();
 });
 return Result;
}
UPhysicsConstraintComponent* TryValidateConstraintRecord(const FDasConstraintTransportRecord& R,das::Context* C){
 auto* Owner=TryConstraintOwner(R,C);if(!Owner)return nullptr;
 UPrimitiveComponent *Body1=nullptr,*Body2=nullptr;FName Bone1,Bone2;
 Owner->GetConstrainedComponents(Body1,Bone1,Body2,Bone2);
 if((R.HadBody1 && !R.Body1.IsValid()) || (R.HadBody2 && !R.Body2.IsValid()) || R.Body1.Get()!=Body1 || R.Body2.Get()!=Body2 || R.Bone1!=Bone1 || R.Bone2!=Bone2 || R.Broken!=Owner->IsBroken() || (Owner->ConstraintInstance.ConstraintHandle.IsValid() && !ConstraintGeneration(Owner)) || (R.Generation && R.Generation->bDeleted) || ConstraintGeneration(Owner)!=R.Generation)return nullptr;
 return Owner;
}
UPhysicsConstraintComponent* ValidateConstraintRecord(const FDasConstraintTransportRecord& R,das::Context* C){
 auto* Owner=TryValidateConstraintRecord(R,C);if(!Owner)C->throw_error("constraint owner/context/generation expired; acquire GetConstraint again");return Owner;
}
const FDasConstraintTransportRecord& ConstraintRecord(const FDasConstraintHandle& H,das::Context* C){
 if(C!=ActiveContext || !ActiveContext)C->throw_error("constraint requires active context");
 const auto* Value=ActiveContext->StructValues.Find(H.Token);
 if(!Value || Value->GetScriptStruct()!=FDasConstraintTransportRecord::StaticStruct())C->throw_error("constraint handle is null, expired, or belongs to another context");
 return Value->Get<FDasConstraintTransportRecord>();
}
FDasConstraintHandle StoreConstraintRecord(const FDasConstraintTransportRecord& R,das::Context* C,das::LineInfoArg* At){
 ValidateConstraintRecord(R,C);
 if(At && ActiveContext->StructValues.Num()>=ActiveContext->StructNextCollection)CollectStructValues(*ActiveContext,At);
 if(ActiveContext->StructValues.Num()>=4096)C->throw_error("struct value table limit exceeded");
 FInstancedStruct Value;Value.InitializeAs(FDasConstraintTransportRecord::StaticStruct(),reinterpret_cast<const uint8*>(&R));
 const auto Token=NextStructValueToken++;ActiveContext->StructValues.Add(Token,MoveTemp(Value));return {Token};
}
bool ConstraintValid(const FDasConstraintHandle& H,das::Context* C){
 if(!ActiveContext || C!=ActiveContext || !IsInGameThread())return false;
 const auto* V=ActiveContext->StructValues.Find(H.Token);
 return V && V->GetScriptStruct()==FDasConstraintTransportRecord::StaticStruct() && TryValidateConstraintRecord(V->Get<FDasConstraintTransportRecord>(),C);
}
void ArgumentConstraint(const FDasCallHandle& H,const char* Path,const FDasConstraintHandle& V,das::Context* C){
 auto& Call=ResolveCall(H,C);auto Field=ResolveCallField(Call,Path,false,C);
 auto* P=CastField<FStructProperty>(Field.Property);
 if(!P || P->Struct!=FConstraintInstanceAccessor::StaticStruct())C->throw_error("expected native constraint accessor argument");
 const auto& R=ConstraintRecord(V,C);auto* Owner=ValidateConstraintRecord(R,C);
 FInstancedStruct Record;Record.InitializeAs(FDasConstraintTransportRecord::StaticStruct(),reinterpret_cast<const uint8*>(&R));
 Call.ConstraintRecords.Add(Field.Path,MoveTemp(Record));
 *static_cast<FConstraintInstanceAccessor*>(Field.Memory)=Owner->GetConstraint();Call.Assigned.Add(Field.Path);
}
void ValidateConstraintCallInputs(FMarshalledCall& Call,das::Context* C){
 for(const auto& Pair:Call.ConstraintRecords){
  const auto& R=Pair.Value.Get<FDasConstraintTransportRecord>();auto* Owner=ValidateConstraintRecord(R,C);
  const auto Field=ResolveCallField(Call,TCHAR_TO_UTF8(*Pair.Key),false,C);
  if(static_cast<const FConstraintInstanceAccessor*>(Field.Memory)->Get()!=&Owner->ConstraintInstance)C->throw_error("constraint accessor provenance changed before invocation");
 }
}
void CaptureConstraintCallOutput(FMarshalledCall& Call,das::Context* C){
 if(Call.Function->GetOuterUClass()!=UPhysicsConstraintComponent::StaticClass() || Call.Function->GetFName()!=TEXT("GetConstraint"))return;
 FDasConstraintTransportRecord Record;
  Record.Owner=Cast<UPhysicsConstraintComponent>(Call.Target.Get());auto* Owner=ConstraintOwner(Record,C);
  UPrimitiveComponent *Body1=nullptr,*Body2=nullptr;Owner->GetConstrainedComponents(Body1,Record.Bone1,Body2,Record.Bone2);
  Record.Body1=Body1;Record.Body2=Body2;Record.HadBody1=Body1!=nullptr;Record.HadBody2=Body2!=nullptr;Record.Broken=Owner->IsBroken();Record.Generation=ConstraintGeneration(Owner);

  ValidateConstraintRecord(Record,C);
  FInstancedStruct Value;Value.InitializeAs(FDasConstraintTransportRecord::StaticStruct(),reinterpret_cast<const uint8*>(&Record));
  Call.ConstraintRecords.Add(TEXT("returnvalue"),MoveTemp(Value));
}
FDasConstraintHandle ResultConstraint(const FDasCallHandle& H,const char* Path,das::Context* C,das::LineInfoArg* At){
 auto& Call=ResolveCall(H,C);auto Field=ResolveCallField(Call,Path,true,C);
 auto* P=CastField<FStructProperty>(Field.Property);
 if(!P || P->Struct!=FConstraintInstanceAccessor::StaticStruct())C->throw_error("expected native constraint accessor result");
 const auto* Stored=Call.ConstraintRecords.Find(Field.Path);
 if(!Stored)C->throw_error("constraint result requires component-owned accessor provenance");
 const auto& Record=Stored->Get<FDasConstraintTransportRecord>();
 auto* Owner=ValidateConstraintRecord(Record,C);
 if(static_cast<const FConstraintInstanceAccessor*>(Field.Memory)->Get()!=&Owner->ConstraintInstance)C->throw_error("constraint result owner changed");
 return StoreConstraintRecord(Record,C,At);
}
