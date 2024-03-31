#include "ReflectionHelpers.h"

#include "UObject/Object.h"
#include "UObject/Package.h"

UClass* getClassByName(FString className) {
	UObject* classPackage = ANY_PACKAGE;
	UClass* clazz = FindObject<UClass>(classPackage, *className);
	return clazz;
}

UObject* newObjectFromClass(UObject* owner, UClass* clazz, FName name) {
	FStaticConstructObjectParameters params(clazz);
	params.Outer = owner == nullptr ? GetTransientPackage() : owner;
	params.Name = name;
	params.SetFlags = RF_NoFlags;
	params.Template = nullptr;
	params.bCopyTransientsFromClassDefaults = false;
	params.InstanceGraph = nullptr;
	params.ExternalPackage = nullptr;
	return (StaticConstructObject_Internal(params));
}

UObject* newObjectFromClass(FStaticConstructObjectParameters params) {
	return (StaticConstructObject_Internal(params));
}