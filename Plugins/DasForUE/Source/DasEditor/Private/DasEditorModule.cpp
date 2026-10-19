#include "DasDataTableService.h"
#include "Engine/DataTable.h"
#include "Misc/FileHelper.h"
#include "DasAssetPreflight.h"
#include "DasDataAssetService.h"
#include "Engine/DataAsset.h"
#include "Modules/ModuleManager.h"
#include "DasComponentCompilerExtension.h"
#include "BlueprintCompilationManager.h"
#include "HAL/IConsoleManager.h"
#include "DasDSLService.h"
#include "DasInterfaceService.h"
#include "DasTypeService.h"
#include "Engine/Blueprint.h"
#include "Misc/PackageName.h"
class FDasEditorModule final : public IModuleInterface
{
    IConsoleObject* TableCreateCommand = nullptr;
    IConsoleObject* AssetCheckCommand = nullptr;
    IConsoleObject* AssetCreateCommand = nullptr;
    IConsoleObject* InterfacesCommand = nullptr;
    IConsoleObject* StructFieldsCommand = nullptr;
    IConsoleObject* EnumValuesCommand = nullptr;
    IConsoleObject* EnumRenameCommand = nullptr;
    IConsoleObject* EnumAppendCommand = nullptr;
    IConsoleObject* TypesCommand = nullptr;
    IConsoleObject* ApplyCommand = nullptr;
    IConsoleObject* ReimportCommand = nullptr;

public:
    virtual void StartupModule() override
    {
        TableCreateCommand = IConsoleManager::Get().RegisterConsoleCommand(TEXT("Das.DSL.Table.Create"),
            TEXT("Das.DSL.Table.Create /Game/Types/ItemRow.ItemRow /Game/Items/DT_Items C:/Data/Items.json (new dirty asset, no overwrite/save)"),
                FConsoleCommandWithArgsDelegate::CreateLambda(
                    [](const TArray<FString>& Args)
                    {
                        FString Error, Json;
                        UDataTable* Table = nullptr;
                        if (Args.Num() != 3)
                        {
                            UE_LOG(
                                LogTemp, Warning,
                                TEXT("DAS_TABLE_CREATE rejected: expected row struct path, new table package and JSON filename"));
                            return;
                        }
                        FString TypePath = Args[0];
                        if (!TypePath.Contains(TEXT(".")) && FPackageName::IsValidLongPackageName(TypePath))
                            TypePath += TEXT(".")+ FPackageName::GetLongPackageAssetName(TypePath);
                        if (!FFileHelper::LoadFileToString(Json, *Args[2]))
                        {
                            UE_LOG(LogTemp, Warning,
                                   TEXT("DAS_TABLE_CREATE rejected: could not read JSON file %s"), *Args[2]);
                            return;
                        }
                        auto* RowType = FPackageName::IsValidObjectPath(TypePath)
                                            ? LoadObject<UScriptStruct>(nullptr, *TypePath, nullptr, LOAD_NoWarn)
                                            : nullptr;
                        if (!FDasDataTableService::CreateFromJson(RowType, Json, Args[1], Table, Error))
                        {
                            UE_LOG(LogTemp, Warning, TEXT("DAS_TABLE_CREATE rejected: %s"), *Error);
                            return;
                        }
                        UE_LOG(LogTemp, Display,
                               TEXT("DAS_TABLE_CREATE PASS %s rows=%d (dirty; save in editor)"), *Table->GetPathName(),
                                    Table->GetRowMap().Num());
                    }),
                ECVF_Default);

        AssetCheckCommand = IConsoleManager::Get().RegisterConsoleCommand(
            TEXT("Das.DSL.Asset.Check"),
                 TEXT("Das.DSL.Asset.Check /Game/Items/Potion (read-only preflight, not packaged verification)"),
                      FConsoleCommandWithArgsDelegate::CreateLambda(
                          [](const TArray<FString>& Args)
                          {
                              FString Path = Args.Num() == 1 ? Args[0] : FString();
                              if (!Path.Contains(TEXT(".")) && FPackageName::IsValidLongPackageName(Path))
                                  Path += TEXT(".")+ FPackageName::GetLongPackageAssetName(Path);
                              auto Report = FDasAssetPreflight::Inspect(
                                  FPackageName::IsValidObjectPath(Path)
                                      ? LoadObject<UDataAsset>(nullptr, *Path, nullptr, LOAD_NoWarn)
                                      : nullptr);
                              for (const auto& I : Report.Issues)
                              {
                                  UE_LOG(LogTemp, Display,
                                         TEXT("DAS_ASSET_CHECK %s %s %s: %s"), *I.Severity, *I.Code, *I.Resource,
                                              *I.Message);
                              }
                              UE_LOG(LogTemp, Display,
                                     TEXT("DAS_ASSET_CHECK_RESULT=%s packaged_verified=false"),
                                          Report.HasErrors() ? TEXT("FAIL"): TEXT("PASS"));
                          }),
                      ECVF_Default);

        AssetCreateCommand = IConsoleManager::Get().RegisterConsoleCommand(TEXT("Das.DSL.Asset.Create"),
            TEXT("Das.DSL.Asset.Create /Game/Classes/BP_Item /Game/Items/Potion (creates dirty asset; save in editor)"),
                 FConsoleCommandWithArgsDelegate::CreateLambda(
                     [](const TArray<FString>& Args)
                     {
                         FString Error;
                         UDataAsset* Asset = nullptr;
                         UBlueprint* BP = Args.Num() == 2
                             ? LoadObject<UBlueprint>(
                                   nullptr, *(Args[0] + TEXT(".")+ FPackageName::GetLongPackageAssetName(Args[0])),
                                              nullptr, LOAD_NoWarn)
                                   : nullptr;
                         if (Args.Num() != 2 || !FDasDataAssetService::Create(BP, Args[1], Asset, Error))
                         {
                             UE_LOG(LogTemp, Warning,
                                    TEXT("DAS_ASSET_CREATE rejected: %s"),
                                         Args.Num() != 2 ? TEXT("Expected Blueprint package and new asset package")
                                                         : *Error);
                         }
                         else
                         {
                             UE_LOG(LogTemp, Display,
                                    TEXT("DAS_ASSET_CREATE PASS %s (dirty; save in editor)"), *Asset->GetPathName());
                         }
                     }),
                 ECVF_Default);

        FDasInterfaceService::RegisterCompiler();
        FBlueprintCompilationManager::RegisterCompilerExtension(UBlueprint::StaticClass(),
                                                                NewObject<UDasComponentCompilerExtension>());
        InterfacesCommand = IConsoleManager::Get().RegisterConsoleCommand(
            TEXT("Das.DSL.Interfaces.Apply"),
                 TEXT("Das.DSL.Interfaces.Apply Definition.das /Game/Interfaces"),
                      FConsoleCommandWithArgsDelegate::CreateLambda(
                          [](const TArray<FString>& Args)
                          {
                              FString Error;
                              TArray<UBlueprint*> Types;
                              if (Args.Num() != 2 || !FDasInterfaceService::Apply(Args[0], Args[1], Types, Error))
                              {
                                  UE_LOG(LogTemp, Warning, TEXT("DAS_INTERFACE_APPLY rejected: %s"), *Error);
                              }
                              else
                              {
                                  UE_LOG(LogTemp, Display, TEXT("DAS_INTERFACE_APPLY PASS %d"), Types.Num());
                              }
                          }),
                      ECVF_Default);
        StructFieldsCommand = IConsoleManager::Get().RegisterConsoleCommand(
            TEXT("Das.DSL.Types.MigrateStructFields"),
                 TEXT("Das.DSL.Types.MigrateStructFields Script.das /Game/Registry.Registry [Rules.json]"),
                      FConsoleCommandWithArgsDelegate::CreateLambda(
                          [](const TArray<FString>& Args)
                          {
                              FString Error;
                              if ((Args.Num() != 2 && Args.Num() != 3) ||
                                  !FDasTypeService::MigrateStructFields(Args[0], Args[1], Error,
                                                                        Args.Num() == 3 ? Args[2] : FString()))
                                  UE_LOG(LogTemp, Warning, TEXT("DAS_STRUCT_MIGRATION rejected: %s"), *Error);
                          }),
                      ECVF_Default);

        EnumValuesCommand = IConsoleManager::Get().RegisterConsoleCommand(TEXT("Das.DSL.Types.MigrateEnumValues"),
            TEXT("Das.DSL.Types.MigrateEnumValues Script.das /Game/Registry.Registry Rules.json"),
                 FConsoleCommandWithArgsDelegate::CreateLambda(
                     [](const TArray<FString>& Args)
                     {
                         FString Error;
                         if (Args.Num() != 3 || !FDasTypeService::MigrateEnumValues(Args[0], Args[1], Args[2], Error))
                             UE_LOG(LogTemp, Warning, TEXT("DAS_ENUM_VALUES rejected: %s"), *Error);
                     }),
                 ECVF_Default);

        EnumRenameCommand = IConsoleManager::Get().RegisterConsoleCommand(
            TEXT("Das.DSL.Types.RenameEnumMembers"),
                 TEXT("Das.DSL.Types.RenameEnumMembers Script.das /Game/Registry.Registry"),
                      FConsoleCommandWithArgsDelegate::CreateLambda(
                          [](const TArray<FString>& Args)
                          {
                              FString Error;
                              if (Args.Num() != 2 || !FDasTypeService::RenameEnumMembers(Args[0], Args[1], Error))
                                  UE_LOG(LogTemp, Warning, TEXT("DAS_ENUM_RENAME rejected: %s"), *Error);
                          }),
                      ECVF_Default);

        EnumAppendCommand = IConsoleManager::Get().RegisterConsoleCommand(
            TEXT("Das.DSL.Types.ExtendEnums"),
                 TEXT("Das.DSL.Types.ExtendEnums Script.das /Game/Registry.Registry [graphs=repair]"),
                      FConsoleCommandWithArgsDelegate::CreateLambda(
                          [](const TArray<FString>& Args)
                          {
                              FString Error;
                              if ((Args.Num() != 2 && !(Args.Num() == 3 && Args[2] == TEXT("graphs=repair"))) ||
                                   !FDasTypeService::ExtendEnums(Args[0], Args[1], Error, Args.Num() == 3))
                                  UE_LOG(LogTemp, Warning, TEXT("DAS_ENUM_APPEND rejected: %s"), *Error);
                          }),
                      ECVF_Default);

        TypesCommand = IConsoleManager::Get().RegisterConsoleCommand(TEXT("Das.DSL.Types.Apply"),
            TEXT("Das.DSL.Types.Apply Script.das /Game/Folder [RegistryPackage]"),
                 FConsoleCommandWithArgsDelegate::CreateLambda(
                     [](const TArray<FString>& Args)
                     {
                         FString Error;
                         TArray<UObject*> Assets;
                         UDasTypeRegistry* Registry = nullptr;
                         if ((Args.Num() != 2 && Args.Num() != 3) ||
                             !FDasTypeService::Apply(Args[0], Args[1], Assets, Error) ||
                             (Args.Num() == 3 && !FDasTypeService::BuildRegistry(Assets, Args[2], Registry, Error)))
                             UE_LOG(LogTemp, Warning, TEXT("DAS_TYPES_APPLY rejected: %s"), *Error);
                     }),
                 ECVF_Default);
        ApplyCommand = IConsoleManager::Get().RegisterConsoleCommand(TEXT("Das.DSL.Apply"),
            TEXT("Das.DSL.Apply ScriptName.das /Game/Folder/BP_Name ClassStableId"),
                 FConsoleCommandWithArgsDelegate::CreateLambda(
                     [](const TArray<FString>& Args)
                     {
                         FString Error;
                         UBlueprint* BP = nullptr;
                         if (Args.Num() != 3 || !FDasDSLService::Apply(Args[0], Args[1], Args[2], BP, Error))
                             UE_LOG(LogTemp, Warning,
                                    TEXT("DAS_DSL_APPLY rejected: %s"),
                                         Args.Num() != 3 ? TEXT("Expected script, asset package and class ID"): *Error);
                     }),
                 ECVF_Default);
        ReimportCommand = IConsoleManager::Get().RegisterConsoleCommand(TEXT("Das.DSL.Reimport"),
            TEXT("Das.DSL.Reimport /Game/Folder/BP_Name [defaults=preserve_overrides | defaults=preserve_instances | defaults=refuse | fields=rename_by_id | fields=rename_with_dependencies | fields=widen_numeric | fields=widen_numeric_with_dependencies | fields=convert_numeric[_with_dependencies] | fields=archive_remove[_with_dependencies] | fields=archive_restore[_with_dependencies] [integers=exact|truncate|floor|ceil|nearest_away] [floats=exact|round]]"),
                FConsoleCommandWithArgsDelegate::CreateLambda(
                    [](const TArray<FString>& Args)
                    {
                        FString Error;
                        UBlueprint* BP = nullptr;
                        if (Args.Num() == 2 &&
                            (Args[1] ==
                             TEXT("fields=archive_remove") ||
                                  Args[1] == TEXT("fields=archive_remove_with_dependencies") ||
                                                  Args[1] == TEXT("fields=archive_restore") ||
                                                                  Args[1] ==
                                                                      TEXT("fields=archive_restore_with_dependencies")))
                        {
                            BP = LoadObject<UBlueprint>(
                                nullptr, *(Args[0] + TEXT(".")+ FPackageName::GetLongPackageAssetName(Args[0])),
                                           nullptr, LOAD_NoWarn);
                            if (!FDasDSLService::ReimportArchivedFields(
                                    BP, Args[1].Contains(TEXT("restore")), Args[1].EndsWith(TEXT("with_dependencies")),
                                                                                            Error))
                                UE_LOG(LogTemp, Warning, TEXT("DAS_DSL_ARCHIVE rejected: %s"), *Error);
                            return;
                        }
                        if (Args.Num() >= 2 &&
                            (Args[1] == TEXT("fields=convert_numeric") ||
                                             Args[1] == TEXT("fields=convert_numeric_with_dependencies")))
                        {
                            FDasNumericMigrationOptions O;
                            O.IncludeDependencies = Args[1].EndsWith(TEXT("with_dependencies"));
                            bool Good = Args.Num() <= 4;
                            TSet<FString> Seen;
                            for (int32 I = 2; I < Args.Num(); ++I)
                            {
                                FString Key, Value;
                                if (!Args[I].Split(TEXT("="), &Key, &Value) || Seen.Contains(Key))
                                {
                                    Good = false;
                                    break;
                                }
                                Seen.Add(Key);
                                if (Key == TEXT("integers"))
                                {
                                    if (Value == TEXT("exact"))
                                        O.Integers = EDasIntegerRule::Exact;
                                    else if (Value == TEXT("truncate"))
                                        O.Integers = EDasIntegerRule::Truncate;
                                    else if (Value == TEXT("floor"))
                                        O.Integers = EDasIntegerRule::Floor;
                                    else if (Value == TEXT("ceil"))
                                        O.Integers = EDasIntegerRule::Ceil;
                                    else if (Value == TEXT("nearest_away"))
                                        O.Integers = EDasIntegerRule::NearestAway;
                                    else
                                        Good = false;
                                }
                                else if (Key == TEXT("floats"))
                                {
                                    if (Value == TEXT("exact"))
                                        O.Floats = EDasFloatRule::Exact;
                                    else if (Value == TEXT("round"))
                                        O.Floats = EDasFloatRule::RoundNearest;
                                    else
                                        Good = false;
                                }
                                else
                                    Good = false;
                            }
                            if (Good)
                                BP = LoadObject<UBlueprint>(
                                    nullptr, *(Args[0] + TEXT(".")+ FPackageName::GetLongPackageAssetName(Args[0])),
                                               nullptr, LOAD_NoWarn);
                            if (!Good || !FDasDSLService::ReimportConvertedNumericFields(BP, O, Error))
                                UE_LOG(LogTemp, Warning,
                                       TEXT("DAS_DSL_CONVERT rejected: %s"),
                                            Good ? *Error : TEXT("Unknown or repeated rounding policy"));
                            return;
                        }
                        if (Args.Num() == 1 || Args.Num() == 2)
                            BP = LoadObject<UBlueprint>(
                                nullptr, *(Args[0] + TEXT(".")+ FPackageName::GetLongPackageAssetName(Args[0])),
                                           nullptr, LOAD_NoWarn);
                        const bool Valid =
                            Args.Num() == 1 ||
                            (Args.Num() == 2 &&
                             (Args[1] ==
                              TEXT("defaults=preserve_instances") ||
                                  Args[1] ==
                                      TEXT("defaults=preserve_overrides") ||
                                          Args[1] ==
                                              TEXT("defaults=refuse") ||
                                                  Args[1] ==
                                                      TEXT("fields=rename_by_id") ||
                                                          Args[1] ==
                                                              TEXT("fields=rename_with_dependencies") ||
                                                                  Args[1] ==
                                                                      TEXT("fields=widen_numeric") ||
                                                                          Args[1] ==
                                                                              TEXT("fields=widen_numeric_with_dependencies")));
                        if (Valid && Args.Num() == 2 &&
                            (Args[1] == TEXT("fields=widen_numeric") ||
                                             Args[1] == TEXT("fields=widen_numeric_with_dependencies")))
                        {
                            if (!FDasDSLService::ReimportWidenedNumericFields(
                                    BP, Error, Args[1] == TEXT("fields=widen_numeric_with_dependencies")))
                                UE_LOG(LogTemp, Warning, TEXT("DAS_DSL_WIDEN rejected: %s"), *Error);
                            return;
                        }
                        const auto DefaultPolicy =
                            Args.Num() == 1
                                ? EDasDefaultMigrationPolicy::Automatic
                                : Args.Num() == 2 &&
                                      Args[1] == TEXT("defaults=preserve_instances")
                                                      ? EDasDefaultMigrationPolicy::PreserveInstances
                                                      : Args.Num() == 2 &&
                                                            Args[1] ==
                                                                TEXT("defaults=preserve_overrides")
                                                                     ? EDasDefaultMigrationPolicy::PreserveOverrides
                                                                     : EDasDefaultMigrationPolicy::Refuse;
                        if (!Valid ||
                            !(Args.Num() == 2 && (Args[1] == TEXT("fields=rename_by_id") ||
                                                                  Args[1] == TEXT("fields=rename_with_dependencies"))
                                                  ? FDasDSLService::ReimportRenamedFields(
                                                        BP, Error, Args[1] == TEXT("fields=rename_with_dependencies"))
                                                        : FDasDSLService::Reimport(BP, Error, DefaultPolicy)))
                            UE_LOG(
                                LogTemp, Warning,
                                TEXT("DAS_DSL_REIMPORT rejected: %s"), !Valid
                                    ? TEXT("Expected asset package and optional defaults=preserve_overrides, defaults=preserve_instances, defaults=refuse or field migration policy")
                                    : *Error);
                    }),
                ECVF_Default);
    }
    virtual void ShutdownModule() override
    {
        if (TableCreateCommand)
            IConsoleManager::Get().UnregisterConsoleObject(TableCreateCommand);
        if (AssetCheckCommand)
            IConsoleManager::Get().UnregisterConsoleObject(AssetCheckCommand);
        if (AssetCreateCommand)
            IConsoleManager::Get().UnregisterConsoleObject(AssetCreateCommand);
        if (InterfacesCommand)
            IConsoleManager::Get().UnregisterConsoleObject(InterfacesCommand);
        if (StructFieldsCommand)
            IConsoleManager::Get().UnregisterConsoleObject(StructFieldsCommand);
        if (EnumValuesCommand)
            IConsoleManager::Get().UnregisterConsoleObject(EnumValuesCommand);
        if (EnumRenameCommand)
            IConsoleManager::Get().UnregisterConsoleObject(EnumRenameCommand);
        if (EnumAppendCommand)
            IConsoleManager::Get().UnregisterConsoleObject(EnumAppendCommand);
        if (TypesCommand)
            IConsoleManager::Get().UnregisterConsoleObject(TypesCommand);
        if (ApplyCommand)
            IConsoleManager::Get().UnregisterConsoleObject(ApplyCommand);
        if (ReimportCommand)
            IConsoleManager::Get().UnregisterConsoleObject(ReimportCommand);
    }
};
IMPLEMENT_MODULE(FDasEditorModule, DasEditor)
