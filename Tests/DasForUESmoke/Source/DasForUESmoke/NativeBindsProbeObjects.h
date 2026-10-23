#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NativeBindsProbeObjects.generated.h"

// Classes bound as script types by the native binds probe (DasNativeBindsProbe.cpp): Probe, its child and
// grandchild, with methods of several shapes.
UCLASS()
class UNativeBindsProbe : public UObject
{
    GENERATED_BODY()
public:
    int32 Value = 0;
    UPROPERTY()
    TObjectPtr<UNativeBindsProbe> Partner;

    int32 GetValue() const { return Value; }
    void SetValue(int32 NewValue) { Value = NewValue; }
    int32 Scaled(int32 Factor) const { return Value * Factor; }
    float Scaled(float Factor) const { return Value * Factor; }
    UNativeBindsProbe* GetPartner() const { return Partner; }
    void SetPartner(UNativeBindsProbe* NewPartner) { Partner = NewPartner; }
};

UCLASS()
class UNativeBindsProbeChild : public UNativeBindsProbe
{
    GENERATED_BODY()
public:
    int32 ChildOnly() const { return Value * 10; }
};

UCLASS()
class UNativeBindsProbeGrandchild : public UNativeBindsProbeChild
{
    GENERATED_BODY()
};
