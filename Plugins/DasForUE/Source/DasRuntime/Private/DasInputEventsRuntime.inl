// Opaque immutable values share the existing bounded/rooted snapshot table.
FDasStructValue StoreInputEvent(const UScriptStruct* Type,const void* Source,das::Context* C,das::LineInfoArg* At){
 if(!ActiveContext || C!=ActiveContext)C->throw_error("input event requires active context");
 FInstancedStruct Copy;FString Error;if(!CaptureDasInputEvent(Type,Source,Copy,Error))C->throw_error(TCHAR_TO_UTF8(*Error));
 if(At && ActiveContext->StructValues.Num()>=ActiveContext->StructNextCollection)CollectStructValues(*ActiveContext,At);
 if(ActiveContext->StructValues.Num()>=4096)C->throw_error("struct value table limit exceeded");
 const uint64 Token=NextStructValueToken++;ActiveContext->StructValues.Add(Token,MoveTemp(Copy));return {Token};
}
template<class V> const FInstancedStruct& ResolveInputEvent(const V& Value,UScriptStruct* Type,das::Context* C){
 const auto& S=ResolveStructValue({Value.Token},C);FString Error;
 if(S.GetScriptStruct()!=Type || !ValidateDasInputEventSnapshot(S,Error))C->throw_error("input event snapshot identity/data mismatch");return S;
}
uint32 FocusEventUser(const FDasFocusEventValue& V,das::Context* C){return ResolveInputEvent(V,FFocusEvent::StaticStruct(),C).Get<FFocusEvent>().GetUser();}
const char* FocusEventCause(const FDasFocusEventValue& V,das::Context* C){
 switch(ResolveInputEvent(V,FFocusEvent::StaticStruct(),C).Get<FFocusEvent>().GetCause()){
 case EFocusCause::Mouse:return "Mouse";case EFocusCause::Navigation:return "Navigation";case EFocusCause::SetDirectly:return "SetDirectly";case EFocusCause::Cleared:return "Cleared";case EFocusCause::OtherWidgetLostFocus:return "OtherWidgetLostFocus";case EFocusCause::WindowActivate:return "WindowActivate";default:C->throw_error("invalid focus cause");}
}
#define INPUT_EVENT_ADAPTER(Name,Native) \
 FDas##Name##Value InputFrom##Name(const FDasStructValue& H,das::Context* C){ResolveInputEvent(H,Native::StaticStruct(),C);return {H.Token};} \
 void Argument##Name(const FDasCallHandle& H,const char* Path,const FDas##Name##Value& Value,das::Context* C){ \
  auto& Call=ResolveCall(H,C);const auto Field=ResolveCallField(Call,Path,false,C);auto* P=CastField<FStructProperty>(Field.Property); \
  if(!P || P->Struct!=Native::StaticStruct() || !DasInputEventFunctionAllowed(Call.Function.Get()))C->throw_error("input event argument requires exact allowed scalar function"); \
  const auto& Snapshot=ResolveInputEvent(Value,P->Struct,C);P->Struct->CopyScriptStruct(Field.Memory,Snapshot.GetMemory());Call.InputEventTokens.Add(Field.Path,Value.Token);Call.Assigned.Add(Field.Path); \
 } \
 FDas##Name##Value Result##Name(const FDasCallHandle& H,const char* Path,das::Context* C,das::LineInfoArg* At){ \
  auto& Call=ResolveCall(H,C);const auto Field=ResolveCallField(Call,Path,true,C);auto* P=CastField<FStructProperty>(Field.Property); \
  if(!P || P->Struct!=Native::StaticStruct() || !DasInputEventFunctionAllowed(Call.Function.Get()))C->throw_error("input event result requires exact allowed scalar function"); \
  return {StoreInputEvent(P->Struct,Field.Memory,C,At).Token}; \
 }
INPUT_EVENT_ADAPTER(InputEvent,FInputEvent)
INPUT_EVENT_ADAPTER(KeyEvent,FKeyEvent)
INPUT_EVENT_ADAPTER(AnalogInputEvent,FAnalogInputEvent)
INPUT_EVENT_ADAPTER(CharacterEvent,FCharacterEvent)
INPUT_EVENT_ADAPTER(PointerEvent,FPointerEvent)
INPUT_EVENT_ADAPTER(NavigationEvent,FNavigationEvent)
#undef INPUT_EVENT_ADAPTER
template<class T> struct TInputEventAnnotation:FMathValueAnnotation<T>{
 TInputEventAnnotation(const char* Name,das::ModuleLibrary& L,const char* Cpp):FMathValueAnnotation<T>(Name,L,Cpp){}
 virtual bool isPod()const override{return true;}
 // Keep tHandle identity in debug/root metadata; never erase it to uint64.
 // Inherit ManagedStructureAnnotation clone: copy from the referenced payload, not its address.
};

void ValidateInputEventCallInputs(FMarshalledCall& Call,das::Context* C){
 for(TFieldIterator<FStructProperty> It(Call.Function.Get());It;++It){auto* P=*It;if(!P->HasAnyPropertyFlags(CPF_Parm) || !DasIsInputEventType(P->Struct))continue;
  if(!DasInputEventFunctionAllowed(Call.Function.Get()))C->throw_error("input event function is outside snapshot allowlist");
  if(P->HasAnyPropertyFlags(CPF_ReturnParm))continue;
  const bool Output=P->HasAnyPropertyFlags(CPF_OutParm) && !P->HasAnyPropertyFlags(CPF_ConstParm);
  if(Output && !P->HasAnyPropertyFlags(CPF_ReferenceParm))continue;
  FString Error;if(!ValidateDasInputEventData(P->Struct,P->ContainerPtrToValuePtr<void>(Call.Frame->GetStructMemory()),Error))C->throw_error(TCHAR_TO_UTF8(*Error));
 }
}
