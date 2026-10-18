#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/CollisionProfile.h"
#include "DasCollisionProfileProbe.generated.h"
UCLASS()
class DASRUNTIME_API ADasCollisionProfileProbe:public AActor {
 GENERATED_BODY()
public:
 UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Probe") FCollisionProfileName Profile=FCollisionProfileName(TEXT("Before"));
 UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Probe") FCollisionProfileName Readonly=FCollisionProfileName(TEXT("Before"));
 UPROPERTY() FCollisionProfileName Hidden=FCollisionProfileName(TEXT("Before"));
 UPROPERTY() FCollisionProfileName Fixed[2];
 UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Probe") TArray<FCollisionProfileName> Profiles;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Probe") int32 Counter=7;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Probe") FVector Direction=FVector::ZeroVector;
 UPROPERTY(BlueprintGetter=ReadSemantic,BlueprintSetter=WriteSemantic,Category="Probe") FCollisionProfileName Semantic=FCollisionProfileName(TEXT("Before"));
 UPROPERTY(BlueprintReadOnly,Category="Probe") TObjectPtr<AActor> OtherActor;
 int32 SetterCalls=0;
 UFUNCTION(BlueprintPure,Category="Probe") FCollisionProfileName ReadSemantic()const{return Semantic;}
 UFUNCTION(BlueprintCallable,Category="Probe") void WriteSemantic(const FCollisionProfileName& Value){Semantic=Value;++SetterCalls;}
 UFUNCTION(BlueprintCallable,Category="Probe") void DestroyOther(){if(IsValid(OtherActor))OtherActor->Destroy();}
};
UCLASS()
class DASRUNTIME_API ADasCollisionProfileDerivedProbe:public ADasCollisionProfileProbe {
 GENERATED_BODY()
};
