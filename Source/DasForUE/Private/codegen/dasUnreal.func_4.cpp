// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_4() {
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
	using _method_234 = das::das_call_member< UObject * (FObjectInitializer::*)() const,&FObjectInitializer::GetArchetype >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1073:23
	makeExtern<DAS_CALL_METHOD(_method_234), SimNode_ExtFuncCall >(lib,"GetArchetype","das_call_member< UObject * (FObjectInitializer::*)() const , &FObjectInitializer::GetArchetype >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_235 = das::das_call_member< UObject * (FObjectInitializer::*)() const,&FObjectInitializer::GetObj >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1081:23
	makeExtern<DAS_CALL_METHOD(_method_235), SimNode_ExtFuncCall >(lib,"GetObj","das_call_member< UObject * (FObjectInitializer::*)() const , &FObjectInitializer::GetObj >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_236 = das::das_call_member< UClass * (FObjectInitializer::*)() const,&FObjectInitializer::GetClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1089:26
	makeExtern<DAS_CALL_METHOD(_method_236), SimNode_ExtFuncCall >(lib,"GetClass","das_call_member< UClass * (FObjectInitializer::*)() const , &FObjectInitializer::GetClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_237 = das::das_call_member< UObject * (FObjectInitializer::*)(UObject *,FName,const UClass *,bool) const,&FObjectInitializer::CreateEditorOnlyDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1155:27
	makeExtern<DAS_CALL_METHOD(_method_237), SimNode_ExtFuncCall >(lib,"CreateEditorOnlyDefaultSubobject","das_call_member< UObject * (FObjectInitializer::*)(UObject *,FName,const UClass *,bool) const , &FObjectInitializer::CreateEditorOnlyDefaultSubobject >::invoke")
		->args({"self","Outer","SubobjectName","ReturnType","bTransient"})
		->arg_init(4,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_238 = das::das_call_member< UObject * (FObjectInitializer::*)(UObject *,FName,const UClass *,const UClass *,bool,bool) const,&FObjectInitializer::CreateDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1166:27
	makeExtern<DAS_CALL_METHOD(_method_238), SimNode_ExtFuncCall >(lib,"CreateDefaultSubobject","das_call_member< UObject * (FObjectInitializer::*)(UObject *,FName,const UClass *,const UClass *,bool,bool) const , &FObjectInitializer::CreateDefaultSubobject >::invoke")
		->args({"self","Outer","SubobjectFName","ReturnType","ClassToCreateByDefault","bIsRequired","bIsTransient"})
		->arg_init(5,make_smart<ExprConstBool>(true))
		->arg_init(6,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_239 = das::das_call_member< const FObjectInitializer & (FObjectInitializer::*)(FName,const UClass *) const,&FObjectInitializer::SetDefaultSubobjectClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1173:28
	makeExtern<DAS_CALL_METHOD(_method_239), SimNode_ExtFuncCallRef >(lib,"SetDefaultSubobjectClass","das_call_member< const FObjectInitializer & (FObjectInitializer::*)(FName,const UClass *) const , &FObjectInitializer::SetDefaultSubobjectClass >::invoke")
		->args({"self","SubobjectName","Class"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_240 = das::das_call_member< const FObjectInitializer & (FObjectInitializer::*)(FName) const,&FObjectInitializer::DoNotCreateDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1194:28
	makeExtern<DAS_CALL_METHOD(_method_240), SimNode_ExtFuncCallRef >(lib,"DoNotCreateDefaultSubobject","das_call_member< const FObjectInitializer & (FObjectInitializer::*)(FName) const , &FObjectInitializer::DoNotCreateDefaultSubobject >::invoke")
		->args({"self","SubobjectName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1270:30
	makeExtern< void (*)(const FObjectInitializer *,UObject *,const wchar_t *) , _dasUnreal_static_74_AssertIfInConstructor , SimNode_ExtFuncCall >(lib,"AssertIfInConstructor","_dasUnreal_static_74_AssertIfInConstructor")
		->args({"self","Outer","ErrorMessage"})
		->arg_type(0,makeType<TExplicit<const FObjectInitializer*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_242 = das::das_call_member< void (FObjectInitializer::*)(),&FObjectInitializer::FinalizeSubobjectClassInitialization >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1272:19
	makeExtern<DAS_CALL_METHOD(_method_242), SimNode_ExtFuncCall >(lib,"FinalizeSubobjectClassInitialization","das_call_member< void (FObjectInitializer::*)() , &FObjectInitializer::FinalizeSubobjectClassInitialization >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1278:45
	makeExtern< FObjectInitializer & (*)(const FObjectInitializer *) , _dasUnreal_static_75_Get , SimNode_ExtFuncCallRef >(lib,"Get","_dasUnreal_static_75_Get")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FObjectInitializer*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStaticConstructObjectParameters,const UClass *>(*this,lib,"FStaticConstructObjectParameters","FStaticConstructObjectParameters")
		->args({"InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1545:21
	makeExtern< void (*)(const FScriptIntegrationObjectHelper *,const FObjectInitializer &,UObject *,UClass *,UObject *) , _dasUnreal_static_76_InitProperties , SimNode_ExtFuncCall >(lib,"InitProperties","_dasUnreal_static_76_InitProperties")
		->args({"self","ObjectInitializer","Obj","DefaultsClass","DefaultData"})
		->arg_type(0,makeType<TExplicit<const FScriptIntegrationObjectHelper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1555:21
	makeExtern< bool (*)(const FScriptIntegrationObjectHelper *,const FObjectInitializer &) , _dasUnreal_static_77_InitSubobjectProperties , SimNode_ExtFuncCall >(lib,"InitSubobjectProperties","_dasUnreal_static_77_InitSubobjectProperties")
		->args({"self","ObjectInitializer"})
		->arg_type(0,makeType<TExplicit<const FScriptIntegrationObjectHelper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1567:21
	makeExtern< void (*)(const FScriptIntegrationObjectHelper *,const FObjectInitializer &,UClass *,bool,bool) , _dasUnreal_static_78_InstanceSubobjects , SimNode_ExtFuncCall >(lib,"InstanceSubobjects","_dasUnreal_static_78_InstanceSubobjects")
		->args({"self","ObjectInitializer","Class","bNeedInstancing","bNeedSubobjectInstancing"})
		->arg_type(0,makeType<TExplicit<const FScriptIntegrationObjectHelper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1578:21
	makeExtern< void (*)(const FScriptIntegrationObjectHelper *,FObjectInitializer &) , _dasUnreal_static_79_PostConstructInitObject , SimNode_ExtFuncCall >(lib,"PostConstructInitObject","_dasUnreal_static_79_PostConstructInitObject")
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
	makeExtern< void (*)(FReferenceCollector &,UObject **) , _dasUnreal_virtual_80_AddStableReference , SimNode_ExtFuncCall >(lib,"AddStableReference","_dasUnreal_virtual_80_AddStableReference")
		->args({"self","Object"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_249 = das::das_call_member< void (FReferenceCollector::*)(const UStruct *,void *,const UObject *),&FReferenceCollector::AddPropertyReferences >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2529:23
	makeExtern<DAS_CALL_METHOD(_method_249), SimNode_ExtFuncCall >(lib,"AddPropertyReferences","das_call_member< void (FReferenceCollector::*)(const UStruct *,void *,const UObject *) , &FReferenceCollector::AddPropertyReferences >::invoke")
		->args({"self","Struct","Instance","ReferencingObject"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_250 = das::das_call_member< void (FReferenceCollector::*)(const UScriptStruct *,void *,const UObject *),&FReferenceCollector::AddPropertyReferencesWithStructARO >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2532:23
	makeExtern<DAS_CALL_METHOD(_method_250), SimNode_ExtFuncCall >(lib,"AddPropertyReferencesWithStructARO","das_call_member< void (FReferenceCollector::*)(const UScriptStruct *,void *,const UObject *) , &FReferenceCollector::AddPropertyReferencesWithStructARO >::invoke")
		->args({"self","Struct","Instance","ReferencingObject"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_251 = das::das_call_member< void (FReferenceCollector::*)(const UClass *,void *,const UObject *),&FReferenceCollector::AddPropertyReferencesWithStructARO >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2535:23
	makeExtern<DAS_CALL_METHOD(_method_251), SimNode_ExtFuncCall >(lib,"AddPropertyReferencesWithStructARO","das_call_member< void (FReferenceCollector::*)(const UClass *,void *,const UObject *) , &FReferenceCollector::AddPropertyReferencesWithStructARO >::invoke")
		->args({"self","Class","Instance","ReferencingObject"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_252 = das::das_call_member< void (FReferenceCollector::*)(const UStruct *,void *,const UObject *),&FReferenceCollector::AddPropertyReferencesLimitedToObjectProperties >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2538:23
	makeExtern<DAS_CALL_METHOD(_method_252), SimNode_ExtFuncCall >(lib,"AddPropertyReferencesLimitedToObjectProperties","das_call_member< void (FReferenceCollector::*)(const UStruct *,void *,const UObject *) , &FReferenceCollector::AddPropertyReferencesLimitedToObjectProperties >::invoke")
		->args({"self","Struct","Instance","ReferencingObject"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2546:15
	makeExtern< bool (*)(const FReferenceCollector &) , _dasUnreal_virtual_81_NeedsPropertyReferencer , SimNode_ExtFuncCall >(lib,"NeedsPropertyReferencer","_dasUnreal_virtual_81_NeedsPropertyReferencer")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2559:15
	makeExtern< void (*)(FReferenceCollector &,bool) , _dasUnreal_virtual_82_AllowEliminatingReferences , SimNode_ExtFuncCall >(lib,"AllowEliminatingReferences","_dasUnreal_virtual_82_AllowEliminatingReferences")
		->args({"self","bAllow"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2565:15
	makeExtern< void (*)(FReferenceCollector &,FProperty *) , _dasUnreal_virtual_83_SetSerializedProperty , SimNode_ExtFuncCall >(lib,"SetSerializedProperty","_dasUnreal_virtual_83_SetSerializedProperty")
		->args({"self","Inproperty"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2569:27
	makeExtern< FProperty * (*)(const FReferenceCollector &) , _dasUnreal_virtual_84_GetSerializedProperty , SimNode_ExtFuncCall >(lib,"GetSerializedProperty","_dasUnreal_virtual_84_GetSerializedProperty")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2574:15
	makeExtern< bool (*)(FReferenceCollector &,UObject **) , _dasUnreal_virtual_85_MarkWeakObjectReferenceForClearing , SimNode_ExtFuncCall >(lib,"MarkWeakObjectReferenceForClearing","_dasUnreal_virtual_85_MarkWeakObjectReferenceForClearing")
		->args({"self","WeakReference"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2578:15
	makeExtern< void (*)(FReferenceCollector &,bool) , _dasUnreal_virtual_86_SetIsProcessingNativeReferences , SimNode_ExtFuncCall >(lib,"SetIsProcessingNativeReferences","_dasUnreal_virtual_86_SetIsProcessingNativeReferences")
		->args({"self","bIsNative"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2582:15
	makeExtern< bool (*)(const FReferenceCollector &) , _dasUnreal_virtual_87_IsProcessingNativeReferences , SimNode_ExtFuncCall >(lib,"IsProcessingNativeReferences","_dasUnreal_virtual_87_IsProcessingNativeReferences")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2585:15
	makeExtern< bool (*)(const FReferenceCollector &) , _dasUnreal_virtual_88_NeedsInitialReferences , SimNode_ExtFuncCall >(lib,"NeedsInitialReferences","_dasUnreal_virtual_88_NeedsInitialReferences")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2794:31
	makeExtern< void (*)(FReferenceFinder &,UObject *,UObject *,FProperty *) , _dasUnreal_virtual_89_FindReferences , SimNode_ExtFuncCall >(lib,"FindReferences","_dasUnreal_virtual_89_FindReferences")
		->args({"self","Object","ReferencingObject","ReferencingProperty"})
		->arg_init(2,make_smart<ExprConstPtr>())
		->arg_init(3,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2798:15
	makeExtern< bool (*)(const FReferenceFinder &) , _dasUnreal_virtual_90_IsIgnoringArchetypeRef , SimNode_ExtFuncCall >(lib,"IsIgnoringArchetypeRef","_dasUnreal_virtual_90_IsIgnoringArchetypeRef")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2799:15
	makeExtern< bool (*)(const FReferenceFinder &) , _dasUnreal_virtual_91_IsIgnoringTransient , SimNode_ExtFuncCall >(lib,"IsIgnoringTransient","_dasUnreal_virtual_91_IsIgnoringTransient")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2800:15
	makeExtern< void (*)(FReferenceFinder &,FProperty *) , _dasUnreal_virtual_92_SetSerializedProperty , SimNode_ExtFuncCall >(lib,"SetSerializedProperty","_dasUnreal_virtual_92_SetSerializedProperty")
		->args({"self","Inproperty"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2804:27
	makeExtern< FProperty * (*)(const FReferenceFinder &) , _dasUnreal_virtual_93_GetSerializedProperty , SimNode_ExtFuncCall >(lib,"GetSerializedProperty","_dasUnreal_virtual_93_GetSerializedProperty")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2879:14
	makeExtern< void (*)(const FCoreUObjectDelegates *,UObject *) , _dasUnreal_static_94_BroadcastOnObjectModified , SimNode_ExtFuncCall >(lib,"BroadcastOnObjectModified","_dasUnreal_static_94_BroadcastOnObjectModified")
		->args({"self","Object"})
		->arg_type(0,makeType<TExplicit<const FCoreUObjectDelegates*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3062:32
	makeExtern< UScriptStruct * (*)() , GetFallbackStruct , SimNode_ExtFuncCall >(lib,"GetFallbackStruct","GetFallbackStruct")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3074:26
	makeExtern< UObject * (*)(FName,EConstructDynamicType) , ConstructDynamicType , SimNode_ExtFuncCall >(lib,"ConstructDynamicType","ConstructDynamicType")
		->args({"TypePathName","ConstructionSpecifier"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3080:23
	makeExtern< FName (*)(FName) , GetDynamicTypeClassName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDynamicTypeClassName","GetDynamicTypeClassName")
		->args({"TypePathName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3085:27
	makeExtern< UPackage * (*)(const wchar_t *) , FindOrConstructDynamicTypePackage , SimNode_ExtFuncCall >(lib,"FindOrConstructDynamicTypePackage","FindOrConstructDynamicTypePackage")
		->args({"PackageName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3093:22
	makeExtern< bool (*)() , GetAllowNativeComponentClassOverrides , SimNode_ExtFuncCall >(lib,"GetAllowNativeComponentClassOverrides","GetAllowNativeComponentClassOverrides")
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<UE::FAssetLog,const wchar_t *>(*this,lib,"FAssetLog","UE::FAssetLog")
		->args({"InPath"});
	addCtorAndUsing<UE::FAssetLog,const UObject *>(*this,lib,"FAssetLog","UE::FAssetLog")
		->args({"InObject"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3135:22
	makeExtern< void (*)(const FName &,ELogVerbosity::Type,const UE::FAssetLog &,const FString &,const char *,int) , UE::Core::Private::RecordAssetLog , SimNode_ExtFuncCall >(lib,"RecordAssetLog","UE::Core::Private::RecordAssetLog")
		->args({"CategoryName","Verbosity","AssetLog","Message","File","Line"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

