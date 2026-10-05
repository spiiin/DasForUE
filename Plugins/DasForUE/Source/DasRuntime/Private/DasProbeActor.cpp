#include "DasProbeActor.h"
#include "DasRuntime.h"
#include "Components/SceneComponent.h"
#include "Modules/ModuleManager.h"

ADasProbeActor::ADasProbeActor()
{
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProbeRoot"));
}

int32 ADasProbeActor::ExecuteDas(const FString& ScriptFile, int32 Input)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const auto Result = Runtime.RunFile(Runtime.ScriptPath(ScriptFile), Input);
    if (!Result.bSucceeded)
    {
        UE_LOG(LogTemp, Error, TEXT("daScript probe: %s"), *Result.Error);
        return MIN_int32;
    }
    return Result.Value;
}
