// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:55:7
struct FString_GeneratedAnnotation : ManagedStructureAnnotation<FString> {
	virtual bool canCopy() const override { return std::is_copy_constructible<FString>::value; }
	virtual bool canClone() const override { return std::is_copy_constructible<FString>::value; }
	virtual bool canMove() const override { return std::is_copy_constructible<FString>::value; }
	FString_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FString", ml, "FString") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2534:8
struct FTextRange_GeneratedAnnotation : ManagedStructureAnnotation<FTextRange> {
	FTextRange_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FTextRange", ml, "FTextRange") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(BeginIndex)>("BeginIndex","BeginIndex");
		addField<DAS_BIND_MANAGED_FIELD(EndIndex)>("EndIndex","EndIndex");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:167:7
struct FVTableHelper_GeneratedAnnotation : ManagedStructureAnnotation<FVTableHelper> {
	FVTableHelper_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FVTableHelper", ml, "FVTableHelper") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:2178:7
struct FRestoreForUObjectOverwrite_GeneratedAnnotation : ManagedStructureAnnotation<FRestoreForUObjectOverwrite> {
	FRestoreForUObjectOverwrite_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FRestoreForUObjectOverwrite", ml, "FRestoreForUObjectOverwrite") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:33:7
struct UObjectBase_GeneratedAnnotation : ManagedStructureAnnotation<UObjectBase> {
	UObjectBase_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("UObjectBase", ml, "UObjectBase") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:57:7
struct UObjectBaseUtility_GeneratedAnnotation : ManagedStructureAnnotation<UObjectBaseUtility> {
	UObjectBaseUtility_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("UObjectBaseUtility", ml, "UObjectBaseUtility") {
	}
	void init () {
	}
};

