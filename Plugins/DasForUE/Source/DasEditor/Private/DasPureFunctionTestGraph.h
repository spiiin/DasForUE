#pragma once
#include "CoreMinimal.h"
class UBlueprint;class UPackage;class UFunction;
bool AddDasPureConsumerGraph(UBlueprint* BP,UFunction* Function,FString& Error);
UBlueprint* CreateDasPureConsumer(UClass* Parent,UPackage* Package,FName Name,FString& Error);
