#pragma once
#include "CoreMinimal.h"
#include "DasCallbackValueTypes.generated.h"

// Native reflected fixtures exercise the same generic adapters as authored types.
UENUM()
enum class EDasCallbackMode : uint8 { Idle=0, Active=7, Paused=42 };
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasCallbackEnumRefSignal,const EDasCallbackMode&,Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasCallbackRecordRefSignal,const FVector4&,Value);
