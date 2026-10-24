// Gameplay method calls answered by native binds (DasNativeBinds.h, Reflects): RunDasNativeRoutesProbe runs every
// entry of Content/Das/Scripts/native_routes_probe.das (the binds) and native_routes_reflected.das (the same code
// with `options ue_native_routes = false`, reflection) with an actor in a game world as self. Both must give the
// same results and errors; DasNativeRoutedCalls shows which path ran. Used by the automation test
// DasForUE.NativeRoutes (temporary world) and by the packaged flag -DasNativeBinds.
#include "Components/SceneComponent.h"
#include "DasNativeBinds.h"
#include "DasRuntime.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "HAL/PlatformTime.h"
#include "Misc/AutomationTest.h"
#include "Modules/ModuleManager.h"

namespace DasNativeRoutesProbe
{
TWeakObjectPtr<APawn> Pawn;
TWeakObjectPtr<AActor> Victim;
UObject* GetPawn()
{
    return Pawn.Get();
}
UObject* GetVictim()
{
    return Victim.Get();
}
void DestroyVictim()
{
    if (AActor* Actor = Victim.Get())
        Actor->Destroy();
}

// Refused Reflects: each module binds one wrong method on the AActor type of ue_engine.
bool SetLocation(AActor& A, const FVector& V)
{
    return A.SetActorLocation(V);
}
void BeginPlay(AActor&) {}
float DistanceTo(const AActor& A, const AActor* Other)
{
    return A.GetDistanceTo(Other);
}
FVector Location(const AActor& A)
{
    return A.GetActorLocation();
}
void BuildOtherSignature(FDasNativeModule& Binds)
{
    Binds.Class<AActor>().Method<&SetLocation>(TEXT("bad_set_location")).Reflects(TEXT("K2_SetActorLocation"));
}
void BuildEvent(FDasNativeModule& Binds)
{
    Binds.Class<AActor>().Method<&BeginPlay>(TEXT("bad_begin_play")).Reflects(TEXT("ReceiveBeginPlay"));
}
void BuildNonNull(FDasNativeModule& Binds)
{
    Binds.Class<AActor>()
        .Method<&DistanceTo>(TEXT("bad_distance_to"), EDasBind::NonNullArgs)
        .Reflects(TEXT("GetDistanceTo"));
}
void BuildMissing(FDasNativeModule& Binds)
{
    Binds.Class<AActor>().Method<&Location>(TEXT("bad_location")).Reflects(TEXT("NoSuchFunction"));
}
void BuildTaken(FDasNativeModule& Binds)
{
    Binds.Class<AActor>().Method<&Location>(TEXT("bad_location")).Reflects(TEXT("K2_GetActorLocation"));
}
void BuildTwice(FDasNativeModule& Binds)
{
    Binds.Class<AActor>()
        .Method<&Location>(TEXT("bad_location"))
        .Reflects(TEXT("K2_GetActorLocation"))
        .Reflects(TEXT("GetActorLocation"));
}
}

DAS_NATIVE_BINDS(SmokeRoutes, "smoke_routes")
{
    using namespace DasNativeRoutesProbe;
    Binds.Function<&GetPawn>(TEXT("smoke_routes_pawn"));
    Binds.Function<&GetVictim>(TEXT("smoke_routes_victim"));
    Binds.Function<&DestroyVictim>(TEXT("smoke_routes_destroy_victim"));
}

// Returns false with the failed checks in Report. World: a game world to spawn into, or nullptr for a temporary one.
bool RunDasNativeRoutesProbe(UWorld* GameWorld, FString& Report)
{
    using namespace DasNativeRoutesProbe;
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    bool bPassed = true;
    auto Check = [&](bool bOK, const FString& Name, const FString& Detail = FString())
    {
        bPassed &= bOK;
        Report += FString::Printf(TEXT("%s %s"), bOK ? TEXT("PASS") : TEXT("FAIL"), *Name);
        if (!Detail.IsEmpty())
            Report += TEXT(": ") + Detail;
        Report += TEXT("\n");
    };
    UWorld* World = GameWorld;
    if (!World)
    {
        World = UWorld::CreateWorld(EWorldType::Game, false, TEXT("DasNativeRoutesProbe"));
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
    Self->SetActorRotation(FRotator(0, 90, 0));
    Self->Tags.Add(TEXT("probe_tag"));
    Pawn = Cast<APawn>(Spawn(APawn::StaticClass(), FVector(0, 0, 500)));

    const FString Routed = R.ScriptPath(TEXT("native_routes_probe.das"));
    const FString Reflected = R.ScriptPath(TEXT("native_routes_reflected.das"));
    // Runs Entry in Path; Calls: how many gameplay calls the binds answered.
    auto Run = [&](const FString& Path, const TCHAR* Entry, int32 Input, uint64& Calls)
    {
        const uint64 Before = DasNativeRoutedCalls();
        const auto Result = R.RunNamedPersistent(Path, Entry, Input, Self);
        Calls = DasNativeRoutedCalls() - Before;
        return Result;
    };
    // Expected: what the entry returns (0: its own check passed); bRouted: a bind answers it.
    auto Case = [&](const TCHAR* Entry, int32 Expected, bool bRouted, TFunctionRef<bool()> After)
    {
        uint64 RoutedCalls = 0, ReflectedCalls = 0;
        const auto B = Run(Reflected, Entry, 0, ReflectedCalls);
        const bool bAfterB = After();
        const auto A = Run(Routed, Entry, 0, RoutedCalls);
        const bool bAfterA = After();
        Check(A.bSucceeded && A.Value == Expected && bAfterA && RoutedCalls == (bRouted ? 1 : 0),
              FString(TEXT("native_routes_")) + Entry,
              FString::Printf(TEXT("value %d, bind calls %llu %s"), A.Value, RoutedCalls, *A.Error.Left(300)));
        Check(B.bSucceeded && B.Value == Expected && bAfterB && ReflectedCalls == 0,
              FString(TEXT("native_routes_reflected_")) + Entry,
              FString::Printf(TEXT("value %d, bind calls %llu %s"), B.Value, ReflectedCalls, *B.Error.Left(300)));
    };
    auto None = []
    {
        return true;
    };
    Case(TEXT("actor_location"), 0, true, None);
    Case(TEXT("actor_rotation"), 0, true, None);
    Case(TEXT("actor_forward"), 0, true, None);
    Case(TEXT("actor_transform"), 0, true, None);
    Case(TEXT("set_actor_scale"), 0, true,
         [&]
         {
             const bool bScaled = Self->GetActorScale3D().Equals(FVector(2, 2, 2));
             Self->SetActorScale3D(FVector::OneVector);
             return bScaled;
         });
    Case(TEXT("actor_has_tag"), 0, true, None);
    Case(TEXT("actor_life_span"), 0, true, None);
    Case(TEXT("set_actor_hidden"), 0, true,
         [&]
         {
             const bool bHidden = Self->IsHidden();
             Self->SetActorHiddenInGame(false);
             return bHidden;
         });
    Case(TEXT("component_location"), 0, true, None);
    Case(TEXT("pawn_location"), 0, true, None);
    Case(TEXT("pawn_locally_controlled"), 0, true, None);
    // Policies: construction-unsafe and authority-only UFUNCTIONs keep the reflected call.
    Case(TEXT("component_is_active"), Self->GetRootComponent()->IsActive() ? 1 : 0, false, None);
    Case(TEXT("set_replicates"), 0, false, None);

    // A destroyed receiver: both paths refuse it with the same message (the files differ only in the option line).
    FString Errors[2];
    for (int32 I = 0; I < 2; ++I)
    {
        Victim = Spawn(AActor::StaticClass(), FVector::ZeroVector);
        uint64 Calls = 0;
        const auto Result = Run(I == 0 ? Routed : Reflected, TEXT("destroyed_receiver"), 0, Calls);
        Errors[I] = Result.bSucceeded ? FString() : Result.Error;
        Errors[I].ReplaceInline(TEXT("native_routes_reflected.das"), TEXT("native_routes_probe.das"));
        // The call stack differs: the routed call has the wrapper's frame.
        Errors[I] = Errors[I].Left(Errors[I].Find(TEXT("CALL STACK")));
    }
    const bool bRuntime = !Errors[0].IsEmpty() && !Errors[0].Contains(TEXT("[DAS:compile]"));
    Check(bRuntime && Errors[0] == Errors[1], TEXT("native_routes_destroyed_receiver_same_error"),
          Errors[0].Left(300) + TEXT(" | ") + Errors[1].Left(300));

    // Cost of one call on each path (informational; the machine's speed varies between runs).
    auto Time = [&](const FString& Path)
    {
        constexpr int32 Count = 1 << 14;
        uint64 Calls = 0;
        Run(Path, TEXT("bench"), 16, Calls); // warm-up
        const double Start = FPlatformTime::Seconds();
        const auto Result = Run(Path, TEXT("bench"), Count, Calls);
        const double Ns = (FPlatformTime::Seconds() - Start) * 1e9 / Count;
        return Result.bSucceeded && Result.Value == Count ? Ns : -1.0;
    };
    const double RoutedNs = Time(Routed), ReflectedNs = Time(Reflected);
    Check(RoutedNs > 0 && ReflectedNs > 0, TEXT("native_routes_bench"),
          FString::Printf(TEXT("K2_GetActorLocation: bind %.0f ns, reflection %.0f ns a call"), RoutedNs,
                          ReflectedNs));

    // Refused Reflects: the module reports why (DasNativeModuleStatus) and is not built.
    const FString Suffix = FGuid::NewGuid().ToString(EGuidFormats::Digits).Left(8).ToLower();
    struct FBad
    {
        const TCHAR* Name;
        void (*Build)(FDasNativeModule&);
        const TCHAR* Expected;
    };
    const FBad Bad[] = {
        {TEXT("other_signature"), &BuildOtherSignature,
         TEXT("reflects UFUNCTION K2_SetActorLocation, which has another type of parameter bSweep")},
        {TEXT("event"), &BuildEvent, TEXT("reflects UFUNCTION ReceiveBeginPlay, which is an event")},
        {TEXT("non_null_args"), &BuildNonNull, TEXT("reflects UFUNCTION GetDistanceTo, which is NonNullArgs")},
        {TEXT("missing"), &BuildMissing, TEXT("which is not a UFUNCTION of class Actor")},
        {TEXT("taken"), &BuildTaken, TEXT("which is already reflected by 'get_actor_location' (module 'ue_engine')")},
        {TEXT("twice"), &BuildTwice, TEXT("must follow a Method that reflects nothing yet")}};
    for (const FBad& B : Bad)
        RegisterDasNativeModule(*(FString(TEXT("smoke_reflects_")) + B.Name + TEXT("_") + Suffix), B.Build);
    DescribeDasNativeModules(); // builds the newly registered modules
    for (const FBad& B : Bad)
    {
        const FString Module = FString(TEXT("smoke_reflects_")) + B.Name + TEXT("_") + Suffix;
        const FString Why = DasNativeModuleStatus(*Module);
        Check(Why.Contains(B.Expected), FString(TEXT("native_routes_refused_")) + B.Name, Why);
        UnregisterDasNativeModule(*Module);
    }

    R.ReleaseObject(Self);
    for (AActor* Actor : {Self, static_cast<AActor*>(Pawn.Get())})
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
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasNativeRoutesTest, "DasForUE.NativeRoutes",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasNativeRoutesTest::RunTest(const FString&)
{
    // The refused registrations log their reason once.
    const auto Contains = EAutomationExpectedErrorFlags::Contains;
    AddExpectedError(TEXT("which has another type of parameter"), Contains, 1);
    AddExpectedError(TEXT("which is an event"), Contains, 1);
    AddExpectedError(TEXT("which is NonNullArgs"), Contains, 1);
    AddExpectedError(TEXT("which is not a UFUNCTION"), Contains, 1);
    AddExpectedError(TEXT("which is already reflected by"), Contains, 1);
    AddExpectedError(TEXT("must follow a Method that reflects nothing yet"), Contains, 1);
    FString Report;
    const bool bPassed = RunDasNativeRoutesProbe(nullptr, Report);
    TArray<FString> Lines;
    Report.ParseIntoArrayLines(Lines);
    for (const FString& Line : Lines)
    {
        UE_LOG(LogTemp, Display, TEXT("DAS_NATIVE_ROUTES %s"), *Line);
        if (Line.StartsWith(TEXT("FAIL")))
            AddError(Line);
    }
    return bPassed;
}
#endif
