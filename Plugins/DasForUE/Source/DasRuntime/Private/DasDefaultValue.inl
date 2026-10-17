// The persisted JSON is decoded against FProperty into a compilation-local,
// typed value tree. No VM values or UObject pointers enter this tree.
namespace {
enum class EDasDefaultNode : uint8 { Boolean,Number,String,Text,Name,Enum,Asset,Class,Struct,Array,Set,Map };
struct FDasDefaultValue {
 EDasDefaultNode Kind=EDasDefaultNode::Struct;
 FProperty* Property=nullptr;
 FString Text,Location;
 double Number=0;
 bool Boolean=false;
 TArray<FDasDefaultValue> Children;
};
struct FDasDefaultBudget {
 int32 Nodes=0,Elements=0;
 int64 Bytes=0;
 bool Enter(int32 Depth,int64 Size,FString& Error){
  Bytes+=Size;if(Depth>16 || ++Nodes>8192 || Bytes>16*1024*1024){Error=TEXT("Default exceeds depth/node/byte limit");return false;}return true;
 }
};
bool DefaultJson(const FString& Text,TSharedPtr<FJsonValue>& Out,FString& Error){
 if(Text.Len()>1048576){Error=TEXT("Default exceeds wire limit");return false;}
 // FJsonObject otherwise silently overwrites duplicate object keys.
 struct FScope{bool Object;TSet<FString> Names;};TArray<FScope> Stack;
 auto Scan=TJsonReaderFactory<>::Create(Text);EJsonNotation Token;int32 Tokens=0;
 while(Scan->ReadNext(Token)){
  if(++Tokens>32768 || Stack.Num()>32){Error=TEXT("Default JSON exceeds token/depth limit");return false;}
  if(Token==EJsonNotation::Error){Error=Scan->GetErrorMessage();return false;}
  if(Token==EJsonNotation::ObjectEnd || Token==EJsonNotation::ArrayEnd){if(Stack.IsEmpty())return false;Stack.Pop();continue;}
  if(!Stack.IsEmpty() && Stack.Last().Object){const FString Name=Scan->GetIdentifier();if(Stack.Last().Names.Contains(Name)){Error=TEXT("Duplicate default JSON field: ")+Name;return false;}Stack.Last().Names.Add(Name);}
  if(Token==EJsonNotation::ObjectStart || Token==EJsonNotation::ArrayStart)Stack.Add({Token==EJsonNotation::ObjectStart,{}});
 }
 if(!Stack.IsEmpty() || !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text),Out) || !Out){Error=TEXT("Malformed default JSON");return false;}return true;
}
bool DefaultTaggedString(const TSharedPtr<FJsonValue>& Json,const TCHAR* Tag,FString& Out){
 const TSharedPtr<FJsonObject>* Object=nullptr;if(!Json || !Json->TryGetObject(Object) || (*Object)->Values.Num()!=1)return false;
 auto Field=(*Object)->TryGetField(Tag);return Field && Field->Type==EJson::String && Field->TryGetString(Out);
}
bool ReadDefault(FProperty* P,const TSharedPtr<FJsonValue>& Json,FDasDefaultValue& Out,FDasDefaultBudget& Budget,int32 Depth,const FString& Location,FString& Error);
bool ReadStructDefault(UScriptStruct* Type,const TSharedPtr<FJsonObject>& Object,FDasDefaultValue& Out,FDasDefaultBudget& Budget,int32 Depth,FString& Error){
 TSet<FProperty*> Seen;
 for(const auto& Pair:Object->Values){const FString Name(*Pair.Key);auto* Field=FindDasStructField(Type,Name);if(!Field || Seen.Contains(Field)){Error=TEXT("Unknown or duplicate struct default field: ")+Out.Location+TEXT(".")+Name;return false;}Seen.Add(Field);
  auto& Child=Out.Children.AddDefaulted_GetRef();if(!ReadDefault(Field,Pair.Value,Child,Budget,Depth+1,Out.Location+TEXT(".")+Name,Error))return false;
 }return true;
}
bool ReadDefault(FProperty* P,const TSharedPtr<FJsonValue>& Json,FDasDefaultValue& Out,FDasDefaultBudget& Budget,int32 Depth,const FString& Location,FString& Error){
 Out.Property=P;Out.Location=Location;
 auto Fail=[&](){Error=TEXT("Invalid or unsupported typed default: ")+Location;return false;};
 if(!P || P->ArrayDim!=1 || !Json)return Fail();if(!Budget.Enter(Depth,P->GetElementSize(),Error))return false;
 if(CastField<FArrayProperty>(P) || CastField<FSetProperty>(P) || CastField<FMapProperty>(P)){
  const TArray<TSharedPtr<FJsonValue>>* Items=nullptr;if(!Json->TryGetArray(Items))return Fail();Budget.Elements+=Items->Num();if(Budget.Elements>4096){Error=TEXT("Default exceeds total container element limit");return false;}
  auto* A=CastField<FArrayProperty>(P);auto* S=CastField<FSetProperty>(P);auto* M=CastField<FMapProperty>(P);Out.Kind=A?EDasDefaultNode::Array:S?EDasDefaultNode::Set:EDasDefaultNode::Map;
  for(int32 I=0;I<Items->Num();++I){const FString At=Location+FString::Printf(TEXT("[%d]"),I);
   if(M){const TArray<TSharedPtr<FJsonValue>>* Pair=nullptr;if(!(*Items)[I]->TryGetArray(Pair) || Pair->Num()!=2)return Fail();auto& Key=Out.Children.AddDefaulted_GetRef();if(!ReadDefault(M->KeyProp,(*Pair)[0],Key,Budget,Depth+1,At+TEXT(".key"),Error))return false;auto& Value=Out.Children.AddDefaulted_GetRef();if(!ReadDefault(M->ValueProp,(*Pair)[1],Value,Budget,Depth+1,At+TEXT(".value"),Error))return false;}
   else {auto& Child=Out.Children.AddDefaulted_GetRef();if(!ReadDefault(A?A->Inner:S->ElementProp,(*Items)[I],Child,Budget,Depth+1,At,Error))return false;}
  }return true;
 }
 if(auto* Struct=CastField<FStructProperty>(P)){
  if(!ValidateDasValueStruct(Struct->Struct->GetPathName(),Error))return false;
  Out.Kind=EDasDefaultNode::Struct;
  const TSharedPtr<FJsonObject>* Object=nullptr;
  if(Json->TryGetObject(Object))return ReadStructDefault(Struct->Struct,*Object,Out,Budget,Depth,Error);
  return Fail();
 }
 if(auto* Class=CastField<FClassProperty>(P)){
  if(!DefaultTaggedString(Json,TEXT("class"),Out.Text))return Fail();Out.Kind=EDasDefaultNode::Class;
 }else if(DasReferencePropertyClass(P)){
  if(!DefaultTaggedString(Json,TEXT("asset"),Out.Text))return Fail();Out.Kind=EDasDefaultNode::Asset;
 }else if(CastField<FTextProperty>(P)){
  if(!DefaultTaggedString(Json,TEXT("text"),Out.Text))return Fail();Out.Kind=EDasDefaultNode::Text;
 }else if(DasEnumPropertyType(P)){
  if(Json->Type!=EJson::String || !Json->TryGetString(Out.Text))return Fail();Out.Kind=EDasDefaultNode::Enum;
 }else if(CastField<FBoolProperty>(P)){
  Out.Kind=EDasDefaultNode::Boolean;return Json->Type==EJson::Boolean && Json->TryGetBool(Out.Boolean)?true:Fail();
 }else if(auto* Numeric=CastField<FNumericProperty>(P)){
  Out.Kind=EDasDefaultNode::Number;if(Json->Type!=EJson::Number || !Json->TryGetNumber(Out.Number) || !FMath::IsFinite(Out.Number))return Fail();const double N=Out.Number;
  if(Numeric->IsA<FIntProperty>())return N>=MIN_int32 && N<=MAX_int32 && FMath::FloorToDouble(N)==N?true:Fail();
  if(Numeric->IsA<FByteProperty>())return N>=0 && N<=255 && FMath::FloorToDouble(N)==N?true:Fail();
  if(Numeric->IsA<FFloatProperty>())return FMath::IsFinite(float(N)) && (N==0 || float(N)!=0)?true:Fail();
  return Numeric->IsA<FDoubleProperty>()?true:Fail();
 }else {
  if(Json->Type!=EJson::String || !Json->TryGetString(Out.Text))return Fail();Out.Kind=CastField<FNameProperty>(P)?EDasDefaultNode::Name:EDasDefaultNode::String;
  if(!CastField<FNameProperty>(P) && !CastField<FStrProperty>(P) && !CastField<FTextProperty>(P))return Fail();
 }
 if(Out.Text.Len()>65536)return Fail();Budget.Bytes+=int64(Out.Text.Len())*sizeof(TCHAR);if(Budget.Bytes>16*1024*1024){Error=TEXT("Default exceeds byte limit");return false;}for(TCHAR C:Out.Text)if(C==0)return Fail();
 if(Out.Kind==EDasDefaultNode::Name && Out.Text.Len()>=NAME_SIZE)return Fail();
 if((Out.Kind==EDasDefaultNode::Asset || Out.Kind==EDasDefaultNode::Class) && Out.Text.Len()>4096)return Fail();
 return true;
}
bool WriteDefault(const FDasDefaultValue& Value,void* Data,FDasDefaultRoots& Roots,FString& Error){
 auto* P=Value.Property;
 auto Fail=[&](const FString& Reason){Error=Reason+TEXT(" at ")+Value.Location;return false;};
 switch(Value.Kind){
 case EDasDefaultNode::Boolean:CastFieldChecked<FBoolProperty>(P)->SetPropertyValue(Data,Value.Boolean);return true;
 case EDasDefaultNode::Number:{auto* N=CastFieldChecked<FNumericProperty>(P);if(N->IsInteger())N->SetIntPropertyValue(Data,int64(Value.Number));else N->SetFloatingPointPropertyValue(Data,Value.Number);return true;}
 case EDasDefaultNode::String:CastFieldChecked<FStrProperty>(P)->SetPropertyValue(Data,Value.Text);return true;
 case EDasDefaultNode::Text:{FText Text;if(!ParseDasTextDefault(Value.Text,Text,Error))return false;CastFieldChecked<FTextProperty>(P)->SetPropertyValue(Data,Text);return true;}
 case EDasDefaultNode::Name:CastFieldChecked<FNameProperty>(P)->SetPropertyValue(Data,FName(*Value.Text));return true;
 case EDasDefaultNode::Enum:{auto* E=DasEnumPropertyType(P);const int64 N=E->GetValueByNameString(Value.Text);const int32 I=E->GetIndexByValue(N);return (I!=INDEX_NONE && !E->GetNameStringByIndex(I).EndsWith(TEXT("_MAX")) && N>=0 && N<255 && SetDasEnumPropertyNumber(P,Data,N))?true:Fail(TEXT("Unknown or reserved enum member"));}
 case EDasDefaultNode::Asset:case EDasDefaultNode::Class:{const bool Class=Value.Kind==EDasDefaultNode::Class;auto* Base=Class?CastFieldChecked<FClassProperty>(P)->MetaClass.Get():DasReferencePropertyClass(P);UObject* Object=nullptr;if(!Base || !ResolveDasReferenceDefault(Class,Base->GetPathName(),Value.Text,Object,Error)){Error+=TEXT(" at ")+Value.Location;return false;}if(Object)Roots.Objects.Add(Object);if(Class){CastFieldChecked<FClassProperty>(P)->SetObjectPropertyValue(Data,Object);return true;}return DasReferenceSet(P,Data,Object);}
 case EDasDefaultNode::Struct:for(const auto& Child:Value.Children)if(!WriteDefault(Child,Child.Property->ContainerPtrToValuePtr<void>(Data),Roots,Error))return false;return true;

 case EDasDefaultNode::Array:{auto* A=CastFieldChecked<FArrayProperty>(P);FScriptArrayHelper H(A,Data);H.EmptyValues();H.AddValues(Value.Children.Num());for(int32 I=0;I<Value.Children.Num();++I)if(!WriteDefault(Value.Children[I],H.GetRawPtr(I),Roots,Error))return false;return true;}
 case EDasDefaultNode::Set:{auto* S=CastFieldChecked<FSetProperty>(P);FScriptSetHelper H(S,Data);H.EmptyElements();for(const auto& Child:Value.Children){const int32 I=H.AddDefaultValue_Invalid_NeedsRehash();if(!WriteDefault(Child,H.GetElementPtr(I),Roots,Error))return false;for(int32 J=0;J<I;++J)if(H.IsValidIndex(J) && S->ElementProp->Identical(H.GetElementPtr(J),H.GetElementPtr(I)))return Fail(TEXT("Duplicate set default under Unreal equality"));}H.Rehash();return true;}
 case EDasDefaultNode::Map:{auto* M=CastFieldChecked<FMapProperty>(P);FScriptMapHelper H(M,Data);H.EmptyValues();for(int32 N=0;N<Value.Children.Num();N+=2){const int32 I=H.AddDefaultValue_Invalid_NeedsRehash();if(!WriteDefault(Value.Children[N],H.GetKeyPtr(I),Roots,Error) || !WriteDefault(Value.Children[N+1],H.GetValuePtr(I),Roots,Error))return false;for(int32 J=0;J<I;++J)if(H.IsValidIndex(J) && M->KeyProp->Identical(H.GetKeyPtr(J),H.GetKeyPtr(I)))return Fail(TEXT("Duplicate map default key under Unreal equality"));}H.Rehash();return true;}
 }return Fail(TEXT("Unsupported default node"));
}
bool ParseDasTypedDefault(const FString& Path,const FString& Text,FInstancedStruct& Out,FString& Error){
 Out.Reset();Error.Reset();auto* Property=DasContainerProperty(Path);auto* Type=Property?Cast<UScriptStruct>(Property->GetOwnerUObject()):FindObject<UScriptStruct>(nullptr,*Path);if(!Type || !ValidateDasValueStruct(Path,Error))return false;
 FDasDefaultRoots Roots;auto& Candidate=Roots.Values.AddDefaulted_GetRef();Candidate.InitializeAs(Type);
 if(!Text.IsEmpty()){
  TSharedPtr<FJsonValue> Json;if(!DefaultJson(Text,Json,Error))return false;
  FDasDefaultBudget Budget;FDasDefaultValue Value;Value.Location=TEXT("default");
  if(Property){if(!ReadDefault(Property,Json,Value,Budget,0,Value.Location,Error) || !WriteDefault(Value,Property->ContainerPtrToValuePtr<void>(Candidate.GetMutableMemory()),Roots,Error))return false;}
  else {const TSharedPtr<FJsonObject>* Object=nullptr;if(!Json->TryGetObject(Object)){Error=TEXT("Struct default requires JSON fields");return false;}if(!ReadStructDefault(Type,*Object,Value,Budget,0,Error) || !WriteDefault(Value,Candidate.GetMutableMemory(),Roots,Error))return false;}
 }
 if(!ValidateDasStructSnapshot(Candidate,Error))return false;Out=MoveTemp(Candidate);return true;
}
}

// Shared syntax validation for source lowering and persisted values.
bool ParseDasDefaultJson(const FString& Text,TSharedPtr<FJsonValue>& Out,FString& Error){return DefaultJson(Text,Out,Error);}

bool ParseDasPropertyDefault(FProperty* Property,const TSharedPtr<FJsonValue>& Json,void* Data,FString& Error){
 Error.Reset();FDasDefaultValue Value;FDasDefaultBudget Budget;if(!ReadDefault(Property,Json,Value,Budget,0,Property?Property->GetName():TEXT("default"),Error))return false;
 // Pin the property owner and every loaded object until the final copy.
 FDasDefaultRoots Roots;Roots.Objects.Add(Property->GetOwnerUObject());
 struct FStorage {
  FProperty* P;void* Memory;
  explicit FStorage(FProperty* In):P(In),Memory(FMemory::Malloc(In->GetSize(),In->GetMinAlignment())){P->InitializeValue(Memory);}
  ~FStorage(){P->DestroyValue(Memory);FMemory::Free(Memory);}
 } Storage(Property);
 if(!WriteDefault(Value,Storage.Memory,Roots,Error))return false;
 // Reuse snapshot policy validation for structs and supported container shapes.
 UScriptStruct* Type=nullptr;FProperty* CarrierValue=nullptr;
 if(auto* Struct=CastField<FStructProperty>(Property))Type=Struct->Struct;
 else if(CastField<FArrayProperty>(Property) || CastField<FSetProperty>(Property) || CastField<FMapProperty>(Property)){Type=DasContainerCarrierForProperty(Property);if(!Type){Error=TEXT("Unsupported reflected default container shape");return false;}CarrierValue=Type->FindPropertyByName(TEXT("Values"));}
 if(Type){auto& Snapshot=Roots.Values.AddDefaulted_GetRef();Snapshot.InitializeAs(Type);if(CarrierValue)CarrierValue->CopyCompleteValue(CarrierValue->ContainerPtrToValuePtr<void>(Snapshot.GetMutableMemory()),Storage.Memory);else Type->CopyScriptStruct(Snapshot.GetMutableMemory(),Storage.Memory);if(!ValidateDasStructSnapshot(Snapshot,Error))return false;}
 if(Data)Property->CopyCompleteValue(Data,Storage.Memory);return true;
}
TSharedPtr<FJsonValue> ExportDasPropertyDefault(FProperty* P,const void* Data){
 if(auto* A=CastField<FArrayProperty>(P)){TArray<TSharedPtr<FJsonValue>> Items;FScriptArrayHelper H(A,Data);for(int32 I=0;I<H.Num();++I)Items.Add(ExportDasPropertyDefault(A->Inner,H.GetRawPtr(I)));return MakeShared<FJsonValueArray>(Items);}
 if(auto* S=CastField<FSetProperty>(P)){TArray<TSharedPtr<FJsonValue>> Items;FScriptSetHelper H(S,Data);for(int32 I=0;I<H.GetMaxIndex();++I)if(H.IsValidIndex(I))Items.Add(ExportDasPropertyDefault(S->ElementProp,H.GetElementPtr(I)));return MakeShared<FJsonValueArray>(Items);}
 if(auto* M=CastField<FMapProperty>(P)){TArray<TSharedPtr<FJsonValue>> Items;FScriptMapHelper H(M,Data);for(int32 I=0;I<H.GetMaxIndex();++I)if(H.IsValidIndex(I))Items.Add(MakeShared<FJsonValueArray>(TArray<TSharedPtr<FJsonValue>>{ExportDasPropertyDefault(M->KeyProp,H.GetKeyPtr(I)),ExportDasPropertyDefault(M->ValueProp,H.GetValuePtr(I))}));return MakeShared<FJsonValueArray>(Items);}
 if(auto* S=CastField<FStructProperty>(P)){auto Object=MakeShared<FJsonObject>();for(TFieldIterator<FProperty> It(S->Struct);It;++It)Object->SetField(It->GetName(),ExportDasPropertyDefault(*It,It->ContainerPtrToValuePtr<void>(Data)));return MakeShared<FJsonValueObject>(Object);}
 if(auto* E=DasEnumPropertyType(P))return MakeShared<FJsonValueString>(E->GetNameStringByValue(DasEnumPropertyNumber(P,Data)));
 if(auto* Class=CastField<FClassProperty>(P)){auto Object=MakeShared<FJsonObject>();auto* V=Class->GetObjectPropertyValue(Data);Object->SetStringField(TEXT("class"),V?V->GetPathName():FString());return MakeShared<FJsonValueObject>(Object);}
 if(DasReferencePropertyClass(P)){auto Object=MakeShared<FJsonObject>();auto* V=DasReferenceGet(P,Data);Object->SetStringField(TEXT("asset"),V?V->GetPathName():FString());return MakeShared<FJsonValueObject>(Object);}
 if(auto* B=CastField<FBoolProperty>(P))return MakeShared<FJsonValueBoolean>(B->GetPropertyValue(Data));
 if(auto* N=CastField<FNumericProperty>(P))return MakeShared<FJsonValueNumber>(N->IsInteger()?double(N->GetSignedIntPropertyValue(Data)):N->GetFloatingPointPropertyValue(Data));
 if(auto* N=CastField<FNameProperty>(P))return MakeShared<FJsonValueString>(N->GetPropertyValue(Data).ToString());
 if(auto* T=CastField<FTextProperty>(P)){auto Object=MakeShared<FJsonObject>();FString Wire;FTextStringHelper::WriteToBuffer(Wire,T->GetPropertyValue(Data));Object->SetStringField(TEXT("text"),Wire);return MakeShared<FJsonValueObject>(Object);}
 if(auto* S=CastField<FStrProperty>(P))return MakeShared<FJsonValueString>(S->GetPropertyValue(Data));
 return MakeShared<FJsonValueNull>();
}
