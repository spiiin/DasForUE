#include "DasPawnAICommandlet.h"
#include "DasRuntime.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NavigationData.h"
#include "UObject/StrongObjectPtr.h"
#include "Misc/ScopeExit.h"
UDasPawnAICommandlet::UDasPawnAICommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasPawnAICommandlet::Main(const FString& Params)
{
    FString Probe;
    if (!FParse::Value(*Params, TEXT("Probe="), Probe))
        return 1;
    const auto IVS = UWorld::InitializationValues()
                         .CreatePhysicsScene(true)
                         .ShouldSimulatePhysics(false)
                         .AllowAudioPlayback(false)
                         .CreateNavigation(false)
                         .CreateAISystem(false);
    TStrongObjectPtr<UWorld> World(
        UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &IVS));
    if (!World.IsValid())
        return 1;
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    World->InitializeActorsForPlay(FURL());
    auto* Character = World->SpawnActor<ACharacter>();
    auto* Controller = World->SpawnActor<AAIController>();
    if (!Character || !Controller)
        return 1;
    Controller->Possess(Character);
    auto* Perception = NewObject<UAIPerceptionComponent>(Controller);
    Controller->AddInstanceComponent(Perception);
    Perception->RegisterComponent();
    Controller->SetPerceptionComponent(*Perception);
    TStrongObjectPtr<UNavigationPath> EmptyPath(NewObject<UNavigationPath>(World.Get()));
    TStrongObjectPtr<UNavigationPath> RoutePath(NewObject<UNavigationPath>(World.Get()));
    const TArray<FVector> Points{FVector(0, 0, 0), FVector(100, 0, 0), FVector(100, 100, 0)};
    FNavPathSharedPtr NativePath = MakeShared<FNavigationPath, ESPMode::ThreadSafe>(Points);
    NativePath->MarkReady();
    NativePath->SetIsPartial(true);
    RoutePath->SetPath(NativePath);
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    ON_SCOPE_EXIT
    {
        for (UObject* Object :
             {static_cast<UObject*>(Character), static_cast<UObject*>(Controller), static_cast<UObject*>(Perception),
              static_cast<UObject*>(EmptyPath.Get()), static_cast<UObject*>(RoutePath.Get())})
            Runtime.ReleaseObject(Object);
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    World->BeginPlay();
    World->GetWorldSettings()->NotifyBeginPlay();
    auto Run = [&](int32 Phase, UObject* Self, const TCHAR* Name)
    {
        const auto R = Runtime.RunFile(Probe, Phase, false, Self);
        if (!R.bSucceeded || R.Value != 42)
        {
            UE_LOG(LogTemp, Error, TEXT("DAS_PAWN_AI_FAIL %s value=%d error=%s"), Name, R.Value, *R.Error);
            return false;
        }
        return true;
    };
    auto Check = [&](bool Good, const TCHAR* Name)
    {
        UE_LOG(LogTemp, Display, TEXT("DAS_PAWN_AI_%s %s"), Good ? TEXT("PASS"): TEXT("FAIL"), Name);
        return Good;
    };
    if (!Run(0, Character, TEXT("pawn_character")))
        return 1;
    if (!Check(Character->GetPendingMovementInputVector().IsNearlyZero() &&
                   FMath::IsNearlyEqual(Character->GetLastMovementInputVector().X, 0.5) &&
                   Character->GetCharacterMovement()->MovementMode == MOVE_Flying &&
                   Character->GetCharacterMovement()->PendingLaunchVelocity.Equals(FVector(100, 0, 150)),
               TEXT("pawn_input_character_mode_and_launch")))
        return 1;
    if (!Run(1, Controller, TEXT("controller")))
        return 1;
    if (!Check(Controller->GetPawn() == Character && Character->GetController() == Controller &&
                   Controller->GetFocusActor() == nullptr,
               TEXT("ai_possession_focus_clear")))
        return 1;
    if (!Run(2, EmptyPath.Get(), TEXT("empty_path")))
        return 1;
    if (!Check(!EmptyPath->IsValid() && EmptyPath->GetPathLength() == -1, TEXT("empty_navigation_path")))
        return 1;
    if (!Run(3, RoutePath.Get(), TEXT("constructed_path")))
        return 1;
    if (!Check(RoutePath->IsValid() && RoutePath->IsPartial() &&
                   FMath::IsNearlyEqual(RoutePath->GetPathLength(), 200.0),
               TEXT("constructed_navigation_path_length_and_partial")))
        return 1;
    if (!Run(4, Perception, TEXT("perception")))
        return 1;
    if (!Check(Perception->IsRegistered() && Perception->GetOwner() == Controller,
               TEXT("perception_arrays_and_receiver_guards")))
        return 1;
    if (!Check(UNavigationSystemV1::GetNavigationSystem(Character) == nullptr,
               TEXT("navigation_fixture_has_no_system")))
        return 1;
    if (!Run(5, Character, TEXT("native_navigation_statics")))
        return 1;
    if (!Check(!UNavigationSystemV1::IsNavigationBeingBuilt(Character) &&
                   !UNavigationSystemV1::IsNavigationBeingBuiltOrLocked(Character),
               TEXT("native_navigation_static_queries")))
        return 1;
    UE_LOG(LogTemp, Display, TEXT("DAS_PAWN_AI_RUNTIME=PASS"));
    return 0;
}
