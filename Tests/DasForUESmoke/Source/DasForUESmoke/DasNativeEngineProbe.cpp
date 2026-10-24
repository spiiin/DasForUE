// The plugin's native module ue_engine (DasNativeEngine.cpp): RunDasNativeEngineProbe spawns actors and components
// in a game world and checks them through Content/Das/Scripts/native_engine_probe.das, run with the actor as self.
// The module "smoke_engine" hands the other objects to the script (its object results use the ue_engine types).
// Used by the automation test DasForUE.NativeEngine (temporary world) and by the packaged flag -DasNativeBinds.
#include "Components/SceneComponent.h"
#include "DasNativeBinds.h"
#include "DasRuntime.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Misc/AutomationTest.h"
#include "Modules/ModuleManager.h"

namespace DasNativeEngineProbe
{
TWeakObjectPtr<AActor> Other;
TWeakObjectPtr<USceneComponent> Child;
TWeakObjectPtr<APawn> Pawn;
TWeakObjectPtr<AActor> Victim;
AActor* GetOther()
{
    return Other.Get();
}
USceneComponent* GetChild()
{
    return Child.Get();
}
APawn* GetPawn()
{
    return Pawn.Get();
}
AActor* GetVictim()
{
    return Victim.Get();
}
}

DAS_NATIVE_BINDS(SmokeEngine, "smoke_engine")
{
    using namespace DasNativeEngineProbe;
    Binds.Function<&GetOther>(TEXT("smoke_engine_other"));
    Binds.Function<&GetChild>(TEXT("smoke_engine_child"));
    Binds.Function<&GetPawn>(TEXT("smoke_engine_pawn"));
    Binds.Function<&GetVictim>(TEXT("smoke_engine_victim"));
}

// Returns false with the failed checks in Report. World: a game world to spawn into, or nullptr for a temporary one.
bool RunDasNativeEngineProbe(UWorld* GameWorld, FString& Report)
{
    using namespace DasNativeEngineProbe;
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    bool bPassed = true;
    auto Check = [&](bool bOK, const TCHAR* Name, const FString& Detail = FString())
    {
        bPassed &= bOK;
        Report += FString::Printf(TEXT("%s %s"), bOK ? TEXT("PASS") : TEXT("FAIL"), Name);
        if (!Detail.IsEmpty())
            Report += TEXT(": ") + Detail;
        Report += TEXT("\n");
    };
    UWorld* World = GameWorld;
    if (!World)
    {
        World = UWorld::CreateWorld(EWorldType::Game, false, TEXT("DasNativeEngineProbe"));
        GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
        World->InitializeActorsForPlay(FURL());
    }
    auto Spawn = [&](UClass* Class, const FVector& Location)
    {
        AActor* Actor = World->SpawnActor(Class, &Location);
        auto* Root = NewObject<USceneComponent>(Actor, TEXT("Root"));
        Actor->SetRootComponent(Root);
        Root->RegisterComponent();
        Actor->SetActorLocation(Location);
        return Actor;
    };
    AActor* Self = Spawn(AActor::StaticClass(), FVector(100, 200, 300));
    Self->Tags.Add(TEXT("probe_tag"));
    auto* ChildComponent = NewObject<USceneComponent>(Self, TEXT("Child"));
    ChildComponent->SetupAttachment(Self->GetRootComponent());
    ChildComponent->ComponentTags.Add(TEXT("probe_tag"));
    ChildComponent->RegisterComponent();
    Other = Spawn(AActor::StaticClass(), FVector(1000, 0, 0));
    Child = ChildComponent;
    Pawn = Cast<APawn>(Spawn(APawn::StaticClass(), FVector(0, 0, 500)));
    Victim = Spawn(AActor::StaticClass(), FVector::ZeroVector);

    // Every bound method, with the actor as self; the script returns 0 or the number of the first failed check.
    const FString Path = R.ScriptPath(TEXT("native_engine_probe.das"));
    const int32 Flags = (World->HasBegunPlay() ? 1 : 0) | (World->GetGameInstance() ? 2 : 0);
    const auto Values = R.RunNamedPersistent(Path, TEXT("main"), Flags, Self);
    FString Detail = Values.bSucceeded ? FString() : Values.Error;
    if (Values.bSucceeded && Values.Value != 0)
        Detail = FString::Printf(TEXT("check %d failed"), Values.Value);
    Check(Values.bSucceeded && Values.Value == 0, TEXT("native_engine_values"), Detail);
    // The calls reached the real objects.
    const bool bApplied = Self->IsHidden() && Self->GetOwner() == Other.Get() && !ChildComponent->IsActive();
    Check(bApplied, TEXT("native_engine_changes_visible_in_cpp"));

    // ue_engine and the generated bindings together: AActor is the native type, the generated wrapper keeps its
    // full name, conversions work.
    const auto Mix = R.RunNamedPersistent(R.ScriptPath(TEXT("native_engine_generated_mix.das")), TEXT("main"), 0, Self);
    const bool bMixed = Mix.bSucceeded && Mix.Value == 0;
    Check(bMixed, TEXT("native_engine_with_generated_bindings"), Mix.bSucceeded ? FString() : Mix.Error.Left(300));
    // ue_engine next to the DSL: Actor (DSL base), AActor (native type) and ActorRef unqualified in one script.
    const auto Dsl = R.RunNamedPersistent(R.ScriptPath(TEXT("native_engine_dsl_mix.das")), TEXT("main"), 0, Self);
    const FString DslDetail = Dsl.bSucceeded ? FString::Printf(TEXT("check %d"), Dsl.Value) : Dsl.Error.Left(300);
    Check(Dsl.bSucceeded && Dsl.Value == 0, TEXT("native_engine_with_dsl"), DslDetail);

    // A null receiver and a destroyed actor are script errors.
    auto Refused = [&](const TCHAR* Name, const TCHAR* Entry, const TCHAR* Expected)
    {
        const auto Result = R.RunNamedPersistent(Path, Entry, 0, Self);
        Check(!Result.bSucceeded && Result.Error.Contains(Expected), Name, Result.Error.Left(300));
    };
    const TCHAR* NullReceiver = TEXT("AActor.get_actor_location: receiver is null or expired");
    Refused(TEXT("native_engine_null_actor_refused"), TEXT("null_actor"), NullReceiver);
    Refused(TEXT("native_engine_destroyed_actor_refused"), TEXT("destroyed_actor"), NullReceiver);
    const TCHAR* NullParent = TEXT("USceneComponent.attach_to: argument 1 is null or expired");
    Refused(TEXT("native_engine_null_attach_parent_refused"), TEXT("null_attach_parent"), NullParent);

    R.ReleaseObject(Self);
    for (AActor* Actor : {Self, Other.Get(), static_cast<AActor*>(Pawn.Get())})
        if (Actor)
            Actor->Destroy();
    if (!GameWorld)
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    }
    return bPassed;
}

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasNativeEngineTest, "DasForUE.NativeEngine",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasNativeEngineTest::RunTest(const FString&)
{
    FString Report;
    const bool bPassed = RunDasNativeEngineProbe(nullptr, Report);
    TArray<FString> Lines;
    Report.ParseIntoArrayLines(Lines);
    for (const FString& Line : Lines)
    {
        UE_LOG(LogTemp, Display, TEXT("DAS_NATIVE_ENGINE %s"), *Line);
        if (Line.StartsWith(TEXT("FAIL")))
            AddError(Line);
    }
    return bPassed;
}
#endif
