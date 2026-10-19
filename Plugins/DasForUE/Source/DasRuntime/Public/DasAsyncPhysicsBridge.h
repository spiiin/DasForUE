#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DasAsyncPhysicsBridge.generated.h"
class UDasAsyncPhysicsBridge;
// Native-only borrowed scope. It carries no body/proxy pointer and grants no
// UObject operations. Copies expire when the engine callback returns.
struct DASRUNTIME_API FDasAsyncPhysicsStep
{
    uint64 Epoch = 0, Revision = 0;
    float DeltaTime = 0, SimTime = 0;
    bool IsCurrent() const;

private:
    TWeakObjectPtr<UDasAsyncPhysicsBridge> Bridge;
    friend class UDasAsyncPhysicsBridge;
};
UCLASS(Transient, NotBlueprintable)
class DASRUNTIME_API UDasAsyncPhysicsBridge final : public UActorComponent
{
    GENERATED_BODY()
public:
    using FCallback = TFunction<bool(const FDasAsyncPhysicsStep&, FString&)>;
    // Trusted native lifecycle API; intentionally no UFUNCTION or DAS extern.
    static UDasAsyncPhysicsBridge* Create(UObject* ScriptOwner, FCallback Callback, FString& Error);
    void Revoke();
    bool IsLiveBinding() const;
    const FString& GetLastError() const { return LastError; }
    uint64 GetBindingRevision() const { return Revision; }
    static void RevokeOwner(const UObject* Owner);
    static void RevokeOwnerExcept(const UObject* Owner, uint64 Revision);
    static void RevokeWorld(const UWorld* World);
    static void RevokeAll();
    static void FlushDeferred();
    virtual void AsyncPhysicsTickComponent(float Dt, float SimTime) override;
    virtual void Deactivate() override;
    virtual void OnUnregister() override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    virtual void BeginDestroy() override;

private:
    TWeakObjectPtr<UObject> ScriptOwner;
    TWeakObjectPtr<UWorld> BoundWorld;
    FCallback Callback;
    uint64 Revision = 0;
    bool Active = false, Executing = false, Pending = false, Finalizing = false;
    FString LastError;
    void Finalize();
    void QueueFinalize();
    friend struct FDasAsyncPhysicsStep;
};
