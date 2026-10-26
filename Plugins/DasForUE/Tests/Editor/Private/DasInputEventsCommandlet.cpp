#include "DasInputEventsCommandlet.h"
#include "DasInputEvents.h"
#include "DasRuntime.h"
#include "DasActor.h"
#include "DasSchema.h"
#include "Input/Events.h"
#include "Layout/WidgetPath.h"
#include "Widgets/SLeafWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
#include "Misc/ScopeExit.h"
#include "Misc/FileHelper.h"
#include <limits>
namespace
{
class SInputSnapshotLeaf : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SInputSnapshotLeaf) {}
    SLATE_END_ARGS()
    void Construct(const FArguments&) {}
    virtual FVector2D ComputeDesiredSize(float) const override { return FVector2D(10, 10); }
    virtual int32 OnPaint(const FPaintArgs&, const FGeometry&, const FSlateRect&, FSlateWindowElementList&, int32 Layer,
                          const FWidgetStyle&, bool) const override
    {
        return Layer;
    }
};
}
UDasInputEventsCommandlet::UDasInputEventsCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasInputEventsCommandlet::Main(const FString& Params)
{
    FString Script;
    if (!FParse::Value(*Params, TEXT("Script="), Script))
        return 1;
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool B, const TCHAR* Name)
    {
        ++Checks;
        UE_LOG(LogTemp, Display, TEXT("DAS_INPUT_EVENTS_%s %s"), B ? TEXT("PASS"): TEXT("FAIL"), Name);
        if (!B)
            ++Failures;
    };
    FString Error;
    TWeakPtr<SWidget> WeakPathWidget;
    TArray<FInstancedStruct> Snapshots;
    TArray<uint64> Timestamps;
    {
        FModifierKeysState Mods(true, false, false, true, true, false, false, true, true);
        FWidgetPath Path;
        auto Leaf = SNew(SInputSnapshotLeaf);
        WeakPathWidget = Leaf;
        Path.Widgets.AddWidget(FArrangedWidget(Leaf, FGeometry()));
        FInputEvent Base(Mods, 7, true);
        FKeyEvent Key(EKeys::SpaceBar, Mods, 7, true, 65, 66);
        FAnalogInputEvent Analog(EKeys::Gamepad_LeftX, Mods, 7, false, 65, 66, 0.75f);
        FCharacterEvent Character(TCHAR(0x416), Mods, 7, false);
        TSet<FKey> Buttons{EKeys::LeftMouseButton, EKeys::RightMouseButton};
        FPointerEvent Pointer(7, 3, FVector2D(12, 20), FVector2D(10, 19), Buttons, EKeys::LeftMouseButton, 0.5f, Mods);
        FNavigationEvent Navigation(Mods, 7, EUINavigation::Left, ENavigationGenesis::User);
        auto Capture = [&](auto& E, UScriptStruct* Type)
        {
            E.SetEventPath(Path);
            FInstancedStruct S;
            Check(CaptureDasInputEvent(Type, &E, S, Error), *Type->GetName());
            Check(E.GetEventPath() == &Path, TEXT("capture_does_not_modify_source_path"));
            Timestamps.Add(E.GetEventTimestamp());
            Snapshots.Add(MoveTemp(S));
        };
        Capture(Base, FInputEvent::StaticStruct());
        Capture(Key, FKeyEvent::StaticStruct());
        Capture(Analog, FAnalogInputEvent::StaticStruct());
        Capture(Character, FCharacterEvent::StaticStruct());
        Capture(Pointer, FPointerEvent::StaticStruct());
        Capture(Navigation, FNavigationEvent::StaticStruct());
        Pointer = FPointerEvent();
        Buttons.Empty();
    }
    Check(!WeakPathWidget.IsValid(), TEXT("snapshots_do_not_retain_original_path_widget"));
    auto BaseOf = [&](int32 I) -> const FInputEvent&
    {
        switch (I)
        {
        case 0:
            return Snapshots[I].Get<FInputEvent>();
        case 1:
            return Snapshots[I].Get<FKeyEvent>();
        case 2:
            return Snapshots[I].Get<FAnalogInputEvent>();
        case 3:
            return Snapshots[I].Get<FCharacterEvent>();
        case 4:
            return Snapshots[I].Get<FPointerEvent>();
        default:
            return Snapshots[I].Get<FNavigationEvent>();
        }
    };
    for (int32 I = 0; I < Snapshots.Num(); ++I)
    {
        Check(ValidateDasInputEventSnapshot(Snapshots[I], Error) && BaseOf(I).GetEventPath() == nullptr &&
                  BaseOf(I).GetEventTimestamp() == Timestamps[I] && BaseOf(I).GetUserIndex() == 7,
              TEXT("detached_metadata_survives_original_scope"));
    }
    Check(Snapshots[4].Get<FPointerEvent>().GetPressedButtons().Num() == 2 &&
              Snapshots[4].Get<FPointerEvent>().IsMouseButtonDown(EKeys::RightMouseButton),
          TEXT("owned_pressed_keys_survive_source_mutation"));
    FInstancedStruct Out = Snapshots[1];
    FAnalogInputEvent BadAnalog(EKeys::Gamepad_LeftX, FModifierKeysState(), 0, false, 0, 0,
                                std::numeric_limits<float>::quiet_NaN());
    Check(!CaptureDasInputEvent(FAnalogInputEvent::StaticStruct(), &BadAnalog, Out, Error) &&
              Out.GetScriptStruct() == FKeyEvent::StaticStruct(),
          TEXT("nonfinite_rejects_without_partial_output"));
    TSet<FKey> TooMany;
    for (int32 I = 0; I < 257; ++I)
        TooMany.Add(FKey(FName(*FString::Printf(TEXT("AuditButton%d"), I))));
    FPointerEvent Oversized(0, FVector2D::ZeroVector, FVector2D::ZeroVector, TooMany, EKeys::LeftMouseButton, 0,
                            FModifierKeysState());
    Check(!CaptureDasInputEvent(FPointerEvent::StaticStruct(), &Oversized, Out, Error) &&
              Out.GetScriptStruct() == FKeyEvent::StaticStruct(),
          TEXT("pressed_key_budget_atomic_rejection"));
    FInstancedStruct Borrowed;
    {
        FWidgetPath P;
        FKeyEvent E;
        E.SetEventPath(P);
        Borrowed.InitializeAs(FKeyEvent::StaticStruct(), reinterpret_cast<const uint8*>(&E));
        Check(!ValidateDasInputEventSnapshot(Borrowed, Error), TEXT("borrowed_path_not_a_valid_snapshot"));
    }
    Borrowed.Reset();
    Check(!CaptureDasInputEvent(TBaseStructure<FVector>::Get(), nullptr, Out, Error),
          TEXT("foreign_native_identity_rejected"));
    Check(!ValidateDasValueStruct(FKeyEvent::StaticStruct()->GetPathName(), Error),
          TEXT("reflected_storage_defaults_still_closed"));
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    auto* Actor = World->SpawnActor<ADasActor>();
    TArray<FDasScalarValue> Args;
    for (const auto& S : Snapshots)
    {
        FDasScalarValue V;
        V.Kind = 12;
        V.ObjectClass = S.GetScriptStruct()->GetPathName();
        V.StructValue = S;
        Args.Add(MoveTemp(V));
    }
    Check(Snapshots[1].Get<FKeyEvent>().GetCharacter() == 65 && Snapshots[1].Get<FKeyEvent>().GetKeyCode() == 66 &&
              Snapshots[3].Get<FCharacterEvent>().GetCharacter() == TCHAR(0x416),
          TEXT("original_key_character_codes"));
    Check(Snapshots[0].Get<FInputEvent>().GetModifierKeys().AreCapsLocked() &&
              Snapshots[5].Get<FNavigationEvent>().GetNavigationType() == EUINavigation::Left &&
              Snapshots[5].Get<FNavigationEvent>().GetNavigationGenesis() == ENavigationGenesis::User,
          TEXT("capslock_and_navigation_payload"));
    const auto Captured = Runtime.DispatchInputEventSnapshots(Script, TEXT("accept_events"), Snapshots, Actor);
    if (!Captured.bSucceeded)
        UE_LOG(LogTemp, Display, TEXT("Capture issue: %s / %s"), *Captured.Error, *Captured.ReloadError);
    Check(Captured.bSucceeded && Captured.Value == 33, TEXT("33_generated_getters_and_projections"));
    const auto ReloadEvents = Snapshots;
    Snapshots.Empty();
    Args.Empty();
    CollectGarbage(RF_NoFlags);
    const auto Read = Runtime.RunNamedPersistent(Script, TEXT("read_saved"), 0, Actor);
    if (!Read.bSucceeded)
        UE_LOG(LogTemp, Display, TEXT("Saved issue: %s"), *Read.Error);
    Check(Read.bSucceeded && Read.Value == 1, TEXT("DAS_globals_clone_arrays_and_collection_keep_owned_data"));
    for (const TCHAR* Entry : {TEXT("bad_default"), TEXT("bad_kind"), TEXT("bad_navigation"), TEXT("bad_unassigned")})
    {
        const auto R = Runtime.RunNamedPersistent(Script, Entry, 0, Actor);
        const FString Expected = FString(Entry) == TEXT("bad_default")
                                 ? TEXT("struct is null")
                                 : FString(Entry) == TEXT("bad_kind")
                                       ? TEXT("identity/data mismatch")
                                       : FString(Entry) == TEXT("bad_navigation")? TEXT("active synchronous")
                                                                                 : TEXT("missing input argument");
        Check(!R.bSucceeded && R.Error.Contains(Expected), TEXT("invalid_dispatch_rejected_for_expected_reason"));
        UE_LOG(LogTemp, Display, TEXT("Expected rejection %s: %s"), Entry, *R.Error);
    }
    const auto AfterErrors = Runtime.RunNamedPersistent(Script, TEXT("read_saved"), 0, Actor);
    Check(AfterErrors.bSucceeded && AfterErrors.Value == 1, TEXT("failed_calls_preserve_saved_values"));
    FString Original;
    FFileHelper::LoadFileToString(Original, *Script);
    ON_SCOPE_EXIT
    {
        FFileHelper::SaveStringToFile(Original, *Script);
    };
    FFileHelper::SaveStringToFile(Original + TEXT("\n// input snapshot reload revision\n"), *Script);
    const auto Reload = Runtime.RunNamedPersistent(Script, TEXT("read_saved"), 0, Actor);
    Check(Reload.bSucceeded && Reload.ReloadError.IsEmpty() && Reload.Revision > Captured.Revision &&
              Reload.Value == -1,
          TEXT("reload_replaces_old_context_and_preserves_native_entry_contract"));
    const auto Fresh = Runtime.DispatchInputEventSnapshots(Script, TEXT("accept_events"), ReloadEvents, Actor);
    Check(Fresh.bSucceeded && Fresh.Value == 33, TEXT("detached_native_values_reenter_fresh_context_safely"));
    const auto FreshRead = Runtime.RunNamedPersistent(Script, TEXT("read_saved"), 0, Actor);
    Check(FreshRead.bSucceeded && FreshRead.Value == 1, TEXT("fresh_context_clone_roots_survive_collection"));
    Runtime.ReleaseObject(Actor);
    Check(Runtime.StructSnapshotCount(Actor) == 0, TEXT("release_clears_snapshot_table"));
    Actor->Destroy();
    UE_LOG(LogTemp, Display,
           TEXT("DAS_INPUT_EVENTS_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
