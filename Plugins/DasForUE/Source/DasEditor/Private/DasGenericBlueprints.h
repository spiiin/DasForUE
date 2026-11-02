#pragma once
#include "CoreMinimal.h"
class UBlueprint;
class UK2Node_CallFunction;
class UDasScriptClassData;
struct FDasDeclaredClass;
struct FDasClassSchema;
bool DasCheckBlueprintParent(UClass* Parent, FString& Error);
UDasScriptClassData* DasGenericClassData(UBlueprint* BP);
bool DasCanReimportGenericBlueprint(UBlueprint* BP);
bool DasWireGenericOwner(UK2Node_CallFunction* Call, UBlueprint* BP);
bool DasPrepareGenericBlueprint(UBlueprint* BP, const FDasDeclaredClass& Class, const FDasClassSchema& Schema,
                                const FString& Script, bool BodyOnly, FString& Error);
void DasFinishGenericBlueprint(UBlueprint* BP);
