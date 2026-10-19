#pragma once
#include "CoreMinimal.h"
class UBlueprint;
class FDasInterfaceService
{
public:
    static void RegisterCompiler();
    static bool Apply(const FString& Script, const FString& Folder, TArray<UBlueprint*>& Out, FString& Error);
};
