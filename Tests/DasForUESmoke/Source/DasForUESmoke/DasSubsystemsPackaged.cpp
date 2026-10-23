#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "DasRuntime.h"
#include "DasSubsystems.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "HAL/PlatformProperties.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/CoreDelegates.h"
#include "Misc/Paths.h"
#include "UObject/UnrealType.h"
#include "UObject/UObjectGlobals.h"
namespace
{
constexpr const TCHAR* WorldPath = TEXT("/Game/DasSubsystems/Packaged/BP_Missions.BP_Missions_C");
constexpr const TCHAR* SessionPath = TEXT("/Game/DasSubsystems/Packaged/BP_Session.BP_Session_C");
int Read(UObject* Object, const TCHAR* Name)
{
    auto* Property = Object ? FindFProperty<FIntProperty>(Object->GetClass(), Name) : nullptr;
    return Property ? Property->GetPropertyValue_InContainer(Object) : -1;
}
int Advance(UObject* Object)
{
    auto* Function = Object ? Object->FindFunction(TEXT("Advance")) : nullptr;
    if (!Function)
        return -1;
    int Result = -1;
    Object->ProcessEvent(Function, &Result);
    return Result;
}
struct FSubsystemPackageProbe
{
    // Observational addresses only. PreGC clears them before memory can be reclaimed.
    UDasWorldSubsystem* WorldService = nullptr;
    UDasGameInstanceSubsystem* SessionService = nullptr;
    bool WorldEnded = false, SessionEnded = false, EndErrors = false, Good = true;
    int Checks = 0;
    FDelegateHandle BeforeGC, EngineExit;
    void Check(bool OK, const TCHAR* Name)
    {
        ++Checks;
        Good &= OK;
        UE_LOG(LogTemp, Display, TEXT("DAS_SUBSYSTEMS_PACKAGED_%s %s"), OK ? TEXT("PASS") : TEXT("FAIL"), Name);
    }
    template <typename T> void Observe(T*& Service, bool& Ended)
    {
        if (Service && (!IsValid(Service) || !Service->IsDasActive()))
        {
            Ended = !Service->IsDasActive() && Read(Service, TEXT("Starts")) == 1 && Read(Service, TEXT("Stops")) == 1;
            EndErrors |= !Service->LastDasError.IsEmpty();
            Service = nullptr;
        }
    }
    void ObserveEnds()
    {
        Observe(WorldService, WorldEnded);
        Observe(SessionService, SessionEnded);
    }
    void Finish()
    {
        // UGameEngine::PreExit already called GI::Shutdown and World::CleanupWorld.
        // Runtime's OnPreExit handler has not shut the VM down yet.
        ObserveEnds();
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        Check(WorldEnded, TEXT("natural_world_Deinitialize_once"));
        Check(SessionEnded, TEXT("natural_GameInstance_Deinitialize_once"));
        Check(Runtime.PersistentContextCount() == 0 && Runtime.ScriptTimerCount() == 0 &&
                  Runtime.SubscriptionCount() == 0,
              TEXT("native_shutdown_releases_owned_VM_and_timers"));
        Check(!EndErrors, TEXT("Deinitialize_without_script_errors"));
        UE_LOG(LogTemp, Display,
               TEXT("DAS_SUBSYSTEMS_PACKAGED_RESULT=%s checks=%d"), Good ? TEXT("PASS") : TEXT("FAIL"), Checks);
        // Release observers now, while the global delegates are still alive.
        FCoreUObjectDelegates::GetPreGarbageCollectDelegate().Remove(BeforeGC);
        BeforeGC.Reset();
        FCoreDelegates::OnEnginePreExit.Remove(EngineExit);
        EngineExit.Reset();
    }
    void Start()
    {
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        Check(!GIsEditor && FPlatformProperties::RequiresCookedData(), TEXT("cooked_process_without_editor"));
        TArray<FString> Config;
        GConfig->GetArray(TEXT("/Script/DasRuntime.DasSubsystems"), TEXT("Classes"), Config, GGameIni);
        Check(Config.Num() == 2 && Config.Contains(WorldPath) && Config.Contains(SessionPath),
              TEXT("cooked_GameIni_contains_both_soft_classes"));
        // Deliberately never LoadObject/TryLoadClass here: only the native bootstrap may load them.
        auto* WorldClass = FindObject<UClass>(nullptr, WorldPath);
        auto* SessionClass = FindObject<UClass>(nullptr, SessionPath);
        Check(WorldClass && SessionClass && WorldClass->IsChildOf(UDasWorldSubsystem::StaticClass()) &&
                  SessionClass->IsChildOf(UDasGameInstanceSubsystem::StaticClass()),
              TEXT("config_bootstrap_loaded_cooked_generated_classes"));
        UWorld* World = nullptr;
        for (const auto& Context : GEngine->GetWorldContexts())
            if (Context.World() && Context.World()->IsGameWorld())
                World = Context.World();
        auto* Instance = World ? World->GetGameInstance() : nullptr;
        WorldService = WorldClass && World ? Cast<UDasWorldSubsystem>(World->GetSubsystemBase(WorldClass)) : nullptr;
        SessionService = SessionClass && Instance
                             ? Cast<UDasGameInstanceSubsystem>(Instance->GetSubsystemBase(SessionClass))
                             : nullptr;
        Check(WorldService && WorldService->GetOuter() == World && WorldService->IsDasActive() &&
              Read(WorldService, TEXT("Starts")) == 1 && Read(WorldService, TEXT("Stops")) == 0,
                                                              TEXT("world_native_collection_created_and_initialized"));
        Check(SessionService && SessionService->GetOuter() == Instance && SessionService->IsDasActive() &&
              Read(SessionService, TEXT("Starts")) == 1 &&
                   Read(SessionService, TEXT("Stops")) == 0,
                        TEXT("GameInstance_native_collection_created_and_initialized"));
        Check(Advance(WorldService) == 1 && Advance(WorldService) == 2 &&
              FPaths::FileExists(Runtime.ScriptPath(TEXT("dsl_world_subsystem.das"))) && WorldService &&
                                 WorldService->LastDasError.IsEmpty(),
                                 TEXT("world_cooked_script_and_reflected_method"));
        Check(Advance(SessionService) == 1 && Advance(SessionService) == 2 &&
              FPaths::FileExists(Runtime.ScriptPath(TEXT("dsl_gameinstance_subsystem.das"))) && SessionService &&
                                 SessionService->LastDasError.IsEmpty(),
                                 TEXT("session_cooked_script_and_reflected_method"));
        for (UObject* Service : {static_cast<UObject*>(WorldService), static_cast<UObject*>(SessionService)})
            if (auto* Function = Service ? Service->FindFunction(TEXT("ArmTimer")) : nullptr)
                Service->ProcessEvent(Function, nullptr);
        Check(Runtime.ScriptTimerCount() == 2 && Runtime.PersistentContextCount() == 2,
              TEXT("two_services_own_live_VM_and_timer"));
        // Only request normal process exit; the test never manually deinitializes collections.
        FPlatformMisc::RequestExitWithStatus(false, Good ? 0 : 1);
    }
};
}
void StartDasSubsystemsValidation()
{
    auto State = MakeShared<FSubsystemPackageProbe>();
    State->BeforeGC = FCoreUObjectDelegates::GetPreGarbageCollectDelegate().AddLambda(
        [Weak = TWeakPtr<FSubsystemPackageProbe>(State)]
        {
            if (auto Keep = Weak.Pin())
                Keep->ObserveEnds();
        });
    State->EngineExit = FCoreDelegates::OnEnginePreExit.AddLambda(
        [State]
        {
            auto Keep = State;
            Keep->Finish();
        });
    FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(
        [State](float)
        {
            State->Start();
            return false;
        }));
}
