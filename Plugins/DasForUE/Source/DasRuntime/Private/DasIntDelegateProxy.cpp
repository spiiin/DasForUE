#include "DasIntDelegateProxy.h"
#include "DasRuntime.h"
#include "Modules/ModuleManager.h"

void UDasIntDelegateProxy::Receive(int32 Value)
{
    if (!IsInGameThread()) return; // This bridge intentionally supports game-thread events only.
    if (auto* Runtime = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
        Runtime->DispatchIntEvent(SubscriptionId, Value);
}
