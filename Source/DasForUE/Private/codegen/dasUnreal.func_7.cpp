// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_7() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2728:2
	makeExtern< UClass * (*)(const UClass *) , _dasUnreal_static_150_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_150_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UClass*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2728:2
	makeExtern< const wchar_t * (*)(const UClass *) , _dasUnreal_static_151_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_151_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UClass*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2728:2
	makeExtern< void (*)(const UClass *) , _dasUnreal_static_152_StaticRegisterNativesUClass , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUClass","_dasUnreal_static_152_StaticRegisterNativesUClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UClass*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_458 = das::das_call_member< UPackage * (UClass::*)() const,&UClass::GetOuterUPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2729:2
	makeExtern<DAS_CALL_METHOD(_method_458), SimNode_ExtFuncCall >(lib,"GetOuterUPackage","das_call_member< UPackage * (UClass::*)() const , &UClass::GetOuterUPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_459 = das::das_call_member< void (UClass::*)(),&UClass::DestroyPropertiesPendingDestruction >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2772:23
	makeExtern<DAS_CALL_METHOD(_method_459), SimNode_ExtFuncCall >(lib,"DestroyPropertiesPendingDestruction","das_call_member< void (UClass::*)() , &UClass::DestroyPropertiesPendingDestruction >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2780:15
	makeExtern< void (*)(UClass &,FUObjectSerializeContext *) , _dasUnreal_virtual_153_ConditionalRecompileClass , SimNode_ExtFuncCall >(lib,"ConditionalRecompileClass","_dasUnreal_virtual_153_ConditionalRecompileClass")
		->args({"self","InLoadContext"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2781:15
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_154_FlushCompilationQueueForLevel , SimNode_ExtFuncCall >(lib,"FlushCompilationQueueForLevel","_dasUnreal_virtual_154_FlushCompilationQueueForLevel")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_462 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsFunctionHidden >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2796:23
	makeExtern<DAS_CALL_METHOD(_method_462), SimNode_ExtFuncCall >(lib,"IsFunctionHidden","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsFunctionHidden >::invoke")
		->args({"self","InFunction"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_463 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsAutoExpandCategory >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2798:23
	makeExtern<DAS_CALL_METHOD(_method_463), SimNode_ExtFuncCall >(lib,"IsAutoExpandCategory","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsAutoExpandCategory >::invoke")
		->args({"self","InCategory"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_464 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsPrioritizeCategory >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2800:23
	makeExtern<DAS_CALL_METHOD(_method_464), SimNode_ExtFuncCall >(lib,"IsPrioritizeCategory","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsPrioritizeCategory >::invoke")
		->args({"self","InCategory"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_465 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsAutoCollapseCategory >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2802:23
	makeExtern<DAS_CALL_METHOD(_method_465), SimNode_ExtFuncCall >(lib,"IsAutoCollapseCategory","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsAutoCollapseCategory >::invoke")
		->args({"self","InCategory"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_466 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsClassGroupName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2804:23
	makeExtern<DAS_CALL_METHOD(_method_466), SimNode_ExtFuncCall >(lib,"IsClassGroupName","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsClassGroupName >::invoke")
		->args({"self","InGroupName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_467 = das::das_call_member< void * (UClass::*)(),&UClass::GetOrCreateSparseClassData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2864:8
	makeExtern<DAS_CALL_METHOD(_method_467), SimNode_ExtFuncCall >(lib,"GetOrCreateSparseClassData","das_call_member< void * (UClass::*)() , &UClass::GetOrCreateSparseClassData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_468 = das::das_call_member< UScriptStruct * (UClass::*)() const,&UClass::GetSparseClassDataStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2869:33
	makeExtern<DAS_CALL_METHOD(_method_468), SimNode_ExtFuncCall >(lib,"GetSparseClassDataStruct","das_call_member< UScriptStruct * (UClass::*)() const , &UClass::GetSparseClassDataStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_469 = das::das_call_member< void (UClass::*)(UScriptStruct *),&UClass::SetSparseClassDataStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2871:23
	makeExtern<DAS_CALL_METHOD(_method_469), SimNode_ExtFuncCall >(lib,"SetSparseClassDataStruct","das_call_member< void (UClass::*)(UScriptStruct *) , &UClass::SetSparseClassDataStruct >::invoke")
		->args({"self","InSparseClassDataStruct"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_470 = das::das_call_member< void (UClass::*)(bool),&UClass::ClearSparseClassDataStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2877:23
	makeExtern<DAS_CALL_METHOD(_method_470), SimNode_ExtFuncCall >(lib,"ClearSparseClassDataStruct","das_call_member< void (UClass::*)(bool) , &UClass::ClearSparseClassDataStruct >::invoke")
		->args({"self","bInRecomplingOnLoad"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2880:30
	makeExtern< void (*)(const UClass *) , _dasUnreal_static_155_AssembleReferenceTokenStreams , SimNode_ExtFuncCall >(lib,"AssembleReferenceTokenStreams","_dasUnreal_static_155_AssembleReferenceTokenStreams")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UClass*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2976:34
	makeExtern< UClass * (*)(UClass &) , _dasUnreal_virtual_156_GetAuthoritativeClass , SimNode_ExtFuncCall >(lib,"GetAuthoritativeClass","_dasUnreal_virtual_156_GetAuthoritativeClass")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_473 = das::das_call_member< const UClass * (UClass::*)() const,&UClass::GetAuthoritativeClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2977:16
	makeExtern<DAS_CALL_METHOD(_method_473), SimNode_ExtFuncCall >(lib,"GetAuthoritativeClass","das_call_member< const UClass * (UClass::*)() const , &UClass::GetAuthoritativeClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_474 = das::das_call_member< void (UClass::*)(UFunction *,FName),&UClass::AddFunctionToFunctionMap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2995:7
	makeExtern<DAS_CALL_METHOD(_method_474), SimNode_ExtFuncCall >(lib,"AddFunctionToFunctionMap","das_call_member< void (UClass::*)(UFunction *,FName) , &UClass::AddFunctionToFunctionMap >::invoke")
		->args({"self","Function","FuncName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_475 = das::das_call_member< void (UClass::*)(UFunction *),&UClass::RemoveFunctionFromFunctionMap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3011:7
	makeExtern<DAS_CALL_METHOD(_method_475), SimNode_ExtFuncCall >(lib,"RemoveFunctionFromFunctionMap","das_call_member< void (UClass::*)(UFunction *) , &UClass::RemoveFunctionFromFunctionMap >::invoke")
		->args({"self","Function"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_476 = das::das_call_member< void (UClass::*)(),&UClass::ClearFunctionMapsCaches >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3025:23
	makeExtern<DAS_CALL_METHOD(_method_476), SimNode_ExtFuncCall >(lib,"ClearFunctionMapsCaches","das_call_member< void (UClass::*)() , &UClass::ClearFunctionMapsCaches >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_477 = das::das_call_member< UFunction * (UClass::*)(FName,EIncludeSuperFlag::Type) const,&UClass::FindFunctionByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3028:29
	makeExtern<DAS_CALL_METHOD(_method_477), SimNode_ExtFuncCall >(lib,"FindFunctionByName","das_call_member< UFunction * (UClass::*)(FName,EIncludeSuperFlag::Type) const , &UClass::FindFunctionByName >::invoke")
		->args({"self","InName","IncludeSuper"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<EIncludeSuperFlag::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3032:31
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_157_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_157_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3033:31
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_158_FinishDestroy , SimNode_ExtFuncCall >(lib,"FinishDestroy","_dasUnreal_virtual_158_FinishDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3034:31
	makeExtern< void (*)(UClass &,UClass *,const wchar_t *,const wchar_t *) , _dasUnreal_virtual_159_DeferredRegister , SimNode_ExtFuncCall >(lib,"DeferredRegister","_dasUnreal_virtual_159_DeferredRegister")
		->args({"self","UClassStaticClass","PackageName","InName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3035:31
	makeExtern< bool (*)(UClass &,const wchar_t *,UObject *,unsigned int) , _dasUnreal_virtual_160_Rename , SimNode_ExtFuncCall >(lib,"Rename","_dasUnreal_virtual_160_Rename")
		->args({"self","NewName","NewOuter","Flags"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->arg_init(2,make_smart<ExprConstPtr>())
		->arg_init(3,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3036:31
	makeExtern< void (*)(UClass &,EObjectFlags) , _dasUnreal_virtual_161_TagSubobjects , SimNode_ExtFuncCall >(lib,"TagSubobjects","_dasUnreal_virtual_161_TagSubobjects")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3037:31
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_162_PostInitProperties , SimNode_ExtFuncCall >(lib,"PostInitProperties","_dasUnreal_virtual_162_PostInitProperties")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3039:55
	makeExtern< FRestoreForUObjectOverwrite * (*)(UClass &) , _dasUnreal_virtual_163_GetRestoreForUObjectOverwrite , SimNode_ExtFuncCall >(lib,"GetRestoreForUObjectOverwrite","_dasUnreal_virtual_163_GetRestoreForUObjectOverwrite")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3040:34
	makeExtern< FString (*)(UClass &) , _dasUnreal_virtual_164_GetDesc , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDesc","_dasUnreal_virtual_164_GetDesc")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3045:15
	makeExtern< bool (*)(const UClass &) , _dasUnreal_virtual_165_IsAsset , SimNode_ExtFuncCall >(lib,"IsAsset","_dasUnreal_virtual_165_IsAsset")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3046:15
	makeExtern< bool (*)(const UClass &) , _dasUnreal_virtual_166_IsNameStableForNetworking , SimNode_ExtFuncCall >(lib,"IsNameStableForNetworking","_dasUnreal_virtual_166_IsNameStableForNetworking")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3051:31
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_167_Bind , SimNode_ExtFuncCall >(lib,"Bind","_dasUnreal_virtual_167_Bind")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3052:39
	makeExtern< const wchar_t * (*)(const UClass &) , _dasUnreal_virtual_168_GetPrefixCPP , SimNode_ExtFuncCall >(lib,"GetPrefixCPP","_dasUnreal_virtual_168_GetPrefixCPP")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3057:31
	makeExtern< void (*)(UClass &,UStruct *) , _dasUnreal_virtual_169_SetSuperStruct , SimNode_ExtFuncCall >(lib,"SetSuperStruct","_dasUnreal_virtual_169_SetSuperStruct")
		->args({"self","NewSuperStruct"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3058:31
	makeExtern< bool (*)(const UClass &) , _dasUnreal_virtual_170_IsStructTrashed , SimNode_ExtFuncCall >(lib,"IsStructTrashed","_dasUnreal_virtual_170_IsStructTrashed")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_492 = das::das_call_member< const ICppClassTypeInfo * (UClass::*)() const,&UClass::GetCppTypeInfo >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3178:27
	makeExtern<DAS_CALL_METHOD(_method_492), SimNode_ExtFuncCall >(lib,"GetCppTypeInfo","das_call_member< const ICppClassTypeInfo * (UClass::*)() const , &UClass::GetCppTypeInfo >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_493 = das::das_call_member< void (UClass::*)(const FCppClassTypeInfoStatic *),&UClass::SetCppTypeInfoStatic >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3185:7
	makeExtern<DAS_CALL_METHOD(_method_493), SimNode_ExtFuncCall >(lib,"SetCppTypeInfoStatic","das_call_member< void (UClass::*)(const FCppClassTypeInfoStatic *) , &UClass::SetCppTypeInfoStatic >::invoke")
		->args({"self","InCppTypeInfoStatic"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_494 = das::das_call_member< const FString (UClass::*)() const,&UClass::GetConfigName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3201:32
	makeExtern<DAS_CALL_METHOD(_method_494), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetConfigName","das_call_member< const FString (UClass::*)() const , &UClass::GetConfigName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_495 = das::das_call_member< UClass * (UClass::*)() const,&UClass::GetSuperClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3204:10
	makeExtern<DAS_CALL_METHOD(_method_495), SimNode_ExtFuncCall >(lib,"GetSuperClass","das_call_member< UClass * (UClass::*)() const , &UClass::GetSuperClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_496 = das::das_call_member< int (UClass::*)(),&UClass::GetDefaultsCount >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3213:8
	makeExtern<DAS_CALL_METHOD(_method_496), SimNode_ExtFuncCall >(lib,"GetDefaultsCount","das_call_member< int (UClass::*)() , &UClass::GetDefaultsCount >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_497 = das::das_call_member< UObject * (UClass::*)(bool) const,&UClass::GetDefaultObject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3223:11
	makeExtern<DAS_CALL_METHOD(_method_497), SimNode_ExtFuncCall >(lib,"GetDefaultObject","das_call_member< UObject * (UClass::*)(bool) const , &UClass::GetDefaultObject >::invoke")
		->args({"self","bCreateIfNeeded"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3241:15
	makeExtern< void (*)(UClass &,UObject *) , _dasUnreal_virtual_171_PostLoadInstance , SimNode_ExtFuncCall >(lib,"PostLoadInstance","_dasUnreal_virtual_171_PostLoadInstance")
		->args({"self","InObj"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3250:15
	makeExtern< void (*)(UClass &,unsigned char *,const unsigned char *) , _dasUnreal_virtual_172_InitPropertiesFromCustomList , SimNode_ExtFuncCall >(lib,"InitPropertiesFromCustomList","_dasUnreal_virtual_172_InitPropertiesFromCustomList")
		->args({"self","DataPtr","DefaultDataPtr"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3260:15
	makeExtern< bool (*)(const UClass &) , _dasUnreal_virtual_173_CanCreateAssetOfClass , SimNode_ExtFuncCall >(lib,"CanCreateAssetOfClass","_dasUnreal_virtual_173_CanCreateAssetOfClass")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_501 = das::das_call_member< FName (UClass::*)() const,&UClass::GetDefaultObjectName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3269:24
	makeExtern<DAS_CALL_METHOD(_method_501), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDefaultObjectName","das_call_member< FName (UClass::*)() const , &UClass::GetDefaultObjectName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3272:17
	makeExtern< unsigned char * (*)(const UClass &,UObject *,UFunction *) , _dasUnreal_virtual_174_GetPersistentUberGraphFrame , SimNode_ExtFuncCall >(lib,"GetPersistentUberGraphFrame","_dasUnreal_virtual_174_GetPersistentUberGraphFrame")
		->args({"self","Obj","FuncToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3278:15
	makeExtern< void (*)(const UClass &,UObject *,bool,bool,UClass *) , _dasUnreal_virtual_175_CreatePersistentUberGraphFrame , SimNode_ExtFuncCall >(lib,"CreatePersistentUberGraphFrame","_dasUnreal_virtual_175_CreatePersistentUberGraphFrame")
		->args({"self","Obj","bCreateOnlyIfEmpty","bSkipSuperClass","OldClass"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->arg_init(3,make_smart<ExprConstBool>(false))
		->arg_init(4,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3283:15
	makeExtern< void (*)(const UClass &,UObject *,bool) , _dasUnreal_virtual_176_DestroyPersistentUberGraphFrame , SimNode_ExtFuncCall >(lib,"DestroyPersistentUberGraphFrame","_dasUnreal_virtual_176_DestroyPersistentUberGraphFrame")
		->args({"self","Obj","bSkipSuperClass"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_505 = das::das_call_member< UObject * (UClass::*)(FName),&UClass::GetDefaultSubobjectByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3300:27
	makeExtern<DAS_CALL_METHOD(_method_505), SimNode_ExtFuncCall >(lib,"GetDefaultSubobjectByName","das_call_member< UObject * (UClass::*)(FName) , &UClass::GetDefaultSubobjectByName >::invoke")
		->args({"self","ToFind"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_506 = das::das_call_member< void (UClass::*)(UObject *,const UClass *),&UClass::AddDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3303:7
	makeExtern<DAS_CALL_METHOD(_method_506), SimNode_ExtFuncCall >(lib,"AddDefaultSubobject","das_call_member< void (UClass::*)(UObject *,const UClass *) , &UClass::AddDefaultSubobject >::invoke")
		->args({"self","NewSubobject","BaseClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_507 = das::das_call_member< FString (UClass::*)() const,&UClass::GetDescription >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3369:26
	makeExtern<DAS_CALL_METHOD(_method_507), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDescription","das_call_member< FString (UClass::*)() const , &UClass::GetDescription >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_508 = das::das_call_member< void (UClass::*)(bool),&UClass::AssembleReferenceTokenStream >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3377:23
	makeExtern<DAS_CALL_METHOD(_method_508), SimNode_ExtFuncCall >(lib,"AssembleReferenceTokenStream","das_call_member< void (UClass::*)(bool) , &UClass::AssembleReferenceTokenStream >::invoke")
		->args({"self","bForce"})
		->arg_init(1,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_509 = das::das_call_member< bool (UClass::*)(const UClass *) const,&UClass::ImplementsInterface >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3384:23
	makeExtern<DAS_CALL_METHOD(_method_509), SimNode_ExtFuncCall >(lib,"ImplementsInterface","das_call_member< bool (UClass::*)(const UClass *) const , &UClass::ImplementsInterface >::invoke")
		->args({"self","SomeInterface"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3409:15
	makeExtern< void (*)(UClass &,UObject *) , _dasUnreal_virtual_177_PostLoadDefaultObject , SimNode_ExtFuncCall >(lib,"PostLoadDefaultObject","_dasUnreal_virtual_177_PostLoadDefaultObject")
		->args({"self","Object"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3417:31
	makeExtern< void (*)(UClass &,bool) , _dasUnreal_virtual_178_PurgeClass , SimNode_ExtFuncCall >(lib,"PurgeClass","_dasUnreal_virtual_178_PurgeClass")
		->args({"self","bRecompilingOnLoad"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3426:33
	makeExtern< UClass * (*)(const UClass *,UClass *,UClass *) , _dasUnreal_static_179_FindCommonBase , SimNode_ExtFuncCall >(lib,"FindCommonBase","_dasUnreal_static_179_FindCommonBase")
		->args({"self","InClassA","InClassB"})
		->arg_type(0,makeType<TExplicit<const UClass*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3442:31
	makeExtern< bool (*)(const UClass &,FName) , _dasUnreal_virtual_180_IsFunctionImplementedInScript , SimNode_ExtFuncCall >(lib,"IsFunctionImplementedInScript","_dasUnreal_virtual_180_IsFunctionImplementedInScript")
		->args({"self","InFunctionName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3452:19
	makeExtern< UObject * (*)(const UClass &,const UClass *,const FName) , _dasUnreal_virtual_181_FindArchetype , SimNode_ExtFuncCall >(lib,"FindArchetype","_dasUnreal_virtual_181_FindArchetype")
		->args({"self","ArchetypeClass","ArchetypeName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3455:35
	makeExtern< UObject * (*)(const UClass &) , _dasUnreal_virtual_182_GetArchetypeForCDO , SimNode_ExtFuncCall >(lib,"GetArchetypeForCDO","_dasUnreal_virtual_182_GetArchetypeForCDO")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_516 = das::das_call_member< const void * (UClass::*)() const,&UClass::GetArchetypeForSparseClassData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3458:30
	makeExtern<DAS_CALL_METHOD(_method_516), SimNode_ExtFuncCall >(lib,"GetArchetypeForSparseClassData","das_call_member< const void * (UClass::*)() const , &UClass::GetArchetypeForSparseClassData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_517 = das::das_call_member< UScriptStruct * (UClass::*)() const,&UClass::GetSparseClassDataArchetypeStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3461:33
	makeExtern<DAS_CALL_METHOD(_method_517), SimNode_ExtFuncCall >(lib,"GetSparseClassDataArchetypeStruct","das_call_member< UScriptStruct * (UClass::*)() const , &UClass::GetSparseClassDataArchetypeStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_518 = das::das_call_member< bool (UClass::*)() const,&UClass::OverridesSparseClassDataArchetype >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3464:23
	makeExtern<DAS_CALL_METHOD(_method_518), SimNode_ExtFuncCall >(lib,"OverridesSparseClassDataArchetype","das_call_member< bool (UClass::*)() const , &UClass::OverridesSparseClassDataArchetype >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_519 = das::das_call_member< void (UClass::*)(),&UClass::SetUpRuntimeReplicationData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3481:23
	makeExtern<DAS_CALL_METHOD(_method_519), SimNode_ExtFuncCall >(lib,"SetUpRuntimeReplicationData","das_call_member< void (UClass::*)() , &UClass::SetUpRuntimeReplicationData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3486:30
	makeExtern< bool (*)(const UClass *,UClass *) , _dasUnreal_static_183_IsSafeToSerializeToStructuredArchives , SimNode_ExtFuncCall >(lib,"IsSafeToSerializeToStructuredArchives","_dasUnreal_static_183_IsSafeToSerializeToStructuredArchives")
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
	using _method_521 = das::das_call_member< bool (FSavePackageResultStruct::*)() const,&FSavePackageResultStruct::IsSuccessful >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:129:7
	makeExtern<DAS_CALL_METHOD(_method_521), SimNode_ExtFuncCall >(lib,"IsSuccessful","das_call_member< bool (FSavePackageResultStruct::*)() const , &FSavePackageResultStruct::IsSuccessful >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:144:2
	makeExtern< UClass * (*)(const UPackage *) , _dasUnreal_static_184_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_184_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:144:2
	makeExtern< const wchar_t * (*)(const UPackage *) , _dasUnreal_static_185_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_185_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:144:2
	makeExtern< void (*)(const UPackage *) , _dasUnreal_static_186_StaticRegisterNativesUPackage , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUPackage","_dasUnreal_static_186_StaticRegisterNativesUPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_525 = das::das_call_member< bool (UPackage::*)() const,&UPackage::GetHasBeenEndLoaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:231:7
	makeExtern<DAS_CALL_METHOD(_method_525), SimNode_ExtFuncCall >(lib,"GetHasBeenEndLoaded","das_call_member< bool (UPackage::*)() const , &UPackage::GetHasBeenEndLoaded >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_526 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetHasBeenEndLoaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:235:7
	makeExtern<DAS_CALL_METHOD(_method_526), SimNode_ExtFuncCall >(lib,"SetHasBeenEndLoaded","das_call_member< void (UPackage::*)(bool) , &UPackage::SetHasBeenEndLoaded >::invoke")
		->args({"self","bValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:328:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_187_IsNameStableForNetworking , SimNode_ExtFuncCall >(lib,"IsNameStableForNetworking","_dasUnreal_virtual_187_IsNameStableForNetworking")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:329:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_188_NeedsLoadForClient , SimNode_ExtFuncCall >(lib,"NeedsLoadForClient","_dasUnreal_virtual_188_NeedsLoadForClient")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:330:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_189_NeedsLoadForServer , SimNode_ExtFuncCall >(lib,"NeedsLoadForServer","_dasUnreal_virtual_189_NeedsLoadForServer")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:331:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_190_IsPostLoadThreadSafe , SimNode_ExtFuncCall >(lib,"IsPostLoadThreadSafe","_dasUnreal_virtual_190_IsPostLoadThreadSafe")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:332:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_191_IsDestructionThreadSafe , SimNode_ExtFuncCall >(lib,"IsDestructionThreadSafe","_dasUnreal_virtual_191_IsDestructionThreadSafe")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_532 = das::das_call_member< void (UPackage::*)(bool,bool),&UPackage::SetLoadedByEditorPropertiesOnly >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:336:7
	makeExtern<DAS_CALL_METHOD(_method_532), SimNode_ExtFuncCall >(lib,"SetLoadedByEditorPropertiesOnly","das_call_member< void (UPackage::*)(bool,bool) , &UPackage::SetLoadedByEditorPropertiesOnly >::invoke")
		->args({"self","bIsEditorOnly","bRecursive"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_533 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsLoadedByEditorPropertiesOnly >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:338:7
	makeExtern<DAS_CALL_METHOD(_method_533), SimNode_ExtFuncCall >(lib,"IsLoadedByEditorPropertiesOnly","das_call_member< bool (UPackage::*)() const , &UPackage::IsLoadedByEditorPropertiesOnly >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_534 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetDynamicPIEPackagePending >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:341:7
	makeExtern<DAS_CALL_METHOD(_method_534), SimNode_ExtFuncCall >(lib,"SetDynamicPIEPackagePending","das_call_member< void (UPackage::*)(bool) , &UPackage::SetDynamicPIEPackagePending >::invoke")
		->args({"self","bInIsDynamicPIEPackagePending"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_535 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsDynamicPIEPackagePending >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:343:7
	makeExtern<DAS_CALL_METHOD(_method_535), SimNode_ExtFuncCall >(lib,"IsDynamicPIEPackagePending","das_call_member< bool (UPackage::*)() const , &UPackage::IsDynamicPIEPackagePending >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:350:15
	makeExtern< void (*)(UPackage &) , _dasUnreal_virtual_192_PostInitProperties , SimNode_ExtFuncCall >(lib,"PostInitProperties","_dasUnreal_virtual_192_PostInitProperties")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:352:15
	makeExtern< void (*)(UPackage &) , _dasUnreal_virtual_193_FinishDestroy , SimNode_ExtFuncCall >(lib,"FinishDestroy","_dasUnreal_virtual_193_FinishDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:358:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_194_IsAsset , SimNode_ExtFuncCall >(lib,"IsAsset","_dasUnreal_virtual_194_IsAsset")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_539 = das::das_call_member< int (UPackage::*)() const,&UPackage::GetPIEInstanceID >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:417:8
	makeExtern<DAS_CALL_METHOD(_method_539), SimNode_ExtFuncCall >(lib,"GetPIEInstanceID","das_call_member< int (UPackage::*)() const , &UPackage::GetPIEInstanceID >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_540 = das::das_call_member< void (UPackage::*)(int),&UPackage::SetPIEInstanceID >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:430:7
	makeExtern<DAS_CALL_METHOD(_method_540), SimNode_ExtFuncCall >(lib,"SetPIEInstanceID","das_call_member< void (UPackage::*)(int) , &UPackage::SetPIEInstanceID >::invoke")
		->args({"self","InPIEInstanceID"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_541 = das::das_call_member< int (UPackage::*)() const,&UPackage::GetLinkerLicenseeVersion >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:447:8
	makeExtern<DAS_CALL_METHOD(_method_541), SimNode_ExtFuncCall >(lib,"GetLinkerLicenseeVersion","das_call_member< int (UPackage::*)() const , &UPackage::GetLinkerLicenseeVersion >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_542 = das::das_call_member< void (UPackage::*)(float),&UPackage::SetLoadTime >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:461:7
	makeExtern<DAS_CALL_METHOD(_method_542), SimNode_ExtFuncCall >(lib,"SetLoadTime","das_call_member< void (UPackage::*)(float) , &UPackage::SetLoadTime >::invoke")
		->args({"self","InLoadTime"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_543 = das::das_call_member< float (UPackage::*)() const,&UPackage::GetLoadTime >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:473:8
	makeExtern<DAS_CALL_METHOD(_method_543), SimNode_ExtFuncCall >(lib,"GetLoadTime","das_call_member< float (UPackage::*)() const , &UPackage::GetLoadTime >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_544 = das::das_call_member< FName (UPackage::*)() const,&UPackage::GetFolderName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:488:8
	makeExtern<DAS_CALL_METHOD(_method_544), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFolderName","das_call_member< FName (UPackage::*)() const , &UPackage::GetFolderName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_545 = das::das_call_member< void (UPackage::*)(FName),&UPackage::SetFolderName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:497:7
	makeExtern<DAS_CALL_METHOD(_method_545), SimNode_ExtFuncCall >(lib,"SetFolderName","das_call_member< void (UPackage::*)(FName) , &UPackage::SetFolderName >::invoke")
		->args({"self","name"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_546 = das::das_call_member< void (UPackage::*)(),&UPackage::ClearDirtyFlag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:506:7
	makeExtern<DAS_CALL_METHOD(_method_546), SimNode_ExtFuncCall >(lib,"ClearDirtyFlag","das_call_member< void (UPackage::*)() , &UPackage::ClearDirtyFlag >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_547 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetDirtyFlag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:514:7
	makeExtern<DAS_CALL_METHOD(_method_547), SimNode_ExtFuncCall >(lib,"SetDirtyFlag","das_call_member< void (UPackage::*)(bool) , &UPackage::SetDirtyFlag >::invoke")
		->args({"self","bIsDirty"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_548 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsDirty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:521:7
	makeExtern<DAS_CALL_METHOD(_method_548), SimNode_ExtFuncCall >(lib,"IsDirty","das_call_member< bool (UPackage::*)() const , &UPackage::IsDirty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_549 = das::das_call_member< void (UPackage::*)(),&UPackage::MarkAsNewlyCreated >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:530:7
	makeExtern<DAS_CALL_METHOD(_method_549), SimNode_ExtFuncCall >(lib,"MarkAsNewlyCreated","das_call_member< void (UPackage::*)() , &UPackage::MarkAsNewlyCreated >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_550 = das::das_call_member< void (UPackage::*)(),&UPackage::MarkAsUnloaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:540:7
	makeExtern<DAS_CALL_METHOD(_method_550), SimNode_ExtFuncCall >(lib,"MarkAsUnloaded","das_call_member< void (UPackage::*)() , &UPackage::MarkAsUnloaded >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

