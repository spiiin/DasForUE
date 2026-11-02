#include "DasPlayerController.h"
#include "DasTypeRegistry.h"
#include "DasActorHostRuntime.h"
ADasPlayerController::ADasPlayerController()
{
    PrimaryActorTick.bCanEverTick = true;
}
void ADasPlayerController::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    FDasActorHostRuntime::OnConstruction(*this, Transform);
}
void ADasPlayerController::ApplyDasSceneRoot()
{
    FDasActorHostRuntime::ApplyDasSceneRoot(*this);
}
void ADasPlayerController::BeginPlay()
{
    Super::BeginPlay();
    FDasActorHostRuntime::BeginPlay(*this);
}
void ADasPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    FDasActorHostRuntime::Tick(*this, DeltaSeconds);
}
void ADasPlayerController::EndPlay(const EEndPlayReason::Type Reason)
{
    FDasActorHostRuntime::EndPlay(*this, Reason);
    Super::EndPlay(Reason);
}
void ADasPlayerController::RecordLifecycleResult(const FDasExecutionResult& Result)
{
    FDasActorHostRuntime::RecordLifecycleResult(*this, Result);
}
FDasScalarValue ADasPlayerController::ExecuteDasFrame(const FString& File, const FString& Entry,
                                                      const TArray<FDasScalarValue>& Args, int32 Kind,
                                                      const FString& ResultClass)
{
    return FDasActorHostRuntime::ExecuteDasFrame(*this, File, Entry, Args, Kind, ResultClass);
}
FDasScalarValue ADasPlayerController::ExecuteDasFrameOutputs(const FString& File, const FString& Entry,
                                                             const TArray<FDasScalarValue>& Args, int32 Kind,
                                                             const FString& ResultClass,
                                                             TArray<FDasScalarValue>& Outputs)
{
    return FDasActorHostRuntime::ExecuteDasFrameOutputs(*this, File, Entry, Args, Kind, ResultClass, Outputs);
}
void ADasPlayerController::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    ApplyDasSceneRoot();
}
void ADasPlayerController::Destroyed()
{
    Super::Destroyed();
    if (auto* R = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
        R->ReleaseObject(this);
}
FDasScalarValue ADasPlayerController::ExecuteDasConstFrame(const FString& File, const FString& Entry,
                                                           const TArray<FDasScalarValue>& Args, int32 Kind,
                                                           const FString& ResultClass) const
{
    return const_cast<ADasPlayerController*>(this)->ExecuteDasFrame(File, Entry, Args, Kind, ResultClass);
}
FDasScalarValue ADasPlayerController::ExecuteDasConstFrameOutputs(const FString& File, const FString& Entry,
                                                                  const TArray<FDasScalarValue>& Args, int32 Kind,
                                                                  const FString& ResultClass,
                                                                  TArray<FDasScalarValue>& Outputs) const
{
    return const_cast<ADasPlayerController*>(this)->ExecuteDasFrameOutputs(File, Entry, Args, Kind, ResultClass,
                                                                           Outputs);
}
