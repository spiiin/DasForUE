#include "DasAuthoredAssetsCommandlet.h"
#include "DasAssetPreflight.h"
#include "DasDSLService.h"
#include "DasDataAssetService.h"
#include "DasDataAssets.h"
#include "DasAuthoring.h"
#include "DasRuntime.h"
#include "DasSoftReferences.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/Package.h"
#include "UObject/MetaData.h"
#include "UObject/SavePackage.h"
#include "UObject/Linker.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
#include "UObject/UnrealType.h"
#include "Misc/PackageName.h"
#include "Misc/ScopeExit.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#include "Async/TaskGraphInterfaces.h"
#include "Tickable.h"
#include "Containers/Ticker.h"

UDasAuthoredAssetsCommandlet::UDasAuthoredAssetsCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasAuthoredAssetsCommandlet::Main(const FString& Params)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    auto& M = UAssetManager::Get();
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool Good, const TCHAR* Label)
    {
        ++Checks;
        if (!Good)
            ++Failures;
        UE_LOG(LogTemp, Display, TEXT("DAS_AUTHORED_ASSETS_%s %s"), Good ? TEXT("PASS"): TEXT("FAIL"), Label);
        return Good;
    };
    FString Error, Author;
    FDasAuthoringModule PublicModel;
    if (!Check(R.ReadAuthoringScript(R.ScriptPath(TEXT("dataasset_items.das")), PublicModel, Error),
                                     TEXT("public_dataasset_dsl_example")))
    {
        UE_LOG(LogTemp, Display, TEXT("%s"), *Error);
        return 1;
    }
    if (!FFileHelper::LoadFileToString(Author, *R.ScriptPath(TEXT("dataasset_items.das"))))
        return 2;
    const FString Tag = FGuid::NewGuid().ToString(EGuidFormats::Digits),
                  Type = TEXT("DasAuthoredAssets_")+ Tag, Base = TEXT("/Game/Tests/DasAuthoredAssets/")+ Tag;
    const FString AuthorFile = TEXT("dataasset_author_probe_")+ Tag +
                                    TEXT(".das"),
                                         AuthorPath = R.ScriptPath(AuthorFile),
                                         ProbePath = R.ScriptPath(TEXT("dataasset_usage_probe_")+ Tag + TEXT(".das"));
    Author.ReplaceInline(TEXT("primary_asset_type=\"Item\""), *(TEXT("primary_asset_type=\"")+ Type + TEXT("\"")));
    if (!FFileHelper::SaveStringToFile(Author, *AuthorPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return 2;
    TArray<FString> Files, Packages;
    TArray<TWeakObjectPtr<UObject>> Created;
    ON_SCOPE_EXIT
    {
        M.RemovePrimaryAssetType(FPrimaryAssetType(*Type));
        FlushAsyncLoading();
        for (const auto& P : Packages)
            if (auto* Package = FindPackage(nullptr, *P))
                ResetLoaders(Package);
        for (auto O : Created)
            if (O.IsValid())
                FAssetRegistryModule::AssetDeleted(O.Get());
        IFileManager::Get().Delete(*AuthorPath);
        IFileManager::Get().Delete(*ProbePath);
        for (const auto& File : Files)
            IFileManager::Get().Delete(*File);
    };
    FDasAuthoringModule Model;
    if (!R.ReadAuthoringScript(AuthorPath, Model, Error))
    {
        UE_LOG(LogTemp, Display, TEXT("%s"), *Error);
        return 1;
    }
    const auto* ItemDecl =
        Model.Classes.FindByPredicate([](const auto& C) { return C.Name == TEXT("ItemDefinition"); });
    const auto* PlainDecl =
        Model.Classes.FindByPredicate([](const auto& C) { return C.Name == TEXT("TuningDefinition"); });
    if (!ItemDecl || !PlainDecl)
        return 1;
    UBlueprint* ItemBP = nullptr;
    UBlueprint* PlainBP = nullptr;
    if (!Check(FDasDSLService::Apply(AuthorFile, Base + TEXT("/Classes/BP_Item"), ItemDecl->Id, ItemBP, Error) &&
                                     FDasDSLService::Apply(
                                         AuthorFile, Base + TEXT("/Classes/BP_Tuning"), PlainDecl->Id, PlainBP, Error),
                                         TEXT("import_primary_and_data_only_blueprints")))
    {
        UE_LOG(LogTemp, Display, TEXT("%s"), *Error);
        return 1;
    }
    TStrongObjectPtr<UBlueprint> KeepItem(ItemBP), KeepPlain(PlainBP);
    FString ChangedAuthor = Author;
    ChangedAuthor.ReplaceInline(TEXT("Score:int=10"), TEXT("Score:int=11"));
    if (!FFileHelper::SaveStringToFile(ChangedAuthor, *AuthorPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return 2;
    UBlueprint* Reimported = nullptr;
    const bool Rejected =
        !FDasDSLService::Apply(AuthorFile, Base + TEXT("/Classes/BP_Item"), ItemDecl->Id, Reimported, Error,
                                                       EDasDefaultMigrationPolicy::PreserveOverrides);
    Check(Rejected &&
          Error.Contains(TEXT("migration is deferred")) &&
                         FindFProperty<FIntProperty>(
                             ItemBP->GeneratedClass,
                             TEXT("Score"))->GetPropertyValue_InContainer(ItemBP->GeneratedClass->GetDefaultObject()) ==
                             10,
                             TEXT("declaration_update_rejected_before_default_migration"));
    if (!FFileHelper::SaveStringToFile(Author, *AuthorPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return 2;

    auto Save = [&](UObject* Object)
    {
        const auto Package = Object->GetOutermost()->GetName();
        const FString File = FPackageName::LongPackageNameToFilename(Package, FPackageName::GetAssetPackageExtension());
        check(Package.StartsWith(Base + TEXT("/")));
        Files.AddUnique(File);
        Packages.AddUnique(Package);
        Created.Add(Object);
        IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
        FSavePackageArgs Args;
        Args.TopLevelFlags = RF_Public | RF_Standalone;
        return UPackage::SavePackage(Object->GetOutermost(), Object, *File, Args);
    };
    UDataAsset* Plain = nullptr;
    UDataAsset* Root = nullptr;
    if (!Check(FDasDataAssetService::Create(PlainBP, Base + TEXT("/Items/Presentation"), Plain, Error) &&
                                            FDasDataAssetService::Create(
                                                ItemBP, Base + TEXT("/Items/Potion"), Root, Error),
                                                TEXT("editor_creates_authored_asset_instances")))
    {
        UE_LOG(LogTemp, Display, TEXT("%s"), *Error);
        return 1;
    }
    auto* Primary = CastChecked<UDasPrimaryDataAsset>(Root);
    const FPrimaryAssetId Id = Primary->GetPrimaryAssetId();
    Check(Id == FPrimaryAssetId(FPrimaryAssetType(*Type), TEXT("Potion")) && !Plain->GetPrimaryAssetId().IsValid(),
                                TEXT("primary_id_uses_persisted_type_and_instance_name"));
    UDataAsset* Duplicate = nullptr;
    Check(!FDasDataAssetService::Create(ItemBP, Base + TEXT("/Items/Potion"), Duplicate, Error) && !Duplicate,
                                        TEXT("editor_creation_never_overwrites_existing_assets"));
    FindFProperty<FIntProperty>(Root->GetClass(), TEXT("Score"))->SetPropertyValue_InContainer(Root, 73);
    auto* Icon = FindFProperty<FSoftObjectProperty>(Root->GetClass(), TEXT("Icon"));
    auto* Soft = Icon;
    if (!Check(Soft && Icon->GetMetaData(TEXT("AssetBundles")) ==
                                         TEXT("UI"), TEXT("bundle_metadata_is_on_reflected_soft_field")))
        return 1;
    const FString RootPath = Root->GetPathName(), BundlePath = Plain->GetPathName();
    Soft->SetPropertyValue(Soft->ContainerPtrToValuePtr<void>(Root), FSoftObjectPtr(FSoftObjectPath(BundlePath)));
    Primary->UpdateAssetBundleData();
    FAssetBundleData Built;
    M.InitializeAssetBundlesFromMetadata(Primary, Built);
    Check(Built.FindEntry(TEXT("UI")) &&
                          Built.FindEntry(TEXT("UI"))->AssetPaths.Contains(FSoftObjectPath(BundlePath).GetAssetPath()),
                                          TEXT("native_bundle_builder_reads_dsl_metadata_and_soft_reference"));
    if (!Check(Save(ItemBP) && Save(PlainBP) && Save(Plain) && Save(Root),
               TEXT("save_blueprints_instances_and_bundle_registry_tags")))
        return 1;
    auto Bad = Model;
    Bad.Classes.FindByPredicate([&](const auto& C) { return C.Id == ItemDecl->Id; })
        ->Metadata.Add(TEXT("PrimaryAssetType"), TEXT("Bad:Type"));
    FDasClassSchema Schema;
    Check(!Bad.ToActorSchema(ItemDecl->Id, Schema, Error), TEXT("invalid_primary_asset_type_rejected"));
    FAssetRegistryModule::GetRegistry().ScanFilesSynchronous(Files, true);
    Check(M.ScanPathForPrimaryAssets(FPrimaryAssetType(*Type),
                                     Base + TEXT("/Items"), UDasPrimaryDataAsset::StaticClass(), false, false, true) ==
                                         1 &&
                                     M.GetPrimaryAssetPath(Id) == FSoftObjectPath(RootPath),
                                     TEXT("assetmanager_discovers_saved_primary_instance_and_bundles"));
    auto Ready = FDasAssetPreflight::Inspect(Root);
    if (Ready.HasErrors())
        UE_LOG(LogTemp, Display, TEXT("Preflight: %s"), *Ready.ToJson());
    Check(!Ready.HasErrors() && Ready.Scripts.Contains(AuthorPath) && Ready.PrimaryId == Id.ToString(),
          TEXT("asset_preflight_saved_class_sources_registry_and_bundle"));
    Root->MarkPackageDirty();
    Check(FDasAssetPreflight::Inspect(Root).HasIssue(TEXT("asset_unsaved_changes")),
                                                     TEXT("asset_preflight_detects_unsaved_instance"));
    Root->GetOutermost()->SetDirtyFlag(false);
    auto& Meta = ItemBP->GetOutermost()->GetMetaData();
    const FString OriginalSource = Meta.GetValue(ItemBP, TEXT("DasDSLSource"));
    Meta.SetValue(ItemBP, TEXT("DasDSLSource"), *(TEXT("missing_preflight_")+ Tag + TEXT(".das")));
    Check(FDasAssetPreflight::Inspect(Root).HasIssue(TEXT("script_missing")),
                                                     TEXT("asset_preflight_detects_missing_source"));
    Meta.SetValue(ItemBP, TEXT("DasDSLSource"), *OriginalSource);
    ItemBP->GetOutermost()->SetDirtyFlag(false);
    const auto OldSoft = Soft->GetPropertyValue(Soft->ContainerPtrToValuePtr<void>(Root));
    Soft->SetPropertyValue(Soft->ContainerPtrToValuePtr<void>(Root), FSoftObjectPtr());
    Check(FDasAssetPreflight::Inspect(Root).HasIssue(
        TEXT("bundle_registry_stale")), TEXT("asset_preflight_detects_stale_bundle_tags_without_updating_them"));
    Soft->SetPropertyValue(Soft->ContainerPtrToValuePtr<void>(Root), OldSoft);
    FPrimaryAssetRules Never;
    Never.CookRule = EPrimaryAssetCookRule::NeverCook;
    M.SetPrimaryAssetRules(Id, Never);
    Check(FDasAssetPreflight::Inspect(Root).HasIssue(TEXT("asset_never_cook")),
                                                     TEXT("asset_preflight_detects_nevercook_policy"));
    M.SetPrimaryAssetRules(Id, FPrimaryAssetRules());
    for (auto* O : {Root, Plain})
    {
        O->GetOutermost()->SetDirtyFlag(false);
        O->ClearFlags(RF_Standalone);
    }
    Root = nullptr;
    Plain = nullptr;
    Primary = nullptr;
    CollectGarbage(RF_NoFlags);
    Check(!FSoftObjectPath(RootPath).ResolveObject() && !FSoftObjectPath(BundlePath).ResolveObject(),
          TEXT("data_instances_unloaded_before_runtime_load"));
    FString Fixture, Probe;
    if (!FParse::Value(*Params, TEXT("Fixture="), Fixture) || !FFileHelper::LoadFileToString(Probe, *Fixture))
        return 2;
    Probe.ReplaceInline(TEXT("__AUTHOR__"), *AuthorFile);
    Probe.ReplaceInline(TEXT("__TYPE__"), *Type);
    Probe.ReplaceInline(TEXT("__BUNDLE__"), *BundlePath);
    if (!FFileHelper::SaveStringToFile(Probe, *ProbePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return 2;
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    TStrongObjectPtr<AActor> Owner(World->SpawnActor<AActor>());
    ON_SCOPE_EXIT
    {
        R.ReleaseObject(Owner.Get());
        M.UnloadPrimaryAsset(Id);
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    auto Call = [&](const TCHAR* Entry)
    {
        auto V = R.RunNamedPersistent(ProbePath, Entry, 0, Owner.Get());
        if (!V.bSucceeded)
            UE_LOG(LogTemp, Display, TEXT("Authored Asset VM: %s"), *V.Error);
        return V;
    };
    const auto Start = Call(TEXT("start"));
    if (!Check(Start.bSucceeded && Start.Value == 1, TEXT("runtime_find_and_start_bundle_load")))
        return 1;
    bool Completed = false;
    const double Until = FPlatformTime::Seconds() + 5.;
    while (FPlatformTime::Seconds() < Until)
    {
        ++GFrameCounter;
        ProcessAsyncLoading(true, false, .005);
        FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread);
        FTickableGameObject::TickObjects(nullptr, LEVELTICK_All, false, .01f);
        FTSTicker::GetCoreTicker().Tick(.01f);
        auto V = Call(TEXT("status"));
        if (V.bSucceeded && V.Value == 1)
        {
            Completed = true;
            break;
        }
        FPlatformProcess::Sleep(.01f);
    }
    Check(Completed && FSoftObjectPath(BundlePath).ResolveObject(),
          TEXT("async_loaded_asset_preserves_overrides_and_executes_dsl_method"));
    CollectGarbage(RF_NoFlags);
    auto Read = Call(TEXT("after_gc"));
    Check(Read.bSucceeded && Read.Value == 1, TEXT("typed_dataasset_reference_and_method_survive_gc"));
    const auto Unload = Call(TEXT("unload"));
    CollectGarbage(RF_NoFlags);
    Check(Unload.bSucceeded && Unload.Value == 1 && !M.GetPrimaryAssetHandle(Id).IsValid() &&
              !FSoftObjectPath(RootPath).ResolveObject() && !FSoftObjectPath(BundlePath).ResolveObject(),
          TEXT("unload_releases_authored_asset_and_bundle"));
    UE_LOG(LogTemp, Display,
           TEXT("DAS_AUTHORED_ASSETS_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
