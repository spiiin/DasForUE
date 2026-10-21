#include "DasUEAotCommandlet.h"
#include "DasUEAotProbe.h"
#include "Misc/Parse.h"
UDasUEAotCommandlet::UDasUEAotCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasUEAotCommandlet::Main(const FString& Params)
{
    FString Output, Report;
    if (FParse::Value(*Params, TEXT("Output="), Output))
    {
        if (!GenerateDasUEAotProbe(Output, Report))
        {
            UE_LOG(LogTemp, Error, TEXT("%s"), *Report);
            return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("DAS_UE_AOT_GENERATED=PASS"));
        return 0;
    }
    const bool Good = RunDasUEAotProbe(Report);
    UE_LOG(LogTemp, Display, TEXT("%s"), *Report);
    return Good ? 0 : 1;
}
