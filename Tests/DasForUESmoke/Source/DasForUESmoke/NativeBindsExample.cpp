// The example of Docs/Guide/Native/NativeBinds.md: a game module that binds its own C++ code for scripts. The guide
// quotes the [doc:...] blocks of this file and of Content/Das/Scripts/native_binds_example.das; Scripts/
// TestNativeBindsDocs.py keeps them equal. RunDasNativeBindsExample runs the script with a target as self (automation
// test DasForUE.NativeBindsExample, packaged flag -DasNativeBinds).
#include "Components/SceneComponent.h"
#include "DasNativeBinds.h"
#include "DasRuntime.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Misc/AutomationTest.h"
#include "Modules/ModuleManager.h"
#include "NativeBindsExampleObjects.h"

// [doc:module]
// A free function: arguments and results are ordinary C++ types.
static int32 DamageFor(int32 Base, float Multiplier)
{
    return FMath::RoundToInt(Base * Multiplier);
}
// A method can also be a function whose first parameter is the receiver.
static void Heal(AExampleTarget& Target, int32 Amount)
{
    if (Amount < 0)
        DasNativeError(TEXT("heal amount must not be negative")); // a script error after the call returns
    else
        Target.Health += Amount;
}
// Extends a type another module declared: every AActor (and AExampleTarget) gets this method.
static double DistanceToOrigin(const AActor& Actor)
{
    return Actor.GetActorLocation().Length();
}

DAS_NATIVE_BINDS(ExampleGame, "example_game")
{
    Binds.Function<&DamageFor>(TEXT("damage_for"), EDasBind::Pure);
    Binds.Class<AExampleTarget>(TEXT("AExampleTarget")) // the script type, named as in C++
        .Method<&AExampleTarget::GetHealth>(TEXT("get_health"))
        .Method<&AExampleTarget::IsAlive>(TEXT("is_alive"))
        .Method<&AExampleTarget::ApplyDamage>(TEXT("apply_damage"))
        .Method<&Heal>(TEXT("heal"));
    Binds.Class<AActor>().Method<&DistanceToOrigin>(TEXT("distance_to_origin"));
}
// [/doc]

// Returns false with the failed checks in Report. World: a game world to spawn into, or nullptr for a temporary one.
bool RunDasNativeBindsExample(UWorld* GameWorld, FString& Report)
{
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
        World = UWorld::CreateWorld(EWorldType::Game, false, TEXT("DasNativeBindsExample"));
        GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
        World->InitializeActorsForPlay(FURL());
    }
    const FVector Location(300, 400, 0);
    auto* Target = World->SpawnActor<AExampleTarget>(Location, FRotator::ZeroRotator);
    auto* Root = NewObject<USceneComponent>(Target, TEXT("Root"));
    Target->SetRootComponent(Root);
    Root->RegisterComponent();
    Target->SetActorLocation(Location);

    const FString Path = R.ScriptPath(TEXT("native_binds_example.das"));
    const auto Main = R.RunNamedPersistent(Path, TEXT("main"), 0, Target);
    const FString Detail = Main.bSucceeded ? FString::Printf(TEXT("check %d"), Main.Value) : Main.Error.Left(300);
    Check(Main.bSucceeded && Main.Value == 0, TEXT("native_binds_example_script"), Detail);
    // damage_for(10, 1.5) = 15, then heal(5): the script changed the C++ object.
    Check(Target->Health == 90, TEXT("native_binds_example_changes_visible_in_cpp"), FString::FromInt(Target->Health));
    auto Refused = [&](const TCHAR* Name, const TCHAR* Entry, const TCHAR* Expected)
    {
        const auto Result = R.RunNamedPersistent(Path, Entry, 0, Target);
        Check(!Result.bSucceeded && Result.Error.Contains(Expected), Name, Result.Error.Left(300));
    };
    Refused(TEXT("native_binds_example_error_reported"), TEXT("bad_heal"),
            TEXT("AExampleTarget.heal: heal amount must not be negative"));
    Refused(TEXT("native_binds_example_null_receiver_refused"), TEXT("null_target"),
            TEXT("AExampleTarget.get_health: receiver is null or expired"));

    R.ReleaseObject(Target);
    Target->Destroy();
    if (!GameWorld)
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    }
    return bPassed;
}

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasNativeBindsExampleTest, "DasForUE.NativeBindsExample",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasNativeBindsExampleTest::RunTest(const FString&)
{
    FString Report;
    const bool bPassed = RunDasNativeBindsExample(nullptr, Report);
    TArray<FString> Lines;
    Report.ParseIntoArrayLines(Lines);
    for (const FString& Line : Lines)
    {
        UE_LOG(LogTemp, Display, TEXT("DAS_NATIVE_BINDS_EXAMPLE %s"), *Line);
        if (Line.StartsWith(TEXT("FAIL")))
            AddError(Line);
    }
    return bPassed;
}
#endif
