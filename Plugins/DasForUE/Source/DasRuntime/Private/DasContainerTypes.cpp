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

const FString StructMapPrefix=TEXT("/Script/DasRuntime.DasStructMap_");
TMap<FString,TWeakObjectPtr<UScriptStruct>> StructMapCarriers;
TMap<FString,FString> StructMapLayouts;
UScriptStruct* StructMapCarrier(UScriptStruct* Element,bool StringKey){
    if(!IsInGameThread() || !IsValid(Element) || (!IsDasMathStructPath(Element->GetPathName()) && (!Element->IsA<UUserDefinedStruct>() || !Element->GetPathName().StartsWith(TEXT("/Game/")))))return nullptr;
    FString Error;if(!ValidateDasValueStruct(Element->GetPathName(),Error))return nullptr;
    const FTCHARToUTF8 Text(*Element->GetPathName());const TCHAR* Digits=TEXT("0123456789ABCDEF");FString Path=StructMapPrefix+(StringKey?TEXT("S_"):TEXT("I_"));
    for(int32 I=0;I<Text.Length();++I){const uint8 Byte=uint8(Text.Get()[I]);Path.AppendChar(Digits[Byte>>4]);Path.AppendChar(Digits[Byte&15]);}
    if(Path.Len()>=NAME_SIZE)return nullptr;const FString Layout=StructArrayLayout(Element);
    if(const auto* Cached=StructMapCarriers.Find(Path)){auto* S=Cached->Get();auto* Values=S?CastField<FMapProperty>(S->FindPropertyByName(TEXT("Values"))):nullptr;auto* V=Values?CastField<FStructProperty>(Values->ValueProp):nullptr;return V && V->Struct==Element && V->GetElementSize()==Element->GetStructureSize() && (StringKey?Values->KeyProp->IsA<FStrProperty>():Values->KeyProp->IsA<FIntProperty>()) && StructMapLayouts.FindRef(Path)==Layout?S:nullptr;}
    if(StructMapCarriers.Num()>=1024 || FindObject<UScriptStruct>(nullptr,*Path))return nullptr;
    auto* S=NewObject<UScriptStruct>(CreatePackage(TEXT("/Script/DasRuntime")),*Path.RightChop(FString(TEXT("/Script/DasRuntime.")).Len()),RF_Transient);
    auto* Values=new FMapProperty(S,TEXT("Values"),RF_Transient);
    if(StringKey)Values->KeyProp=new FStrProperty(Values,TEXT("Key"),RF_Transient);else Values->KeyProp=new FIntProperty(Values,TEXT("Key"),RF_Transient);
    Values->KeyProp->SetPropertyFlags(CPF_HasGetValueTypeHash);
    auto* Value=new FStructProperty(Values,TEXT("Value"),RF_Transient);Value->Struct=Element;Values->ValueProp=Value;
    S->AddCppProperty(Values);S->Bind();S->StaticLink(true);S->AddToRoot();StructMapCarriers.Add(Path,S);StructMapLayouts.Add(Path,Layout);return S;
}
UScriptStruct* ResolveStructMapCarrier(const FString& Path){
    if(!Path.StartsWith(StructMapPrefix) || Path.Len()>=NAME_SIZE)return nullptr;
    FString Encoded=Path.RightChop(StructMapPrefix.Len());const bool StringKey=Encoded.StartsWith(TEXT("S_"));if(!StringKey && !Encoded.StartsWith(TEXT("I_")))return nullptr;const FString Hex=Encoded.RightChop(2);if(Hex.IsEmpty() || Hex.Len()%2)return nullptr;
    auto Digit=[](TCHAR C){return C>='0' && C<='9'?int32(C-'0'):C>='A' && C<='F'?int32(C-'A'+10):-1;};TArray<ANSICHAR> Bytes;
    for(int32 I=0;I<Hex.Len();I+=2){const int32 H=Digit(Hex[I]),L=Digit(Hex[I+1]);if(H<0 || L<0 || !(H || L))return nullptr;Bytes.Add(ANSICHAR((H<<4)|L));}Bytes.Add(0);
    const FString ElementPath=UTF8_TO_TCHAR(Bytes.GetData());if(!ElementPath.StartsWith(TEXT("/Game/")) && !IsDasMathStructPath(ElementPath))return nullptr;
    auto* Element=FindObject<UScriptStruct>(nullptr,*ElementPath);if(!Element)Element=LoadObject<UScriptStruct>(nullptr,*ElementPath,nullptr,LOAD_NoWarn);
    auto* S=StructMapCarrier(Element,StringKey);return S && S->GetPathName()==Path?S:nullptr;
}


const FString ReflectedMapPrefix=TEXT("/Script/DasRuntime.DasReflectedMap_");
TMap<FString,TWeakObjectPtr<UScriptStruct>> ReflectedMapCarriers;
TMap<FString,FString> ReflectedMapLayouts;
UScriptStruct* ReflectedMapCarrier(UObject* Element,bool StringKey){
    if(!IsInGameThread() || !IsValid(Element))return nullptr;
    auto* Enum=Cast<UDasDefinedEnum>(Element);auto* Class=Cast<UClass>(Element);
    if(Enum?!Element->GetPathName().StartsWith(TEXT("/Game/")):!Class || !Element->GetPathName().StartsWith(TEXT("/Script/")))return nullptr;
    FString Layout=Element->GetPathName();if(Enum)for(int32 I=0;I<Enum->NumEnums();++I)Layout+=TEXT("|")+Enum->GetNameByIndex(I).ToString()+TEXT(":")+FString::Printf(TEXT("%lld"),Enum->GetValueByIndex(I))+TEXT(":")+Enum->MemberIds.FindRef(Enum->GetValueByIndex(I));else Layout+=Class->HasAnyClassFlags(CLASS_Interface)?TEXT(":interface"):TEXT(":object");
    FString Path=ReflectedMapPrefix+(StringKey?TEXT("S_"):TEXT("I_"))+(Enum?TEXT("E_"):TEXT("C_"));const FTCHARToUTF8 Text(*Element->GetPathName());const TCHAR* Digits=TEXT("0123456789ABCDEF");
    for(int32 I=0;I<Text.Length();++I){const uint8 B=uint8(Text.Get()[I]);Path.AppendChar(Digits[B>>4]);Path.AppendChar(Digits[B&15]);}if(Path.Len()>=NAME_SIZE)return nullptr;
    if(const auto* Cached=ReflectedMapCarriers.Find(Path)){auto* S=Cached->Get();auto* M=S?CastField<FMapProperty>(S->FindPropertyByName(TEXT("Values"))):nullptr;return M && (StringKey?M->KeyProp->IsA<FStrProperty>():M->KeyProp->IsA<FIntProperty>()) && (Enum?DasEnumPropertyType(M->ValueProp)==Enum:DasReferencePropertyClass(M->ValueProp)==Class && bool(CastField<FInterfaceProperty>(M->ValueProp))==Class->HasAnyClassFlags(CLASS_Interface)) && ReflectedMapLayouts.FindRef(Path)==Layout?S:nullptr;}
    if(ReflectedMapCarriers.Num()>=1024 || FindObject<UScriptStruct>(nullptr,*Path))return nullptr;
    auto* S=NewObject<UScriptStruct>(CreatePackage(TEXT("/Script/DasRuntime")),*Path.RightChop(FString(TEXT("/Script/DasRuntime.")).Len()),RF_Transient);auto* M=new FMapProperty(S,TEXT("Values"),RF_Transient);
    if(StringKey)M->KeyProp=new FStrProperty(M,TEXT("Key"),RF_Transient);else M->KeyProp=new FIntProperty(M,TEXT("Key"),RF_Transient);M->KeyProp->SetPropertyFlags(CPF_HasGetValueTypeHash);
    if(Enum){auto* V=new FByteProperty(M,TEXT("Value"),RF_Transient);V->Enum=Enum;M->ValueProp=V;}else if(Class->HasAnyClassFlags(CLASS_Interface)){auto* V=new FInterfaceProperty(M,TEXT("Value"),RF_Transient);V->InterfaceClass=Class;M->ValueProp=V;}else {auto* V=new FObjectProperty(M,TEXT("Value"),RF_Transient);V->PropertyClass=Class;M->ValueProp=V;}
    S->AddCppProperty(M);S->Bind();S->StaticLink(true);S->AddToRoot();ReflectedMapCarriers.Add(Path,S);ReflectedMapLayouts.Add(Path,Layout);return S;
}
UScriptStruct* ResolveReflectedMapCarrier(const FString& Path){
    if(!Path.StartsWith(ReflectedMapPrefix) || Path.Len()>=NAME_SIZE)return nullptr;const FString Text=Path.RightChop(ReflectedMapPrefix.Len());const bool StringKey=Text.StartsWith(TEXT("S_"));if(!StringKey && !Text.StartsWith(TEXT("I_")))return nullptr;
    const FString Kind=Text.Mid(2,2);if(Kind!=TEXT("E_") && Kind!=TEXT("C_"))return nullptr;const FString Hex=Text.RightChop(4);if(Hex.IsEmpty() || Hex.Len()%2)return nullptr;TArray<ANSICHAR> Bytes;
    auto Digit=[](TCHAR C){return C>='0' && C<='9'?int32(C-'0'):C>='A' && C<='F'?int32(C-'A'+10):-1;};for(int32 I=0;I<Hex.Len();I+=2){int32 H=Digit(Hex[I]),L=Digit(Hex[I+1]);if(H<0 || L<0 || !(H || L))return nullptr;Bytes.Add(ANSICHAR((H<<4)|L));}Bytes.Add(0);
    const FString ElementPath=UTF8_TO_TCHAR(Bytes.GetData());UObject* Element=nullptr;if(Kind==TEXT("E_")){if(!ElementPath.StartsWith(TEXT("/Game/")))return nullptr;Element=FindObject<UDasDefinedEnum>(nullptr,*ElementPath);if(!Element)Element=LoadObject<UDasDefinedEnum>(nullptr,*ElementPath,nullptr,LOAD_NoWarn);}else {if(!ElementPath.StartsWith(TEXT("/Script/")))return nullptr;Element=FindObject<UClass>(nullptr,*ElementPath);if(!Element)Element=LoadObject<UClass>(nullptr,*ElementPath,nullptr,LOAD_NoWarn);}
    auto* S=ReflectedMapCarrier(Element,StringKey);return S && S->GetPathName()==Path?S:nullptr;
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

const FString ReferenceArrayPrefix=TEXT("/Script/DasRuntime.DasReferenceArray_");
TMap<FString,TWeakObjectPtr<UScriptStruct>> ReferenceArrayCarriers;
UScriptStruct* ReferenceArrayCarrier(UClass* Element){
    if(!IsInGameThread() || !IsValid(Element) || !Element->GetPathName().StartsWith(TEXT("/Script/")))return nullptr;
    const FTCHARToUTF8 Text(*Element->GetPathName());const TCHAR* Digits=TEXT("0123456789ABCDEF");FString Path=ReferenceArrayPrefix;
    for(int32 I=0;I<Text.Length();++I){const uint8 Byte=uint8(Text.Get()[I]);Path.AppendChar(Digits[Byte>>4]);Path.AppendChar(Digits[Byte&15]);}
    if(Path.Len()>=NAME_SIZE)return nullptr;
    if(const auto* Cached=ReferenceArrayCarriers.Find(Path)){auto* S=Cached->Get();auto* Values=S?CastField<FArrayProperty>(S->FindPropertyByName(TEXT("Values"))):nullptr;return Values && DasReferencePropertyClass(Values->Inner)==Element && bool(CastField<FInterfaceProperty>(Values->Inner))==Element->HasAnyClassFlags(CLASS_Interface)?S:nullptr;}
    if(ReferenceArrayCarriers.Num()>=1024 || FindObject<UScriptStruct>(nullptr,*Path))return nullptr;
    auto* S=NewObject<UScriptStruct>(CreatePackage(TEXT("/Script/DasRuntime")),*Path.RightChop(FString(TEXT("/Script/DasRuntime.")).Len()),RF_Transient);
    auto* Values=new FArrayProperty(S,TEXT("Values"),RF_Transient);
    if(Element->HasAnyClassFlags(CLASS_Interface)){auto* Inner=new FInterfaceProperty(Values,TEXT("Values"),RF_Transient);Inner->InterfaceClass=Element;Values->Inner=Inner;}
    else {auto* Inner=new FObjectProperty(Values,TEXT("Values"),RF_Transient);Inner->PropertyClass=Element;Values->Inner=Inner;}
    S->AddCppProperty(Values);S->Bind();S->StaticLink(true);S->AddToRoot();ReferenceArrayCarriers.Add(Path,S);return S;
}
UScriptStruct* ResolveReferenceArrayCarrier(const FString& Path){
    if(!Path.StartsWith(ReferenceArrayPrefix) || Path.Len()>=NAME_SIZE)return nullptr;const FString Hex=Path.RightChop(ReferenceArrayPrefix.Len());if(Hex.IsEmpty() || Hex.Len()%2)return nullptr;
    auto Digit=[](TCHAR C){return C>='0' && C<='9'?int32(C-'0'):C>='A' && C<='F'?int32(C-'A'+10):-1;};TArray<ANSICHAR> Bytes;
    for(int32 I=0;I<Hex.Len();I+=2){const int32 H=Digit(Hex[I]),L=Digit(Hex[I+1]);if(H<0 || L<0 || !(H || L))return nullptr;Bytes.Add(ANSICHAR((H<<4)|L));}Bytes.Add(0);
    const FString ClassPath=UTF8_TO_TCHAR(Bytes.GetData());if(!ClassPath.StartsWith(TEXT("/Script/")))return nullptr;auto* C=FindObject<UClass>(nullptr,*ClassPath);if(!C)C=LoadObject<UClass>(nullptr,*ClassPath,nullptr,LOAD_NoWarn);
    auto* S=ReferenceArrayCarrier(C);return S && S->GetPathName()==Path?S:nullptr;
}

}
void ResetDasStructArrayCarriers(){for(auto& Item:ReflectedMapCarriers)if(auto* S=Item.Value.Get())S->RemoveFromRoot();ReflectedMapCarriers.Empty();ReflectedMapLayouts.Empty();for(auto& Item:StructMapCarriers)if(auto* S=Item.Value.Get())S->RemoveFromRoot();StructMapCarriers.Empty();StructMapLayouts.Empty();for(auto& Item:ReferenceArrayCarriers)if(auto* S=Item.Value.Get())S->RemoveFromRoot();ReferenceArrayCarriers.Empty();for(auto& Item:EnumArrayCarriers)if(auto* S=Item.Value.Get())S->RemoveFromRoot();EnumArrayCarriers.Empty();EnumArrayLayouts.Empty();for(auto& Item:StructArrayCarriers)if(auto* S=Item.Value.Get())S->RemoveFromRoot();StructArrayCarriers.Empty();StructArrayLayouts.Empty();}
UScriptStruct* DasContainerCarrier(const FDasDeclaredType& Type){
    if(Type.Kind==TEXT("map") && Type.Key && Type.Value && (Type.Key->Kind==TEXT("int") || Type.Key->Kind==TEXT("string"))){if(Type.Value->Kind==TEXT("enum"))return ReflectedMapCarrier(FindObject<UDasDefinedEnum>(nullptr,*Type.Value->TypePath),Type.Key->Kind==TEXT("string"));if(Type.Value->Kind==TEXT("object"))return ReflectedMapCarrier(FindObject<UClass>(nullptr,*Type.Value->TypePath),Type.Key->Kind==TEXT("string"));}
    if(Type.Kind==TEXT("map") && Type.Key && Type.Value && (Type.Key->Kind==TEXT("int") || Type.Key->Kind==TEXT("string")) && Type.Value->Kind==TEXT("struct"))return StructMapCarrier(FindObject<UScriptStruct>(nullptr,*Type.Value->TypePath),Type.Key->Kind==TEXT("string"));
    if(Type.Kind==TEXT("array") && Type.Element && Type.Element->Kind==TEXT("object"))return ReferenceArrayCarrier(FindObject<UClass>(nullptr,*Type.Element->TypePath));
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
    if(Path.StartsWith(ReflectedMapPrefix)){auto* S=ResolveReflectedMapCarrier(Path);return S?S->FindPropertyByName(TEXT("Values")):nullptr;}
    if(Path.StartsWith(StructMapPrefix)){auto* S=ResolveStructMapCarrier(Path);return S?S->FindPropertyByName(TEXT("Values")):nullptr;}
    if(Path.StartsWith(ReferenceArrayPrefix)){auto* S=ResolveReferenceArrayCarrier(Path);return S?S->FindPropertyByName(TEXT("Values")):nullptr;}
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
    if(const auto* M=CastField<FMapProperty>(Property))if(M->KeyProp->IsA<FIntProperty>() || M->KeyProp->IsA<FStrProperty>()){if(auto* E=DasEnumPropertyType(M->ValueProp))if(auto* S=ReflectedMapCarrier(E,M->KeyProp->IsA<FStrProperty>()))return S;if(auto* C=DasReferencePropertyClass(M->ValueProp))if(auto* S=ReflectedMapCarrier(C,M->KeyProp->IsA<FStrProperty>()))return S;}
    if(const auto* Map=CastField<FMapProperty>(Property))if(const auto* Value=CastField<FStructProperty>(Map->ValueProp))if(Map->KeyProp->IsA<FIntProperty>() || Map->KeyProp->IsA<FStrProperty>())if(auto* Carrier=StructMapCarrier(Value->Struct,Map->KeyProp->IsA<FStrProperty>()))return Carrier;
    if(const auto* Array=CastField<FArrayProperty>(Property))if(auto* C=DasReferencePropertyClass(Array->Inner))if(auto* Carrier=ReferenceArrayCarrier(C))return Carrier;
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
