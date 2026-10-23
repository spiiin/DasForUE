#pragma once
#include "CoreMinimal.h"

// Script values with a C++ layout that generated AOT code and game modules share with the runtime: FDateTime and
// FTimespan as ticks (the ue_foundation types UEDateTime and UETimespan). Plain data, no daScript headers.
struct FDasDateTimeValue
{
    int64 Ticks = 0;
};
struct FDasTimespanValue
{
    int64 Ticks = 0;
};
