#pragma once

#include "CoreMinimal.h"

class UObject;
class UClass;

static UClass* getClassByName(FString className);
static UObject* newObjectFromClass(UObject* owner, UClass* clazz, FName name);
static UObject* newObjectFromClass(FStaticConstructObjectParameters params);