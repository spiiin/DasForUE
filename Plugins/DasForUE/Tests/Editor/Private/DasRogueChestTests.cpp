#include "DasActor.h"
#include "DasDSLService.h"
#include "DasInterfaceService.h"
#include "DasRuntime.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "Engine/Engine.h"
#include "Misc/AutomationTest.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/ScopeExit.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"
#include "Editor.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasRogueChestTest, "DasForUE.Ports.RogueChest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasRogueChestTest::RunTest(const FString&)
{
    if (!FParse::Param(FCommandLine::Get(), TEXT("DasRoguePort")))
    {
        AddInfo(TEXT("External ActionRoguelike port is opt-in; use Scripts/TestRogueChest.ps1."));
        return true;
    }
    FString Error;
    TArray<UBlueprint*> Interfaces;
    if (!FDasInterfaceService::Apply(TEXT("rogue_interaction_interface.das"), TEXT("/Game/DasRogue/Types"), Interfaces,
                                                                                   Error))
    {
        AddError(Error);
        return false;
    }
    UBlueprint *Chest = nullptr, *Observer = nullptr;
    if (!FDasDSLService::Apply(
            TEXT("rogue_item_chest.das"),
                 TEXT("/Game/DasRogue/BP_RogueItemChest"), TEXT("rogue.RogueItemChest"), Chest, Error) ||
                      !FDasDSLService::Apply(TEXT("rogue_item_chest_observer.das"),
                                                  TEXT("/Game/DasRogue/BP_ChestObserver"),
                                                       TEXT("roguetest.RogueItemChestObserver"), Observer, Error))
    {
        AddError(Error);
        return false;
    }
    TestTrue(TEXT("Real UE interface membership"),
                  Interfaces.Num() == 1 && Chest->GeneratedClass->ImplementsInterface(Interfaces[0]->GeneratedClass));
    TestTrue(TEXT("Child inherits interface membership without redeclaring it"),
                  Observer->GeneratedClass->ImplementsInterface(Interfaces[0]->GeneratedClass) &&
                      Observer->ImplementedInterfaces.IsEmpty());
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    auto& Context = GEngine->CreateNewWorldContext(EWorldType::Game);
    Context.SetCurrentWorld(World.Get());
    ON_SCOPE_EXIT
    {
        if (World->HasBegunPlay())
            World->EndPlay(EEndPlayReason::Quit);
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    auto* First = World->SpawnActor<ADasActor>(Observer->GeneratedClass);
    auto* Second = World->SpawnActor<ADasActor>(Observer->GeneratedClass);
    if (!TestNotNull(TEXT("Spawn first chest"), First) || !TestNotNull(TEXT("Spawn second chest"), Second))
        return false;
    World->InitializeActorsForPlay(FURL());
    World->BeginPlay();
    World->GetWorldSettings()->NotifyBeginPlay();
    auto Mesh = [](AActor* Actor, const TCHAR* Name)
    {
        auto* P = FindFProperty<FObjectPropertyBase>(Actor->GetClass(), Name);
        return P ? Cast<UStaticMeshComponent>(P->GetObjectPropertyValue_InContainer(Actor)) : nullptr;
    };
    auto Number = [](AActor* Actor, const TCHAR* Name)
    {
        auto* P = FindFProperty<FFloatProperty>(Actor->GetClass(), Name);
        return P ? P->GetPropertyValue_InContainer(Actor) : -9999.0f;
    };
    auto Count = [](AActor* Actor)
    {
        auto* P = FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("CompletionCount"));
        return P ? P->GetPropertyValue_InContainer(Actor) : -9999;
    };
    auto Tick = [&](float Delta)
    {
        // The engine clamps long world ticks; advance with ordinary short frames.
        const int32 Frames = FMath::Max(1, FMath::RoundToInt(Delta / 0.1f));
        for (int32 I = 0; I < Frames; ++I)
        {
            ++GFrameCounter;
            World->Tick(LEVELTICK_All, Delta / Frames);
        }
    };
    auto Interact = [this, First](AActor* Actor)
    {
        UFunction* Event = First->FindFunction(TEXT("TryInteract"));
        if (!TestNotNull(TEXT("Script interface caller"), Event))
            return false;
        FStructOnScope Frame(Event);
        auto* Target = FindFProperty<FObjectPropertyBase>(Event, TEXT("target"));
        auto* Result = FindFProperty<FBoolProperty>(Event, TEXT("ReturnValue"));
        if (!TestNotNull(TEXT("Interface caller target"), Target) ||
                         !TestNotNull(TEXT("Interface caller result"), Result))
            return false;
        Target->SetObjectPropertyValue_InContainer(Frame.GetStructMemory(), Actor);
        First->ProcessEvent(Event, Frame.GetStructMemory());
        return Result->GetPropertyValue_InContainer(Frame.GetStructMemory());
    };
    TestFalse(TEXT("Null interaction is rejected"), Interact(nullptr));
    auto* Unrelated = World->SpawnActor<AActor>();
    TestFalse(TEXT("Non-implementing target is rejected"), Interact(Unrelated));
    auto* Base = Mesh(First, TEXT("BaseMeshComponent"));
    auto* Lid = Mesh(First, TEXT("LidMeshComponent"));
    if (!TestNotNull(TEXT("Base component"), Base) || !TestNotNull(TEXT("Lid component"), Lid))
        return false;
    TestTrue(TEXT("Declared root and attachment"), First->GetRootComponent() == Base && Lid->GetAttachParent() == Base);
    TestEqual(TEXT("Lid collision"), Lid->GetCollisionProfileName(), FName(TEXT("NoCollision")));
    TestFalse(TEXT("Idle chest does not tick"), First->IsActorTickEnabled());
    Lid->SetRelativeLocation(FVector(5, 6, 7));
    Lid->SetRelativeScale3D(FVector(2, 3, 4));
    TestTrue(TEXT("Script interface call succeeds"), Interact(First));
    TestTrue(TEXT("Interact enables tick"), First->IsActorTickEnabled());
    Tick(0.5f);
    TestTrue(TEXT("Constant speed after half second"),
                  FMath::IsNearlyEqual(Number(First, TEXT("CurrentAnimationPitch")), 25.0f));
    TestTrue(TEXT("Lid pitch reaches component"), FMath::IsNearlyEqual(Lid->GetRelativeRotation().Pitch, 25.0));
    TestTrue(TEXT("World transform is updated, not only the reflected field"),
                  FMath::IsNearlyEqual(Lid->GetComponentRotation().Pitch, 25.0));
    TestTrue(TEXT("Rotation preserves designer location and scale"),
                  Lid->GetRelativeLocation().Equals(FVector(5, 6, 7)) &&
                      Lid->GetRelativeScale3D().Equals(FVector(2, 3, 4)));
    TestEqual(TEXT("Second instance is independent"), Number(Second, TEXT("CurrentAnimationPitch")), 0.0f);
    TestTrue(TEXT("Script interface call succeeds"), Interact(First));
    TestEqual(TEXT("Repeated interaction preserves progress"), Number(First, TEXT("CurrentAnimationPitch")), 25.0f);
    for (int32 I = 0; I < 6; ++I)
        Tick(0.5f);
    TestEqual(TEXT("Animation clamps at target"), Number(First, TEXT("CurrentAnimationPitch")), 120.0f);
    TestFalse(TEXT("Completion disables tick"), First->IsActorTickEnabled());
    TestEqual(TEXT("Overridable completion dispatched once"), Count(First), 1);
    Tick(0.5f);
    TestEqual(TEXT("Idle does not repeat completion"), Count(First), 1);
    // Preserve the original behavior: interacting with an open chest completes again next tick.
    TestTrue(TEXT("Script interface call succeeds"), Interact(First));
    Tick(0.1f);
    TestEqual(TEXT("Open chest interaction matches original"), Count(First), 2);
    auto* Target = FindFProperty<FFloatProperty>(Second->GetClass(), TEXT("AnimationTargetPitch"));
    if (!TestNotNull(TEXT("Editable target"), Target))
        return false;
    Target->SetPropertyValue_InContainer(Second, -30.0f);
    TestTrue(TEXT("Cross-instance interface call succeeds"), Interact(Second));
    Tick(0.5f);
    TestTrue(TEXT("Negative target interpolates down"),
                  FMath::IsNearlyEqual(Number(Second, TEXT("CurrentAnimationPitch")), -25.0f));
    Tick(0.5f);
    TestEqual(TEXT("Negative target clamps"), Number(Second, TEXT("CurrentAnimationPitch")), -30.0f);
    TestFalse(TEXT("Second completion disables tick"), Second->IsActorTickEnabled());
    TestTrue(TEXT("First script has no runtime error"), First->LastDasError.IsEmpty());
    TestTrue(TEXT("Second script has no runtime error"), Second->LastDasError.IsEmpty());
    return !HasAnyErrors();
}
