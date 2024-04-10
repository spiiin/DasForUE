// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_11() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3058:31
	makeExtern< bool (*)(const UClass &) , _dasUnreal_virtual_302_IsStructTrashed , SimNode_ExtFuncCall >(lib,"IsStructTrashed","_dasUnreal_virtual_302_IsStructTrashed")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_830 = das::das_call_member< const ICppClassTypeInfo * (UClass::*)() const,&UClass::GetCppTypeInfo >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3178:27
	makeExtern<DAS_CALL_METHOD(_method_830), SimNode_ExtFuncCall >(lib,"GetCppTypeInfo","das_call_member< const ICppClassTypeInfo * (UClass::*)() const , &UClass::GetCppTypeInfo >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_831 = das::das_call_member< void (UClass::*)(const FCppClassTypeInfoStatic *),&UClass::SetCppTypeInfoStatic >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3185:7
	makeExtern<DAS_CALL_METHOD(_method_831), SimNode_ExtFuncCall >(lib,"SetCppTypeInfoStatic","das_call_member< void (UClass::*)(const FCppClassTypeInfoStatic *) , &UClass::SetCppTypeInfoStatic >::invoke")
		->args({"self","InCppTypeInfoStatic"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_832 = das::das_call_member< const FString (UClass::*)() const,&UClass::GetConfigName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3201:32
	makeExtern<DAS_CALL_METHOD(_method_832), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetConfigName","das_call_member< const FString (UClass::*)() const , &UClass::GetConfigName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_833 = das::das_call_member< UClass * (UClass::*)() const,&UClass::GetSuperClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3204:10
	makeExtern<DAS_CALL_METHOD(_method_833), SimNode_ExtFuncCall >(lib,"GetSuperClass","das_call_member< UClass * (UClass::*)() const , &UClass::GetSuperClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_834 = das::das_call_member< int (UClass::*)(),&UClass::GetDefaultsCount >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3213:8
	makeExtern<DAS_CALL_METHOD(_method_834), SimNode_ExtFuncCall >(lib,"GetDefaultsCount","das_call_member< int (UClass::*)() , &UClass::GetDefaultsCount >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_835 = das::das_call_member< UObject * (UClass::*)(bool) const,&UClass::GetDefaultObject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3223:11
	makeExtern<DAS_CALL_METHOD(_method_835), SimNode_ExtFuncCall >(lib,"GetDefaultObject","das_call_member< UObject * (UClass::*)(bool) const , &UClass::GetDefaultObject >::invoke")
		->args({"self","bCreateIfNeeded"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3241:15
	makeExtern< void (*)(UClass &,UObject *) , _dasUnreal_virtual_303_PostLoadInstance , SimNode_ExtFuncCall >(lib,"PostLoadInstance","_dasUnreal_virtual_303_PostLoadInstance")
		->args({"self","InObj"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3250:15
	makeExtern< void (*)(UClass &,unsigned char *,const unsigned char *) , _dasUnreal_virtual_304_InitPropertiesFromCustomList , SimNode_ExtFuncCall >(lib,"InitPropertiesFromCustomList","_dasUnreal_virtual_304_InitPropertiesFromCustomList")
		->args({"self","DataPtr","DefaultDataPtr"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3260:15
	makeExtern< bool (*)(const UClass &) , _dasUnreal_virtual_305_CanCreateAssetOfClass , SimNode_ExtFuncCall >(lib,"CanCreateAssetOfClass","_dasUnreal_virtual_305_CanCreateAssetOfClass")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_839 = das::das_call_member< FName (UClass::*)() const,&UClass::GetDefaultObjectName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3269:24
	makeExtern<DAS_CALL_METHOD(_method_839), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDefaultObjectName","das_call_member< FName (UClass::*)() const , &UClass::GetDefaultObjectName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3272:17
	makeExtern< unsigned char * (*)(const UClass &,UObject *,UFunction *) , _dasUnreal_virtual_306_GetPersistentUberGraphFrame , SimNode_ExtFuncCall >(lib,"GetPersistentUberGraphFrame","_dasUnreal_virtual_306_GetPersistentUberGraphFrame")
		->args({"self","Obj","FuncToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3278:15
	makeExtern< void (*)(const UClass &,UObject *,bool,bool,UClass *) , _dasUnreal_virtual_307_CreatePersistentUberGraphFrame , SimNode_ExtFuncCall >(lib,"CreatePersistentUberGraphFrame","_dasUnreal_virtual_307_CreatePersistentUberGraphFrame")
		->args({"self","Obj","bCreateOnlyIfEmpty","bSkipSuperClass","OldClass"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->arg_init(3,make_smart<ExprConstBool>(false))
		->arg_init(4,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3283:15
	makeExtern< void (*)(const UClass &,UObject *,bool) , _dasUnreal_virtual_308_DestroyPersistentUberGraphFrame , SimNode_ExtFuncCall >(lib,"DestroyPersistentUberGraphFrame","_dasUnreal_virtual_308_DestroyPersistentUberGraphFrame")
		->args({"self","Obj","bSkipSuperClass"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_843 = das::das_call_member< UObject * (UClass::*)(FName),&UClass::GetDefaultSubobjectByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3300:27
	makeExtern<DAS_CALL_METHOD(_method_843), SimNode_ExtFuncCall >(lib,"GetDefaultSubobjectByName","das_call_member< UObject * (UClass::*)(FName) , &UClass::GetDefaultSubobjectByName >::invoke")
		->args({"self","ToFind"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_844 = das::das_call_member< void (UClass::*)(UObject *,const UClass *),&UClass::AddDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3303:7
	makeExtern<DAS_CALL_METHOD(_method_844), SimNode_ExtFuncCall >(lib,"AddDefaultSubobject","das_call_member< void (UClass::*)(UObject *,const UClass *) , &UClass::AddDefaultSubobject >::invoke")
		->args({"self","NewSubobject","BaseClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_845 = das::das_call_member< bool (UClass::*)(EClassFlags) const,&UClass::HasAnyClassFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3326:19
	makeExtern<DAS_CALL_METHOD(_method_845), SimNode_ExtFuncCall >(lib,"HasAnyClassFlags","das_call_member< bool (UClass::*)(EClassFlags) const , &UClass::HasAnyClassFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_846 = das::das_call_member< bool (UClass::*)(EClassFlags) const,&UClass::HasAllClassFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3337:19
	makeExtern<DAS_CALL_METHOD(_method_846), SimNode_ExtFuncCall >(lib,"HasAllClassFlags","das_call_member< bool (UClass::*)(EClassFlags) const , &UClass::HasAllClassFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_847 = das::das_call_member< EClassFlags (UClass::*)() const,&UClass::GetClassFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3347:26
	makeExtern<DAS_CALL_METHOD(_method_847), SimNode_ExtFuncCall >(lib,"GetClassFlags","das_call_member< EClassFlags (UClass::*)() const , &UClass::GetClassFlags >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_848 = das::das_call_member< bool (UClass::*)(EClassCastFlags) const,&UClass::HasAnyCastFlag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3360:19
	makeExtern<DAS_CALL_METHOD(_method_848), SimNode_ExtFuncCall >(lib,"HasAnyCastFlag","das_call_member< bool (UClass::*)(EClassCastFlags) const , &UClass::HasAnyCastFlag >::invoke")
		->args({"self","FlagToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_849 = das::das_call_member< bool (UClass::*)(EClassCastFlags) const,&UClass::HasAllCastFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3364:19
	makeExtern<DAS_CALL_METHOD(_method_849), SimNode_ExtFuncCall >(lib,"HasAllCastFlags","das_call_member< bool (UClass::*)(EClassCastFlags) const , &UClass::HasAllCastFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_850 = das::das_call_member< FString (UClass::*)() const,&UClass::GetDescription >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3369:26
	makeExtern<DAS_CALL_METHOD(_method_850), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDescription","das_call_member< FString (UClass::*)() const , &UClass::GetDescription >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_851 = das::das_call_member< void (UClass::*)(bool),&UClass::AssembleReferenceTokenStream >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3377:23
	makeExtern<DAS_CALL_METHOD(_method_851), SimNode_ExtFuncCall >(lib,"AssembleReferenceTokenStream","das_call_member< void (UClass::*)(bool) , &UClass::AssembleReferenceTokenStream >::invoke")
		->args({"self","bForce"})
		->arg_init(1,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_852 = das::das_call_member< bool (UClass::*)(const UClass *) const,&UClass::ImplementsInterface >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3384:23
	makeExtern<DAS_CALL_METHOD(_method_852), SimNode_ExtFuncCall >(lib,"ImplementsInterface","das_call_member< bool (UClass::*)(const UClass *) const , &UClass::ImplementsInterface >::invoke")
		->args({"self","SomeInterface"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3409:15
	makeExtern< void (*)(UClass &,UObject *) , _dasUnreal_virtual_309_PostLoadDefaultObject , SimNode_ExtFuncCall >(lib,"PostLoadDefaultObject","_dasUnreal_virtual_309_PostLoadDefaultObject")
		->args({"self","Object"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3417:31
	makeExtern< void (*)(UClass &,bool) , _dasUnreal_virtual_310_PurgeClass , SimNode_ExtFuncCall >(lib,"PurgeClass","_dasUnreal_virtual_310_PurgeClass")
		->args({"self","bRecompilingOnLoad"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3426:33
	makeExtern< UClass * (*)(const UClass *,UClass *,UClass *) , _dasUnreal_static_311_FindCommonBase , SimNode_ExtFuncCall >(lib,"FindCommonBase","_dasUnreal_static_311_FindCommonBase")
		->args({"self","InClassA","InClassB"})
		->arg_type(0,makeType<TExplicit<const UClass*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3442:31
	makeExtern< bool (*)(const UClass &,FName) , _dasUnreal_virtual_312_IsFunctionImplementedInScript , SimNode_ExtFuncCall >(lib,"IsFunctionImplementedInScript","_dasUnreal_virtual_312_IsFunctionImplementedInScript")
		->args({"self","InFunctionName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3449:31
	makeExtern< bool (*)(const UClass &,FProperty *) , _dasUnreal_virtual_313_HasProperty , SimNode_ExtFuncCall >(lib,"HasProperty","_dasUnreal_virtual_313_HasProperty")
		->args({"self","InProperty"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3452:19
	makeExtern< UObject * (*)(const UClass &,const UClass *,const FName) , _dasUnreal_virtual_314_FindArchetype , SimNode_ExtFuncCall >(lib,"FindArchetype","_dasUnreal_virtual_314_FindArchetype")
		->args({"self","ArchetypeClass","ArchetypeName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3455:35
	makeExtern< UObject * (*)(const UClass &) , _dasUnreal_virtual_315_GetArchetypeForCDO , SimNode_ExtFuncCall >(lib,"GetArchetypeForCDO","_dasUnreal_virtual_315_GetArchetypeForCDO")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_860 = das::das_call_member< const void * (UClass::*)() const,&UClass::GetArchetypeForSparseClassData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3458:30
	makeExtern<DAS_CALL_METHOD(_method_860), SimNode_ExtFuncCall >(lib,"GetArchetypeForSparseClassData","das_call_member< const void * (UClass::*)() const , &UClass::GetArchetypeForSparseClassData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_861 = das::das_call_member< UScriptStruct * (UClass::*)() const,&UClass::GetSparseClassDataArchetypeStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3461:33
	makeExtern<DAS_CALL_METHOD(_method_861), SimNode_ExtFuncCall >(lib,"GetSparseClassDataArchetypeStruct","das_call_member< UScriptStruct * (UClass::*)() const , &UClass::GetSparseClassDataArchetypeStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_862 = das::das_call_member< bool (UClass::*)() const,&UClass::OverridesSparseClassDataArchetype >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3464:23
	makeExtern<DAS_CALL_METHOD(_method_862), SimNode_ExtFuncCall >(lib,"OverridesSparseClassDataArchetype","das_call_member< bool (UClass::*)() const , &UClass::OverridesSparseClassDataArchetype >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_863 = das::das_call_member< void (UClass::*)(),&UClass::SetUpRuntimeReplicationData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3481:23
	makeExtern<DAS_CALL_METHOD(_method_863), SimNode_ExtFuncCall >(lib,"SetUpRuntimeReplicationData","das_call_member< void (UClass::*)() , &UClass::SetUpRuntimeReplicationData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3486:30
	makeExtern< bool (*)(const UClass *,UClass *) , _dasUnreal_static_316_IsSafeToSerializeToStructuredArchives , SimNode_ExtFuncCall >(lib,"IsSafeToSerializeToStructuredArchives","_dasUnreal_static_316_IsSafeToSerializeToStructuredArchives")
		->args({"self","InClass"})
		->arg_type(0,makeType<TExplicit<const UClass*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3610:22
	makeExtern< void (*)(UClass *,UClass *,UClass *,const wchar_t *,const wchar_t *) , InitializePrivateStaticClass , SimNode_ExtFuncCall >(lib,"InitializePrivateStaticClass","InitializePrivateStaticClass")
		->args({"TClass_Super_StaticClass","TClass_PrivateStaticClass","TClass_WithinClass_StaticClass","PackageName","Name"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FSavePackageResultStruct>(*this,lib,"FSavePackageResultStruct","FSavePackageResultStruct");
	addCtorAndUsing<FSavePackageResultStruct,ESavePackageResult>(*this,lib,"FSavePackageResultStruct","FSavePackageResultStruct")
		->args({"InResult"});
	addCtorAndUsing<FSavePackageResultStruct,ESavePackageResult,long long>(*this,lib,"FSavePackageResultStruct","FSavePackageResultStruct")
		->args({"InResult","InTotalFileSize"});
	using _method_865 = das::das_call_member< bool (FSavePackageResultStruct::*)() const,&FSavePackageResultStruct::IsSuccessful >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:129:7
	makeExtern<DAS_CALL_METHOD(_method_865), SimNode_ExtFuncCall >(lib,"IsSuccessful","das_call_member< bool (FSavePackageResultStruct::*)() const , &FSavePackageResultStruct::IsSuccessful >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:144:2
	makeExtern< UClass * (*)(const UPackage *) , _dasUnreal_static_317_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_317_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:144:2
	makeExtern< const wchar_t * (*)(const UPackage *) , _dasUnreal_static_318_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_318_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:144:2
	makeExtern< void (*)(const UPackage *) , _dasUnreal_static_319_StaticRegisterNativesUPackage , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUPackage","_dasUnreal_static_319_StaticRegisterNativesUPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_869 = das::das_call_member< bool (UPackage::*)() const,&UPackage::GetHasBeenEndLoaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:231:7
	makeExtern<DAS_CALL_METHOD(_method_869), SimNode_ExtFuncCall >(lib,"GetHasBeenEndLoaded","das_call_member< bool (UPackage::*)() const , &UPackage::GetHasBeenEndLoaded >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_870 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetHasBeenEndLoaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:235:7
	makeExtern<DAS_CALL_METHOD(_method_870), SimNode_ExtFuncCall >(lib,"SetHasBeenEndLoaded","das_call_member< void (UPackage::*)(bool) , &UPackage::SetHasBeenEndLoaded >::invoke")
		->args({"self","bValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:328:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_320_IsNameStableForNetworking , SimNode_ExtFuncCall >(lib,"IsNameStableForNetworking","_dasUnreal_virtual_320_IsNameStableForNetworking")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:329:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_321_NeedsLoadForClient , SimNode_ExtFuncCall >(lib,"NeedsLoadForClient","_dasUnreal_virtual_321_NeedsLoadForClient")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:330:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_322_NeedsLoadForServer , SimNode_ExtFuncCall >(lib,"NeedsLoadForServer","_dasUnreal_virtual_322_NeedsLoadForServer")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:331:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_323_IsPostLoadThreadSafe , SimNode_ExtFuncCall >(lib,"IsPostLoadThreadSafe","_dasUnreal_virtual_323_IsPostLoadThreadSafe")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:332:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_324_IsDestructionThreadSafe , SimNode_ExtFuncCall >(lib,"IsDestructionThreadSafe","_dasUnreal_virtual_324_IsDestructionThreadSafe")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_876 = das::das_call_member< void (UPackage::*)(bool,bool),&UPackage::SetLoadedByEditorPropertiesOnly >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:336:7
	makeExtern<DAS_CALL_METHOD(_method_876), SimNode_ExtFuncCall >(lib,"SetLoadedByEditorPropertiesOnly","das_call_member< void (UPackage::*)(bool,bool) , &UPackage::SetLoadedByEditorPropertiesOnly >::invoke")
		->args({"self","bIsEditorOnly","bRecursive"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_877 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsLoadedByEditorPropertiesOnly >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:338:7
	makeExtern<DAS_CALL_METHOD(_method_877), SimNode_ExtFuncCall >(lib,"IsLoadedByEditorPropertiesOnly","das_call_member< bool (UPackage::*)() const , &UPackage::IsLoadedByEditorPropertiesOnly >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_878 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetDynamicPIEPackagePending >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:341:7
	makeExtern<DAS_CALL_METHOD(_method_878), SimNode_ExtFuncCall >(lib,"SetDynamicPIEPackagePending","das_call_member< void (UPackage::*)(bool) , &UPackage::SetDynamicPIEPackagePending >::invoke")
		->args({"self","bInIsDynamicPIEPackagePending"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_879 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsDynamicPIEPackagePending >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:343:7
	makeExtern<DAS_CALL_METHOD(_method_879), SimNode_ExtFuncCall >(lib,"IsDynamicPIEPackagePending","das_call_member< bool (UPackage::*)() const , &UPackage::IsDynamicPIEPackagePending >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:350:15
	makeExtern< void (*)(UPackage &) , _dasUnreal_virtual_325_PostInitProperties , SimNode_ExtFuncCall >(lib,"PostInitProperties","_dasUnreal_virtual_325_PostInitProperties")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:352:15
	makeExtern< void (*)(UPackage &) , _dasUnreal_virtual_326_FinishDestroy , SimNode_ExtFuncCall >(lib,"FinishDestroy","_dasUnreal_virtual_326_FinishDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:355:15
	makeExtern< void (*)(UPackage &,FArchive &) , _dasUnreal_virtual_327_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_327_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:358:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_328_IsAsset , SimNode_ExtFuncCall >(lib,"IsAsset","_dasUnreal_virtual_328_IsAsset")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:369:14
	makeExtern< void (*)(const UPackage *,UObject *,FReferenceCollector &) , _dasUnreal_static_329_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_static_329_AddReferencedObjects")
		->args({"self","InThis","Collector"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_885 = das::das_call_member< int (UPackage::*)() const,&UPackage::GetPIEInstanceID >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:417:8
	makeExtern<DAS_CALL_METHOD(_method_885), SimNode_ExtFuncCall >(lib,"GetPIEInstanceID","das_call_member< int (UPackage::*)() const , &UPackage::GetPIEInstanceID >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_886 = das::das_call_member< void (UPackage::*)(int),&UPackage::SetPIEInstanceID >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:430:7
	makeExtern<DAS_CALL_METHOD(_method_886), SimNode_ExtFuncCall >(lib,"SetPIEInstanceID","das_call_member< void (UPackage::*)(int) , &UPackage::SetPIEInstanceID >::invoke")
		->args({"self","InPIEInstanceID"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_887 = das::das_call_member< int (UPackage::*)() const,&UPackage::GetLinkerLicenseeVersion >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:447:8
	makeExtern<DAS_CALL_METHOD(_method_887), SimNode_ExtFuncCall >(lib,"GetLinkerLicenseeVersion","das_call_member< int (UPackage::*)() const , &UPackage::GetLinkerLicenseeVersion >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_888 = das::das_call_member< void (UPackage::*)(float),&UPackage::SetLoadTime >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:461:7
	makeExtern<DAS_CALL_METHOD(_method_888), SimNode_ExtFuncCall >(lib,"SetLoadTime","das_call_member< void (UPackage::*)(float) , &UPackage::SetLoadTime >::invoke")
		->args({"self","InLoadTime"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_889 = das::das_call_member< float (UPackage::*)() const,&UPackage::GetLoadTime >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:473:8
	makeExtern<DAS_CALL_METHOD(_method_889), SimNode_ExtFuncCall >(lib,"GetLoadTime","das_call_member< float (UPackage::*)() const , &UPackage::GetLoadTime >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_890 = das::das_call_member< void (UPackage::*)(),&UPackage::ClearDirtyFlag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:506:7
	makeExtern<DAS_CALL_METHOD(_method_890), SimNode_ExtFuncCall >(lib,"ClearDirtyFlag","das_call_member< void (UPackage::*)() , &UPackage::ClearDirtyFlag >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_891 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetDirtyFlag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:514:7
	makeExtern<DAS_CALL_METHOD(_method_891), SimNode_ExtFuncCall >(lib,"SetDirtyFlag","das_call_member< void (UPackage::*)(bool) , &UPackage::SetDirtyFlag >::invoke")
		->args({"self","bIsDirty"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_892 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsDirty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:521:7
	makeExtern<DAS_CALL_METHOD(_method_892), SimNode_ExtFuncCall >(lib,"IsDirty","das_call_member< bool (UPackage::*)() const , &UPackage::IsDirty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_893 = das::das_call_member< void (UPackage::*)(),&UPackage::MarkAsNewlyCreated >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:530:7
	makeExtern<DAS_CALL_METHOD(_method_893), SimNode_ExtFuncCall >(lib,"MarkAsNewlyCreated","das_call_member< void (UPackage::*)() , &UPackage::MarkAsNewlyCreated >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_894 = das::das_call_member< void (UPackage::*)(),&UPackage::MarkAsUnloaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:540:7
	makeExtern<DAS_CALL_METHOD(_method_894), SimNode_ExtFuncCall >(lib,"MarkAsUnloaded","das_call_member< void (UPackage::*)() , &UPackage::MarkAsUnloaded >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_895 = das::das_call_member< void (UPackage::*)(),&UPackage::MarkAsFullyLoaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:550:7
	makeExtern<DAS_CALL_METHOD(_method_895), SimNode_ExtFuncCall >(lib,"MarkAsFullyLoaded","das_call_member< void (UPackage::*)() , &UPackage::MarkAsFullyLoaded >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_896 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsFullyLoaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:560:7
	makeExtern<DAS_CALL_METHOD(_method_896), SimNode_ExtFuncCall >(lib,"IsFullyLoaded","das_call_member< bool (UPackage::*)() const , &UPackage::IsFullyLoaded >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_897 = das::das_call_member< void (UPackage::*)(),&UPackage::FullyLoad >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:565:7
	makeExtern<DAS_CALL_METHOD(_method_897), SimNode_ExtFuncCall >(lib,"FullyLoad","das_call_member< void (UPackage::*)() , &UPackage::FullyLoad >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_898 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetCanBeImportedFlag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:580:7
	makeExtern<DAS_CALL_METHOD(_method_898), SimNode_ExtFuncCall >(lib,"SetCanBeImportedFlag","das_call_member< void (UPackage::*)(bool) , &UPackage::SetCanBeImportedFlag >::invoke")
		->args({"self","bInCanBeImported"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_899 = das::das_call_member< bool (UPackage::*)() const,&UPackage::CanBeImported >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:590:7
	makeExtern<DAS_CALL_METHOD(_method_899), SimNode_ExtFuncCall >(lib,"CanBeImported","das_call_member< bool (UPackage::*)() const , &UPackage::CanBeImported >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:597:15
	makeExtern< void (*)(UPackage &,EObjectFlags) , _dasUnreal_virtual_330_TagSubobjects , SimNode_ExtFuncCall >(lib,"TagSubobjects","_dasUnreal_virtual_330_TagSubobjects")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_901 = das::das_call_member< void (UPackage::*)(),&UPackage::ThisContainsMap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:602:7
	makeExtern<DAS_CALL_METHOD(_method_901), SimNode_ExtFuncCall >(lib,"ThisContainsMap","das_call_member< void (UPackage::*)() , &UPackage::ThisContainsMap >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_902 = das::das_call_member< bool (UPackage::*)() const,&UPackage::ContainsMap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:612:7
	makeExtern<DAS_CALL_METHOD(_method_902), SimNode_ExtFuncCall >(lib,"ContainsMap","das_call_member< bool (UPackage::*)() const , &UPackage::ContainsMap >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_903 = das::das_call_member< void (UPackage::*)(bool),&UPackage::ThisRequiresLocalizationGather >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:620:7
	makeExtern<DAS_CALL_METHOD(_method_903), SimNode_ExtFuncCall >(lib,"ThisRequiresLocalizationGather","das_call_member< void (UPackage::*)(bool) , &UPackage::ThisRequiresLocalizationGather >::invoke")
		->args({"self","Value"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_904 = das::das_call_member< bool (UPackage::*)() const,&UPackage::RequiresLocalizationGather >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:637:7
	makeExtern<DAS_CALL_METHOD(_method_904), SimNode_ExtFuncCall >(lib,"RequiresLocalizationGather","das_call_member< bool (UPackage::*)() const , &UPackage::RequiresLocalizationGather >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_905 = das::das_call_member< void (UPackage::*)(unsigned int),&UPackage::SetPackageFlagsTo >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:645:7
	makeExtern<DAS_CALL_METHOD(_method_905), SimNode_ExtFuncCall >(lib,"SetPackageFlagsTo","das_call_member< void (UPackage::*)(unsigned int) , &UPackage::SetPackageFlagsTo >::invoke")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_906 = das::das_call_member< void (UPackage::*)(unsigned int),&UPackage::SetPackageFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:663:19
	makeExtern<DAS_CALL_METHOD(_method_906), SimNode_ExtFuncCall >(lib,"SetPackageFlags","das_call_member< void (UPackage::*)(unsigned int) , &UPackage::SetPackageFlags >::invoke")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_907 = das::das_call_member< void (UPackage::*)(unsigned int),&UPackage::ClearPackageFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:673:19
	makeExtern<DAS_CALL_METHOD(_method_907), SimNode_ExtFuncCall >(lib,"ClearPackageFlags","das_call_member< void (UPackage::*)(unsigned int) , &UPackage::ClearPackageFlags >::invoke")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_908 = das::das_call_member< bool (UPackage::*)(unsigned int) const,&UPackage::HasAnyPackageFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:686:19
	makeExtern<DAS_CALL_METHOD(_method_908), SimNode_ExtFuncCall >(lib,"HasAnyPackageFlags","das_call_member< bool (UPackage::*)(unsigned int) const , &UPackage::HasAnyPackageFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_909 = das::das_call_member< bool (UPackage::*)(unsigned int) const,&UPackage::HasAllPackagesFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:697:19
	makeExtern<DAS_CALL_METHOD(_method_909), SimNode_ExtFuncCall >(lib,"HasAllPackagesFlags","das_call_member< bool (UPackage::*)(unsigned int) const , &UPackage::HasAllPackagesFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_910 = das::das_call_member< unsigned int (UPackage::*)() const,&UPackage::GetPackageFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:707:21
	makeExtern<DAS_CALL_METHOD(_method_910), SimNode_ExtFuncCall >(lib,"GetPackageFlags","das_call_member< unsigned int (UPackage::*)() const , &UPackage::GetPackageFlags >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_911 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsExternallyReferenceable >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:715:19
	makeExtern<DAS_CALL_METHOD(_method_911), SimNode_ExtFuncCall >(lib,"IsExternallyReferenceable","das_call_member< bool (UPackage::*)() const , &UPackage::IsExternallyReferenceable >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_912 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetIsExternallyReferenceable >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:725:19
	makeExtern<DAS_CALL_METHOD(_method_912), SimNode_ExtFuncCall >(lib,"SetIsExternallyReferenceable","das_call_member< void (UPackage::*)(bool) , &UPackage::SetIsExternallyReferenceable >::invoke")
		->args({"self","bValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_913 = das::das_call_member< bool (UPackage::*)() const,&UPackage::HasThumbnailMap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:739:7
	makeExtern<DAS_CALL_METHOD(_method_913), SimNode_ExtFuncCall >(lib,"HasThumbnailMap","das_call_member< bool (UPackage::*)() const , &UPackage::HasThumbnailMap >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_914 = das::das_call_member< int64 (UPackage::*)() const,&UPackage::GetFileSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:832:20
	makeExtern<DAS_CALL_METHOD(_method_914), SimNode_ExtFuncCall >(lib,"GetFileSize","das_call_member< int64 (UPackage::*)() const , &UPackage::GetFileSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_915 = das::das_call_member< UObject * (UPackage::*)(EObjectFlags) const,&UPackage::FindAssetInPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:878:11
	makeExtern<DAS_CALL_METHOD(_method_915), SimNode_ExtFuncCall >(lib,"FindAssetInPackage","das_call_member< UObject * (UPackage::*)(EObjectFlags) const , &UPackage::FindAssetInPackage >::invoke")
		->args({"self","RequiredTopLevelFlags"})
		->arg_init(1,make_smart<ExprConstEnumeration>(0,makeType<EObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_916 = das::das_call_member< bool (UPackage::*)() const,&UPackage::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:893:7
	makeExtern<DAS_CALL_METHOD(_method_916), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (UPackage::*)() const , &UPackage::HasMetaData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_917 = das::das_call_member< UMetaData * (UPackage::*)(),&UPackage::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:909:19
	makeExtern<DAS_CALL_METHOD(_method_917), SimNode_ExtFuncCall >(lib,"GetMetaData","das_call_member< UMetaData * (UPackage::*)() , &UPackage::GetMetaData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:950:14
	makeExtern< void (*)(const UPackage *) , _dasUnreal_static_331_WaitForAsyncFileWrites , SimNode_ExtFuncCall >(lib,"WaitForAsyncFileWrites","_dasUnreal_static_331_WaitForAsyncFileWrites")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:953:14
	makeExtern< bool (*)(const UPackage *) , _dasUnreal_static_332_HasAsyncFileWrites , SimNode_ExtFuncCall >(lib,"HasAsyncFileWrites","_dasUnreal_static_332_HasAsyncFileWrites")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:962:14
	makeExtern< bool (*)(const UPackage *,UPackage *,const UObject *) , _dasUnreal_static_333_IsEmptyPackage , SimNode_ExtFuncCall >(lib,"IsEmptyPackage","_dasUnreal_static_333_IsEmptyPackage")
		->args({"self","Package","LastReferencer"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->arg_init(2,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:155:2
	makeExtern< FFieldClass * (*)(const FProperty *) , _dasUnreal_static_334_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_334_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:155:2
	makeExtern< uint64 (*)(const FProperty *) , _dasUnreal_static_335_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_335_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:155:2
	makeExtern< uint64 (*)(const FProperty *) , _dasUnreal_static_336_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_336_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:207:31
	makeExtern< void (*)(FProperty &,FArchive &) , _dasUnreal_virtual_337_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_337_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

