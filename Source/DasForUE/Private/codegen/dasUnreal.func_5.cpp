// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_5() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2079:32
	makeExtern< FText (*)(const UEnum &,int) , _dasUnreal_virtual_103_GetDisplayNameTextByIndex , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDisplayNameTextByIndex","_dasUnreal_virtual_103_GetDisplayNameTextByIndex")
		->args({"self","InIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_332 = das::das_call_member< FText (UEnum::*)(int64) const,&UEnum::GetDisplayNameTextByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2082:24
	makeExtern<DAS_CALL_METHOD(_method_332), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDisplayNameTextByValue","das_call_member< FText (UEnum::*)(int64) const , &UEnum::GetDisplayNameTextByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_333 = das::das_call_member< bool (UEnum::*)(FText &,int64) const,&UEnum::FindDisplayNameTextByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2085:23
	makeExtern<DAS_CALL_METHOD(_method_333), SimNode_ExtFuncCall >(lib,"FindDisplayNameTextByValue","das_call_member< bool (UEnum::*)(FText &,int64) const , &UEnum::FindDisplayNameTextByValue >::invoke")
		->args({"self","Out","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2096:34
	makeExtern< FString (*)(const UEnum &,int) , _dasUnreal_virtual_104_GetAuthoredNameStringByIndex , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredNameStringByIndex","_dasUnreal_virtual_104_GetAuthoredNameStringByIndex")
		->args({"self","InIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_335 = das::das_call_member< FString (UEnum::*)(int64) const,&UEnum::GetAuthoredNameStringByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2099:26
	makeExtern<DAS_CALL_METHOD(_method_335), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredNameStringByValue","das_call_member< FString (UEnum::*)(int64) const , &UEnum::GetAuthoredNameStringByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_336 = das::das_call_member< bool (UEnum::*)(FString &,int64) const,&UEnum::FindAuthoredNameStringByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2102:23
	makeExtern<DAS_CALL_METHOD(_method_336), SimNode_ExtFuncCall >(lib,"FindAuthoredNameStringByValue","das_call_member< bool (UEnum::*)(FString &,int64) const , &UEnum::FindAuthoredNameStringByValue >::invoke")
		->args({"self","Out","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_337 = das::das_call_member< int64 (UEnum::*)() const,&UEnum::GetMaxEnumValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2105:24
	makeExtern<DAS_CALL_METHOD(_method_337), SimNode_ExtFuncCall >(lib,"GetMaxEnumValue","das_call_member< int64 (UEnum::*)() const , &UEnum::GetMaxEnumValue >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_338 = das::das_call_member< bool (UEnum::*)(int64) const,&UEnum::IsValidEnumValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2108:23
	makeExtern<DAS_CALL_METHOD(_method_338), SimNode_ExtFuncCall >(lib,"IsValidEnumValue","das_call_member< bool (UEnum::*)(int64) const , &UEnum::IsValidEnumValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_339 = das::das_call_member< bool (UEnum::*)(FName) const,&UEnum::IsValidEnumName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2111:23
	makeExtern<DAS_CALL_METHOD(_method_339), SimNode_ExtFuncCall >(lib,"IsValidEnumName","das_call_member< bool (UEnum::*)(FName) const , &UEnum::IsValidEnumName >::invoke")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_340 = das::das_call_member< void (UEnum::*)(),&UEnum::RemoveNamesFromMasterList >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2115:7
	makeExtern<DAS_CALL_METHOD(_method_340), SimNode_ExtFuncCall >(lib,"RemoveNamesFromMasterList","das_call_member< void (UEnum::*)() , &UEnum::RemoveNamesFromMasterList >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_341 = das::das_call_member< void (UEnum::*)(),&UEnum::RemoveNamesFromPrimaryList >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2121:23
	makeExtern<DAS_CALL_METHOD(_method_341), SimNode_ExtFuncCall >(lib,"RemoveNamesFromPrimaryList","das_call_member< void (UEnum::*)() , &UEnum::RemoveNamesFromPrimaryList >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_342 = das::das_call_member< UEnum::ECppForm (UEnum::*)() const,&UEnum::GetCppForm >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2137:11
	makeExtern<DAS_CALL_METHOD(_method_342), SimNode_ExtFuncCall >(lib,"GetCppForm","das_call_member< UEnum::ECppForm (UEnum::*)() const , &UEnum::GetCppForm >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_343 = das::das_call_member< void (UEnum::*)(EEnumFlags),&UEnum::SetEnumFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2142:7
	makeExtern<DAS_CALL_METHOD(_method_343), SimNode_ExtFuncCall >(lib,"SetEnumFlags","das_call_member< void (UEnum::*)(EEnumFlags) , &UEnum::SetEnumFlags >::invoke")
		->args({"self","FlagsToSet"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_344 = das::das_call_member< bool (UEnum::*)(EEnumFlags) const,&UEnum::HasAnyEnumFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2147:7
	makeExtern<DAS_CALL_METHOD(_method_344), SimNode_ExtFuncCall >(lib,"HasAnyEnumFlags","das_call_member< bool (UEnum::*)(EEnumFlags) const , &UEnum::HasAnyEnumFlags >::invoke")
		->args({"self","InFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2158:14
	makeExtern< bool (*)(const UEnum &,const wchar_t *) , _dasUnreal_static_105_IsFullEnumName , SimNode_ExtFuncCall >(lib,"IsFullEnumName","_dasUnreal_static_105_IsFullEnumName")
		->args({"self","InEnumName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2169:34
	makeExtern< FString (*)(const UEnum &,const wchar_t *) , _dasUnreal_virtual_106_GenerateFullEnumName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GenerateFullEnumName","_dasUnreal_virtual_106_GenerateFullEnumName")
		->args({"self","InEnumName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2213:31
	makeExtern< int64 (*)(const UEnum &,const wchar_t *&) , _dasUnreal_static_107_ParseEnum , SimNode_ExtFuncCall >(lib,"ParseEnum","_dasUnreal_static_107_ParseEnum")
		->args({"self","Str"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_348 = das::das_call_member< bool (UEnum::*)() const,&UEnum::ContainsExistingMax >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2220:23
	makeExtern<DAS_CALL_METHOD(_method_348), SimNode_ExtFuncCall >(lib,"ContainsExistingMax","das_call_member< bool (UEnum::*)() const , &UEnum::ContainsExistingMax >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_349 = das::das_call_member< int (UEnum::*)() const,&UEnum::NumEnums >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2235:8
	makeExtern<DAS_CALL_METHOD(_method_349), SimNode_ExtFuncCall >(lib,"NumEnums","das_call_member< int (UEnum::*)() const , &UEnum::NumEnums >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_350 = das::das_call_member< FString (UEnum::*)() const,&UEnum::GenerateEnumPrefix >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2246:26
	makeExtern<DAS_CALL_METHOD(_method_350), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GenerateEnumPrefix","das_call_member< FString (UEnum::*)() const , &UEnum::GenerateEnumPrefix >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_351 = das::das_call_member< FText (UEnum::*)(int) const,&UEnum::GetToolTipTextByIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2256:24
	makeExtern<DAS_CALL_METHOD(_method_351), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetToolTipTextByIndex","das_call_member< FText (UEnum::*)(int) const , &UEnum::GetToolTipTextByIndex >::invoke")
		->args({"self","NameIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_352 = das::das_call_member< bool (UEnum::*)(const wchar_t *,int) const,&UEnum::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2268:23
	makeExtern<DAS_CALL_METHOD(_method_352), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (UEnum::*)(const wchar_t *,int) const , &UEnum::HasMetaData >::invoke")
		->args({"self","Key","NameIndex"})
		->arg_init(2,make_smart<ExprConstInt>(-1))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_353 = das::das_call_member< FString (UEnum::*)(const wchar_t *,int,bool) const,&UEnum::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2279:26
	makeExtern<DAS_CALL_METHOD(_method_353), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetMetaData","das_call_member< FString (UEnum::*)(const wchar_t *,int,bool) const , &UEnum::GetMetaData >::invoke")
		->args({"self","Key","NameIndex","bAllowRemap"})
		->arg_init(2,make_smart<ExprConstInt>(-1))
		->arg_init(3,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_354 = das::das_call_member< void (UEnum::*)(const wchar_t *,const wchar_t *,int) const,&UEnum::SetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2289:23
	makeExtern<DAS_CALL_METHOD(_method_354), SimNode_ExtFuncCall >(lib,"SetMetaData","das_call_member< void (UEnum::*)(const wchar_t *,const wchar_t *,int) const , &UEnum::SetMetaData >::invoke")
		->args({"self","Key","InValue","NameIndex"})
		->arg_init(3,make_smart<ExprConstInt>(-1))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_355 = das::das_call_member< void (UEnum::*)(const wchar_t *,int) const,&UEnum::RemoveMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2298:23
	makeExtern<DAS_CALL_METHOD(_method_355), SimNode_ExtFuncCall >(lib,"RemoveMetaData","das_call_member< void (UEnum::*)(const wchar_t *,int) const , &UEnum::RemoveMetaData >::invoke")
		->args({"self","Key","NameIndex"})
		->arg_init(2,make_smart<ExprConstInt>(-1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2512:31
	makeExtern< void (*)(UEnum &) , _dasUnreal_virtual_108_BeginDestroy , SimNode_ExtFuncCall >(lib,"BeginDestroy","_dasUnreal_virtual_108_BeginDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2648:15
	makeExtern< bool (*)(const FCppClassTypeInfo &) , _dasUnreal_virtual_109_IsAbstract , SimNode_ExtFuncCall >(lib,"IsAbstract","_dasUnreal_virtual_109_IsAbstract")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FImplementedInterface>(*this,lib,"FImplementedInterface","FImplementedInterface");
	addCtorAndUsing<FImplementedInterface,UClass *,int,bool>(*this,lib,"FImplementedInterface","FImplementedInterface")
		->args({"InClass","InOffset","InImplementedByK2"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2728:2
	makeExtern< UClass * (*)(const UClass &) , _dasUnreal_static_110_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_110_StaticClass")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2728:2
	makeExtern< const wchar_t * (*)(const UClass &) , _dasUnreal_static_111_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_111_StaticPackage")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2728:2
	makeExtern< void (*)(const UClass &) , _dasUnreal_static_112_StaticRegisterNativesUClass , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUClass","_dasUnreal_static_112_StaticRegisterNativesUClass")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_361 = das::das_call_member< UPackage * (UClass::*)() const,&UClass::GetOuterUPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2729:2
	makeExtern<DAS_CALL_METHOD(_method_361), SimNode_ExtFuncCall >(lib,"GetOuterUPackage","das_call_member< UPackage * (UClass::*)() const , &UClass::GetOuterUPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_362 = das::das_call_member< void (UClass::*)(),&UClass::DestroyPropertiesPendingDestruction >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2772:23
	makeExtern<DAS_CALL_METHOD(_method_362), SimNode_ExtFuncCall >(lib,"DestroyPropertiesPendingDestruction","das_call_member< void (UClass::*)() , &UClass::DestroyPropertiesPendingDestruction >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2780:15
	makeExtern< void (*)(UClass &,FUObjectSerializeContext *) , _dasUnreal_virtual_113_ConditionalRecompileClass , SimNode_ExtFuncCall >(lib,"ConditionalRecompileClass","_dasUnreal_virtual_113_ConditionalRecompileClass")
		->args({"self","InLoadContext"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2781:15
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_114_FlushCompilationQueueForLevel , SimNode_ExtFuncCall >(lib,"FlushCompilationQueueForLevel","_dasUnreal_virtual_114_FlushCompilationQueueForLevel")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_365 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsFunctionHidden >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2796:23
	makeExtern<DAS_CALL_METHOD(_method_365), SimNode_ExtFuncCall >(lib,"IsFunctionHidden","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsFunctionHidden >::invoke")
		->args({"self","InFunction"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_366 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsAutoExpandCategory >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2798:23
	makeExtern<DAS_CALL_METHOD(_method_366), SimNode_ExtFuncCall >(lib,"IsAutoExpandCategory","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsAutoExpandCategory >::invoke")
		->args({"self","InCategory"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_367 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsPrioritizeCategory >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2800:23
	makeExtern<DAS_CALL_METHOD(_method_367), SimNode_ExtFuncCall >(lib,"IsPrioritizeCategory","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsPrioritizeCategory >::invoke")
		->args({"self","InCategory"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_368 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsAutoCollapseCategory >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2802:23
	makeExtern<DAS_CALL_METHOD(_method_368), SimNode_ExtFuncCall >(lib,"IsAutoCollapseCategory","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsAutoCollapseCategory >::invoke")
		->args({"self","InCategory"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_369 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsClassGroupName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2804:23
	makeExtern<DAS_CALL_METHOD(_method_369), SimNode_ExtFuncCall >(lib,"IsClassGroupName","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsClassGroupName >::invoke")
		->args({"self","InGroupName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_370 = das::das_call_member< void * (UClass::*)(),&UClass::GetOrCreateSparseClassData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2864:8
	makeExtern<DAS_CALL_METHOD(_method_370), SimNode_ExtFuncCall >(lib,"GetOrCreateSparseClassData","das_call_member< void * (UClass::*)() , &UClass::GetOrCreateSparseClassData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_371 = das::das_call_member< void (UClass::*)(bool),&UClass::ClearSparseClassDataStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2877:23
	makeExtern<DAS_CALL_METHOD(_method_371), SimNode_ExtFuncCall >(lib,"ClearSparseClassDataStruct","das_call_member< void (UClass::*)(bool) , &UClass::ClearSparseClassDataStruct >::invoke")
		->args({"self","bInRecomplingOnLoad"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2880:30
	makeExtern< void (*)(const UClass &) , _dasUnreal_static_115_AssembleReferenceTokenStreams , SimNode_ExtFuncCall >(lib,"AssembleReferenceTokenStreams","_dasUnreal_static_115_AssembleReferenceTokenStreams")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2976:34
	makeExtern< UClass * (*)(UClass &) , _dasUnreal_virtual_116_GetAuthoritativeClass , SimNode_ExtFuncCall >(lib,"GetAuthoritativeClass","_dasUnreal_virtual_116_GetAuthoritativeClass")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_374 = das::das_call_member< const UClass * (UClass::*)() const,&UClass::GetAuthoritativeClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2977:16
	makeExtern<DAS_CALL_METHOD(_method_374), SimNode_ExtFuncCall >(lib,"GetAuthoritativeClass","das_call_member< const UClass * (UClass::*)() const , &UClass::GetAuthoritativeClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_375 = das::das_call_member< void (UClass::*)(),&UClass::ClearFunctionMapsCaches >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3025:23
	makeExtern<DAS_CALL_METHOD(_method_375), SimNode_ExtFuncCall >(lib,"ClearFunctionMapsCaches","das_call_member< void (UClass::*)() , &UClass::ClearFunctionMapsCaches >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3032:31
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_117_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_117_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3033:31
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_118_FinishDestroy , SimNode_ExtFuncCall >(lib,"FinishDestroy","_dasUnreal_virtual_118_FinishDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3034:31
	makeExtern< void (*)(UClass &,UClass *,const wchar_t *,const wchar_t *) , _dasUnreal_virtual_119_DeferredRegister , SimNode_ExtFuncCall >(lib,"DeferredRegister","_dasUnreal_virtual_119_DeferredRegister")
		->args({"self","UClassStaticClass","PackageName","InName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3035:31
	makeExtern< bool (*)(UClass &,const wchar_t *,UObject *,unsigned int) , _dasUnreal_virtual_120_Rename , SimNode_ExtFuncCall >(lib,"Rename","_dasUnreal_virtual_120_Rename")
		->args({"self","NewName","NewOuter","Flags"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->arg_init(2,make_smart<ExprConstPtr>())
		->arg_init(3,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3036:31
	makeExtern< void (*)(UClass &,EObjectFlags) , _dasUnreal_virtual_121_TagSubobjects , SimNode_ExtFuncCall >(lib,"TagSubobjects","_dasUnreal_virtual_121_TagSubobjects")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3037:31
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_122_PostInitProperties , SimNode_ExtFuncCall >(lib,"PostInitProperties","_dasUnreal_virtual_122_PostInitProperties")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3039:55
	makeExtern< FRestoreForUObjectOverwrite * (*)(UClass &) , _dasUnreal_virtual_123_GetRestoreForUObjectOverwrite , SimNode_ExtFuncCall >(lib,"GetRestoreForUObjectOverwrite","_dasUnreal_virtual_123_GetRestoreForUObjectOverwrite")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3040:34
	makeExtern< FString (*)(UClass &) , _dasUnreal_virtual_124_GetDesc , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDesc","_dasUnreal_virtual_124_GetDesc")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3045:15
	makeExtern< bool (*)(const UClass &) , _dasUnreal_virtual_125_IsAsset , SimNode_ExtFuncCall >(lib,"IsAsset","_dasUnreal_virtual_125_IsAsset")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3046:15
	makeExtern< bool (*)(const UClass &) , _dasUnreal_virtual_126_IsNameStableForNetworking , SimNode_ExtFuncCall >(lib,"IsNameStableForNetworking","_dasUnreal_virtual_126_IsNameStableForNetworking")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3051:31
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_127_Bind , SimNode_ExtFuncCall >(lib,"Bind","_dasUnreal_virtual_127_Bind")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3052:39
	makeExtern< const wchar_t * (*)(const UClass &) , _dasUnreal_virtual_128_GetPrefixCPP , SimNode_ExtFuncCall >(lib,"GetPrefixCPP","_dasUnreal_virtual_128_GetPrefixCPP")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3057:31
	makeExtern< void (*)(UClass &,UStruct *) , _dasUnreal_virtual_129_SetSuperStruct , SimNode_ExtFuncCall >(lib,"SetSuperStruct","_dasUnreal_virtual_129_SetSuperStruct")
		->args({"self","NewSuperStruct"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3058:31
	makeExtern< bool (*)(const UClass &) , _dasUnreal_virtual_130_IsStructTrashed , SimNode_ExtFuncCall >(lib,"IsStructTrashed","_dasUnreal_virtual_130_IsStructTrashed")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_390 = das::das_call_member< const ICppClassTypeInfo * (UClass::*)() const,&UClass::GetCppTypeInfo >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3178:27
	makeExtern<DAS_CALL_METHOD(_method_390), SimNode_ExtFuncCall >(lib,"GetCppTypeInfo","das_call_member< const ICppClassTypeInfo * (UClass::*)() const , &UClass::GetCppTypeInfo >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_391 = das::das_call_member< void (UClass::*)(const FCppClassTypeInfoStatic *),&UClass::SetCppTypeInfoStatic >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3185:7
	makeExtern<DAS_CALL_METHOD(_method_391), SimNode_ExtFuncCall >(lib,"SetCppTypeInfoStatic","das_call_member< void (UClass::*)(const FCppClassTypeInfoStatic *) , &UClass::SetCppTypeInfoStatic >::invoke")
		->args({"self","InCppTypeInfoStatic"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_392 = das::das_call_member< const FString (UClass::*)() const,&UClass::GetConfigName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3201:32
	makeExtern<DAS_CALL_METHOD(_method_392), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetConfigName","das_call_member< const FString (UClass::*)() const , &UClass::GetConfigName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_393 = das::das_call_member< UClass * (UClass::*)() const,&UClass::GetSuperClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3204:10
	makeExtern<DAS_CALL_METHOD(_method_393), SimNode_ExtFuncCall >(lib,"GetSuperClass","das_call_member< UClass * (UClass::*)() const , &UClass::GetSuperClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_394 = das::das_call_member< int (UClass::*)(),&UClass::GetDefaultsCount >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3213:8
	makeExtern<DAS_CALL_METHOD(_method_394), SimNode_ExtFuncCall >(lib,"GetDefaultsCount","das_call_member< int (UClass::*)() , &UClass::GetDefaultsCount >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_395 = das::das_call_member< UObject * (UClass::*)(bool) const,&UClass::GetDefaultObject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3223:11
	makeExtern<DAS_CALL_METHOD(_method_395), SimNode_ExtFuncCall >(lib,"GetDefaultObject","das_call_member< UObject * (UClass::*)(bool) const , &UClass::GetDefaultObject >::invoke")
		->args({"self","bCreateIfNeeded"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3241:15
	makeExtern< void (*)(UClass &,UObject *) , _dasUnreal_virtual_131_PostLoadInstance , SimNode_ExtFuncCall >(lib,"PostLoadInstance","_dasUnreal_virtual_131_PostLoadInstance")
		->args({"self","InObj"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3250:15
	makeExtern< void (*)(UClass &,unsigned char *,const unsigned char *) , _dasUnreal_virtual_132_InitPropertiesFromCustomList , SimNode_ExtFuncCall >(lib,"InitPropertiesFromCustomList","_dasUnreal_virtual_132_InitPropertiesFromCustomList")
		->args({"self","DataPtr","DefaultDataPtr"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3260:15
	makeExtern< bool (*)(const UClass &) , _dasUnreal_virtual_133_CanCreateAssetOfClass , SimNode_ExtFuncCall >(lib,"CanCreateAssetOfClass","_dasUnreal_virtual_133_CanCreateAssetOfClass")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_399 = das::das_call_member< FName (UClass::*)() const,&UClass::GetDefaultObjectName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3269:24
	makeExtern<DAS_CALL_METHOD(_method_399), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDefaultObjectName","das_call_member< FName (UClass::*)() const , &UClass::GetDefaultObjectName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3278:15
	makeExtern< void (*)(const UClass &,UObject *,bool,bool,UClass *) , _dasUnreal_virtual_134_CreatePersistentUberGraphFrame , SimNode_ExtFuncCall >(lib,"CreatePersistentUberGraphFrame","_dasUnreal_virtual_134_CreatePersistentUberGraphFrame")
		->args({"self","Obj","bCreateOnlyIfEmpty","bSkipSuperClass","OldClass"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->arg_init(3,make_smart<ExprConstBool>(false))
		->arg_init(4,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3283:15
	makeExtern< void (*)(const UClass &,UObject *,bool) , _dasUnreal_virtual_135_DestroyPersistentUberGraphFrame , SimNode_ExtFuncCall >(lib,"DestroyPersistentUberGraphFrame","_dasUnreal_virtual_135_DestroyPersistentUberGraphFrame")
		->args({"self","Obj","bSkipSuperClass"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_402 = das::das_call_member< UObject * (UClass::*)(FName),&UClass::GetDefaultSubobjectByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3300:27
	makeExtern<DAS_CALL_METHOD(_method_402), SimNode_ExtFuncCall >(lib,"GetDefaultSubobjectByName","das_call_member< UObject * (UClass::*)(FName) , &UClass::GetDefaultSubobjectByName >::invoke")
		->args({"self","ToFind"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_403 = das::das_call_member< void (UClass::*)(UObject *,const UClass *),&UClass::AddDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3303:7
	makeExtern<DAS_CALL_METHOD(_method_403), SimNode_ExtFuncCall >(lib,"AddDefaultSubobject","das_call_member< void (UClass::*)(UObject *,const UClass *) , &UClass::AddDefaultSubobject >::invoke")
		->args({"self","NewSubobject","BaseClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_404 = das::das_call_member< FString (UClass::*)() const,&UClass::GetDescription >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3369:26
	makeExtern<DAS_CALL_METHOD(_method_404), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDescription","das_call_member< FString (UClass::*)() const , &UClass::GetDescription >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_405 = das::das_call_member< void (UClass::*)(bool),&UClass::AssembleReferenceTokenStream >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3377:23
	makeExtern<DAS_CALL_METHOD(_method_405), SimNode_ExtFuncCall >(lib,"AssembleReferenceTokenStream","das_call_member< void (UClass::*)(bool) , &UClass::AssembleReferenceTokenStream >::invoke")
		->args({"self","bForce"})
		->arg_init(1,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_406 = das::das_call_member< bool (UClass::*)(const UClass *) const,&UClass::ImplementsInterface >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3384:23
	makeExtern<DAS_CALL_METHOD(_method_406), SimNode_ExtFuncCall >(lib,"ImplementsInterface","das_call_member< bool (UClass::*)(const UClass *) const , &UClass::ImplementsInterface >::invoke")
		->args({"self","SomeInterface"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3409:15
	makeExtern< void (*)(UClass &,UObject *) , _dasUnreal_virtual_136_PostLoadDefaultObject , SimNode_ExtFuncCall >(lib,"PostLoadDefaultObject","_dasUnreal_virtual_136_PostLoadDefaultObject")
		->args({"self","Object"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3417:31
	makeExtern< void (*)(UClass &,bool) , _dasUnreal_virtual_137_PurgeClass , SimNode_ExtFuncCall >(lib,"PurgeClass","_dasUnreal_virtual_137_PurgeClass")
		->args({"self","bRecompilingOnLoad"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3426:33
	makeExtern< UClass * (*)(const UClass &,UClass *,UClass *) , _dasUnreal_static_138_FindCommonBase , SimNode_ExtFuncCall >(lib,"FindCommonBase","_dasUnreal_static_138_FindCommonBase")
		->args({"self","InClassA","InClassB"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3442:31
	makeExtern< bool (*)(const UClass &,FName) , _dasUnreal_virtual_139_IsFunctionImplementedInScript , SimNode_ExtFuncCall >(lib,"IsFunctionImplementedInScript","_dasUnreal_virtual_139_IsFunctionImplementedInScript")
		->args({"self","InFunctionName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3452:19
	makeExtern< UObject * (*)(const UClass &,const UClass *,const FName) , _dasUnreal_virtual_140_FindArchetype , SimNode_ExtFuncCall >(lib,"FindArchetype","_dasUnreal_virtual_140_FindArchetype")
		->args({"self","ArchetypeClass","ArchetypeName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3455:35
	makeExtern< UObject * (*)(const UClass &) , _dasUnreal_virtual_141_GetArchetypeForCDO , SimNode_ExtFuncCall >(lib,"GetArchetypeForCDO","_dasUnreal_virtual_141_GetArchetypeForCDO")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_413 = das::das_call_member< const void * (UClass::*)() const,&UClass::GetArchetypeForSparseClassData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3458:30
	makeExtern<DAS_CALL_METHOD(_method_413), SimNode_ExtFuncCall >(lib,"GetArchetypeForSparseClassData","das_call_member< const void * (UClass::*)() const , &UClass::GetArchetypeForSparseClassData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_414 = das::das_call_member< bool (UClass::*)() const,&UClass::OverridesSparseClassDataArchetype >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3464:23
	makeExtern<DAS_CALL_METHOD(_method_414), SimNode_ExtFuncCall >(lib,"OverridesSparseClassDataArchetype","das_call_member< bool (UClass::*)() const , &UClass::OverridesSparseClassDataArchetype >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_415 = das::das_call_member< void (UClass::*)(),&UClass::SetUpRuntimeReplicationData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3481:23
	makeExtern<DAS_CALL_METHOD(_method_415), SimNode_ExtFuncCall >(lib,"SetUpRuntimeReplicationData","das_call_member< void (UClass::*)() , &UClass::SetUpRuntimeReplicationData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3486:30
	makeExtern< bool (*)(const UClass &,UClass *) , _dasUnreal_static_142_IsSafeToSerializeToStructuredArchives , SimNode_ExtFuncCall >(lib,"IsSafeToSerializeToStructuredArchives","_dasUnreal_static_142_IsSafeToSerializeToStructuredArchives")
		->args({"self","InClass"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3610:22
	makeExtern< void (*)(UClass *,UClass *,UClass *,const wchar_t *,const wchar_t *) , InitializePrivateStaticClass , SimNode_ExtFuncCall >(lib,"InitializePrivateStaticClass","InitializePrivateStaticClass")
		->args({"TClass_Super_StaticClass","TClass_PrivateStaticClass","TClass_WithinClass_StaticClass","PackageName","Name"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

