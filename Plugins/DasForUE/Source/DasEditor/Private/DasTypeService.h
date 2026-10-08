#pragma once
#include "CoreMinimal.h"
class UDasTypeRegistry;
class FDasTypeService {
public:
 static bool MigrateStructFields(const FString& Script,const FString& RegistryPath,FString& Error,const FString& RulesFile=FString());
 static bool MigrateEnumValues(const FString& Script,const FString& RegistryPath,const FString& RulesFile,FString& Error);
 static bool RenameEnumMembers(const FString& Script,const FString& RegistryPath,FString& Error);
 static bool ExtendEnums(const FString& Script,const FString& RegistryPath,FString& Error,bool RepairGraphs=false);
 static bool BuildRegistry(const TArray<UObject*>& Types,const FString& Package,UDasTypeRegistry*& Out,FString& Error);
 static bool Apply(const FString& Script,const FString& Folder,TArray<UObject*>& Out,FString& Error);
};
