#include "DasNativeHashProbeComponent.h"
#include "UObject/GarbageCollection.h"
void UDasNativeHashProbeComponent::SetSemanticObject(UObject* Value){if(CollectOnSet)CollectGarbage(RF_NoFlags);++SetterCalls;SemanticObject=Value;}
