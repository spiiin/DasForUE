#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DasMontageProxy.generated.h"
class UAnimInstance;
class UAnimMontage;
class USkeletalMeshComponent;
struct FBranchingPointNotifyPayload;

UENUM(BlueprintType)
enum class EDasMontageEventKind : uint8
{
    Completed,
    Interrupted,
    BlendOut,
    NotifyBegin,
    NotifyEnd
};
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasMontageEvent
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    EDasMontageEventKind Kind = EDasMontageEventKind::Completed;
    UPROPERTY(BlueprintReadWrite)
    FName NotifyName;
};

// One playback instance and one managed subscription ID; never stores VM pointers.
UCLASS(Transient)
class DASRUNTIME_API UDasMontageProxy : public UObject
{
    GENERATED_BODY()
public:
    uint64 SubscriptionId = 0;
    int32 InstanceId = INDEX_NONE;
    bool Active = true, Finished = false;
    TWeakObjectPtr<UObject> Owner;
    TWeakObjectPtr<USkeletalMeshComponent> Mesh;
    TWeakObjectPtr<UAnimInstance> Anim;
    TWeakObjectPtr<UAnimMontage> Montage;
    bool IsLive() const;
    bool IsPlaying() const;
    void Attach();
    void Stop(float BlendOut);
    void Deactivate();
    virtual void BeginDestroy() override;
    void BlendingOut(UAnimMontage*, bool Interrupted);
    void Ended(UAnimMontage*, bool Interrupted);
    UFUNCTION()
    void NotifyBegin(FName Name, const FBranchingPointNotifyPayload& Payload);
    UFUNCTION()
    void NotifyEnd(FName Name, const FBranchingPointNotifyPayload& Payload);

private:
    void Disconnect();
    void Queue(EDasMontageEventKind Kind, FName Name = NAME_None);
};
