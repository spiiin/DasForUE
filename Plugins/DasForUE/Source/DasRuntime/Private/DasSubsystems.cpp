#include "DasSubsystems.h"
#include "DasActorHostRuntime.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Misc/ConfigCacheIni.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/SoftObjectPath.h"
namespace
{
void InitializeConfiguredSubsystems(FSubsystemCollectionBase& Collection, UClass* Base);
}
void UDasWorldSubsystem::RecordLifecycleResult(const FDasExecutionResult& R)
{
    FDasActorHostRuntime::RecordLifecycleResult(*this, R);
}
FDasScalarValue UDasWorldSubsystem::ExecuteDasFrame(const FString& F, const FString& E,
                                                    const TArray<FDasScalarValue>& A, int32 K, const FString& R)
{
    return FDasActorHostRuntime::ExecuteDasFrame(*this, F, E, A, K, R);
}
FDasScalarValue UDasWorldSubsystem::ExecuteDasFrameOutputs(const FString& F, const FString& E,
                                                           const TArray<FDasScalarValue>& A, int32 K, const FString& R,
                                                           TArray<FDasScalarValue>& O)
{
    return FDasActorHostRuntime::ExecuteDasFrameOutputs(*this, F, E, A, K, R, O);
}
FDasScalarValue UDasWorldSubsystem::ExecuteDasConstFrame(const FString& F, const FString& E,
                                                         const TArray<FDasScalarValue>& A, int32 K,
                                                         const FString& R) const
{
    return const_cast<UDasWorldSubsystem*>(this)->ExecuteDasFrame(F, E, A, K, R);
}
FDasScalarValue UDasWorldSubsystem::ExecuteDasConstFrameOutputs(const FString& F, const FString& E,
                                                                const TArray<FDasScalarValue>& A, int32 K,
                                                                const FString& R, TArray<FDasScalarValue>& O) const
{
    return const_cast<UDasWorldSubsystem*>(this)->ExecuteDasFrameOutputs(F, E, A, K, R, O);
}

bool UDasWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if ((GetClass() != StaticClass() && (LifecycleScript.IsEmpty() || AuthoredTypeId.IsEmpty())) ||
        GetClass()->HasAnyClassFlags(CLASS_Abstract | CLASS_NewerVersionExists | CLASS_Deprecated))
        return false;
    auto* World = Cast<UWorld>(Outer);
    return World && World->IsGameWorld();
}
void UDasWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    if (GetClass() == StaticClass())
        InitializeConfiguredSubsystems(Collection, StaticClass());
    if (bDasActive || IsTemplate() || LifecycleScript.IsEmpty())
        return;
    bDasActive = true;
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    // Reuse the owned-session ABI: Initialize is once per object, never once per reload.
    RecordLifecycleResult(Runtime.RunLifecycle(Runtime.ScriptPath(LifecycleScript), EDasCallback::BeginPlay, this));
}
void UDasWorldSubsystem::Deinitialize()
{
    if (auto* Runtime = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
    {
        if (bDasActive)
        {
            bDasActive = false;
            RecordLifecycleResult(
                Runtime->RunLifecycle(Runtime->ScriptPath(LifecycleScript), EDasCallback::EndPlay, this));
        }
        Runtime->ReleaseObject(this);
    }
    bDasActive = false;
    Super::Deinitialize();
}
void UDasWorldSubsystem::BeginDestroy()
{
    // GC fallback releases resources only; user code runs at collection Deinitialize, not during GC.
    if (IsInGameThread())
        if (auto* Runtime = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
            Runtime->ReleaseObject(this);
    Super::BeginDestroy();
}

void UDasGameInstanceSubsystem::RecordLifecycleResult(const FDasExecutionResult& R)
{
    FDasActorHostRuntime::RecordLifecycleResult(*this, R);
}
FDasScalarValue UDasGameInstanceSubsystem::ExecuteDasFrame(const FString& F, const FString& E,
                                                           const TArray<FDasScalarValue>& A, int32 K, const FString& R)
{
    return FDasActorHostRuntime::ExecuteDasFrame(*this, F, E, A, K, R);
}
FDasScalarValue UDasGameInstanceSubsystem::ExecuteDasFrameOutputs(const FString& F, const FString& E,
                                                                  const TArray<FDasScalarValue>& A, int32 K,
                                                                  const FString& R, TArray<FDasScalarValue>& O)
{
    return FDasActorHostRuntime::ExecuteDasFrameOutputs(*this, F, E, A, K, R, O);
}
FDasScalarValue UDasGameInstanceSubsystem::ExecuteDasConstFrame(const FString& F, const FString& E,
                                                                const TArray<FDasScalarValue>& A, int32 K,
                                                                const FString& R) const
{
    return const_cast<UDasGameInstanceSubsystem*>(this)->ExecuteDasFrame(F, E, A, K, R);
}
FDasScalarValue UDasGameInstanceSubsystem::ExecuteDasConstFrameOutputs(const FString& F, const FString& E,
                                                                       const TArray<FDasScalarValue>& A, int32 K,
                                                                       const FString& R,
                                                                       TArray<FDasScalarValue>& O) const
{
    return const_cast<UDasGameInstanceSubsystem*>(this)->ExecuteDasFrameOutputs(F, E, A, K, R, O);
}

bool UDasGameInstanceSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if ((GetClass() != StaticClass() && (LifecycleScript.IsEmpty() || AuthoredTypeId.IsEmpty())) ||
        GetClass()->HasAnyClassFlags(CLASS_Abstract | CLASS_NewerVersionExists | CLASS_Deprecated))
        return false;
    return IsValid(Cast<UGameInstance>(Outer)) && !Outer->IsTemplate();
}
void UDasGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    if (GetClass() == StaticClass())
        InitializeConfiguredSubsystems(Collection, StaticClass());
    if (bDasActive || IsTemplate() || LifecycleScript.IsEmpty())
        return;
    bDasActive = true;
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    // Reuse the owned-session ABI: Initialize is once per object, never once per reload.
    RecordLifecycleResult(Runtime.RunLifecycle(Runtime.ScriptPath(LifecycleScript), EDasCallback::BeginPlay, this));
}
void UDasGameInstanceSubsystem::Deinitialize()
{
    if (auto* Runtime = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
    {
        if (bDasActive)
        {
            bDasActive = false;
            RecordLifecycleResult(
                Runtime->RunLifecycle(Runtime->ScriptPath(LifecycleScript), EDasCallback::EndPlay, this));
        }
        Runtime->ReleaseObject(this);
    }
    bDasActive = false;
    Super::Deinitialize();
}
void UDasGameInstanceSubsystem::BeginDestroy()
{
    // GC fallback releases resources only; user code runs at collection Deinitialize, not during GC.
    if (IsInGameThread())
        if (auto* Runtime = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
            Runtime->ReleaseObject(this);
    Super::BeginDestroy();
}

namespace
{
TArray<TStrongObjectPtr<UClass>> LoadedSubsystemClasses;
}
void LoadDasSubsystemClasses()
{
    TArray<FString> Paths;
    GConfig->GetArray(TEXT("/Script/DasRuntime.DasSubsystems"), TEXT("Classes"), Paths, GGameIni);
    for (const auto& Path : Paths)
    {
        UClass* Class = FSoftClassPath(Path).TryLoadClass<USubsystem>();
        if (!Class || (!Class->IsChildOf(UDasWorldSubsystem::StaticClass()) &&
                       !Class->IsChildOf(UDasGameInstanceSubsystem::StaticClass())))
        {
            UE_LOG(LogTemp, Error, TEXT("Invalid authored subsystem class in DasSubsystems.Classes: %s"), *Path);
            continue;
        }
        if (!LoadedSubsystemClasses.ContainsByPredicate([&](const auto& Entry) { return Entry.Get() == Class; }))
            LoadedSubsystemClasses.Emplace(Class);
    }
}
void ResetDasSubsystemClasses()
{
    LoadedSubsystemClasses.Empty();
}

namespace
{
void InitializeConfiguredSubsystems(FSubsystemCollectionBase& Collection, UClass* Base)
{
    LoadDasSubsystemClasses();
    for (const auto& Class : LoadedSubsystemClasses)
        if (Class.Get() != Base && Class->IsChildOf(Base))
            Collection.InitializeDependency(Class.Get());
}
}
