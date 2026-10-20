#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DasSchema.h"
#include "DasScriptDelegateProxy.generated.h"
struct FDasScriptDelegateLease
{
    bool Active = true;
};
// No VM address is retained: dispatch uses a weak owner, a generation lease and a stable export name.
UCLASS(Transient)
class DASRUNTIME_API UDasScriptDelegateProxy : public UObject
{
    GENERATED_BODY()
public:
    TWeakObjectPtr<UObject> Owner;
    TWeakPtr<FDasScriptDelegateLease> Lease;
    FString Script, Entry, LastError;
    FDasMethodSchema Schema;
    FName Configure(UFunction* Signature, FString& Error);
    bool IsActive() const;
    static void ResetReceivers();
    DECLARE_FUNCTION(execInvoke);

private:
    bool Invoke(UFunction* Signature, const void* Input, void* Staged, FString& Error);
};
// Native event registrations are owned independently of temporary delegate value tokens.
struct FDasOwnedScriptDelegate
{
    TWeakObjectPtr<UObject> Source;
    FName Event;
    FScriptDelegate Delegate;
    ~FDasOwnedScriptDelegate();
};
