#include "DasContainerTypes.h"
#include "DasAuthoring.h"
#include "UObject/UnrealType.h"

UScriptStruct* DasContainerCarrier(const FDasDeclaredType& Type){
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
    if(!Property)return nullptr;
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
