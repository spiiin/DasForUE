// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_8() {
	using _method_550 = das::das_call_member< int64 (UEnum::*)(const FString &,EGetByNameFlags) const,&UEnum::GetValueByNameString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2069:24
	makeExtern<DAS_CALL_METHOD(_method_550), SimNode_ExtFuncCall >(lib,"GetValueByNameString","das_call_member< int64 (UEnum::*)(const FString &,EGetByNameFlags) const , &UEnum::GetValueByNameString >::invoke")
		->args({"self","SearchString","Flags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EGetByNameFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2079:32
	makeExtern< FText (*)(const UEnum &,int) , _dasUnreal_virtual_172_GetDisplayNameTextByIndex , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDisplayNameTextByIndex","_dasUnreal_virtual_172_GetDisplayNameTextByIndex")
		->args({"self","InIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_552 = das::das_call_member< FText (UEnum::*)(int64) const,&UEnum::GetDisplayNameTextByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2082:24
	makeExtern<DAS_CALL_METHOD(_method_552), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDisplayNameTextByValue","das_call_member< FText (UEnum::*)(int64) const , &UEnum::GetDisplayNameTextByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_553 = das::das_call_member< bool (UEnum::*)(FText &,int64) const,&UEnum::FindDisplayNameTextByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2085:23
	makeExtern<DAS_CALL_METHOD(_method_553), SimNode_ExtFuncCall >(lib,"FindDisplayNameTextByValue","das_call_member< bool (UEnum::*)(FText &,int64) const , &UEnum::FindDisplayNameTextByValue >::invoke")
		->args({"self","Out","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2096:34
	makeExtern< FString (*)(const UEnum &,int) , _dasUnreal_virtual_173_GetAuthoredNameStringByIndex , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredNameStringByIndex","_dasUnreal_virtual_173_GetAuthoredNameStringByIndex")
		->args({"self","InIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_555 = das::das_call_member< FString (UEnum::*)(int64) const,&UEnum::GetAuthoredNameStringByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2099:26
	makeExtern<DAS_CALL_METHOD(_method_555), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredNameStringByValue","das_call_member< FString (UEnum::*)(int64) const , &UEnum::GetAuthoredNameStringByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_556 = das::das_call_member< bool (UEnum::*)(FString &,int64) const,&UEnum::FindAuthoredNameStringByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2102:23
	makeExtern<DAS_CALL_METHOD(_method_556), SimNode_ExtFuncCall >(lib,"FindAuthoredNameStringByValue","das_call_member< bool (UEnum::*)(FString &,int64) const , &UEnum::FindAuthoredNameStringByValue >::invoke")
		->args({"self","Out","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_557 = das::das_call_member< int64 (UEnum::*)() const,&UEnum::GetMaxEnumValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2105:24
	makeExtern<DAS_CALL_METHOD(_method_557), SimNode_ExtFuncCall >(lib,"GetMaxEnumValue","das_call_member< int64 (UEnum::*)() const , &UEnum::GetMaxEnumValue >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_558 = das::das_call_member< bool (UEnum::*)(int64) const,&UEnum::IsValidEnumValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2108:23
	makeExtern<DAS_CALL_METHOD(_method_558), SimNode_ExtFuncCall >(lib,"IsValidEnumValue","das_call_member< bool (UEnum::*)(int64) const , &UEnum::IsValidEnumValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_559 = das::das_call_member< bool (UEnum::*)(FName) const,&UEnum::IsValidEnumName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2111:23
	makeExtern<DAS_CALL_METHOD(_method_559), SimNode_ExtFuncCall >(lib,"IsValidEnumName","das_call_member< bool (UEnum::*)(FName) const , &UEnum::IsValidEnumName >::invoke")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_560 = das::das_call_member< void (UEnum::*)(),&UEnum::RemoveNamesFromMasterList >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2115:7
	makeExtern<DAS_CALL_METHOD(_method_560), SimNode_ExtFuncCall >(lib,"RemoveNamesFromMasterList","das_call_member< void (UEnum::*)() , &UEnum::RemoveNamesFromMasterList >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_561 = das::das_call_member< void (UEnum::*)(),&UEnum::RemoveNamesFromPrimaryList >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2121:23
	makeExtern<DAS_CALL_METHOD(_method_561), SimNode_ExtFuncCall >(lib,"RemoveNamesFromPrimaryList","das_call_member< void (UEnum::*)() , &UEnum::RemoveNamesFromPrimaryList >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_562 = das::das_call_member< UEnum::ECppForm (UEnum::*)() const,&UEnum::GetCppForm >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2137:11
	makeExtern<DAS_CALL_METHOD(_method_562), SimNode_ExtFuncCall >(lib,"GetCppForm","das_call_member< UEnum::ECppForm (UEnum::*)() const , &UEnum::GetCppForm >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_563 = das::das_call_member< void (UEnum::*)(EEnumFlags),&UEnum::SetEnumFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2142:7
	makeExtern<DAS_CALL_METHOD(_method_563), SimNode_ExtFuncCall >(lib,"SetEnumFlags","das_call_member< void (UEnum::*)(EEnumFlags) , &UEnum::SetEnumFlags >::invoke")
		->args({"self","FlagsToSet"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_564 = das::das_call_member< bool (UEnum::*)(EEnumFlags) const,&UEnum::HasAnyEnumFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2147:7
	makeExtern<DAS_CALL_METHOD(_method_564), SimNode_ExtFuncCall >(lib,"HasAnyEnumFlags","das_call_member< bool (UEnum::*)(EEnumFlags) const , &UEnum::HasAnyEnumFlags >::invoke")
		->args({"self","InFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2158:14
	makeExtern< bool (*)(const UEnum *,const wchar_t *) , _dasUnreal_static_174_IsFullEnumName , SimNode_ExtFuncCall >(lib,"IsFullEnumName","_dasUnreal_static_174_IsFullEnumName")
		->args({"self","InEnumName"})
		->arg_type(0,makeType<TExplicit<const UEnum*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2169:34
	makeExtern< FString (*)(const UEnum &,const wchar_t *) , _dasUnreal_virtual_175_GenerateFullEnumName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GenerateFullEnumName","_dasUnreal_virtual_175_GenerateFullEnumName")
		->args({"self","InEnumName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2213:31
	makeExtern< int64 (*)(const UEnum *,const wchar_t *&) , _dasUnreal_static_176_ParseEnum , SimNode_ExtFuncCall >(lib,"ParseEnum","_dasUnreal_static_176_ParseEnum")
		->args({"self","Str"})
		->arg_type(0,makeType<TExplicit<const UEnum*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_568 = das::das_call_member< bool (UEnum::*)() const,&UEnum::ContainsExistingMax >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2220:23
	makeExtern<DAS_CALL_METHOD(_method_568), SimNode_ExtFuncCall >(lib,"ContainsExistingMax","das_call_member< bool (UEnum::*)() const , &UEnum::ContainsExistingMax >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_569 = das::das_call_member< int (UEnum::*)() const,&UEnum::NumEnums >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2235:8
	makeExtern<DAS_CALL_METHOD(_method_569), SimNode_ExtFuncCall >(lib,"NumEnums","das_call_member< int (UEnum::*)() const , &UEnum::NumEnums >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_570 = das::das_call_member< FString (UEnum::*)() const,&UEnum::GenerateEnumPrefix >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2246:26
	makeExtern<DAS_CALL_METHOD(_method_570), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GenerateEnumPrefix","das_call_member< FString (UEnum::*)() const , &UEnum::GenerateEnumPrefix >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_571 = das::das_call_member< FText (UEnum::*)(int) const,&UEnum::GetToolTipTextByIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2256:24
	makeExtern<DAS_CALL_METHOD(_method_571), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetToolTipTextByIndex","das_call_member< FText (UEnum::*)(int) const , &UEnum::GetToolTipTextByIndex >::invoke")
		->args({"self","NameIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_572 = das::das_call_member< bool (UEnum::*)(const wchar_t *,int) const,&UEnum::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2268:23
	makeExtern<DAS_CALL_METHOD(_method_572), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (UEnum::*)(const wchar_t *,int) const , &UEnum::HasMetaData >::invoke")
		->args({"self","Key","NameIndex"})
		->arg_init(2,make_smart<ExprConstInt>(-1))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_573 = das::das_call_member< FString (UEnum::*)(const wchar_t *,int,bool) const,&UEnum::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2279:26
	makeExtern<DAS_CALL_METHOD(_method_573), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetMetaData","das_call_member< FString (UEnum::*)(const wchar_t *,int,bool) const , &UEnum::GetMetaData >::invoke")
		->args({"self","Key","NameIndex","bAllowRemap"})
		->arg_init(2,make_smart<ExprConstInt>(-1))
		->arg_init(3,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_574 = das::das_call_member< void (UEnum::*)(const wchar_t *,const wchar_t *,int) const,&UEnum::SetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2289:23
	makeExtern<DAS_CALL_METHOD(_method_574), SimNode_ExtFuncCall >(lib,"SetMetaData","das_call_member< void (UEnum::*)(const wchar_t *,const wchar_t *,int) const , &UEnum::SetMetaData >::invoke")
		->args({"self","Key","InValue","NameIndex"})
		->arg_init(3,make_smart<ExprConstInt>(-1))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_575 = das::das_call_member< void (UEnum::*)(const wchar_t *,int) const,&UEnum::RemoveMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2298:23
	makeExtern<DAS_CALL_METHOD(_method_575), SimNode_ExtFuncCall >(lib,"RemoveMetaData","das_call_member< void (UEnum::*)(const wchar_t *,int) const , &UEnum::RemoveMetaData >::invoke")
		->args({"self","Key","NameIndex"})
		->arg_init(2,make_smart<ExprConstInt>(-1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2512:31
	makeExtern< void (*)(UEnum &) , _dasUnreal_virtual_177_BeginDestroy , SimNode_ExtFuncCall >(lib,"BeginDestroy","_dasUnreal_virtual_177_BeginDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2648:15
	makeExtern< bool (*)(const FCppClassTypeInfo &) , _dasUnreal_virtual_178_IsAbstract , SimNode_ExtFuncCall >(lib,"IsAbstract","_dasUnreal_virtual_178_IsAbstract")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FImplementedInterface>(*this,lib,"FImplementedInterface","FImplementedInterface");
	addCtorAndUsing<FImplementedInterface,UClass *,int,bool>(*this,lib,"FImplementedInterface","FImplementedInterface")
		->args({"InClass","InOffset","InImplementedByK2"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2728:2
	makeExtern< UClass * (*)(const UClass *) , _dasUnreal_static_179_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_179_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UClass*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2728:2
	makeExtern< const wchar_t * (*)(const UClass *) , _dasUnreal_static_180_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_180_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UClass*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2728:2
	makeExtern< void (*)(const UClass *) , _dasUnreal_static_181_StaticRegisterNativesUClass , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUClass","_dasUnreal_static_181_StaticRegisterNativesUClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UClass*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_581 = das::das_call_member< UPackage * (UClass::*)() const,&UClass::GetOuterUPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2729:2
	makeExtern<DAS_CALL_METHOD(_method_581), SimNode_ExtFuncCall >(lib,"GetOuterUPackage","das_call_member< UPackage * (UClass::*)() const , &UClass::GetOuterUPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_582 = das::das_call_member< void (UClass::*)(),&UClass::DestroyPropertiesPendingDestruction >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2772:23
	makeExtern<DAS_CALL_METHOD(_method_582), SimNode_ExtFuncCall >(lib,"DestroyPropertiesPendingDestruction","das_call_member< void (UClass::*)() , &UClass::DestroyPropertiesPendingDestruction >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2780:15
	makeExtern< void (*)(UClass &,FUObjectSerializeContext *) , _dasUnreal_virtual_182_ConditionalRecompileClass , SimNode_ExtFuncCall >(lib,"ConditionalRecompileClass","_dasUnreal_virtual_182_ConditionalRecompileClass")
		->args({"self","InLoadContext"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2781:15
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_183_FlushCompilationQueueForLevel , SimNode_ExtFuncCall >(lib,"FlushCompilationQueueForLevel","_dasUnreal_virtual_183_FlushCompilationQueueForLevel")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_585 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsFunctionHidden >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2796:23
	makeExtern<DAS_CALL_METHOD(_method_585), SimNode_ExtFuncCall >(lib,"IsFunctionHidden","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsFunctionHidden >::invoke")
		->args({"self","InFunction"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_586 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsAutoExpandCategory >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2798:23
	makeExtern<DAS_CALL_METHOD(_method_586), SimNode_ExtFuncCall >(lib,"IsAutoExpandCategory","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsAutoExpandCategory >::invoke")
		->args({"self","InCategory"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_587 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsPrioritizeCategory >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2800:23
	makeExtern<DAS_CALL_METHOD(_method_587), SimNode_ExtFuncCall >(lib,"IsPrioritizeCategory","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsPrioritizeCategory >::invoke")
		->args({"self","InCategory"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_588 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsAutoCollapseCategory >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2802:23
	makeExtern<DAS_CALL_METHOD(_method_588), SimNode_ExtFuncCall >(lib,"IsAutoCollapseCategory","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsAutoCollapseCategory >::invoke")
		->args({"self","InCategory"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_589 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsClassGroupName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2804:23
	makeExtern<DAS_CALL_METHOD(_method_589), SimNode_ExtFuncCall >(lib,"IsClassGroupName","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsClassGroupName >::invoke")
		->args({"self","InGroupName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_590 = das::das_call_member< void * (UClass::*)(),&UClass::GetOrCreateSparseClassData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2864:8
	makeExtern<DAS_CALL_METHOD(_method_590), SimNode_ExtFuncCall >(lib,"GetOrCreateSparseClassData","das_call_member< void * (UClass::*)() , &UClass::GetOrCreateSparseClassData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_591 = das::das_call_member< UScriptStruct * (UClass::*)() const,&UClass::GetSparseClassDataStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2869:33
	makeExtern<DAS_CALL_METHOD(_method_591), SimNode_ExtFuncCall >(lib,"GetSparseClassDataStruct","das_call_member< UScriptStruct * (UClass::*)() const , &UClass::GetSparseClassDataStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_592 = das::das_call_member< void (UClass::*)(UScriptStruct *),&UClass::SetSparseClassDataStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2871:23
	makeExtern<DAS_CALL_METHOD(_method_592), SimNode_ExtFuncCall >(lib,"SetSparseClassDataStruct","das_call_member< void (UClass::*)(UScriptStruct *) , &UClass::SetSparseClassDataStruct >::invoke")
		->args({"self","InSparseClassDataStruct"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_593 = das::das_call_member< void (UClass::*)(bool),&UClass::ClearSparseClassDataStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2877:23
	makeExtern<DAS_CALL_METHOD(_method_593), SimNode_ExtFuncCall >(lib,"ClearSparseClassDataStruct","das_call_member< void (UClass::*)(bool) , &UClass::ClearSparseClassDataStruct >::invoke")
		->args({"self","bInRecomplingOnLoad"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2880:30
	makeExtern< void (*)(const UClass *) , _dasUnreal_static_184_AssembleReferenceTokenStreams , SimNode_ExtFuncCall >(lib,"AssembleReferenceTokenStreams","_dasUnreal_static_184_AssembleReferenceTokenStreams")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UClass*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2976:34
	makeExtern< UClass * (*)(UClass &) , _dasUnreal_virtual_185_GetAuthoritativeClass , SimNode_ExtFuncCall >(lib,"GetAuthoritativeClass","_dasUnreal_virtual_185_GetAuthoritativeClass")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_596 = das::das_call_member< const UClass * (UClass::*)() const,&UClass::GetAuthoritativeClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2977:16
	makeExtern<DAS_CALL_METHOD(_method_596), SimNode_ExtFuncCall >(lib,"GetAuthoritativeClass","das_call_member< const UClass * (UClass::*)() const , &UClass::GetAuthoritativeClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_597 = das::das_call_member< void (UClass::*)(UFunction *,FName),&UClass::AddFunctionToFunctionMap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2995:7
	makeExtern<DAS_CALL_METHOD(_method_597), SimNode_ExtFuncCall >(lib,"AddFunctionToFunctionMap","das_call_member< void (UClass::*)(UFunction *,FName) , &UClass::AddFunctionToFunctionMap >::invoke")
		->args({"self","Function","FuncName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_598 = das::das_call_member< void (UClass::*)(UFunction *),&UClass::RemoveFunctionFromFunctionMap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3011:7
	makeExtern<DAS_CALL_METHOD(_method_598), SimNode_ExtFuncCall >(lib,"RemoveFunctionFromFunctionMap","das_call_member< void (UClass::*)(UFunction *) , &UClass::RemoveFunctionFromFunctionMap >::invoke")
		->args({"self","Function"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_599 = das::das_call_member< void (UClass::*)(),&UClass::ClearFunctionMapsCaches >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3025:23
	makeExtern<DAS_CALL_METHOD(_method_599), SimNode_ExtFuncCall >(lib,"ClearFunctionMapsCaches","das_call_member< void (UClass::*)() , &UClass::ClearFunctionMapsCaches >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_600 = das::das_call_member< UFunction * (UClass::*)(FName,EIncludeSuperFlag::Type) const,&UClass::FindFunctionByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3028:29
	makeExtern<DAS_CALL_METHOD(_method_600), SimNode_ExtFuncCall >(lib,"FindFunctionByName","das_call_member< UFunction * (UClass::*)(FName,EIncludeSuperFlag::Type) const , &UClass::FindFunctionByName >::invoke")
		->args({"self","InName","IncludeSuper"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<EIncludeSuperFlag::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3032:31
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_186_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_186_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3033:31
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_187_FinishDestroy , SimNode_ExtFuncCall >(lib,"FinishDestroy","_dasUnreal_virtual_187_FinishDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3034:31
	makeExtern< void (*)(UClass &,UClass *,const wchar_t *,const wchar_t *) , _dasUnreal_virtual_188_DeferredRegister , SimNode_ExtFuncCall >(lib,"DeferredRegister","_dasUnreal_virtual_188_DeferredRegister")
		->args({"self","UClassStaticClass","PackageName","InName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3035:31
	makeExtern< bool (*)(UClass &,const wchar_t *,UObject *,unsigned int) , _dasUnreal_virtual_189_Rename , SimNode_ExtFuncCall >(lib,"Rename","_dasUnreal_virtual_189_Rename")
		->args({"self","NewName","NewOuter","Flags"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->arg_init(2,make_smart<ExprConstPtr>())
		->arg_init(3,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3036:31
	makeExtern< void (*)(UClass &,EObjectFlags) , _dasUnreal_virtual_190_TagSubobjects , SimNode_ExtFuncCall >(lib,"TagSubobjects","_dasUnreal_virtual_190_TagSubobjects")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3037:31
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_191_PostInitProperties , SimNode_ExtFuncCall >(lib,"PostInitProperties","_dasUnreal_virtual_191_PostInitProperties")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3039:55
	makeExtern< FRestoreForUObjectOverwrite * (*)(UClass &) , _dasUnreal_virtual_192_GetRestoreForUObjectOverwrite , SimNode_ExtFuncCall >(lib,"GetRestoreForUObjectOverwrite","_dasUnreal_virtual_192_GetRestoreForUObjectOverwrite")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3040:34
	makeExtern< FString (*)(UClass &) , _dasUnreal_virtual_193_GetDesc , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDesc","_dasUnreal_virtual_193_GetDesc")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3045:15
	makeExtern< bool (*)(const UClass &) , _dasUnreal_virtual_194_IsAsset , SimNode_ExtFuncCall >(lib,"IsAsset","_dasUnreal_virtual_194_IsAsset")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3046:15
	makeExtern< bool (*)(const UClass &) , _dasUnreal_virtual_195_IsNameStableForNetworking , SimNode_ExtFuncCall >(lib,"IsNameStableForNetworking","_dasUnreal_virtual_195_IsNameStableForNetworking")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3051:31
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_196_Bind , SimNode_ExtFuncCall >(lib,"Bind","_dasUnreal_virtual_196_Bind")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3052:39
	makeExtern< const wchar_t * (*)(const UClass &) , _dasUnreal_virtual_197_GetPrefixCPP , SimNode_ExtFuncCall >(lib,"GetPrefixCPP","_dasUnreal_virtual_197_GetPrefixCPP")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3057:31
	makeExtern< void (*)(UClass &,UStruct *) , _dasUnreal_virtual_198_SetSuperStruct , SimNode_ExtFuncCall >(lib,"SetSuperStruct","_dasUnreal_virtual_198_SetSuperStruct")
		->args({"self","NewSuperStruct"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3058:31
	makeExtern< bool (*)(const UClass &) , _dasUnreal_virtual_199_IsStructTrashed , SimNode_ExtFuncCall >(lib,"IsStructTrashed","_dasUnreal_virtual_199_IsStructTrashed")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_615 = das::das_call_member< const ICppClassTypeInfo * (UClass::*)() const,&UClass::GetCppTypeInfo >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3178:27
	makeExtern<DAS_CALL_METHOD(_method_615), SimNode_ExtFuncCall >(lib,"GetCppTypeInfo","das_call_member< const ICppClassTypeInfo * (UClass::*)() const , &UClass::GetCppTypeInfo >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_616 = das::das_call_member< void (UClass::*)(const FCppClassTypeInfoStatic *),&UClass::SetCppTypeInfoStatic >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3185:7
	makeExtern<DAS_CALL_METHOD(_method_616), SimNode_ExtFuncCall >(lib,"SetCppTypeInfoStatic","das_call_member< void (UClass::*)(const FCppClassTypeInfoStatic *) , &UClass::SetCppTypeInfoStatic >::invoke")
		->args({"self","InCppTypeInfoStatic"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_617 = das::das_call_member< const FString (UClass::*)() const,&UClass::GetConfigName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3201:32
	makeExtern<DAS_CALL_METHOD(_method_617), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetConfigName","das_call_member< const FString (UClass::*)() const , &UClass::GetConfigName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_618 = das::das_call_member< UClass * (UClass::*)() const,&UClass::GetSuperClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3204:10
	makeExtern<DAS_CALL_METHOD(_method_618), SimNode_ExtFuncCall >(lib,"GetSuperClass","das_call_member< UClass * (UClass::*)() const , &UClass::GetSuperClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_619 = das::das_call_member< int (UClass::*)(),&UClass::GetDefaultsCount >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3213:8
	makeExtern<DAS_CALL_METHOD(_method_619), SimNode_ExtFuncCall >(lib,"GetDefaultsCount","das_call_member< int (UClass::*)() , &UClass::GetDefaultsCount >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_620 = das::das_call_member< UObject * (UClass::*)(bool) const,&UClass::GetDefaultObject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3223:11
	makeExtern<DAS_CALL_METHOD(_method_620), SimNode_ExtFuncCall >(lib,"GetDefaultObject","das_call_member< UObject * (UClass::*)(bool) const , &UClass::GetDefaultObject >::invoke")
		->args({"self","bCreateIfNeeded"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3241:15
	makeExtern< void (*)(UClass &,UObject *) , _dasUnreal_virtual_200_PostLoadInstance , SimNode_ExtFuncCall >(lib,"PostLoadInstance","_dasUnreal_virtual_200_PostLoadInstance")
		->args({"self","InObj"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3250:15
	makeExtern< void (*)(UClass &,unsigned char *,const unsigned char *) , _dasUnreal_virtual_201_InitPropertiesFromCustomList , SimNode_ExtFuncCall >(lib,"InitPropertiesFromCustomList","_dasUnreal_virtual_201_InitPropertiesFromCustomList")
		->args({"self","DataPtr","DefaultDataPtr"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3260:15
	makeExtern< bool (*)(const UClass &) , _dasUnreal_virtual_202_CanCreateAssetOfClass , SimNode_ExtFuncCall >(lib,"CanCreateAssetOfClass","_dasUnreal_virtual_202_CanCreateAssetOfClass")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_624 = das::das_call_member< FName (UClass::*)() const,&UClass::GetDefaultObjectName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3269:24
	makeExtern<DAS_CALL_METHOD(_method_624), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDefaultObjectName","das_call_member< FName (UClass::*)() const , &UClass::GetDefaultObjectName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3272:17
	makeExtern< unsigned char * (*)(const UClass &,UObject *,UFunction *) , _dasUnreal_virtual_203_GetPersistentUberGraphFrame , SimNode_ExtFuncCall >(lib,"GetPersistentUberGraphFrame","_dasUnreal_virtual_203_GetPersistentUberGraphFrame")
		->args({"self","Obj","FuncToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3278:15
	makeExtern< void (*)(const UClass &,UObject *,bool,bool,UClass *) , _dasUnreal_virtual_204_CreatePersistentUberGraphFrame , SimNode_ExtFuncCall >(lib,"CreatePersistentUberGraphFrame","_dasUnreal_virtual_204_CreatePersistentUberGraphFrame")
		->args({"self","Obj","bCreateOnlyIfEmpty","bSkipSuperClass","OldClass"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->arg_init(3,make_smart<ExprConstBool>(false))
		->arg_init(4,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3283:15
	makeExtern< void (*)(const UClass &,UObject *,bool) , _dasUnreal_virtual_205_DestroyPersistentUberGraphFrame , SimNode_ExtFuncCall >(lib,"DestroyPersistentUberGraphFrame","_dasUnreal_virtual_205_DestroyPersistentUberGraphFrame")
		->args({"self","Obj","bSkipSuperClass"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_628 = das::das_call_member< UObject * (UClass::*)(FName),&UClass::GetDefaultSubobjectByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3300:27
	makeExtern<DAS_CALL_METHOD(_method_628), SimNode_ExtFuncCall >(lib,"GetDefaultSubobjectByName","das_call_member< UObject * (UClass::*)(FName) , &UClass::GetDefaultSubobjectByName >::invoke")
		->args({"self","ToFind"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_629 = das::das_call_member< void (UClass::*)(UObject *,const UClass *),&UClass::AddDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3303:7
	makeExtern<DAS_CALL_METHOD(_method_629), SimNode_ExtFuncCall >(lib,"AddDefaultSubobject","das_call_member< void (UClass::*)(UObject *,const UClass *) , &UClass::AddDefaultSubobject >::invoke")
		->args({"self","NewSubobject","BaseClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_630 = das::das_call_member< FString (UClass::*)() const,&UClass::GetDescription >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3369:26
	makeExtern<DAS_CALL_METHOD(_method_630), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDescription","das_call_member< FString (UClass::*)() const , &UClass::GetDescription >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_631 = das::das_call_member< void (UClass::*)(bool),&UClass::AssembleReferenceTokenStream >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3377:23
	makeExtern<DAS_CALL_METHOD(_method_631), SimNode_ExtFuncCall >(lib,"AssembleReferenceTokenStream","das_call_member< void (UClass::*)(bool) , &UClass::AssembleReferenceTokenStream >::invoke")
		->args({"self","bForce"})
		->arg_init(1,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_632 = das::das_call_member< bool (UClass::*)(const UClass *) const,&UClass::ImplementsInterface >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3384:23
	makeExtern<DAS_CALL_METHOD(_method_632), SimNode_ExtFuncCall >(lib,"ImplementsInterface","das_call_member< bool (UClass::*)(const UClass *) const , &UClass::ImplementsInterface >::invoke")
		->args({"self","SomeInterface"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3409:15
	makeExtern< void (*)(UClass &,UObject *) , _dasUnreal_virtual_206_PostLoadDefaultObject , SimNode_ExtFuncCall >(lib,"PostLoadDefaultObject","_dasUnreal_virtual_206_PostLoadDefaultObject")
		->args({"self","Object"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3417:31
	makeExtern< void (*)(UClass &,bool) , _dasUnreal_virtual_207_PurgeClass , SimNode_ExtFuncCall >(lib,"PurgeClass","_dasUnreal_virtual_207_PurgeClass")
		->args({"self","bRecompilingOnLoad"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3426:33
	makeExtern< UClass * (*)(const UClass *,UClass *,UClass *) , _dasUnreal_static_208_FindCommonBase , SimNode_ExtFuncCall >(lib,"FindCommonBase","_dasUnreal_static_208_FindCommonBase")
		->args({"self","InClassA","InClassB"})
		->arg_type(0,makeType<TExplicit<const UClass*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3442:31
	makeExtern< bool (*)(const UClass &,FName) , _dasUnreal_virtual_209_IsFunctionImplementedInScript , SimNode_ExtFuncCall >(lib,"IsFunctionImplementedInScript","_dasUnreal_virtual_209_IsFunctionImplementedInScript")
		->args({"self","InFunctionName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3449:31
	makeExtern< bool (*)(const UClass &,FProperty *) , _dasUnreal_virtual_210_HasProperty , SimNode_ExtFuncCall >(lib,"HasProperty","_dasUnreal_virtual_210_HasProperty")
		->args({"self","InProperty"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3452:19
	makeExtern< UObject * (*)(const UClass &,const UClass *,const FName) , _dasUnreal_virtual_211_FindArchetype , SimNode_ExtFuncCall >(lib,"FindArchetype","_dasUnreal_virtual_211_FindArchetype")
		->args({"self","ArchetypeClass","ArchetypeName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3455:35
	makeExtern< UObject * (*)(const UClass &) , _dasUnreal_virtual_212_GetArchetypeForCDO , SimNode_ExtFuncCall >(lib,"GetArchetypeForCDO","_dasUnreal_virtual_212_GetArchetypeForCDO")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_640 = das::das_call_member< const void * (UClass::*)() const,&UClass::GetArchetypeForSparseClassData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3458:30
	makeExtern<DAS_CALL_METHOD(_method_640), SimNode_ExtFuncCall >(lib,"GetArchetypeForSparseClassData","das_call_member< const void * (UClass::*)() const , &UClass::GetArchetypeForSparseClassData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_641 = das::das_call_member< UScriptStruct * (UClass::*)() const,&UClass::GetSparseClassDataArchetypeStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3461:33
	makeExtern<DAS_CALL_METHOD(_method_641), SimNode_ExtFuncCall >(lib,"GetSparseClassDataArchetypeStruct","das_call_member< UScriptStruct * (UClass::*)() const , &UClass::GetSparseClassDataArchetypeStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_642 = das::das_call_member< bool (UClass::*)() const,&UClass::OverridesSparseClassDataArchetype >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3464:23
	makeExtern<DAS_CALL_METHOD(_method_642), SimNode_ExtFuncCall >(lib,"OverridesSparseClassDataArchetype","das_call_member< bool (UClass::*)() const , &UClass::OverridesSparseClassDataArchetype >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_643 = das::das_call_member< void (UClass::*)(),&UClass::SetUpRuntimeReplicationData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3481:23
	makeExtern<DAS_CALL_METHOD(_method_643), SimNode_ExtFuncCall >(lib,"SetUpRuntimeReplicationData","das_call_member< void (UClass::*)() , &UClass::SetUpRuntimeReplicationData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3486:30
	makeExtern< bool (*)(const UClass *,UClass *) , _dasUnreal_static_213_IsSafeToSerializeToStructuredArchives , SimNode_ExtFuncCall >(lib,"IsSafeToSerializeToStructuredArchives","_dasUnreal_static_213_IsSafeToSerializeToStructuredArchives")
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
}
}

