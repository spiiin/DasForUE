#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

struct FDasTypeDeclaration
{
    FString Name;
    FString Parent;
    FString Fields;
};

struct FDasExecutionResult
{
    bool bSucceeded = false;
    bool bUsedAot = false;
    int32 Value = 0;
    FString Error;
    TArray<FDasTypeDeclaration> Types;
};

class DASRUNTIME_API FDasRuntimeModule final : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    virtual bool SupportsDynamicReloading() override { return false; }
    FDasExecutionResult RunFile(const FString& Path, int32 Input, bool bUseAot = false);
    FString ScriptPath(const FString& Name) const;
private:
    FString Root;
    bool bInitialized = false;
};

// Shared by the editor commandlet and the packaged smoke game.
DASRUNTIME_API bool RunDasFoundationTests(FString& Report);
