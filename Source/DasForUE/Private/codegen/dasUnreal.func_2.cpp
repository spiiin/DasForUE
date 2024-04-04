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
	using _method_85 = das::das_call_member< FName (UObjectBase::*)() const,&UObjectBase::GetFName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:155:20
	makeExtern<DAS_CALL_METHOD(_method_85), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFName","das_call_member< FName (UObjectBase::*)() const , &UObjectBase::GetFName >::invoke")
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
	using _method_88 = das::das_call_member< UPackage * (UObjectBase::*)() const,&UObjectBase::GetExternalPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:167:28
	makeExtern<DAS_CALL_METHOD(_method_88), SimNode_ExtFuncCall >(lib,"GetExternalPackage","das_call_member< UPackage * (UObjectBase::*)() const , &UObjectBase::GetExternalPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_89 = das::das_call_member< void (UObjectBase::*)(UPackage *),&UObjectBase::SetExternalPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:170:23
	makeExtern<DAS_CALL_METHOD(_method_89), SimNode_ExtFuncCall >(lib,"SetExternalPackage","das_call_member< void (UObjectBase::*)(UPackage *) , &UObjectBase::SetExternalPackage >::invoke")
		->args({"self","InPackage"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_90 = das::das_call_member< UPackage * (UObjectBase::*)() const,&UObjectBase::GetExternalPackageInternal >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:173:28
	makeExtern<DAS_CALL_METHOD(_method_90), SimNode_ExtFuncCall >(lib,"GetExternalPackageInternal","das_call_member< UPackage * (UObjectBase::*)() const , &UObjectBase::GetExternalPackageInternal >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_91 = das::das_call_member< EObjectFlags (UObjectBase::*)() const,&UObjectBase::GetFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:193:27
	makeExtern<DAS_CALL_METHOD(_method_91), SimNode_ExtFuncCall >(lib,"GetFlags","das_call_member< EObjectFlags (UObjectBase::*)() const , &UObjectBase::GetFlags >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_92 = das::das_call_member< void (UObjectBase::*)(EObjectFlags),&UObjectBase::AtomicallySetFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:203:21
	makeExtern<DAS_CALL_METHOD(_method_92), SimNode_ExtFuncCall >(lib,"AtomicallySetFlags","das_call_member< void (UObjectBase::*)(EObjectFlags) , &UObjectBase::AtomicallySetFlags >::invoke")
		->args({"self","FlagsToAdd"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_93 = das::das_call_member< void (UObjectBase::*)(EObjectFlags),&UObjectBase::AtomicallyClearFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:220:21
	makeExtern<DAS_CALL_METHOD(_method_93), SimNode_ExtFuncCall >(lib,"AtomicallyClearFlags","das_call_member< void (UObjectBase::*)(EObjectFlags) , &UObjectBase::AtomicallyClearFlags >::invoke")
		->args({"self","FlagsToClear"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:276:22
	makeExtern< bool (*)() , UObjectInitialized , SimNode_ExtFuncCall >(lib,"UObjectInitialized","UObjectInitialized")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:281:22
	makeExtern< void (*)(UObjectBase *,bool) , UObjectForceRegistration , SimNode_ExtFuncCall >(lib,"UObjectForceRegistration","UObjectForceRegistration")
		->args({"Object","bCheckForModuleRelease"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:454:22
	makeExtern< void (*)(FName,bool) , ProcessNewlyLoadedUObjects , SimNode_ExtFuncCall >(lib,"ProcessNewlyLoadedUObjects","ProcessNewlyLoadedUObjects")
		->args({"Package","bCanProcessNewlyLoadedObjects"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
}
}

