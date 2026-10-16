#pragma once
#include "CoreMinimal.h"
#include "DasSchema.h"
#include "UObject/StrongObjectPtr.h"
class UBlueprint;
class FDasBlueprintRollback {
    struct FImpl;TUniquePtr<FImpl> Impl;
public:
    FDasBlueprintRollback();~FDasBlueprintRollback();
    static bool GatherScope(UBlueprint* Root,TArray<UBlueprint*>& Blueprints,TArray<TStrongObjectPtr<UObject>>& Assets,FString& Error,bool DefaultsOnly=false);
    bool Prepare(UBlueprint* Root,const FDasClassSchema& Fields,const TArray<UBlueprint*>& Blueprints,const TArray<TStrongObjectPtr<UObject>>& Assets,FString& Error);
    bool Finish(FString& Error);
    bool Restore(FString& Error);
};
