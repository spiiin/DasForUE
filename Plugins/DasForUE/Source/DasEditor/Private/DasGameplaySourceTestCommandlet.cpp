#include "DasGameplaySourceTestCommandlet.h"
#include "DasTypeService.h"
#include "Misc/Parse.h"

int32 UDasGameplaySourceTestCommandlet::Main(const FString& Params)
{
    if (FParse::Param(*Params, TEXT("StructMapFixtures")))
    {
        TArray<UObject*> Types;
        FString Error;
        if (!FDasTypeService::Apply(TEXT("ue_struct_map_types.das"), TEXT("/Game/DasStructMaps"), Types, Error))
        {
            UE_LOG(LogTemp, Error, TEXT("DAS_GAMEPLAY_FIXTURES FAIL %s"), *Error);
            return 1;
        }
    }
    return Super::Main(Params);
}
