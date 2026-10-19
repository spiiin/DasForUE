#include "DasConstraintSimulationCommandlet.h"
#include "DasRuntime.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/WorldSettings.h"
#include "Components/BoxComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "UObject/StrongObjectPtr.h"
#include "Misc/ScopeExit.h"

UDasConstraintSimulationCommandlet::UDasConstraintSimulationCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasConstraintSimulationCommandlet::Main(const FString& Params)
{
    FString Probe;
    if (!FParse::Value(*Params, TEXT("Probe="), Probe))
        return 1;
    const auto IVS = UWorld::InitializationValues()
                         .CreatePhysicsScene(true)
                         .ShouldSimulatePhysics(true)
                         .AllowAudioPlayback(false)
                         .CreateNavigation(false)
                         .CreateAISystem(false);
    TStrongObjectPtr<UWorld> World(
        UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &IVS));
    if (!World.IsValid() || !World->GetPhysicsScene())
        return 1;
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    World->InitializeActorsForPlay(FURL());
    AActor* Actors[2]{};
    UBoxComponent* Bodies[2]{};
    for (int32 I = 0; I < 2; ++I)
    {
        Actors[I] = World->SpawnActor<AActor>();
        auto* Body = NewObject<UBoxComponent>(Actors[I]);
        Bodies[I] = Body;
        Actors[I]->SetRootComponent(Body);
        Actors[I]->AddInstanceComponent(Body);
        Body->SetMobility(EComponentMobility::Movable);
        Body->SetBoxExtent(FVector(20));
        Body->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        Body->SetCollisionResponseToAllChannels(ECR_Ignore);
        Body->RegisterComponent();
        Body->SetEnableGravity(false);
        Body->SetSimulatePhysics(true);
        Body->SetMassOverrideInKg(NAME_None, 1.f, true);
        Body->SetLinearDamping(0);
        Body->SetAngularDamping(0);
    }
    auto* Joint = NewObject<UPhysicsConstraintComponent>(Actors[0]);
    Actors[0]->AddInstanceComponent(Joint);
    Joint->RegisterComponent();
    Joint->SetConstrainedComponents(Bodies[0], NAME_None, Bodies[1], NAME_None);
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    ON_SCOPE_EXIT
    {
        Joint->TermComponentConstraint();
        Runtime.ReleaseObject(Actors[0]);
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    World->BeginPlay();
    World->GetWorldSettings()->NotifyBeginPlay();
    if (!Actors[0]->HasActorBegunPlay() || !Actors[1]->HasActorBegunPlay() || !Bodies[0]->IsSimulatingPhysics() ||
        !Bodies[1]->IsSimulatingPhysics())
        return 1;
    constexpr float Dt = 1.f / 60.f;
    int32 Steps = 0;
    auto Step = [&](int32 Frames)
    {
        for (int32 I = 0; I < Frames; ++I)
        {
            ++GFrameCounter;
            ++Steps;
            World->Tick(LEVELTICK_All, Dt);
        }
    };
    auto Reset = [&]()
    {
        Joint->TermComponentConstraint();
        for (auto* Body : Bodies)
        {
            Body->SetWorldLocationAndRotation(FVector::ZeroVector, FQuat::Identity, false, nullptr,
                                              ETeleportType::TeleportPhysics);
            Body->SetPhysicsLinearVelocity(FVector::ZeroVector);
            Body->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
            Body->WakeAllRigidBodies();
        }
        Joint->SetWorldLocationAndRotation(FVector::ZeroVector, FQuat::Identity);
        Joint->InitComponentConstraint();
    };
    auto Frame = [&](int32 I)
    {
        return Joint->ConstraintInstance.GetRefFrame(I == 0 ? EConstraintFrame::Frame1 : EConstraintFrame::Frame2) *
               Bodies[I]->GetComponentTransform();
    };
    auto LinearError = [&]()
    {
        return Frame(0).InverseTransformPosition(Frame(1).GetLocation());
    };
    auto Angle = [&]()
    {
        return FMath::RadiansToDegrees(Frame(0).GetRotation().AngularDistance(Frame(1).GetRotation()));
    };
    auto Check = [&](bool Good, const TCHAR* Name)
    {
        UE_LOG(LogTemp, Display,
               TEXT("DAS_CONSTRAINT_SIM_%s %s step=%d body1=%s body2=%s local_delta=%s relative_angle=%g broken=%d"),
                    Good ? TEXT("PASS")
                         : TEXT("FAIL"), Name, Steps, *Bodies[0]->GetComponentLocation().ToString(),
                                *Bodies[1]->GetComponentLocation().ToString(), *LinearError().ToString(), Angle(),
                                Joint->IsBroken());
        return Good;
    };
    auto Run = [&](int32 Phase)
    {
        auto R = Runtime.RunFile(Probe, Phase, false, Actors[0]);
        if (!R.bSucceeded || R.Value != 42)
        {
            UE_LOG(LogTemp, Error,
                   TEXT("DAS_CONSTRAINT_SIM_PHASE_FAIL phase=%d value=%d error=%s"), Phase, R.Value, *R.Error);
            return false;
        }
        return true;
    };
    Reset();
    if (!Run(0))
        return 1;
    Step(30);
    const double ComX = (Bodies[0]->GetComponentLocation().X + Bodies[1]->GetComponentLocation().X) / 2;
    if (!Check(LinearError().Size() < 2 && ComX > 20 && ComX < 30 && Bodies[0]->GetComponentLocation().X > 15,
               TEXT("locked_linear_transfers_impulse")))
        return 1;
    Reset();
    if (!Run(1))
        return 1;
    Step(30);
    if (!Check(FMath::Abs(LinearError().X) > 40 && FMath::Abs(LinearError().X) < 60 &&
                   Bodies[0]->GetComponentLocation().Size() < 2,
               TEXT("free_linear_axis")))
        return 1;
    Reset();
    if (!Run(2))
        return 1;
    Step(60);
    if (!Check(FMath::Abs(LinearError().X) > 10 && FMath::Abs(LinearError().X) < 28 &&
                   FMath::Abs(LinearError().Y) < 2 && FMath::Abs(LinearError().Z) < 2,
               TEXT("limited_linear_axis")))
        return 1;
    Reset();
    if (!Run(3))
        return 1;
    Step(30);
    if (!Check(Angle() < 3 && FMath::RadiansToDegrees(Bodies[0]->GetComponentQuat().GetAngle()) > 10,
               TEXT("locked_angular_transfers_rotation")))
        return 1;
    Reset();
    if (!Run(4))
        return 1;
    Step(30);
    if (!Check(Angle() > 35 && Angle() < 55 && FMath::RadiansToDegrees(Bodies[0]->GetComponentQuat().GetAngle()) < 3,
               TEXT("free_angular_twist")))
        return 1;
    Reset();
    if (!Run(5))
        return 1;
    Step(60);
    if (!Check(Angle() > 5 && Angle() < 24, TEXT("limited_angular_twist")))
        return 1;
    Reset();
    if (!Run(6))
        return 1;
    Step(30);
    if (!Check(Joint->IsBroken() &&
                   FVector::Dist(Bodies[0]->GetComponentLocation(), Bodies[1]->GetComponentLocation()) > 40,
               TEXT("break_releases_bodies_and_rejects_handle")))
        return 1;
    Reset();
    if (!Run(7))
        return 1;
    Step(30);
    if (!Check(!Joint->IsBroken() && LinearError().Size() < 2, TEXT("recreate_rejects_old_pending_and_snapshot")))
        return 1;
    Reset();
    if (!Run(8))
        return 1;
    Step(18);
    if (!Check(FMath::Abs(LinearError().X) > 25, TEXT("dynamic_linear_free_precondition")))
        return 1;
    if (!Run(9))
        return 1;
    Step(90);
    if (!Check(FMath::Abs(LinearError().X) < 14, TEXT("runtime_linear_limit_update")))
        return 1;
    Reset();
    if (!Run(10))
        return 1;
    Step(18);
    if (!Check(Angle() > 23, TEXT("dynamic_angular_free_precondition")))
        return 1;
    if (!Run(11))
        return 1;
    Step(90);
    if (!Check(Angle() < 14, TEXT("runtime_angular_limit_update")))
        return 1;
    UE_LOG(LogTemp, Display, TEXT("DAS_CONSTRAINT_SIMULATION=PASS steps=%d fixed_dt=%g"), Steps, Dt);
    return 0;
}
