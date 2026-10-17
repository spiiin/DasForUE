#pragma once
#include "CoreMinimal.h"
#include "DasNativeGameplay.generated.h"
inline const FDasNativeGameplayField* DasNativeGameplayField(const FString& Class,const FString& Name){for(const auto& F:DasNativeGameplayFields)if(Class.Equals(F.Class,ESearchCase::CaseSensitive) && Name.Equals(F.Name,ESearchCase::CaseSensitive))return &F;return nullptr;}
inline const FDasNativeGameplayMethod* DasNativeGameplayMethod(const FString& Class,const FString& Name){for(const auto& F:DasNativeGameplayMethods)if(Class.Equals(F.Class,ESearchCase::CaseSensitive) && Name.Equals(F.Name,ESearchCase::CaseSensitive))return &F;return nullptr;}
