// Auto-reimport asks before a field migration that also modifies a saved map: the plan lists the changed fields
// and the map, Cancel leaves everything as it is, Apply migrates the class and the placed actor (default value
// first, then the removal). Long plans are summarized.
#include "DasAutoReimport.h"
#include "DasDSLService.h"
#include "DasActor.h"
#include "DasRuntime.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Blueprint.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "UObject/SavePackage.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
namespace DasReimportConfirm
{
FString HostScript(const FString& Module, bool Scaler, int32 Points)
{
    FString S =
        FString::Printf(TEXT("options gen2\nrequire ue_dsl\n"
                                     "[ue_class(module_id=\"%s\",id=\"%s.host\",parent=\"/Script/Engine.Actor\","
                                     "Blueprintable)]\nclass ConfirmHost {\n"
                                     "    @ue_property=(EditAnywhere,BlueprintReadWrite)\n    @ue_id=\"%s.host.points\"\n"
                                     "    Points : int = %d\n"),
                             *Module, *Module, *Module, Points);
    if (Scaler)
        S += FString::Printf(TEXT("    @ue_property=(EditAnywhere,BlueprintReadWrite)\n    @ue_id=\"%s.host.scaler\"\n"
                                  "    Scaler : float = 1.0f\n"),
                                  *Module);
    return S + FString::Printf(TEXT("    [ue_function(id=\"%s.host.keep\",BlueprintCallable)]\n"
                                    "    def Keep():int { return 3 }\n}\n"),
                                    *Module);
}
FString Value(UObject* O, const TCHAR* Name)
{
    FProperty* P = O ? O->GetClass()->FindPropertyByName(Name) : nullptr;
    FString Out;
    if (P)
        P->ExportTextItem_Direct(Out, P->ContainerPtrToValuePtr<void>(O), nullptr, O, PPF_None);
    return P ? Out : TEXT("<missing>");
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasReimportConfirmTest, "DasForUE.Reload.ConfirmDependencies",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasReimportConfirmTest::RunTest(const FString&)
{
    using namespace DasReimportConfirm;
    // Long plans: a count instead of every field and asset.
    {
        FDasReimportPlan P;
        for (int32 I = 0; I < 60; ++I)
            P.Removed.Add(FString::Printf(TEXT("Field%d"), I));
        P.Defaults.Add(TEXT("Points: 10 -> 20"));
        for (int32 I = 0; I < 5; ++I)
            P.Maps.Add(FString::Printf(TEXT("/Game/Maps/Map%d"), I));
        TestEqual(TEXT("summary counts"), P.Summary(), FString(TEXT("Removed 60, new defaults 1")));
        const FString Short = P.Short();
        TestTrue(TEXT("short lists five fields"), Short.Contains(TEXT("Field4")) && !Short.Contains(TEXT("Field5:")));
        TestTrue(TEXT("short counts the rest"), Short.Contains(TEXT("...and 56 more")));
        TestTrue(TEXT("short counts assets"), Short.Contains(TEXT("Will modify 5 assets: 5 maps, 0 Blueprints")));
        TestTrue(TEXT("details cap each group"), P.Details(10).Contains(TEXT("...and 50 more")));
    }

    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Tag = FGuid::NewGuid().ToString(EGuidFormats::Digits).Left(12).ToLower();
    const FString Module = TEXT("confirm") + Tag, Script = TEXT("reimport_confirm_") + Tag +
                                                                TEXT(".das"), Folder = TEXT("/Game/DasConfirm/") + Tag,
                                                                                            Path = R.ScriptPath(Script);
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*Path);
    };
    FString Error;
    UBlueprint* BP = nullptr;
    if (!TestTrue(
            TEXT("write v1"), FFileHelper::SaveStringToFile(HostScript(Module, true, 10), *Path)) ||
            !TestTrue(TEXT("apply v1"),
                           FDasDSLService::Apply(Script, Folder + TEXT("/BP_Host"), Module + TEXT(".host"), BP, Error)))
    {
        AddInfo(Error);
        return false;
    }
    TStrongObjectPtr<UBlueprint> KeepBP(BP);
    // A saved map with a placed instance: a saved referencer outside the class.
    UWorld* World =
        UWorld::CreateWorld(EWorldType::Editor, false, TEXT("Map"), CreatePackage(*(Folder + TEXT("/Map"))));
    TStrongObjectPtr<UWorld> KeepWorld(World);
    World->SetFlags(RF_Public | RF_Standalone);
    GEngine->CreateNewWorldContext(EWorldType::Editor).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
        World->ClearFlags(RF_Public | RF_Standalone);
    };
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("Placed");
    AActor* Placed = World->SpawnActor<AActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
    TestTrue(TEXT("override instance value"), Placed && Placed->GetClass()->FindPropertyByName(TEXT("Scaler")));
    if (auto* P = Placed ? Placed->GetClass()->FindPropertyByName(TEXT("Scaler")) : nullptr)
        P->ImportText_Direct(TEXT("3.0"), P->ContainerPtrToValuePtr<void>(Placed), Placed, PPF_None);
    for (UObject* Asset : {static_cast<UObject*>(BP), static_cast<UObject*>(World)})
    {
        const FString File = FPackageName::LongPackageNameToFilename(
            Asset->GetOutermost()->GetName(),
            Asset->IsA<UWorld>() ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension());
        FSavePackageArgs Args;
        Args.TopLevelFlags = RF_Public | RF_Standalone;
        Args.SaveFlags = SAVE_NoError;
        TestTrue(TEXT("save ") + Asset->GetName(), UPackage::SavePackage(Asset->GetOutermost(), Asset, *File, Args));
        FAssetRegistryModule::GetRegistry().ScanFilesSynchronous({File}, true);
    }
    auto FindPlaced = [&]()
    {
        for (TActorIterator<AActor> It(World); It; ++It)
            if (It->GetFName() == TEXT("Placed"))
                return *It;
        return static_cast<AActor*>(nullptr);
    };

    // One save removes Scaler and changes the Points default.
    TestTrue(TEXT("write v2"), FFileHelper::SaveStringToFile(HostScript(Module, false, 20), *Path));
    FDasReimportPlan Plan;
    TestTrue(TEXT("plan"), FDasDSLService::PlanReimport(BP, Plan, Error));
    TestTrue(TEXT("plan needs confirmation"), Plan.NeedsConfirmation);
    TestEqual(TEXT("plan removes Scaler"), Plan.Removed, TArray<FString>{TEXT("Scaler")});
    TestEqual(TEXT("plan changes the Points default"), Plan.Defaults, TArray<FString>{TEXT("Points: 10 -> 20")});
    TestTrue(TEXT("plan lists the map"), Plan.Maps.Contains(Folder + TEXT("/Map")));

    FString Full = FPaths::ConvertRelativePathToFull(Path);
    FPaths::NormalizeFilename(Full);
    FDasAutoReimport Auto;
    Auto.ReimportFiles({Full});
    TestEqual(TEXT("waits for confirmation"), Auto.PendingConfirmations(), 1);
    TestTrue(TEXT("nothing changed while waiting"), BP->GeneratedClass->FindPropertyByName(TEXT("Scaler")) != nullptr);

    Auto.Decline(BP->GetPathName());
    TestEqual(TEXT("cancel closes the question"), Auto.PendingConfirmations(), 0);
    TestTrue(TEXT("cancel keeps the class"), BP->GeneratedClass->FindPropertyByName(TEXT("Scaler")) != nullptr);

    Auto.ReimportFiles({Full});
    TestEqual(TEXT("asks again on the next save"), Auto.PendingConfirmations(), 1);
    Auto.Confirm(BP->GetPathName());
    TestEqual(TEXT("apply closes the question"), Auto.PendingConfirmations(), 0);
    TestTrue(TEXT("field removed"), BP->GeneratedClass->FindPropertyByName(TEXT("Scaler")) == nullptr);
    TestEqual(TEXT("new default on the class"), Value(BP->GeneratedClass->GetDefaultObject(), TEXT("Points")),
                                                      FString(TEXT("20")));
    auto* Actor = Cast<ADasActor>(FindPlaced());
    TestTrue(TEXT("placed actor migrated"),
                  Actor && Value(Actor, TEXT("Points")) == TEXT("20") &&
                                                                Value(Actor, TEXT("Scaler")) == TEXT("<missing>"));
    TestTrue(TEXT("removed value archived"), Actor && Actor->RetiredDasValues.Num() == 1);
    return true;
}
#endif
