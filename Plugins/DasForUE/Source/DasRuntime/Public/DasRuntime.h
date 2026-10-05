#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "UObject/WeakObjectPtr.h"
#include "UObject/WeakObjectPtrTemplates.h"

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
    FString ReloadError; // Candidate rejected; a successful result may still use the previous version.
    uint32 Revision = 0;
    TArray<FDasTypeDeclaration> Types;
};

enum class EDasCallback : uint8 { Main, BeginPlay, Tick, EndPlay };

class DASRUNTIME_API FDasRuntimeModule final : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    virtual bool SupportsDynamicReloading() override { return false; }
    FDasExecutionResult RunFile(const FString& Path, int32 Input, bool bUseAot = false, TWeakObjectPtr<UObject> Self = {});
    FDasExecutionResult RunPersistent(const FString& Path, int32 Input, TWeakObjectPtr<UObject> Self);
    FDasExecutionResult RunLifecycle(const FString& Path, EDasCallback Event, TWeakObjectPtr<UObject> Self, float DeltaSeconds = 0.f, int32 EndReason = 0);
    // Internal reflected delegate bridge: IDs never contain a VM/function pointer.
    void DispatchIntEvent(uint64 SubscriptionId, int32 Value);
    int32 SubscriptionCount(const UObject* Owner = nullptr) const;
    void ReleaseObject(UObject* Object);
    int32 PersistentContextCount(const UWorld* World = nullptr) const;
    bool ExportBindings(const FString& SelectionPath, const FString& OutputPath, FString& Error);
    FString ScriptPath(const FString& Name) const;
private:
    FDasExecutionResult RunCallback(const FString& Path, EDasCallback Event, int32 Input, float DeltaSeconds, TWeakObjectPtr<UObject> Self);
    FDelegateHandle WorldCleanupHandle;
    FDelegateHandle PostGCHandle;
    FString Root;
    bool bInitialized = false;
};

// Shared by the editor commandlet and the packaged smoke game.
DASRUNTIME_API bool RunDasFoundationTests(FString& Report);
