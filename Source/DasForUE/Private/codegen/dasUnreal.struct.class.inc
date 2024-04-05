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

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:906:9
struct UObject_FAssetRegistryTagMetadata_GeneratedAnnotation : ManagedStructureAnnotation<UObject::FAssetRegistryTagMetadata> {
	UObject_FAssetRegistryTagMetadata_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FAssetRegistryTagMetadata", ml, "UObject::FAssetRegistryTagMetadata") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(DisplayName)>("DisplayName","DisplayName");
		addField<DAS_BIND_MANAGED_FIELD(TooltipText)>("TooltipText","TooltipText");
		addField<DAS_BIND_MANAGED_FIELD(Suffix)>("Suffix","Suffix");
		addField<DAS_BIND_MANAGED_FIELD(ImportantValue)>("ImportantValue","ImportantValue");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:85:7
struct UObject_GeneratedAnnotation : ManagedStructureAnnotation<UObject, false, false> {
	UObject_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("UObject", ml, "UObject") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1723:8
struct FObjectNetPushIdHelper_GeneratedAnnotation : ManagedStructureAnnotation<FObjectNetPushIdHelper> {
	FObjectNetPushIdHelper_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FObjectNetPushIdHelper", ml, "FObjectNetPushIdHelper") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:155:7
struct UField_GeneratedAnnotation : ManagedStructureAnnotation<UField, false, false> {
	UField_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("UField", ml, "UField") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(Next)>("Next","Next");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:383:7
struct UStruct_GeneratedAnnotation : ManagedStructureAnnotation<UStruct, false, false> {
	UStruct_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("UStruct", ml, "UStruct") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(PropertiesSize)>("PropertiesSize","PropertiesSize");
		addField<DAS_BIND_MANAGED_FIELD(MinAlignment)>("MinAlignment","MinAlignment");
		addField<DAS_BIND_MANAGED_FIELD(FieldPathSerialNumber)>("FieldPathSerialNumber","FieldPathSerialNumber");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1999:7
struct UEnum_GeneratedAnnotation : ManagedStructureAnnotation<UEnum, false, false> {
	UEnum_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("UEnum", ml, "UEnum") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(CppType)>("CppType","CppType");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2601:8
struct FCppClassTypeTraitsBase_GeneratedAnnotation : ManagedStructureAnnotation<FCppClassTypeTraitsBase> {
	FCppClassTypeTraitsBase_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FCppClassTypeTraitsBase", ml, "FCppClassTypeTraitsBase") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2622:8
struct ICppClassTypeInfo_GeneratedAnnotation : ManagedStructureAnnotation<ICppClassTypeInfo> {
	ICppClassTypeInfo_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("ICppClassTypeInfo", ml, "ICppClassTypeInfo") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2629:8
struct FCppClassTypeInfoStatic_GeneratedAnnotation : ManagedStructureAnnotation<FCppClassTypeInfoStatic> {
	FCppClassTypeInfoStatic_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FCppClassTypeInfoStatic", ml, "FCppClassTypeInfoStatic") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(bIsAbstract)>("bIsAbstract","bIsAbstract");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2636:8
struct FCppClassTypeInfo_GeneratedAnnotation : ManagedStructureAnnotation<FCppClassTypeInfo> {
	FCppClassTypeInfo_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FCppClassTypeInfo", ml, "FCppClassTypeInfo") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2659:8
struct FImplementedInterface_GeneratedAnnotation : ManagedStructureAnnotation<FImplementedInterface> {
	FImplementedInterface_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FImplementedInterface", ml, "FImplementedInterface") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(PointerOffset)>("PointerOffset","PointerOffset");
		addField<DAS_BIND_MANAGED_FIELD(bImplementedByK2)>("bImplementedByK2","bImplementedByK2");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2684:8
struct FNativeFunctionLookup_GeneratedAnnotation : ManagedStructureAnnotation<FNativeFunctionLookup> {
	FNativeFunctionLookup_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FNativeFunctionLookup", ml, "FNativeFunctionLookup") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(Name)>("Name","Name");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2705:8
struct FClassFunctionLinkInfo_GeneratedAnnotation : ManagedStructureAnnotation<FClassFunctionLinkInfo> {
	FClassFunctionLinkInfo_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FClassFunctionLinkInfo", ml, "FClassFunctionLinkInfo") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(FuncNameUTF8)>("FuncNameUTF8","FuncNameUTF8");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2726:7
struct UClass_GeneratedAnnotation : ManagedStructureAnnotation<UClass, false, false> {
	UClass_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("UClass", ml, "UClass") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(ClassUnique)>("ClassUnique","ClassUnique");
		addField<DAS_BIND_MANAGED_FIELD(FirstOwnedClassRep)>("FirstOwnedClassRep","FirstOwnedClassRep");
		addField<DAS_BIND_MANAGED_FIELD(bCooked)>("bCooked","bCooked");
		addField<DAS_BIND_MANAGED_FIELD(bLayoutChanging)>("bLayoutChanging","bLayoutChanging");
		addField<DAS_BIND_MANAGED_FIELD(ClassFlags)>("ClassFlags","ClassFlags");
		addField<DAS_BIND_MANAGED_FIELD(ClassCastFlags)>("ClassCastFlags","ClassCastFlags");
		addField<DAS_BIND_MANAGED_FIELD(ClassConfigName)>("ClassConfigName","ClassConfigName");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:4037:8
struct FTestUninitializedScriptStructMembersTest_GeneratedAnnotation : ManagedStructureAnnotation<FTestUninitializedScriptStructMembersTest> {
	FTestUninitializedScriptStructMembersTest_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FTestUninitializedScriptStructMembersTest", ml, "FTestUninitializedScriptStructMembersTest") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(UninitializedObjectReference)>("UninitializedObjectReference","UninitializedObjectReference");
		addField<DAS_BIND_MANAGED_FIELD(InitializedObjectReference)>("InitializedObjectReference","InitializedObjectReference");
		addField<DAS_BIND_MANAGED_FIELD(UnusedValue)>("UnusedValue","UnusedValue");
	}
};

