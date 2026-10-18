bool FDasRuntimeModule::ExportNativeGameplayCoverage(const FString& OutputPath,const FString& ProfilePath,FString& Error){
#if !WITH_EDITOR
 Error=TEXT("API coverage requires editor metadata");return false;
#else
 if(!IsInGameThread()){Error=TEXT("API coverage requires game thread");return false;}
 FString Text;TSharedPtr<FJsonObject> Profile;
 if(!FFileHelper::LoadFileToString(Text,*ProfilePath) || !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text),Profile) || !Profile.IsValid()){Error=TEXT("Cannot read coverage profile");return false;}
 double Version=0;const TArray<TSharedPtr<FJsonValue>> *Modules=nullptr,*Groups=nullptr;
 if(!Profile->TryGetNumberField(TEXT("schema_version"),Version) || Version!=1 || !Profile->TryGetArrayField(TEXT("modules"),Modules) || Modules->IsEmpty() || !Profile->TryGetArrayField(TEXT("subsystems"),Groups)){Error=TEXT("Invalid coverage profile");return false;}
 TSet<FString> Packages;
 for(const auto& Value:*Modules){FString Module;if(!Value->TryGetString(Module) || Module.IsEmpty() || !FModuleManager::Get().LoadModule(*Module)){Error=TEXT("Cannot load coverage module: ")+Module;return false;}Packages.Add(TEXT("/Script/")+Module);}
 struct FGroup {FString Name;TSet<FString> Packages;TArray<UClass*> Roots;};TArray<FGroup> Rules;
 for(const auto& Value:*Groups){auto O=Value->AsObject();FGroup G;const TArray<TSharedPtr<FJsonValue>> *M=nullptr,*Roots=nullptr;
  if(!O.IsValid() || !O->TryGetStringField(TEXT("id"),G.Name) || !O->TryGetArrayField(TEXT("modules"),M) || !O->TryGetArrayField(TEXT("roots"),Roots)){Error=TEXT("Invalid subsystem rule");return false;}
  for(const auto& P:*M)G.Packages.Add(TEXT("/Script/")+P->AsString());
  for(const auto& P:*Roots){auto* C=FindObject<UClass>(nullptr,*P->AsString());if(!C){Error=TEXT("Unknown subsystem root: ")+P->AsString();return false;}G.Roots.Add(C);}Rules.Add(MoveTemp(G));
 }
 auto Strings=[](const TArray<FString>& Values){TArray<TSharedPtr<FJsonValue>> A;for(const auto& S:Values)A.Add(MakeShared<FJsonValueString>(S));return A;};
 auto Assessment=[&](const FDasNativeAssessment& R,auto D){D->SetBoolField(TEXT("eligible"),R.Primary.IsEmpty());D->SetStringField(TEXT("primary_reason"),R.Primary);D->SetArrayField(TEXT("reasons"),Strings(R.Reasons));D->SetArrayField(TEXT("type_issues"),R.TypeIssues);if(!R.MarshallingError.IsEmpty())D->SetStringField(TEXT("marshalling_error"),R.MarshallingError);};
 TArray<UClass*> Classes;for(TObjectIterator<UClass> I;I;++I)if(Packages.Contains(I->GetOutermost()->GetName()) && I->HasAnyClassFlags(CLASS_Native))Classes.Add(*I);
 Classes.Sort([](const UClass& A,const UClass& B){return A.GetPathName()<B.GetPathName();});
 TMap<FString,TSharedPtr<FJsonObject>> Definitions;TArray<TSharedPtr<FJsonValue>> Rows,Excluded,LibraryRequests;
 for(auto* Class:Classes){
  auto D=MakeShared<FJsonObject>();const FString Path=Class->GetPathName();D->SetStringField(TEXT("path"),Path);D->SetStringField(TEXT("module"),Class->GetOutermost()->GetName().Mid(8));
  if(Class->HasAnyClassFlags(CLASS_Interface|CLASS_Deprecated|CLASS_NewerVersionExists)){D->SetStringField(TEXT("reason"),Class->HasAnyClassFlags(CLASS_Interface)?TEXT("interface_route"):TEXT("deprecated_or_replaced_class"));Excluded.Add(MakeShared<FJsonValueObject>(D));continue;}
  FString Group=TEXT("other");for(const auto& G:Rules){bool Match=G.Packages.Contains(Class->GetOutermost()->GetName());for(auto* Base:G.Roots)Match|=Class->IsChildOf(Base);if(Match){Group=G.Name;break;}}
  const bool Library=Class->IsChildOf(UBlueprintFunctionLibrary::StaticClass());D->SetStringField(TEXT("subsystem"),Group);D->SetBoolField(TEXT("function_library"),Library);D->SetStringField(TEXT("super"),Class->GetSuperClass()?Class->GetSuperClass()->GetPathName():TEXT(""));
  TArray<FString> Fields,Methods;TSet<FName> Names;
  for(TFieldIterator<FProperty> I(Class);I;++I){auto* P=*I;const FString Id=TEXT("field:")+P->GetOwnerStruct()->GetPathName()+TEXT(".")+P->GetName();Fields.Add(Id);
   if(Definitions.Contains(Id))continue;auto V=MakeShared<FJsonObject>();V->SetStringField(TEXT("id"),Id);V->SetStringField(TEXT("domain"),TEXT("field"));V->SetStringField(TEXT("name"),P->GetName());V->SetStringField(TEXT("declaring_class"),P->GetOwnerStruct()->GetPathName());const auto A=DasAssessNativeField(P,true);FString Shape;if(A.Primary.IsEmpty())AppendCallShape(P,Shape);V->SetStringField(TEXT("signature"),Shape);Assessment(A,V);Definitions.Add(Id,V);
  }
  for(TFieldIterator<UFunction> I(Class);I;++I){auto* F=*I;if(Names.Contains(F->GetFName()))continue;Names.Add(F->GetFName());const FString Id=TEXT("method:")+F->GetOuterUClass()->GetPathName()+TEXT(".")+F->GetName();Methods.Add(Id);
   if(!Definitions.Contains(Id)){auto V=MakeShared<FJsonObject>();V->SetStringField(TEXT("id"),Id);V->SetStringField(TEXT("domain"),TEXT("method"));V->SetStringField(TEXT("name"),F->GetName());V->SetStringField(TEXT("declaring_class"),F->GetOuterUClass()->GetPathName());const auto A=DasAssessNativeMethod(F,true);V->SetStringField(TEXT("signature"),A.Primary.IsEmpty()?CallSignatureKey(F):TEXT(""));Assessment(A,V);Definitions.Add(Id,V);}
   if(Library && F->HasAllFunctionFlags(FUNC_Static|FUNC_Native|FUNC_Public|FUNC_BlueprintCallable)){
    auto R=MakeShared<FJsonObject>();R->SetStringField(TEXT("class"),Path);R->SetStringField(TEXT("function"),F->GetName());R->SetStringField(TEXT("alias"),TEXT("audit_")+FMD5::HashAnsiString(*(Path+TEXT(".")+F->GetName())));R->SetBoolField(TEXT("required"),false);LibraryRequests.Add(MakeShared<FJsonValueObject>(R));
   }
  }
  Fields.Sort();Methods.Sort();D->SetArrayField(TEXT("fields"),Strings(Fields));D->SetArrayField(TEXT("methods"),Strings(Methods));Rows.Add(MakeShared<FJsonValueObject>(D));
 }
 // Exercise the existing static/library exporter rather than approximating its policy.
 const FString SelectionPath=OutputPath+TEXT(".library-selection.json"),LibraryPath=OutputPath+TEXT(".libraries.json");auto Selection=MakeShared<FJsonObject>();Selection->SetArrayField(TEXT("functions"),LibraryRequests);FString Json;
 FJsonSerializer::Serialize(Selection,TJsonWriterFactory<>::Create(&Json));if(!FFileHelper::SaveStringToFile(Json,*SelectionPath,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) || !ExportBindings(SelectionPath,LibraryPath,Error))return false;
 auto Document=MakeShared<FJsonObject>();Document->SetNumberField(TEXT("schema_version"),1);Document->SetStringField(TEXT("engine_version"),FEngineVersion::Current().ToString());Document->SetStringField(TEXT("scope"),TEXT("Loaded reflected native classes in explicitly loaded modules; no C++ headers or non-reflected API inventory"));Document->SetObjectField(TEXT("profile"),Profile);Document->SetArrayField(TEXT("classes"),Rows);Document->SetArrayField(TEXT("excluded_classes"),Excluded);
 TArray<FString> Keys;Definitions.GetKeys(Keys);Keys.Sort();TArray<TSharedPtr<FJsonValue>> Members;for(const auto& K:Keys)Members.Add(MakeShared<FJsonValueObject>(Definitions[K]));Document->SetArrayField(TEXT("members"),Members);
 Json.Reset();FJsonSerializer::Serialize(Document,TJsonWriterFactory<>::Create(&Json));if(!FFileHelper::SaveStringToFile(Json,*OutputPath,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM)){Error=TEXT("Cannot save coverage inventory");return false;}return true;
#endif
}
