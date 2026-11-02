#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DasObject.generated.h"

// Plain owned script objects use the common Blueprint frame bridge, without Actor lifecycle.
UCLASS(Blueprintable, BlueprintType, EditInlineNew)
class DASRUNTIME_API UDasObject : public UObject
{
    GENERATED_BODY()
public:
    virtual void BeginDestroy() override;
};
