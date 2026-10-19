#pragma once
#include "CoreMinimal.h"
#include "DasAsyncPhysicsBridge.h"
class UPrimitiveComponent;
// Opaque value representation in DAS. No exposed fields or pointer conversions.
struct FDasPhysicsBodyToken
{
    uint64 Value = 0;
};
class FDasPhysicsBodyScope;
class FDasPhysicsBodyBindings
{
public:
    static constexpr int32 Capacity = 8, MaxCallsPerStep = 64;
    explicit FDasPhysicsBodyBindings(UObject* Owner);
    ~FDasPhysicsBodyBindings();
    bool Bind(int32 Slot, UPrimitiveComponent* Body, FString& Error);
    bool Clear(int32 Slot, FString& Error);

private:
    struct FImpl;
    TUniquePtr<FImpl> Impl;
    friend class FDasPhysicsBodyScope;
};
// Stack-only transaction. Tokens are meaningful exclusively in this step and
// this VM. Forces are bounded and applied before leaving the same preSim step,
// only after the script has completed successfully and all bodies revalidate.
class FDasPhysicsBodyScope
{
    FDasPhysicsBodyBindings& Bindings;
    const FDasAsyncPhysicsStep& Step;
    uint64 Tokens[FDasPhysicsBodyBindings::Capacity] = {};
    FVector Pending[FDasPhysicsBodyBindings::Capacity] = {};
    bool Touched[FDasPhysicsBodyBindings::Capacity] = {};
    int32 Calls = 0;

public:
    FDasPhysicsBodyScope(FDasPhysicsBodyBindings& In, const FDasAsyncPhysicsStep& S) : Bindings(In), Step(S)
    {
        for (auto& V : Pending)
            V = FVector::ZeroVector;
    }
    FDasPhysicsBodyScope(const FDasPhysicsBodyScope&) = delete;
    FDasPhysicsBodyScope& operator=(const FDasPhysicsBodyScope&) = delete;
    uint64 Acquire(int32 Slot, FString& Error);
    bool AddForce(uint64 Token, const FVector& Force, bool AccelChange, FString& Error);
    bool Commit(FString& Error);
};
