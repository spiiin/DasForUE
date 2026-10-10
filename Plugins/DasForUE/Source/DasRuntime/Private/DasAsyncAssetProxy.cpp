#include "DasAsyncAssetProxy.h"
#include "DasRuntime.h"
#include "DasSchema.h"
#include "UObject/Class.h"
#include "UObject/UObjectGlobals.h"
#include "Modules/ModuleManager.h"
#include "UObject/StrongObjectPtr.h"
void UDasAsyncAssetProxy::Complete(){if(!IsInGameThread())return;TStrongObjectPtr<UDasAsyncAssetProxy> Keep(this);auto* Loaded=Path.ResolveObject();auto* Expected=FindObject<UClass>(nullptr,*ExpectedClass);if(Loaded && (!Expected || !DasReferenceMatches(Loaded,Expected))){UE_LOG(LogTemp,Warning,TEXT("DAS_ASSET_LOAD wrong class for %s; expected %s"),*Path.ToString(),*ExpectedClass);Loaded=nullptr;}if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=8;V.ObjectValue=Loaded;R->DispatchScalarEvent(SubscriptionId,V);}}
