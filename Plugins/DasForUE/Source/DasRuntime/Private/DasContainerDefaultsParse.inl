// Persisted container defaults are bounded JSON data, never VM handles.
namespace {
bool ContainerDefaultLeaf(FProperty* P,const TSharedPtr<FJsonValue>& V,void* Data,FString& Error){
 if(!V)return false;
 if(auto* E=DasEnumPropertyType(P)){FString Member;if(V->Type!=EJson::String || !V->TryGetString(Member))return false;const int64 N=E->GetValueByNameString(Member);const int32 I=E->GetIndexByValue(N);return I!=INDEX_NONE && !E->GetNameStringByIndex(I).EndsWith(TEXT("_MAX")) && N>=0 && N<255 && SetDasEnumPropertyNumber(P,Data,N);}
 if(auto* B=CastField<FBoolProperty>(P)){bool X;if(V->Type!=EJson::Boolean || !V->TryGetBool(X))return false;B->SetPropertyValue(Data,X);return true;}
 if(auto* N=CastField<FNumericProperty>(P)){double X;if(V->Type!=EJson::Number || !V->TryGetNumber(X) || !FMath::IsFinite(X))return false;if(N->IsA<FIntProperty>()){if(X<MIN_int32 || X>MAX_int32 || FMath::FloorToDouble(X)!=X)return false;N->SetIntPropertyValue(Data,int64(X));return true;}if(N->IsFloatingPoint()){if(N->IsA<FFloatProperty>() && (!FMath::IsFinite(float(X)) || (X!=0 && float(X)==0)))return false;N->SetFloatingPointPropertyValue(Data,X);return true;}return false;}
 FString Text;if(V->Type!=EJson::String || !V->TryGetString(Text) || Text.Len()>65536)return false;
 for(TCHAR C:Text)if(C==0)return false;
 if(auto* S=CastField<FStrProperty>(P)){S->SetPropertyValue(Data,Text);return true;}
 if(auto* N=CastField<FNameProperty>(P)){if(Text.Len()>=NAME_SIZE)return false;N->SetPropertyValue(Data,FName(*Text));return true;}
 if(auto* S=CastField<FStructProperty>(P)){if(S->Struct!=TBaseStructure<FVector>::Get() && S->Struct!=TBaseStructure<FVector2D>::Get() && S->Struct!=TBaseStructure<FRotator>::Get() && S->Struct!=TBaseStructure<FTransform>::Get())return false;FInstancedStruct Value;if(!ParseDasStructDefault(S->Struct->GetPathName(),Text,Value,Error))return false;S->Struct->CopyScriptStruct(Data,Value.GetMemory());return true;}
 return false;
}
bool ParseContainerDefault(const FString& Path,const FString& Text,FInstancedStruct& Out,FString& Error){
 Out.Reset();auto* Values=DasContainerProperty(Path);auto* Type=Values?Cast<UScriptStruct>(Values->GetOwnerUObject()):nullptr;if(!Type || !ValidateDasValueStruct(Path,Error))return false;
 FInstancedStruct Candidate;Candidate.InitializeAs(Type);if(Text.IsEmpty()){Out=MoveTemp(Candidate);return true;}
 if(Text.Len()>1048576){Error=TEXT("Container default exceeds wire limit");return false;}
 TArray<TSharedPtr<FJsonValue>> Items;auto Reader=TJsonReaderFactory<>::Create(Text);if(!FJsonSerializer::Deserialize(Reader,Items) || Items.Num()>4096){Error=TEXT("Container default requires bounded JSON items");return false;}
 int64 ElementBytes=0;if(auto* A=CastField<FArrayProperty>(Values))ElementBytes=A->Inner->GetElementSize();else if(auto* S=CastField<FSetProperty>(Values))ElementBytes=S->ElementProp->GetElementSize();else if(auto* M=CastField<FMapProperty>(Values))ElementBytes=M->KeyProp->GetElementSize()+M->ValueProp->GetElementSize();if(ElementBytes*Items.Num()>16*1024*1024){Error=TEXT("Container defaults exceed byte limit");return false;}
 void* Data=Values->ContainerPtrToValuePtr<void>(Candidate.GetMutableMemory());
 auto Fail=[&](){if(Error.IsEmpty())Error=TEXT("Invalid, duplicate or unsupported container default value");return false;};
 if(auto* A=CastField<FArrayProperty>(Values)){FScriptArrayHelper H(A,Data);H.AddValues(Items.Num());for(int32 I=0;I<Items.Num();++I)if(!ContainerDefaultLeaf(A->Inner,Items[I],H.GetRawPtr(I),Error))return Fail();}
 else if(auto* S=CastField<FSetProperty>(Values)){FScriptSetHelper H(S,Data);for(const auto& Item:Items){const int32 Slot=H.AddDefaultValue_Invalid_NeedsRehash();if(!ContainerDefaultLeaf(S->ElementProp,Item,H.GetElementPtr(Slot),Error))return Fail();}for(int32 I=0;I<H.GetMaxIndex();++I)if(H.IsValidIndex(I))for(int32 J=0;J<I;++J)if(H.IsValidIndex(J) && S->ElementProp->Identical(H.GetElementPtr(I),H.GetElementPtr(J))){Error=TEXT("Duplicate set default element under Unreal equality");return false;}H.Rehash();}
 else if(auto* M=CastField<FMapProperty>(Values)){FScriptMapHelper H(M,Data);for(const auto& Item:Items){const TArray<TSharedPtr<FJsonValue>>* Pair=nullptr;if(!Item->TryGetArray(Pair) || Pair->Num()!=2)return Fail();const int32 Slot=H.AddDefaultValue_Invalid_NeedsRehash();if(!ContainerDefaultLeaf(M->KeyProp,(*Pair)[0],H.GetKeyPtr(Slot),Error) || !ContainerDefaultLeaf(M->ValueProp,(*Pair)[1],H.GetValuePtr(Slot),Error))return Fail();}for(int32 I=0;I<H.GetMaxIndex();++I)if(H.IsValidIndex(I))for(int32 J=0;J<I;++J)if(H.IsValidIndex(J) && M->KeyProp->Identical(H.GetKeyPtr(I),H.GetKeyPtr(J))){Error=TEXT("Duplicate map default key under Unreal equality");return false;}H.Rehash();}
 else return Fail();
 if(!ValidateDasStructSnapshot(Candidate,Error))return false;Out=MoveTemp(Candidate);return true;
}
}
