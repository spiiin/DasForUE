das::TypeDeclPtr DSLNativeValueType(das::Program* Program,das::Module* Module,const FDasDeclaredType& Type,const das::LineInfo& At){
 if(Type.Kind==TEXT("object")){const TCHAR* Source=nullptr;for(const auto& V:DasNativeGameplayViews)if(Type.TypePath==V.Class){Source=V.Source;break;}if(!Source)DSLFail(TEXT("Native object type is not in generated registry: ")+Type.TypePath);auto Types=Program->thisModuleGroup->findStructure(DSLUtf8(Source),Module);if(Types.size()!=1)DSLFail(TEXT("Import ue_native_gameplay for native view: ")+FString(Source));auto* T=new das::TypeDecl(das::Type::tStructure,At);T->structType=Types[0];return T;}
 if(Type.Kind==TEXT("struct")){const auto& Path=Type.TypePath;const char* Name=Path==TEXT("/Script/CoreUObject.Vector")?"UEVector":Path==TEXT("/Script/CoreUObject.Vector2D")?"UEVector2D":Path==TEXT("/Script/CoreUObject.Rotator")?"UERotator":Path==TEXT("/Script/CoreUObject.Transform")?"UETransform":nullptr;if(!Name)DSLFail(TEXT("Unsupported generated native struct: ")+Path);return DSLHandleType(*Program->thisModuleGroup,Module,Name);}
 return DSLPropertyType(Type.Kind,*Program->thisModuleGroup,Module,At);
}
FDasDeclaredProperty DSLNativeReferenceProperty(const das::TypeDeclPtr& Receiver,const das::string& Name){
 if(Name.empty() || Name.length()>=NAME_SIZE)DSLFail(TEXT("Invalid native reference field name"));const FString Path=DSLTypedReferencePath(Receiver),Field=DSLText(Name);const auto* Row=DasNativeGameplayField(Path,Field);auto* Class=FindObject<UClass>(nullptr,*Path);auto* P=Row && Class?Class->FindPropertyByName(*Field):nullptr;
 FString Shape;if(P)AppendCallShape(P,Shape);if(!P || P->GetName()!=Field || Shape!=Row->Signature || P->GetOwnerClass()->GetPathName()!=Row->Declaring || !P->HasAnyPropertyFlags(CPF_BlueprintVisible) || P->HasAnyPropertyFlags(CPF_Deprecated|CPF_EditorOnly) || P->HasSetterOrGetter() || P->HasAnyPropertyFlags(CPF_BlueprintReadOnly)!=Row->ReadOnly)DSLFail(TEXT("Native field not generated or stale: ")+Path+TEXT(".")+Field);
 FDasDeclaredProperty D;D.Name=Field;D.Type.Kind=Row->Kind;D.Type.TypePath=Row->TypePath;D.bReadOnly=Row->ReadOnly;return D;
}
FDasDeclaredLiteral DSLNativeDefault(const FString& Class,const FString& Method,const FString& Name,const FString& Kind){
 FDasDeclaredLiteral D;for(const auto& Row:DasNativeGameplayDefaults)if(Class==Row.Class && Method==Row.Method && Name==Row.Name){const FString Text=Row.Text;
  if(Kind==TEXT("bool") && (Text==TEXT("true") || Text==TEXT("false"))){D.Kind=TEXT("bool");D.Boolean=Text==TEXT("true");}
  else if(Kind==TEXT("string")){D.Kind=TEXT("string");D.String=Text;}
  else if(Kind==TEXT("int")){int32 Value=0;if(LexTryParseString(Value,*Text)){D.Kind=TEXT("number");D.Number=Value;}}
  else if(Kind==TEXT("float") || Kind==TEXT("double")){double Value=0;if(LexTryParseString(Value,*Text) && FMath::IsFinite(Value)){D.Kind=TEXT("number");D.Number=Value;}}break;
 }return D;
}
void DSLNativeMethodDescriptor(das::Program* Program,das::Module* Module,const das::TypeDeclPtr& Receiver,const das::string& Name,const das::LineInfo& At,FDasDeclaredClass& Declaration,FDasDeclaredMethod& Method,FDSLMethodSourceSignature& Descriptor){
 if(Name.empty() || Name.length()>=NAME_SIZE)DSLFail(TEXT("Invalid native method name"));Declaration.Id=DSLTypedReferencePath(Receiver);Declaration.Name=DSLText(Receiver->structType->name);Declaration.NativeParent=Declaration.Id;
 const auto* Row=DasNativeGameplayMethod(Declaration.Id,DSLText(Name));auto* Class=FindObject<UClass>(nullptr,*Declaration.Id);auto* Function=Row && Class?Class->FindFunctionByName(*DSLText(Name)):nullptr;if(!Function || CallSignatureKey(Function)!=Row->Signature)DSLFail(TEXT("Native method not generated or stale: ")+Declaration.Id+TEXT(".")+DSLText(Name));
 Method.Name=DSLText(Name);Method.Id=Declaration.Id+TEXT(".")+Method.Name;Method.ReturnType.Kind=TEXT("void");Method.bConst=Function->HasAnyFunctionFlags(FUNC_Const);Descriptor.Scope={FString(),FString(),FString(),nullptr,nullptr};
 for(TFieldIterator<FProperty> It(Function);It;++It)if(It->HasAnyPropertyFlags(CPF_Parm)){FDasDeclaredType Type;Type.Kind=DasNativeGameplayKind(*It,Type.TypePath);if(Type.Kind.IsEmpty())DSLFail(TEXT("Unsupported native method parameter"));const auto SourceType=DSLNativeValueType(Program,Module,Type,At);const auto Source=DSLMethodSourceName(SourceType);
  if(It->HasAnyPropertyFlags(CPF_ReturnParm)){Method.ReturnType=Type;Descriptor.Result=Source;continue;}FDasDeclaredParameter Param;Param.Id=Method.Id+TEXT(".")+It->GetName();Param.Name=It->GetName();Param.Type=Type;const bool Out=It->HasAnyPropertyFlags(CPF_OutParm) && !It->HasAnyPropertyFlags(CPF_ConstParm);Param.Direction=Out?(It->HasAnyPropertyFlags(CPF_ReferenceParm)?TEXT("inout"):TEXT("out")):TEXT("in");if(!Out)Param.Default=DSLNativeDefault(Declaration.Id,Method.Name,Param.Name,Type.Kind);Method.Parameters.Add(Param);Descriptor.Parameters.push_back(Source);
 }
 // Defaults must form a trailing suffix in daScript. Keep unsupported defaults
 // explicit rather than reinterpret C++ expressions or silently invent values.
 bool Trailing=true;for(int32 I=Method.Parameters.Num()-1;I>=0;--I){if(Method.Parameters[I].Default.Kind.IsEmpty())Trailing=false;if(!Trailing)Method.Parameters[I].Default={};}
 Declaration.Methods.Add(Method);
}
