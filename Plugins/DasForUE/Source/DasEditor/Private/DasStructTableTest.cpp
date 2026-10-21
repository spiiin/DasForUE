// Struct migration with DataTables in scope: rows are captured by field ID and restored after rename, add, drop
// and a failed (rolled back) change. The row struct is used only by the table, so no class is recompiled.
#include "DasDataTableService.h"
#include "DasRuntime.h"
#include "DasSchema.h"
#include "DasTypeRegistry.h"
#include "DasTypeService.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/DataTable.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/ScopeExit.h"
#include "UObject/SavePackage.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
void DasFailStructMigrationForTesting(int32);
namespace DasStructTable
{
bool Save(UObject* O)
{
    const FString File =
        FPackageName::LongPackageNameToFilename(O->GetOutermost()->GetName(), FPackageName::GetAssetPackageExtension());
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public | RF_Standalone;
    Args.SaveFlags = SAVE_NoError;
    if (!UPackage::SavePackage(O->GetOutermost(), O, *File, Args))
        return false;
    FAssetRegistryModule::GetRegistry().ScanFilesSynchronous({File}, true);
    return true;
}
FString RowScript(const FString& Module, bool Renamed, bool Stock, bool Label)
{
    FString S = FString::Printf(TEXT("options gen2\nrequire ue_dsl\n"
                                     "[ue_struct(id=\"%s.Row\", module_id=\"%s\", BlueprintType)]\nstruct MigRow {\n"),
                                     *Module, *Module);
    if (Label)
        S += FString::Printf(TEXT("    @ue_property=(EditAnywhere, BlueprintReadWrite)\n    @ue_id=\"%s.Row.Label\"\n"
                                  "    Label : string = \"\"\n"),
                                  *Module);
    S += FString::Printf(TEXT("    @ue_property=(EditAnywhere, BlueprintReadWrite)\n    @ue_id=\"%s.Row.Price\"\n"
                              "    %s : int = 0\n"),
                              *Module, Renamed ? TEXT("Cost") : TEXT("Price"));
    if (Stock)
        S += FString::Printf(TEXT("    @ue_property=(EditAnywhere, BlueprintReadWrite)\n    @ue_id=\"%s.Row.Stock\"\n"
                                  "    Stock : int = 4\n"),
                                  *Module);
    return S + TEXT("}\n");
}
FString Cell(UDataTable* T, const TCHAR* Row, const TCHAR* Field)
{
    const uint8* Memory = T ? T->FindRowUnchecked(Row) : nullptr;
    FProperty* P = Memory ? FindDasStructField(const_cast<UScriptStruct*>(T->GetRowStruct()), Field) : nullptr;
    FString Out;
    if (P)
        P->ExportTextItem_Direct(Out, P->ContainerPtrToValuePtr<void>(Memory), nullptr, T, PPF_None);
    return P ? Out : TEXT("<missing>");
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasStructDataTableTest, "DasForUE.Types.StructDataTable",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasStructDataTableTest::RunTest(const FString&)
{
    using namespace DasStructTable;
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Tag = FGuid::NewGuid().ToString(EGuidFormats::Digits).Left(12).ToLower();
    const FString Module = TEXT("tablemig") + Tag,
                                Script = TEXT("struct_table_") + Tag +
                                              TEXT(".das"),
                                                   Rules = TEXT("struct_table_") + Tag +
                                                                TEXT(".json"),
                                                                     Folder = TEXT("/Game/DasStructTable/") + Tag;
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*R.ScriptPath(Script));
        IFileManager::Get().Delete(*R.ScriptPath(Rules));
        DasFailStructMigrationForTesting(0);
    };
    FString Error;
    TArray<UObject*> Types;
    UDasTypeRegistry* Registry = nullptr;
    UDataTable* Table = nullptr;
    if (!TestTrue(
            TEXT("write v1"),
                 FFileHelper::SaveStringToFile(RowScript(Module, false, false, true), *R.ScriptPath(Script))) ||
            !TestTrue(
                TEXT("import row struct"), FDasTypeService::Apply(Script, Folder, Types, Error) && Types.Num() == 1) ||
                !TestTrue(TEXT("registry"), FDasTypeService::BuildRegistry(Types, Folder + TEXT("/Registry"), Registry, Error)) ||
                    !TestTrue(TEXT("create table"), FDasDataTableService::CreateFromJson(
                                                        Cast<UScriptStruct>(Types[0]),
                                                        TEXT("[{\"Name\":\"Potion\",\"Label\":\"Heal\",\"Price\":3},"
                           "{\"Name\":\"Sword\",\"Label\":\"Cut\",\"Price\":25}]"),
                                                             Folder + TEXT("/DT_Items"), Table, Error)) ||
                                                        !TestTrue(TEXT("save assets"),
                                                                       Save(Types[0]) && Save(Registry) && Save(Table)))
    {
        AddInfo(Error);
        return false;
    }
    TStrongObjectPtr<UDataTable> Keep(Table);
    const FString RegistryPath = Registry->GetPathName();
    auto Migrate = [&](const FString& Text, const FString& RulesText = FString())
    {
        Error.Reset();
        if (!RulesText.IsEmpty())
            FFileHelper::SaveStringToFile(RulesText, *R.ScriptPath(Rules));
        return FFileHelper::SaveStringToFile(Text, *R.ScriptPath(Script)) &&
               FDasTypeService::MigrateStructFields(Script, RegistryPath, Error,
                                                    RulesText.IsEmpty() ? FString() : Rules);
    };

    // Rename (same ID) and add: values follow the field ID, the new field gets its default.
    TestTrue(TEXT("rename and add migrate"), Migrate(RowScript(Module, true, true, true)));
    AddInfo(Error);
    TestEqual(TEXT("renamed value kept"), Cell(Table, TEXT("Sword"), TEXT("Cost")), FString(TEXT("25")));
    TestEqual(TEXT("other value kept"), Cell(Table, TEXT("Potion"), TEXT("Label")), FString(TEXT("Heal")));
    TestEqual(TEXT("added field default"), Cell(Table, TEXT("Potion"), TEXT("Stock")), FString(TEXT("4")));
    TestTrue(TEXT("table marked dirty"), Table->GetOutermost()->IsDirty());

    // A failure after the struct compiled rolls the struct and the rows back.
    DasFailStructMigrationForTesting(1);
    TestFalse(TEXT("injected failure refused"), Migrate(RowScript(Module, false, true, true)));
    DasFailStructMigrationForTesting(0);
    TestTrue(TEXT("rollback message"), Error.Contains(TEXT("rolled back")));
    TestEqual(TEXT("rows restored after rollback"), Cell(Table, TEXT("Sword"), TEXT("Cost")), FString(TEXT("25")));

    // Drop needs an explicit rule; the other values stay.
    TestFalse(TEXT("drop without rule refused"), Migrate(RowScript(Module, true, true, false)));
    TestTrue(TEXT("drop with rule migrates"),
                  Migrate(RowScript(Module, true, true, false),
                          FString::Printf(TEXT("{\"schema_version\":1,\"rules\":[{\"field_id\":\"%s.Row.Label\","
                                          "\"operation\":\"drop\"}]}"),
                                               *Module)));
    AddInfo(Error);
    TestEqual(TEXT("dropped field gone"), Cell(Table, TEXT("Potion"), TEXT("Label")), FString(TEXT("<missing>")));
    TestEqual(TEXT("values kept after drop"), Cell(Table, TEXT("Potion"), TEXT("Cost")), FString(TEXT("3")));
    TestEqual(TEXT("row count"), Table->GetRowMap().Num(), 2);
    return true;
}
#endif
