#pragma once
#include "BlueprintCompilerExtension.h"
#include "DasComponentCompilerExtension.generated.h"
UCLASS()
class UDasComponentCompilerExtension:public UBlueprintCompilerExtension {
 GENERATED_BODY()
protected:
 virtual void ProcessBlueprintCompiled(const FKismetCompilerContext& Context,const FBlueprintCompiledData& Data)override;
};
