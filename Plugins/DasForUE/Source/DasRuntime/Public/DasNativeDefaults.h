#pragma once
#include "CoreMinimal.h"
struct FDasDeclaredLiteral;struct FDasDeclaredClass;struct FDasAuthoringModule;
class FProperty;
DASRUNTIME_API bool DasParseEditableDefault(FProperty*,const FDasDeclaredLiteral&,void*,FString&);
DASRUNTIME_API UClass* DasNativeDefaultBase(const FString& NativeParent);
DASRUNTIME_API UObject* DasNativeDefaultTarget(const FString& NativeParent,UObject* Host,const FString& Component,FString& Error);
DASRUNTIME_API bool DasValidateNativeDefaults(const FDasDeclaredClass&,FString&);
DASRUNTIME_API FString DasNativeDefaultKey(const FString& Id,const FString& Component,const FString& Property);
DASRUNTIME_API FString DasNativeDefaultContract(const FDasDeclaredLiteral&);
DASRUNTIME_API bool DasCheckNativeDefaultOwner(const FDasAuthoringModule&,const FString& Id,UObject* Self,FString& Error);
