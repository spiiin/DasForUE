#include "DasActorHostRuntime.h"
#include "DasTypeRegistry.h"
#include "DasProbeActor.h"
#include "DasRuntime.h"
#include "DasScriptComponent.h"
#include "Components/SceneComponent.h"
#include "Modules/ModuleManager.h"
#include "Misc/Paths.h"
#include "Blueprint/BlueprintExceptionInfo.h"
#include "UObject/Stack.h"
#include "UObject/Script.h"
#include "UObject/UnrealType.h"

ADasProbeActor::ADasProbeActor()
{
    PrimaryActorTick.bCanEverTick = true;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProbeRoot"));
}

void ADasProbeActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    FDasActorHostRuntime::OnConstruction(*this, Transform);
}
void ADasProbeActor::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    // Loaded/cooked actors restore the SCS root without repeating construction.
    ApplyDasSceneRoot();
}
void ADasProbeActor::ApplyDasSceneRoot()
{
    FDasActorHostRuntime::ApplyDasSceneRoot(*this);
}

int32 ADasProbeActor::ExecuteDas(const FString& ScriptFile, int32 Input)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const auto Result = Runtime.RunPersistent(Runtime.ScriptPath(ScriptFile), Input, TWeakObjectPtr<UObject>(this));
    if (!Result.bSucceeded)
    {
        UE_LOG(LogTemp, Error, TEXT("daScript probe: %s"), *Result.Error);
        return MIN_int32;
    }
    return Result.Value;
}

int32 ADasProbeActor::ExecuteDasNamed(const FString& ScriptFile, const FString& EntryPoint, int32 Input)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const auto Result = Runtime.RunNamedPersistent(Runtime.ScriptPath(ScriptFile), EntryPoint, Input, this);
    RecordLifecycleResult(Result);
    if (!Result.bSucceeded)
    {
        if (!Runtime.IsAuthoredMethodDispatchActive())
            UE_LOG(LogTemp, Error, TEXT("daScript method: %s"), *Result.Error);
        return MIN_int32;
    }
    return Result.Value;
}

float ADasProbeActor::ExecuteDasFloat(const FString& File, const FString& Entry, float Input)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const auto V = R.RunNamedFloatPersistent(R.ScriptPath(File), Entry, Input, this);
    RecordLifecycleResult(V);
    return V.FloatValue;
}
bool ADasProbeActor::ExecuteDasBool(const FString& File, const FString& Entry, bool Input)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const auto V = R.RunNamedBoolPersistent(R.ScriptPath(File), Entry, Input, this);
    RecordLifecycleResult(V);
    return V.BoolValue;
}
void ADasProbeActor::ExecuteDasVoid(const FString& File, const FString& Entry)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    RecordLifecycleResult(R.RunNamedVoidPersistent(R.ScriptPath(File), Entry, this));
}

FDasScalarValue ADasProbeActor::ExecuteDasFrame(const FString& File, const FString& Entry,
                                                const TArray<FDasScalarValue>& Args, int32 Kind,
                                                const FString& ResultClass)
{
    return FDasActorHostRuntime::ExecuteDasFrame(*this, File, Entry, Args, Kind, ResultClass);
}
FDasScalarValue ADasProbeActor::ExecuteDasFrameOutputs(const FString& File, const FString& Entry,
                                                       const TArray<FDasScalarValue>& Args, int32 Kind,
                                                       const FString& ResultClass, TArray<FDasScalarValue>& Outputs)
{
    return FDasActorHostRuntime::ExecuteDasFrameOutputs(*this, File, Entry, Args, Kind, ResultClass, Outputs);
}
void ADasProbeActor::RecordLifecycleResult(const FDasExecutionResult& Result)
{
    FDasActorHostRuntime::RecordLifecycleResult(*this, Result);
}

void ADasProbeActor::BeginPlay()
{
    Super::BeginPlay();
    FDasActorHostRuntime::BeginPlay(*this);
}

void ADasProbeActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    FDasActorHostRuntime::Tick(*this, DeltaSeconds);
}

void ADasProbeActor::EndPlay(const EEndPlayReason::Type Reason)
{
    FDasActorHostRuntime::EndPlay(*this, Reason);
    Super::EndPlay(Reason);
}

void ADasProbeActor::Destroyed()
{
    // Super routes EndPlay; its callback must run before the context is released.
    Super::Destroyed();
    if (auto* Runtime = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
        Runtime->ReleaseObject(this);
}

// Const graph adapters share dispatch/session and diagnostic handling with ordinary
// calls. Source contracts are validated before these internal entry points are used.
FDasScalarValue ADasProbeActor::ExecuteDasConstFrame(const FString& File, const FString& Entry,
                                                     const TArray<FDasScalarValue>& Args, int32 Kind,
                                                     const FString& ResultClass) const
{
    return const_cast<ADasProbeActor*>(this)->ExecuteDasFrame(File, Entry, Args, Kind, ResultClass);
}
FDasScalarValue ADasProbeActor::ExecuteDasConstFrameOutputs(const FString& File, const FString& Entry,
                                                            const TArray<FDasScalarValue>& Args, int32 Kind,
                                                            const FString& ResultClass,
                                                            TArray<FDasScalarValue>& Outputs) const
{
    return const_cast<ADasProbeActor*>(this)->ExecuteDasFrameOutputs(File, Entry, Args, Kind, ResultClass, Outputs);
}
