#pragma once
#include "DasAuthoring.h"
#include "DasNativeDefaults.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/UnrealType.h"
#include "UObject/StrProperty.h"

// Shared parser for both initial import and staged default merging.
static bool DasComponentDefaultValue(FProperty* P,const FDasDeclaredLiteral& V,void* Data,FString& Error){return DasParseEditableDefault(P,V,Data,Error);}
static bool DasComponentDefaults(UClass* Class,const FDasDeclaredComponent& C,UObject* Template,FString& Error){
 for(const auto& Pair:C.Defaults)if(!DasComponentDefaultValue(Class->FindPropertyByName(*Pair.Key),Pair.Value,nullptr,Error))return false;
 if(Template)for(const auto& Pair:C.Defaults){
  auto* P=Class->FindPropertyByName(*Pair.Key);Template->PreEditChange(P);
  if(!DasComponentDefaultValue(P,Pair.Value,P->ContainerPtrToValuePtr<void>(Template),Error))return false;
  FPropertyChangedEvent Changed(P,EPropertyChangeType::ValueSet);Template->PostEditChangeProperty(Changed);
 }
 return true;
}
