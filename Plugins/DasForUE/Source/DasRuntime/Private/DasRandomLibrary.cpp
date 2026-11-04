#include "DasRandomLibrary.h"

int32 UDasRandomLibrary::RandomIntegerInRange(int32& Seed, int32 Minimum, int32 Maximum)
{
    if (Maximum < Minimum || int64(Maximum) - Minimum + 1 > MAX_int32)
        return Minimum;
    FRandomStream Stream(Seed);
    const int32 Value = Stream.RandRange(Minimum, Maximum);
    Seed = Stream.GetCurrentSeed();
    return Value;
}

float UDasRandomLibrary::RandomFloatInRange(int32& Seed, float Minimum, float Maximum)
{
    if (!FMath::IsFinite(Minimum) || !FMath::IsFinite(Maximum) || Maximum < Minimum)
        return Minimum;
    FRandomStream Stream(Seed);
    const float Value = Stream.FRandRange(Minimum, Maximum);
    Seed = Stream.GetCurrentSeed();
    return Value;
}
