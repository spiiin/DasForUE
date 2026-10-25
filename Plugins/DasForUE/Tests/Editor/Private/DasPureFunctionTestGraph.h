#pragma once
#include "CoreMinimal.h"
class UBlueprint;
class UPackage;
class UFunction;
bool AddDasPureConsumerGraph(UBlueprint* BP, UFunction* Function, FString& Error, bool UseDefaults = false);
UBlueprint* CreateDasPureConsumer(UClass* Parent, UPackage* Package, FName Name, FString& Error,
                                  FName Method = NAME_None, bool UseDefaults = false);
