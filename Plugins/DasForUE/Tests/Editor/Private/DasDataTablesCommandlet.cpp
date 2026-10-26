#include "DasDataTablesCommandlet.h"
#include "DasTypeService.h"
#include "DasDataTableService.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "DasRuntime.h"
#include "DasSchema.h"
#include "UObject/Package.h"
#include "Misc/Paths.h"
#include "Misc/PackageName.h"
#include "Engine/DataTable.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
#include "UObject/UnrealType.h"
#include "StructUtils/UserDefinedStruct.h"
#include "StructUtils/InstancedStruct.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"

UDasDataTablesCommandlet::UDasDataTablesCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasDataTablesCommandlet::Main(const FString& Params)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FString Error;
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool Good, const TCHAR* Label)
    {
        ++Checks;
        if (!Good)
            ++Failures;
        UE_LOG(LogTemp, Display, TEXT("DAS_DATA_TABLES_%s %s"), Good ? TEXT("PASS"): TEXT("FAIL"), Label);
        return Good;
    };
    const FString Tag = FGuid::NewGuid().ToString(EGuidFormats::Digits),
                  Folder = TEXT("/Game/Tests/DasDataTables/")+ Tag;
    const FString ExampleFile = TEXT("datatable_example_probe_")+ Tag +
                                     TEXT(".das"), ProbeFile = TEXT("datatable_runtime_probe_")+ Tag + TEXT(".das");
    const FString ExamplePath = R.ScriptPath(ExampleFile), ProbePath = R.ScriptPath(ProbeFile);
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*ExamplePath, false, true);
        IFileManager::Get().Delete(*ProbePath, false, true);
    };
    TArray<UObject*> Types;
    if (!Check(FDasTypeService::Apply(TEXT("datatable_item_types.das"), Folder, Types, Error) && Types.Num() == 1,
                                      TEXT("authored_item_row_schema")))
    {
        UE_LOG(LogTemp, Display, TEXT("%s"), *Error);
        return 1;
    }
    auto* RowType = Cast<UScriptStruct>(Types[0]);
    if (!RowType)
        return 1;
    TStrongObjectPtr<UDataTable> Table;
    ON_SCOPE_EXIT
    {
        if (Table.IsValid())
        {
            R.ReleaseObject(Table.Get());
            FAssetRegistryModule::AssetDeleted(Table.Get());
            Table->GetOutermost()->SetDirtyFlag(false);
            Table->ClearFlags(RF_Public | RF_Standalone);
        }
        for (auto* T : Types)
        {
            FAssetRegistryModule::AssetDeleted(T);
            T->GetOutermost()->SetDirtyFlag(false);
            T->ClearFlags(RF_Standalone);
        }
    };
    auto* Label = CastField<FStrProperty>(FindDasStructField(RowType, TEXT("Label")));
    auto* Price = CastField<FIntProperty>(FindDasStructField(RowType, TEXT("Price")));
    auto* Weight = CastField<FDoubleProperty>(FindDasStructField(RowType, TEXT("Weight")));
    if (!Check(Label && Price && Weight, TEXT("reflected_item_fields")))
        return 1;
    FString ItemsFile, Json;
    if (!FParse::Value(*Params, TEXT("ItemsJson="), ItemsFile) || !FFileHelper::LoadFileToString(Json, *ItemsFile))
        return 2;
    auto Reject = [&](const FString& Input, const TCHAR* LabelText)
    {
        const FString BadPackage = Folder + TEXT("/Rejected_")+ FString::FromInt(Checks);
        UDataTable* Rejected = nullptr;
        Error.Empty();
        const bool Refused = !FDasDataTableService::CreateFromJson(RowType, Input, BadPackage, Rejected, Error);
        Check(Refused && !Error.IsEmpty() && !Rejected && !FindPackage(nullptr, *BadPackage) &&
                  !FPackageName::DoesPackageExist(BadPackage),
              LabelText);
        if (Rejected)
        {
            FAssetRegistryModule::AssetDeleted(Rejected);
            Rejected->GetOutermost()->SetDirtyFlag(false);
            Rejected->ClearFlags(RF_Public | RF_Standalone);
        }
    };
    Reject(TEXT("{"), TEXT("malformed_json_no_asset_created"));
    Reject(TEXT("[42]"), TEXT("non_object_row_no_asset_created"));
    Reject(TEXT("[{\"Name\":3,\"Label\":\"Bad\",\"Price\":1,\"Weight\":1}]"),
                TEXT("non_string_row_key_refused_without_engine_error"));
    Reject(Json.Replace(TEXT("\"Price\":25"), TEXT("\"Price\":\"oops\"")), TEXT("wrong_scalar_type_no_asset_created"));
    Reject(Json.Replace(TEXT("\"Price\":25"), TEXT("\"Price\":2147483648")), TEXT("integer_overflow_no_asset_created"));
    Reject(Json.Replace(TEXT("\"Price\":25"), TEXT("\"Price\":25.5")), TEXT("fractional_integer_no_asset_created"));
    Reject(Json.Replace(TEXT("\"Price\":25,"), TEXT("")), TEXT("missing_column_no_asset_created"));
    Reject(Json.Replace(TEXT("\"Price\":25"), TEXT("\"Extra\":1,\"Price\":25")),
                        TEXT("extra_column_native_import_diagnostics_no_asset_created"));
    Reject(Json.Replace(TEXT("\"Name\":\"Sword\""), TEXT("\"Name\":\"potion\"")),
                        TEXT("case_insensitive_duplicate_row_refused"));
    UDataTable* Created = nullptr;
    Error.Empty();
    const FString TablePackage = Folder + TEXT("/DT_Items");
    if (!Check(FDasDataTableService::CreateFromJson(RowType, Json, TablePackage, Created, Error) && Created &&
                   Created->GetRowStruct() == RowType && Created->GetRowMap().Num() == 2 &&
                   Created->GetOutermost()->IsDirty(),
               TEXT("create_dirty_authored_table_from_public_json")))
    {
        UE_LOG(LogTemp, Display, TEXT("%s"), *Error);
        return 1;
    }
    Table.Reset(Created);
    UDataTable* Duplicate = nullptr;
    Error.Empty();
    Check(!FDasDataTableService::CreateFromJson(RowType, TEXT("[]"), TablePackage, Duplicate, Error) && !Duplicate &&
                                                Table->GetRowMap().Num() == 2,
                                                TEXT("existing_asset_never_overwritten"));
    const FString EmptyPackage = Folder + TEXT("/DT_Empty");
    UDataTable* EmptyAsset = nullptr;
    Error.Empty();
    const bool EmptyCreated =
        FDasDataTableService::CreateFromJson(RowType, TEXT("[]"), EmptyPackage, EmptyAsset, Error);
    Check(EmptyCreated && EmptyAsset && EmptyAsset->GetRowMap().IsEmpty(),
          TEXT("empty_json_array_creates_empty_table"));
    if (EmptyAsset)
    {
        FAssetRegistryModule::AssetDeleted(EmptyAsset);
        EmptyAsset->GetOutermost()->SetDirtyFlag(false);
        EmptyAsset->ClearFlags(RF_Public | RF_Standalone);
    }
    FString Example, Probe, Fixture;
    if (!FParse::Value(*Params, TEXT("Fixture="), Fixture) || !FFileHelper::LoadFileToString(Probe, *Fixture) ||
                       !FFileHelper::LoadFileToString(Example, *R.ScriptPath(TEXT("datatable_items.das"))))
        return 2;
    Example.ReplaceInline(TEXT("/Game/ItemTypes"), *Folder);
    Probe.ReplaceInline(TEXT("__FOLDER__"), *Folder);
    Probe.ReplaceInline(TEXT("__EXAMPLE__"), *FPaths::GetBaseFilename(ExampleFile));
    if (!FFileHelper::SaveStringToFile(Example, *ExamplePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
        !FFileHelper::SaveStringToFile(Probe, *ProbePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return 2;
    auto Call = [&](const TCHAR* Entry)
    {
        auto V = R.RunNamedPersistent(ProbePath, Entry, 0, Table.Get());
        if (!V.bSucceeded)
            UE_LOG(LogTemp, Display, TEXT("DataTable VM: %s"), *V.Error);
        return V;
    };
    const auto Read = Call(TEXT("start"));
    Check(Read.bSucceeded && Read.Value == 1, TEXT("typed_rows_names_missing_and_wrong_type"));
    Table->RemoveRow(TEXT("Potion"));
    CollectGarbage(RF_NoFlags);
    const auto Retained = Call(TEXT("after_mutation"));
    Check(Retained.bSucceeded && Retained.Value == 1, TEXT("owned_row_survives_native_removal_and_gc"));
    Table->EmptyTable();
    const auto Empty = Call(TEXT("empty_table"));
    Check(Empty.bSucceeded && Empty.Value == 1, TEXT("empty_name_list"));
    UE_LOG(LogTemp, Display,
           TEXT("DAS_DATA_TABLES_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
