// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:56:6
class Enumeration_ELoadFlags : public das::Enumeration {
public:
	Enumeration_ELoadFlags() : das::Enumeration("ELoadFlags") {
		external = true;
		cppName = "ELoadFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("LOAD_None", "LOAD_None", int64_t(ELoadFlags::LOAD_None), das::LineInfo());
		addIEx("LOAD_Async", "LOAD_Async", int64_t(ELoadFlags::LOAD_Async), das::LineInfo());
		addIEx("LOAD_NoWarn", "LOAD_NoWarn", int64_t(ELoadFlags::LOAD_NoWarn), das::LineInfo());
		addIEx("LOAD_EditorOnly", "LOAD_EditorOnly", int64_t(ELoadFlags::LOAD_EditorOnly), das::LineInfo());
		addIEx("LOAD_ResolvingDeferredExports", "LOAD_ResolvingDeferredExports", int64_t(ELoadFlags::LOAD_ResolvingDeferredExports), das::LineInfo());
		addIEx("LOAD_Verify", "LOAD_Verify", int64_t(ELoadFlags::LOAD_Verify), das::LineInfo());
		addIEx("LOAD_NoVerify", "LOAD_NoVerify", int64_t(ELoadFlags::LOAD_NoVerify), das::LineInfo());
		addIEx("LOAD_IsVerifying", "LOAD_IsVerifying", int64_t(ELoadFlags::LOAD_IsVerifying), das::LineInfo());
		addIEx("LOAD_SkipLoadImportedPackages", "LOAD_SkipLoadImportedPackages", int64_t(ELoadFlags::LOAD_SkipLoadImportedPackages), das::LineInfo());
		addIEx("LOAD_RegenerateBulkDataGuids", "LOAD_RegenerateBulkDataGuids", int64_t(ELoadFlags::LOAD_RegenerateBulkDataGuids), das::LineInfo());
		addIEx("LOAD_DisableDependencyPreloading", "LOAD_DisableDependencyPreloading", int64_t(ELoadFlags::LOAD_DisableDependencyPreloading), das::LineInfo());
		addIEx("LOAD_Quiet", "LOAD_Quiet", int64_t(ELoadFlags::LOAD_Quiet), das::LineInfo());
		addIEx("LOAD_FindIfFail", "LOAD_FindIfFail", int64_t(ELoadFlags::LOAD_FindIfFail), das::LineInfo());
		addIEx("LOAD_MemoryReader", "LOAD_MemoryReader", int64_t(ELoadFlags::LOAD_MemoryReader), das::LineInfo());
		addIEx("LOAD_NoRedirects", "LOAD_NoRedirects", int64_t(ELoadFlags::LOAD_NoRedirects), das::LineInfo());
		addIEx("LOAD_ForDiff", "LOAD_ForDiff", int64_t(ELoadFlags::LOAD_ForDiff), das::LineInfo());
		addIEx("LOAD_PackageForPIE", "LOAD_PackageForPIE", int64_t(ELoadFlags::LOAD_PackageForPIE), das::LineInfo());
		addIEx("LOAD_DeferDependencyLoads", "LOAD_DeferDependencyLoads", int64_t(ELoadFlags::LOAD_DeferDependencyLoads), das::LineInfo());
		addIEx("LOAD_ForFileDiff", "LOAD_ForFileDiff", int64_t(ELoadFlags::LOAD_ForFileDiff), das::LineInfo());
		addIEx("LOAD_DisableCompileOnLoad", "LOAD_DisableCompileOnLoad", int64_t(ELoadFlags::LOAD_DisableCompileOnLoad), das::LineInfo());
		addIEx("LOAD_DisableEngineVersionChecks", "LOAD_DisableEngineVersionChecks", int64_t(ELoadFlags::LOAD_DisableEngineVersionChecks), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:85:6
class Enumeration_ESaveFlags : public das::Enumeration {
public:
	Enumeration_ESaveFlags() : das::Enumeration("ESaveFlags") {
		external = true;
		cppName = "ESaveFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("SAVE_None", "SAVE_None", int64_t(ESaveFlags::SAVE_None), das::LineInfo());
		addIEx("SAVE_NoError", "SAVE_NoError", int64_t(ESaveFlags::SAVE_NoError), das::LineInfo());
		addIEx("SAVE_FromAutosave", "SAVE_FromAutosave", int64_t(ESaveFlags::SAVE_FromAutosave), das::LineInfo());
		addIEx("SAVE_KeepDirty", "SAVE_KeepDirty", int64_t(ESaveFlags::SAVE_KeepDirty), das::LineInfo());
		addIEx("SAVE_KeepGUID", "SAVE_KeepGUID", int64_t(ESaveFlags::SAVE_KeepGUID), das::LineInfo());
		addIEx("SAVE_Async", "SAVE_Async", int64_t(ESaveFlags::SAVE_Async), das::LineInfo());
		addIEx("SAVE_Unversioned_Native", "SAVE_Unversioned_Native", int64_t(ESaveFlags::SAVE_Unversioned_Native), das::LineInfo());
		addIEx("SAVE_CutdownPackage", "SAVE_CutdownPackage", int64_t(ESaveFlags::SAVE_CutdownPackage), das::LineInfo());
		addIEx("SAVE_KeepEditorOnlyCookedPackages", "SAVE_KeepEditorOnlyCookedPackages", int64_t(ESaveFlags::SAVE_KeepEditorOnlyCookedPackages), das::LineInfo());
		addIEx("SAVE_Concurrent", "SAVE_Concurrent", int64_t(ESaveFlags::SAVE_Concurrent), das::LineInfo());
		addIEx("SAVE_DiffOnly", "SAVE_DiffOnly", int64_t(ESaveFlags::SAVE_DiffOnly), das::LineInfo());
		addIEx("SAVE_DiffCallstack", "SAVE_DiffCallstack", int64_t(ESaveFlags::SAVE_DiffCallstack), das::LineInfo());
		addIEx("SAVE_ComputeHash", "SAVE_ComputeHash", int64_t(ESaveFlags::SAVE_ComputeHash), das::LineInfo());
		addIEx("SAVE_CompareLinker", "SAVE_CompareLinker", int64_t(ESaveFlags::SAVE_CompareLinker), das::LineInfo());
		addIEx("SAVE_BulkDataByReference", "SAVE_BulkDataByReference", int64_t(ESaveFlags::SAVE_BulkDataByReference), das::LineInfo());
		addIEx("SAVE_Unversioned_Properties", "SAVE_Unversioned_Properties", int64_t(ESaveFlags::SAVE_Unversioned_Properties), das::LineInfo());
		addIEx("SAVE_Optional", "SAVE_Optional", int64_t(ESaveFlags::SAVE_Optional), das::LineInfo());
		addIEx("SAVE_AllowTimeout", "SAVE_AllowTimeout", int64_t(ESaveFlags::SAVE_AllowTimeout), das::LineInfo());
		addIEx("SAVE_RehydratePayloads", "SAVE_RehydratePayloads", int64_t(ESaveFlags::SAVE_RehydratePayloads), das::LineInfo());
		addIEx("SAVE_Unversioned", "SAVE_Unversioned", int64_t(ESaveFlags::SAVE_Unversioned), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:116:6
class Enumeration_EPackageFlags : public das::Enumeration {
public:
	Enumeration_EPackageFlags() : das::Enumeration("EPackageFlags") {
		external = true;
		cppName = "EPackageFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("PKG_None", "PKG_None", int64_t(EPackageFlags::PKG_None), das::LineInfo());
		addIEx("PKG_NewlyCreated", "PKG_NewlyCreated", int64_t(EPackageFlags::PKG_NewlyCreated), das::LineInfo());
		addIEx("PKG_ClientOptional", "PKG_ClientOptional", int64_t(EPackageFlags::PKG_ClientOptional), das::LineInfo());
		addIEx("PKG_ServerSideOnly", "PKG_ServerSideOnly", int64_t(EPackageFlags::PKG_ServerSideOnly), das::LineInfo());
		addIEx("PKG_CompiledIn", "PKG_CompiledIn", int64_t(EPackageFlags::PKG_CompiledIn), das::LineInfo());
		addIEx("PKG_ForDiffing", "PKG_ForDiffing", int64_t(EPackageFlags::PKG_ForDiffing), das::LineInfo());
		addIEx("PKG_EditorOnly", "PKG_EditorOnly", int64_t(EPackageFlags::PKG_EditorOnly), das::LineInfo());
		addIEx("PKG_Developer", "PKG_Developer", int64_t(EPackageFlags::PKG_Developer), das::LineInfo());
		addIEx("PKG_UncookedOnly", "PKG_UncookedOnly", int64_t(EPackageFlags::PKG_UncookedOnly), das::LineInfo());
		addIEx("PKG_Cooked", "PKG_Cooked", int64_t(EPackageFlags::PKG_Cooked), das::LineInfo());
		addIEx("PKG_ContainsNoAsset", "PKG_ContainsNoAsset", int64_t(EPackageFlags::PKG_ContainsNoAsset), das::LineInfo());
		addIEx("PKG_NotExternallyReferenceable", "PKG_NotExternallyReferenceable", int64_t(EPackageFlags::PKG_NotExternallyReferenceable), das::LineInfo());
		addIEx("PKG_UnversionedProperties", "PKG_UnversionedProperties", int64_t(EPackageFlags::PKG_UnversionedProperties), das::LineInfo());
		addIEx("PKG_ContainsMapData", "PKG_ContainsMapData", int64_t(EPackageFlags::PKG_ContainsMapData), das::LineInfo());
		addIEx("PKG_IsSaving", "PKG_IsSaving", int64_t(EPackageFlags::PKG_IsSaving), das::LineInfo());
		addIEx("PKG_Compiling", "PKG_Compiling", int64_t(EPackageFlags::PKG_Compiling), das::LineInfo());
		addIEx("PKG_ContainsMap", "PKG_ContainsMap", int64_t(EPackageFlags::PKG_ContainsMap), das::LineInfo());
		addIEx("PKG_RequiresLocalizationGather", "PKG_RequiresLocalizationGather", int64_t(EPackageFlags::PKG_RequiresLocalizationGather), das::LineInfo());
		addIEx("PKG_PlayInEditor", "PKG_PlayInEditor", int64_t(EPackageFlags::PKG_PlayInEditor), das::LineInfo());
		addIEx("PKG_ContainsScript", "PKG_ContainsScript", int64_t(EPackageFlags::PKG_ContainsScript), das::LineInfo());
		addIEx("PKG_DisallowExport", "PKG_DisallowExport", int64_t(EPackageFlags::PKG_DisallowExport), das::LineInfo());
		addIEx("PKG_CookGenerated", "PKG_CookGenerated", int64_t(EPackageFlags::PKG_CookGenerated), das::LineInfo());
		addIEx("PKG_DynamicImports", "PKG_DynamicImports", int64_t(EPackageFlags::PKG_DynamicImports), das::LineInfo());
		addIEx("PKG_RuntimeGenerated", "PKG_RuntimeGenerated", int64_t(EPackageFlags::PKG_RuntimeGenerated), das::LineInfo());
		addIEx("PKG_ReloadingForCooker", "PKG_ReloadingForCooker", int64_t(EPackageFlags::PKG_ReloadingForCooker), das::LineInfo());
		addIEx("PKG_FilterEditorOnly", "PKG_FilterEditorOnly", int64_t(EPackageFlags::PKG_FilterEditorOnly), das::LineInfo());
		addIEx("PKG_TransientFlags", "PKG_TransientFlags", int64_t(EPackageFlags::PKG_TransientFlags), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:162:6
class Enumeration_EStaticConstructor : public das::Enumeration {
public:
	Enumeration_EStaticConstructor() : das::Enumeration("EStaticConstructor") {
		external = true;
		cppName = "EStaticConstructor";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("EC_StaticConstructor", "EC_StaticConstructor", int64_t(EStaticConstructor::EC_StaticConstructor), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:163:6
class Enumeration_EInternal : public das::Enumeration {
public:
	Enumeration_EInternal() : das::Enumeration("EInternal") {
		external = true;
		cppName = "EInternal";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("EC_InternalUseOnlyConstructor", "EC_InternalUseOnlyConstructor", int64_t(EInternal::EC_InternalUseOnlyConstructor), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:164:6
class Enumeration_ECppProperty : public das::Enumeration {
public:
	Enumeration_ECppProperty() : das::Enumeration("ECppProperty") {
		external = true;
		cppName = "ECppProperty";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("EC_CppProperty", "EC_CppProperty", int64_t(ECppProperty::EC_CppProperty), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:186:6
class Enumeration_EClassFlags : public das::Enumeration {
public:
	Enumeration_EClassFlags() : das::Enumeration("EClassFlags") {
		external = true;
		cppName = "EClassFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("CLASS_None", "CLASS_None", int64_t(EClassFlags::CLASS_None), das::LineInfo());
		addIEx("CLASS_Abstract", "CLASS_Abstract", int64_t(EClassFlags::CLASS_Abstract), das::LineInfo());
		addIEx("CLASS_DefaultConfig", "CLASS_DefaultConfig", int64_t(EClassFlags::CLASS_DefaultConfig), das::LineInfo());
		addIEx("CLASS_Config", "CLASS_Config", int64_t(EClassFlags::CLASS_Config), das::LineInfo());
		addIEx("CLASS_Transient", "CLASS_Transient", int64_t(EClassFlags::CLASS_Transient), das::LineInfo());
		addIEx("CLASS_Optional", "CLASS_Optional", int64_t(EClassFlags::CLASS_Optional), das::LineInfo());
		addIEx("CLASS_MatchedSerializers", "CLASS_MatchedSerializers", int64_t(EClassFlags::CLASS_MatchedSerializers), das::LineInfo());
		addIEx("CLASS_ProjectUserConfig", "CLASS_ProjectUserConfig", int64_t(EClassFlags::CLASS_ProjectUserConfig), das::LineInfo());
		addIEx("CLASS_Native", "CLASS_Native", int64_t(EClassFlags::CLASS_Native), das::LineInfo());
		addIEx("CLASS_NoExport", "CLASS_NoExport", int64_t(EClassFlags::CLASS_NoExport), das::LineInfo());
		addIEx("CLASS_NotPlaceable", "CLASS_NotPlaceable", int64_t(EClassFlags::CLASS_NotPlaceable), das::LineInfo());
		addIEx("CLASS_PerObjectConfig", "CLASS_PerObjectConfig", int64_t(EClassFlags::CLASS_PerObjectConfig), das::LineInfo());
		addIEx("CLASS_ReplicationDataIsSetUp", "CLASS_ReplicationDataIsSetUp", int64_t(EClassFlags::CLASS_ReplicationDataIsSetUp), das::LineInfo());
		addIEx("CLASS_EditInlineNew", "CLASS_EditInlineNew", int64_t(EClassFlags::CLASS_EditInlineNew), das::LineInfo());
		addIEx("CLASS_CollapseCategories", "CLASS_CollapseCategories", int64_t(EClassFlags::CLASS_CollapseCategories), das::LineInfo());
		addIEx("CLASS_Interface", "CLASS_Interface", int64_t(EClassFlags::CLASS_Interface), das::LineInfo());
		addIEx("CLASS_CustomConstructor", "CLASS_CustomConstructor", int64_t(EClassFlags::CLASS_CustomConstructor), das::LineInfo());
		addIEx("CLASS_Const", "CLASS_Const", int64_t(EClassFlags::CLASS_Const), das::LineInfo());
		addIEx("CLASS_NeedsDeferredDependencyLoading", "CLASS_NeedsDeferredDependencyLoading", int64_t(EClassFlags::CLASS_NeedsDeferredDependencyLoading), das::LineInfo());
		addIEx("CLASS_CompiledFromBlueprint", "CLASS_CompiledFromBlueprint", int64_t(EClassFlags::CLASS_CompiledFromBlueprint), das::LineInfo());
		addIEx("CLASS_MinimalAPI", "CLASS_MinimalAPI", int64_t(EClassFlags::CLASS_MinimalAPI), das::LineInfo());
		addIEx("CLASS_RequiredAPI", "CLASS_RequiredAPI", int64_t(EClassFlags::CLASS_RequiredAPI), das::LineInfo());
		addIEx("CLASS_DefaultToInstanced", "CLASS_DefaultToInstanced", int64_t(EClassFlags::CLASS_DefaultToInstanced), das::LineInfo());
		addIEx("CLASS_TokenStreamAssembled", "CLASS_TokenStreamAssembled", int64_t(EClassFlags::CLASS_TokenStreamAssembled), das::LineInfo());
		addIEx("CLASS_HasInstancedReference", "CLASS_HasInstancedReference", int64_t(EClassFlags::CLASS_HasInstancedReference), das::LineInfo());
		addIEx("CLASS_Hidden", "CLASS_Hidden", int64_t(EClassFlags::CLASS_Hidden), das::LineInfo());
		addIEx("CLASS_Deprecated", "CLASS_Deprecated", int64_t(EClassFlags::CLASS_Deprecated), das::LineInfo());
		addIEx("CLASS_HideDropDown", "CLASS_HideDropDown", int64_t(EClassFlags::CLASS_HideDropDown), das::LineInfo());
		addIEx("CLASS_GlobalUserConfig", "CLASS_GlobalUserConfig", int64_t(EClassFlags::CLASS_GlobalUserConfig), das::LineInfo());
		addIEx("CLASS_Intrinsic", "CLASS_Intrinsic", int64_t(EClassFlags::CLASS_Intrinsic), das::LineInfo());
		addIEx("CLASS_Constructed", "CLASS_Constructed", int64_t(EClassFlags::CLASS_Constructed), das::LineInfo());
		addIEx("CLASS_ConfigDoNotCheckDefaults", "CLASS_ConfigDoNotCheckDefaults", int64_t(EClassFlags::CLASS_ConfigDoNotCheckDefaults), das::LineInfo());
		addIEx("CLASS_NewerVersionExists", "CLASS_NewerVersionExists", int64_t(EClassFlags::CLASS_NewerVersionExists), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:317:6
class Enumeration_EClassCastFlags : public das::Enumeration {
public:
	Enumeration_EClassCastFlags() : das::Enumeration("EClassCastFlags") {
		external = true;
		cppName = "EClassCastFlags";
		baseType = (das::Type) das::ToBasicType<uint64>::type;
		addIEx("CASTCLASS_None", "CASTCLASS_None", int64_t(EClassCastFlags::CASTCLASS_None), das::LineInfo());
		addIEx("CASTCLASS_UField", "CASTCLASS_UField", int64_t(EClassCastFlags::CASTCLASS_UField), das::LineInfo());
		addIEx("CASTCLASS_FInt8Property", "CASTCLASS_FInt8Property", int64_t(EClassCastFlags::CASTCLASS_FInt8Property), das::LineInfo());
		addIEx("CASTCLASS_UEnum", "CASTCLASS_UEnum", int64_t(EClassCastFlags::CASTCLASS_UEnum), das::LineInfo());
		addIEx("CASTCLASS_UStruct", "CASTCLASS_UStruct", int64_t(EClassCastFlags::CASTCLASS_UStruct), das::LineInfo());
		addIEx("CASTCLASS_UScriptStruct", "CASTCLASS_UScriptStruct", int64_t(EClassCastFlags::CASTCLASS_UScriptStruct), das::LineInfo());
		addIEx("CASTCLASS_UClass", "CASTCLASS_UClass", int64_t(EClassCastFlags::CASTCLASS_UClass), das::LineInfo());
		addIEx("CASTCLASS_FByteProperty", "CASTCLASS_FByteProperty", int64_t(EClassCastFlags::CASTCLASS_FByteProperty), das::LineInfo());
		addIEx("CASTCLASS_FIntProperty", "CASTCLASS_FIntProperty", int64_t(EClassCastFlags::CASTCLASS_FIntProperty), das::LineInfo());
		addIEx("CASTCLASS_FFloatProperty", "CASTCLASS_FFloatProperty", int64_t(EClassCastFlags::CASTCLASS_FFloatProperty), das::LineInfo());
		addIEx("CASTCLASS_FUInt64Property", "CASTCLASS_FUInt64Property", int64_t(EClassCastFlags::CASTCLASS_FUInt64Property), das::LineInfo());
		addIEx("CASTCLASS_FClassProperty", "CASTCLASS_FClassProperty", int64_t(EClassCastFlags::CASTCLASS_FClassProperty), das::LineInfo());
		addIEx("CASTCLASS_FUInt32Property", "CASTCLASS_FUInt32Property", int64_t(EClassCastFlags::CASTCLASS_FUInt32Property), das::LineInfo());
		addIEx("CASTCLASS_FInterfaceProperty", "CASTCLASS_FInterfaceProperty", int64_t(EClassCastFlags::CASTCLASS_FInterfaceProperty), das::LineInfo());
		addIEx("CASTCLASS_FNameProperty", "CASTCLASS_FNameProperty", int64_t(EClassCastFlags::CASTCLASS_FNameProperty), das::LineInfo());
		addIEx("CASTCLASS_FStrProperty", "CASTCLASS_FStrProperty", int64_t(EClassCastFlags::CASTCLASS_FStrProperty), das::LineInfo());
		addIEx("CASTCLASS_FProperty", "CASTCLASS_FProperty", int64_t(EClassCastFlags::CASTCLASS_FProperty), das::LineInfo());
		addIEx("CASTCLASS_FObjectProperty", "CASTCLASS_FObjectProperty", int64_t(EClassCastFlags::CASTCLASS_FObjectProperty), das::LineInfo());
		addIEx("CASTCLASS_FBoolProperty", "CASTCLASS_FBoolProperty", int64_t(EClassCastFlags::CASTCLASS_FBoolProperty), das::LineInfo());
		addIEx("CASTCLASS_FUInt16Property", "CASTCLASS_FUInt16Property", int64_t(EClassCastFlags::CASTCLASS_FUInt16Property), das::LineInfo());
		addIEx("CASTCLASS_UFunction", "CASTCLASS_UFunction", int64_t(EClassCastFlags::CASTCLASS_UFunction), das::LineInfo());
		addIEx("CASTCLASS_FStructProperty", "CASTCLASS_FStructProperty", int64_t(EClassCastFlags::CASTCLASS_FStructProperty), das::LineInfo());
		addIEx("CASTCLASS_FArrayProperty", "CASTCLASS_FArrayProperty", int64_t(EClassCastFlags::CASTCLASS_FArrayProperty), das::LineInfo());
		addIEx("CASTCLASS_FInt64Property", "CASTCLASS_FInt64Property", int64_t(EClassCastFlags::CASTCLASS_FInt64Property), das::LineInfo());
		addIEx("CASTCLASS_FDelegateProperty", "CASTCLASS_FDelegateProperty", int64_t(EClassCastFlags::CASTCLASS_FDelegateProperty), das::LineInfo());
		addIEx("CASTCLASS_FNumericProperty", "CASTCLASS_FNumericProperty", int64_t(EClassCastFlags::CASTCLASS_FNumericProperty), das::LineInfo());
		addIEx("CASTCLASS_FMulticastDelegateProperty", "CASTCLASS_FMulticastDelegateProperty", int64_t(EClassCastFlags::CASTCLASS_FMulticastDelegateProperty), das::LineInfo());
		addIEx("CASTCLASS_FObjectPropertyBase", "CASTCLASS_FObjectPropertyBase", int64_t(EClassCastFlags::CASTCLASS_FObjectPropertyBase), das::LineInfo());
		addIEx("CASTCLASS_FWeakObjectProperty", "CASTCLASS_FWeakObjectProperty", int64_t(EClassCastFlags::CASTCLASS_FWeakObjectProperty), das::LineInfo());
		addIEx("CASTCLASS_FLazyObjectProperty", "CASTCLASS_FLazyObjectProperty", int64_t(EClassCastFlags::CASTCLASS_FLazyObjectProperty), das::LineInfo());
		addIEx("CASTCLASS_FSoftObjectProperty", "CASTCLASS_FSoftObjectProperty", int64_t(EClassCastFlags::CASTCLASS_FSoftObjectProperty), das::LineInfo());
		addIEx("CASTCLASS_FTextProperty", "CASTCLASS_FTextProperty", int64_t(EClassCastFlags::CASTCLASS_FTextProperty), das::LineInfo());
		addIEx("CASTCLASS_FInt16Property", "CASTCLASS_FInt16Property", int64_t(EClassCastFlags::CASTCLASS_FInt16Property), das::LineInfo());
		addIEx("CASTCLASS_FDoubleProperty", "CASTCLASS_FDoubleProperty", int64_t(EClassCastFlags::CASTCLASS_FDoubleProperty), das::LineInfo());
		addIEx("CASTCLASS_FSoftClassProperty", "CASTCLASS_FSoftClassProperty", int64_t(EClassCastFlags::CASTCLASS_FSoftClassProperty), das::LineInfo());
		addIEx("CASTCLASS_UPackage", "CASTCLASS_UPackage", int64_t(EClassCastFlags::CASTCLASS_UPackage), das::LineInfo());
		addIEx("CASTCLASS_ULevel", "CASTCLASS_ULevel", int64_t(EClassCastFlags::CASTCLASS_ULevel), das::LineInfo());
		addIEx("CASTCLASS_AActor", "CASTCLASS_AActor", int64_t(EClassCastFlags::CASTCLASS_AActor), das::LineInfo());
		addIEx("CASTCLASS_APlayerController", "CASTCLASS_APlayerController", int64_t(EClassCastFlags::CASTCLASS_APlayerController), das::LineInfo());
		addIEx("CASTCLASS_APawn", "CASTCLASS_APawn", int64_t(EClassCastFlags::CASTCLASS_APawn), das::LineInfo());
		addIEx("CASTCLASS_USceneComponent", "CASTCLASS_USceneComponent", int64_t(EClassCastFlags::CASTCLASS_USceneComponent), das::LineInfo());
		addIEx("CASTCLASS_UPrimitiveComponent", "CASTCLASS_UPrimitiveComponent", int64_t(EClassCastFlags::CASTCLASS_UPrimitiveComponent), das::LineInfo());
		addIEx("CASTCLASS_USkinnedMeshComponent", "CASTCLASS_USkinnedMeshComponent", int64_t(EClassCastFlags::CASTCLASS_USkinnedMeshComponent), das::LineInfo());
		addIEx("CASTCLASS_USkeletalMeshComponent", "CASTCLASS_USkeletalMeshComponent", int64_t(EClassCastFlags::CASTCLASS_USkeletalMeshComponent), das::LineInfo());
		addIEx("CASTCLASS_UBlueprint", "CASTCLASS_UBlueprint", int64_t(EClassCastFlags::CASTCLASS_UBlueprint), das::LineInfo());
		addIEx("CASTCLASS_UDelegateFunction", "CASTCLASS_UDelegateFunction", int64_t(EClassCastFlags::CASTCLASS_UDelegateFunction), das::LineInfo());
		addIEx("CASTCLASS_UStaticMeshComponent", "CASTCLASS_UStaticMeshComponent", int64_t(EClassCastFlags::CASTCLASS_UStaticMeshComponent), das::LineInfo());
		addIEx("CASTCLASS_FMapProperty", "CASTCLASS_FMapProperty", int64_t(EClassCastFlags::CASTCLASS_FMapProperty), das::LineInfo());
		addIEx("CASTCLASS_FSetProperty", "CASTCLASS_FSetProperty", int64_t(EClassCastFlags::CASTCLASS_FSetProperty), das::LineInfo());
		addIEx("CASTCLASS_FEnumProperty", "CASTCLASS_FEnumProperty", int64_t(EClassCastFlags::CASTCLASS_FEnumProperty), das::LineInfo());
		addIEx("CASTCLASS_USparseDelegateFunction", "CASTCLASS_USparseDelegateFunction", int64_t(EClassCastFlags::CASTCLASS_USparseDelegateFunction), das::LineInfo());
		addIEx("CASTCLASS_FMulticastInlineDelegateProperty", "CASTCLASS_FMulticastInlineDelegateProperty", int64_t(EClassCastFlags::CASTCLASS_FMulticastInlineDelegateProperty), das::LineInfo());
		addIEx("CASTCLASS_FMulticastSparseDelegateProperty", "CASTCLASS_FMulticastSparseDelegateProperty", int64_t(EClassCastFlags::CASTCLASS_FMulticastSparseDelegateProperty), das::LineInfo());
		addIEx("CASTCLASS_FFieldPathProperty", "CASTCLASS_FFieldPathProperty", int64_t(EClassCastFlags::CASTCLASS_FFieldPathProperty), das::LineInfo());
		addIEx("CASTCLASS_FObjectPtrProperty", "CASTCLASS_FObjectPtrProperty", int64_t(EClassCastFlags::CASTCLASS_FObjectPtrProperty), das::LineInfo());
		addIEx("CASTCLASS_FClassPtrProperty", "CASTCLASS_FClassPtrProperty", int64_t(EClassCastFlags::CASTCLASS_FClassPtrProperty), das::LineInfo());
		addIEx("CASTCLASS_FLargeWorldCoordinatesRealProperty", "CASTCLASS_FLargeWorldCoordinatesRealProperty", int64_t(EClassCastFlags::CASTCLASS_FLargeWorldCoordinatesRealProperty), das::LineInfo());
		addIEx("CASTCLASS_FOptionalProperty", "CASTCLASS_FOptionalProperty", int64_t(EClassCastFlags::CASTCLASS_FOptionalProperty), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:393:6
class Enumeration_EPropertyFlags : public das::Enumeration {
public:
	Enumeration_EPropertyFlags() : das::Enumeration("EPropertyFlags") {
		external = true;
		cppName = "EPropertyFlags";
		baseType = (das::Type) das::ToBasicType<uint64>::type;
		addIEx("CPF_None", "CPF_None", int64_t(EPropertyFlags::CPF_None), das::LineInfo());
		addIEx("CPF_Edit", "CPF_Edit", int64_t(EPropertyFlags::CPF_Edit), das::LineInfo());
		addIEx("CPF_ConstParm", "CPF_ConstParm", int64_t(EPropertyFlags::CPF_ConstParm), das::LineInfo());
		addIEx("CPF_BlueprintVisible", "CPF_BlueprintVisible", int64_t(EPropertyFlags::CPF_BlueprintVisible), das::LineInfo());
		addIEx("CPF_ExportObject", "CPF_ExportObject", int64_t(EPropertyFlags::CPF_ExportObject), das::LineInfo());
		addIEx("CPF_BlueprintReadOnly", "CPF_BlueprintReadOnly", int64_t(EPropertyFlags::CPF_BlueprintReadOnly), das::LineInfo());
		addIEx("CPF_Net", "CPF_Net", int64_t(EPropertyFlags::CPF_Net), das::LineInfo());
		addIEx("CPF_EditFixedSize", "CPF_EditFixedSize", int64_t(EPropertyFlags::CPF_EditFixedSize), das::LineInfo());
		addIEx("CPF_Parm", "CPF_Parm", int64_t(EPropertyFlags::CPF_Parm), das::LineInfo());
		addIEx("CPF_OutParm", "CPF_OutParm", int64_t(EPropertyFlags::CPF_OutParm), das::LineInfo());
		addIEx("CPF_ZeroConstructor", "CPF_ZeroConstructor", int64_t(EPropertyFlags::CPF_ZeroConstructor), das::LineInfo());
		addIEx("CPF_ReturnParm", "CPF_ReturnParm", int64_t(EPropertyFlags::CPF_ReturnParm), das::LineInfo());
		addIEx("CPF_DisableEditOnTemplate", "CPF_DisableEditOnTemplate", int64_t(EPropertyFlags::CPF_DisableEditOnTemplate), das::LineInfo());
		addIEx("CPF_NonNullable", "CPF_NonNullable", int64_t(EPropertyFlags::CPF_NonNullable), das::LineInfo());
		addIEx("CPF_Transient", "CPF_Transient", int64_t(EPropertyFlags::CPF_Transient), das::LineInfo());
		addIEx("CPF_Config", "CPF_Config", int64_t(EPropertyFlags::CPF_Config), das::LineInfo());
		addIEx("CPF_RequiredParm", "CPF_RequiredParm", int64_t(EPropertyFlags::CPF_RequiredParm), das::LineInfo());
		addIEx("CPF_DisableEditOnInstance", "CPF_DisableEditOnInstance", int64_t(EPropertyFlags::CPF_DisableEditOnInstance), das::LineInfo());
		addIEx("CPF_EditConst", "CPF_EditConst", int64_t(EPropertyFlags::CPF_EditConst), das::LineInfo());
		addIEx("CPF_GlobalConfig", "CPF_GlobalConfig", int64_t(EPropertyFlags::CPF_GlobalConfig), das::LineInfo());
		addIEx("CPF_InstancedReference", "CPF_InstancedReference", int64_t(EPropertyFlags::CPF_InstancedReference), das::LineInfo());
		addIEx("CPF_DuplicateTransient", "CPF_DuplicateTransient", int64_t(EPropertyFlags::CPF_DuplicateTransient), das::LineInfo());
		addIEx("CPF_SaveGame", "CPF_SaveGame", int64_t(EPropertyFlags::CPF_SaveGame), das::LineInfo());
		addIEx("CPF_NoClear", "CPF_NoClear", int64_t(EPropertyFlags::CPF_NoClear), das::LineInfo());
		addIEx("CPF_ReferenceParm", "CPF_ReferenceParm", int64_t(EPropertyFlags::CPF_ReferenceParm), das::LineInfo());
		addIEx("CPF_BlueprintAssignable", "CPF_BlueprintAssignable", int64_t(EPropertyFlags::CPF_BlueprintAssignable), das::LineInfo());
		addIEx("CPF_Deprecated", "CPF_Deprecated", int64_t(EPropertyFlags::CPF_Deprecated), das::LineInfo());
		addIEx("CPF_IsPlainOldData", "CPF_IsPlainOldData", int64_t(EPropertyFlags::CPF_IsPlainOldData), das::LineInfo());
		addIEx("CPF_RepSkip", "CPF_RepSkip", int64_t(EPropertyFlags::CPF_RepSkip), das::LineInfo());
		addIEx("CPF_RepNotify", "CPF_RepNotify", int64_t(EPropertyFlags::CPF_RepNotify), das::LineInfo());
		addIEx("CPF_Interp", "CPF_Interp", int64_t(EPropertyFlags::CPF_Interp), das::LineInfo());
		addIEx("CPF_NonTransactional", "CPF_NonTransactional", int64_t(EPropertyFlags::CPF_NonTransactional), das::LineInfo());
		addIEx("CPF_EditorOnly", "CPF_EditorOnly", int64_t(EPropertyFlags::CPF_EditorOnly), das::LineInfo());
		addIEx("CPF_NoDestructor", "CPF_NoDestructor", int64_t(EPropertyFlags::CPF_NoDestructor), das::LineInfo());
		addIEx("CPF_AutoWeak", "CPF_AutoWeak", int64_t(EPropertyFlags::CPF_AutoWeak), das::LineInfo());
		addIEx("CPF_ContainsInstancedReference", "CPF_ContainsInstancedReference", int64_t(EPropertyFlags::CPF_ContainsInstancedReference), das::LineInfo());
		addIEx("CPF_AssetRegistrySearchable", "CPF_AssetRegistrySearchable", int64_t(EPropertyFlags::CPF_AssetRegistrySearchable), das::LineInfo());
		addIEx("CPF_SimpleDisplay", "CPF_SimpleDisplay", int64_t(EPropertyFlags::CPF_SimpleDisplay), das::LineInfo());
		addIEx("CPF_AdvancedDisplay", "CPF_AdvancedDisplay", int64_t(EPropertyFlags::CPF_AdvancedDisplay), das::LineInfo());
		addIEx("CPF_Protected", "CPF_Protected", int64_t(EPropertyFlags::CPF_Protected), das::LineInfo());
		addIEx("CPF_BlueprintCallable", "CPF_BlueprintCallable", int64_t(EPropertyFlags::CPF_BlueprintCallable), das::LineInfo());
		addIEx("CPF_BlueprintAuthorityOnly", "CPF_BlueprintAuthorityOnly", int64_t(EPropertyFlags::CPF_BlueprintAuthorityOnly), das::LineInfo());
		addIEx("CPF_TextExportTransient", "CPF_TextExportTransient", int64_t(EPropertyFlags::CPF_TextExportTransient), das::LineInfo());
		addIEx("CPF_NonPIEDuplicateTransient", "CPF_NonPIEDuplicateTransient", int64_t(EPropertyFlags::CPF_NonPIEDuplicateTransient), das::LineInfo());
		addIEx("CPF_ExposeOnSpawn", "CPF_ExposeOnSpawn", int64_t(EPropertyFlags::CPF_ExposeOnSpawn), das::LineInfo());
		addIEx("CPF_PersistentInstance", "CPF_PersistentInstance", int64_t(EPropertyFlags::CPF_PersistentInstance), das::LineInfo());
		addIEx("CPF_UObjectWrapper", "CPF_UObjectWrapper", int64_t(EPropertyFlags::CPF_UObjectWrapper), das::LineInfo());
		addIEx("CPF_HasGetValueTypeHash", "CPF_HasGetValueTypeHash", int64_t(EPropertyFlags::CPF_HasGetValueTypeHash), das::LineInfo());
		addIEx("CPF_NativeAccessSpecifierPublic", "CPF_NativeAccessSpecifierPublic", int64_t(EPropertyFlags::CPF_NativeAccessSpecifierPublic), das::LineInfo());
		addIEx("CPF_NativeAccessSpecifierProtected", "CPF_NativeAccessSpecifierProtected", int64_t(EPropertyFlags::CPF_NativeAccessSpecifierProtected), das::LineInfo());
		addIEx("CPF_NativeAccessSpecifierPrivate", "CPF_NativeAccessSpecifierPrivate", int64_t(EPropertyFlags::CPF_NativeAccessSpecifierPrivate), das::LineInfo());
		addIEx("CPF_SkipSerialization", "CPF_SkipSerialization", int64_t(EPropertyFlags::CPF_SkipSerialization), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:484:12
class Enumeration_EArrayPropertyFlags : public das::Enumeration {
public:
	Enumeration_EArrayPropertyFlags() : das::Enumeration("EArrayPropertyFlags") {
		external = true;
		cppName = "EArrayPropertyFlags";
		baseType = (das::Type) das::ToBasicType<uint8>::type;
		addIEx("None", "None", int64_t(EArrayPropertyFlags::None), das::LineInfo());
		addIEx("UsesMemoryImageAllocator", "UsesMemoryImageAllocator", int64_t(EArrayPropertyFlags::UsesMemoryImageAllocator), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:495:12
class Enumeration_EMapPropertyFlags : public das::Enumeration {
public:
	Enumeration_EMapPropertyFlags() : das::Enumeration("EMapPropertyFlags") {
		external = true;
		cppName = "EMapPropertyFlags";
		baseType = (das::Type) das::ToBasicType<uint8>::type;
		addIEx("None", "None", int64_t(EMapPropertyFlags::None), das::LineInfo());
		addIEx("UsesMemoryImageAllocator", "UsesMemoryImageAllocator", int64_t(EMapPropertyFlags::UsesMemoryImageAllocator), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:503:12
class Enumeration_EPropertyObjectReferenceType : public das::Enumeration {
public:
	Enumeration_EPropertyObjectReferenceType() : das::Enumeration("EPropertyObjectReferenceType") {
		external = true;
		cppName = "EPropertyObjectReferenceType";
		baseType = (das::Type) das::ToBasicType<uint32>::type;
		addIEx("None", "None", int64_t(EPropertyObjectReferenceType::None), das::LineInfo());
		addIEx("Strong", "Strong", int64_t(EPropertyObjectReferenceType::Strong), das::LineInfo());
		addIEx("Weak", "Weak", int64_t(EPropertyObjectReferenceType::Weak), das::LineInfo());
		addIEx("Soft", "Soft", int64_t(EPropertyObjectReferenceType::Soft), das::LineInfo());
		addIEx("Conservative", "Conservative", int64_t(EPropertyObjectReferenceType::Conservative), das::LineInfo());
		addIEx("MAX", "MAX", int64_t(EPropertyObjectReferenceType::MAX), das::LineInfo());
		addIEx("Any", "Any", int64_t(EPropertyObjectReferenceType::Any), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:521:6
class Enumeration_EObjectFlags : public das::Enumeration {
public:
	Enumeration_EObjectFlags() : das::Enumeration("EObjectFlags") {
		external = true;
		cppName = "EObjectFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("RF_NoFlags", "RF_NoFlags", int64_t(EObjectFlags::RF_NoFlags), das::LineInfo());
		addIEx("RF_Public", "RF_Public", int64_t(EObjectFlags::RF_Public), das::LineInfo());
		addIEx("RF_Standalone", "RF_Standalone", int64_t(EObjectFlags::RF_Standalone), das::LineInfo());
		addIEx("RF_MarkAsNative", "RF_MarkAsNative", int64_t(EObjectFlags::RF_MarkAsNative), das::LineInfo());
		addIEx("RF_Transactional", "RF_Transactional", int64_t(EObjectFlags::RF_Transactional), das::LineInfo());
		addIEx("RF_ClassDefaultObject", "RF_ClassDefaultObject", int64_t(EObjectFlags::RF_ClassDefaultObject), das::LineInfo());
		addIEx("RF_ArchetypeObject", "RF_ArchetypeObject", int64_t(EObjectFlags::RF_ArchetypeObject), das::LineInfo());
		addIEx("RF_Transient", "RF_Transient", int64_t(EObjectFlags::RF_Transient), das::LineInfo());
		addIEx("RF_MarkAsRootSet", "RF_MarkAsRootSet", int64_t(EObjectFlags::RF_MarkAsRootSet), das::LineInfo());
		addIEx("RF_TagGarbageTemp", "RF_TagGarbageTemp", int64_t(EObjectFlags::RF_TagGarbageTemp), das::LineInfo());
		addIEx("RF_NeedInitialization", "RF_NeedInitialization", int64_t(EObjectFlags::RF_NeedInitialization), das::LineInfo());
		addIEx("RF_NeedLoad", "RF_NeedLoad", int64_t(EObjectFlags::RF_NeedLoad), das::LineInfo());
		addIEx("RF_KeepForCooker", "RF_KeepForCooker", int64_t(EObjectFlags::RF_KeepForCooker), das::LineInfo());
		addIEx("RF_NeedPostLoad", "RF_NeedPostLoad", int64_t(EObjectFlags::RF_NeedPostLoad), das::LineInfo());
		addIEx("RF_NeedPostLoadSubobjects", "RF_NeedPostLoadSubobjects", int64_t(EObjectFlags::RF_NeedPostLoadSubobjects), das::LineInfo());
		addIEx("RF_NewerVersionExists", "RF_NewerVersionExists", int64_t(EObjectFlags::RF_NewerVersionExists), das::LineInfo());
		addIEx("RF_BeginDestroyed", "RF_BeginDestroyed", int64_t(EObjectFlags::RF_BeginDestroyed), das::LineInfo());
		addIEx("RF_FinishDestroyed", "RF_FinishDestroyed", int64_t(EObjectFlags::RF_FinishDestroyed), das::LineInfo());
		addIEx("RF_BeingRegenerated", "RF_BeingRegenerated", int64_t(EObjectFlags::RF_BeingRegenerated), das::LineInfo());
		addIEx("RF_DefaultSubObject", "RF_DefaultSubObject", int64_t(EObjectFlags::RF_DefaultSubObject), das::LineInfo());
		addIEx("RF_WasLoaded", "RF_WasLoaded", int64_t(EObjectFlags::RF_WasLoaded), das::LineInfo());
		addIEx("RF_TextExportTransient", "RF_TextExportTransient", int64_t(EObjectFlags::RF_TextExportTransient), das::LineInfo());
		addIEx("RF_LoadCompleted", "RF_LoadCompleted", int64_t(EObjectFlags::RF_LoadCompleted), das::LineInfo());
		addIEx("RF_InheritableComponentTemplate", "RF_InheritableComponentTemplate", int64_t(EObjectFlags::RF_InheritableComponentTemplate), das::LineInfo());
		addIEx("RF_DuplicateTransient", "RF_DuplicateTransient", int64_t(EObjectFlags::RF_DuplicateTransient), das::LineInfo());
		addIEx("RF_StrongRefOnFrame", "RF_StrongRefOnFrame", int64_t(EObjectFlags::RF_StrongRefOnFrame), das::LineInfo());
		addIEx("RF_NonPIEDuplicateTransient", "RF_NonPIEDuplicateTransient", int64_t(EObjectFlags::RF_NonPIEDuplicateTransient), das::LineInfo());
		addIEx("RF_Dynamic", "RF_Dynamic", int64_t(EObjectFlags::RF_Dynamic), das::LineInfo());
		addIEx("RF_WillBeLoaded", "RF_WillBeLoaded", int64_t(EObjectFlags::RF_WillBeLoaded), das::LineInfo());
		addIEx("RF_HasExternalPackage", "RF_HasExternalPackage", int64_t(EObjectFlags::RF_HasExternalPackage), das::LineInfo());
		addIEx("RF_PendingKill", "RF_PendingKill", int64_t(EObjectFlags::RF_PendingKill), das::LineInfo());
		addIEx("RF_Garbage", "RF_Garbage", int64_t(EObjectFlags::RF_Garbage), das::LineInfo());
		addIEx("RF_AllocatedInSharedPage", "RF_AllocatedInSharedPage", int64_t(EObjectFlags::RF_AllocatedInSharedPage), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:598:12
class Enumeration_EInternalObjectFlags : public das::Enumeration {
public:
	Enumeration_EInternalObjectFlags() : das::Enumeration("EInternalObjectFlags") {
		external = true;
		cppName = "EInternalObjectFlags";
		baseType = (das::Type) das::ToBasicType<int32>::type;
		addIEx("None", "None", int64_t(EInternalObjectFlags::None), das::LineInfo());
		addIEx("LoaderImport", "LoaderImport", int64_t(EInternalObjectFlags::LoaderImport), das::LineInfo());
		addIEx("Garbage", "Garbage", int64_t(EInternalObjectFlags::Garbage), das::LineInfo());
		addIEx("ReachableInCluster", "ReachableInCluster", int64_t(EInternalObjectFlags::ReachableInCluster), das::LineInfo());
		addIEx("ClusterRoot", "ClusterRoot", int64_t(EInternalObjectFlags::ClusterRoot), das::LineInfo());
		addIEx("Native", "Native", int64_t(EInternalObjectFlags::Native), das::LineInfo());
		addIEx("Async", "Async", int64_t(EInternalObjectFlags::Async), das::LineInfo());
		addIEx("AsyncLoading", "AsyncLoading", int64_t(EInternalObjectFlags::AsyncLoading), das::LineInfo());
		addIEx("Unreachable", "Unreachable", int64_t(EInternalObjectFlags::Unreachable), das::LineInfo());
		addIEx("PendingKill", "PendingKill", int64_t(EInternalObjectFlags::PendingKill), das::LineInfo());
		addIEx("RootSet", "RootSet", int64_t(EInternalObjectFlags::RootSet), das::LineInfo());
		addIEx("PendingConstruction", "PendingConstruction", int64_t(EInternalObjectFlags::PendingConstruction), das::LineInfo());
		addIEx("GarbageCollectionKeepFlags", "GarbageCollectionKeepFlags", int64_t(EInternalObjectFlags::GarbageCollectionKeepFlags), das::LineInfo());
		addIEx("MirroredFlags", "MirroredFlags", int64_t(EInternalObjectFlags::MirroredFlags), das::LineInfo());
		addIEx("AllFlags", "AllFlags", int64_t(EInternalObjectFlags::AllFlags), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:630:12
class Enumeration_EEnumFlags : public das::Enumeration {
public:
	Enumeration_EEnumFlags() : das::Enumeration("EEnumFlags") {
		external = true;
		cppName = "EEnumFlags";
		baseType = (das::Type) das::ToBasicType<uint8>::type;
		addIEx("None", "None", int64_t(EEnumFlags::None), das::LineInfo());
		addIEx("Flags", "Flags", int64_t(EEnumFlags::Flags), das::LineInfo());
		addIEx("NewerVersionExists", "NewerVersionExists", int64_t(EEnumFlags::NewerVersionExists), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:2129:7
class Enumeration_UE_ELoadConfigPropagationFlags : public das::Enumeration {
public:
	Enumeration_UE_ELoadConfigPropagationFlags() : das::Enumeration("UE_ELoadConfigPropagationFlags") {
		external = true;
		cppName = "UE::ELoadConfigPropagationFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("LCPF_None", "LCPF_None", int64_t(UE::ELoadConfigPropagationFlags::LCPF_None), das::LineInfo());
		addIEx("LCPF_ReadParentSections", "LCPF_ReadParentSections", int64_t(UE::ELoadConfigPropagationFlags::LCPF_ReadParentSections), das::LineInfo());
		addIEx("LCPF_PropagateToChildDefaultObjects", "LCPF_PropagateToChildDefaultObjects", int64_t(UE::ELoadConfigPropagationFlags::LCPF_PropagateToChildDefaultObjects), das::LineInfo());
		addIEx("LCPF_PropagateToInstances", "LCPF_PropagateToInstances", int64_t(UE::ELoadConfigPropagationFlags::LCPF_PropagateToInstances), das::LineInfo());
		addIEx("LCPF_ReloadingConfigData", "LCPF_ReloadingConfigData", int64_t(UE::ELoadConfigPropagationFlags::LCPF_ReloadingConfigData), das::LineInfo());
		addIEx("LCPF_PersistentFlags", "LCPF_PersistentFlags", int64_t(UE::ELoadConfigPropagationFlags::LCPF_PersistentFlags), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectMarks.h:45:6
class Enumeration_EObjectMark : public das::Enumeration {
public:
	Enumeration_EObjectMark() : das::Enumeration("EObjectMark") {
		external = true;
		cppName = "EObjectMark";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("OBJECTMARK_NOMARKS", "OBJECTMARK_NOMARKS", int64_t(EObjectMark::OBJECTMARK_NOMARKS), das::LineInfo());
		addIEx("OBJECTMARK_Saved", "OBJECTMARK_Saved", int64_t(EObjectMark::OBJECTMARK_Saved), das::LineInfo());
		addIEx("OBJECTMARK_TagImp", "OBJECTMARK_TagImp", int64_t(EObjectMark::OBJECTMARK_TagImp), das::LineInfo());
		addIEx("OBJECTMARK_TagExp", "OBJECTMARK_TagExp", int64_t(EObjectMark::OBJECTMARK_TagExp), das::LineInfo());
		addIEx("OBJECTMARK_NotForClient", "OBJECTMARK_NotForClient", int64_t(EObjectMark::OBJECTMARK_NotForClient), das::LineInfo());
		addIEx("OBJECTMARK_NotForServer", "OBJECTMARK_NotForServer", int64_t(EObjectMark::OBJECTMARK_NotForServer), das::LineInfo());
		addIEx("OBJECTMARK_NotAlwaysLoadedForEditorGame", "OBJECTMARK_NotAlwaysLoadedForEditorGame", int64_t(EObjectMark::OBJECTMARK_NotAlwaysLoadedForEditorGame), das::LineInfo());
		addIEx("OBJECTMARK_EditorOnly", "OBJECTMARK_EditorOnly", int64_t(EObjectMark::OBJECTMARK_EditorOnly), das::LineInfo());
		addIEx("OBJECTMARK_NotForTargetPlatform", "OBJECTMARK_NotForTargetPlatform", int64_t(EObjectMark::OBJECTMARK_NotForTargetPlatform), das::LineInfo());
		addIEx("OBJECTMARK_INHERITEDMARKS", "OBJECTMARK_INHERITEDMARKS", int64_t(EObjectMark::OBJECTMARK_INHERITEDMARKS), das::LineInfo());
		addIEx("OBJECTMARK_ALLMARKS", "OBJECTMARK_ALLMARKS", int64_t(EObjectMark::OBJECTMARK_ALLMARKS), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:43:12
class Enumeration_EObjectFullNameFlags : public das::Enumeration {
public:
	Enumeration_EObjectFullNameFlags() : das::Enumeration("EObjectFullNameFlags") {
		external = true;
		cppName = "EObjectFullNameFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("None", "None", int64_t(EObjectFullNameFlags::None), das::LineInfo());
		addIEx("IncludeClassPackage", "IncludeClassPackage", int64_t(EObjectFullNameFlags::IncludeClassPackage), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:38:13
class Enumeration_UE_Net_EFragmentRegistrationFlags : public das::Enumeration {
public:
	Enumeration_UE_Net_EFragmentRegistrationFlags() : das::Enumeration("UE_Net_EFragmentRegistrationFlags") {
		external = true;
		cppName = "UE::Net::EFragmentRegistrationFlags";
		baseType = (das::Type) das::ToBasicType<uint32>::type;
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:51:7
class Enumeration_ECastCheckedType_Type : public das::Enumeration {
public:
	Enumeration_ECastCheckedType_Type() : das::Enumeration("ECastCheckedType_Type") {
		external = true;
		cppName = "ECastCheckedType::Type";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("NullAllowed", "NullAllowed", int64_t(ECastCheckedType::Type::NullAllowed), das::LineInfo());
		addIEx("NullChecked", "NullChecked", int64_t(ECastCheckedType::Type::NullChecked), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:62:12
class Enumeration_EEditChangePropagationFlags : public das::Enumeration {
public:
	Enumeration_EEditChangePropagationFlags() : das::Enumeration("EEditChangePropagationFlags") {
		external = true;
		cppName = "EEditChangePropagationFlags";
		baseType = (das::Type) das::ToBasicType<uint8>::type;
		addIEx("None", "None", int64_t(EEditChangePropagationFlags::None), das::LineInfo());
		addIEx("OnlyMarkRealignedInstancesAsDirty", "OnlyMarkRealignedInstancesAsDirty", int64_t(EEditChangePropagationFlags::OnlyMarkRealignedInstancesAsDirty), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1696:13
class Enumeration_UObject_ENetFields_Private : public das::Enumeration {
public:
	Enumeration_UObject_ENetFields_Private() : das::Enumeration("UObject_ENetFields_Private") {
		external = true;
		cppName = "UObject::ENetFields_Private";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("NETFIELD_REP_START", "NETFIELD_REP_START", int64_t(UObject::ENetFields_Private::NETFIELD_REP_START), das::LineInfo());
		addIEx("NETFIELD_REP_END", "NETFIELD_REP_END", int64_t(UObject::ENetFields_Private::NETFIELD_REP_END), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:756:6
class Enumeration_EStructFlags : public das::Enumeration {
public:
	Enumeration_EStructFlags() : das::Enumeration("EStructFlags") {
		external = true;
		cppName = "EStructFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("STRUCT_NoFlags", "STRUCT_NoFlags", int64_t(EStructFlags::STRUCT_NoFlags), das::LineInfo());
		addIEx("STRUCT_Native", "STRUCT_Native", int64_t(EStructFlags::STRUCT_Native), das::LineInfo());
		addIEx("STRUCT_IdenticalNative", "STRUCT_IdenticalNative", int64_t(EStructFlags::STRUCT_IdenticalNative), das::LineInfo());
		addIEx("STRUCT_HasInstancedReference", "STRUCT_HasInstancedReference", int64_t(EStructFlags::STRUCT_HasInstancedReference), das::LineInfo());
		addIEx("STRUCT_NoExport", "STRUCT_NoExport", int64_t(EStructFlags::STRUCT_NoExport), das::LineInfo());
		addIEx("STRUCT_Atomic", "STRUCT_Atomic", int64_t(EStructFlags::STRUCT_Atomic), das::LineInfo());
		addIEx("STRUCT_Immutable", "STRUCT_Immutable", int64_t(EStructFlags::STRUCT_Immutable), das::LineInfo());
		addIEx("STRUCT_AddStructReferencedObjects", "STRUCT_AddStructReferencedObjects", int64_t(EStructFlags::STRUCT_AddStructReferencedObjects), das::LineInfo());
		addIEx("STRUCT_RequiredAPI", "STRUCT_RequiredAPI", int64_t(EStructFlags::STRUCT_RequiredAPI), das::LineInfo());
		addIEx("STRUCT_NetSerializeNative", "STRUCT_NetSerializeNative", int64_t(EStructFlags::STRUCT_NetSerializeNative), das::LineInfo());
		addIEx("STRUCT_SerializeNative", "STRUCT_SerializeNative", int64_t(EStructFlags::STRUCT_SerializeNative), das::LineInfo());
		addIEx("STRUCT_CopyNative", "STRUCT_CopyNative", int64_t(EStructFlags::STRUCT_CopyNative), das::LineInfo());
		addIEx("STRUCT_IsPlainOldData", "STRUCT_IsPlainOldData", int64_t(EStructFlags::STRUCT_IsPlainOldData), das::LineInfo());
		addIEx("STRUCT_NoDestructor", "STRUCT_NoDestructor", int64_t(EStructFlags::STRUCT_NoDestructor), das::LineInfo());
		addIEx("STRUCT_ZeroConstructor", "STRUCT_ZeroConstructor", int64_t(EStructFlags::STRUCT_ZeroConstructor), das::LineInfo());
		addIEx("STRUCT_ExportTextItemNative", "STRUCT_ExportTextItemNative", int64_t(EStructFlags::STRUCT_ExportTextItemNative), das::LineInfo());
		addIEx("STRUCT_ImportTextItemNative", "STRUCT_ImportTextItemNative", int64_t(EStructFlags::STRUCT_ImportTextItemNative), das::LineInfo());
		addIEx("STRUCT_PostSerializeNative", "STRUCT_PostSerializeNative", int64_t(EStructFlags::STRUCT_PostSerializeNative), das::LineInfo());
		addIEx("STRUCT_SerializeFromMismatchedTag", "STRUCT_SerializeFromMismatchedTag", int64_t(EStructFlags::STRUCT_SerializeFromMismatchedTag), das::LineInfo());
		addIEx("STRUCT_NetDeltaSerializeNative", "STRUCT_NetDeltaSerializeNative", int64_t(EStructFlags::STRUCT_NetDeltaSerializeNative), das::LineInfo());
		addIEx("STRUCT_PostScriptConstruct", "STRUCT_PostScriptConstruct", int64_t(EStructFlags::STRUCT_PostScriptConstruct), das::LineInfo());
		addIEx("STRUCT_NetSharedSerialization", "STRUCT_NetSharedSerialization", int64_t(EStructFlags::STRUCT_NetSharedSerialization), das::LineInfo());
		addIEx("STRUCT_Trashed", "STRUCT_Trashed", int64_t(EStructFlags::STRUCT_Trashed), das::LineInfo());
		addIEx("STRUCT_NewerVersionExists", "STRUCT_NewerVersionExists", int64_t(EStructFlags::STRUCT_NewerVersionExists), das::LineInfo());
		addIEx("STRUCT_CanEditChange", "STRUCT_CanEditChange", int64_t(EStructFlags::STRUCT_CanEditChange), das::LineInfo());
		addIEx("STRUCT_Inherit", "STRUCT_Inherit", int64_t(EStructFlags::STRUCT_Inherit), das::LineInfo());
		addIEx("STRUCT_ComputedFlags", "STRUCT_ComputedFlags", int64_t(EStructFlags::STRUCT_ComputedFlags), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1980:12
class Enumeration_EGetByNameFlags : public das::Enumeration {
public:
	Enumeration_EGetByNameFlags() : das::Enumeration("EGetByNameFlags") {
		external = true;
		cppName = "EGetByNameFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("None", "None", int64_t(EGetByNameFlags::None), das::LineInfo());
		addIEx("ErrorIfNotFound", "ErrorIfNotFound", int64_t(EGetByNameFlags::ErrorIfNotFound), das::LineInfo());
		addIEx("CaseSensitive", "CaseSensitive", int64_t(EGetByNameFlags::CaseSensitive), das::LineInfo());
		addIEx("CheckAuthoredName", "CheckAuthoredName", int64_t(EGetByNameFlags::CheckAuthoredName), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2006:13
class Enumeration_UEnum_ECppForm : public das::Enumeration {
public:
	Enumeration_UEnum_ECppForm() : das::Enumeration("UEnum_ECppForm") {
		external = true;
		cppName = "UEnum::ECppForm";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("Regular", "Regular", int64_t(UEnum::ECppForm::Regular), das::LineInfo());
		addIEx("Namespaced", "Namespaced", int64_t(UEnum::ECppForm::Namespaced), das::LineInfo());
		addIEx("EnumClass", "EnumClass", int64_t(UEnum::ECppForm::EnumClass), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2698:7
class Enumeration_EIncludeSuperFlag_Type : public das::Enumeration {
public:
	Enumeration_EIncludeSuperFlag_Type() : das::Enumeration("EIncludeSuperFlag_Type") {
		external = true;
		cppName = "EIncludeSuperFlag::Type";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("ExcludeSuper", "ExcludeSuper", int64_t(EIncludeSuperFlag::Type::ExcludeSuper), das::LineInfo());
		addIEx("IncludeSuper", "IncludeSuper", int64_t(EIncludeSuperFlag::Type::IncludeSuper), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2712:12
class Enumeration_EGetSparseClassDataMethod : public das::Enumeration {
public:
	Enumeration_EGetSparseClassDataMethod() : das::Enumeration("EGetSparseClassDataMethod") {
		external = true;
		cppName = "EGetSparseClassDataMethod";
		baseType = (das::Type) das::ToBasicType<uint8>::type;
		addIEx("CreateIfNull", "CreateIfNull", int64_t(EGetSparseClassDataMethod::CreateIfNull), das::LineInfo());
		addIEx("ArchetypeIfNull", "ArchetypeIfNull", int64_t(EGetSparseClassDataMethod::ArchetypeIfNull), das::LineInfo());
		addIEx("ReturnIfNull", "ReturnIfNull", int64_t(EGetSparseClassDataMethod::ReturnIfNull), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3656:12
class Enumeration_EInstancePropertyValueFlags : public das::Enumeration {
public:
	Enumeration_EInstancePropertyValueFlags() : das::Enumeration("EInstancePropertyValueFlags") {
		external = true;
		cppName = "EInstancePropertyValueFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("None", "None", int64_t(EInstancePropertyValueFlags::None), das::LineInfo());
		addIEx("CausesInstancing", "CausesInstancing", int64_t(EInstancePropertyValueFlags::CausesInstancing), das::LineInfo());
		addIEx("AllowSelfReference", "AllowSelfReference", int64_t(EInstancePropertyValueFlags::AllowSelfReference), das::LineInfo());
		addIEx("DoNotCreateNewInstance", "DoNotCreateNewInstance", int64_t(EInstancePropertyValueFlags::DoNotCreateNewInstance), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3672:12
class Enumeration_EObjectInstancingGraphOptions : public das::Enumeration {
public:
	Enumeration_EObjectInstancingGraphOptions() : das::Enumeration("EObjectInstancingGraphOptions") {
		external = true;
		cppName = "EObjectInstancingGraphOptions";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("None", "None", int64_t(EObjectInstancingGraphOptions::None), das::LineInfo());
		addIEx("DisableInstancing", "DisableInstancing", int64_t(EObjectInstancingGraphOptions::DisableInstancing), das::LineInfo());
		addIEx("InstanceTemplatesOnly", "InstanceTemplatesOnly", int64_t(EObjectInstancingGraphOptions::InstanceTemplatesOnly), das::LineInfo());
	}
};

