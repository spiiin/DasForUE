#pragma once
#include "CoreMinimal.h"
#include "DasPropertyAccess.h"
#if DAS_PROJECT_NATIVE_GAMEPLAY
#include "DasProjectNativeGameplay.generated.h"
#else
#include "DasNativeGameplay.generated.h"
#endif
inline FDasPropertyAccess DasNativeAccessContract(const FDasNativeGameplayField& Row)
{
    FDasPropertyAccess A;
    A.NativeGetter = Row.NativeGetter;
    A.NativeSetter = Row.NativeSetter;
    A.GetterUnsafe = Row.GetterUnsafe;
    A.SetterUnsafe = Row.SetterUnsafe;
    A.Getter = FName(Row.Getter);
    A.Setter = FName(Row.Setter);
    return A;
}
inline const FDasNativeGameplayField* DasNativeGameplayField(const FString& Class, const FString& Name)
{
    for (const auto& F : DasNativeGameplayFields)
        if (Class.Equals(F.Class, ESearchCase::CaseSensitive) && Name.Equals(F.Name, ESearchCase::CaseSensitive))
            return &F;
    return nullptr;
}
inline const FDasNativeGameplayMethod* DasNativeGameplayMethod(const FString& Class, const FString& Name)
{
    for (const auto& F : DasNativeGameplayMethods)
        if (Class.Equals(F.Class, ESearchCase::CaseSensitive) && Name.Equals(F.Name, ESearchCase::CaseSensitive))
            return &F;
    return nullptr;
}
