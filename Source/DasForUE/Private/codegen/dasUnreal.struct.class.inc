// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/EngineVersionBase.h:28:7
struct FEngineVersionBase_GeneratedAnnotation : ManagedStructureAnnotation<FEngineVersionBase> {
	FEngineVersionBase_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FEngineVersionBase", ml, "FEngineVersionBase") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:129:7
struct FOutputDevice_GeneratedAnnotation : ManagedStructureAnnotation<FOutputDevice> {
	FOutputDevice_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FOutputDevice", ml, "FOutputDevice") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:737:9
struct FArchiveState_FFastPathLoadBuffer_GeneratedAnnotation : ManagedStructureAnnotation<FArchiveState::FFastPathLoadBuffer> {
	FArchiveState_FFastPathLoadBuffer_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FFastPathLoadBuffer", ml, "FArchiveState::FFastPathLoadBuffer") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(StartFastPathLoadBuffer)>("StartFastPathLoadBuffer","StartFastPathLoadBuffer");
		addField<DAS_BIND_MANAGED_FIELD(EndFastPathLoadBuffer)>("EndFastPathLoadBuffer","EndFastPathLoadBuffer");
		addField<DAS_BIND_MANAGED_FIELD(OriginalFastPathLoadBuffer)>("OriginalFastPathLoadBuffer","OriginalFastPathLoadBuffer");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:65:8
struct FArchiveState_GeneratedAnnotation : ManagedStructureAnnotation<FArchiveState> {
	FArchiveState_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FArchiveState", ml, "FArchiveState") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(ArSerializingDefaults)>("ArSerializingDefaults","ArSerializingDefaults");
		addField<DAS_BIND_MANAGED_FIELD(ArPortFlags)>("ArPortFlags","ArPortFlags");
		addField<DAS_BIND_MANAGED_FIELD(ArMaxSerializeSize)>("ArMaxSerializeSize","ArMaxSerializeSize");
		addField<DAS_BIND_MANAGED_FIELD(ArCustomPropertyList)>("ArCustomPropertyList","ArCustomPropertyList");
		addField<DAS_BIND_MANAGED_FIELD(ArDebugSerializationFlags)>("ArDebugSerializationFlags","ArDebugSerializationFlags");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:2113:8
struct FArchive_FScopeSetDebugSerializationFlags_GeneratedAnnotation : ManagedStructureAnnotation<FArchive::FScopeSetDebugSerializationFlags> {
	FArchive_FScopeSetDebugSerializationFlags_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FScopeSetDebugSerializationFlags", ml, "FArchive::FScopeSetDebugSerializationFlags") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:2164:8
struct FArchive_FScopeAddDebugData_GeneratedAnnotation : ManagedStructureAnnotation<FArchive::FScopeAddDebugData> {
	FArchive_FScopeAddDebugData_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FScopeAddDebugData", ml, "FArchive::FScopeAddDebugData") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1134:7
struct FArchive_GeneratedAnnotation : ManagedStructureAnnotation<FArchive> {
	FArchive_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FArchive", ml, "FArchive") {
	}
	void init () {
	}
};

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

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:22:8
struct FPrimaryAssetType_GeneratedAnnotation : ManagedStructureAnnotation<FPrimaryAssetType> {
	FPrimaryAssetType_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FPrimaryAssetType", ml, "FPrimaryAssetType") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:104:8
struct FPrimaryAssetId_GeneratedAnnotation : ManagedStructureAnnotation<FPrimaryAssetId> {
	FPrimaryAssetId_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FPrimaryAssetId", ml, "FPrimaryAssetId") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(PrimaryAssetType)>("PrimaryAssetType","PrimaryAssetType");
		addField<DAS_BIND_MANAGED_FIELD(PrimaryAssetName)>("PrimaryAssetName","PrimaryAssetName");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:24:1
struct FStatGroup_STATGROUP_Script_GeneratedAnnotation : ManagedStructureAnnotation<FStatGroup_STATGROUP_Script> {
	FStatGroup_STATGROUP_Script_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FStatGroup_STATGROUP_Script", ml, "FStatGroup_STATGROUP_Script") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:380:8
struct FBlueprintExceptionInfo_GeneratedAnnotation : ManagedStructureAnnotation<FBlueprintExceptionInfo> {
	FBlueprintExceptionInfo_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FBlueprintExceptionInfo", ml, "FBlueprintExceptionInfo") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:409:8
struct FScriptInstrumentationSignal_GeneratedAnnotation : ManagedStructureAnnotation<FScriptInstrumentationSignal> {
	FScriptInstrumentationSignal_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FScriptInstrumentationSignal", ml, "FScriptInstrumentationSignal") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:474:7
struct FBlueprintCoreDelegates_GeneratedAnnotation : ManagedStructureAnnotation<FBlueprintCoreDelegates> {
	FBlueprintCoreDelegates_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FBlueprintCoreDelegates", ml, "FBlueprintCoreDelegates") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:504:24
struct FBlueprintContextTracker_GeneratedAnnotation : ManagedStructureAnnotation<FBlueprintContextTracker> {
	FBlueprintContextTracker_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FBlueprintContextTracker", ml, "FBlueprintContextTracker") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:594:8
struct FEditorScriptExecutionGuard_GeneratedAnnotation : ManagedStructureAnnotation<FEditorScriptExecutionGuard> {
	FEditorScriptExecutionGuard_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FEditorScriptExecutionGuard", ml, "FEditorScriptExecutionGuard") {
	}
	void init () {
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

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:62:7
struct FFieldClass_GeneratedAnnotation : ManagedStructureAnnotation<FFieldClass> {
	FFieldClass_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FFieldClass", ml, "FFieldClass") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:264:7
struct FFieldVariant_GeneratedAnnotation : ManagedStructureAnnotation<FFieldVariant> {
	FFieldVariant_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FFieldVariant", ml, "FFieldVariant") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:412:7
struct FField_GeneratedAnnotation : ManagedStructureAnnotation<FField> {
	FField_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FField", ml, "FField") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(Owner)>("Owner","Owner");
		addField<DAS_BIND_MANAGED_FIELD(Next)>("Next","Next");
		addField<DAS_BIND_MANAGED_FIELD(NamePrivate)>("NamePrivate","NamePrivate");
		addField<DAS_BIND_MANAGED_FIELD(FlagsPrivate)>("FlagsPrivate","FlagsPrivate");
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
		addField<DAS_BIND_MANAGED_FIELD(ChildProperties)>("ChildProperties","ChildProperties");
		addField<DAS_BIND_MANAGED_FIELD(PropertiesSize)>("PropertiesSize","PropertiesSize");
		addField<DAS_BIND_MANAGED_FIELD(MinAlignment)>("MinAlignment","MinAlignment");
		addField<DAS_BIND_MANAGED_FIELD(PropertyLink)>("PropertyLink","PropertyLink");
		addField<DAS_BIND_MANAGED_FIELD(RefLink)>("RefLink","RefLink");
		addField<DAS_BIND_MANAGED_FIELD(DestructorLink)>("DestructorLink","DestructorLink");
		addField<DAS_BIND_MANAGED_FIELD(PostConstructLink)>("PostConstructLink","PostConstructLink");
		addField<DAS_BIND_MANAGED_FIELD(FieldPathSerialNumber)>("FieldPathSerialNumber","FieldPathSerialNumber");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:904:10
struct UScriptStruct_ICppStructOps_FCapabilities_GeneratedAnnotation : ManagedStructureAnnotation<UScriptStruct::ICppStructOps::FCapabilities> {
	UScriptStruct_ICppStructOps_FCapabilities_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FCapabilities", ml, "UScriptStruct::ICppStructOps::FCapabilities") {
	}
	void init () {
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

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1780:7
struct UFunction_GeneratedAnnotation : ManagedStructureAnnotation<UFunction, false, false> {
	UFunction_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("UFunction", ml, "UFunction") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(FunctionFlags)>("FunctionFlags","FunctionFlags");
		addField<DAS_BIND_MANAGED_FIELD(NumParms)>("NumParms","NumParms");
		addField<DAS_BIND_MANAGED_FIELD(ParmsSize)>("ParmsSize","ParmsSize");
		addField<DAS_BIND_MANAGED_FIELD(ReturnValueOffset)>("ReturnValueOffset","ReturnValueOffset");
		addField<DAS_BIND_MANAGED_FIELD(RPCId)>("RPCId","RPCId");
		addField<DAS_BIND_MANAGED_FIELD(RPCResponseId)>("RPCResponseId","RPCResponseId");
		addField<DAS_BIND_MANAGED_FIELD(FirstPropertyToInit)>("FirstPropertyToInit","FirstPropertyToInit");
		addField<DAS_BIND_MANAGED_FIELD(EventGraphFunction)>("EventGraphFunction","EventGraphFunction");
		addField<DAS_BIND_MANAGED_FIELD(EventGraphCallOffset)>("EventGraphCallOffset","EventGraphCallOffset");
		addField<DAS_BIND_MANAGED_FIELD(SingletonPtr)>("SingletonPtr","SingletonPtr");
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
		addField<DAS_BIND_MANAGED_FIELD(PropertiesPendingDestruction)>("PropertiesPendingDestruction","PropertiesPendingDestruction");
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

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:153:7
struct FProperty_GeneratedAnnotation : ManagedStructureAnnotation<FProperty> {
	FProperty_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FProperty", ml, "FProperty") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(ArrayDim)>("ArrayDim","ArrayDim");
		addField<DAS_BIND_MANAGED_FIELD(ElementSize)>("ElementSize","ElementSize");
		addField<DAS_BIND_MANAGED_FIELD(RepIndex)>("RepIndex","RepIndex");
		addField<DAS_BIND_MANAGED_FIELD(PropertyLinkNext)>("PropertyLinkNext","PropertyLinkNext");
		addField<DAS_BIND_MANAGED_FIELD(NextRef)>("NextRef","NextRef");
		addField<DAS_BIND_MANAGED_FIELD(DestructorLinkNext)>("DestructorLinkNext","DestructorLinkNext");
		addField<DAS_BIND_MANAGED_FIELD(PostConstructLinkNext)>("PostConstructLinkNext","PostConstructLinkNext");
		addField<DAS_BIND_MANAGED_FIELD(RepNotifyFunc)>("RepNotifyFunc","RepNotifyFunc");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1139:7
struct FPropertyHelpers_GeneratedAnnotation : ManagedStructureAnnotation<FPropertyHelpers> {
	FPropertyHelpers_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FPropertyHelpers", ml, "FPropertyHelpers") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1151:8
struct UEProperty_Private_FProperty_DoNotUse_GeneratedAnnotation : ManagedStructureAnnotation<UEProperty_Private::FProperty_DoNotUse> {
	UEProperty_Private_FProperty_DoNotUse_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FProperty_DoNotUse", ml, "UEProperty_Private::FProperty_DoNotUse") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1170:8
struct FDefinedProperty_GeneratedAnnotation : ManagedStructureAnnotation<FDefinedProperty> {
	FDefinedProperty_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FDefinedProperty", ml, "FDefinedProperty") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(Property)>("Property","Property");
		addField<DAS_BIND_MANAGED_FIELD(Index)>("Index","Index");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1486:23
struct FNumericProperty_GeneratedAnnotation : ManagedStructureAnnotation<FNumericProperty> {
	FNumericProperty_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FNumericProperty", ml, "FNumericProperty") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1900:23
struct FByteProperty_GeneratedAnnotation : ManagedStructureAnnotation<FByteProperty> {
	virtual bool canCopy() const override { return std::is_copy_constructible<FByteProperty>::value; }
	virtual bool canClone() const override { return std::is_copy_constructible<FByteProperty>::value; }
	virtual bool canMove() const override { return std::is_copy_constructible<FByteProperty>::value; }
	FByteProperty_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FByteProperty", ml, "FByteProperty") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1974:23
struct FInt8Property_GeneratedAnnotation : ManagedStructureAnnotation<FInt8Property> {
	virtual bool canCopy() const override { return std::is_copy_constructible<FInt8Property>::value; }
	virtual bool canClone() const override { return std::is_copy_constructible<FInt8Property>::value; }
	virtual bool canMove() const override { return std::is_copy_constructible<FInt8Property>::value; }
	FInt8Property_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FInt8Property", ml, "FInt8Property") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2013:23
struct FInt16Property_GeneratedAnnotation : ManagedStructureAnnotation<FInt16Property> {
	virtual bool canCopy() const override { return std::is_copy_constructible<FInt16Property>::value; }
	virtual bool canClone() const override { return std::is_copy_constructible<FInt16Property>::value; }
	virtual bool canMove() const override { return std::is_copy_constructible<FInt16Property>::value; }
	FInt16Property_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FInt16Property", ml, "FInt16Property") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2053:23
struct FIntProperty_GeneratedAnnotation : ManagedStructureAnnotation<FIntProperty> {
	virtual bool canCopy() const override { return std::is_copy_constructible<FIntProperty>::value; }
	virtual bool canClone() const override { return std::is_copy_constructible<FIntProperty>::value; }
	virtual bool canMove() const override { return std::is_copy_constructible<FIntProperty>::value; }
	FIntProperty_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FIntProperty", ml, "FIntProperty") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2092:23
struct FInt64Property_GeneratedAnnotation : ManagedStructureAnnotation<FInt64Property> {
	virtual bool canCopy() const override { return std::is_copy_constructible<FInt64Property>::value; }
	virtual bool canClone() const override { return std::is_copy_constructible<FInt64Property>::value; }
	virtual bool canMove() const override { return std::is_copy_constructible<FInt64Property>::value; }
	FInt64Property_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FInt64Property", ml, "FInt64Property") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2131:23
struct FUInt16Property_GeneratedAnnotation : ManagedStructureAnnotation<FUInt16Property> {
	virtual bool canCopy() const override { return std::is_copy_constructible<FUInt16Property>::value; }
	virtual bool canClone() const override { return std::is_copy_constructible<FUInt16Property>::value; }
	virtual bool canMove() const override { return std::is_copy_constructible<FUInt16Property>::value; }
	FUInt16Property_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FUInt16Property", ml, "FUInt16Property") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2170:23
struct FUInt32Property_GeneratedAnnotation : ManagedStructureAnnotation<FUInt32Property> {
	virtual bool canCopy() const override { return std::is_copy_constructible<FUInt32Property>::value; }
	virtual bool canClone() const override { return std::is_copy_constructible<FUInt32Property>::value; }
	virtual bool canMove() const override { return std::is_copy_constructible<FUInt32Property>::value; }
	FUInt32Property_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FUInt32Property", ml, "FUInt32Property") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2209:23
struct FUInt64Property_GeneratedAnnotation : ManagedStructureAnnotation<FUInt64Property> {
	virtual bool canCopy() const override { return std::is_copy_constructible<FUInt64Property>::value; }
	virtual bool canClone() const override { return std::is_copy_constructible<FUInt64Property>::value; }
	virtual bool canMove() const override { return std::is_copy_constructible<FUInt64Property>::value; }
	FUInt64Property_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FUInt64Property", ml, "FUInt64Property") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2249:23
struct FFloatProperty_GeneratedAnnotation : ManagedStructureAnnotation<FFloatProperty> {
	virtual bool canCopy() const override { return std::is_copy_constructible<FFloatProperty>::value; }
	virtual bool canClone() const override { return std::is_copy_constructible<FFloatProperty>::value; }
	virtual bool canMove() const override { return std::is_copy_constructible<FFloatProperty>::value; }
	FFloatProperty_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FFloatProperty", ml, "FFloatProperty") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2288:23
struct FDoubleProperty_GeneratedAnnotation : ManagedStructureAnnotation<FDoubleProperty> {
	virtual bool canCopy() const override { return std::is_copy_constructible<FDoubleProperty>::value; }
	virtual bool canClone() const override { return std::is_copy_constructible<FDoubleProperty>::value; }
	virtual bool canMove() const override { return std::is_copy_constructible<FDoubleProperty>::value; }
	FDoubleProperty_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FDoubleProperty", ml, "FDoubleProperty") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2338:23
struct FBoolProperty_GeneratedAnnotation : ManagedStructureAnnotation<FBoolProperty> {
	FBoolProperty_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FBoolProperty", ml, "FBoolProperty") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2491:23
struct FObjectPropertyBase_GeneratedAnnotation : ManagedStructureAnnotation<FObjectPropertyBase> {
	FObjectPropertyBase_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FObjectPropertyBase", ml, "FObjectPropertyBase") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3564:23
struct FStrProperty_GeneratedAnnotation : ManagedStructureAnnotation<FStrProperty> {
	FStrProperty_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FStrProperty", ml, "FStrProperty") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5788:23
struct FStructProperty_GeneratedAnnotation : ManagedStructureAnnotation<FStructProperty> {
	FStructProperty_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FStructProperty", ml, "FStructProperty") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:6181:24
struct FCustomPropertyListNode_GeneratedAnnotation : ManagedStructureAnnotation<FCustomPropertyListNode> {
	FCustomPropertyListNode_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FCustomPropertyListNode", ml, "FCustomPropertyListNode") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(Property)>("Property","Property");
		addField<DAS_BIND_MANAGED_FIELD(ArrayIndex)>("ArrayIndex","ArrayIndex");
		addField<DAS_BIND_MANAGED_FIELD(SubPropertyList)>("SubPropertyList","SubPropertyList");
		addField<DAS_BIND_MANAGED_FIELD(PropertyListNext)>("PropertyListNext","PropertyListNext");
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:7164:23
struct UPropertyWrapper_GeneratedAnnotation : ManagedStructureAnnotation<UPropertyWrapper, false, false> {
	UPropertyWrapper_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("UPropertyWrapper", ml, "UPropertyWrapper") {
	}
	void init () {
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:136:8
struct FFrame_GeneratedAnnotation : ManagedStructureAnnotation<FFrame> {
	FFrame_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation ("FFrame", ml, "FFrame") {
	}
	void init () {
		addField<DAS_BIND_MANAGED_FIELD(Node)>("Node","Node");
		addField<DAS_BIND_MANAGED_FIELD(Object)>("Object","Object");
		addField<DAS_BIND_MANAGED_FIELD(Code)>("Code","Code");
		addField<DAS_BIND_MANAGED_FIELD(Locals)>("Locals","Locals");
		addField<DAS_BIND_MANAGED_FIELD(MostRecentProperty)>("MostRecentProperty","MostRecentProperty");
		addField<DAS_BIND_MANAGED_FIELD(MostRecentPropertyAddress)>("MostRecentPropertyAddress","MostRecentPropertyAddress");
		addField<DAS_BIND_MANAGED_FIELD(MostRecentPropertyContainer)>("MostRecentPropertyContainer","MostRecentPropertyContainer");
		addField<DAS_BIND_MANAGED_FIELD(PreviousFrame)>("PreviousFrame","PreviousFrame");
		addField<DAS_BIND_MANAGED_FIELD(PropertyChainForCompiledIn)>("PropertyChainForCompiledIn","PropertyChainForCompiledIn");
		addField<DAS_BIND_MANAGED_FIELD(CurrentNativeFunction)>("CurrentNativeFunction","CurrentNativeFunction");
		addField<DAS_BIND_MANAGED_FIELD(PreviousTrackingFrame)>("PreviousTrackingFrame","PreviousTrackingFrame");
		addField<DAS_BIND_MANAGED_FIELD(bArrayContextFailed)>("bArrayContextFailed","bArrayContextFailed");
		addField<DAS_BIND_MANAGED_FIELD(bAbortingExecution)>("bAbortingExecution","bAbortingExecution");
		addField<DAS_BIND_MANAGED_FIELD(DepthCounter)>("DepthCounter","DepthCounter");
	}
};

