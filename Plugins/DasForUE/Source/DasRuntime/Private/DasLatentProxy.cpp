#include "DasLatentProxy.h"
#include "DasRuntime.h"
#include "Modules/ModuleManager.h"
#include "UObject/StrongObjectPtr.h"
void UDasLatentProxy::Complete(int32 Linkage){if(!IsInGameThread())return;TStrongObjectPtr<UDasLatentProxy> Keep(this);if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=3;R->DispatchScalarEvent(SubscriptionId,V);}}
