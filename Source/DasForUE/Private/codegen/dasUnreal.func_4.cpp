// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_4() {
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
	using _method_223 = das::das_call_member< UObject * (FObjectInitializer::*)() const,&FObjectInitializer::GetArchetype >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1073:23
	makeExtern<DAS_CALL_METHOD(_method_223), SimNode_ExtFuncCall >(lib,"GetArchetype","das_call_member< UObject * (FObjectInitializer::*)() const , &FObjectInitializer::GetArchetype >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_224 = das::das_call_member< UObject * (FObjectInitializer::*)() const,&FObjectInitializer::GetObj >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1081:23
	makeExtern<DAS_CALL_METHOD(_method_224), SimNode_ExtFuncCall >(lib,"GetObj","das_call_member< UObject * (FObjectInitializer::*)() const , &FObjectInitializer::GetObj >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_225 = das::das_call_member< UClass * (FObjectInitializer::*)() const,&FObjectInitializer::GetClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1089:26
	makeExtern<DAS_CALL_METHOD(_method_225), SimNode_ExtFuncCall >(lib,"GetClass","das_call_member< UClass * (FObjectInitializer::*)() const , &FObjectInitializer::GetClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_226 = das::das_call_member< UObject * (FObjectInitializer::*)(UObject *,FName,const UClass *,bool) const,&FObjectInitializer::CreateEditorOnlyDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1155:27
	makeExtern<DAS_CALL_METHOD(_method_226), SimNode_ExtFuncCall >(lib,"CreateEditorOnlyDefaultSubobject","das_call_member< UObject * (FObjectInitializer::*)(UObject *,FName,const UClass *,bool) const , &FObjectInitializer::CreateEditorOnlyDefaultSubobject >::invoke")
		->args({"self","Outer","SubobjectName","ReturnType","bTransient"})
		->arg_init(4,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_227 = das::das_call_member< UObject * (FObjectInitializer::*)(UObject *,FName,const UClass *,const UClass *,bool,bool) const,&FObjectInitializer::CreateDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1166:27
	makeExtern<DAS_CALL_METHOD(_method_227), SimNode_ExtFuncCall >(lib,"CreateDefaultSubobject","das_call_member< UObject * (FObjectInitializer::*)(UObject *,FName,const UClass *,const UClass *,bool,bool) const , &FObjectInitializer::CreateDefaultSubobject >::invoke")
		->args({"self","Outer","SubobjectFName","ReturnType","ClassToCreateByDefault","bIsRequired","bIsTransient"})
		->arg_init(5,make_smart<ExprConstBool>(true))
		->arg_init(6,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_228 = das::das_call_member< const FObjectInitializer & (FObjectInitializer::*)(FName,const UClass *) const,&FObjectInitializer::SetDefaultSubobjectClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1173:28
	makeExtern<DAS_CALL_METHOD(_method_228), SimNode_ExtFuncCallRef >(lib,"SetDefaultSubobjectClass","das_call_member< const FObjectInitializer & (FObjectInitializer::*)(FName,const UClass *) const , &FObjectInitializer::SetDefaultSubobjectClass >::invoke")
		->args({"self","SubobjectName","Class"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_229 = das::das_call_member< const FObjectInitializer & (FObjectInitializer::*)(FName) const,&FObjectInitializer::DoNotCreateDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1194:28
	makeExtern<DAS_CALL_METHOD(_method_229), SimNode_ExtFuncCallRef >(lib,"DoNotCreateDefaultSubobject","das_call_member< const FObjectInitializer & (FObjectInitializer::*)(FName) const , &FObjectInitializer::DoNotCreateDefaultSubobject >::invoke")
		->args({"self","SubobjectName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1270:30
	makeExtern< void (*)(const FObjectInitializer *,UObject *,const wchar_t *) , _dasUnreal_static_70_AssertIfInConstructor , SimNode_ExtFuncCall >(lib,"AssertIfInConstructor","_dasUnreal_static_70_AssertIfInConstructor")
		->args({"self","Outer","ErrorMessage"})
		->arg_type(0,makeType<TExplicit<const FObjectInitializer*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_231 = das::das_call_member< void (FObjectInitializer::*)(),&FObjectInitializer::FinalizeSubobjectClassInitialization >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1272:19
	makeExtern<DAS_CALL_METHOD(_method_231), SimNode_ExtFuncCall >(lib,"FinalizeSubobjectClassInitialization","das_call_member< void (FObjectInitializer::*)() , &FObjectInitializer::FinalizeSubobjectClassInitialization >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1278:45
	makeExtern< FObjectInitializer & (*)(const FObjectInitializer *) , _dasUnreal_static_71_Get , SimNode_ExtFuncCallRef >(lib,"Get","_dasUnreal_static_71_Get")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FObjectInitializer*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStaticConstructObjectParameters,const UClass *>(*this,lib,"FStaticConstructObjectParameters","FStaticConstructObjectParameters")
		->args({"InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1545:21
	makeExtern< void (*)(const FScriptIntegrationObjectHelper *,const FObjectInitializer &,UObject *,UClass *,UObject *) , _dasUnreal_static_72_InitProperties , SimNode_ExtFuncCall >(lib,"InitProperties","_dasUnreal_static_72_InitProperties")
		->args({"self","ObjectInitializer","Obj","DefaultsClass","DefaultData"})
		->arg_type(0,makeType<TExplicit<const FScriptIntegrationObjectHelper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1555:21
	makeExtern< bool (*)(const FScriptIntegrationObjectHelper *,const FObjectInitializer &) , _dasUnreal_static_73_InitSubobjectProperties , SimNode_ExtFuncCall >(lib,"InitSubobjectProperties","_dasUnreal_static_73_InitSubobjectProperties")
		->args({"self","ObjectInitializer"})
		->arg_type(0,makeType<TExplicit<const FScriptIntegrationObjectHelper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1567:21
	makeExtern< void (*)(const FScriptIntegrationObjectHelper *,const FObjectInitializer &,UClass *,bool,bool) , _dasUnreal_static_74_InstanceSubobjects , SimNode_ExtFuncCall >(lib,"InstanceSubobjects","_dasUnreal_static_74_InstanceSubobjects")
		->args({"self","ObjectInitializer","Class","bNeedInstancing","bNeedSubobjectInstancing"})
		->arg_type(0,makeType<TExplicit<const FScriptIntegrationObjectHelper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1578:21
	makeExtern< void (*)(const FScriptIntegrationObjectHelper *,FObjectInitializer &) , _dasUnreal_static_75_PostConstructInitObject , SimNode_ExtFuncCall >(lib,"PostConstructInitObject","_dasUnreal_static_75_PostConstructInitObject")
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
	makeExtern< void (*)(FReferenceCollector &,UObject **) , _dasUnreal_virtual_76_AddStableReference , SimNode_ExtFuncCall >(lib,"AddStableReference","_dasUnreal_virtual_76_AddStableReference")
		->args({"self","Object"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_238 = das::das_call_member< void (FReferenceCollector::*)(const UStruct *,void *,const UObject *),&FReferenceCollector::AddPropertyReferences >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2529:23
	makeExtern<DAS_CALL_METHOD(_method_238), SimNode_ExtFuncCall >(lib,"AddPropertyReferences","das_call_member< void (FReferenceCollector::*)(const UStruct *,void *,const UObject *) , &FReferenceCollector::AddPropertyReferences >::invoke")
		->args({"self","Struct","Instance","ReferencingObject"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_239 = das::das_call_member< void (FReferenceCollector::*)(const UScriptStruct *,void *,const UObject *),&FReferenceCollector::AddPropertyReferencesWithStructARO >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2532:23
	makeExtern<DAS_CALL_METHOD(_method_239), SimNode_ExtFuncCall >(lib,"AddPropertyReferencesWithStructARO","das_call_member< void (FReferenceCollector::*)(const UScriptStruct *,void *,const UObject *) , &FReferenceCollector::AddPropertyReferencesWithStructARO >::invoke")
		->args({"self","Struct","Instance","ReferencingObject"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_240 = das::das_call_member< void (FReferenceCollector::*)(const UClass *,void *,const UObject *),&FReferenceCollector::AddPropertyReferencesWithStructARO >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2535:23
	makeExtern<DAS_CALL_METHOD(_method_240), SimNode_ExtFuncCall >(lib,"AddPropertyReferencesWithStructARO","das_call_member< void (FReferenceCollector::*)(const UClass *,void *,const UObject *) , &FReferenceCollector::AddPropertyReferencesWithStructARO >::invoke")
		->args({"self","Class","Instance","ReferencingObject"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_241 = das::das_call_member< void (FReferenceCollector::*)(const UStruct *,void *,const UObject *),&FReferenceCollector::AddPropertyReferencesLimitedToObjectProperties >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2538:23
	makeExtern<DAS_CALL_METHOD(_method_241), SimNode_ExtFuncCall >(lib,"AddPropertyReferencesLimitedToObjectProperties","das_call_member< void (FReferenceCollector::*)(const UStruct *,void *,const UObject *) , &FReferenceCollector::AddPropertyReferencesLimitedToObjectProperties >::invoke")
		->args({"self","Struct","Instance","ReferencingObject"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2546:15
	makeExtern< bool (*)(const FReferenceCollector &) , _dasUnreal_virtual_77_NeedsPropertyReferencer , SimNode_ExtFuncCall >(lib,"NeedsPropertyReferencer","_dasUnreal_virtual_77_NeedsPropertyReferencer")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2559:15
	makeExtern< void (*)(FReferenceCollector &,bool) , _dasUnreal_virtual_78_AllowEliminatingReferences , SimNode_ExtFuncCall >(lib,"AllowEliminatingReferences","_dasUnreal_virtual_78_AllowEliminatingReferences")
		->args({"self","bAllow"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2565:15
	makeExtern< void (*)(FReferenceCollector &,FProperty *) , _dasUnreal_virtual_79_SetSerializedProperty , SimNode_ExtFuncCall >(lib,"SetSerializedProperty","_dasUnreal_virtual_79_SetSerializedProperty")
		->args({"self","Inproperty"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2569:27
	makeExtern< FProperty * (*)(const FReferenceCollector &) , _dasUnreal_virtual_80_GetSerializedProperty , SimNode_ExtFuncCall >(lib,"GetSerializedProperty","_dasUnreal_virtual_80_GetSerializedProperty")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2574:15
	makeExtern< bool (*)(FReferenceCollector &,UObject **) , _dasUnreal_virtual_81_MarkWeakObjectReferenceForClearing , SimNode_ExtFuncCall >(lib,"MarkWeakObjectReferenceForClearing","_dasUnreal_virtual_81_MarkWeakObjectReferenceForClearing")
		->args({"self","WeakReference"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2578:15
	makeExtern< void (*)(FReferenceCollector &,bool) , _dasUnreal_virtual_82_SetIsProcessingNativeReferences , SimNode_ExtFuncCall >(lib,"SetIsProcessingNativeReferences","_dasUnreal_virtual_82_SetIsProcessingNativeReferences")
		->args({"self","bIsNative"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2582:15
	makeExtern< bool (*)(const FReferenceCollector &) , _dasUnreal_virtual_83_IsProcessingNativeReferences , SimNode_ExtFuncCall >(lib,"IsProcessingNativeReferences","_dasUnreal_virtual_83_IsProcessingNativeReferences")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2585:15
	makeExtern< bool (*)(const FReferenceCollector &) , _dasUnreal_virtual_84_NeedsInitialReferences , SimNode_ExtFuncCall >(lib,"NeedsInitialReferences","_dasUnreal_virtual_84_NeedsInitialReferences")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2794:31
	makeExtern< void (*)(FReferenceFinder &,UObject *,UObject *,FProperty *) , _dasUnreal_virtual_85_FindReferences , SimNode_ExtFuncCall >(lib,"FindReferences","_dasUnreal_virtual_85_FindReferences")
		->args({"self","Object","ReferencingObject","ReferencingProperty"})
		->arg_init(2,make_smart<ExprConstPtr>())
		->arg_init(3,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2798:15
	makeExtern< bool (*)(const FReferenceFinder &) , _dasUnreal_virtual_86_IsIgnoringArchetypeRef , SimNode_ExtFuncCall >(lib,"IsIgnoringArchetypeRef","_dasUnreal_virtual_86_IsIgnoringArchetypeRef")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2799:15
	makeExtern< bool (*)(const FReferenceFinder &) , _dasUnreal_virtual_87_IsIgnoringTransient , SimNode_ExtFuncCall >(lib,"IsIgnoringTransient","_dasUnreal_virtual_87_IsIgnoringTransient")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2800:15
	makeExtern< void (*)(FReferenceFinder &,FProperty *) , _dasUnreal_virtual_88_SetSerializedProperty , SimNode_ExtFuncCall >(lib,"SetSerializedProperty","_dasUnreal_virtual_88_SetSerializedProperty")
		->args({"self","Inproperty"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2804:27
	makeExtern< FProperty * (*)(const FReferenceFinder &) , _dasUnreal_virtual_89_GetSerializedProperty , SimNode_ExtFuncCall >(lib,"GetSerializedProperty","_dasUnreal_virtual_89_GetSerializedProperty")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2879:14
	makeExtern< void (*)(const FCoreUObjectDelegates *,UObject *) , _dasUnreal_static_90_BroadcastOnObjectModified , SimNode_ExtFuncCall >(lib,"BroadcastOnObjectModified","_dasUnreal_static_90_BroadcastOnObjectModified")
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
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3151:33
	makeExtern< FString (*)(const FAssetMsg *,const wchar_t *) , _dasUnreal_static_91_FormatPathForAssetLog , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FormatPathForAssetLog","_dasUnreal_static_91_FormatPathForAssetLog")
		->args({"self","Path"})
		->arg_type(0,makeType<TExplicit<const FAssetMsg*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3157:33
	makeExtern< FString (*)(const FAssetMsg *,const UObject *) , _dasUnreal_static_92_FormatPathForAssetLog , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FormatPathForAssetLog","_dasUnreal_static_92_FormatPathForAssetLog")
		->args({"self","Object"})
		->arg_type(0,makeType<TExplicit<const FAssetMsg*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3159:33
	makeExtern< FString (*)(const FAssetMsg *,const wchar_t *,const FString &) , _dasUnreal_static_93_GetAssetLogString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAssetLogString","_dasUnreal_static_93_GetAssetLogString")
		->args({"self","Path","Message"})
		->arg_type(0,makeType<TExplicit<const FAssetMsg*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3161:33
	makeExtern< FString (*)(const FAssetMsg *,const UObject *,const FString &) , _dasUnreal_static_94_GetAssetLogString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAssetLogString","_dasUnreal_static_94_GetAssetLogString")
		->args({"self","Object","Message"})
		->arg_type(0,makeType<TExplicit<const FAssetMsg*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3210:22
	makeExtern< bool (*)(const UObject *,bool) , IsEditorOnlyObject , SimNode_ExtFuncCall >(lib,"IsEditorOnlyObject","IsEditorOnlyObject")
		->args({"InObject","bCheckRecursive"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3212:22
	makeExtern< bool (*)(const UObject *,bool,bool) , IsEditorOnlyObject , SimNode_ExtFuncCall >(lib,"IsEditorOnlyObject","IsEditorOnlyObject")
		->args({"InObject","bCheckRecursive","bCheckMarks"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3740:23
	makeExtern< void (*)(UEnum *&,const UECodeGen_Private::FEnumParams &) , UECodeGen_Private::ConstructUEnum , SimNode_ExtFuncCall >(lib,"ConstructUEnum","UECodeGen_Private::ConstructUEnum")
		->args({"OutEnum","Params"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3741:23
	makeExtern< void (*)(UScriptStruct *&,const UECodeGen_Private::FStructParams &) , UECodeGen_Private::ConstructUScriptStruct , SimNode_ExtFuncCall >(lib,"ConstructUScriptStruct","UECodeGen_Private::ConstructUScriptStruct")
		->args({"OutStruct","Params"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3742:23
	makeExtern< void (*)(UPackage *&,const UECodeGen_Private::FPackageParams &) , UECodeGen_Private::ConstructUPackage , SimNode_ExtFuncCall >(lib,"ConstructUPackage","UECodeGen_Private::ConstructUPackage")
		->args({"OutPackage","Params"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3743:23
	makeExtern< void (*)(UClass *&,const UECodeGen_Private::FClassParams &) , UECodeGen_Private::ConstructUClass , SimNode_ExtFuncCall >(lib,"ConstructUClass","UECodeGen_Private::ConstructUClass")
		->args({"OutClass","Params"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3787:39
	makeExtern< EDataValidationResult (*)(EDataValidationResult,EDataValidationResult) , CombineDataValidationResults , SimNode_ExtFuncCall >(lib,"CombineDataValidationResults","CombineDataValidationResults")
		->args({"Result1","Result2"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<UObjectBase,UClass *,EObjectFlags,EInternalObjectFlags,UObject *,FName,int,int>(*this,lib,"UObjectBase","UObjectBase")
		->args({"InClass","InFlags","InInternalFlags","InOuter","InName","InInternalIndex","InSerialNumber"})
		->arg_init(5,make_smart<ExprConstInt>(-1))
		->arg_init(6,make_smart<ExprConstInt>(0));
	using _method_260 = das::das_call_member< bool (UObjectBase::*)() const,&UObjectBase::IsValidLowLevel >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:122:23
	makeExtern<DAS_CALL_METHOD(_method_260), SimNode_ExtFuncCall >(lib,"IsValidLowLevel","das_call_member< bool (UObjectBase::*)() const , &UObjectBase::IsValidLowLevel >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_261 = das::das_call_member< bool (UObjectBase::*)(bool) const,&UObjectBase::IsValidLowLevelFast >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:131:23
	makeExtern<DAS_CALL_METHOD(_method_261), SimNode_ExtFuncCall >(lib,"IsValidLowLevelFast","das_call_member< bool (UObjectBase::*)(bool) const , &UObjectBase::IsValidLowLevelFast >::invoke")
		->args({"self","bRecursive"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_262 = das::das_call_member< unsigned int (UObjectBase::*)() const,&UObjectBase::GetUniqueID >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:137:21
	makeExtern<DAS_CALL_METHOD(_method_262), SimNode_ExtFuncCall >(lib,"GetUniqueID","das_call_member< unsigned int (UObjectBase::*)() const , &UObjectBase::GetUniqueID >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_263 = das::das_call_member< UClass * (UObjectBase::*)() const,&UObjectBase::GetClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:143:22
	makeExtern<DAS_CALL_METHOD(_method_263), SimNode_ExtFuncCall >(lib,"GetClass","das_call_member< UClass * (UObjectBase::*)() const , &UObjectBase::GetClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_264 = das::das_call_member< UObject * (UObjectBase::*)() const,&UObjectBase::GetOuter >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:149:23
	makeExtern<DAS_CALL_METHOD(_method_264), SimNode_ExtFuncCall >(lib,"GetOuter","das_call_member< UObject * (UObjectBase::*)() const , &UObjectBase::GetOuter >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_265 = das::das_call_member< FName (UObjectBase::*)() const,&UObjectBase::GetFName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:155:20
	makeExtern<DAS_CALL_METHOD(_method_265), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFName","das_call_member< FName (UObjectBase::*)() const , &UObjectBase::GetFName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:161:32
	makeExtern< FName (*)(const UObjectBase &) , _dasUnreal_virtual_95_GetFNameForStatID , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFNameForStatID","_dasUnreal_virtual_95_GetFNameForStatID")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:164:33
	makeExtern< FString (*)(const UObjectBase *,const wchar_t *) , _dasUnreal_static_96_RemoveClassPrefix , SimNode_ExtFuncCallAndCopyOrMove >(lib,"RemoveClassPrefix","_dasUnreal_static_96_RemoveClassPrefix")
		->args({"self","ClassName"})
		->arg_type(0,makeType<TExplicit<const UObjectBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_268 = das::das_call_member< UPackage * (UObjectBase::*)() const,&UObjectBase::GetExternalPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:167:28
	makeExtern<DAS_CALL_METHOD(_method_268), SimNode_ExtFuncCall >(lib,"GetExternalPackage","das_call_member< UPackage * (UObjectBase::*)() const , &UObjectBase::GetExternalPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_269 = das::das_call_member< void (UObjectBase::*)(UPackage *),&UObjectBase::SetExternalPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:170:23
	makeExtern<DAS_CALL_METHOD(_method_269), SimNode_ExtFuncCall >(lib,"SetExternalPackage","das_call_member< void (UObjectBase::*)(UPackage *) , &UObjectBase::SetExternalPackage >::invoke")
		->args({"self","InPackage"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_270 = das::das_call_member< UPackage * (UObjectBase::*)() const,&UObjectBase::GetExternalPackageInternal >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:173:28
	makeExtern<DAS_CALL_METHOD(_method_270), SimNode_ExtFuncCall >(lib,"GetExternalPackageInternal","das_call_member< UPackage * (UObjectBase::*)() const , &UObjectBase::GetExternalPackageInternal >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

