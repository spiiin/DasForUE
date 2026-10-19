#include "DasImageSoftBrushCommandlet.h"
#include "DasRuntime.h"
#include "DasProbeActor.h"
#include "DasScriptUserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Framework/Application/SlateApplication.h"
#include "Interfaces/ISlateNullRendererModule.h"
#include "Null/NullApplication.h"
#include "UObject/Package.h"
#include "UObject/Linker.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
#include "Misc/PackageName.h"
#include "Misc/ScopeExit.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Async/TaskGraphInterfaces.h"
#include "Containers/Ticker.h"
#include "Tickable.h"
UDasImageSoftBrushCommandlet::UDasImageSoftBrushCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasImageSoftBrushCommandlet::Main(const FString& Params)
{
    FString Script;
    if (!FParse::Value(*Params, TEXT("Script="), Script))
        return 2;
    const bool OwnSlate = !FSlateApplication::IsInitialized();
    if (OwnSlate)
    {
        FSlateApplication::Create(MakeShareable(FNullApplication::CreateNullApplication()));
        FSlateApplication::Get().InitializeRenderer(FModuleManager::LoadModuleChecked<ISlateNullRendererModule>(
            TEXT("SlateNullRenderer")).CreateSlateNullRenderer());
    }
    ON_SCOPE_EXIT
    {
        if (OwnSlate)
            FSlateApplication::Shutdown();
    };
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool Good, const TCHAR* Name)
    {
        ++Checks;
        if (!Good)
            ++Failures;
        UE_LOG(LogTemp, Display, TEXT("DAS_IMAGE_SOFT_BRUSH_%s %s"), Good ? TEXT("PASS"): TEXT("FAIL"), Name);
        return Good;
    }; // Probe paths are fixed in the script; the backing directory is unique per run.
    static const TCHAR* Names[] = {
        TEXT("TexA"), TEXT("TexB"), TEXT("TexC"), TEXT("TexD"), TEXT("TexE"),
                                                                     TEXT("TexF"), TEXT("TexG"), TEXT("TexH"), TEXT("Mat")};
    const FString Mount = TEXT("/DasSoftBrushProbe/"),
                               Directory = FPaths::ConvertRelativePathToFull(
                                   FPaths::ProjectSavedDir() / TEXT("DasImageSoftBrush")/
                                                                    FGuid::NewGuid().ToString(EGuidFormats::Digits)) +
                                   TEXT("/");
    auto PathOf = [&](int32 I)
    {
        return FSoftObjectPath(Mount + Names[I] + TEXT(".")+ Names[I]);
    };
    TArray<FString> Files;
    IFileManager::Get().MakeDirectory(*Directory, true);
    FPackageName::RegisterMountPoint(Mount, Directory);
    ON_SCOPE_EXIT
    {
        FlushAsyncLoading();
        CollectGarbage(RF_NoFlags);
        for (const TCHAR* Name : Names)
            if (auto* P = FindPackage(nullptr, *(Mount + Name)))
                ResetLoaders(P);
        FPackageName::UnRegisterMountPoint(Mount, Directory);
        for (const auto& File : Files)
            IFileManager::Get().Delete(*File);
        IFileManager::Get().DeleteDirectory(*Directory, false, true);
    };
    auto Save = [&](UObject* Asset)
    {
        Files.Add(Directory + Asset->GetName() + TEXT(".uasset"));
        FSavePackageArgs Args;
        Args.TopLevelFlags = RF_Public | RF_Standalone;
        const bool Ok = UPackage::SavePackage(Asset->GetOutermost(), Asset, *Files.Last(), Args);
        Asset->ClearFlags(RF_Standalone);
        Asset->GetOutermost()->SetDirtyFlag(false);
        return Ok;
    };
    bool Saved = true;
    TArray<uint8> Pixels;
    Pixels.Init(255, 8 * 4 * 4);
    for (int32 I = 0; I < 8; ++I)
    {
        auto* T = NewObject<UTexture2D>(CreatePackage(*(Mount + Names[I])), Names[I], RF_Public | RF_Standalone);
        T->Source.Init(8, 4, 1, 1, TSF_BGRA8, Pixels.GetData());
        T->CompressionSettings = TC_EditorIcon;
        T->MipGenSettings = TMGS_NoMipmaps;
        Saved &= Save(T);
    }
    auto* Material =
        NewObject<UMaterialInstanceConstant>(CreatePackage(*(Mount + Names[8])), Names[8], RF_Public | RF_Standalone);
    Material->SetParentEditorOnly(UMaterial::GetDefaultMaterial(MD_Surface));
    Saved &= Save(Material);
    if (!Check(Saved, TEXT("temporary_texture_and_material_packages")))
        return 1;
    CollectGarbage(RF_NoFlags);
    bool Unloaded = true;
    for (int32 I = 0; I < 9; ++I)
        Unloaded &= PathOf(I).ResolveObject() == nullptr;
    if (!Check(Unloaded, TEXT("fixtures_unloaded_before_requests")))
        return 1;
    auto& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
    Registry.ScanFilesSynchronous(Files, true);
    bool Known = true;
    for (int32 I = 0; I < 9; ++I)
        Known &= Registry.GetAssetByObjectPath(PathOf(I)).IsValid();
    if (!Check(Known, TEXT("fixtures_registered_on_disk")))
        return 1;
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    auto* Actor = World->SpawnActor<ADasProbeActor>();
    TStrongObjectPtr<UDasScriptUserWidget> Widget(NewObject<UDasScriptUserWidget>(World.Get()));
    Widget->Initialize();
    if (!Widget->WidgetTree)
        Widget->WidgetTree = NewObject<UWidgetTree>(Widget.Get(), TEXT("WidgetTree"), RF_Transient);
    UImage* Image = Widget->WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Brush"));
    Widget->WidgetTree->RootWidget = Image;
    TStrongObjectPtr<UTexture2D> Sync(
        NewObject<UTexture2D>(GetTransientPackage(), TEXT("DasSoftBrushSync"), RF_Transient));
    TSharedPtr<SWidget> Slate;
    ON_SCOPE_EXIT
    {
        Slate.Reset();
        if (Widget)
            Widget->ReleaseSlateResources(true);
    };
    auto Resource = [&]
    {
        return Image->GetBrush().GetResourceObject();
    };
    auto Run = [&](const TCHAR* Entry, int32 Input)
    {
        auto R = Runtime.RunNamedPersistent(Script, Entry, Input, Actor);
        if (!R.bSucceeded)
            UE_LOG(LogTemp, Display, TEXT("Soft brush issue %s: %s / %s"), Entry, *R.Error, *R.ReloadError);
        return R;
    };
    auto Status = [&]
    {
        auto R = Run(TEXT("status"), 0);
        return R.bSucceeded ? R.Value : -100;
    };
    auto Setup = [&]()
    {
        TArray<FDasScalarValue> Args;
        for (UObject* O : {static_cast<UObject*>(Image), static_cast<UObject*>(Sync.Get())})
        {
            FDasScalarValue V;
            V.Kind = 8;
            V.ObjectClass = TEXT("/Script/CoreUObject.Object");
            V.ObjectValue = O;
            Args.Add(V);
        }
        return Runtime.RunNamedFramePersistent(Script, TEXT("setup"), Args, EDasMethodSignature::Int, Actor);
    };
    auto Pump = [&](TFunctionRef<bool()> Done, double Seconds)
    {
        const double End = FPlatformTime::Seconds() + Seconds;
        while (FPlatformTime::Seconds() < End)
        {
            ++GFrameCounter;
            ProcessAsyncLoading(true, false, .005);
            FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread);
            FTickableGameObject::TickObjects(nullptr, LEVELTICK_All, false, .01f);
            FTSTicker::GetCoreTicker().Tick(.01f);
            if (Done())
                return true;
            FPlatformProcess::Sleep(.01f);
        }
        return false;
    };
    // A cancelled stream may still finish the package load; wait for it so a late write would be observed.
    auto Settle = [&](int32 I)
    {
        Pump([&] { return PathOf(I).ResolveObject() != nullptr; }, 2.);
        Pump([] { return false; }, .1);
    };

    Check(Setup().bSucceeded, TEXT("setup_weak_image_reference"));
    auto Early = Run(TEXT("load_texture"), 0);
    Check(!Early.bSucceeded && Early.Error.Contains(TEXT("constructed Image")) && !PathOf(0).ResolveObject(),
                                                    TEXT("unconstructed_image_rejected_without_loading"));
    Slate = Widget->TakeWidget();
    UObject* Initial = Resource();
    auto BrushSize = [&]
    {
        return FVector2D(Image->GetBrush().GetImageSize());
    };
    auto SetBrushSize = [&](const FVector2D& Size)
    {
        FSlateBrush Brush = Image->GetBrush();
        Brush.SetImageSize(Size);
        Image->SetBrush(Brush);
    };
    SetBrushSize(FVector2D(17, 19));
    auto First = Run(TEXT("load_texture"), 0);
    Check(First.bSucceeded && First.Value == 1 && Resource() == Initial,
          TEXT("request_is_pending_without_changing_brush"));
    Check(Pump([&] { return Status() == 2; }, 5.), TEXT("texture_async_load_completes"));
    // NullRHI editor builds no texture platform data, so the matched size is whatever UE reports.
    auto* TexA = Cast<UTexture2D>(PathOf(0).ResolveObject());
    Check(TexA && Resource() == TexA && BrushSize() == FVector2D(TexA->GetSizeX(), TexA->GetSizeY()) &&
              BrushSize() != FVector2D(17, 19),
          TEXT("native_setter_applies_texture_with_match_size"));
    auto Mat = Run(TEXT("load_material"), 8);
    Check(Mat.bSucceeded && Mat.Value == 1 && Pump([&] { return Status() == 2; }, 5.) && Resource() &&
              Resource() == PathOf(8).ResolveObject(),
          TEXT("material_async_load_completes"));
    auto Cached = Run(TEXT("load_texture"), 0);
    Check(Cached.bSucceeded && Cached.Value == 2 && Resource() == TexA, TEXT("loaded_asset_applies_synchronously"));
    SetBrushSize(FVector2D(17, 19));
    auto Newer = Run(TEXT("supersede"), 0);
    const bool NewerReady = Newer.bSucceeded && Newer.Value == 1 && Pump([&] { return Status() == 2; }, 5.);
    Settle(1);
    Check(NewerReady && Resource() && Resource() == PathOf(2).ResolveObject(),
          TEXT("newer_request_supersedes_older_completion"));
    Check(BrushSize() == FVector2D(17, 19), TEXT("unmatched_size_keeps_brush_size"));
    auto SyncCall = Run(TEXT("load_then_sync"), 3);
    Settle(3);
    Check(SyncCall.bSucceeded && SyncCall.Value == 4 && Status() == 4 && Resource() == Sync.Get(),
          TEXT("generated_sync_setter_cancels_pending_request"));
    auto External = Run(TEXT("load_texture"), 4);
    Image->SetBrushResourceObject(TexA);
    Check(External.bSucceeded && External.Value == 1 && Pump([&] { return Status() != 1; }, 5.) && Status() == 4 &&
              Resource() == TexA,
          TEXT("external_brush_change_wins_over_late_completion"));
    auto Gone = Run(TEXT("load_texture"), 5);
    Slate.Reset();
    Widget->ReleaseSlateResources(true);
    Settle(5);
    Check(Gone.bSucceeded && Gone.Value == 1 && Status() == 4 && Resource() == TexA,
          TEXT("NativeDestruct_cancels_pending_request"));
    Slate = Widget->TakeWidget();
    auto Loaded = Run(TEXT("load_texture"), 8);
    Check(!Loaded.bSucceeded && Loaded.Error.Contains(TEXT("incompatible")) && Resource() == TexA,
                                                      TEXT("loaded_material_rejected_as_texture"));
    auto Missing = Run(TEXT("load_texture"), 9);
    Check(!Missing.bSucceeded && Missing.Error.Contains(TEXT("not registered")) && Resource() == TexA,
                                                        TEXT("unregistered_path_rejected"));
    auto Clear = Run(TEXT("clear_texture"), 0);
    Check(Clear.bSucceeded && Clear.Value == 2 && Resource() == nullptr, TEXT("null_soft_reference_clears_brush"));
    TexA = nullptr;
    CollectGarbage(RF_NoFlags);
    bool Released = true;
    for (int32 I = 0; I < 9; ++I)
        Released &= PathOf(I).ResolveObject() == nullptr;
    Check(Released, TEXT("finished_and_cancelled_requests_do_not_pin_assets"));
    auto Wrong = Run(TEXT("load_texture"), 8);
    Check(!Wrong.bSucceeded &&
          Wrong.Error.Contains(TEXT("type mismatch")) && !PathOf(8).ResolveObject() && Resource() == nullptr,
                               TEXT("unloaded_material_rejected_by_registry_class_without_loading"));
    auto Pending = Run(TEXT("load_texture"), 6);
    FString Original;
    FFileHelper::LoadFileToString(Original, *Script);
    ON_SCOPE_EXIT
    {
        FFileHelper::SaveStringToFile(Original, *Script);
    };
    FFileHelper::SaveStringToFile(Original + TEXT("\n// ImageSoftBrush reload revision\n"), *Script);
    auto Reloaded = Setup();
    const int32 After = Status();
    Settle(6);
    Check(Pending.bSucceeded && Pending.Value == 1 && Reloaded.bSucceeded && Reloaded.ReloadError.IsEmpty() &&
              Reloaded.Revision > Pending.Revision && After == 0 && Resource() == nullptr,
          TEXT("reload_revokes_old_context_requests"));
    auto Last = Run(TEXT("load_texture"), 7);
    Runtime.ReleaseObject(Actor);
    Settle(7);
    Check(Last.bSucceeded && Last.Value == 1 && Resource() == nullptr, TEXT("owner_release_revokes_pending_request"));
    Actor->Destroy();
    UE_LOG(LogTemp, Display,
           TEXT("DAS_IMAGE_SOFT_BRUSH_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
