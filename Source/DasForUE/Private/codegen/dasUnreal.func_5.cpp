// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_5() {
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
	using _method_281 = das::das_call_member< UObject * (FObjectInitializer::*)() const,&FObjectInitializer::GetArchetype >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1073:23
	makeExtern<DAS_CALL_METHOD(_method_281), SimNode_ExtFuncCall >(lib,"GetArchetype","das_call_member< UObject * (FObjectInitializer::*)() const , &FObjectInitializer::GetArchetype >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_282 = das::das_call_member< UObject * (FObjectInitializer::*)() const,&FObjectInitializer::GetObj >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1081:23
	makeExtern<DAS_CALL_METHOD(_method_282), SimNode_ExtFuncCall >(lib,"GetObj","das_call_member< UObject * (FObjectInitializer::*)() const , &FObjectInitializer::GetObj >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_283 = das::das_call_member< UClass * (FObjectInitializer::*)() const,&FObjectInitializer::GetClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1089:26
	makeExtern<DAS_CALL_METHOD(_method_283), SimNode_ExtFuncCall >(lib,"GetClass","das_call_member< UClass * (FObjectInitializer::*)() const , &FObjectInitializer::GetClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_284 = das::das_call_member< UObject * (FObjectInitializer::*)(UObject *,FName,const UClass *,bool) const,&FObjectInitializer::CreateEditorOnlyDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1155:27
	makeExtern<DAS_CALL_METHOD(_method_284), SimNode_ExtFuncCall >(lib,"CreateEditorOnlyDefaultSubobject","das_call_member< UObject * (FObjectInitializer::*)(UObject *,FName,const UClass *,bool) const , &FObjectInitializer::CreateEditorOnlyDefaultSubobject >::invoke")
		->args({"self","Outer","SubobjectName","ReturnType","bTransient"})
		->arg_init(4,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_285 = das::das_call_member< UObject * (FObjectInitializer::*)(UObject *,FName,const UClass *,const UClass *,bool,bool) const,&FObjectInitializer::CreateDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1166:27
	makeExtern<DAS_CALL_METHOD(_method_285), SimNode_ExtFuncCall >(lib,"CreateDefaultSubobject","das_call_member< UObject * (FObjectInitializer::*)(UObject *,FName,const UClass *,const UClass *,bool,bool) const , &FObjectInitializer::CreateDefaultSubobject >::invoke")
		->args({"self","Outer","SubobjectFName","ReturnType","ClassToCreateByDefault","bIsRequired","bIsTransient"})
		->arg_init(5,make_smart<ExprConstBool>(true))
		->arg_init(6,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_286 = das::das_call_member< const FObjectInitializer & (FObjectInitializer::*)(FName,const UClass *) const,&FObjectInitializer::SetDefaultSubobjectClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1173:28
	makeExtern<DAS_CALL_METHOD(_method_286), SimNode_ExtFuncCallRef >(lib,"SetDefaultSubobjectClass","das_call_member< const FObjectInitializer & (FObjectInitializer::*)(FName,const UClass *) const , &FObjectInitializer::SetDefaultSubobjectClass >::invoke")
		->args({"self","SubobjectName","Class"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_287 = das::das_call_member< const FObjectInitializer & (FObjectInitializer::*)(FName) const,&FObjectInitializer::DoNotCreateDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1194:28
	makeExtern<DAS_CALL_METHOD(_method_287), SimNode_ExtFuncCallRef >(lib,"DoNotCreateDefaultSubobject","das_call_member< const FObjectInitializer & (FObjectInitializer::*)(FName) const , &FObjectInitializer::DoNotCreateDefaultSubobject >::invoke")
		->args({"self","SubobjectName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1270:30
	makeExtern< void (*)(const FObjectInitializer *,UObject *,const wchar_t *) , _dasUnreal_static_95_AssertIfInConstructor , SimNode_ExtFuncCall >(lib,"AssertIfInConstructor","_dasUnreal_static_95_AssertIfInConstructor")
		->args({"self","Outer","ErrorMessage"})
		->arg_type(0,makeType<TExplicit<const FObjectInitializer*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_289 = das::das_call_member< void (FObjectInitializer::*)(),&FObjectInitializer::FinalizeSubobjectClassInitialization >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1272:19
	makeExtern<DAS_CALL_METHOD(_method_289), SimNode_ExtFuncCall >(lib,"FinalizeSubobjectClassInitialization","das_call_member< void (FObjectInitializer::*)() , &FObjectInitializer::FinalizeSubobjectClassInitialization >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1278:45
	makeExtern< FObjectInitializer & (*)(const FObjectInitializer *) , _dasUnreal_static_96_Get , SimNode_ExtFuncCallRef >(lib,"Get","_dasUnreal_static_96_Get")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FObjectInitializer*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStaticConstructObjectParameters,const UClass *>(*this,lib,"FStaticConstructObjectParameters","FStaticConstructObjectParameters")
		->args({"InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1545:21
	makeExtern< void (*)(const FScriptIntegrationObjectHelper *,const FObjectInitializer &,UObject *,UClass *,UObject *) , _dasUnreal_static_97_InitProperties , SimNode_ExtFuncCall >(lib,"InitProperties","_dasUnreal_static_97_InitProperties")
		->args({"self","ObjectInitializer","Obj","DefaultsClass","DefaultData"})
		->arg_type(0,makeType<TExplicit<const FScriptIntegrationObjectHelper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1555:21
	makeExtern< bool (*)(const FScriptIntegrationObjectHelper *,const FObjectInitializer &) , _dasUnreal_static_98_InitSubobjectProperties , SimNode_ExtFuncCall >(lib,"InitSubobjectProperties","_dasUnreal_static_98_InitSubobjectProperties")
		->args({"self","ObjectInitializer"})
		->arg_type(0,makeType<TExplicit<const FScriptIntegrationObjectHelper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1567:21
	makeExtern< void (*)(const FScriptIntegrationObjectHelper *,const FObjectInitializer &,UClass *,bool,bool) , _dasUnreal_static_99_InstanceSubobjects , SimNode_ExtFuncCall >(lib,"InstanceSubobjects","_dasUnreal_static_99_InstanceSubobjects")
		->args({"self","ObjectInitializer","Class","bNeedInstancing","bNeedSubobjectInstancing"})
		->arg_type(0,makeType<TExplicit<const FScriptIntegrationObjectHelper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1578:21
	makeExtern< void (*)(const FScriptIntegrationObjectHelper *,FObjectInitializer &) , _dasUnreal_static_100_PostConstructInitObject , SimNode_ExtFuncCall >(lib,"PostConstructInitObject","_dasUnreal_static_100_PostConstructInitObject")
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
	makeExtern< void (*)(FReferenceCollector &,UObject **) , _dasUnreal_virtual_101_AddStableReference , SimNode_ExtFuncCall >(lib,"AddStableReference","_dasUnreal_virtual_101_AddStableReference")
		->args({"self","Object"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_296 = das::das_call_member< void (FReferenceCollector::*)(const UStruct *,void *,const UObject *),&FReferenceCollector::AddPropertyReferences >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2529:23
	makeExtern<DAS_CALL_METHOD(_method_296), SimNode_ExtFuncCall >(lib,"AddPropertyReferences","das_call_member< void (FReferenceCollector::*)(const UStruct *,void *,const UObject *) , &FReferenceCollector::AddPropertyReferences >::invoke")
		->args({"self","Struct","Instance","ReferencingObject"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_297 = das::das_call_member< void (FReferenceCollector::*)(const UScriptStruct *,void *,const UObject *),&FReferenceCollector::AddPropertyReferencesWithStructARO >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2532:23
	makeExtern<DAS_CALL_METHOD(_method_297), SimNode_ExtFuncCall >(lib,"AddPropertyReferencesWithStructARO","das_call_member< void (FReferenceCollector::*)(const UScriptStruct *,void *,const UObject *) , &FReferenceCollector::AddPropertyReferencesWithStructARO >::invoke")
		->args({"self","Struct","Instance","ReferencingObject"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_298 = das::das_call_member< void (FReferenceCollector::*)(const UClass *,void *,const UObject *),&FReferenceCollector::AddPropertyReferencesWithStructARO >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2535:23
	makeExtern<DAS_CALL_METHOD(_method_298), SimNode_ExtFuncCall >(lib,"AddPropertyReferencesWithStructARO","das_call_member< void (FReferenceCollector::*)(const UClass *,void *,const UObject *) , &FReferenceCollector::AddPropertyReferencesWithStructARO >::invoke")
		->args({"self","Class","Instance","ReferencingObject"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_299 = das::das_call_member< void (FReferenceCollector::*)(const UStruct *,void *,const UObject *),&FReferenceCollector::AddPropertyReferencesLimitedToObjectProperties >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2538:23
	makeExtern<DAS_CALL_METHOD(_method_299), SimNode_ExtFuncCall >(lib,"AddPropertyReferencesLimitedToObjectProperties","das_call_member< void (FReferenceCollector::*)(const UStruct *,void *,const UObject *) , &FReferenceCollector::AddPropertyReferencesLimitedToObjectProperties >::invoke")
		->args({"self","Struct","Instance","ReferencingObject"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2546:15
	makeExtern< bool (*)(const FReferenceCollector &) , _dasUnreal_virtual_102_NeedsPropertyReferencer , SimNode_ExtFuncCall >(lib,"NeedsPropertyReferencer","_dasUnreal_virtual_102_NeedsPropertyReferencer")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2559:15
	makeExtern< void (*)(FReferenceCollector &,bool) , _dasUnreal_virtual_103_AllowEliminatingReferences , SimNode_ExtFuncCall >(lib,"AllowEliminatingReferences","_dasUnreal_virtual_103_AllowEliminatingReferences")
		->args({"self","bAllow"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2565:15
	makeExtern< void (*)(FReferenceCollector &,FProperty *) , _dasUnreal_virtual_104_SetSerializedProperty , SimNode_ExtFuncCall >(lib,"SetSerializedProperty","_dasUnreal_virtual_104_SetSerializedProperty")
		->args({"self","Inproperty"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2569:27
	makeExtern< FProperty * (*)(const FReferenceCollector &) , _dasUnreal_virtual_105_GetSerializedProperty , SimNode_ExtFuncCall >(lib,"GetSerializedProperty","_dasUnreal_virtual_105_GetSerializedProperty")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2574:15
	makeExtern< bool (*)(FReferenceCollector &,UObject **) , _dasUnreal_virtual_106_MarkWeakObjectReferenceForClearing , SimNode_ExtFuncCall >(lib,"MarkWeakObjectReferenceForClearing","_dasUnreal_virtual_106_MarkWeakObjectReferenceForClearing")
		->args({"self","WeakReference"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2578:15
	makeExtern< void (*)(FReferenceCollector &,bool) , _dasUnreal_virtual_107_SetIsProcessingNativeReferences , SimNode_ExtFuncCall >(lib,"SetIsProcessingNativeReferences","_dasUnreal_virtual_107_SetIsProcessingNativeReferences")
		->args({"self","bIsNative"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2582:15
	makeExtern< bool (*)(const FReferenceCollector &) , _dasUnreal_virtual_108_IsProcessingNativeReferences , SimNode_ExtFuncCall >(lib,"IsProcessingNativeReferences","_dasUnreal_virtual_108_IsProcessingNativeReferences")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2585:15
	makeExtern< bool (*)(const FReferenceCollector &) , _dasUnreal_virtual_109_NeedsInitialReferences , SimNode_ExtFuncCall >(lib,"NeedsInitialReferences","_dasUnreal_virtual_109_NeedsInitialReferences")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2794:31
	makeExtern< void (*)(FReferenceFinder &,UObject *,UObject *,FProperty *) , _dasUnreal_virtual_110_FindReferences , SimNode_ExtFuncCall >(lib,"FindReferences","_dasUnreal_virtual_110_FindReferences")
		->args({"self","Object","ReferencingObject","ReferencingProperty"})
		->arg_init(2,make_smart<ExprConstPtr>())
		->arg_init(3,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2798:15
	makeExtern< bool (*)(const FReferenceFinder &) , _dasUnreal_virtual_111_IsIgnoringArchetypeRef , SimNode_ExtFuncCall >(lib,"IsIgnoringArchetypeRef","_dasUnreal_virtual_111_IsIgnoringArchetypeRef")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2799:15
	makeExtern< bool (*)(const FReferenceFinder &) , _dasUnreal_virtual_112_IsIgnoringTransient , SimNode_ExtFuncCall >(lib,"IsIgnoringTransient","_dasUnreal_virtual_112_IsIgnoringTransient")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2800:15
	makeExtern< void (*)(FReferenceFinder &,FProperty *) , _dasUnreal_virtual_113_SetSerializedProperty , SimNode_ExtFuncCall >(lib,"SetSerializedProperty","_dasUnreal_virtual_113_SetSerializedProperty")
		->args({"self","Inproperty"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2804:27
	makeExtern< FProperty * (*)(const FReferenceFinder &) , _dasUnreal_virtual_114_GetSerializedProperty , SimNode_ExtFuncCall >(lib,"GetSerializedProperty","_dasUnreal_virtual_114_GetSerializedProperty")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2879:14
	makeExtern< void (*)(const FCoreUObjectDelegates *,UObject *) , _dasUnreal_static_115_BroadcastOnObjectModified , SimNode_ExtFuncCall >(lib,"BroadcastOnObjectModified","_dasUnreal_static_115_BroadcastOnObjectModified")
		->args({"self","Object"})
		->arg_type(0,makeType<TExplicit<const FCoreUObjectDelegates*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3062:32
	makeExtern< UScriptStruct * (*)() , GetFallbackStruct , SimNode_ExtFuncCall >(lib,"GetFallbackStruct","GetFallbackStruct")
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
	makeExtern< FString (*)(const FAssetMsg *,const wchar_t *) , _dasUnreal_static_116_FormatPathForAssetLog , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FormatPathForAssetLog","_dasUnreal_static_116_FormatPathForAssetLog")
		->args({"self","Path"})
		->arg_type(0,makeType<TExplicit<const FAssetMsg*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3157:33
	makeExtern< FString (*)(const FAssetMsg *,const UObject *) , _dasUnreal_static_117_FormatPathForAssetLog , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FormatPathForAssetLog","_dasUnreal_static_117_FormatPathForAssetLog")
		->args({"self","Object"})
		->arg_type(0,makeType<TExplicit<const FAssetMsg*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3159:33
	makeExtern< FString (*)(const FAssetMsg *,const wchar_t *,const FString &) , _dasUnreal_static_118_GetAssetLogString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAssetLogString","_dasUnreal_static_118_GetAssetLogString")
		->args({"self","Path","Message"})
		->arg_type(0,makeType<TExplicit<const FAssetMsg*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3161:33
	makeExtern< FString (*)(const FAssetMsg *,const UObject *,const FString &) , _dasUnreal_static_119_GetAssetLogString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAssetLogString","_dasUnreal_static_119_GetAssetLogString")
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
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3787:39
	makeExtern< EDataValidationResult (*)(EDataValidationResult,EDataValidationResult) , CombineDataValidationResults , SimNode_ExtFuncCall >(lib,"CombineDataValidationResults","CombineDataValidationResults")
		->args({"Result1","Result2"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<UObjectBase,UClass *,EObjectFlags,EInternalObjectFlags,UObject *,FName,int,int>(*this,lib,"UObjectBase","UObjectBase")
		->args({"InClass","InFlags","InInternalFlags","InOuter","InName","InInternalIndex","InSerialNumber"})
		->arg_init(5,make_smart<ExprConstInt>(-1))
		->arg_init(6,make_smart<ExprConstInt>(0));
	using _method_318 = das::das_call_member< bool (UObjectBase::*)() const,&UObjectBase::IsValidLowLevel >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:122:23
	makeExtern<DAS_CALL_METHOD(_method_318), SimNode_ExtFuncCall >(lib,"IsValidLowLevel","das_call_member< bool (UObjectBase::*)() const , &UObjectBase::IsValidLowLevel >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_319 = das::das_call_member< bool (UObjectBase::*)(bool) const,&UObjectBase::IsValidLowLevelFast >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:131:23
	makeExtern<DAS_CALL_METHOD(_method_319), SimNode_ExtFuncCall >(lib,"IsValidLowLevelFast","das_call_member< bool (UObjectBase::*)(bool) const , &UObjectBase::IsValidLowLevelFast >::invoke")
		->args({"self","bRecursive"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_320 = das::das_call_member< unsigned int (UObjectBase::*)() const,&UObjectBase::GetUniqueID >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:137:21
	makeExtern<DAS_CALL_METHOD(_method_320), SimNode_ExtFuncCall >(lib,"GetUniqueID","das_call_member< unsigned int (UObjectBase::*)() const , &UObjectBase::GetUniqueID >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_321 = das::das_call_member< UClass * (UObjectBase::*)() const,&UObjectBase::GetClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:143:22
	makeExtern<DAS_CALL_METHOD(_method_321), SimNode_ExtFuncCall >(lib,"GetClass","das_call_member< UClass * (UObjectBase::*)() const , &UObjectBase::GetClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_322 = das::das_call_member< UObject * (UObjectBase::*)() const,&UObjectBase::GetOuter >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:149:23
	makeExtern<DAS_CALL_METHOD(_method_322), SimNode_ExtFuncCall >(lib,"GetOuter","das_call_member< UObject * (UObjectBase::*)() const , &UObjectBase::GetOuter >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_323 = das::das_call_member< FName (UObjectBase::*)() const,&UObjectBase::GetFName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:155:20
	makeExtern<DAS_CALL_METHOD(_method_323), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFName","das_call_member< FName (UObjectBase::*)() const , &UObjectBase::GetFName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:161:32
	makeExtern< FName (*)(const UObjectBase &) , _dasUnreal_virtual_120_GetFNameForStatID , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFNameForStatID","_dasUnreal_virtual_120_GetFNameForStatID")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:164:33
	makeExtern< FString (*)(const UObjectBase *,const wchar_t *) , _dasUnreal_static_121_RemoveClassPrefix , SimNode_ExtFuncCallAndCopyOrMove >(lib,"RemoveClassPrefix","_dasUnreal_static_121_RemoveClassPrefix")
		->args({"self","ClassName"})
		->arg_type(0,makeType<TExplicit<const UObjectBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_326 = das::das_call_member< UPackage * (UObjectBase::*)() const,&UObjectBase::GetExternalPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:167:28
	makeExtern<DAS_CALL_METHOD(_method_326), SimNode_ExtFuncCall >(lib,"GetExternalPackage","das_call_member< UPackage * (UObjectBase::*)() const , &UObjectBase::GetExternalPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_327 = das::das_call_member< void (UObjectBase::*)(UPackage *),&UObjectBase::SetExternalPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:170:23
	makeExtern<DAS_CALL_METHOD(_method_327), SimNode_ExtFuncCall >(lib,"SetExternalPackage","das_call_member< void (UObjectBase::*)(UPackage *) , &UObjectBase::SetExternalPackage >::invoke")
		->args({"self","InPackage"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_328 = das::das_call_member< UPackage * (UObjectBase::*)() const,&UObjectBase::GetExternalPackageInternal >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:173:28
	makeExtern<DAS_CALL_METHOD(_method_328), SimNode_ExtFuncCall >(lib,"GetExternalPackageInternal","das_call_member< UPackage * (UObjectBase::*)() const , &UObjectBase::GetExternalPackageInternal >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_329 = das::das_call_member< EObjectFlags (UObjectBase::*)() const,&UObjectBase::GetFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:193:27
	makeExtern<DAS_CALL_METHOD(_method_329), SimNode_ExtFuncCall >(lib,"GetFlags","das_call_member< EObjectFlags (UObjectBase::*)() const , &UObjectBase::GetFlags >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_330 = das::das_call_member< void (UObjectBase::*)(EObjectFlags),&UObjectBase::AtomicallySetFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:203:21
	makeExtern<DAS_CALL_METHOD(_method_330), SimNode_ExtFuncCall >(lib,"AtomicallySetFlags","das_call_member< void (UObjectBase::*)(EObjectFlags) , &UObjectBase::AtomicallySetFlags >::invoke")
		->args({"self","FlagsToAdd"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_331 = das::das_call_member< void (UObjectBase::*)(EObjectFlags),&UObjectBase::AtomicallyClearFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:220:21
	makeExtern<DAS_CALL_METHOD(_method_331), SimNode_ExtFuncCall >(lib,"AtomicallyClearFlags","das_call_member< void (UObjectBase::*)(EObjectFlags) , &UObjectBase::AtomicallyClearFlags >::invoke")
		->args({"self","FlagsToClear"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:234:14
	makeExtern< void (*)(const UObjectBase *,UObject *) , _dasUnreal_static_122_PrefetchClass , SimNode_ExtFuncCall >(lib,"PrefetchClass","_dasUnreal_static_122_PrefetchClass")
		->args({"self","Object"})
		->arg_type(0,makeType<TExplicit<const UObjectBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:235:14
	makeExtern< void (*)(const UObjectBase *,UObject *) , _dasUnreal_static_123_PrefetchOuter , SimNode_ExtFuncCall >(lib,"PrefetchOuter","_dasUnreal_static_123_PrefetchOuter")
		->args({"self","Object"})
		->arg_type(0,makeType<TExplicit<const UObjectBase*> >(lib))
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
	using _method_334 = das::das_call_member< void (UObjectBaseUtility::*)(EObjectFlags),&UObjectBaseUtility::SetFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:100:19
	makeExtern<DAS_CALL_METHOD(_method_334), SimNode_ExtFuncCall >(lib,"SetFlags","das_call_member< void (UObjectBaseUtility::*)(EObjectFlags) , &UObjectBaseUtility::SetFlags >::invoke")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_335 = das::das_call_member< void (UObjectBaseUtility::*)(EObjectFlags),&UObjectBaseUtility::ClearFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:108:19
	makeExtern<DAS_CALL_METHOD(_method_335), SimNode_ExtFuncCall >(lib,"ClearFlags","das_call_member< void (UObjectBaseUtility::*)(EObjectFlags) , &UObjectBaseUtility::ClearFlags >::invoke")
		->args({"self","FlagsToClear"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_336 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const,&UObjectBaseUtility::HasAnyFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:121:19
	makeExtern<DAS_CALL_METHOD(_method_336), SimNode_ExtFuncCall >(lib,"HasAnyFlags","das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const , &UObjectBaseUtility::HasAnyFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_337 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const,&UObjectBaseUtility::HasAllFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:133:19
	makeExtern<DAS_CALL_METHOD(_method_337), SimNode_ExtFuncCall >(lib,"HasAllFlags","das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const , &UObjectBaseUtility::HasAllFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_338 = das::das_call_member< EObjectFlags (UObjectBaseUtility::*)(EObjectFlags) const,&UObjectBaseUtility::GetMaskedFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:145:27
	makeExtern<DAS_CALL_METHOD(_method_338), SimNode_ExtFuncCall >(lib,"GetMaskedFlags","das_call_member< EObjectFlags (UObjectBaseUtility::*)(EObjectFlags) const , &UObjectBaseUtility::GetMaskedFlags >::invoke")
		->args({"self","Mask"})
		->arg_init(1,make_smart<ExprConstEnumeration>(-1,makeType<EObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_339 = das::das_call_member< void (UObjectBaseUtility::*)(EObjectMark) const,&UObjectBaseUtility::Mark >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:159:19
	makeExtern<DAS_CALL_METHOD(_method_339), SimNode_ExtFuncCall >(lib,"Mark","das_call_member< void (UObjectBaseUtility::*)(EObjectMark) const , &UObjectBaseUtility::Mark >::invoke")
		->args({"self","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_340 = das::das_call_member< void (UObjectBaseUtility::*)(EObjectMark) const,&UObjectBaseUtility::UnMark >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:169:19
	makeExtern<DAS_CALL_METHOD(_method_340), SimNode_ExtFuncCall >(lib,"UnMark","das_call_member< void (UObjectBaseUtility::*)(EObjectMark) const , &UObjectBaseUtility::UnMark >::invoke")
		->args({"self","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_341 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectMark) const,&UObjectBaseUtility::HasAnyMarks >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:180:19
	makeExtern<DAS_CALL_METHOD(_method_341), SimNode_ExtFuncCall >(lib,"HasAnyMarks","das_call_member< bool (UObjectBaseUtility::*)(EObjectMark) const , &UObjectBaseUtility::HasAnyMarks >::invoke")
		->args({"self","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_342 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectMark) const,&UObjectBaseUtility::HasAllMarks >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:191:19
	makeExtern<DAS_CALL_METHOD(_method_342), SimNode_ExtFuncCall >(lib,"HasAllMarks","das_call_member< bool (UObjectBaseUtility::*)(EObjectMark) const , &UObjectBaseUtility::HasAllMarks >::invoke")
		->args({"self","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_343 = das::das_call_member< EObjectMark (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetAllMarks >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:202:26
	makeExtern<DAS_CALL_METHOD(_method_343), SimNode_ExtFuncCall >(lib,"GetAllMarks","das_call_member< EObjectMark (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetAllMarks >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_344 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::MarkAsGarbage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:263:19
	makeExtern<DAS_CALL_METHOD(_method_344), SimNode_ExtFuncCall >(lib,"MarkAsGarbage","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::MarkAsGarbage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_345 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::ClearGarbage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:279:19
	makeExtern<DAS_CALL_METHOD(_method_345), SimNode_ExtFuncCall >(lib,"ClearGarbage","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::ClearGarbage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_346 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::AddToRoot >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:295:19
	makeExtern<DAS_CALL_METHOD(_method_346), SimNode_ExtFuncCall >(lib,"AddToRoot","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::AddToRoot >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_347 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::RemoveFromRoot >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:301:19
	makeExtern<DAS_CALL_METHOD(_method_347), SimNode_ExtFuncCall >(lib,"RemoveFromRoot","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::RemoveFromRoot >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

