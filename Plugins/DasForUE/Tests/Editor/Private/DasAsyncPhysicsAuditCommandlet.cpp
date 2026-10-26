#include "DasAsyncPhysicsAuditCommandlet.h"
#include "DasRuntime.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "GameFramework/WorldSettings.h"
#include "Physics/Experimental/PhysicsThreadLibrary.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "Chaos/Framework/PhysicsSolverBase.h"
#include "Framework/Threading.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/StructOnScope.h"
#include "UObject/UObjectIterator.h"
#include "UObject/GarbageCollection.h"
#include "Misc/Parse.h"
#include "Misc/ScopeExit.h"
void UDasAsyncPhysicsAuditBody::AsyncPhysicsTickComponent(float Dt, float SimTime)
{
    ++CallbackCount;
    AllGameThread &= IsInGameThread();
#if PHYSICS_THREAD_CONTEXT
    AllPhysicsContext &= Chaos::IsInPhysicsThreadContext();
    AllFrozenContext &= !Chaos::IsInGameThreadContext();
#else
    AllPhysicsContext = false;
    AllFrozenContext = false;
#endif
    // Acquire and consume only in this engine-owned presim callback. Never retain
    // or test a copied raw handle after callback exit / body destruction.
    auto Handle = GetBodyInstanceAsyncPhysicsTickHandle();
    if (Handle)
    {
        ++ValidCount;
        if (ForceMode)
        {
            auto* Fn = UPhysicsThreadLibrary::StaticClass()->FindFunctionByName(TEXT("AddForce"));
            FStructOnScope Frame(Fn);
            auto* H = FindFProperty<FStructProperty>(Fn, TEXT("Handle"));
            *H->ContainerPtrToValuePtr<FBodyInstanceAsyncPhysicsTickHandle>(Frame.GetStructMemory()) = Handle;
            auto* F = FindFProperty<FStructProperty>(Fn, TEXT("Force"));
            *F->ContainerPtrToValuePtr<FVector>(Frame.GetStructMemory()) = FVector(ForceMode == 1 ? 1000 : 100, 0, 0);
            FindFProperty<FBoolProperty>(
                Fn, TEXT("bAccelChange"))->SetPropertyValue_InContainer(Frame.GetStructMemory(), ForceMode == 2);
            UPhysicsThreadLibrary::StaticClass()->GetDefaultObject()->ProcessEvent(Fn, Frame.GetStructMemory());
            ForcedTime += Dt;
        }
    }
}
UDasAsyncPhysicsAuditCommandlet::UDasAsyncPhysicsAuditCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasAsyncPhysicsAuditCommandlet::Main(const FString& Params)
{
    FString Probe;
    if (!FParse::Value(*Params, TEXT("Probe="), Probe))
        return 1;
    int32 Failures = 0, Checks = 0;
    auto Check = [&](bool Good, const TCHAR* Name)
    {
        ++Checks;
        UE_LOG(LogTemp, Display, TEXT("DAS_ASYNC_PHYSICS_%s %s"), Good ? TEXT("PASS"): TEXT("FAIL"), Name);
        if (!Good)
            ++Failures;
    };
    auto* Type = FBodyInstanceAsyncPhysicsTickHandle::StaticStruct();
    int32 Fields = 0;
    for (TFieldIterator<FProperty> It(Type); It; ++It)
        ++Fields;
    Check(Fields == 0, TEXT("raw_proxy_is_not_reflected_storage"));
    int32 Producers = 0, Consumers = 0;
    for (TObjectIterator<UFunction> It; It; ++It)
    {
        bool In = false, Out = false;
        for (TFieldIterator<FProperty> P(*It); P; ++P)
            if (P->HasAnyPropertyFlags(CPF_Parm))
                if (auto* S = CastField<FStructProperty>(*P); S && S->Struct == Type)
                {
                    Out |= P->HasAnyPropertyFlags(CPF_ReturnParm);
                    In |= !P->HasAnyPropertyFlags(CPF_ReturnParm);
                }
        if (In || Out)
            UE_LOG(LogTemp, Display,
                   TEXT("DAS_ASYNC_PHYSICS_SURFACE %s input=%d return=%d"), *It->GetPathName(), In, Out);
        Producers += Out;
        Consumers += In;
    }
    Check(Producers == 1 && Consumers == 1, TEXT("loaded_reflected_surface_one_getter_one_AddForce"));
    auto* Settings = GetMutableDefault<UPhysicsSettings>();
    const bool OldAsync = Settings->bTickPhysicsAsync;
    const float OldDt = Settings->AsyncFixedTimeStepSize;
    const bool AsyncFixed = FParse::Param(*Params, TEXT("AsyncFixed"));
    Settings->bTickPhysicsAsync = AsyncFixed;
    Settings->AsyncFixedTimeStepSize = 1.f / 60.f;
    ON_SCOPE_EXIT
    {
        Settings->bTickPhysicsAsync = OldAsync;
        Settings->AsyncFixedTimeStepSize = OldDt;
    };
    UE_LOG(LogTemp, Display, TEXT("DAS_ASYNC_PHYSICS_MODE fixed_async=%d"), AsyncFixed);
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
    auto* Actor = World->SpawnActor<AActor>();
    auto* Box = NewObject<UDasAsyncPhysicsAuditBody>(Actor);
    Actor->AddInstanceComponent(Box);
    Actor->SetRootComponent(Box);
    Box->SetBoxExtent(FVector(20));
    Box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Box->SetCollisionResponseToAllChannels(ECR_Ignore);
    Box->RegisterComponent();
    Box->Activate(true);
    Box->SetEnableGravity(false);
    Box->SetLinearDamping(0);
    Box->SetSimulatePhysics(true);
    Box->SetMassOverrideInKg(NAME_None, 10, true);
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    ON_SCOPE_EXIT
    {
        Runtime.ReleaseObject(Actor);
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    World->BeginPlay();
    World->GetWorldSettings()->NotifyBeginPlay();
    Box->EnableAuditTick(true);
    constexpr float Dt = 1.f / 60.f;
    auto Step = [&](int32 N)
    {
        for (int32 I = 0; I < N; ++I)
        {
            ++GFrameCounter;
            World->Tick(LEVELTICK_All, Dt);
        }
    };
    Step(3);
    Check(Box->CallbackCount > 0 && Box->ValidCount > 0 && Box->AllGameThread && Box->AllPhysicsContext &&
              Box->AllFrozenContext,
          TEXT("engine_callback_is_game_thread_in_internal_physics_context"));
    UE_LOG(LogTemp, Display,
           TEXT("DAS_ASYNC_PHYSICS_PHASE callbacks=%d valid=%d GT=%d PTcontext=%d frozen=%d context_checks=%d"),
                Box->CallbackCount, Box->ValidCount, Box->AllGameThread, Box->AllPhysicsContext, Box->AllFrozenContext,
                PHYSICS_THREAD_CONTEXT);
    Box->SetPhysicsLinearVelocity(FVector::ZeroVector);
    Step(3);
    Box->ForceMode = 1;
    Box->ForcedTime = 0;
    Step(30);
    Box->ForceMode = 0;
    Step(3);
    const double VForce = Box->GetPhysicsLinearVelocity().X;
    Check(Box->ForcedTime > 0 && FMath::Abs(VForce - 100 * Box->ForcedTime) < 1,
          TEXT("native_consumer_force_integrates_on_presim_steps"));
    UE_LOG(LogTemp, Display,
           TEXT("DAS_ASYNC_PHYSICS_FORCE velocity=%g expected=%g seconds=%g"), VForce, 100 * Box->ForcedTime,
                Box->ForcedTime);
    Box->SetPhysicsLinearVelocity(FVector::ZeroVector);
    Step(3);
    Box->ForceMode = 2;
    Box->ForcedTime = 0;
    Step(30);
    Box->ForceMode = 0;
    Step(3);
    const double VAccel = Box->GetPhysicsLinearVelocity().X;
    Check(Box->ForcedTime > 0 && FMath::Abs(VAccel - 100 * Box->ForcedTime) < 1,
          TEXT("native_consumer_acceleration_scales_by_mass"));
    UE_LOG(LogTemp, Display,
           TEXT("DAS_ASYNC_PHYSICS_ACCEL velocity=%g expected=%g seconds=%g"), VAccel, 100 * Box->ForcedTime,
                Box->ForcedTime);
    Box->ForceMode = 0;
    auto Handle = Box->GetBodyInstanceAsyncPhysicsTickHandle();
    auto Stamp = Handle.Proxy ? Handle.Proxy->GetSyncTimestamp() : nullptr;
    Check(Stamp && !Stamp->bDeleted, TEXT("live_proxy_generation_marker"));
    Box->RecreatePhysicsState();
    auto Fresh = Box->GetBodyInstanceAsyncPhysicsTickHandle();
    auto FreshStamp = Fresh.Proxy ? Fresh.Proxy->GetSyncTimestamp() : nullptr;
    Check(Stamp && Stamp->bDeleted && FreshStamp && !FreshStamp->bDeleted && FreshStamp != Stamp,
          TEXT("recreate_invalidates_generation_without_dereferencing_old_handle"));
    const int32 BeforeRecreatedCallbacks = Box->ValidCount;
    Step(2);
    Check(Box->ValidCount > BeforeRecreatedCallbacks, TEXT("recreated_body_receives_callbacks"));
    // VM use is rejected before acquiring an escaping raw capability, even though
    // ordinary game-thread execution satisfies the runtime's usual thread guard.
    for (int32 Mode = 0; Mode < 2; ++Mode)
    {
        const auto R = Runtime.RunFile(Probe, Mode, false, Actor);
        Check(!R.bSucceeded && R.Error.Contains(TEXT("scoped frozen-game-thread physics callback adapter")),
                                                Mode == 0 ? TEXT("ordinary_DAS_getter_rejected")
                                                          : TEXT("ordinary_DAS_consumer_rejected"));
        if (R.bSucceeded || !R.Error.Contains(TEXT("scoped frozen-game-thread physics callback adapter")))
            UE_LOG(LogTemp, Error, TEXT("mode=%d %s"), Mode, *R.Error);
    }
    const int32 Before = Box->CallbackCount;
    Box->EnableAuditTick(false);
    Step(2);
    Check(Box->CallbackCount == Before, TEXT("unregister_stops_callbacks"));
    TWeakObjectPtr<UDasAsyncPhysicsAuditBody> Weak = Box;
    Runtime.ReleaseObject(Actor);
    Actor->Destroy();
    Actor = nullptr;
    CollectGarbage(RF_NoFlags);
    Check(!Weak.IsValid() && FreshStamp && FreshStamp->bDeleted,
          TEXT("generation_marker_does_not_pin_destroyed_component"));
    UE_LOG(LogTemp, Display,
           TEXT("DAS_ASYNC_PHYSICS_AUDIT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
