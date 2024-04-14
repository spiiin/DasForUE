#pragma once

#include "CoreMinimal.h"

#include "TestClass.generated.h"

class UStaticMeshComponent;

UCLASS(BlueprintType, Blueprintable)
class DASFORUE_API UTestClass : public UObject {
public:

private:
	GENERATED_BODY()
public:
	UTestClass();
public:

	UFUNCTION(BlueprintCallable)
	void SaySomething(FString Msg);

	UFUNCTION(BlueprintCallable)
	void SaySomethingElse(FString Msg);

	UFUNCTION(BlueprintCallable)
	FString SaySomething4(int a, FString Msg, float b, int c);

	UFUNCTION(BlueprintCallable)
	void SayArray(TArray<int> IntArray);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TestProperty1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TestProperty2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TestProperty3;
};
