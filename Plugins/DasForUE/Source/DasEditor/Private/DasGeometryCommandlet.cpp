#include "DasGeometryCommandlet.h"
#include "DasGeometry.h"
#include "DasRuntime.h"
#include "DasActor.h"
#include "DasSchema.h"
#include "Layout/Geometry.h"
#include "Components/Border.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
#include "Misc/ScopeExit.h"
#include "Misc/FileHelper.h"
#include "Async/Async.h"
#include <limits>
UDasGeometryCommandlet::UDasGeometryCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasGeometryCommandlet::Main(const FString& Params)
{
    FString Script;
    if (!FParse::Value(*Params, TEXT("Script="), Script))
        return 1;
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool B, const TCHAR* N)
    {
        ++Checks;
        if (!B)
            ++Failures;
        UE_LOG(LogTemp, Display, TEXT("DAS_GEOMETRY_%s %s"), B ? TEXT("PASS"): TEXT("FAIL"), N);
    };
    FString Error;
    FInstancedStruct Snapshot;
    const auto Root = FGeometry::MakeRoot(FVector2f(100, 50), FSlateLayoutTransform());
    auto Source = Root.MakeChild(FVector2f(100, 50), FSlateLayoutTransform(2, FVector2f(10, 20)),
                                 FSlateRenderTransform(FScale2f(1.5f, 0.5f)), FVector2f::ZeroVector);
    Check(CaptureDasGeometry(FGeometry::StaticStruct(), &Source, Snapshot, Error),
          TEXT("capture_native_render_and_layout_transforms"));
    Source = FGeometry();
    const auto& G = Snapshot.Get<FGeometry>();
    Check(FVector2D(G.LocalToAbsolute(FVector2f(4, 6))).Equals(FVector2D(22, 26)),
          TEXT("owned_copy_survives_source_mutation"));
    const auto Complex = Root.MakeChild(FVector2f(30, 20), FSlateLayoutTransform(-2, FVector2f(40, 30)),
                                        FSlateRenderTransform(FQuat2f(0.7f)), FVector2f(0.5f, 0.5f));
    FInstancedStruct ComplexCopy;
    Check(CaptureDasGeometry(FGeometry::StaticStruct(), &Complex, ComplexCopy, Error) &&
              ComplexCopy.Get<FGeometry>().GetAccumulatedRenderTransform() == Complex.GetAccumulatedRenderTransform(),
          TEXT("rotation_reflection_pivot_preserved_without_reconstruction"));
    Check(FVector2D(ComplexCopy.Get<FGeometry>().AbsoluteToLocal(Complex.LocalToAbsolute(FVector2f(2, 3))))
              .Equals(FVector2D(2, 3), 0.0001),
          TEXT("complex_transform_roundtrip"));
    const auto Singular = FGeometry::MakeRoot(FVector2f(10, 10), FSlateLayoutTransform(0));
    FInstancedStruct SingularCopy;
    Check(CaptureDasGeometry(FGeometry::StaticStruct(), &Singular, SingularCopy, Error),
          TEXT("finite_singular_geometry_can_be_observed"));
    Check(!ValidateDasGeometryData(FGeometry::StaticStruct(), &Singular, Error, true) &&
          Error.Contains(TEXT("singular")), TEXT("inverse_singular_rejects_before_native_check"));
    const auto Tiny = FGeometry::MakeRoot(FVector2f(10, 10), FSlateLayoutTransform(1.e-30f));
    Check(!ValidateDasGeometryData(FGeometry::StaticStruct(), &Tiny, Error, true), TEXT("inverse_underflow_rejected"));
    const auto Bad =
        FGeometry::MakeRoot(FVector2f(10, 10), FSlateLayoutTransform(std::numeric_limits<float>::infinity()));
    FInstancedStruct Out = Snapshot;
    Check(!CaptureDasGeometry(FGeometry::StaticStruct(), &Bad, Out, Error) &&
              Out.Get<FGeometry>().GetAccumulatedRenderTransform() == G.GetAccumulatedRenderTransform(),
          TEXT("nonfinite_capture_is_atomic"));
    Check(!CaptureDasGeometry(TBaseStructure<FVector>::Get(), &Source, Out, Error),
          TEXT("wrong_native_identity_rejected"));
    Check(!ValidateDasValueStruct(FGeometry::StaticStruct()->GetPathName(), Error),
          TEXT("reflected_storage_defaults_still_closed"));
    Check(!Async(EAsyncExecution::ThreadPool,
                 [&]()
                 {
                     FString E;
                     return ValidateDasGeometrySnapshot(Snapshot, E);
                 })
               .Get(),
          TEXT("worker_thread_access_rejected"));
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    auto* Actor = World->SpawnActor<ADasActor>();
    auto* Player = World->SpawnActor<APlayerController>();
    const auto Run = [&](const TCHAR* Entry)
    {
        auto R = Runtime.RunNamedPersistent(Script, Entry, 0, Actor);
        if (!R.bSucceeded)
            UE_LOG(LogTemp, Display, TEXT("Geometry issue %s: %s / %s"), Entry, *R.Error, *R.ReloadError);
        return R;
    };
    auto Captured = Runtime.DispatchGeometrySnapshot(Script, TEXT("accept_geometry"), Snapshot, Actor);
    if (!Captured.bSucceeded)
        UE_LOG(LogTemp, Display, TEXT("Geometry dispatch issue: %s / %s"), *Captured.Error, *Captured.ReloadError);
    Check(Captured.bSucceeded && Captured.Value == 12,
          TEXT("12_generated_consumers_native_render_math_and_headless_world"));
    auto* Widget = NewObject<UBorder>(World.Get());
    TWeakObjectPtr<UObject> WeakWidget = Widget;
    auto ObjectCall = [&](const TCHAR* Entry, UObject* Object)
    {
        FDasScalarValue A;
        A.Kind = 8;
        A.ObjectClass = TEXT("/Script/CoreUObject.Object");
        A.ObjectValue = Object;
        auto R = Runtime.RunNamedFramePersistent(Script, Entry, {A}, EDasMethodSignature::Int, Actor);
        if (!R.bSucceeded)
            UE_LOG(LogTemp, Display, TEXT("Geometry object issue %s: %s"), Entry, *R.Error);
        return R;
    };
    Check(ObjectCall(TEXT("widget_geometry"), Widget).Value == 3,
                     TEXT("three_widget_geometry_producers_copy_borrowed_return"));
    Widget = nullptr;
    CollectGarbage(RF_NoFlags);
    Check(!WeakWidget.IsValid(), TEXT("geometry_snapshots_do_not_pin_widget"));
    Check(ObjectCall(TEXT("player_geometry"), Player).Value == 1, TEXT("player_geometry_producer_headless_fallback"));
    Snapshot.Reset();
    CollectGarbage(RF_NoFlags);
    auto Read = Run(TEXT("read_saved"));
    Check(Read.bSucceeded && Read.Value == 1, TEXT("globals_clone_arrays_and_GC_keep_owned_geometry"));
    for (auto Pair : {TPair<const TCHAR*, const TCHAR*>{TEXT("bad_default"), TEXT("struct is null")},
                                                        {TEXT("bad_unassigned"), TEXT("missing input argument")},
                                                         {TEXT("bad_overflow"), TEXT("overflow")},
                                                          {TEXT("bad_player"), TEXT("live player controller")}})
    {
        auto R = Run(Pair.Key);
        Check(!R.bSucceeded && R.Error.Contains(Pair.Value),
              TEXT("invalid_geometry_call_rejected_for_expected_reason"));
    }
    auto R = Runtime.DispatchGeometrySnapshot(Script, TEXT("inverse_geometry"), SingularCopy, Actor);
    Check(!R.bSucceeded && R.Error.Contains(TEXT("singular")), TEXT("script_inverse_singular_rejected"));
    auto Forward = Runtime.DispatchGeometrySnapshot(Script, TEXT("forward_geometry"), SingularCopy, Actor);
    Check(Forward.bSucceeded && Forward.Value == 1, TEXT("script_forward_singular_is_safe"));
    FString Original;
    FFileHelper::LoadFileToString(Original, *Script);
    ON_SCOPE_EXIT
    {
        FFileHelper::SaveStringToFile(Original, *Script);
    };
    FFileHelper::SaveStringToFile(Original + TEXT("\n// Geometry reload revision\n"), *Script);
    auto Reload = Run(TEXT("read_saved"));
    Check(Reload.bSucceeded && Reload.ReloadError.IsEmpty() && Reload.Revision > Captured.Revision &&
              Reload.Value == -1,
          TEXT("reload_discards_old_values_and_preserves_native_contract"));
    FInstancedStruct Fresh;
    auto NewGeometry = Root.MakeChild(FVector2f(100, 50), FSlateLayoutTransform(2, FVector2f(10, 20)),
                                      FSlateRenderTransform(FScale2f(1.5f, 0.5f)), FVector2f::ZeroVector);
    CaptureDasGeometry(FGeometry::StaticStruct(), &NewGeometry, Fresh, Error);
    auto Again = Runtime.DispatchGeometrySnapshot(Script, TEXT("accept_geometry"), Fresh, Actor);
    Check(Again.bSucceeded && Again.Value == 12, TEXT("fresh_native_snapshot_after_reload"));
    Runtime.ReleaseObject(Actor);
    Check(Runtime.StructSnapshotCount(Actor) == 0, TEXT("release_clears_snapshot_table"));
    Actor->Destroy();
    Player->Destroy();
    UE_LOG(LogTemp, Display, TEXT("DAS_GEOMETRY_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
