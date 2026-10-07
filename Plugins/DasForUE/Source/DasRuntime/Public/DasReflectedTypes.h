#pragma once
#include "DasAuthoring.h"
struct FDasEnumEntry {FString Name;int64 Value=0;};
struct FDasEnumDeclaration {FString Id,Name;TArray<FDasEnumEntry> Entries;TMap<FString,FString> Metadata;};
struct DASRUNTIME_API FDasReflectedTypes {
 FString Id;TArray<FDasEnumDeclaration> Enums;TArray<FDasDeclaredClass> Structs;
 bool Validate(FString& Error)const;
 bool ToJson(FString& Out,FString& Error)const;
 static bool FromJson(const FString& Json,FDasReflectedTypes& Out,FString& Error);
};
