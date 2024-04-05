// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_2() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2236:13
	makeExtern< void (*)(signed char &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2237:13
	makeExtern< void (*)(short &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
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
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:231:23
	makeExtern< bool (*)(UObject *) , UE::IsSavingPackage , SimNode_ExtFuncCall >(lib,"IsSavingPackage","UE::IsSavingPackage")
		->args({"InOuter"})
		->arg_init(0,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:240:27
	makeExtern< UPackage * (*)() , GetTransientPackage , SimNode_ExtFuncCall >(lib,"GetTransientPackage","GetTransientPackage")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:243:26
	makeExtern< UObject * (*)(UClass *) , GetTransientOuterForRename , SimNode_ExtFuncCall >(lib,"GetTransientOuterForRename","GetTransientOuterForRename")
		->args({"ForClass"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:250:32
	makeExtern< const FString * (*)(const FString &) , GetIniFilenameFromObjectsReference , SimNode_ExtFuncCall >(lib,"GetIniFilenameFromObjectsReference","GetIniFilenameFromObjectsReference")
		->args({"ObjectsReferenceString"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:261:25
	makeExtern< FString (*)(const FString &,const FString *,bool) , ResolveIniObjectsReference , SimNode_ExtFuncCallAndCopyOrMove >(lib,"ResolveIniObjectsReference","ResolveIniObjectsReference")
		->args({"ObjectReference","IniFilename","bThrow"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->arg_init(2,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:275:22
	makeExtern< bool (*)(UObject *&,FString &,bool,bool,unsigned int,const FLinkerInstancingContext *) , ResolveName , SimNode_ExtFuncCall >(lib,"ResolveName","ResolveName")
		->args({"Outer","ObjectsReferenceString","Create","Throw","LoadFlags","InstancingContext"})
		->arg_init(4,make_smart<ExprConstUInt>(0x0))
		->arg_init(5,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:278:22
	makeExtern< bool (*)(UObject *,unsigned int,const wchar_t *) , SafeLoadError , SimNode_ExtFuncCall >(lib,"SafeLoadError","SafeLoadError")
		->args({"Outer","LoadFlags","ErrorMessage"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:298:26
	makeExtern< UObject * (*)(UClass *,UObject *,FName,bool,bool,EObjectFlags,EInternalObjectFlags) , StaticFindObjectFast , SimNode_ExtFuncCall >(lib,"StaticFindObjectFast","StaticFindObjectFast")
		->args({"Class","InOuter","InName","bExactClass","bAnyPackage","ExclusiveFlags","ExclusiveInternalFlags"})
		->arg_init(5,make_smart<ExprConstEnumeration>(0,makeType<EObjectFlags>(lib)))
		->arg_init(6,make_smart<ExprConstEnumeration>(0,makeType<EInternalObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:313:26
	makeExtern< UObject * (*)(UClass *,UObject *,FName,bool,EObjectFlags,EInternalObjectFlags) , StaticFindObjectFast , SimNode_ExtFuncCall >(lib,"StaticFindObjectFast","StaticFindObjectFast")
		->args({"Class","InOuter","InName","bExactClass","ExclusiveFlags","ExclusiveInternalFlags"})
		->arg_init(3,make_smart<ExprConstBool>(false))
		->arg_init(4,make_smart<ExprConstEnumeration>(0,makeType<EObjectFlags>(lib)))
		->arg_init(5,make_smart<ExprConstEnumeration>(0,makeType<EInternalObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:331:26
	makeExtern< UObject * (*)(UClass *,UObject *,FName,bool,bool,EObjectFlags,EInternalObjectFlags) , StaticFindObjectFastSafe , SimNode_ExtFuncCall >(lib,"StaticFindObjectFastSafe","StaticFindObjectFastSafe")
		->args({"Class","InOuter","InName","bExactClass","bAnyPackage","ExclusiveFlags","ExclusiveInternalFlags"})
		->arg_init(5,make_smart<ExprConstEnumeration>(0,makeType<EObjectFlags>(lib)))
		->arg_init(6,make_smart<ExprConstEnumeration>(0,makeType<EInternalObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:348:26
	makeExtern< UObject * (*)(UClass *,UObject *,FName,bool,EObjectFlags,EInternalObjectFlags) , StaticFindObjectFastSafe , SimNode_ExtFuncCall >(lib,"StaticFindObjectFastSafe","StaticFindObjectFastSafe")
		->args({"Class","InOuter","InName","bExactClass","ExclusiveFlags","ExclusiveInternalFlags"})
		->arg_init(3,make_smart<ExprConstBool>(false))
		->arg_init(4,make_smart<ExprConstEnumeration>(0,makeType<EObjectFlags>(lib)))
		->arg_init(5,make_smart<ExprConstEnumeration>(0,makeType<EInternalObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:360:26
	makeExtern< UObject * (*)(UClass *,UObject *,const wchar_t *,bool) , StaticFindObject , SimNode_ExtFuncCall >(lib,"StaticFindObject","StaticFindObject")
		->args({"Class","InOuter","Name","ExactClass"})
		->arg_init(3,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:374:26
	makeExtern< UObject * (*)(UClass *,UObject *,const wchar_t *,bool) , StaticFindObjectChecked , SimNode_ExtFuncCall >(lib,"StaticFindObjectChecked","StaticFindObjectChecked")
		->args({"Class","InOuter","Name","ExactClass"})
		->arg_init(3,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:377:26
	makeExtern< UObject * (*)(UClass *,UObject *,const wchar_t *,bool) , StaticFindObjectSafe , SimNode_ExtFuncCall >(lib,"StaticFindObjectSafe","StaticFindObjectSafe")
		->args({"Class","InOuter","Name","ExactClass"})
		->arg_init(3,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:483:26
	makeExtern< UObject * (*)(UClass *,const wchar_t *,EFindFirstObjectOptions,ELogVerbosity::Type,const wchar_t *) , StaticFindFirstObject , SimNode_ExtFuncCall >(lib,"StaticFindFirstObject","StaticFindFirstObject")
		->args({"Class","Name","Options","AmbiguousMessageVerbosity","InCurrentOperation"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EFindFirstObjectOptions>(lib)))
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<ELogVerbosity::Type>(lib)))
		->arg_init(4,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:499:26
	makeExtern< UObject * (*)(UClass *,const wchar_t *,EFindFirstObjectOptions,ELogVerbosity::Type,const wchar_t *) , StaticFindFirstObjectSafe , SimNode_ExtFuncCall >(lib,"StaticFindFirstObjectSafe","StaticFindFirstObjectSafe")
		->args({"Class","Name","Options","AmbiguousMessageVerbosity","InCurrentOperation"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EFindFirstObjectOptions>(lib)))
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<ELogVerbosity::Type>(lib)))
		->arg_init(4,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:523:22
	makeExtern< bool (*)(const wchar_t *,const wchar_t *,UClass *,UObject *&,UObject *,EParseObjectLoadingPolicy,bool *) , ParseObject , SimNode_ExtFuncCall >(lib,"ParseObject","ParseObject")
		->args({"Stream","Match","Class","DestRes","InParent","LoadingPolicy","bInvalidObject"})
		->arg_init(6,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:524:13
	makeExtern< bool (*)(const wchar_t *,const wchar_t *,UClass *,UObject *&,UObject *,bool *) , ParseObject , SimNode_ExtFuncCall >(lib,"ParseObject","ParseObject")
		->args({"Stream","Match","Class","DestRes","InParent","bInvalidObject"})
		->arg_init(5,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:544:26
	makeExtern< UObject * (*)(UClass *,UObject *,const wchar_t *,const wchar_t *,unsigned int,UPackageMap *,bool,const FLinkerInstancingContext *) , StaticLoadObject , SimNode_ExtFuncCall >(lib,"StaticLoadObject","StaticLoadObject")
		->args({"Class","InOuter","Name","Filename","LoadFlags","Sandbox","bAllowObjectReconciliation","InstancingContext"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->arg_init(4,make_smart<ExprConstUInt>(0x0))
		->arg_init(5,make_smart<ExprConstPtr>())
		->arg_init(6,make_smart<ExprConstBool>(true))
		->arg_init(7,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:547:25
	makeExtern< UClass * (*)(UClass *,UObject *,const wchar_t *,const wchar_t *,unsigned int,UPackageMap *) , StaticLoadClass , SimNode_ExtFuncCall >(lib,"StaticLoadClass","StaticLoadClass")
		->args({"BaseClass","InOuter","Name","Filename","LoadFlags","Sandbox"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->arg_init(4,make_smart<ExprConstUInt>(0x0))
		->arg_init(5,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:558:26
	makeExtern< UObject * (*)(const FStaticConstructObjectParameters &) , StaticConstructObject_Internal , SimNode_ExtFuncCall >(lib,"StaticConstructObject_Internal","StaticConstructObject_Internal")
		->args({"Params"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:577:26
	makeExtern< UObject * (*)(const UObject *,UObject *,const FName,EObjectFlags,UClass *,EDuplicateMode::Type,EInternalObjectFlags) , StaticDuplicateObject , SimNode_ExtFuncCall >(lib,"StaticDuplicateObject","StaticDuplicateObject")
		->args({"SourceObject","DestOuter","DestName","FlagMask","DestClass","DuplicateMode","InternalFlagsMask"})
		->arg_init(3,make_smart<ExprConstEnumeration>(-1,makeType<EObjectFlags>(lib)))
		->arg_init(4,make_smart<ExprConstPtr>())
		->arg_init(5,make_smart<ExprConstEnumeration>(0,makeType<EDuplicateMode::Type>(lib)))
		->arg_init(6,make_smart<ExprConstEnumeration>(-5242880,makeType<EInternalObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:613:22
	makeExtern< void (*)(float,bool,float) , StaticTick , SimNode_ExtFuncCall >(lib,"StaticTick","StaticTick")
		->args({"DeltaTime","bUseFullTimeLimit","AsyncLoadingTime"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->arg_init(2,make_smart<ExprConstFloat>(0.00499999988824129))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:712:22
	makeExtern< void (*)() , CancelAsyncLoading , SimNode_ExtFuncCall >(lib,"CancelAsyncLoading","CancelAsyncLoading")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:719:22
	makeExtern< bool (*)() , IsEventDrivenLoaderEnabledInCookedBuilds , SimNode_ExtFuncCall >(lib,"IsEventDrivenLoaderEnabledInCookedBuilds","IsEventDrivenLoaderEnabledInCookedBuilds")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:724:22
	makeExtern< bool (*)() , IsEventDrivenLoaderEnabled , SimNode_ExtFuncCall >(lib,"IsEventDrivenLoaderEnabled","IsEventDrivenLoaderEnabled")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:733:23
	makeExtern< float (*)(const FName &) , GetAsyncLoadPercentage , SimNode_ExtFuncCall >(lib,"GetAsyncLoadPercentage","GetAsyncLoadPercentage")
		->args({"PackageName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:738:22
	makeExtern< bool (*)() , IsInGarbageCollectorThread , SimNode_ExtFuncCall >(lib,"IsInGarbageCollectorThread","IsInGarbageCollectorThread")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:746:22
	makeExtern< void (*)(EObjectFlags,bool) , CollectGarbage , SimNode_ExtFuncCall >(lib,"CollectGarbage","CollectGarbage")
		->args({"KeepFlags","bPerformFullPurge"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:754:22
	makeExtern< bool (*)(EObjectFlags,bool) , TryCollectGarbage , SimNode_ExtFuncCall >(lib,"TryCollectGarbage","TryCollectGarbage")
		->args({"KeepFlags","bPerformFullPurge"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:764:22
	makeExtern< bool (*)(bool,double) , UnhashUnreachableObjects , SimNode_ExtFuncCall >(lib,"UnhashUnreachableObjects","UnhashUnreachableObjects")
		->args({"bUseTimeLimit","TimeLimit"})
		->arg_init(1,make_smart<ExprConstDouble>(0.00000000000000000))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:771:22
	makeExtern< bool (*)() , IsIncrementalUnhashPending , SimNode_ExtFuncCall >(lib,"IsIncrementalUnhashPending","IsIncrementalUnhashPending")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:778:22
	makeExtern< bool (*)() , IsIncrementalPurgePending , SimNode_ExtFuncCall >(lib,"IsIncrementalPurgePending","IsIncrementalPurgePending")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:785:22
	makeExtern< void (*)(bool) , GatherUnreachableObjects , SimNode_ExtFuncCall >(lib,"GatherUnreachableObjects","GatherUnreachableObjects")
		->args({"bForceSingleThreaded"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:796:22
	makeExtern< void (*)(bool,double) , IncrementalPurgeGarbage , SimNode_ExtFuncCall >(lib,"IncrementalPurgeGarbage","IncrementalPurgeGarbage")
		->args({"bUseTimeLimit","TimeLimit"})
		->arg_init(1,make_smart<ExprConstDouble>(0.00200000000000000))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:818:23
	makeExtern< FName (*)(UObject *,const UClass *,FName,EUniqueObjectNameOptions) , MakeUniqueObjectName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"MakeUniqueObjectName","MakeUniqueObjectName")
		->args({"Outer","Class","BaseName","Options"})
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<EUniqueObjectNameOptions>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:832:23
	makeExtern< FName (*)(const FString &,const FName) , MakeObjectNameFromDisplayLabel , SimNode_ExtFuncCallAndCopyOrMove >(lib,"MakeObjectNameFromDisplayLabel","MakeObjectNameFromDisplayLabel")
		->args({"DisplayLabel","CurrentObjectName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:864:23
	makeExtern< int (*)() , GetNumAsyncPackages , SimNode_ExtFuncCall >(lib,"GetNumAsyncPackages","GetNumAsyncPackages")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:871:22
	makeExtern< bool (*)() , IsLoading , SimNode_ExtFuncCall >(lib,"IsLoading","IsLoading")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:878:22
	makeExtern< void (*)(bool) , SetAsyncLoadingAllowed , SimNode_ExtFuncCall >(lib,"SetAsyncLoadingAllowed","SetAsyncLoadingAllowed")
		->args({"bAllowAsyncLoading"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:905:42
	makeExtern< EAsyncPackageState::Type (*)(bool,bool,double) , ProcessAsyncLoading , SimNode_ExtFuncCall >(lib,"ProcessAsyncLoading","ProcessAsyncLoading")
		->args({"bUseTimeLimit","bUseFullTimeLimit","TimeLimit"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:917:22
	makeExtern< void (*)(FUObjectSerializeContext *,const wchar_t *) , BeginLoad , SimNode_ExtFuncCall >(lib,"BeginLoad","BeginLoad")
		->args({"LoadContext","DebugContext"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:918:22
	makeExtern< void (*)(FUObjectSerializeContext *) , EndLoad , SimNode_ExtFuncCall >(lib,"EndLoad","EndLoad")
		->args({"LoadContext"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:927:27
	makeExtern< UPackage * (*)(UObject *,const wchar_t *) , FindPackage , SimNode_ExtFuncCall >(lib,"FindPackage","FindPackage")
		->args({"InOuter","PackageName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:948:27
	makeExtern< UPackage * (*)(const wchar_t *) , CreatePackage , SimNode_ExtFuncCall >(lib,"CreatePackage","CreatePackage")
		->args({"PackageName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:965:22
	makeExtern< bool (*)(UObject *,bool) , SaveToTransactionBuffer , SimNode_ExtFuncCall >(lib,"SaveToTransactionBuffer","SaveToTransactionBuffer")
		->args({"Object","bMarkDirty"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:975:22
	makeExtern< void (*)(UObject *) , SnapshotTransactionBuffer , SimNode_ExtFuncCall >(lib,"SnapshotTransactionBuffer","SnapshotTransactionBuffer")
		->args({"Object"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1018:26
	makeExtern< UObject * (*)(const UClass *,UObject *,FName,EObjectFlags,EInternalObjectFlags,bool,bool *,UPackage *) , StaticAllocateObject , SimNode_ExtFuncCall >(lib,"StaticAllocateObject","StaticAllocateObject")
		->args({"Class","InOuter","Name","SetFlags","InternalSetFlags","bCanReuseSubobjects","bOutReusedSubobject","ExternalPackage"})
		->arg_init(4,make_smart<ExprConstEnumeration>(0,makeType<EInternalObjectFlags>(lib)))
		->arg_init(5,make_smart<ExprConstBool>(false))
		->arg_init(6,make_smart<ExprConstPtr>())
		->arg_init(7,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FObjectInitializer>(*this,lib,"FObjectInitializer","FObjectInitializer");
	addCtorAndUsing<FObjectInitializer,UObject *,const FStaticConstructObjectParameters &>(*this,lib,"FObjectInitializer","FObjectInitializer")
		->args({"InObj","StaticConstructParams"});
	using _method_82 = das::das_call_member< UObject * (FObjectInitializer::*)() const,&FObjectInitializer::GetArchetype >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1073:23
	makeExtern<DAS_CALL_METHOD(_method_82), SimNode_ExtFuncCall >(lib,"GetArchetype","das_call_member< UObject * (FObjectInitializer::*)() const , &FObjectInitializer::GetArchetype >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_83 = das::das_call_member< UObject * (FObjectInitializer::*)() const,&FObjectInitializer::GetObj >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1081:23
	makeExtern<DAS_CALL_METHOD(_method_83), SimNode_ExtFuncCall >(lib,"GetObj","das_call_member< UObject * (FObjectInitializer::*)() const , &FObjectInitializer::GetObj >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_84 = das::das_call_member< UClass * (FObjectInitializer::*)() const,&FObjectInitializer::GetClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1089:26
	makeExtern<DAS_CALL_METHOD(_method_84), SimNode_ExtFuncCall >(lib,"GetClass","das_call_member< UClass * (FObjectInitializer::*)() const , &FObjectInitializer::GetClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_85 = das::das_call_member< UObject * (FObjectInitializer::*)(UObject *,FName,const UClass *,bool) const,&FObjectInitializer::CreateEditorOnlyDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1155:27
	makeExtern<DAS_CALL_METHOD(_method_85), SimNode_ExtFuncCall >(lib,"CreateEditorOnlyDefaultSubobject","das_call_member< UObject * (FObjectInitializer::*)(UObject *,FName,const UClass *,bool) const , &FObjectInitializer::CreateEditorOnlyDefaultSubobject >::invoke")
		->args({"self","Outer","SubobjectName","ReturnType","bTransient"})
		->arg_init(4,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_86 = das::das_call_member< UObject * (FObjectInitializer::*)(UObject *,FName,const UClass *,const UClass *,bool,bool) const,&FObjectInitializer::CreateDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1166:27
	makeExtern<DAS_CALL_METHOD(_method_86), SimNode_ExtFuncCall >(lib,"CreateDefaultSubobject","das_call_member< UObject * (FObjectInitializer::*)(UObject *,FName,const UClass *,const UClass *,bool,bool) const , &FObjectInitializer::CreateDefaultSubobject >::invoke")
		->args({"self","Outer","SubobjectFName","ReturnType","ClassToCreateByDefault","bIsRequired","bIsTransient"})
		->arg_init(5,make_smart<ExprConstBool>(true))
		->arg_init(6,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_87 = das::das_call_member< const FObjectInitializer & (FObjectInitializer::*)(FName,const UClass *) const,&FObjectInitializer::SetDefaultSubobjectClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1173:28
	makeExtern<DAS_CALL_METHOD(_method_87), SimNode_ExtFuncCallRef >(lib,"SetDefaultSubobjectClass","das_call_member< const FObjectInitializer & (FObjectInitializer::*)(FName,const UClass *) const , &FObjectInitializer::SetDefaultSubobjectClass >::invoke")
		->args({"self","SubobjectName","Class"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_88 = das::das_call_member< const FObjectInitializer & (FObjectInitializer::*)(FName) const,&FObjectInitializer::DoNotCreateDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1194:28
	makeExtern<DAS_CALL_METHOD(_method_88), SimNode_ExtFuncCallRef >(lib,"DoNotCreateDefaultSubobject","das_call_member< const FObjectInitializer & (FObjectInitializer::*)(FName) const , &FObjectInitializer::DoNotCreateDefaultSubobject >::invoke")
		->args({"self","SubobjectName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1270:30
	makeExtern< void (*)(const FObjectInitializer &,UObject *,const wchar_t *) , _dasUnreal_static_9_AssertIfInConstructor , SimNode_ExtFuncCall >(lib,"AssertIfInConstructor","_dasUnreal_static_9_AssertIfInConstructor")
		->args({"self","Outer","ErrorMessage"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_90 = das::das_call_member< void (FObjectInitializer::*)(),&FObjectInitializer::FinalizeSubobjectClassInitialization >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1272:19
	makeExtern<DAS_CALL_METHOD(_method_90), SimNode_ExtFuncCall >(lib,"FinalizeSubobjectClassInitialization","das_call_member< void (FObjectInitializer::*)() , &FObjectInitializer::FinalizeSubobjectClassInitialization >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1278:45
	makeExtern< FObjectInitializer & (*)(const FObjectInitializer &) , _dasUnreal_static_10_Get , SimNode_ExtFuncCallRef >(lib,"Get","_dasUnreal_static_10_Get")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStaticConstructObjectParameters,const UClass *>(*this,lib,"FStaticConstructObjectParameters","FStaticConstructObjectParameters")
		->args({"InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1545:21
	makeExtern< void (*)(const FScriptIntegrationObjectHelper &,const FObjectInitializer &,UObject *,UClass *,UObject *) , _dasUnreal_static_11_InitProperties , SimNode_ExtFuncCall >(lib,"InitProperties","_dasUnreal_static_11_InitProperties")
		->args({"self","ObjectInitializer","Obj","DefaultsClass","DefaultData"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1555:21
	makeExtern< bool (*)(const FScriptIntegrationObjectHelper &,const FObjectInitializer &) , _dasUnreal_static_12_InitSubobjectProperties , SimNode_ExtFuncCall >(lib,"InitSubobjectProperties","_dasUnreal_static_12_InitSubobjectProperties")
		->args({"self","ObjectInitializer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1567:21
	makeExtern< void (*)(const FScriptIntegrationObjectHelper &,const FObjectInitializer &,UClass *,bool,bool) , _dasUnreal_static_13_InstanceSubobjects , SimNode_ExtFuncCall >(lib,"InstanceSubobjects","_dasUnreal_static_13_InstanceSubobjects")
		->args({"self","ObjectInitializer","Class","bNeedInstancing","bNeedSubobjectInstancing"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1578:21
	makeExtern< void (*)(const FScriptIntegrationObjectHelper &,FObjectInitializer &) , _dasUnreal_static_14_PostConstructInitObject , SimNode_ExtFuncCall >(lib,"PostConstructInitObject","_dasUnreal_static_14_PostConstructInitObject")
		->args({"self","ObjectInitializer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1586:22
	makeExtern< void (*)(const UClass *,const UClass *) , CheckIsClassChildOf_Internal , SimNode_ExtFuncCall >(lib,"CheckIsClassChildOf_Internal","CheckIsClassChildOf_Internal")
		->args({"Parent","Child"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1672:26
	makeExtern< UObject * (*)(UClass *,const UObject *,UObject *,const FName) , DuplicateObject_Internal , SimNode_ExtFuncCall >(lib,"DuplicateObject_Internal","DuplicateObject_Internal")
		->args({"Class","SourceObject","Outer","NAME_None"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1698:22
	makeExtern< bool (*)(UObject *) , UsesPerObjectConfig , SimNode_ExtFuncCall >(lib,"UsesPerObjectConfig","UsesPerObjectConfig")
		->args({"SourceObject"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1703:25
	makeExtern< FString (*)(UObject *) , GetConfigFilename , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetConfigFilename","GetConfigFilename")
		->args({"SourceObject"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

