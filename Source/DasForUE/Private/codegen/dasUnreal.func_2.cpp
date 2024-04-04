// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_2() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2238:13
	makeExtern< void (*)(int &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2239:13
	makeExtern< void (*)(long long &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2240:13
	makeExtern< void (*)(unsigned char &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2241:13
	makeExtern< void (*)(unsigned short &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2242:13
	makeExtern< void (*)(unsigned int &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2243:13
	makeExtern< void (*)(unsigned long long &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2244:13
	makeExtern< void (*)(float &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2245:13
	makeExtern< void (*)(double &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2246:13
	makeExtern< void (*)(bool &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2247:13
	makeExtern< void (*)(FString &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2265:29
	makeExtern< FString (*)(bool) , LexToString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"LexToString","LexToString")
		->args({"Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2275:34
	makeExtern< FString (*)(const FString &) , LexToString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"LexToString","LexToString")
		->args({"Str"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2288:29
	makeExtern< FString (*)(float) , LexToSanitizedString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"LexToSanitizedString","LexToSanitizedString")
		->args({"Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2294:29
	makeExtern< FString (*)(double) , LexToSanitizedString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"LexToSanitizedString","LexToSanitizedString")
		->args({"Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2338:13
	makeExtern< bool (*)(bool &,const wchar_t *) , LexTryParseString , SimNode_ExtFuncCall >(lib,"LexTryParseString","LexTryParseString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2369:26
	makeExtern< const wchar_t * (*)(const FString &) , ToCStr , SimNode_ExtFuncCall >(lib,"ToCStr","ToCStr")
		->args({"Str"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2377:16
	makeExtern< void (*)(FString &) , StringConv::InlineCombineSurrogates , SimNode_ExtFuncCall >(lib,"InlineCombineSurrogates","StringConv::InlineCombineSurrogates")
		->args({"Str"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2522:16
	makeExtern< int (*)(const FString &,const int) , FindMatchingClosingParenthesis , SimNode_ExtFuncCall >(lib,"FindMatchingClosingParenthesis","FindMatchingClosingParenthesis")
		->args({"TargetString","StartSearch"})
		->arg_init(1,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2532:18
	makeExtern< FString (*)(const FString &,const wchar_t *) , SlugStringForValidName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"SlugStringForValidName","SlugStringForValidName")
		->args({"DisplayString","ReplaceWith"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FTextRange>(*this,lib,"FTextRange","FTextRange");
	addCtorAndUsing<FTextRange,int,int>(*this,lib,"FTextRange","FTextRange")
		->args({"InBeginIndex","InEndIndex"});
	using _method_76 = das::das_call_member< int (FTextRange::*)() const,&FTextRange::Len >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2569:8
	makeExtern<DAS_CALL_METHOD(_method_76), SimNode_ExtFuncCall >(lib,"Len","das_call_member< int (FTextRange::*)() const , &FTextRange::Len >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_77 = das::das_call_member< bool (FTextRange::*)() const,&FTextRange::IsEmpty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2570:7
	makeExtern<DAS_CALL_METHOD(_method_77), SimNode_ExtFuncCall >(lib,"IsEmpty","das_call_member< bool (FTextRange::*)() const , &FTextRange::IsEmpty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_78 = das::das_call_member< void (FTextRange::*)(int),&FTextRange::Offset >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2571:7
	makeExtern<DAS_CALL_METHOD(_method_78), SimNode_ExtFuncCall >(lib,"Offset","das_call_member< void (FTextRange::*)(int) , &FTextRange::Offset >::invoke")
		->args({"self","Amount"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_79 = das::das_call_member< bool (FTextRange::*)(int) const,&FTextRange::Contains >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2572:7
	makeExtern<DAS_CALL_METHOD(_method_79), SimNode_ExtFuncCall >(lib,"Contains","das_call_member< bool (FTextRange::*)(int) const , &FTextRange::Contains >::invoke")
		->args({"self","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_80 = das::das_call_member< bool (FTextRange::*)(int) const,&FTextRange::InclusiveContains >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2573:7
	makeExtern<DAS_CALL_METHOD(_method_80), SimNode_ExtFuncCall >(lib,"InclusiveContains","das_call_member< bool (FTextRange::*)(int) const , &FTextRange::InclusiveContains >::invoke")
		->args({"self","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_81 = das::das_call_member< FTextRange (FTextRange::*)(const FTextRange &) const,&FTextRange::Intersect >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2575:13
	makeExtern<DAS_CALL_METHOD(_method_81), SimNode_ExtFuncCallAndCopyOrMove >(lib,"Intersect","das_call_member< FTextRange (FTextRange::*)(const FTextRange &) const , &FTextRange::Intersect >::invoke")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FVTableHelper>(*this,lib,"FVTableHelper","FVTableHelper");
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:516:30
	makeExtern< const wchar_t * (*)(EPropertyObjectReferenceType) , LexToString , SimNode_ExtFuncCall >(lib,"LexToString","LexToString")
		->args({"Type"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_82 = das::das_call_member< bool (UObjectBase::*)() const,&UObjectBase::IsValidLowLevel >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:122:23
	makeExtern<DAS_CALL_METHOD(_method_82), SimNode_ExtFuncCall >(lib,"IsValidLowLevel","das_call_member< bool (UObjectBase::*)() const , &UObjectBase::IsValidLowLevel >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_83 = das::das_call_member< bool (UObjectBase::*)(bool) const,&UObjectBase::IsValidLowLevelFast >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:131:23
	makeExtern<DAS_CALL_METHOD(_method_83), SimNode_ExtFuncCall >(lib,"IsValidLowLevelFast","das_call_member< bool (UObjectBase::*)(bool) const , &UObjectBase::IsValidLowLevelFast >::invoke")
		->args({"self","bRecursive"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_84 = das::das_call_member< unsigned int (UObjectBase::*)() const,&UObjectBase::GetUniqueID >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:137:21
	makeExtern<DAS_CALL_METHOD(_method_84), SimNode_ExtFuncCall >(lib,"GetUniqueID","das_call_member< unsigned int (UObjectBase::*)() const , &UObjectBase::GetUniqueID >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_85 = das::das_call_member< UObject * (UObjectBase::*)() const,&UObjectBase::GetOuter >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:149:23
	makeExtern<DAS_CALL_METHOD(_method_85), SimNode_ExtFuncCall >(lib,"GetOuter","das_call_member< UObject * (UObjectBase::*)() const , &UObjectBase::GetOuter >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_86 = das::das_call_member< FName (UObjectBase::*)() const,&UObjectBase::GetFName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:155:20
	makeExtern<DAS_CALL_METHOD(_method_86), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFName","das_call_member< FName (UObjectBase::*)() const , &UObjectBase::GetFName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:161:32
	makeExtern< FName (*)(const UObjectBase &) , _dasUnreal_virtual_9_GetFNameForStatID , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFNameForStatID","_dasUnreal_virtual_9_GetFNameForStatID")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:164:33
	makeExtern< FString (*)(const UObjectBase &,const wchar_t *) , _dasUnreal_static_10_RemoveClassPrefix , SimNode_ExtFuncCallAndCopyOrMove >(lib,"RemoveClassPrefix","_dasUnreal_static_10_RemoveClassPrefix")
		->args({"self","ClassName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_89 = das::das_call_member< UPackage * (UObjectBase::*)() const,&UObjectBase::GetExternalPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:167:28
	makeExtern<DAS_CALL_METHOD(_method_89), SimNode_ExtFuncCall >(lib,"GetExternalPackage","das_call_member< UPackage * (UObjectBase::*)() const , &UObjectBase::GetExternalPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_90 = das::das_call_member< void (UObjectBase::*)(UPackage *),&UObjectBase::SetExternalPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:170:23
	makeExtern<DAS_CALL_METHOD(_method_90), SimNode_ExtFuncCall >(lib,"SetExternalPackage","das_call_member< void (UObjectBase::*)(UPackage *) , &UObjectBase::SetExternalPackage >::invoke")
		->args({"self","InPackage"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_91 = das::das_call_member< UPackage * (UObjectBase::*)() const,&UObjectBase::GetExternalPackageInternal >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:173:28
	makeExtern<DAS_CALL_METHOD(_method_91), SimNode_ExtFuncCall >(lib,"GetExternalPackageInternal","das_call_member< UPackage * (UObjectBase::*)() const , &UObjectBase::GetExternalPackageInternal >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_92 = das::das_call_member< EObjectFlags (UObjectBase::*)() const,&UObjectBase::GetFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:193:27
	makeExtern<DAS_CALL_METHOD(_method_92), SimNode_ExtFuncCall >(lib,"GetFlags","das_call_member< EObjectFlags (UObjectBase::*)() const , &UObjectBase::GetFlags >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_93 = das::das_call_member< void (UObjectBase::*)(EObjectFlags),&UObjectBase::AtomicallySetFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:203:21
	makeExtern<DAS_CALL_METHOD(_method_93), SimNode_ExtFuncCall >(lib,"AtomicallySetFlags","das_call_member< void (UObjectBase::*)(EObjectFlags) , &UObjectBase::AtomicallySetFlags >::invoke")
		->args({"self","FlagsToAdd"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_94 = das::das_call_member< void (UObjectBase::*)(EObjectFlags),&UObjectBase::AtomicallyClearFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:220:21
	makeExtern<DAS_CALL_METHOD(_method_94), SimNode_ExtFuncCall >(lib,"AtomicallyClearFlags","das_call_member< void (UObjectBase::*)(EObjectFlags) , &UObjectBase::AtomicallyClearFlags >::invoke")
		->args({"self","FlagsToClear"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:234:14
	makeExtern< void (*)(const UObjectBase &,UObject *) , _dasUnreal_static_11_PrefetchClass , SimNode_ExtFuncCall >(lib,"PrefetchClass","_dasUnreal_static_11_PrefetchClass")
		->args({"self","Object"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:235:14
	makeExtern< void (*)(const UObjectBase &,UObject *) , _dasUnreal_static_12_PrefetchOuter , SimNode_ExtFuncCall >(lib,"PrefetchOuter","_dasUnreal_static_12_PrefetchOuter")
		->args({"self","Object"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:276:22
	makeExtern< bool (*)() , UObjectInitialized , SimNode_ExtFuncCall >(lib,"UObjectInitialized","UObjectInitialized")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:281:22
	makeExtern< void (*)(UObjectBase *,bool) , UObjectForceRegistration , SimNode_ExtFuncCall >(lib,"UObjectForceRegistration","UObjectForceRegistration")
		->args({"Object","bCheckForModuleRelease"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:451:30
	makeExtern< UEnum * (*)(UObject *,const wchar_t *,SIZE_T,unsigned int,bool) , FindExistingEnumIfHotReloadOrDynamic , SimNode_ExtFuncCall >(lib,"FindExistingEnumIfHotReloadOrDynamic","FindExistingEnumIfHotReloadOrDynamic")
		->args({"Outer","EnumName","Size","Crc","bIsDynamic"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:454:22
	makeExtern< void (*)(FName,bool) , ProcessNewlyLoadedUObjects , SimNode_ExtFuncCall >(lib,"ProcessNewlyLoadedUObjects","ProcessNewlyLoadedUObjects")
		->args({"Package","bCanProcessNewlyLoadedObjects"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectMarks.h:67:22
	makeExtern< void (*)(const UObjectBase *,EObjectMark) , MarkObject , SimNode_ExtFuncCall >(lib,"MarkObject","MarkObject")
		->args({"Object","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectMarks.h:75:22
	makeExtern< void (*)(const UObjectBase *,EObjectMark) , UnMarkObject , SimNode_ExtFuncCall >(lib,"UnMarkObject","UnMarkObject")
		->args({"Object","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectMarks.h:83:22
	makeExtern< void (*)(EObjectMark) , MarkAllObjects , SimNode_ExtFuncCall >(lib,"MarkAllObjects","MarkAllObjects")
		->args({"Marks"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectMarks.h:91:22
	makeExtern< void (*)(EObjectMark) , UnMarkAllObjects , SimNode_ExtFuncCall >(lib,"UnMarkAllObjects","UnMarkAllObjects")
		->args({"Marks"})
		->arg_init(0,make_smart<ExprConstEnumeration>(-1,makeType<EObjectMark>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectMarks.h:100:22
	makeExtern< bool (*)(const UObjectBase *,EObjectMark) , ObjectHasAnyMarks , SimNode_ExtFuncCall >(lib,"ObjectHasAnyMarks","ObjectHasAnyMarks")
		->args({"Object","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectMarks.h:109:22
	makeExtern< bool (*)(const UObjectBase *,EObjectMark) , ObjectHasAllMarks , SimNode_ExtFuncCall >(lib,"ObjectHasAllMarks","ObjectHasAllMarks")
		->args({"Object","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectMarks.h:117:29
	makeExtern< EObjectMark (*)(const UObjectBase *) , ObjectGetAllMarks , SimNode_ExtFuncCall >(lib,"ObjectGetAllMarks","ObjectGetAllMarks")
		->args({"Object"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<UObjectBaseUtility>(*this,lib,"UObjectBaseUtility","UObjectBaseUtility");
	addCtorAndUsing<UObjectBaseUtility,EObjectFlags>(*this,lib,"UObjectBaseUtility","UObjectBaseUtility")
		->args({"InFlags"});
	using _method_97 = das::das_call_member< void (UObjectBaseUtility::*)(EObjectFlags),&UObjectBaseUtility::SetFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:100:19
	makeExtern<DAS_CALL_METHOD(_method_97), SimNode_ExtFuncCall >(lib,"SetFlags","das_call_member< void (UObjectBaseUtility::*)(EObjectFlags) , &UObjectBaseUtility::SetFlags >::invoke")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_98 = das::das_call_member< void (UObjectBaseUtility::*)(EObjectFlags),&UObjectBaseUtility::ClearFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:108:19
	makeExtern<DAS_CALL_METHOD(_method_98), SimNode_ExtFuncCall >(lib,"ClearFlags","das_call_member< void (UObjectBaseUtility::*)(EObjectFlags) , &UObjectBaseUtility::ClearFlags >::invoke")
		->args({"self","FlagsToClear"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_99 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const,&UObjectBaseUtility::HasAnyFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:121:19
	makeExtern<DAS_CALL_METHOD(_method_99), SimNode_ExtFuncCall >(lib,"HasAnyFlags","das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const , &UObjectBaseUtility::HasAnyFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_100 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const,&UObjectBaseUtility::HasAllFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:133:19
	makeExtern<DAS_CALL_METHOD(_method_100), SimNode_ExtFuncCall >(lib,"HasAllFlags","das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const , &UObjectBaseUtility::HasAllFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_101 = das::das_call_member< EObjectFlags (UObjectBaseUtility::*)(EObjectFlags) const,&UObjectBaseUtility::GetMaskedFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:145:27
	makeExtern<DAS_CALL_METHOD(_method_101), SimNode_ExtFuncCall >(lib,"GetMaskedFlags","das_call_member< EObjectFlags (UObjectBaseUtility::*)(EObjectFlags) const , &UObjectBaseUtility::GetMaskedFlags >::invoke")
		->args({"self","Mask"})
		->arg_init(1,make_smart<ExprConstEnumeration>(-1,makeType<EObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_102 = das::das_call_member< void (UObjectBaseUtility::*)(EObjectMark) const,&UObjectBaseUtility::Mark >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:159:19
	makeExtern<DAS_CALL_METHOD(_method_102), SimNode_ExtFuncCall >(lib,"Mark","das_call_member< void (UObjectBaseUtility::*)(EObjectMark) const , &UObjectBaseUtility::Mark >::invoke")
		->args({"self","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_103 = das::das_call_member< void (UObjectBaseUtility::*)(EObjectMark) const,&UObjectBaseUtility::UnMark >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:169:19
	makeExtern<DAS_CALL_METHOD(_method_103), SimNode_ExtFuncCall >(lib,"UnMark","das_call_member< void (UObjectBaseUtility::*)(EObjectMark) const , &UObjectBaseUtility::UnMark >::invoke")
		->args({"self","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_104 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectMark) const,&UObjectBaseUtility::HasAnyMarks >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:180:19
	makeExtern<DAS_CALL_METHOD(_method_104), SimNode_ExtFuncCall >(lib,"HasAnyMarks","das_call_member< bool (UObjectBaseUtility::*)(EObjectMark) const , &UObjectBaseUtility::HasAnyMarks >::invoke")
		->args({"self","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_105 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectMark) const,&UObjectBaseUtility::HasAllMarks >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:191:19
	makeExtern<DAS_CALL_METHOD(_method_105), SimNode_ExtFuncCall >(lib,"HasAllMarks","das_call_member< bool (UObjectBaseUtility::*)(EObjectMark) const , &UObjectBaseUtility::HasAllMarks >::invoke")
		->args({"self","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_106 = das::das_call_member< EObjectMark (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetAllMarks >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:202:26
	makeExtern<DAS_CALL_METHOD(_method_106), SimNode_ExtFuncCall >(lib,"GetAllMarks","das_call_member< EObjectMark (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetAllMarks >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_107 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsPendingKill >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:211:19
	makeExtern<DAS_CALL_METHOD(_method_107), SimNode_ExtFuncCall >(lib,"IsPendingKill","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsPendingKill >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_108 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::MarkPendingKill >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:231:19
	makeExtern<DAS_CALL_METHOD(_method_108), SimNode_ExtFuncCall >(lib,"MarkPendingKill","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::MarkPendingKill >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_109 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::ClearPendingKill >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:248:19
	makeExtern<DAS_CALL_METHOD(_method_109), SimNode_ExtFuncCall >(lib,"ClearPendingKill","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::ClearPendingKill >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_110 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::MarkAsGarbage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:263:19
	makeExtern<DAS_CALL_METHOD(_method_110), SimNode_ExtFuncCall >(lib,"MarkAsGarbage","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::MarkAsGarbage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_111 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::ClearGarbage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:279:19
	makeExtern<DAS_CALL_METHOD(_method_111), SimNode_ExtFuncCall >(lib,"ClearGarbage","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::ClearGarbage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_112 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::AddToRoot >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:295:19
	makeExtern<DAS_CALL_METHOD(_method_112), SimNode_ExtFuncCall >(lib,"AddToRoot","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::AddToRoot >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_113 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::RemoveFromRoot >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:301:19
	makeExtern<DAS_CALL_METHOD(_method_113), SimNode_ExtFuncCall >(lib,"RemoveFromRoot","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::RemoveFromRoot >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_114 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsRooted >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:311:19
	makeExtern<DAS_CALL_METHOD(_method_114), SimNode_ExtFuncCall >(lib,"IsRooted","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsRooted >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_115 = das::das_call_member< bool (UObjectBaseUtility::*)(),&UObjectBaseUtility::ThisThreadAtomicallyClearedRFUnreachable >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:321:19
	makeExtern<DAS_CALL_METHOD(_method_115), SimNode_ExtFuncCall >(lib,"ThisThreadAtomicallyClearedRFUnreachable","das_call_member< bool (UObjectBaseUtility::*)() , &UObjectBaseUtility::ThisThreadAtomicallyClearedRFUnreachable >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_116 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsUnreachable >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:327:19
	makeExtern<DAS_CALL_METHOD(_method_116), SimNode_ExtFuncCall >(lib,"IsUnreachable","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsUnreachable >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_117 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsPendingKillOrUnreachable >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:334:19
	makeExtern<DAS_CALL_METHOD(_method_117), SimNode_ExtFuncCall >(lib,"IsPendingKillOrUnreachable","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsPendingKillOrUnreachable >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_118 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsNative >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:342:19
	makeExtern<DAS_CALL_METHOD(_method_118), SimNode_ExtFuncCall >(lib,"IsNative","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsNative >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_119 = das::das_call_member< void (UObjectBaseUtility::*)(EInternalObjectFlags) const,&UObjectBaseUtility::SetInternalFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:353:19
	makeExtern<DAS_CALL_METHOD(_method_119), SimNode_ExtFuncCall >(lib,"SetInternalFlags","das_call_member< void (UObjectBaseUtility::*)(EInternalObjectFlags) const , &UObjectBaseUtility::SetInternalFlags >::invoke")
		->args({"self","FlagsToSet"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_120 = das::das_call_member< EInternalObjectFlags (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetInternalFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:368:35
	makeExtern<DAS_CALL_METHOD(_method_120), SimNode_ExtFuncCall >(lib,"GetInternalFlags","das_call_member< EInternalObjectFlags (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetInternalFlags >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_121 = das::das_call_member< bool (UObjectBaseUtility::*)(EInternalObjectFlags) const,&UObjectBaseUtility::HasAnyInternalFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:379:19
	makeExtern<DAS_CALL_METHOD(_method_121), SimNode_ExtFuncCall >(lib,"HasAnyInternalFlags","das_call_member< bool (UObjectBaseUtility::*)(EInternalObjectFlags) const , &UObjectBaseUtility::HasAnyInternalFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_122 = das::das_call_member< void (UObjectBaseUtility::*)(EInternalObjectFlags) const,&UObjectBaseUtility::ClearInternalFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:390:19
	makeExtern<DAS_CALL_METHOD(_method_122), SimNode_ExtFuncCall >(lib,"ClearInternalFlags","das_call_member< void (UObjectBaseUtility::*)(EInternalObjectFlags) const , &UObjectBaseUtility::ClearInternalFlags >::invoke")
		->args({"self","FlagsToClear"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_123 = das::das_call_member< bool (UObjectBaseUtility::*)(EInternalObjectFlags) const,&UObjectBaseUtility::AtomicallyClearInternalFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:405:19
	makeExtern<DAS_CALL_METHOD(_method_123), SimNode_ExtFuncCall >(lib,"AtomicallyClearInternalFlags","das_call_member< bool (UObjectBaseUtility::*)(EInternalObjectFlags) const , &UObjectBaseUtility::AtomicallyClearInternalFlags >::invoke")
		->args({"self","FlagsToClear"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:467:21
	makeExtern< bool (*)(const UObjectBaseUtility &) , _dasUnreal_static_13_IsPendingKillEnabled , SimNode_ExtFuncCall >(lib,"IsPendingKillEnabled","_dasUnreal_static_13_IsPendingKillEnabled")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:473:42
	makeExtern< EInternalObjectFlags (*)(const UObjectBaseUtility &,const EInternalObjectFlags) , _dasUnreal_static_14_FixGarbageOrPendingKillInternalObjectFlags , SimNode_ExtFuncCall >(lib,"FixGarbageOrPendingKillInternalObjectFlags","_dasUnreal_static_14_FixGarbageOrPendingKillInternalObjectFlags")
		->args({"self","InFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:496:15
	makeExtern< bool (*)(const UObjectBaseUtility &) , _dasUnreal_virtual_15_CanBeClusterRoot , SimNode_ExtFuncCall >(lib,"CanBeClusterRoot","_dasUnreal_virtual_15_CanBeClusterRoot")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:506:31
	makeExtern< bool (*)(const UObjectBaseUtility &) , _dasUnreal_virtual_16_CanBeInCluster , SimNode_ExtFuncCall >(lib,"CanBeInCluster","_dasUnreal_virtual_16_CanBeInCluster")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:511:31
	makeExtern< void (*)(UObjectBaseUtility &) , _dasUnreal_virtual_17_CreateCluster , SimNode_ExtFuncCall >(lib,"CreateCluster","_dasUnreal_virtual_17_CreateCluster")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:516:15
	makeExtern< void (*)(UObjectBaseUtility &) , _dasUnreal_virtual_18_OnClusterMarkedAsPendingKill , SimNode_ExtFuncCall >(lib,"OnClusterMarkedAsPendingKill","_dasUnreal_virtual_18_OnClusterMarkedAsPendingKill")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_130 = das::das_call_member< void (UObjectBaseUtility::*)(UObjectBaseUtility *,bool),&UObjectBaseUtility::AddToCluster >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:523:23
	makeExtern<DAS_CALL_METHOD(_method_130), SimNode_ExtFuncCall >(lib,"AddToCluster","das_call_member< void (UObjectBaseUtility::*)(UObjectBaseUtility *,bool) , &UObjectBaseUtility::AddToCluster >::invoke")
		->args({"self","ClusterRootOrObjectFromCluster","bAddAsMutableObject"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_131 = das::das_call_member< FString (UObjectBaseUtility::*)(bool) const,&UObjectBaseUtility::GetFullGroupName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:536:26
	makeExtern<DAS_CALL_METHOD(_method_131), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFullGroupName","das_call_member< FString (UObjectBaseUtility::*)(bool) const , &UObjectBaseUtility::GetFullGroupName >::invoke")
		->args({"self","bStartWithOuter"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_132 = das::das_call_member< FString (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:543:22
	makeExtern<DAS_CALL_METHOD(_method_132), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetName","das_call_member< FString (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_133 = das::das_call_member< void (UObjectBaseUtility::*)(FString &) const,&UObjectBaseUtility::GetName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:549:19
	makeExtern<DAS_CALL_METHOD(_method_133), SimNode_ExtFuncCall >(lib,"GetName","das_call_member< void (UObjectBaseUtility::*)(FString &) const , &UObjectBaseUtility::GetName >::invoke")
		->args({"self","ResultString"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_134 = das::das_call_member< void (UObjectBaseUtility::*)(FString &) const,&UObjectBaseUtility::AppendName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:554:19
	makeExtern<DAS_CALL_METHOD(_method_134), SimNode_ExtFuncCall >(lib,"AppendName","das_call_member< void (UObjectBaseUtility::*)(FString &) const , &UObjectBaseUtility::AppendName >::invoke")
		->args({"self","ResultString"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_135 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsPackageExternal >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:568:23
	makeExtern<DAS_CALL_METHOD(_method_135), SimNode_ExtFuncCall >(lib,"IsPackageExternal","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsPackageExternal >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_136 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::DetachExternalPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:574:23
	makeExtern<DAS_CALL_METHOD(_method_136), SimNode_ExtFuncCall >(lib,"DetachExternalPackage","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::DetachExternalPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_137 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::ReattachExternalPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:580:23
	makeExtern<DAS_CALL_METHOD(_method_137), SimNode_ExtFuncCall >(lib,"ReattachExternalPackage","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::ReattachExternalPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_138 = das::das_call_member< UObject * (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetOutermostObject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:587:27
	makeExtern<DAS_CALL_METHOD(_method_138), SimNode_ExtFuncCall >(lib,"GetOutermostObject","das_call_member< UObject * (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetOutermostObject >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_139 = das::das_call_member< UPackage * (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:594:28
	makeExtern<DAS_CALL_METHOD(_method_139), SimNode_ExtFuncCall >(lib,"GetPackage","das_call_member< UPackage * (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

