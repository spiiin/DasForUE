#pragma once
#include "CoreMinimal.h"
#include "Perception/AIPerceptionTypes.h"
#include "DasPerceptionTypes.generated.h"
class FProperty;
struct FDasScalarValue;
UENUM(BlueprintType)
enum class EDasAISense : uint8
{
    Invalid,
    Sight,
    Hearing,
    Other
};
// Owned, read-only projection of a native stimulus at callback delivery time.
// FAISenseID stays native; its process-local number is never exported to DAS.
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasAIStimulus
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly)
    EDasAISense Sense = EDasAISense::Invalid;
    UPROPERTY(BlueprintReadOnly)
    bool SuccessfullySensed = false;
    UPROPERTY(BlueprintReadOnly)
    bool Expired = false;
    UPROPERTY(BlueprintReadOnly)
    bool Valid = false;
    UPROPERTY(BlueprintReadOnly)
    float Age = 0;
    UPROPERTY(BlueprintReadOnly)
    float ExpirationAge = 0;
    UPROPERTY(BlueprintReadOnly)
    float Strength = 0;
    UPROPERTY(BlueprintReadOnly)
    FVector StimulusLocation = FVector::ZeroVector;
    UPROPERTY(BlueprintReadOnly)
    FVector ReceiverLocation = FVector::ZeroVector;
    UPROPERTY(BlueprintReadOnly)
    FName Tag = NAME_None;
};
DASRUNTIME_API bool CanProjectDasAIStimulus(const FProperty* Source, const UScriptStruct* Expected);
DASRUNTIME_API bool ProjectDasAIStimulus(const FAIStimulus& Source, FDasAIStimulus& Out, FString& Error);
DASRUNTIME_API bool ReadDasAIStimulusCallback(const FProperty* Source, const void* Memory,
                                              const UScriptStruct* Expected, FDasScalarValue& Out, FString& Error);
