// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers\UnrealString.h:55:7
struct FString_GeneratedAnnotation : ManagedStructureAnnotation<FString> {
	virtual bool canCopy() const override { return std::is_copy_constructible<FString>::value; }
	virtual bool canClone() const override { return std::is_copy_constructible<FString>::value; }
	virtual bool canMove() const override { return std::is_copy_constructible<FString>::value; }
	FString_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FString", ml, "FString") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers\UnrealString.h:2534:8
struct FTextRange_GeneratedAnnotation : ManagedStructureAnnotation<FTextRange> {
	FTextRange_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FTextRange", ml, "FTextRange") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(BeginIndex)>("BeginIndex","BeginIndex");
		addField<DAS_BIND_MANAGED_FIELD(EndIndex)>("EndIndex","EndIndex");
	}
};

