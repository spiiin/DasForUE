#include "DasGameplayTags.h"
#include "UObject/Class.h"
bool DasIsGameplayTagType(const UScriptStruct* Type)
{
    return Type == FGameplayTag::StaticStruct() || Type == FGameplayTagContainer::StaticStruct();
}
bool ValidateDasGameplayTagValue(const UScriptStruct* Type, const void* Data, FString& Error)
{
    auto Registered = [](const FGameplayTag& Tag)
    {
        return Tag.IsValid() && FGameplayTag::RequestGameplayTag(Tag.GetTagName(), false) == Tag;
    };
    if (Type == FGameplayTag::StaticStruct())
    {
        const auto& Tag = *static_cast<const FGameplayTag*>(Data);
        if (Tag.IsValid() && !Registered(Tag))
        {
            Error = TEXT("GameplayTag is not in the UE dictionary");
            return false;
        }
        return true;
    }
    if (Type == FGameplayTagContainer::StaticStruct())
    {
        const auto& Tags = *static_cast<const FGameplayTagContainer*>(Data);
        if (Tags.Num() > 256)
        {
            Error = TEXT("GameplayTagContainer exceeds 256 explicit tags");
            return false;
        }
        FGameplayTagContainer Canonical;
        for (const auto& Tag : Tags.GetGameplayTagArray())
        {
            if (!Registered(Tag))
            {
                Error = TEXT("GameplayTagContainer contains an empty/unregistered tag");
                return false;
            }
            Canonical.AddTag(Tag);
        }
        if (Canonical.Num() != Tags.Num() || Canonical.GetGameplayTagParents() != Tags.GetGameplayTagParents())
        {
            Error = TEXT("GameplayTagContainer has duplicate tags or stale parent cache");
            return false;
        }
        return true;
    }
    Error = TEXT("Expected GameplayTag or GameplayTagContainer");
    return false;
}
