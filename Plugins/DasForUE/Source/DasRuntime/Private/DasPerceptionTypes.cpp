#include "DasPerceptionTypes.h"
#include "DasRuntime.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "UObject/UnrealType.h"
bool CanProjectDasAIStimulus(const FProperty* Source, const UScriptStruct* Expected)
{
    const auto* S = CastField<FStructProperty>(Source);
    return S && Source->ArrayDim == 1 && S->Struct == FAIStimulus::StaticStruct() &&
           Expected == FDasAIStimulus::StaticStruct();
}
bool ProjectDasAIStimulus(const FAIStimulus& S, FDasAIStimulus& Out, FString& Error)
{
    Error.Reset();
    // ExpirationAge is BlueprintReadWrite but has no public native getter.
    const auto* Expiration = FindFProperty<FFloatProperty>(FAIStimulus::StaticStruct(), TEXT("ExpirationAge"));
    if (!Expiration)
    {
        Error = TEXT("AIStimulus ExpirationAge schema changed");
        return false;
    }
    FDasAIStimulus V;
    if (S.Type != FAISenseID::InvalidID())
    {
        V.Sense = S.Type == UAISense::GetSenseID<UAISense_Sight>()     ? EDasAISense::Sight
                  : S.Type == UAISense::GetSenseID<UAISense_Hearing>() ? EDasAISense::Hearing
                                                                       : EDasAISense::Other;
    }
    V.SuccessfullySensed = S.WasSuccessfullySensed();
    V.Expired = S.IsExpired();
    V.Valid = S.IsValid();
    V.Age = S.GetAge();
    V.ExpirationAge = Expiration->GetPropertyValue_InContainer(&S);
    V.Strength = S.Strength;
    V.StimulusLocation = S.StimulusLocation;
    V.ReceiverLocation = S.ReceiverLocation;
    V.Tag = S.Tag;
    Out = MoveTemp(V);
    return true;
}
bool ReadDasAIStimulusCallback(const FProperty* Source, const void* Memory, const UScriptStruct* Expected,
                               FDasScalarValue& Out, FString& Error)
{
    if (!IsInGameThread() || !Memory || !CanProjectDasAIStimulus(Source, Expected))
    {
        Error = TEXT("Expected native AIStimulus to owned perception callback view");
        return false;
    }
    FDasAIStimulus View;
    if (!ProjectDasAIStimulus(*static_cast<const FAIStimulus*>(Memory), View, Error))
        return false;
    FDasScalarValue Value;
    Value.Kind = int32(EDasMethodSignature::Struct);
    Value.ObjectClass = FDasAIStimulus::StaticStruct()->GetPathName();
    Value.StructValue.InitializeAs(FDasAIStimulus::StaticStruct(), reinterpret_cast<const uint8*>(&View));
    Out = MoveTemp(Value);
    return true;
}
