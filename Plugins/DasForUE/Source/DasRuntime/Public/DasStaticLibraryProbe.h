#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Components/ActorComponent.h"
#include "DasStaticLibraryProbe.generated.h"
UCLASS()
class DASRUNTIME_API ADasStaticLibraryProbe : public AActor {
 GENERATED_BODY()
public:
 UPROPERTY() TSubclassOf<UBlueprintFunctionLibrary> LibraryClass;
 UPROPERTY() TSubclassOf<UBlueprintFunctionLibrary> ValuesClass;
 // Reflected references retain the complete Blueprint hierarchy during cook/GC.
 UPROPERTY() TSubclassOf<AActor> DefaultActorClass;
 UPROPERTY() TSubclassOf<AActor> DefaultChildClass;
 UPROPERTY() TSubclassOf<UActorComponent> DefaultComponentClass;
};
DASRUNTIME_API bool RunDasStaticLibraryProbe(UClass* Library,FString& Report);

DASRUNTIME_API bool RunDasLibraryValuesProbe(UClass* Library,UObject* WorldContext,FString& Report);

DASRUNTIME_API bool RunDasInstanceDefaultsProbe(ADasStaticLibraryProbe* Carrier,FString& Report);
