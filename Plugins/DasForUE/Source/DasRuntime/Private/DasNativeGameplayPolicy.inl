// Shared reflection eligibility: audit must not maintain a second generator policy.
#if WITH_EDITOR
namespace {
struct FDasNativeAssessment {
 FString Primary,MarshallingError;
 TArray<FString> Reasons;
 TArray<TSharedPtr<FJsonValue>> TypeIssues;
 void Add(const FString& Reason){Reasons.AddUnique(Reason);}
};
void DasAuditTypeIssues(FProperty* P,const FString& Path,TArray<TSharedPtr<FJsonValue>>& Out,int32 Depth=0){
 if(!P || Out.Num()>=64)return;
 FDasDeclaredType T;if(DasNativeGameplayType(P,T))return;
 const int32 Before=Out.Num();
 if(Depth<16){
  if(auto* A=CastField<FArrayProperty>(P))DasAuditTypeIssues(A->Inner,Path+TEXT("[]"),Out,Depth+1);
  else if(auto* M=CastField<FMapProperty>(P)){DasAuditTypeIssues(M->KeyProp,Path+TEXT("{key}"),Out,Depth+1);DasAuditTypeIssues(M->ValueProp,Path+TEXT("{value}"),Out,Depth+1);}
  else if(auto* S=CastField<FSetProperty>(P))DasAuditTypeIssues(S->ElementProp,Path+TEXT("{element}"),Out,Depth+1);
  else if(auto* StructField=CastField<FStructProperty>(P))for(TFieldIterator<FProperty> I(StructField->Struct);I;++I)DasAuditTypeIssues(*I,Path+TEXT(".")+I->GetName(),Out,Depth+1);
 }
 // If all children work separately, the enclosing shape/context is the restriction.
 if(Before==Out.Num()){
  auto D=MakeShared<FJsonObject>();FString Extended;const FString Cpp=P->GetCPPType(&Extended);
  D->SetStringField(TEXT("path"),Path);D->SetStringField(TEXT("cpp_type"),Cpp+Extended);D->SetStringField(TEXT("property_kind"),P->GetClass()->GetName());D->SetNumberField(TEXT("array_dim"),P->ArrayDim);
  D->SetStringField(TEXT("partial_kind"),T.Kind);
  if(auto* S=CastField<FStructProperty>(P)){D->SetStringField(TEXT("type_path"),S->Struct->GetPathName());FString Issue;if(!ValidateDasValueStruct(S->Struct->GetPathName(),Issue))D->SetStringField(TEXT("detail"),Issue);}
  if(auto* E=DasEnumPropertyType(P))D->SetStringField(TEXT("type_path"),E->GetPathName());
  Out.Add(MakeShared<FJsonValueObject>(D));
 }
}
FDasNativeAssessment DasAssessNativeField(FProperty* P,bool Details=false){
 FDasNativeAssessment R;FDasDeclaredType T;const bool Supported=DasNativeGameplayType(P,T);
 if(!P->HasAnyPropertyFlags(CPF_BlueprintVisible))R.Add(TEXT("not_blueprint_visible"));
 if(P->HasAnyPropertyFlags(CPF_Deprecated))R.Add(TEXT("deprecated"));
 if(P->HasAnyPropertyFlags(CPF_EditorOnly))R.Add(TEXT("editor_only"));
 FDasPropertyAccess Access;FString AccessError;const bool Accessor=!DescribeDasPropertyAccess(P,Access,AccessError);if(Accessor)R.Add(TEXT("invalid_accessor_contract"));
 if(!Supported){R.Add(TEXT("unsupported_property_type"));if(Details)DasAuditTypeIssues(P,P->GetName(),R.TypeIssues);}
 // Deterministic primary reason; accessor semantics use the shared value contract.
 if(!P->HasAnyPropertyFlags(CPF_BlueprintVisible) || P->HasAnyPropertyFlags(CPF_Deprecated|CPF_EditorOnly))R.Primary=TEXT("hidden_editor_or_deprecated");
 else if(Accessor)R.Primary=TEXT("invalid_accessor_contract");else if(!Supported)R.Primary=TEXT("unsupported_property_type");return R;
}
FDasNativeAssessment DasAssessNativeMethod(UFunction* F,bool Details=false){
 FDasNativeAssessment R;
 if(!F->HasAnyFunctionFlags(FUNC_BlueprintCallable))R.Add(TEXT("not_blueprint_callable"));
 if(!F->HasAnyFunctionFlags(FUNC_Public))R.Add(TEXT("not_public"));
 if(!F->HasAnyFunctionFlags(FUNC_Native))R.Add(TEXT("not_native"));
 if(F->HasAnyFunctionFlags(FUNC_Static))R.Add(TEXT("static"));
 if(F->HasAnyFunctionFlags(FUNC_Net))R.Add(TEXT("net"));
 if(F->HasAnyFunctionFlags(FUNC_Delegate|FUNC_MulticastDelegate))R.Add(TEXT("delegate_function"));
 if(F->HasAnyFunctionFlags(FUNC_EditorOnly))R.Add(TEXT("editor_only"));
 if(!F->HasAllFunctionFlags(FUNC_BlueprintCallable|FUNC_Public|FUNC_Native) || F->HasAnyFunctionFlags(FUNC_Static|FUNC_Net|FUNC_Delegate|FUNC_MulticastDelegate|FUNC_EditorOnly))R.Primary=TEXT("not_public_local_native_instance_callable");
 for(const auto* Meta:{TEXT("Latent"),TEXT("CustomThunk"),TEXT("BlueprintProtected"),TEXT("DeprecatedFunction"),TEXT("BlueprintInternalUseOnly"),TEXT("WorldContext")})if(F->HasMetaData(Meta)){R.Add(FString(TEXT("metadata:"))+Meta);R.Primary=TEXT("requires_adapter_or_nonpublic_policy");}
 for(TFieldIterator<FProperty> I(F);I;++I)if(I->HasAnyPropertyFlags(CPF_Parm)){FDasDeclaredType T;if(!DasNativeGameplayType(*I,T)){R.Primary=TEXT("unsupported_parameter_type");R.Add(TEXT("unsupported_parameter_type"));if(Details)DasAuditTypeIssues(*I,I->GetName(),R.TypeIssues);}}
 if(R.Primary.IsEmpty()){
  FUnrealDasContext Context(4096);try{DescribeCall(F,&Context);}catch(const std::exception& E){R.Primary=TEXT("unsupported_marshalling_shape");R.Add(R.Primary);R.MarshallingError=UTF8_TO_TCHAR(E.what());}
 }
 return R;
}
}
#endif
