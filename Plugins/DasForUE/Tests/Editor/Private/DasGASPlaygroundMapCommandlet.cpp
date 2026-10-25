#include "DasGASAuthoredCommandlet.h"
#include "DasGASPlaygroundMap.h"
UDasGASPlaygroundMapCommandlet::UDasGASPlaygroundMapCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasGASPlaygroundMapCommandlet::Main(const FString&)
{
    DasGASPlaygroundExample::FPlaygroundMap Map;
    FString Error;
    if (!Map.Prepare(Error, false))
    {
        UE_LOG(LogTemp, Error, TEXT("DAS_GAS_MAP_EXPORT=FAIL %s"), *Error);
        return 1;
    }
    UE_LOG(LogTemp, Display, TEXT("DAS_GAS_MAP_EXPORT=PASS map=/Game/DasExamples/GASCues/GASAbilityMap"));
    return 0;
}
