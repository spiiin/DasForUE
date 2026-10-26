#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DasApiMathFixtures.generated.h"

// Editor-only fixtures: exercise the aggregate marshalling contract, not gameplay API.
USTRUCT(BlueprintType)
struct FDasApiMathPayload
{
    GENERATED_BODY()
    UPROPERTY()
    FVector2D Point = FVector2D::ZeroVector;
    UPROPERTY()
    FVector Direction = FVector::ZeroVector;
    UPROPERTY()
    FRotator Rotation = FRotator::ZeroRotator;
    UPROPERTY()
    FTransform Pose = FTransform::Identity;
    UPROPERTY()
    TArray<FTransform> Poses;
    UPROPERTY()
    TArray<FVector> Directions;
};

UCLASS()
class UDasApiMathLibraryFixture : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure, Category = "daScript Test")
    static FDasApiMathPayload EchoPayload(const FDasApiMathPayload& Value) { return Value; }
    UFUNCTION(BlueprintPure, Category = "daScript Test")
    static TArray<FTransform> EchoPoses(const TArray<FTransform>& Values) { return Values; }
    UFUNCTION(BlueprintCallable, Category = "daScript Test")
    static void AdjustPayload(UPARAM(ref) FDasApiMathPayload& Value)
    {
        Value.Direction.X += 10;
        Value.Pose.AddToTranslation(FVector(1, 2, 3));
    }
};
