#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"

#include "TestActor.generated.h"

class UStaticMeshComponent;

UCLASS(BlueprintType, Blueprintable)
class DASFORUE_API ATestActor : public AActor {
public:

private:
	GENERATED_BODY()
	bool bTickCalled = false;
public:
	ATestActor();

protected:
	virtual void BeginPlay() override;
	
public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMesh;

	UFUNCTION()
	void SetColorByStringInMesh(FString Color);

	UFUNCTION(BlueprintCallable)
	void SetTestActorLocation(FVector NewLocation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Script")
	FString ScriptFilePath; //set this path to compile and run script from file
};
