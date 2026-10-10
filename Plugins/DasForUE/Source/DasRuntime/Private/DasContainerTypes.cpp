#include "DasDefinedEnum.h"
#include "DasReflectedTypes.h"
#include "DasContainerTypes.h"
#include "DasAuthoring.h"
#include "UObject/UnrealType.h"

#include "DasSchema.h"
#include "StructUtils/UserDefinedStruct.h"
#include "UObject/Package.h"

namespace {
const FString StructArrayPrefix=TEXT("/Script/DasRuntime.DasStructArray_");
TMap<FString,TWeakObjectPtr<UScriptStruct>> StructArrayCarriers;
TMap<FString,FString> StructArrayLayouts;
FString StructArrayLayout(UScriptStruct* Type){
    FString Key;TFunction<void(FProperty*)> Field;TFunction<void(UScriptStruct*)> Struct;
    Struct=[&](UScriptStruct* S){Key+=S->GetPathName()+FString::Printf(TEXT(":%d:%d{"),S->GetStructureSize(),S->GetMinAlignment());for(TFieldIterator<FProperty> It(S);It;++It)Field(*It);Key+=TEXT("}");};
    Field=[&](FProperty* P){Key+=P->GetName()+TEXT(":")+P->GetClass()->GetName()+FString::Printf(TEXT(":%d:%d:%d;"),P->GetOffset_ForInternal(),P->GetElementSize(),P->ArrayDim);if(auto* S=CastField<FStructProperty>(P))Struct(S->Struct);else if(auto* A=CastField<FArrayProperty>(P))Field(A->Inner);else if(auto* M=CastField<FMapProperty>(P)){Field(M->KeyProp);Field(M->ValueProp);}else if(auto* Set=CastField<FSetProperty>(P))Field(Set->ElementProp);else if(auto* ReferenceClass=DasReferencePropertyClass(P))Key+=ReferenceClass->GetPathName();else if(auto* E=DasEnumPropertyType(P))Key+=E->GetPathName();};
    Struct(Type);return Key;
}
FString StructArrayPath(UScriptStruct* Element){
    const FTCHARToUTF8 Text(*Element->GetPathName());const TCHAR* Digits=TEXT("0123456789ABCDEF");FString Path=StructArrayPrefix;
    for(int32 I=0;I<Text.Length();++I){const uint8 Byte=uint8(Text.Get()[I]);Path.AppendChar(Digits[Byte>>4]);Path.AppendChar(Digits[Byte&15]);}return Path;
}
UScriptStruct* StructArrayCarrier(UScriptStruct* Element){
    if(!IsInGameThread() || !IsValid(Element) || (!IsDasMathStructPath(Element->GetPathName()) && (!Element->IsA<UUserDefinedStruct>() || !Element->GetPathName().StartsWith(TEXT("/Game/")))))return nullptr;
    FString Error;if(!ValidateDasValueStruct(Element->GetPathName(),Error))return nullptr;const FString Path=StructArrayPath(Element);if(Path.Len()>=NAME_SIZE)return nullptr;const FString Layout=StructArrayLayout(Element);
    if(const auto* Cached=StructArrayCarriers.Find(Path)){auto* S=Cached->Get();auto* Values=S?CastField<FArrayProperty>(S->FindPropertyByName(TEXT("Values"))):nullptr;auto* Inner=Values?CastField<FStructProperty>(Values->Inner):nullptr;return Inner && Inner->Struct==Element && Inner->GetElementSize()==Element->GetStructureSize() && StructArrayLayouts.FindRef(Path)==Layout?S:nullptr;}
    if(StructArrayCarriers.Num()>=1024 || FindObject<UScriptStruct>(nullptr,*Path))return nullptr;
    const FString Name=Path.RightChop(FString(TEXT("/Script/DasRuntime.")).Len());
    auto* S=NewObject<UScriptStruct>(CreatePackage(TEXT("/Script/DasRuntime")),*Name,RF_Transient);
    auto* Values=new FArrayProperty(S,TEXT("Values"),RF_Transient);auto* Inner=new FStructProperty(Values,TEXT("Values"),RF_Transient);Inner->Struct=Element;Values->Inner=Inner;S->AddCppProperty(Values);S->Bind();S->StaticLink(true);S->AddToRoot();StructArrayCarriers.Add(Path,S);StructArrayLayouts.Add(Path,Layout);return S;
}
UScriptStruct* ResolveStructArrayCarrier(const FString& Path){
    if(!Path.StartsWith(StructArrayPrefix) || Path.Len()>=NAME_SIZE)return nullptr;
    const FString Hex=Path.RightChop(StructArrayPrefix.Len());if(Hex.IsEmpty() || Hex.Len()%2)return nullptr;TArray<ANSICHAR> Bytes;
    auto Digit=[](TCHAR C){return C>='0' && C<='9'?int32(C-'0'):C>='A' && C<='F'?int32(C-'A'+10):-1;};
    for(int32 I=0;I<Hex.Len();I+=2){const int32 High=Digit(Hex[I]),Low=Digit(Hex[I+1]);if(High<0 || Low<0 || !(High || Low))return nullptr;Bytes.Add(ANSICHAR((High<<4)|Low));}Bytes.Add(0);
    const FString ElementPath=UTF8_TO_TCHAR(Bytes.GetData());if(!ElementPath.StartsWith(TEXT("/Game/")) && !IsDasMathStructPath(ElementPath))return nullptr;
    auto* Element=FindObject<UScriptStruct>(nullptr,*ElementPath);if(!Element)Element=LoadObject<UScriptStruct>(nullptr,*ElementPath,nullptr,LOAD_NoWarn);
    auto* Carrier=StructArrayCarrier(Element);return Carrier && Carrier->GetPathName()==Path?Carrier:nullptr;
}
const FString EnumArrayPrefix=TEXT("/Script/DasRuntime.DasEnumArray_");
TMap<FString,TWeakObjectPtr<UScriptStruct>> EnumArrayCarriers;
TMap<FString,FString> EnumArrayLayouts;
UScriptStruct* EnumArrayCarrier(UEnum* Element){
    if(!IsInGameThread() || !IsValid(Element) || !Element->IsA<UDasDefinedEnum>() || !Element->GetPathName().StartsWith(TEXT("/Game/")))return nullptr;
    const FTCHARToUTF8 Text(*Element->GetPathName());const TCHAR* Digits=TEXT("0123456789ABCDEF");FString Path=EnumArrayPrefix;
    for(int32 I=0;I<Text.Length();++I){const uint8 Byte=uint8(Text.Get()[I]);Path.AppendChar(Digits[Byte>>4]);Path.AppendChar(Digits[Byte&15]);}
    if(Path.Len()>=NAME_SIZE)return nullptr;FString Layout=Element->GetPathName();auto* Authored=CastChecked<UDasDefinedEnum>(Element);
    for(int32 I=0;I<Element->NumEnums();++I)Layout+=TEXT("|")+Element->GetNameByIndex(I).ToString()+TEXT(":")+FString::Printf(TEXT("%lld"),Element->GetValueByIndex(I))+TEXT(":")+Authored->MemberIds.FindRef(Element->GetValueByIndex(I));
    if(const auto* Cached=EnumArrayCarriers.Find(Path)){auto* S=Cached->Get();auto* Values=S?CastField<FArrayProperty>(S->FindPropertyByName(TEXT("Values"))):nullptr;return Values && DasEnumPropertyType(Values->Inner)==Element && EnumArrayLayouts.FindRef(Path)==Layout?S:nullptr;}
    if(EnumArrayCarriers.Num()>=1024 || FindObject<UScriptStruct>(nullptr,*Path))return nullptr;
    auto* S=NewObject<UScriptStruct>(CreatePackage(TEXT("/Script/DasRuntime")),*Path.RightChop(FString(TEXT("/Script/DasRuntime.")).Len()),RF_Transient);
    auto* Values=new FArrayProperty(S,TEXT("Values"),RF_Transient);auto* Inner=new FByteProperty(Values,TEXT("Values"),RF_Transient);Inner->Enum=Element;Values->Inner=Inner;
    S->AddCppProperty(Values);S->Bind();S->StaticLink(true);S->AddToRoot();EnumArrayCarriers.Add(Path,S);EnumArrayLayouts.Add(Path,Layout);return S;
}
UScriptStruct* ResolveEnumArrayCarrier(const FString& Path){
    if(!Path.StartsWith(EnumArrayPrefix) || Path.Len()>=NAME_SIZE)return nullptr;const FString Hex=Path.RightChop(EnumArrayPrefix.Len());if(Hex.IsEmpty() || Hex.Len()%2)return nullptr;
    auto Digit=[](TCHAR C){return C>='0' && C<='9'?int32(C-'0'):C>='A' && C<='F'?int32(C-'A'+10):-1;};TArray<ANSICHAR> Bytes;
    for(int32 I=0;I<Hex.Len();I+=2){const int32 H=Digit(Hex[I]),L=Digit(Hex[I+1]);if(H<0 || L<0 || !(H || L))return nullptr;Bytes.Add(ANSICHAR((H<<4)|L));}Bytes.Add(0);
    const FString ElementPath=UTF8_TO_TCHAR(Bytes.GetData());if(!ElementPath.StartsWith(TEXT("/Game/")))return nullptr;auto* Element=FindObject<UEnum>(nullptr,*ElementPath);if(!Element)Element=LoadObject<UDasDefinedEnum>(nullptr,*ElementPath,nullptr,LOAD_NoWarn);
    auto* S=EnumArrayCarrier(Element);return S && S->GetPathName()==Path?S:nullptr;
}

}
void ResetDasStructArrayCarriers(){for(auto& Item:EnumArrayCarriers)if(auto* S=Item.Value.Get())S->RemoveFromRoot();EnumArrayCarriers.Empty();EnumArrayLayouts.Empty();for(auto& Item:StructArrayCarriers)if(auto* S=Item.Value.Get())S->RemoveFromRoot();StructArrayCarriers.Empty();StructArrayLayouts.Empty();}
UScriptStruct* DasContainerCarrier(const FDasDeclaredType& Type){
    if(Type.Kind==TEXT("array") && Type.Element && Type.Element->Kind==TEXT("enum"))return EnumArrayCarrier(FindObject<UEnum>(nullptr,*Type.Element->TypePath));
    if(Type.Kind==TEXT("array") && Type.Element && Type.Element->Kind==TEXT("struct"))return StructArrayCarrier(FindObject<UScriptStruct>(nullptr,*Type.Element->TypePath));
    if(Type.Kind==TEXT("array") && Type.Element && Type.Element->Kind==TEXT("int"))return FDasArrayIntCarrier::StaticStruct();
    if(Type.Kind==TEXT("array") && Type.Element && Type.Element->Kind==TEXT("float"))return FDasArrayFloatCarrier::StaticStruct();
    if(Type.Kind==TEXT("array") && Type.Element && Type.Element->Kind==TEXT("double"))return FDasArrayDoubleCarrier::StaticStruct();
    if(Type.Kind==TEXT("array") && Type.Element && Type.Element->Kind==TEXT("bool"))return FDasArrayBoolCarrier::StaticStruct();
    if(Type.Kind==TEXT("array") && Type.Element && Type.Element->Kind==TEXT("string"))return FDasArrayStringCarrier::StaticStruct();
    if(Type.Kind==TEXT("array") && Type.Element && Type.Element->Kind==TEXT("name"))return FDasArrayNameCarrier::StaticStruct();
    if(Type.Kind==TEXT("set") && Type.Element && Type.Element->Kind==TEXT("int"))return FDasSetIntCarrier::StaticStruct();
    if(Type.Kind==TEXT("set") && Type.Element && Type.Element->Kind==TEXT("string"))return FDasSetStringCarrier::StaticStruct();
    if(Type.Kind==TEXT("map") && Type.Key && Type.Value && Type.Key->Kind==TEXT("int") && Type.Value->Kind==TEXT("int"))return FDasMapIntIntCarrier::StaticStruct();
    if(Type.Kind==TEXT("map") && Type.Key && Type.Value && Type.Key->Kind==TEXT("int") && Type.Value->Kind==TEXT("float"))return FDasMapIntFloatCarrier::StaticStruct();
    if(Type.Kind==TEXT("map") && Type.Key && Type.Value && Type.Key->Kind==TEXT("int") && Type.Value->Kind==TEXT("double"))return FDasMapIntDoubleCarrier::StaticStruct();
    if(Type.Kind==TEXT("map") && Type.Key && Type.Value && Type.Key->Kind==TEXT("int") && Type.Value->Kind==TEXT("bool"))return FDasMapIntBoolCarrier::StaticStruct();
    if(Type.Kind==TEXT("map") && Type.Key && Type.Value && Type.Key->Kind==TEXT("int") && Type.Value->Kind==TEXT("string"))return FDasMapIntStringCarrier::StaticStruct();
    if(Type.Kind==TEXT("map") && Type.Key && Type.Value && Type.Key->Kind==TEXT("int") && Type.Value->Kind==TEXT("name"))return FDasMapIntNameCarrier::StaticStruct();
    if(Type.Kind==TEXT("map") && Type.Key && Type.Value && Type.Key->Kind==TEXT("string") && Type.Value->Kind==TEXT("int"))return FDasMapStringIntCarrier::StaticStruct();
    if(Type.Kind==TEXT("map") && Type.Key && Type.Value && Type.Key->Kind==TEXT("string") && Type.Value->Kind==TEXT("float"))return FDasMapStringFloatCarrier::StaticStruct();
    if(Type.Kind==TEXT("map") && Type.Key && Type.Value && Type.Key->Kind==TEXT("string") && Type.Value->Kind==TEXT("double"))return FDasMapStringDoubleCarrier::StaticStruct();
    if(Type.Kind==TEXT("map") && Type.Key && Type.Value && Type.Key->Kind==TEXT("string") && Type.Value->Kind==TEXT("bool"))return FDasMapStringBoolCarrier::StaticStruct();
    if(Type.Kind==TEXT("map") && Type.Key && Type.Value && Type.Key->Kind==TEXT("string") && Type.Value->Kind==TEXT("string"))return FDasMapStringStringCarrier::StaticStruct();
    if(Type.Kind==TEXT("map") && Type.Key && Type.Value && Type.Key->Kind==TEXT("string") && Type.Value->Kind==TEXT("name"))return FDasMapStringNameCarrier::StaticStruct();
    return nullptr;
}
FProperty* DasContainerProperty(const FString& Path){
    if(Path.StartsWith(EnumArrayPrefix)){auto* S=ResolveEnumArrayCarrier(Path);return S?S->FindPropertyByName(TEXT("Values")):nullptr;}
    if(Path.StartsWith(StructArrayPrefix)){auto* S=ResolveStructArrayCarrier(Path);return S?S->FindPropertyByName(TEXT("Values")):nullptr;}
    if(Path==FDasArrayIntCarrier::StaticStruct()->GetPathName())return FDasArrayIntCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"));
    if(Path==FDasArrayFloatCarrier::StaticStruct()->GetPathName())return FDasArrayFloatCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"));
    if(Path==FDasArrayDoubleCarrier::StaticStruct()->GetPathName())return FDasArrayDoubleCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"));
    if(Path==FDasArrayBoolCarrier::StaticStruct()->GetPathName())return FDasArrayBoolCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"));
    if(Path==FDasArrayStringCarrier::StaticStruct()->GetPathName())return FDasArrayStringCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"));
    if(Path==FDasArrayNameCarrier::StaticStruct()->GetPathName())return FDasArrayNameCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"));
    if(Path==FDasSetIntCarrier::StaticStruct()->GetPathName())return FDasSetIntCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"));
    if(Path==FDasSetStringCarrier::StaticStruct()->GetPathName())return FDasSetStringCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"));
    if(Path==FDasMapIntIntCarrier::StaticStruct()->GetPathName())return FDasMapIntIntCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"));
    if(Path==FDasMapIntFloatCarrier::StaticStruct()->GetPathName())return FDasMapIntFloatCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"));
    if(Path==FDasMapIntDoubleCarrier::StaticStruct()->GetPathName())return FDasMapIntDoubleCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"));
    if(Path==FDasMapIntBoolCarrier::StaticStruct()->GetPathName())return FDasMapIntBoolCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"));
    if(Path==FDasMapIntStringCarrier::StaticStruct()->GetPathName())return FDasMapIntStringCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"));
    if(Path==FDasMapIntNameCarrier::StaticStruct()->GetPathName())return FDasMapIntNameCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"));
    if(Path==FDasMapStringIntCarrier::StaticStruct()->GetPathName())return FDasMapStringIntCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"));
    if(Path==FDasMapStringFloatCarrier::StaticStruct()->GetPathName())return FDasMapStringFloatCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"));
    if(Path==FDasMapStringDoubleCarrier::StaticStruct()->GetPathName())return FDasMapStringDoubleCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"));
    if(Path==FDasMapStringBoolCarrier::StaticStruct()->GetPathName())return FDasMapStringBoolCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"));
    if(Path==FDasMapStringStringCarrier::StaticStruct()->GetPathName())return FDasMapStringStringCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"));
    if(Path==FDasMapStringNameCarrier::StaticStruct()->GetPathName())return FDasMapStringNameCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"));
    return nullptr;
}
UScriptStruct* DasContainerCarrierForProperty(const FProperty* Property){
    if(const auto* Array=CastField<FArrayProperty>(Property))if(auto* E=DasEnumPropertyType(Array->Inner))if(auto* Carrier=EnumArrayCarrier(E))return Carrier;
    if(!Property)return nullptr;
    if(const auto* Array=CastField<FArrayProperty>(Property))if(const auto* Inner=CastField<FStructProperty>(Array->Inner))if(auto* Carrier=StructArrayCarrier(Inner->Struct))return Carrier;
    if(FDasArrayIntCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"))->SameType(Property))return FDasArrayIntCarrier::StaticStruct();
    if(FDasArrayFloatCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"))->SameType(Property))return FDasArrayFloatCarrier::StaticStruct();
    if(FDasArrayDoubleCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"))->SameType(Property))return FDasArrayDoubleCarrier::StaticStruct();
    if(FDasArrayBoolCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"))->SameType(Property))return FDasArrayBoolCarrier::StaticStruct();
    if(FDasArrayStringCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"))->SameType(Property))return FDasArrayStringCarrier::StaticStruct();
    if(FDasArrayNameCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"))->SameType(Property))return FDasArrayNameCarrier::StaticStruct();
    if(FDasSetIntCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"))->SameType(Property))return FDasSetIntCarrier::StaticStruct();
    if(FDasSetStringCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"))->SameType(Property))return FDasSetStringCarrier::StaticStruct();
    if(FDasMapIntIntCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"))->SameType(Property))return FDasMapIntIntCarrier::StaticStruct();
    if(FDasMapIntFloatCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"))->SameType(Property))return FDasMapIntFloatCarrier::StaticStruct();
    if(FDasMapIntDoubleCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"))->SameType(Property))return FDasMapIntDoubleCarrier::StaticStruct();
    if(FDasMapIntBoolCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"))->SameType(Property))return FDasMapIntBoolCarrier::StaticStruct();
    if(FDasMapIntStringCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"))->SameType(Property))return FDasMapIntStringCarrier::StaticStruct();
    if(FDasMapIntNameCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"))->SameType(Property))return FDasMapIntNameCarrier::StaticStruct();
    if(FDasMapStringIntCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"))->SameType(Property))return FDasMapStringIntCarrier::StaticStruct();
    if(FDasMapStringFloatCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"))->SameType(Property))return FDasMapStringFloatCarrier::StaticStruct();
    if(FDasMapStringDoubleCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"))->SameType(Property))return FDasMapStringDoubleCarrier::StaticStruct();
    if(FDasMapStringBoolCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"))->SameType(Property))return FDasMapStringBoolCarrier::StaticStruct();
    if(FDasMapStringStringCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"))->SameType(Property))return FDasMapStringStringCarrier::StaticStruct();
    if(FDasMapStringNameCarrier::StaticStruct()->FindPropertyByName(TEXT("Values"))->SameType(Property))return FDasMapStringNameCarrier::StaticStruct();
    return nullptr;
}
