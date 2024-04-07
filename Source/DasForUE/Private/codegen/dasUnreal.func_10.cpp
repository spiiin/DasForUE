// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_10() {
	using _method_738 = das::das_call_member< UFunction * (UClass::*)(FName,EIncludeSuperFlag::Type) const,&UClass::FindFunctionByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3028:29
	makeExtern<DAS_CALL_METHOD(_method_738), SimNode_ExtFuncCall >(lib,"FindFunctionByName","das_call_member< UFunction * (UClass::*)(FName,EIncludeSuperFlag::Type) const , &UClass::FindFunctionByName >::invoke")
		->args({"self","InName","IncludeSuper"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<EIncludeSuperFlag::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3032:31
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_246_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_246_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3033:31
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_247_FinishDestroy , SimNode_ExtFuncCall >(lib,"FinishDestroy","_dasUnreal_virtual_247_FinishDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3034:31
	makeExtern< void (*)(UClass &,UClass *,const wchar_t *,const wchar_t *) , _dasUnreal_virtual_248_DeferredRegister , SimNode_ExtFuncCall >(lib,"DeferredRegister","_dasUnreal_virtual_248_DeferredRegister")
		->args({"self","UClassStaticClass","PackageName","InName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3035:31
	makeExtern< bool (*)(UClass &,const wchar_t *,UObject *,unsigned int) , _dasUnreal_virtual_249_Rename , SimNode_ExtFuncCall >(lib,"Rename","_dasUnreal_virtual_249_Rename")
		->args({"self","NewName","NewOuter","Flags"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->arg_init(2,make_smart<ExprConstPtr>())
		->arg_init(3,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3036:31
	makeExtern< void (*)(UClass &,EObjectFlags) , _dasUnreal_virtual_250_TagSubobjects , SimNode_ExtFuncCall >(lib,"TagSubobjects","_dasUnreal_virtual_250_TagSubobjects")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3037:31
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_251_PostInitProperties , SimNode_ExtFuncCall >(lib,"PostInitProperties","_dasUnreal_virtual_251_PostInitProperties")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3039:55
	makeExtern< FRestoreForUObjectOverwrite * (*)(UClass &) , _dasUnreal_virtual_252_GetRestoreForUObjectOverwrite , SimNode_ExtFuncCall >(lib,"GetRestoreForUObjectOverwrite","_dasUnreal_virtual_252_GetRestoreForUObjectOverwrite")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3040:34
	makeExtern< FString (*)(UClass &) , _dasUnreal_virtual_253_GetDesc , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDesc","_dasUnreal_virtual_253_GetDesc")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3045:15
	makeExtern< bool (*)(const UClass &) , _dasUnreal_virtual_254_IsAsset , SimNode_ExtFuncCall >(lib,"IsAsset","_dasUnreal_virtual_254_IsAsset")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3046:15
	makeExtern< bool (*)(const UClass &) , _dasUnreal_virtual_255_IsNameStableForNetworking , SimNode_ExtFuncCall >(lib,"IsNameStableForNetworking","_dasUnreal_virtual_255_IsNameStableForNetworking")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3051:31
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_256_Bind , SimNode_ExtFuncCall >(lib,"Bind","_dasUnreal_virtual_256_Bind")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3052:39
	makeExtern< const wchar_t * (*)(const UClass &) , _dasUnreal_virtual_257_GetPrefixCPP , SimNode_ExtFuncCall >(lib,"GetPrefixCPP","_dasUnreal_virtual_257_GetPrefixCPP")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3057:31
	makeExtern< void (*)(UClass &,UStruct *) , _dasUnreal_virtual_258_SetSuperStruct , SimNode_ExtFuncCall >(lib,"SetSuperStruct","_dasUnreal_virtual_258_SetSuperStruct")
		->args({"self","NewSuperStruct"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3058:31
	makeExtern< bool (*)(const UClass &) , _dasUnreal_virtual_259_IsStructTrashed , SimNode_ExtFuncCall >(lib,"IsStructTrashed","_dasUnreal_virtual_259_IsStructTrashed")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_753 = das::das_call_member< const ICppClassTypeInfo * (UClass::*)() const,&UClass::GetCppTypeInfo >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3178:27
	makeExtern<DAS_CALL_METHOD(_method_753), SimNode_ExtFuncCall >(lib,"GetCppTypeInfo","das_call_member< const ICppClassTypeInfo * (UClass::*)() const , &UClass::GetCppTypeInfo >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_754 = das::das_call_member< void (UClass::*)(const FCppClassTypeInfoStatic *),&UClass::SetCppTypeInfoStatic >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3185:7
	makeExtern<DAS_CALL_METHOD(_method_754), SimNode_ExtFuncCall >(lib,"SetCppTypeInfoStatic","das_call_member< void (UClass::*)(const FCppClassTypeInfoStatic *) , &UClass::SetCppTypeInfoStatic >::invoke")
		->args({"self","InCppTypeInfoStatic"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_755 = das::das_call_member< const FString (UClass::*)() const,&UClass::GetConfigName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3201:32
	makeExtern<DAS_CALL_METHOD(_method_755), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetConfigName","das_call_member< const FString (UClass::*)() const , &UClass::GetConfigName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_756 = das::das_call_member< UClass * (UClass::*)() const,&UClass::GetSuperClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3204:10
	makeExtern<DAS_CALL_METHOD(_method_756), SimNode_ExtFuncCall >(lib,"GetSuperClass","das_call_member< UClass * (UClass::*)() const , &UClass::GetSuperClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_757 = das::das_call_member< int (UClass::*)(),&UClass::GetDefaultsCount >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3213:8
	makeExtern<DAS_CALL_METHOD(_method_757), SimNode_ExtFuncCall >(lib,"GetDefaultsCount","das_call_member< int (UClass::*)() , &UClass::GetDefaultsCount >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_758 = das::das_call_member< UObject * (UClass::*)(bool) const,&UClass::GetDefaultObject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3223:11
	makeExtern<DAS_CALL_METHOD(_method_758), SimNode_ExtFuncCall >(lib,"GetDefaultObject","das_call_member< UObject * (UClass::*)(bool) const , &UClass::GetDefaultObject >::invoke")
		->args({"self","bCreateIfNeeded"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3241:15
	makeExtern< void (*)(UClass &,UObject *) , _dasUnreal_virtual_260_PostLoadInstance , SimNode_ExtFuncCall >(lib,"PostLoadInstance","_dasUnreal_virtual_260_PostLoadInstance")
		->args({"self","InObj"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3250:15
	makeExtern< void (*)(UClass &,unsigned char *,const unsigned char *) , _dasUnreal_virtual_261_InitPropertiesFromCustomList , SimNode_ExtFuncCall >(lib,"InitPropertiesFromCustomList","_dasUnreal_virtual_261_InitPropertiesFromCustomList")
		->args({"self","DataPtr","DefaultDataPtr"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3260:15
	makeExtern< bool (*)(const UClass &) , _dasUnreal_virtual_262_CanCreateAssetOfClass , SimNode_ExtFuncCall >(lib,"CanCreateAssetOfClass","_dasUnreal_virtual_262_CanCreateAssetOfClass")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_762 = das::das_call_member< FName (UClass::*)() const,&UClass::GetDefaultObjectName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3269:24
	makeExtern<DAS_CALL_METHOD(_method_762), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDefaultObjectName","das_call_member< FName (UClass::*)() const , &UClass::GetDefaultObjectName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3272:17
	makeExtern< unsigned char * (*)(const UClass &,UObject *,UFunction *) , _dasUnreal_virtual_263_GetPersistentUberGraphFrame , SimNode_ExtFuncCall >(lib,"GetPersistentUberGraphFrame","_dasUnreal_virtual_263_GetPersistentUberGraphFrame")
		->args({"self","Obj","FuncToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3278:15
	makeExtern< void (*)(const UClass &,UObject *,bool,bool,UClass *) , _dasUnreal_virtual_264_CreatePersistentUberGraphFrame , SimNode_ExtFuncCall >(lib,"CreatePersistentUberGraphFrame","_dasUnreal_virtual_264_CreatePersistentUberGraphFrame")
		->args({"self","Obj","bCreateOnlyIfEmpty","bSkipSuperClass","OldClass"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->arg_init(3,make_smart<ExprConstBool>(false))
		->arg_init(4,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3283:15
	makeExtern< void (*)(const UClass &,UObject *,bool) , _dasUnreal_virtual_265_DestroyPersistentUberGraphFrame , SimNode_ExtFuncCall >(lib,"DestroyPersistentUberGraphFrame","_dasUnreal_virtual_265_DestroyPersistentUberGraphFrame")
		->args({"self","Obj","bSkipSuperClass"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_766 = das::das_call_member< UObject * (UClass::*)(FName),&UClass::GetDefaultSubobjectByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3300:27
	makeExtern<DAS_CALL_METHOD(_method_766), SimNode_ExtFuncCall >(lib,"GetDefaultSubobjectByName","das_call_member< UObject * (UClass::*)(FName) , &UClass::GetDefaultSubobjectByName >::invoke")
		->args({"self","ToFind"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_767 = das::das_call_member< void (UClass::*)(UObject *,const UClass *),&UClass::AddDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3303:7
	makeExtern<DAS_CALL_METHOD(_method_767), SimNode_ExtFuncCall >(lib,"AddDefaultSubobject","das_call_member< void (UClass::*)(UObject *,const UClass *) , &UClass::AddDefaultSubobject >::invoke")
		->args({"self","NewSubobject","BaseClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_768 = das::das_call_member< FString (UClass::*)() const,&UClass::GetDescription >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3369:26
	makeExtern<DAS_CALL_METHOD(_method_768), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDescription","das_call_member< FString (UClass::*)() const , &UClass::GetDescription >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_769 = das::das_call_member< void (UClass::*)(bool),&UClass::AssembleReferenceTokenStream >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3377:23
	makeExtern<DAS_CALL_METHOD(_method_769), SimNode_ExtFuncCall >(lib,"AssembleReferenceTokenStream","das_call_member< void (UClass::*)(bool) , &UClass::AssembleReferenceTokenStream >::invoke")
		->args({"self","bForce"})
		->arg_init(1,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_770 = das::das_call_member< bool (UClass::*)(const UClass *) const,&UClass::ImplementsInterface >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3384:23
	makeExtern<DAS_CALL_METHOD(_method_770), SimNode_ExtFuncCall >(lib,"ImplementsInterface","das_call_member< bool (UClass::*)(const UClass *) const , &UClass::ImplementsInterface >::invoke")
		->args({"self","SomeInterface"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3409:15
	makeExtern< void (*)(UClass &,UObject *) , _dasUnreal_virtual_266_PostLoadDefaultObject , SimNode_ExtFuncCall >(lib,"PostLoadDefaultObject","_dasUnreal_virtual_266_PostLoadDefaultObject")
		->args({"self","Object"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3417:31
	makeExtern< void (*)(UClass &,bool) , _dasUnreal_virtual_267_PurgeClass , SimNode_ExtFuncCall >(lib,"PurgeClass","_dasUnreal_virtual_267_PurgeClass")
		->args({"self","bRecompilingOnLoad"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3426:33
	makeExtern< UClass * (*)(const UClass *,UClass *,UClass *) , _dasUnreal_static_268_FindCommonBase , SimNode_ExtFuncCall >(lib,"FindCommonBase","_dasUnreal_static_268_FindCommonBase")
		->args({"self","InClassA","InClassB"})
		->arg_type(0,makeType<TExplicit<const UClass*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3442:31
	makeExtern< bool (*)(const UClass &,FName) , _dasUnreal_virtual_269_IsFunctionImplementedInScript , SimNode_ExtFuncCall >(lib,"IsFunctionImplementedInScript","_dasUnreal_virtual_269_IsFunctionImplementedInScript")
		->args({"self","InFunctionName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3449:31
	makeExtern< bool (*)(const UClass &,FProperty *) , _dasUnreal_virtual_270_HasProperty , SimNode_ExtFuncCall >(lib,"HasProperty","_dasUnreal_virtual_270_HasProperty")
		->args({"self","InProperty"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3452:19
	makeExtern< UObject * (*)(const UClass &,const UClass *,const FName) , _dasUnreal_virtual_271_FindArchetype , SimNode_ExtFuncCall >(lib,"FindArchetype","_dasUnreal_virtual_271_FindArchetype")
		->args({"self","ArchetypeClass","ArchetypeName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3455:35
	makeExtern< UObject * (*)(const UClass &) , _dasUnreal_virtual_272_GetArchetypeForCDO , SimNode_ExtFuncCall >(lib,"GetArchetypeForCDO","_dasUnreal_virtual_272_GetArchetypeForCDO")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_778 = das::das_call_member< const void * (UClass::*)() const,&UClass::GetArchetypeForSparseClassData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3458:30
	makeExtern<DAS_CALL_METHOD(_method_778), SimNode_ExtFuncCall >(lib,"GetArchetypeForSparseClassData","das_call_member< const void * (UClass::*)() const , &UClass::GetArchetypeForSparseClassData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_779 = das::das_call_member< UScriptStruct * (UClass::*)() const,&UClass::GetSparseClassDataArchetypeStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3461:33
	makeExtern<DAS_CALL_METHOD(_method_779), SimNode_ExtFuncCall >(lib,"GetSparseClassDataArchetypeStruct","das_call_member< UScriptStruct * (UClass::*)() const , &UClass::GetSparseClassDataArchetypeStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_780 = das::das_call_member< bool (UClass::*)() const,&UClass::OverridesSparseClassDataArchetype >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3464:23
	makeExtern<DAS_CALL_METHOD(_method_780), SimNode_ExtFuncCall >(lib,"OverridesSparseClassDataArchetype","das_call_member< bool (UClass::*)() const , &UClass::OverridesSparseClassDataArchetype >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_781 = das::das_call_member< void (UClass::*)(),&UClass::SetUpRuntimeReplicationData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3481:23
	makeExtern<DAS_CALL_METHOD(_method_781), SimNode_ExtFuncCall >(lib,"SetUpRuntimeReplicationData","das_call_member< void (UClass::*)() , &UClass::SetUpRuntimeReplicationData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3486:30
	makeExtern< bool (*)(const UClass *,UClass *) , _dasUnreal_static_273_IsSafeToSerializeToStructuredArchives , SimNode_ExtFuncCall >(lib,"IsSafeToSerializeToStructuredArchives","_dasUnreal_static_273_IsSafeToSerializeToStructuredArchives")
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
	using _method_783 = das::das_call_member< bool (FSavePackageResultStruct::*)() const,&FSavePackageResultStruct::IsSuccessful >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:129:7
	makeExtern<DAS_CALL_METHOD(_method_783), SimNode_ExtFuncCall >(lib,"IsSuccessful","das_call_member< bool (FSavePackageResultStruct::*)() const , &FSavePackageResultStruct::IsSuccessful >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:144:2
	makeExtern< UClass * (*)(const UPackage *) , _dasUnreal_static_274_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_274_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:144:2
	makeExtern< const wchar_t * (*)(const UPackage *) , _dasUnreal_static_275_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_275_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:144:2
	makeExtern< void (*)(const UPackage *) , _dasUnreal_static_276_StaticRegisterNativesUPackage , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUPackage","_dasUnreal_static_276_StaticRegisterNativesUPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_787 = das::das_call_member< bool (UPackage::*)() const,&UPackage::GetHasBeenEndLoaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:231:7
	makeExtern<DAS_CALL_METHOD(_method_787), SimNode_ExtFuncCall >(lib,"GetHasBeenEndLoaded","das_call_member< bool (UPackage::*)() const , &UPackage::GetHasBeenEndLoaded >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_788 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetHasBeenEndLoaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:235:7
	makeExtern<DAS_CALL_METHOD(_method_788), SimNode_ExtFuncCall >(lib,"SetHasBeenEndLoaded","das_call_member< void (UPackage::*)(bool) , &UPackage::SetHasBeenEndLoaded >::invoke")
		->args({"self","bValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:328:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_277_IsNameStableForNetworking , SimNode_ExtFuncCall >(lib,"IsNameStableForNetworking","_dasUnreal_virtual_277_IsNameStableForNetworking")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:329:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_278_NeedsLoadForClient , SimNode_ExtFuncCall >(lib,"NeedsLoadForClient","_dasUnreal_virtual_278_NeedsLoadForClient")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:330:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_279_NeedsLoadForServer , SimNode_ExtFuncCall >(lib,"NeedsLoadForServer","_dasUnreal_virtual_279_NeedsLoadForServer")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:331:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_280_IsPostLoadThreadSafe , SimNode_ExtFuncCall >(lib,"IsPostLoadThreadSafe","_dasUnreal_virtual_280_IsPostLoadThreadSafe")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:332:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_281_IsDestructionThreadSafe , SimNode_ExtFuncCall >(lib,"IsDestructionThreadSafe","_dasUnreal_virtual_281_IsDestructionThreadSafe")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_794 = das::das_call_member< void (UPackage::*)(bool,bool),&UPackage::SetLoadedByEditorPropertiesOnly >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:336:7
	makeExtern<DAS_CALL_METHOD(_method_794), SimNode_ExtFuncCall >(lib,"SetLoadedByEditorPropertiesOnly","das_call_member< void (UPackage::*)(bool,bool) , &UPackage::SetLoadedByEditorPropertiesOnly >::invoke")
		->args({"self","bIsEditorOnly","bRecursive"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_795 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsLoadedByEditorPropertiesOnly >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:338:7
	makeExtern<DAS_CALL_METHOD(_method_795), SimNode_ExtFuncCall >(lib,"IsLoadedByEditorPropertiesOnly","das_call_member< bool (UPackage::*)() const , &UPackage::IsLoadedByEditorPropertiesOnly >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_796 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetDynamicPIEPackagePending >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:341:7
	makeExtern<DAS_CALL_METHOD(_method_796), SimNode_ExtFuncCall >(lib,"SetDynamicPIEPackagePending","das_call_member< void (UPackage::*)(bool) , &UPackage::SetDynamicPIEPackagePending >::invoke")
		->args({"self","bInIsDynamicPIEPackagePending"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_797 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsDynamicPIEPackagePending >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:343:7
	makeExtern<DAS_CALL_METHOD(_method_797), SimNode_ExtFuncCall >(lib,"IsDynamicPIEPackagePending","das_call_member< bool (UPackage::*)() const , &UPackage::IsDynamicPIEPackagePending >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:350:15
	makeExtern< void (*)(UPackage &) , _dasUnreal_virtual_282_PostInitProperties , SimNode_ExtFuncCall >(lib,"PostInitProperties","_dasUnreal_virtual_282_PostInitProperties")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:352:15
	makeExtern< void (*)(UPackage &) , _dasUnreal_virtual_283_FinishDestroy , SimNode_ExtFuncCall >(lib,"FinishDestroy","_dasUnreal_virtual_283_FinishDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:358:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_284_IsAsset , SimNode_ExtFuncCall >(lib,"IsAsset","_dasUnreal_virtual_284_IsAsset")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_801 = das::das_call_member< int (UPackage::*)() const,&UPackage::GetPIEInstanceID >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:417:8
	makeExtern<DAS_CALL_METHOD(_method_801), SimNode_ExtFuncCall >(lib,"GetPIEInstanceID","das_call_member< int (UPackage::*)() const , &UPackage::GetPIEInstanceID >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_802 = das::das_call_member< void (UPackage::*)(int),&UPackage::SetPIEInstanceID >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:430:7
	makeExtern<DAS_CALL_METHOD(_method_802), SimNode_ExtFuncCall >(lib,"SetPIEInstanceID","das_call_member< void (UPackage::*)(int) , &UPackage::SetPIEInstanceID >::invoke")
		->args({"self","InPIEInstanceID"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_803 = das::das_call_member< int (UPackage::*)() const,&UPackage::GetLinkerLicenseeVersion >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:447:8
	makeExtern<DAS_CALL_METHOD(_method_803), SimNode_ExtFuncCall >(lib,"GetLinkerLicenseeVersion","das_call_member< int (UPackage::*)() const , &UPackage::GetLinkerLicenseeVersion >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_804 = das::das_call_member< void (UPackage::*)(float),&UPackage::SetLoadTime >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:461:7
	makeExtern<DAS_CALL_METHOD(_method_804), SimNode_ExtFuncCall >(lib,"SetLoadTime","das_call_member< void (UPackage::*)(float) , &UPackage::SetLoadTime >::invoke")
		->args({"self","InLoadTime"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_805 = das::das_call_member< float (UPackage::*)() const,&UPackage::GetLoadTime >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:473:8
	makeExtern<DAS_CALL_METHOD(_method_805), SimNode_ExtFuncCall >(lib,"GetLoadTime","das_call_member< float (UPackage::*)() const , &UPackage::GetLoadTime >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_806 = das::das_call_member< FName (UPackage::*)() const,&UPackage::GetFolderName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:488:8
	makeExtern<DAS_CALL_METHOD(_method_806), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFolderName","das_call_member< FName (UPackage::*)() const , &UPackage::GetFolderName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_807 = das::das_call_member< void (UPackage::*)(FName),&UPackage::SetFolderName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:497:7
	makeExtern<DAS_CALL_METHOD(_method_807), SimNode_ExtFuncCall >(lib,"SetFolderName","das_call_member< void (UPackage::*)(FName) , &UPackage::SetFolderName >::invoke")
		->args({"self","name"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_808 = das::das_call_member< void (UPackage::*)(),&UPackage::ClearDirtyFlag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:506:7
	makeExtern<DAS_CALL_METHOD(_method_808), SimNode_ExtFuncCall >(lib,"ClearDirtyFlag","das_call_member< void (UPackage::*)() , &UPackage::ClearDirtyFlag >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_809 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetDirtyFlag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:514:7
	makeExtern<DAS_CALL_METHOD(_method_809), SimNode_ExtFuncCall >(lib,"SetDirtyFlag","das_call_member< void (UPackage::*)(bool) , &UPackage::SetDirtyFlag >::invoke")
		->args({"self","bIsDirty"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_810 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsDirty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:521:7
	makeExtern<DAS_CALL_METHOD(_method_810), SimNode_ExtFuncCall >(lib,"IsDirty","das_call_member< bool (UPackage::*)() const , &UPackage::IsDirty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_811 = das::das_call_member< void (UPackage::*)(),&UPackage::MarkAsNewlyCreated >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:530:7
	makeExtern<DAS_CALL_METHOD(_method_811), SimNode_ExtFuncCall >(lib,"MarkAsNewlyCreated","das_call_member< void (UPackage::*)() , &UPackage::MarkAsNewlyCreated >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_812 = das::das_call_member< void (UPackage::*)(),&UPackage::MarkAsUnloaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:540:7
	makeExtern<DAS_CALL_METHOD(_method_812), SimNode_ExtFuncCall >(lib,"MarkAsUnloaded","das_call_member< void (UPackage::*)() , &UPackage::MarkAsUnloaded >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_813 = das::das_call_member< void (UPackage::*)(),&UPackage::MarkAsFullyLoaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:550:7
	makeExtern<DAS_CALL_METHOD(_method_813), SimNode_ExtFuncCall >(lib,"MarkAsFullyLoaded","das_call_member< void (UPackage::*)() , &UPackage::MarkAsFullyLoaded >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_814 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsFullyLoaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:560:7
	makeExtern<DAS_CALL_METHOD(_method_814), SimNode_ExtFuncCall >(lib,"IsFullyLoaded","das_call_member< bool (UPackage::*)() const , &UPackage::IsFullyLoaded >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_815 = das::das_call_member< void (UPackage::*)(),&UPackage::FullyLoad >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:565:7
	makeExtern<DAS_CALL_METHOD(_method_815), SimNode_ExtFuncCall >(lib,"FullyLoad","das_call_member< void (UPackage::*)() , &UPackage::FullyLoad >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_816 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetCanBeImportedFlag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:580:7
	makeExtern<DAS_CALL_METHOD(_method_816), SimNode_ExtFuncCall >(lib,"SetCanBeImportedFlag","das_call_member< void (UPackage::*)(bool) , &UPackage::SetCanBeImportedFlag >::invoke")
		->args({"self","bInCanBeImported"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_817 = das::das_call_member< bool (UPackage::*)() const,&UPackage::CanBeImported >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:590:7
	makeExtern<DAS_CALL_METHOD(_method_817), SimNode_ExtFuncCall >(lib,"CanBeImported","das_call_member< bool (UPackage::*)() const , &UPackage::CanBeImported >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:597:15
	makeExtern< void (*)(UPackage &,EObjectFlags) , _dasUnreal_virtual_285_TagSubobjects , SimNode_ExtFuncCall >(lib,"TagSubobjects","_dasUnreal_virtual_285_TagSubobjects")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_819 = das::das_call_member< void (UPackage::*)(),&UPackage::ThisContainsMap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:602:7
	makeExtern<DAS_CALL_METHOD(_method_819), SimNode_ExtFuncCall >(lib,"ThisContainsMap","das_call_member< void (UPackage::*)() , &UPackage::ThisContainsMap >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_820 = das::das_call_member< bool (UPackage::*)() const,&UPackage::ContainsMap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:612:7
	makeExtern<DAS_CALL_METHOD(_method_820), SimNode_ExtFuncCall >(lib,"ContainsMap","das_call_member< bool (UPackage::*)() const , &UPackage::ContainsMap >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_821 = das::das_call_member< void (UPackage::*)(bool),&UPackage::ThisRequiresLocalizationGather >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:620:7
	makeExtern<DAS_CALL_METHOD(_method_821), SimNode_ExtFuncCall >(lib,"ThisRequiresLocalizationGather","das_call_member< void (UPackage::*)(bool) , &UPackage::ThisRequiresLocalizationGather >::invoke")
		->args({"self","Value"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_822 = das::das_call_member< bool (UPackage::*)() const,&UPackage::RequiresLocalizationGather >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:637:7
	makeExtern<DAS_CALL_METHOD(_method_822), SimNode_ExtFuncCall >(lib,"RequiresLocalizationGather","das_call_member< bool (UPackage::*)() const , &UPackage::RequiresLocalizationGather >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_823 = das::das_call_member< void (UPackage::*)(unsigned int),&UPackage::SetPackageFlagsTo >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:645:7
	makeExtern<DAS_CALL_METHOD(_method_823), SimNode_ExtFuncCall >(lib,"SetPackageFlagsTo","das_call_member< void (UPackage::*)(unsigned int) , &UPackage::SetPackageFlagsTo >::invoke")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_824 = das::das_call_member< void (UPackage::*)(unsigned int),&UPackage::SetPackageFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:663:19
	makeExtern<DAS_CALL_METHOD(_method_824), SimNode_ExtFuncCall >(lib,"SetPackageFlags","das_call_member< void (UPackage::*)(unsigned int) , &UPackage::SetPackageFlags >::invoke")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_825 = das::das_call_member< void (UPackage::*)(unsigned int),&UPackage::ClearPackageFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:673:19
	makeExtern<DAS_CALL_METHOD(_method_825), SimNode_ExtFuncCall >(lib,"ClearPackageFlags","das_call_member< void (UPackage::*)(unsigned int) , &UPackage::ClearPackageFlags >::invoke")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_826 = das::das_call_member< bool (UPackage::*)(unsigned int) const,&UPackage::HasAnyPackageFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:686:19
	makeExtern<DAS_CALL_METHOD(_method_826), SimNode_ExtFuncCall >(lib,"HasAnyPackageFlags","das_call_member< bool (UPackage::*)(unsigned int) const , &UPackage::HasAnyPackageFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_827 = das::das_call_member< bool (UPackage::*)(unsigned int) const,&UPackage::HasAllPackagesFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:697:19
	makeExtern<DAS_CALL_METHOD(_method_827), SimNode_ExtFuncCall >(lib,"HasAllPackagesFlags","das_call_member< bool (UPackage::*)(unsigned int) const , &UPackage::HasAllPackagesFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_828 = das::das_call_member< unsigned int (UPackage::*)() const,&UPackage::GetPackageFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:707:21
	makeExtern<DAS_CALL_METHOD(_method_828), SimNode_ExtFuncCall >(lib,"GetPackageFlags","das_call_member< unsigned int (UPackage::*)() const , &UPackage::GetPackageFlags >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_829 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsExternallyReferenceable >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:715:19
	makeExtern<DAS_CALL_METHOD(_method_829), SimNode_ExtFuncCall >(lib,"IsExternallyReferenceable","das_call_member< bool (UPackage::*)() const , &UPackage::IsExternallyReferenceable >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_830 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetIsExternallyReferenceable >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:725:19
	makeExtern<DAS_CALL_METHOD(_method_830), SimNode_ExtFuncCall >(lib,"SetIsExternallyReferenceable","das_call_member< void (UPackage::*)(bool) , &UPackage::SetIsExternallyReferenceable >::invoke")
		->args({"self","bValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_831 = das::das_call_member< bool (UPackage::*)() const,&UPackage::HasThumbnailMap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:739:7
	makeExtern<DAS_CALL_METHOD(_method_831), SimNode_ExtFuncCall >(lib,"HasThumbnailMap","das_call_member< bool (UPackage::*)() const , &UPackage::HasThumbnailMap >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_832 = das::das_call_member< int64 (UPackage::*)() const,&UPackage::GetFileSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:832:20
	makeExtern<DAS_CALL_METHOD(_method_832), SimNode_ExtFuncCall >(lib,"GetFileSize","das_call_member< int64 (UPackage::*)() const , &UPackage::GetFileSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_833 = das::das_call_member< UObject * (UPackage::*)(EObjectFlags) const,&UPackage::FindAssetInPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:878:11
	makeExtern<DAS_CALL_METHOD(_method_833), SimNode_ExtFuncCall >(lib,"FindAssetInPackage","das_call_member< UObject * (UPackage::*)(EObjectFlags) const , &UPackage::FindAssetInPackage >::invoke")
		->args({"self","RequiredTopLevelFlags"})
		->arg_init(1,make_smart<ExprConstEnumeration>(0,makeType<EObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
}
}

