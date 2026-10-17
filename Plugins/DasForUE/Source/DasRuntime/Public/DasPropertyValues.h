#pragma once
#include "DasCallFrame.h"
#include "UObject/GCObject.h"
#include "UObject/UObjectGlobals.h"

// Memory points to the property value, not its containing object. Output/native
// memory is unchanged on failure. Only value types accepted by the shared
// descriptor are supported; delegates require their signature/lifetime service.
DASRUNTIME_API bool ReadDasPropertyValue(FProperty* Property,const void* Memory,FDasScalarValue& Out,FString& Error);
DASRUNTIME_API bool WriteDasPropertyValue(FProperty* Property,void* Memory,const FDasScalarValue& Value,FString& Error);

// Owned native snapshots need an explicit GC scope outside UPROPERTY storage.
// Includes references nested in structs, arrays, sets and map keys/values.
struct DASRUNTIME_API FDasPropertyValueRoots final : FGCObject {
    TArray<FDasScalarValue> Values;
    void Keep(const FDasScalarValue& Value){Values.Add(Value);}
    void Keep(const TMap<FString,FDasScalarValue>& Items){for(const auto& Item:Items)Keep(Item.Value);}
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override {return TEXT("DasPropertyValueRoots");}
    static void Remap(FDasScalarValue& Value,const TMap<UObject*,UObject*>& Map);
    static void Remap(TMap<FString,FDasScalarValue>& Values,const TMap<UObject*,UObject*>& Map){for(auto& Item:Values)Remap(Item.Value,Map);}
    void RemapPins(const TMap<UObject*,UObject*>& Map){for(auto& Value:Values)Remap(Value,Map);}
};

// Strict immutable-cache identity, including FText localization history.
DASRUNTIME_API bool IdenticalDasStructSnapshot(const FInstancedStruct& A,const FInstancedStruct& B);
