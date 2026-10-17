#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AITypes.h"
#include "DasAIMoveProxy.generated.h"
class AAIController;
class UPathFollowingComponent;
struct FPathFollowingResult;
UENUM(BlueprintType)
enum class EDasMoveCode:uint8 {Success,Blocked,OffPath,Aborted,Invalid};
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasMoveResult {
 GENERATED_BODY()
 UPROPERTY(BlueprintReadWrite) EDasMoveCode Code=EDasMoveCode::Invalid;
 UPROPERTY(BlueprintReadWrite) int32 RequestId=0;
};
// Owns only RequestId and its delegate; no VM pointers or controller-wide stop.
UCLASS(Transient)
class DASRUNTIME_API UDasAIMoveProxy:public UObject {
 GENERATED_BODY()
public:
 uint64 SubscriptionId=0;
 FAIRequestID RequestId=FAIRequestID::InvalidRequest;
 bool Active=true,Finished=false;
 TWeakObjectPtr<UObject> Owner;
 TWeakObjectPtr<AAIController> Controller;
 TWeakObjectPtr<UPathFollowingComponent> Following;
 FDelegateHandle Completion;
 bool IsLive()const;
 void Attach();
 void Deactivate();
 void Finish(FAIRequestID Id,const FPathFollowingResult& Result);
 void Queue(EDasMoveCode Code);
 virtual void BeginDestroy()override;
};
