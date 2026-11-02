#include "DasObject.h"
#include "DasRuntime.h"
#include "Modules/ModuleManager.h"
void UDasObject::BeginDestroy()
{
    if (IsInGameThread())
        if (auto* Runtime = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
            Runtime->ReleaseObject(this);
    Super::BeginDestroy();
}
