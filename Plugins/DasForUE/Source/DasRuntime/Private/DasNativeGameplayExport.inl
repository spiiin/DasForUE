bool FDasRuntimeModule::ExportNativeGameplay(const FString& OutputPath,FString& Error){
#if !WITH_EDITOR
 Error=TEXT("Native gameplay export requires editor metadata");return false;
#else
 if(!IsInGameThread()){Error=TEXT("Native gameplay export requires game thread");return false;}
 auto Manifest=MakeShared<FJsonObject>();Manifest->SetNumberField(TEXT("schema_version"),5);Manifest->SetStringField(TEXT("engine_version"),FEngineVersion::Current().ToString());

 TMap<FString,UScriptStruct*> Structs;TMap<FString,UEnum*> UsedEnums;
 TFunction<TSharedPtr<FJsonObject>(FProperty*)> TypeJson;
 TypeJson=[&](FProperty* P){FDasDeclaredType T;DasNativeGameplayType(P,T);auto D=MakeShared<FJsonObject>();D->SetStringField(TEXT("kind"),T.Kind);D->SetStringField(TEXT("type_path"),T.TypePath);
  if(T.Kind==TEXT("enum"))UsedEnums.Add(T.TypePath,DasEnumPropertyType(P));
  if(T.Kind==TEXT("map")){auto* M=CastFieldChecked<FMapProperty>(P);D->SetObjectField(TEXT("key"),TypeJson(M->KeyProp));D->SetObjectField(TEXT("value"),TypeJson(M->ValueProp));}
  if(T.Kind==TEXT("set"))D->SetObjectField(TEXT("element"),TypeJson(CastFieldChecked<FSetProperty>(P)->ElementProp));
  if(T.Kind==TEXT("array"))D->SetObjectField(TEXT("element"),TypeJson(CastFieldChecked<FArrayProperty>(P)->Inner));
  if(T.Kind==TEXT("struct") && !IsDasMathStructPath(T.TypePath)){UScriptStruct* S=CastFieldChecked<FStructProperty>(P)->Struct;if(!Structs.Contains(T.TypePath)){Structs.Add(T.TypePath,S);for(TFieldIterator<FProperty> I(S);I;++I)TypeJson(*I);}}
  return D;};
 TArray<TSharedPtr<FJsonValue>> Classes;auto Types=DasNativeGameplayClasses();Types.Sort([](const UClass& A,const UClass& B){return A.GetPathName()<B.GetPathName();});
 for(auto* Class:Types){auto Def=MakeShared<FJsonObject>();Def->SetStringField(TEXT("path"),Class->GetPathName());Def->SetBoolField(TEXT("fixture"),Class==UDasNativeHashProbeComponent::StaticClass());TArray<TSharedPtr<FJsonValue>> Fields,Methods,Skipped;
  auto Reject=[&](const TCHAR* Domain,const FString& Name,const FString& Reason){auto D=MakeShared<FJsonObject>();D->SetStringField(TEXT("domain"),Domain);D->SetStringField(TEXT("name"),Name);D->SetStringField(TEXT("reason"),Reason);Skipped.Add(MakeShared<FJsonValueObject>(D));};
  TArray<FProperty*> Props;for(TFieldIterator<FProperty> It(Class);It;++It)Props.Add(*It);Props.Sort([](const FProperty& A,const FProperty& B){return A.GetName()<B.GetName();});
  for(auto* P:Props){FDasDeclaredType Type;const bool Supported=DasNativeGameplayType(P,Type);FString Path=Type.TypePath,Kind=Type.Kind,Reason;
   if(!P->HasAnyPropertyFlags(CPF_BlueprintVisible) || P->HasAnyPropertyFlags(CPF_Deprecated|CPF_EditorOnly))Reason=TEXT("hidden_editor_or_deprecated");
   else if(P->HasSetterOrGetter() || P->HasMetaData(TEXT("BlueprintGetter")) || P->HasMetaData(TEXT("BlueprintSetter")))Reason=TEXT("requires_native_accessor");
   else if(!Supported)Reason=TEXT("unsupported_property_type");
   if(!Reason.IsEmpty()){Reject(TEXT("field"),P->GetName(),Reason);continue;}
   auto D=TypeJson(P);D->SetStringField(TEXT("name"),P->GetName());D->SetStringField(TEXT("declaring_class"),P->GetOwnerClass()->GetPathName());D->SetBoolField(TEXT("readonly"),P->HasAnyPropertyFlags(CPF_BlueprintReadOnly));FString Shape;AppendCallShape(P,Shape);D->SetStringField(TEXT("signature"),Shape);Fields.Add(MakeShared<FJsonValueObject>(D));
  }
  TArray<UFunction*> Functions;for(TFieldIterator<UFunction> It(Class);It;++It)Functions.Add(*It);Functions.Sort([](const UFunction& A,const UFunction& B){return A.GetName()<B.GetName();});
  TSet<FName> Seen;for(auto* F:Functions){if(Seen.Contains(F->GetFName()))continue;Seen.Add(F->GetFName());FString Reason;
   if(!F->HasAllFunctionFlags(FUNC_BlueprintCallable|FUNC_Public|FUNC_Native) || F->HasAnyFunctionFlags(FUNC_Static|FUNC_Net|FUNC_Delegate|FUNC_MulticastDelegate|FUNC_EditorOnly))Reason=TEXT("not_public_local_native_instance_callable");
   for(const auto* Meta:{TEXT("Latent"),TEXT("CustomThunk"),TEXT("BlueprintProtected"),TEXT("DeprecatedFunction"),TEXT("BlueprintInternalUseOnly"),TEXT("WorldContext"),TEXT("UnsafeDuringActorConstruction")})if(F->HasMetaData(Meta)){Reason=TEXT("requires_adapter_or_nonpublic_policy");break;}
   TArray<TSharedPtr<FJsonValue>> Parameters;
   for(TFieldIterator<FProperty> It(F);It;++It)if(It->HasAnyPropertyFlags(CPF_Parm)){auto* P=*It;FDasDeclaredType Type;if(!DasNativeGameplayType(P,Type)){Reason=TEXT("unsupported_parameter_type");break;}auto D=TypeJson(P);D->SetStringField(TEXT("name"),P->GetName());const bool Return=P->HasAnyPropertyFlags(CPF_ReturnParm),Out=P->HasAnyPropertyFlags(CPF_OutParm) && !P->HasAnyPropertyFlags(CPF_ConstParm);D->SetStringField(TEXT("direction"),Return?TEXT("return"):Out?(P->HasAnyPropertyFlags(CPF_ReferenceParm)?TEXT("inout"):TEXT("out")):TEXT("in"));
    const FName Default(*(TEXT("CPP_Default_")+P->GetName()));if(!Return && !Out && F->HasMetaData(Default))D->SetStringField(TEXT("default_text"),F->GetMetaData(Default));Parameters.Add(MakeShared<FJsonValueObject>(D));}
   if(!Reason.IsEmpty()){Reject(TEXT("method"),F->GetName(),Reason);continue;}
   FUnrealDasContext Context(4096);try{DescribeCall(F,&Context);}catch(const std::exception&){Reject(TEXT("method"),F->GetName(),TEXT("unsupported_marshalling_shape"));continue;}
   auto D=MakeShared<FJsonObject>();D->SetStringField(TEXT("name"),F->GetName());D->SetStringField(TEXT("declaring_class"),F->GetOuterUClass()->GetPathName());D->SetStringField(TEXT("signature"),CallSignatureKey(F));D->SetBoolField(TEXT("const"),F->HasAnyFunctionFlags(FUNC_Const));D->SetArrayField(TEXT("parameters"),Parameters);Methods.Add(MakeShared<FJsonValueObject>(D));
  }
  Def->SetArrayField(TEXT("fields"),Fields);Def->SetArrayField(TEXT("methods"),Methods);Def->SetArrayField(TEXT("skipped"),Skipped);Classes.Add(MakeShared<FJsonValueObject>(Def));
 }
 // Collect only dependencies of accepted members, never partial rejected signatures.
 Structs.Reset();UsedEnums.Reset();for(const auto& C:Classes){const auto D=C->AsObject();auto* Class=FindObject<UClass>(nullptr,*D->GetStringField(TEXT("path")));for(const auto& V:D->GetArrayField(TEXT("fields")))TypeJson(Class->FindPropertyByName(*V->AsObject()->GetStringField(TEXT("name"))));for(const auto& M:D->GetArrayField(TEXT("methods"))){auto* F=Class->FindFunctionByName(*M->AsObject()->GetStringField(TEXT("name")));for(TFieldIterator<FProperty> I(F);I;++I)if(I->HasAnyPropertyFlags(CPF_Parm))TypeJson(*I);}}
 TArray<FString> Paths;UsedEnums.GetKeys(Paths);Paths.Sort();TArray<TSharedPtr<FJsonValue>> Enums;
 for(const auto& Path:Paths){auto* E=FindObject<UEnum>(nullptr,*Path);auto D=MakeShared<FJsonObject>();D->SetStringField(TEXT("path"),Path);TArray<TSharedPtr<FJsonValue>> Entries;for(int32 I=0;I<E->NumEnums();++I){const FString Name=E->GetNameStringByIndex(I);if(Name.EndsWith(TEXT("_MAX")))continue;auto V=MakeShared<FJsonObject>();V->SetStringField(TEXT("name"),Name);V->SetNumberField(TEXT("value"),E->GetValueByIndex(I));Entries.Add(MakeShared<FJsonValueObject>(V));}D->SetArrayField(TEXT("entries"),Entries);Enums.Add(MakeShared<FJsonValueObject>(D));}Manifest->SetArrayField(TEXT("enums"),Enums);

 TArray<FString> StructPaths;Structs.GetKeys(StructPaths);StructPaths.Sort();TArray<TSharedPtr<FJsonValue>> Definitions;
 for(const auto& Path:StructPaths){auto* S=Structs[Path];auto D=MakeShared<FJsonObject>();D->SetStringField(TEXT("path"),Path);TArray<TSharedPtr<FJsonValue>> Members;FString Shape;for(TFieldIterator<FProperty> I(S);I;++I){auto V=TypeJson(*I);V->SetStringField(TEXT("name"),I->GetName());Members.Add(MakeShared<FJsonValueObject>(V));AppendCallShape(*I,Shape);}D->SetStringField(TEXT("signature"),Shape);D->SetArrayField(TEXT("fields"),Members);Definitions.Add(MakeShared<FJsonValueObject>(D));}Manifest->SetArrayField(TEXT("structs"),Definitions);
 Manifest->SetArrayField(TEXT("classes"),Classes);FString Json;FJsonSerializer::Serialize(Manifest,TJsonWriterFactory<>::Create(&Json));if(!FFileHelper::SaveStringToFile(Json,*OutputPath,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM)){Error=TEXT("Cannot save native gameplay manifest");return false;}return true;
#endif
}
