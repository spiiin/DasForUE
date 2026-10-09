#include "DasSchema.h"
#include "DasDefinedEnum.h"
#include "UObject/UnrealType.h"
UEnum* ResolveDasEnum(const FString& Path,FString& Error){auto* E=FindObject<UEnum>(nullptr,*Path);if(!E || (!Path.StartsWith(TEXT("/Script/")) && !(Path.StartsWith(TEXT("/Game/")) && E->IsA<UDasDefinedEnum>()))){Error=TEXT("Expected loaded native or authored enum path");return nullptr;}return E;}
bool ValidateDasEnumNumber(const FString& Path,int64 V,FString& Error){auto* E=ResolveDasEnum(Path,Error);if(!E)return false;const int32 I=E->GetIndexByValue(V);bool Hidden=false;
#if WITH_EDITOR
Hidden=I!=INDEX_NONE && E->HasMetaData(TEXT("Hidden"),I);
#endif
if(I==INDEX_NONE || E->GetNameStringByIndex(I).EndsWith(TEXT("_MAX")) || Hidden || V<0 || V>254){Error=TEXT("Unknown or reserved enum value");return false;}return true;}

UEnum* DasEnumPropertyType(FProperty* Property){if(auto* Byte=CastField<FByteProperty>(Property))return Byte->Enum;if(auto* Enum=CastField<FEnumProperty>(Property))return Enum->GetEnum();return nullptr;}
int64 DasEnumPropertyNumber(FProperty* Property,const void* Memory){if(!Memory)return -1;if(auto* Byte=CastField<FByteProperty>(Property))return Byte->Enum?Byte->GetPropertyValue(Memory):-1;if(auto* Enum=CastField<FEnumProperty>(Property))return Enum->GetUnderlyingProperty()->GetSignedIntPropertyValue(Memory);return -1;}
bool SetDasEnumPropertyNumber(FProperty* Property,void* Memory,int64 Value){auto* EnumType=DasEnumPropertyType(Property);FString Error;if(!EnumType || !Memory || !ValidateDasEnumNumber(EnumType->GetPathName(),Value,Error))return false;if(auto* Byte=CastField<FByteProperty>(Property))Byte->SetPropertyValue(Memory,uint8(Value));else CastFieldChecked<FEnumProperty>(Property)->GetUnderlyingProperty()->SetIntPropertyValue(Memory,uint64(Value));return true;}
