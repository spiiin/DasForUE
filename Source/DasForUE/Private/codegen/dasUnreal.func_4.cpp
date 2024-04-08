// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_4() {
	addCtorAndUsing<FPrimaryAssetType>(*this,lib,"FPrimaryAssetType","FPrimaryAssetType");
	addCtorAndUsing<FPrimaryAssetType,FName>(*this,lib,"FPrimaryAssetType","FPrimaryAssetType")
		->args({"InName"});
	addCtorAndUsing<FPrimaryAssetType,EName>(*this,lib,"FPrimaryAssetType","FPrimaryAssetType")
		->args({"InName"});
	addCtorAndUsing<FPrimaryAssetType,const char *>(*this,lib,"FPrimaryAssetType","FPrimaryAssetType")
		->args({"InName"});
	addCtorAndUsing<FPrimaryAssetType,const FPrimaryAssetType &>(*this,lib,"FPrimaryAssetType","FPrimaryAssetType")
		->args({""});
	using _method_243 = das::das_call_member< FName (FPrimaryAssetType::*)() const,&FPrimaryAssetType::GetName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:41:8
	makeExtern<DAS_CALL_METHOD(_method_243), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetName","das_call_member< FName (FPrimaryAssetType::*)() const , &FPrimaryAssetType::GetName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_244 = das::das_call_member< bool (FPrimaryAssetType::*)() const,&FPrimaryAssetType::IsValid >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:57:7
	makeExtern<DAS_CALL_METHOD(_method_244), SimNode_ExtFuncCall >(lib,"IsValid","das_call_member< bool (FPrimaryAssetType::*)() const , &FPrimaryAssetType::IsValid >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_245 = das::das_call_member< FString (FPrimaryAssetType::*)() const,&FPrimaryAssetType::ToString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:63:10
	makeExtern<DAS_CALL_METHOD(_method_245), SimNode_ExtFuncCallAndCopyOrMove >(lib,"ToString","das_call_member< FString (FPrimaryAssetType::*)() const , &FPrimaryAssetType::ToString >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_246 = das::das_call_member< bool (FPrimaryAssetType::*)(const FPrimaryAssetType &) const,&FPrimaryAssetType::LexicalLess >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:84:7
	makeExtern<DAS_CALL_METHOD(_method_246), SimNode_ExtFuncCall >(lib,"LexicalLess","das_call_member< bool (FPrimaryAssetType::*)(const FPrimaryAssetType &) const , &FPrimaryAssetType::LexicalLess >::invoke")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_247 = das::das_call_member< bool (FPrimaryAssetType::*)(const FPrimaryAssetType &) const,&FPrimaryAssetType::FastLess >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:89:7
	makeExtern<DAS_CALL_METHOD(_method_247), SimNode_ExtFuncCall >(lib,"FastLess","das_call_member< bool (FPrimaryAssetType::*)(const FPrimaryAssetType &) const , &FPrimaryAssetType::FastLess >::invoke")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FPrimaryAssetId>(*this,lib,"FPrimaryAssetId","FPrimaryAssetId");
	addCtorAndUsing<FPrimaryAssetId,FPrimaryAssetType,FName>(*this,lib,"FPrimaryAssetId","FPrimaryAssetId")
		->args({"InAssetType","InAssetName"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:122:41
	makeExtern< FPrimaryAssetId (*)(const FPrimaryAssetId *,const wchar_t *,unsigned int) , _dasUnreal_static_79_ParseTypeAndName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"ParseTypeAndName","_dasUnreal_static_79_ParseTypeAndName")
		->args({"self","TypeAndName","Len"})
		->arg_type(0,makeType<TExplicit<const FPrimaryAssetId*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:123:41
	makeExtern< FPrimaryAssetId (*)(const FPrimaryAssetId *,FName) , _dasUnreal_static_80_ParseTypeAndName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"ParseTypeAndName","_dasUnreal_static_80_ParseTypeAndName")
		->args({"self","TypeAndName"})
		->arg_type(0,makeType<TExplicit<const FPrimaryAssetId*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:124:25
	makeExtern< FPrimaryAssetId (*)(const FPrimaryAssetId *,const FString &) , _dasUnreal_static_81_ParseTypeAndName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"ParseTypeAndName","_dasUnreal_static_81_ParseTypeAndName")
		->args({"self","TypeAndName"})
		->arg_type(0,makeType<TExplicit<const FPrimaryAssetId*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FPrimaryAssetId,const FString &>(*this,lib,"FPrimaryAssetId","FPrimaryAssetId")
		->args({"TypeAndName"});
	addCtorAndUsing<FPrimaryAssetId,const FPrimaryAssetId &>(*this,lib,"FPrimaryAssetId","FPrimaryAssetId")
		->args({""});
	using _method_251 = das::das_call_member< bool (FPrimaryAssetId::*)() const,&FPrimaryAssetId::IsValid >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:139:7
	makeExtern<DAS_CALL_METHOD(_method_251), SimNode_ExtFuncCall >(lib,"IsValid","das_call_member< bool (FPrimaryAssetId::*)() const , &FPrimaryAssetId::IsValid >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_252 = das::das_call_member< FString (FPrimaryAssetId::*)() const,&FPrimaryAssetId::ToString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:145:10
	makeExtern<DAS_CALL_METHOD(_method_252), SimNode_ExtFuncCallAndCopyOrMove >(lib,"ToString","das_call_member< FString (FPrimaryAssetId::*)() const , &FPrimaryAssetId::ToString >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:164:25
	makeExtern< FPrimaryAssetId (*)(const FPrimaryAssetId *,const FString &) , _dasUnreal_static_82_FromString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FromString","_dasUnreal_static_82_FromString")
		->args({"self","String"})
		->arg_type(0,makeType<TExplicit<const FPrimaryAssetId*> >(lib))
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
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:627:27
	makeExtern< UPackage * (*)(UPackage *,const wchar_t *,unsigned int,FArchive *,const FLinkerInstancingContext *) , LoadPackage , SimNode_ExtFuncCall >(lib,"LoadPackage","LoadPackage")
		->args({"InOuter","InLongPackageName","LoadFlags","InReaderOverride","InstancingContext"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->arg_init(4,make_smart<ExprConstPtr>())
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
	using _method_254 = das::das_call_member< UObject * (FObjectInitializer::*)() const,&FObjectInitializer::GetArchetype >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1073:23
	makeExtern<DAS_CALL_METHOD(_method_254), SimNode_ExtFuncCall >(lib,"GetArchetype","das_call_member< UObject * (FObjectInitializer::*)() const , &FObjectInitializer::GetArchetype >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_255 = das::das_call_member< UObject * (FObjectInitializer::*)() const,&FObjectInitializer::GetObj >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1081:23
	makeExtern<DAS_CALL_METHOD(_method_255), SimNode_ExtFuncCall >(lib,"GetObj","das_call_member< UObject * (FObjectInitializer::*)() const , &FObjectInitializer::GetObj >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_256 = das::das_call_member< UClass * (FObjectInitializer::*)() const,&FObjectInitializer::GetClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1089:26
	makeExtern<DAS_CALL_METHOD(_method_256), SimNode_ExtFuncCall >(lib,"GetClass","das_call_member< UClass * (FObjectInitializer::*)() const , &FObjectInitializer::GetClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_257 = das::das_call_member< UObject * (FObjectInitializer::*)(UObject *,FName,const UClass *,bool) const,&FObjectInitializer::CreateEditorOnlyDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1155:27
	makeExtern<DAS_CALL_METHOD(_method_257), SimNode_ExtFuncCall >(lib,"CreateEditorOnlyDefaultSubobject","das_call_member< UObject * (FObjectInitializer::*)(UObject *,FName,const UClass *,bool) const , &FObjectInitializer::CreateEditorOnlyDefaultSubobject >::invoke")
		->args({"self","Outer","SubobjectName","ReturnType","bTransient"})
		->arg_init(4,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_258 = das::das_call_member< UObject * (FObjectInitializer::*)(UObject *,FName,const UClass *,const UClass *,bool,bool) const,&FObjectInitializer::CreateDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1166:27
	makeExtern<DAS_CALL_METHOD(_method_258), SimNode_ExtFuncCall >(lib,"CreateDefaultSubobject","das_call_member< UObject * (FObjectInitializer::*)(UObject *,FName,const UClass *,const UClass *,bool,bool) const , &FObjectInitializer::CreateDefaultSubobject >::invoke")
		->args({"self","Outer","SubobjectFName","ReturnType","ClassToCreateByDefault","bIsRequired","bIsTransient"})
		->arg_init(5,make_smart<ExprConstBool>(true))
		->arg_init(6,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_259 = das::das_call_member< const FObjectInitializer & (FObjectInitializer::*)(FName,const UClass *) const,&FObjectInitializer::SetDefaultSubobjectClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1173:28
	makeExtern<DAS_CALL_METHOD(_method_259), SimNode_ExtFuncCallRef >(lib,"SetDefaultSubobjectClass","das_call_member< const FObjectInitializer & (FObjectInitializer::*)(FName,const UClass *) const , &FObjectInitializer::SetDefaultSubobjectClass >::invoke")
		->args({"self","SubobjectName","Class"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_260 = das::das_call_member< const FObjectInitializer & (FObjectInitializer::*)(FName) const,&FObjectInitializer::DoNotCreateDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1194:28
	makeExtern<DAS_CALL_METHOD(_method_260), SimNode_ExtFuncCallRef >(lib,"DoNotCreateDefaultSubobject","das_call_member< const FObjectInitializer & (FObjectInitializer::*)(FName) const , &FObjectInitializer::DoNotCreateDefaultSubobject >::invoke")
		->args({"self","SubobjectName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1270:30
	makeExtern< void (*)(const FObjectInitializer *,UObject *,const wchar_t *) , _dasUnreal_static_83_AssertIfInConstructor , SimNode_ExtFuncCall >(lib,"AssertIfInConstructor","_dasUnreal_static_83_AssertIfInConstructor")
		->args({"self","Outer","ErrorMessage"})
		->arg_type(0,makeType<TExplicit<const FObjectInitializer*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_262 = das::das_call_member< void (FObjectInitializer::*)(),&FObjectInitializer::FinalizeSubobjectClassInitialization >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1272:19
	makeExtern<DAS_CALL_METHOD(_method_262), SimNode_ExtFuncCall >(lib,"FinalizeSubobjectClassInitialization","das_call_member< void (FObjectInitializer::*)() , &FObjectInitializer::FinalizeSubobjectClassInitialization >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1278:45
	makeExtern< FObjectInitializer & (*)(const FObjectInitializer *) , _dasUnreal_static_84_Get , SimNode_ExtFuncCallRef >(lib,"Get","_dasUnreal_static_84_Get")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FObjectInitializer*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStaticConstructObjectParameters,const UClass *>(*this,lib,"FStaticConstructObjectParameters","FStaticConstructObjectParameters")
		->args({"InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1545:21
	makeExtern< void (*)(const FScriptIntegrationObjectHelper *,const FObjectInitializer &,UObject *,UClass *,UObject *) , _dasUnreal_static_85_InitProperties , SimNode_ExtFuncCall >(lib,"InitProperties","_dasUnreal_static_85_InitProperties")
		->args({"self","ObjectInitializer","Obj","DefaultsClass","DefaultData"})
		->arg_type(0,makeType<TExplicit<const FScriptIntegrationObjectHelper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1555:21
	makeExtern< bool (*)(const FScriptIntegrationObjectHelper *,const FObjectInitializer &) , _dasUnreal_static_86_InitSubobjectProperties , SimNode_ExtFuncCall >(lib,"InitSubobjectProperties","_dasUnreal_static_86_InitSubobjectProperties")
		->args({"self","ObjectInitializer"})
		->arg_type(0,makeType<TExplicit<const FScriptIntegrationObjectHelper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1567:21
	makeExtern< void (*)(const FScriptIntegrationObjectHelper *,const FObjectInitializer &,UClass *,bool,bool) , _dasUnreal_static_87_InstanceSubobjects , SimNode_ExtFuncCall >(lib,"InstanceSubobjects","_dasUnreal_static_87_InstanceSubobjects")
		->args({"self","ObjectInitializer","Class","bNeedInstancing","bNeedSubobjectInstancing"})
		->arg_type(0,makeType<TExplicit<const FScriptIntegrationObjectHelper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1578:21
	makeExtern< void (*)(const FScriptIntegrationObjectHelper *,FObjectInitializer &) , _dasUnreal_static_88_PostConstructInitObject , SimNode_ExtFuncCall >(lib,"PostConstructInitObject","_dasUnreal_static_88_PostConstructInitObject")
		->args({"self","ObjectInitializer"})
		->arg_type(0,makeType<TExplicit<const FScriptIntegrationObjectHelper*> >(lib))
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
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1872:28
	makeExtern< UFunction * (*)(FName) , FindDelegateSignature , SimNode_ExtFuncCall >(lib,"FindDelegateSignature","FindDelegateSignature")
		->args({"DelegateSignatureName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2207:31
	makeExtern< void (*)(FReferenceCollector &,UObject **) , _dasUnreal_virtual_89_AddStableReference , SimNode_ExtFuncCall >(lib,"AddStableReference","_dasUnreal_virtual_89_AddStableReference")
		->args({"self","Object"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_269 = das::das_call_member< void (FReferenceCollector::*)(const UStruct *,void *,const UObject *),&FReferenceCollector::AddPropertyReferences >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2529:23
	makeExtern<DAS_CALL_METHOD(_method_269), SimNode_ExtFuncCall >(lib,"AddPropertyReferences","das_call_member< void (FReferenceCollector::*)(const UStruct *,void *,const UObject *) , &FReferenceCollector::AddPropertyReferences >::invoke")
		->args({"self","Struct","Instance","ReferencingObject"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_270 = das::das_call_member< void (FReferenceCollector::*)(const UScriptStruct *,void *,const UObject *),&FReferenceCollector::AddPropertyReferencesWithStructARO >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2532:23
	makeExtern<DAS_CALL_METHOD(_method_270), SimNode_ExtFuncCall >(lib,"AddPropertyReferencesWithStructARO","das_call_member< void (FReferenceCollector::*)(const UScriptStruct *,void *,const UObject *) , &FReferenceCollector::AddPropertyReferencesWithStructARO >::invoke")
		->args({"self","Struct","Instance","ReferencingObject"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_271 = das::das_call_member< void (FReferenceCollector::*)(const UClass *,void *,const UObject *),&FReferenceCollector::AddPropertyReferencesWithStructARO >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2535:23
	makeExtern<DAS_CALL_METHOD(_method_271), SimNode_ExtFuncCall >(lib,"AddPropertyReferencesWithStructARO","das_call_member< void (FReferenceCollector::*)(const UClass *,void *,const UObject *) , &FReferenceCollector::AddPropertyReferencesWithStructARO >::invoke")
		->args({"self","Class","Instance","ReferencingObject"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_272 = das::das_call_member< void (FReferenceCollector::*)(const UStruct *,void *,const UObject *),&FReferenceCollector::AddPropertyReferencesLimitedToObjectProperties >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2538:23
	makeExtern<DAS_CALL_METHOD(_method_272), SimNode_ExtFuncCall >(lib,"AddPropertyReferencesLimitedToObjectProperties","das_call_member< void (FReferenceCollector::*)(const UStruct *,void *,const UObject *) , &FReferenceCollector::AddPropertyReferencesLimitedToObjectProperties >::invoke")
		->args({"self","Struct","Instance","ReferencingObject"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2546:15
	makeExtern< bool (*)(const FReferenceCollector &) , _dasUnreal_virtual_90_NeedsPropertyReferencer , SimNode_ExtFuncCall >(lib,"NeedsPropertyReferencer","_dasUnreal_virtual_90_NeedsPropertyReferencer")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2559:15
	makeExtern< void (*)(FReferenceCollector &,bool) , _dasUnreal_virtual_91_AllowEliminatingReferences , SimNode_ExtFuncCall >(lib,"AllowEliminatingReferences","_dasUnreal_virtual_91_AllowEliminatingReferences")
		->args({"self","bAllow"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

