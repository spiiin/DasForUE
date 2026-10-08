#include "DasSchema.h"
#include "DasDefinedEnum.h"
UEnum* ResolveDasEnum(const FString& Path,FString& Error){auto* E=FindObject<UEnum>(nullptr,*Path);if(!E || (!Path.StartsWith(TEXT("/Script/")) && !(Path.StartsWith(TEXT("/Game/")) && E->IsA<UDasDefinedEnum>()))){Error=TEXT("Expected loaded native or authored enum path");return nullptr;}return E;}
bool ValidateDasEnumNumber(const FString& Path,int64 V,FString& Error){auto* E=ResolveDasEnum(Path,Error);if(!E)return false;const int32 I=E->GetIndexByValue(V);bool Hidden=false;
#if WITH_EDITOR
Hidden=I!=INDEX_NONE && E->HasMetaData(TEXT("Hidden"),I);
#endif
if(I==INDEX_NONE || E->GetNameStringByIndex(I).EndsWith(TEXT("_MAX")) || Hidden || V<0 || V>254){Error=TEXT("Unknown or reserved enum value");return false;}return true;}
