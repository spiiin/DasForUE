#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DasObject.generated.h"

class UDasScriptClassData;

// Plain owned script objects use the common Blueprint frame bridge, without Actor lifecycle.
UCLASS(Blueprintable, BlueprintType, EditInlineNew)
class DASRUNTIME_API UDasObject : public UObject
{
    GENERATED_BODY()
public:
    // A class without own method graphs still needs a cooked, GC-visible contract.
    UPROPERTY()
    TObjectPtr<UDasScriptClassData> DasClassData;

    virtual void BeginDestroy() override;
};
