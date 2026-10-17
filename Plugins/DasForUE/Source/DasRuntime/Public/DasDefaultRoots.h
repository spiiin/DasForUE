#pragma once
#include "StructUtils/InstancedStruct.h"
#include "UObject/GCObject.h"
// Import staging has no owning UObject yet. Keep every parsed default alive
// until the Blueprint/CDO takes ownership, including across synchronous loads.
struct FDasDefaultRoots final : FGCObject {
 TArray<FInstancedStruct> Values;
 TArray<TObjectPtr<UObject>> Objects;
 virtual void AddReferencedObjects(FReferenceCollector& Collector) override {
  Collector.AddReferencedObjects(Objects);
  for(auto& Value:Values)Value.AddStructReferencedObjects(Collector);
 }
 virtual FString GetReferencerName() const override { return TEXT("DasDefaultRoots"); }
};
