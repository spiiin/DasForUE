// Context capabilities are immutable values. Mutations return a new token, so
// copying a standalone delegate does not turn two values into an alias.
uint64 NextDelegateValue=1;
FDelegateValue ResolveDelegateValue(uint64 Token,const char* Wire,bool Multi,das::Context* C) {
 RequireObjectContext(C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("delegate values cannot be modified/executed in bind_events");
 const FString Key=UTF8_TO_TCHAR(Wire);const FString CacheKey=(Multi?TEXT("multicast:"):TEXT("single:"))+Key;auto* Signature=ActiveContext->DelegateSignatures.Find(CacheKey);
 if(!Signature){if(ActiveContext->DelegateSignatures.Num()>=256)C->throw_error("delegate signature limit reached");FDasAuthoringModule Model;FString Error;if(!FDasAuthoringModule::FromJson(Key,Model,Error) || Model.Classes.Num()!=1 || Model.Classes[0].Methods.Num()!=1)C->throw_error("invalid delegate signature contract");auto* F=MakeDasDelegateSignature(Model.Classes[0].Methods[0],Multi,GetTransientPackage(),NAME_None,Error);if(!F)C->throw_error(TCHAR_TO_UTF8(*Error));ActiveContext->DelegateSignatures.Add(CacheKey,TStrongObjectPtr<UFunction>(F));Signature=ActiveContext->DelegateSignatures.Find(CacheKey);}
 if(!Token){FDelegateValue V;V.Wire=Key;V.Signature=*Signature;V.Multicast=Multi;return V;}
 auto* Found=ActiveContext->DelegateValues.Find(Token);if(!Found || Found->Wire!=Key || Found->Multicast!=Multi)C->throw_error("delegate token/signature belongs to another type or context");return *Found;
}
uint64 StoreDelegateValue(FDelegateValue V,das::Context* C){if(ActiveContext->DelegateValues.Num()>=4096)C->throw_error("delegate value limit reached; release unused values");const uint64 Token=NextDelegateValue++;ActiveContext->DelegateValues.Add(Token,MoveTemp(V));return Token;}
bool LiveDelegateObject(UObject* O){return IsValid(O) && !(Cast<AActor>(O) && CastChecked<AActor>(O)->IsActorBeingDestroyed()) && !(Cast<UActorComponent>(O) && CastChecked<UActorComponent>(O)->IsBeingDestroyed());}
bool DelegateFrameMatches(UFunction* A,UFunction* B){if(!A || !B || A->ParmsSize!=B->ParmsSize || !A->IsSignatureCompatibleWith(B))return false;TFieldIterator<FProperty> Left(A),Right(B);for(;Left && Left->HasAnyPropertyFlags(CPF_Parm);++Left,++Right)if(!Right || Left->GetOffset_ForInternal()!=Right->GetOffset_ForInternal())return false;return true;}
UFunction* DelegateHandler(UObject* O,FName Name,UFunction* Signature,das::Context* C){
 auto* F=LiveDelegateObject(O)?O->FindFunction(Name):nullptr;
 if(!F || F->HasAnyFunctionFlags(FUNC_Static|FUNC_Net|FUNC_Delegate|FUNC_MulticastDelegate) || !DelegateFrameMatches(Signature,F))C->throw_error(TCHAR_TO_UTF8(*FString::Printf(TEXT("delegate requires a live instance UFunction with exactly matching parameter/return types: %s (sizes %d/%d)"),*Name.ToString(),Signature->ParmsSize,F?F->ParmsSize:-1)));return F;
}
FMulticastDelegateProperty* DelegateEventProperty(const FDelegateValue& V,bool Broadcast,das::Context* C){
 auto* O=V.Source.Get();auto* P=LiveDelegateObject(O)?CastField<FMulticastDelegateProperty>(O->GetClass()->FindPropertyByName(V.Property)):nullptr;
 if(!P || !P->SignatureFunction || !DelegateFrameMatches(V.Signature.Get(),P->SignatureFunction) || !P->HasAnyPropertyFlags(Broadcast?CPF_BlueprintCallable:CPF_BlueprintAssignable))C->throw_error("event property expired, changed signature or does not permit this operation");return P;
}
uint64 DelegateBind(uint64 Token,const char* Wire,bool Multi,const FDasObjectHandle& Target,const char* Name,das::Context* C){
 auto V=ResolveDelegateValue(Token,Wire,Multi,C);auto* O=ResolveObject(Target,C);if(!Name || !*Name || FCStringAnsi::Strlen(Name)>=NAME_SIZE)C->throw_error("invalid delegate handler name");const FName N(UTF8_TO_TCHAR(Name));DelegateHandler(O,N,V.Signature.Get(),C);FScriptDelegate D;D.BindUFunction(O,N);
 if(!V.Property.IsNone()){auto* P=DelegateEventProperty(V,false,C);P->AddDelegate(D,V.Source.Get(),P->ContainerPtrToValuePtr<void>(V.Source.Get()));return Token;}
 if(Multi){if(!Token)C->throw_error("event value was not initialized");V.Listeners.AddUnique(D);ActiveContext->DelegateValues[Token]=MoveTemp(V);return Token;}V.Single=D;return StoreDelegateValue(MoveTemp(V),C);
}
uint64 DelegateRemove(uint64 Token,const char* Wire,bool Multi,const FDasObjectHandle& Target,const char* Name,das::Context* C){
 auto V=ResolveDelegateValue(Token,Wire,Multi,C);auto* O=ResolveObject(Target,C);FScriptDelegate D;D.BindUFunction(O,FName(UTF8_TO_TCHAR(Name)));
 if(!V.Property.IsNone()){auto* P=DelegateEventProperty(V,false,C);P->RemoveDelegate(D,V.Source.Get(),P->ContainerPtrToValuePtr<void>(V.Source.Get()));return Token;}
 V.Listeners.Remove(D);if(!Token)C->throw_error("event value was not initialized");ActiveContext->DelegateValues[Token]=MoveTemp(V);return Token;
}
uint64 DelegateClear(uint64 Token,const char* Wire,bool Multi,das::Context* C){auto V=ResolveDelegateValue(Token,Wire,Multi,C);if(!V.Property.IsNone()){auto* P=DelegateEventProperty(V,false,C);P->SetMulticastDelegate(P->ContainerPtrToValuePtr<void>(V.Source.Get()),FMulticastScriptDelegate());return Token;}if(Multi && Token){V.Listeners.Reset();ActiveContext->DelegateValues[Token]=MoveTemp(V);return Token;}return 0;}
bool DelegateIsBound(uint64 Token,const char* Wire,bool Multi,das::Context* C){auto V=ResolveDelegateValue(Token,Wire,Multi,C);if(!V.Property.IsNone()){auto* P=DelegateEventProperty(V,false,C);const auto* D=P->GetMulticastDelegate(P->ContainerPtrToValuePtr<void>(V.Source.Get()));return D && D->IsBound();}if(Multi)return V.Listeners.ContainsByPredicate([](const auto& D){return D.IsBound() && LiveDelegateObject(const_cast<UObject*>(D.GetUObject()));});return V.Single.IsBound() && LiveDelegateObject(const_cast<UObject*>(V.Single.GetUObject()));}
uint64 DelegateCreate(const char* Wire,das::Context* C){return StoreDelegateValue(ResolveDelegateValue(0,Wire,true,C),C);}
uint64 DelegateEventView(const char* Wire,const FDasObjectHandle& Source,const char* Name,das::Context* C){auto V=ResolveDelegateValue(0,Wire,true,C);V.Source=ResolveObject(Source,C);V.Property=FName(UTF8_TO_TCHAR(Name));DelegateEventProperty(V,false,C);for(const auto& Pair:ActiveContext->DelegateValues)if(Pair.Value.Wire==V.Wire && Pair.Value.Source==V.Source && Pair.Value.Property==V.Property)return Pair.Key;return StoreDelegateValue(MoveTemp(V),C);}
FDasCallHandle BeginDelegateCall(uint64 Token,const char* Wire,bool Multi,bool Optional,das::Context* C){
 auto V=ResolveDelegateValue(Token,Wire,Multi,C);if(!Multi && (!V.Single.IsBound() || !LiveDelegateObject(const_cast<UObject*>(V.Single.GetUObject()))) && !Optional)C->throw_error("Execute requires a bound delegate");
 if(ActiveContext->Calls.Num()>=256)C->throw_error("call frame limit reached");auto Call=MakeShared<FMarshalledCall>();Call->Function=V.Signature;Call->Signature=DescribeCall(V.Signature.Get(),C);Call->Frame=MakeUnique<FStructOnScope>(V.Signature.Get());Call->Target=ActiveContext->Self;Call->bAuthored=true;
 // Snapshot once at invocation, matching Unreal multicast behavior when handlers
 // bind/unbind during Broadcast. Bindings contain weak UObject references.
 Call->DelegateInvoke=[V,Optional,C](void* Params)mutable{
   if(!V.Property.IsNone()){auto* P=DelegateEventProperty(V,true,C);const auto* Bound=P->GetMulticastDelegate(P->ContainerPtrToValuePtr<void>(V.Source.Get()));if(Bound){auto Snapshot=*Bound;for(auto* O:Snapshot.GetAllObjects())if(!LiveDelegateObject(O))Snapshot.RemoveAll(O);Snapshot.ProcessDelegate<UObject>(Params);}return;}
   if(V.Multicast){auto Snapshot=V.Listeners;for(const auto& D:Snapshot){auto* O=const_cast<UObject*>(D.GetUObject());if(!D.IsBound() || !LiveDelegateObject(O))continue;DelegateHandler(O,D.GetFunctionName(),V.Signature.Get(),C);TStrongObjectPtr<UObject> Keep(O);D.ProcessDelegate<UObject>(Params);if(AuthoredDispatchFailure && !AuthoredDispatchFailure->IsEmpty())break;}}
   else {auto* O=const_cast<UObject*>(V.Single.GetUObject());if(!V.Single.IsBound() || !LiveDelegateObject(O)){if(Optional)return;C->throw_error("delegate target expired");}DelegateHandler(O,V.Single.GetFunctionName(),V.Signature.Get(),C);TStrongObjectPtr<UObject> Keep(O);V.Single.ProcessDelegate<UObject>(Params);}
 };
 const uint64 Id=NextCallHandle++;ActiveContext->Calls.Add(Id,MoveTemp(Call));return {Id};
}
void ReleaseDelegateValue(uint64 Token,das::Context* C){RequireObjectContext(C);ActiveContext->DelegateValues.Remove(Token);}

FDelegateProperty* SingleDelegateProperty(const FDasObjectHandle& Source,const char* Name,UFunction* Signature,bool Write,das::Context* C){auto* O=ResolveObject(Source,C);auto* P=CastField<FDelegateProperty>(O->GetClass()->FindPropertyByName(FName(UTF8_TO_TCHAR(Name))));if(!P || P->ArrayDim!=1 || !P->SignatureFunction || !Signature->IsSignatureCompatibleWith(P->SignatureFunction) || !P->HasAnyPropertyFlags(CPF_BlueprintVisible) || (Write && P->HasAnyPropertyFlags(CPF_BlueprintReadOnly)))C->throw_error("single delegate field missing, changed signature or is readonly");return P;}
uint64 DelegateGetField(const char* Wire,const FDasObjectHandle& Source,const char* Name,das::Context* C){auto V=ResolveDelegateValue(0,Wire,false,C);auto* P=SingleDelegateProperty(Source,Name,V.Signature.Get(),false,C);V.Single=P->GetPropertyValue_InContainer(ResolveObject(Source,C));return StoreDelegateValue(MoveTemp(V),C);}
void DelegateSetField(const char* Wire,const FDasObjectHandle& Source,const char* Name,uint64 Token,das::Context* C){auto V=ResolveDelegateValue(Token,Wire,false,C);auto* P=SingleDelegateProperty(Source,Name,V.Signature.Get(),true,C);if(V.Single.IsBound())DelegateHandler(const_cast<UObject*>(V.Single.GetUObject()),V.Single.GetFunctionName(),V.Signature.Get(),C);P->SetPropertyValue_InContainer(ResolveObject(Source,C),V.Single);}

void MethodArgdelegate(const FDasCallHandle& H,const char* Path,const char* Wire,uint64 Token,das::Context* C){auto& Call=ResolveCall(H,C);auto Field=ResolveCallField(Call,Path,false,C);auto* P=CastField<FDelegateProperty>(Field.Property);auto V=ResolveDelegateValue(Token,Wire,false,C);if(!P || !P->SignatureFunction || !V.Signature->IsSignatureCompatibleWith(P->SignatureFunction))C->throw_error("delegate method argument signature mismatch");if(V.Single.IsBound())DelegateHandler(const_cast<UObject*>(V.Single.GetUObject()),V.Single.GetFunctionName(),V.Signature.Get(),C);P->SetPropertyValue(Field.Memory,V.Single);Call.Assigned.Add(Field.Path);}
uint64 MethodResultdelegate(const FDasCallHandle& H,const char* Path,const char* Wire,das::Context* C){auto& Call=ResolveCall(H,C);auto Field=ResolveCallField(Call,Path,true,C);auto* P=CastField<FDelegateProperty>(Field.Property);auto V=ResolveDelegateValue(0,Wire,false,C);if(!P || !P->SignatureFunction || !V.Signature->IsSignatureCompatibleWith(P->SignatureFunction))C->throw_error("delegate method result signature mismatch");V.Single=P->GetPropertyValue(Field.Memory);return StoreDelegateValue(MoveTemp(V),C);}

void ArgumentNativeDelegate(const FDasCallHandle& H,const char* Path,const char* Signature,const FDasObjectHandle& Target,const char* Name,das::Context* C){
 auto& Call=ResolveCall(H,C);auto Field=ResolveCallField(Call,Path,false,C);auto* P=CastField<FDelegateProperty>(Field.Property);
 if(!P || !P->SignatureFunction || P->SignatureFunction->GetPathName()!=UTF8_TO_TCHAR(Signature))C->throw_error("native delegate argument signature identity changed");
 FScriptDelegate D;if(Target.Token){auto* O=ResolveObject(Target,C);if(!Name || !*Name || FCStringAnsi::Strlen(Name)>=NAME_SIZE)C->throw_error("invalid delegate handler name");const FName N(UTF8_TO_TCHAR(Name));DelegateHandler(O,N,P->SignatureFunction,C);D.BindUFunction(O,N);Call.ObjectPins.Add(Field.Path,TStrongObjectPtr<UObject>(O));}else if(Name && *Name)C->throw_error("unbound delegate cannot have a handler name");
 P->SetPropertyValue(Field.Memory,D);Call.Assigned.Add(Field.Path);
}
