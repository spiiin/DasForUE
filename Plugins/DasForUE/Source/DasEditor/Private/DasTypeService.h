#pragma once
#include "CoreMinimal.h"
class UDasTypeRegistry;
struct FDasReimportPlan;
class FDasTypeService
{
public:
    // What saving Script would change in the structs of RegistryPath, without changing anything: fields (named
    // Struct.Field), affected Blueprints, maps and DataTables, and the rules a migration needs (drop for removed
    // fields, exact convert for numeric retypes; empty when none). False when the types cannot be read.
    static bool PlanStructMigration(const FString& Script, const FString& RegistryPath, FDasReimportPlan& Out,
                                    FString& RulesJson, FString& Error);
    static bool MigrateStructFields(const FString& Script, const FString& RegistryPath, FString& Error,
                                    const FString& RulesFile = FString());
    static bool MigrateEnumValues(const FString& Script, const FString& RegistryPath, const FString& RulesFile,
                                  FString& Error);
    static bool RenameEnumMembers(const FString& Script, const FString& RegistryPath, FString& Error);
    static bool ExtendEnums(const FString& Script, const FString& RegistryPath, FString& Error,
                            bool RepairGraphs = false);
    static bool BuildRegistry(const TArray<UObject*>& Types, const FString& Package, UDasTypeRegistry*& Out,
                              FString& Error);
    static bool Apply(const FString& Script, const FString& Folder, TArray<UObject*>& Out, FString& Error);
};
