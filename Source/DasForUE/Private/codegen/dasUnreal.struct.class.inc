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

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1034:7
struct FObjectInitializer_GeneratedAnnotation : ManagedStructureAnnotation<FObjectInitializer> {
	FObjectInitializer_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FObjectInitializer", ml, "FObjectInitializer") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1483:8
struct FStaticConstructObjectParameters_GeneratedAnnotation : ManagedStructureAnnotation<FStaticConstructObjectParameters> {
	FStaticConstructObjectParameters_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FStaticConstructObjectParameters", ml, "FStaticConstructObjectParameters") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(Class)>("Class","Class");
		addField<DAS_BIND_MANAGED_FIELD(Outer)>("Outer","Outer");
		addField<DAS_BIND_MANAGED_FIELD(Name)>("Name","Name");
		addField<DAS_BIND_MANAGED_FIELD(SetFlags)>("SetFlags","SetFlags");
		addField<DAS_BIND_MANAGED_FIELD(InternalSetFlags)>("InternalSetFlags","InternalSetFlags");
		addField<DAS_BIND_MANAGED_FIELD(bCopyTransientsFromClassDefaults)>("bCopyTransientsFromClassDefaults","bCopyTransientsFromClassDefaults");
		addField<DAS_BIND_MANAGED_FIELD(bAssumeTemplateIsArchetype)>("bAssumeTemplateIsArchetype","bAssumeTemplateIsArchetype");
		addField<DAS_BIND_MANAGED_FIELD(Template)>("Template","Template");
		addField<DAS_BIND_MANAGED_FIELD(ExternalPackage)>("ExternalPackage","ExternalPackage");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1534:7
struct FScriptIntegrationObjectHelper_GeneratedAnnotation : ManagedStructureAnnotation<FScriptIntegrationObjectHelper> {
	FScriptIntegrationObjectHelper_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FScriptIntegrationObjectHelper", ml, "FScriptIntegrationObjectHelper") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2132:7
struct FVerySlowReferenceCollectorArchiveScope_GeneratedAnnotation : ManagedStructureAnnotation<FVerySlowReferenceCollectorArchiveScope> {
	FVerySlowReferenceCollectorArchiveScope_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FVerySlowReferenceCollectorArchiveScope", ml, "FVerySlowReferenceCollectorArchiveScope") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2602:9
struct FReferenceCollector_AROPrivate_GeneratedAnnotation : ManagedStructureAnnotation<FReferenceCollector::AROPrivate> {
	FReferenceCollector_AROPrivate_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("AROPrivate", ml, "FReferenceCollector::AROPrivate") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2163:7
struct FReferenceCollector_GeneratedAnnotation : ManagedStructureAnnotation<FReferenceCollector> {
	FReferenceCollector_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FReferenceCollector", ml, "FReferenceCollector") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2769:7
struct FReferenceFinder_GeneratedAnnotation : ManagedStructureAnnotation<FReferenceFinder> {
	FReferenceFinder_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FReferenceFinder", ml, "FReferenceFinder") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2850:8
struct FEndLoadPackageContext_GeneratedAnnotation : ManagedStructureAnnotation<FEndLoadPackageContext> {
	FEndLoadPackageContext_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FEndLoadPackageContext", ml, "FEndLoadPackageContext") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(RecursiveDepth)>("RecursiveDepth","RecursiveDepth");
		addField<DAS_BIND_MANAGED_FIELD(bSynchronous)>("bSynchronous","bSynchronous");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2860:8
struct FCoreUObjectDelegates_GeneratedAnnotation : ManagedStructureAnnotation<FCoreUObjectDelegates> {
	FCoreUObjectDelegates_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FCoreUObjectDelegates", ml, "FCoreUObjectDelegates") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3096:97
struct FDynamicClassStaticData_GeneratedAnnotation : ManagedStructureAnnotation<FDynamicClassStaticData> {
	FDynamicClassStaticData_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FDynamicClassStaticData", ml, "FDynamicClassStaticData") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3116:7
struct UE_FAssetLog_GeneratedAnnotation : ManagedStructureAnnotation<UE::FAssetLog> {
	UE_FAssetLog_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FAssetLog", ml, "UE::FAssetLog") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3148:8
struct FAssetMsg_GeneratedAnnotation : ManagedStructureAnnotation<FAssetMsg> {
	FAssetMsg_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FAssetMsg", ml, "FAssetMsg") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3280:9
struct UECodeGen_Private_FMetaDataPairParam_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FMetaDataPairParam> {
	UECodeGen_Private_FMetaDataPairParam_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FMetaDataPairParam", ml, "UECodeGen_Private::FMetaDataPairParam") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(ValueUTF8)>("ValueUTF8","ValueUTF8");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3287:9
struct UECodeGen_Private_FEnumeratorParam_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FEnumeratorParam> {
	UECodeGen_Private_FEnumeratorParam_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FEnumeratorParam", ml, "UECodeGen_Private::FEnumeratorParam") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(Value)>("Value","Value");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3295:9
struct UECodeGen_Private_FPropertyParamsBase_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FPropertyParamsBase> {
	UECodeGen_Private_FPropertyParamsBase_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FPropertyParamsBase", ml, "UECodeGen_Private::FPropertyParamsBase") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(RepNotifyFuncUTF8)>("RepNotifyFuncUTF8","RepNotifyFuncUTF8");
		addField<DAS_BIND_MANAGED_FIELD(PropertyFlags)>("PropertyFlags","PropertyFlags");
		addField<DAS_BIND_MANAGED_FIELD(Flags)>("Flags","Flags");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(ArrayDim)>("ArrayDim","ArrayDim");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3307:9
struct UECodeGen_Private_FPropertyParamsBaseWithoutOffset_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FPropertyParamsBaseWithoutOffset> {
	UECodeGen_Private_FPropertyParamsBaseWithoutOffset_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FPropertyParamsBaseWithoutOffset", ml, "UECodeGen_Private::FPropertyParamsBaseWithoutOffset") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(RepNotifyFuncUTF8)>("RepNotifyFuncUTF8","RepNotifyFuncUTF8");
		addField<DAS_BIND_MANAGED_FIELD(PropertyFlags)>("PropertyFlags","PropertyFlags");
		addField<DAS_BIND_MANAGED_FIELD(Flags)>("Flags","Flags");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(ArrayDim)>("ArrayDim","ArrayDim");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3319:9
struct UECodeGen_Private_FPropertyParamsBaseWithOffset_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FPropertyParamsBaseWithOffset> {
	UECodeGen_Private_FPropertyParamsBaseWithOffset_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FPropertyParamsBaseWithOffset", ml, "UECodeGen_Private::FPropertyParamsBaseWithOffset") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(RepNotifyFuncUTF8)>("RepNotifyFuncUTF8","RepNotifyFuncUTF8");
		addField<DAS_BIND_MANAGED_FIELD(PropertyFlags)>("PropertyFlags","PropertyFlags");
		addField<DAS_BIND_MANAGED_FIELD(Flags)>("Flags","Flags");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(ArrayDim)>("ArrayDim","ArrayDim");
		addField<DAS_BIND_MANAGED_FIELD(Offset)>("Offset","Offset");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3332:9
struct UECodeGen_Private_FGenericPropertyParams_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FGenericPropertyParams> {
	UECodeGen_Private_FGenericPropertyParams_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FGenericPropertyParams", ml, "UECodeGen_Private::FGenericPropertyParams") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(RepNotifyFuncUTF8)>("RepNotifyFuncUTF8","RepNotifyFuncUTF8");
		addField<DAS_BIND_MANAGED_FIELD(PropertyFlags)>("PropertyFlags","PropertyFlags");
		addField<DAS_BIND_MANAGED_FIELD(Flags)>("Flags","Flags");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(ArrayDim)>("ArrayDim","ArrayDim");
		addField<DAS_BIND_MANAGED_FIELD(Offset)>("Offset","Offset");
		addField<DAS_BIND_MANAGED_FIELD(NumMetaData)>("NumMetaData","NumMetaData");
		addField<DAS_BIND_MANAGED_FIELD(MetaDataArray)>("MetaDataArray","MetaDataArray");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3349:9
struct UECodeGen_Private_FBytePropertyParams_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FBytePropertyParams> {
	UECodeGen_Private_FBytePropertyParams_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FBytePropertyParams", ml, "UECodeGen_Private::FBytePropertyParams") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(RepNotifyFuncUTF8)>("RepNotifyFuncUTF8","RepNotifyFuncUTF8");
		addField<DAS_BIND_MANAGED_FIELD(PropertyFlags)>("PropertyFlags","PropertyFlags");
		addField<DAS_BIND_MANAGED_FIELD(Flags)>("Flags","Flags");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(ArrayDim)>("ArrayDim","ArrayDim");
		addField<DAS_BIND_MANAGED_FIELD(Offset)>("Offset","Offset");
		addField<DAS_BIND_MANAGED_FIELD(NumMetaData)>("NumMetaData","NumMetaData");
		addField<DAS_BIND_MANAGED_FIELD(MetaDataArray)>("MetaDataArray","MetaDataArray");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3367:9
struct UECodeGen_Private_FBoolPropertyParams_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FBoolPropertyParams> {
	UECodeGen_Private_FBoolPropertyParams_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FBoolPropertyParams", ml, "UECodeGen_Private::FBoolPropertyParams") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(RepNotifyFuncUTF8)>("RepNotifyFuncUTF8","RepNotifyFuncUTF8");
		addField<DAS_BIND_MANAGED_FIELD(PropertyFlags)>("PropertyFlags","PropertyFlags");
		addField<DAS_BIND_MANAGED_FIELD(Flags)>("Flags","Flags");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(ArrayDim)>("ArrayDim","ArrayDim");
		addField<DAS_BIND_MANAGED_FIELD(ElementSize)>("ElementSize","ElementSize");
		addField<DAS_BIND_MANAGED_FIELD(SizeOfOuter)>("SizeOfOuter","SizeOfOuter");
		addField<DAS_BIND_MANAGED_FIELD(NumMetaData)>("NumMetaData","NumMetaData");
		addField<DAS_BIND_MANAGED_FIELD(MetaDataArray)>("MetaDataArray","MetaDataArray");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3386:9
struct UECodeGen_Private_FObjectPropertyParamsWithoutClass_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FObjectPropertyParamsWithoutClass> {
	UECodeGen_Private_FObjectPropertyParamsWithoutClass_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FObjectPropertyParamsWithoutClass", ml, "UECodeGen_Private::FObjectPropertyParamsWithoutClass") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(RepNotifyFuncUTF8)>("RepNotifyFuncUTF8","RepNotifyFuncUTF8");
		addField<DAS_BIND_MANAGED_FIELD(PropertyFlags)>("PropertyFlags","PropertyFlags");
		addField<DAS_BIND_MANAGED_FIELD(Flags)>("Flags","Flags");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(ArrayDim)>("ArrayDim","ArrayDim");
		addField<DAS_BIND_MANAGED_FIELD(Offset)>("Offset","Offset");
		addField<DAS_BIND_MANAGED_FIELD(NumMetaData)>("NumMetaData","NumMetaData");
		addField<DAS_BIND_MANAGED_FIELD(MetaDataArray)>("MetaDataArray","MetaDataArray");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3403:9
struct UECodeGen_Private_FObjectPropertyParamsWithClass_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FObjectPropertyParamsWithClass> {
	UECodeGen_Private_FObjectPropertyParamsWithClass_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FObjectPropertyParamsWithClass", ml, "UECodeGen_Private::FObjectPropertyParamsWithClass") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(RepNotifyFuncUTF8)>("RepNotifyFuncUTF8","RepNotifyFuncUTF8");
		addField<DAS_BIND_MANAGED_FIELD(PropertyFlags)>("PropertyFlags","PropertyFlags");
		addField<DAS_BIND_MANAGED_FIELD(Flags)>("Flags","Flags");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(ArrayDim)>("ArrayDim","ArrayDim");
		addField<DAS_BIND_MANAGED_FIELD(Offset)>("Offset","Offset");
		addField<DAS_BIND_MANAGED_FIELD(NumMetaData)>("NumMetaData","NumMetaData");
		addField<DAS_BIND_MANAGED_FIELD(MetaDataArray)>("MetaDataArray","MetaDataArray");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3421:9
struct UECodeGen_Private_FObjectPropertyParams_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FObjectPropertyParams> {
	UECodeGen_Private_FObjectPropertyParams_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FObjectPropertyParams", ml, "UECodeGen_Private::FObjectPropertyParams") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(RepNotifyFuncUTF8)>("RepNotifyFuncUTF8","RepNotifyFuncUTF8");
		addField<DAS_BIND_MANAGED_FIELD(PropertyFlags)>("PropertyFlags","PropertyFlags");
		addField<DAS_BIND_MANAGED_FIELD(Flags)>("Flags","Flags");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(ArrayDim)>("ArrayDim","ArrayDim");
		addField<DAS_BIND_MANAGED_FIELD(Offset)>("Offset","Offset");
		addField<DAS_BIND_MANAGED_FIELD(NumMetaData)>("NumMetaData","NumMetaData");
		addField<DAS_BIND_MANAGED_FIELD(MetaDataArray)>("MetaDataArray","MetaDataArray");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3439:9
struct UECodeGen_Private_FClassPropertyParams_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FClassPropertyParams> {
	UECodeGen_Private_FClassPropertyParams_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FClassPropertyParams", ml, "UECodeGen_Private::FClassPropertyParams") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(RepNotifyFuncUTF8)>("RepNotifyFuncUTF8","RepNotifyFuncUTF8");
		addField<DAS_BIND_MANAGED_FIELD(PropertyFlags)>("PropertyFlags","PropertyFlags");
		addField<DAS_BIND_MANAGED_FIELD(Flags)>("Flags","Flags");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(ArrayDim)>("ArrayDim","ArrayDim");
		addField<DAS_BIND_MANAGED_FIELD(Offset)>("Offset","Offset");
		addField<DAS_BIND_MANAGED_FIELD(NumMetaData)>("NumMetaData","NumMetaData");
		addField<DAS_BIND_MANAGED_FIELD(MetaDataArray)>("MetaDataArray","MetaDataArray");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3458:9
struct UECodeGen_Private_FSoftClassPropertyParams_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FSoftClassPropertyParams> {
	UECodeGen_Private_FSoftClassPropertyParams_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FSoftClassPropertyParams", ml, "UECodeGen_Private::FSoftClassPropertyParams") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(RepNotifyFuncUTF8)>("RepNotifyFuncUTF8","RepNotifyFuncUTF8");
		addField<DAS_BIND_MANAGED_FIELD(PropertyFlags)>("PropertyFlags","PropertyFlags");
		addField<DAS_BIND_MANAGED_FIELD(Flags)>("Flags","Flags");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(ArrayDim)>("ArrayDim","ArrayDim");
		addField<DAS_BIND_MANAGED_FIELD(Offset)>("Offset","Offset");
		addField<DAS_BIND_MANAGED_FIELD(NumMetaData)>("NumMetaData","NumMetaData");
		addField<DAS_BIND_MANAGED_FIELD(MetaDataArray)>("MetaDataArray","MetaDataArray");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3476:9
struct UECodeGen_Private_FInterfacePropertyParams_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FInterfacePropertyParams> {
	UECodeGen_Private_FInterfacePropertyParams_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FInterfacePropertyParams", ml, "UECodeGen_Private::FInterfacePropertyParams") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(RepNotifyFuncUTF8)>("RepNotifyFuncUTF8","RepNotifyFuncUTF8");
		addField<DAS_BIND_MANAGED_FIELD(PropertyFlags)>("PropertyFlags","PropertyFlags");
		addField<DAS_BIND_MANAGED_FIELD(Flags)>("Flags","Flags");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(ArrayDim)>("ArrayDim","ArrayDim");
		addField<DAS_BIND_MANAGED_FIELD(Offset)>("Offset","Offset");
		addField<DAS_BIND_MANAGED_FIELD(NumMetaData)>("NumMetaData","NumMetaData");
		addField<DAS_BIND_MANAGED_FIELD(MetaDataArray)>("MetaDataArray","MetaDataArray");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3494:9
struct UECodeGen_Private_FStructPropertyParams_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FStructPropertyParams> {
	UECodeGen_Private_FStructPropertyParams_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FStructPropertyParams", ml, "UECodeGen_Private::FStructPropertyParams") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(RepNotifyFuncUTF8)>("RepNotifyFuncUTF8","RepNotifyFuncUTF8");
		addField<DAS_BIND_MANAGED_FIELD(PropertyFlags)>("PropertyFlags","PropertyFlags");
		addField<DAS_BIND_MANAGED_FIELD(Flags)>("Flags","Flags");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(ArrayDim)>("ArrayDim","ArrayDim");
		addField<DAS_BIND_MANAGED_FIELD(Offset)>("Offset","Offset");
		addField<DAS_BIND_MANAGED_FIELD(NumMetaData)>("NumMetaData","NumMetaData");
		addField<DAS_BIND_MANAGED_FIELD(MetaDataArray)>("MetaDataArray","MetaDataArray");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3512:9
struct UECodeGen_Private_FDelegatePropertyParams_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FDelegatePropertyParams> {
	UECodeGen_Private_FDelegatePropertyParams_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FDelegatePropertyParams", ml, "UECodeGen_Private::FDelegatePropertyParams") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(RepNotifyFuncUTF8)>("RepNotifyFuncUTF8","RepNotifyFuncUTF8");
		addField<DAS_BIND_MANAGED_FIELD(PropertyFlags)>("PropertyFlags","PropertyFlags");
		addField<DAS_BIND_MANAGED_FIELD(Flags)>("Flags","Flags");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(ArrayDim)>("ArrayDim","ArrayDim");
		addField<DAS_BIND_MANAGED_FIELD(Offset)>("Offset","Offset");
		addField<DAS_BIND_MANAGED_FIELD(NumMetaData)>("NumMetaData","NumMetaData");
		addField<DAS_BIND_MANAGED_FIELD(MetaDataArray)>("MetaDataArray","MetaDataArray");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3530:9
struct UECodeGen_Private_FMulticastDelegatePropertyParams_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FMulticastDelegatePropertyParams> {
	UECodeGen_Private_FMulticastDelegatePropertyParams_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FMulticastDelegatePropertyParams", ml, "UECodeGen_Private::FMulticastDelegatePropertyParams") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(RepNotifyFuncUTF8)>("RepNotifyFuncUTF8","RepNotifyFuncUTF8");
		addField<DAS_BIND_MANAGED_FIELD(PropertyFlags)>("PropertyFlags","PropertyFlags");
		addField<DAS_BIND_MANAGED_FIELD(Flags)>("Flags","Flags");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(ArrayDim)>("ArrayDim","ArrayDim");
		addField<DAS_BIND_MANAGED_FIELD(Offset)>("Offset","Offset");
		addField<DAS_BIND_MANAGED_FIELD(NumMetaData)>("NumMetaData","NumMetaData");
		addField<DAS_BIND_MANAGED_FIELD(MetaDataArray)>("MetaDataArray","MetaDataArray");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3548:9
struct UECodeGen_Private_FEnumPropertyParams_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FEnumPropertyParams> {
	UECodeGen_Private_FEnumPropertyParams_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FEnumPropertyParams", ml, "UECodeGen_Private::FEnumPropertyParams") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(RepNotifyFuncUTF8)>("RepNotifyFuncUTF8","RepNotifyFuncUTF8");
		addField<DAS_BIND_MANAGED_FIELD(PropertyFlags)>("PropertyFlags","PropertyFlags");
		addField<DAS_BIND_MANAGED_FIELD(Flags)>("Flags","Flags");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(ArrayDim)>("ArrayDim","ArrayDim");
		addField<DAS_BIND_MANAGED_FIELD(Offset)>("Offset","Offset");
		addField<DAS_BIND_MANAGED_FIELD(NumMetaData)>("NumMetaData","NumMetaData");
		addField<DAS_BIND_MANAGED_FIELD(MetaDataArray)>("MetaDataArray","MetaDataArray");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3566:9
struct UECodeGen_Private_FFieldPathPropertyParams_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FFieldPathPropertyParams> {
	UECodeGen_Private_FFieldPathPropertyParams_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FFieldPathPropertyParams", ml, "UECodeGen_Private::FFieldPathPropertyParams") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(RepNotifyFuncUTF8)>("RepNotifyFuncUTF8","RepNotifyFuncUTF8");
		addField<DAS_BIND_MANAGED_FIELD(PropertyFlags)>("PropertyFlags","PropertyFlags");
		addField<DAS_BIND_MANAGED_FIELD(Flags)>("Flags","Flags");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(ArrayDim)>("ArrayDim","ArrayDim");
		addField<DAS_BIND_MANAGED_FIELD(Offset)>("Offset","Offset");
		addField<DAS_BIND_MANAGED_FIELD(NumMetaData)>("NumMetaData","NumMetaData");
		addField<DAS_BIND_MANAGED_FIELD(MetaDataArray)>("MetaDataArray","MetaDataArray");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3584:9
struct UECodeGen_Private_FArrayPropertyParams_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FArrayPropertyParams> {
	UECodeGen_Private_FArrayPropertyParams_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FArrayPropertyParams", ml, "UECodeGen_Private::FArrayPropertyParams") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(RepNotifyFuncUTF8)>("RepNotifyFuncUTF8","RepNotifyFuncUTF8");
		addField<DAS_BIND_MANAGED_FIELD(PropertyFlags)>("PropertyFlags","PropertyFlags");
		addField<DAS_BIND_MANAGED_FIELD(Flags)>("Flags","Flags");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(ArrayDim)>("ArrayDim","ArrayDim");
		addField<DAS_BIND_MANAGED_FIELD(Offset)>("Offset","Offset");
		addField<DAS_BIND_MANAGED_FIELD(ArrayFlags)>("ArrayFlags","ArrayFlags");
		addField<DAS_BIND_MANAGED_FIELD(NumMetaData)>("NumMetaData","NumMetaData");
		addField<DAS_BIND_MANAGED_FIELD(MetaDataArray)>("MetaDataArray","MetaDataArray");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3602:9
struct UECodeGen_Private_FMapPropertyParams_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FMapPropertyParams> {
	UECodeGen_Private_FMapPropertyParams_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FMapPropertyParams", ml, "UECodeGen_Private::FMapPropertyParams") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(RepNotifyFuncUTF8)>("RepNotifyFuncUTF8","RepNotifyFuncUTF8");
		addField<DAS_BIND_MANAGED_FIELD(PropertyFlags)>("PropertyFlags","PropertyFlags");
		addField<DAS_BIND_MANAGED_FIELD(Flags)>("Flags","Flags");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(ArrayDim)>("ArrayDim","ArrayDim");
		addField<DAS_BIND_MANAGED_FIELD(Offset)>("Offset","Offset");
		addField<DAS_BIND_MANAGED_FIELD(MapFlags)>("MapFlags","MapFlags");
		addField<DAS_BIND_MANAGED_FIELD(NumMetaData)>("NumMetaData","NumMetaData");
		addField<DAS_BIND_MANAGED_FIELD(MetaDataArray)>("MetaDataArray","MetaDataArray");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3641:9
struct UECodeGen_Private_FFunctionParams_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FFunctionParams> {
	UECodeGen_Private_FFunctionParams_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FFunctionParams", ml, "UECodeGen_Private::FFunctionParams") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(OwningClassName)>("OwningClassName","OwningClassName");
		addField<DAS_BIND_MANAGED_FIELD(DelegateName)>("DelegateName","DelegateName");
		addField<DAS_BIND_MANAGED_FIELD(PropertyArray)>("PropertyArray","PropertyArray");
		addField<DAS_BIND_MANAGED_FIELD(NumProperties)>("NumProperties","NumProperties");
		addField<DAS_BIND_MANAGED_FIELD(StructureSize)>("StructureSize","StructureSize");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(RPCId)>("RPCId","RPCId");
		addField<DAS_BIND_MANAGED_FIELD(RPCResponseId)>("RPCResponseId","RPCResponseId");
		addField<DAS_BIND_MANAGED_FIELD(NumMetaData)>("NumMetaData","NumMetaData");
		addField<DAS_BIND_MANAGED_FIELD(MetaDataArray)>("MetaDataArray","MetaDataArray");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3661:9
struct UECodeGen_Private_FEnumParams_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FEnumParams> {
	UECodeGen_Private_FEnumParams_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FEnumParams", ml, "UECodeGen_Private::FEnumParams") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(CppTypeUTF8)>("CppTypeUTF8","CppTypeUTF8");
		addField<DAS_BIND_MANAGED_FIELD(EnumeratorParams)>("EnumeratorParams","EnumeratorParams");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(NumEnumerators)>("NumEnumerators","NumEnumerators");
		addField<DAS_BIND_MANAGED_FIELD(EnumFlags)>("EnumFlags","EnumFlags");
		addField<DAS_BIND_MANAGED_FIELD(CppForm)>("CppForm","CppForm");
		addField<DAS_BIND_MANAGED_FIELD(NumMetaData)>("NumMetaData","NumMetaData");
		addField<DAS_BIND_MANAGED_FIELD(MetaDataArray)>("MetaDataArray","MetaDataArray");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3678:9
struct UECodeGen_Private_FStructParams_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FStructParams> {
	UECodeGen_Private_FStructParams_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FStructParams", ml, "UECodeGen_Private::FStructParams") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(PropertyArray)>("PropertyArray","PropertyArray");
		addField<DAS_BIND_MANAGED_FIELD(NumProperties)>("NumProperties","NumProperties");
		addField<DAS_BIND_MANAGED_FIELD(SizeOf)>("SizeOf","SizeOf");
		addField<DAS_BIND_MANAGED_FIELD(AlignOf)>("AlignOf","AlignOf");
		addField<DAS_BIND_MANAGED_FIELD(ObjectFlags)>("ObjectFlags","ObjectFlags");
		addField<DAS_BIND_MANAGED_FIELD(StructFlags)>("StructFlags","StructFlags");
		addField<DAS_BIND_MANAGED_FIELD(NumMetaData)>("NumMetaData","NumMetaData");
		addField<DAS_BIND_MANAGED_FIELD(MetaDataArray)>("MetaDataArray","MetaDataArray");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3696:9
struct UECodeGen_Private_FPackageParams_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FPackageParams> {
	UECodeGen_Private_FPackageParams_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FPackageParams", ml, "UECodeGen_Private::FPackageParams") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(NameUTF8)>("NameUTF8","NameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(NumSingletons)>("NumSingletons","NumSingletons");
		addField<DAS_BIND_MANAGED_FIELD(PackageFlags)>("PackageFlags","PackageFlags");
		addField<DAS_BIND_MANAGED_FIELD(BodyCRC)>("BodyCRC","BodyCRC");
		addField<DAS_BIND_MANAGED_FIELD(DeclarationsCRC)>("DeclarationsCRC","DeclarationsCRC");
		addField<DAS_BIND_MANAGED_FIELD(NumMetaData)>("NumMetaData","NumMetaData");
		addField<DAS_BIND_MANAGED_FIELD(MetaDataArray)>("MetaDataArray","MetaDataArray");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3710:9
struct UECodeGen_Private_FImplementedInterfaceParams_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FImplementedInterfaceParams> {
	UECodeGen_Private_FImplementedInterfaceParams_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FImplementedInterfaceParams", ml, "UECodeGen_Private::FImplementedInterfaceParams") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(Offset)>("Offset","Offset");
		addField<DAS_BIND_MANAGED_FIELD(bImplementedByK2)>("bImplementedByK2","bImplementedByK2");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3717:9
struct UECodeGen_Private_FClassParams_GeneratedAnnotation : ManagedStructureAnnotation<UECodeGen_Private::FClassParams> {
	UECodeGen_Private_FClassParams_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FClassParams", ml, "UECodeGen_Private::FClassParams") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(ClassConfigNameUTF8)>("ClassConfigNameUTF8","ClassConfigNameUTF8");
		addField<DAS_BIND_MANAGED_FIELD(CppClassInfo)>("CppClassInfo","CppClassInfo");
		addField<DAS_BIND_MANAGED_FIELD(FunctionLinkArray)>("FunctionLinkArray","FunctionLinkArray");
		addField<DAS_BIND_MANAGED_FIELD(PropertyArray)>("PropertyArray","PropertyArray");
		addField<DAS_BIND_MANAGED_FIELD(ImplementedInterfaceArray)>("ImplementedInterfaceArray","ImplementedInterfaceArray");
		addField<DAS_BIND_MANAGED_FIELD(ClassFlags)>("ClassFlags","ClassFlags");
		addField<DAS_BIND_MANAGED_FIELD(NumMetaData)>("NumMetaData","NumMetaData");
		addField<DAS_BIND_MANAGED_FIELD(MetaDataArray)>("MetaDataArray","MetaDataArray");
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

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:904:10
struct UScriptStruct_ICppStructOps_FCapabilities_GeneratedAnnotation : ManagedStructureAnnotation<UScriptStruct::ICppStructOps::FCapabilities> {
	UScriptStruct_ICppStructOps_FCapabilities_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FCapabilities", ml, "UScriptStruct::ICppStructOps::FCapabilities") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(ComputedPropertyFlags)>("ComputedPropertyFlags","ComputedPropertyFlags");
		addField<DAS_BIND_MANAGED_FIELD(HasSerializerObjectReferences)>("HasSerializerObjectReferences","HasSerializerObjectReferences");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:901:9
struct UScriptStruct_ICppStructOps_GeneratedAnnotation : ManagedStructureAnnotation<UScriptStruct::ICppStructOps> {
	UScriptStruct_ICppStructOps_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("ICppStructOps", ml, "UScriptStruct::ICppStructOps") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:897:7
struct UScriptStruct_GeneratedAnnotation : ManagedStructureAnnotation<UScriptStruct, false, false> {
	UScriptStruct_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("UScriptStruct", ml, "UScriptStruct") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(StructFlags)>("StructFlags","StructFlags");
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

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:86:8
struct FSavePackageResultStruct_GeneratedAnnotation : ManagedStructureAnnotation<FSavePackageResultStruct> {
	virtual bool canCopy() const override { return std::is_copy_constructible<FSavePackageResultStruct>::value; }
	virtual bool canClone() const override { return std::is_copy_constructible<FSavePackageResultStruct>::value; }
	virtual bool canMove() const override { return std::is_copy_constructible<FSavePackageResultStruct>::value; }
	FSavePackageResultStruct_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FSavePackageResultStruct", ml, "FSavePackageResultStruct") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(Result)>("Result","Result");
		addField<DAS_BIND_MANAGED_FIELD(TotalFileSize)>("TotalFileSize","TotalFileSize");
		addField<DAS_BIND_MANAGED_FIELD(SerializedPackageFlags)>("SerializedPackageFlags","SerializedPackageFlags");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:142:23
struct UPackage_GeneratedAnnotation : ManagedStructureAnnotation<UPackage, false, false> {
	UPackage_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("UPackage", ml, "UPackage") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(LinkerLicenseeVersion)>("LinkerLicenseeVersion","LinkerLicenseeVersion");
		addField<DAS_BIND_MANAGED_FIELD(FileSize)>("FileSize","FileSize");
		addField<DAS_BIND_MANAGED_FIELD(MetaData)>("MetaData","MetaData");
		addField<DAS_BIND_MANAGED_FIELD(PIEInstanceID)>("PIEInstanceID","PIEInstanceID");
		addField<DAS_BIND_MANAGED_FIELD(FileName)>("FileName","FileName");
	}
};

