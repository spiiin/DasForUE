// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_5() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2565:15
	makeExtern< void (*)(FReferenceCollector &,FProperty *) , _dasUnreal_virtual_92_SetSerializedProperty , SimNode_ExtFuncCall >(lib,"SetSerializedProperty","_dasUnreal_virtual_92_SetSerializedProperty")
		->args({"self","Inproperty"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2569:27
	makeExtern< FProperty * (*)(const FReferenceCollector &) , _dasUnreal_virtual_93_GetSerializedProperty , SimNode_ExtFuncCall >(lib,"GetSerializedProperty","_dasUnreal_virtual_93_GetSerializedProperty")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2574:15
	makeExtern< bool (*)(FReferenceCollector &,UObject **) , _dasUnreal_virtual_94_MarkWeakObjectReferenceForClearing , SimNode_ExtFuncCall >(lib,"MarkWeakObjectReferenceForClearing","_dasUnreal_virtual_94_MarkWeakObjectReferenceForClearing")
		->args({"self","WeakReference"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2578:15
	makeExtern< void (*)(FReferenceCollector &,bool) , _dasUnreal_virtual_95_SetIsProcessingNativeReferences , SimNode_ExtFuncCall >(lib,"SetIsProcessingNativeReferences","_dasUnreal_virtual_95_SetIsProcessingNativeReferences")
		->args({"self","bIsNative"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2582:15
	makeExtern< bool (*)(const FReferenceCollector &) , _dasUnreal_virtual_96_IsProcessingNativeReferences , SimNode_ExtFuncCall >(lib,"IsProcessingNativeReferences","_dasUnreal_virtual_96_IsProcessingNativeReferences")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2585:15
	makeExtern< bool (*)(const FReferenceCollector &) , _dasUnreal_virtual_97_NeedsInitialReferences , SimNode_ExtFuncCall >(lib,"NeedsInitialReferences","_dasUnreal_virtual_97_NeedsInitialReferences")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2794:31
	makeExtern< void (*)(FReferenceFinder &,UObject *,UObject *,FProperty *) , _dasUnreal_virtual_98_FindReferences , SimNode_ExtFuncCall >(lib,"FindReferences","_dasUnreal_virtual_98_FindReferences")
		->args({"self","Object","ReferencingObject","ReferencingProperty"})
		->arg_init(2,make_smart<ExprConstPtr>())
		->arg_init(3,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2798:15
	makeExtern< bool (*)(const FReferenceFinder &) , _dasUnreal_virtual_99_IsIgnoringArchetypeRef , SimNode_ExtFuncCall >(lib,"IsIgnoringArchetypeRef","_dasUnreal_virtual_99_IsIgnoringArchetypeRef")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2799:15
	makeExtern< bool (*)(const FReferenceFinder &) , _dasUnreal_virtual_100_IsIgnoringTransient , SimNode_ExtFuncCall >(lib,"IsIgnoringTransient","_dasUnreal_virtual_100_IsIgnoringTransient")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2800:15
	makeExtern< void (*)(FReferenceFinder &,FProperty *) , _dasUnreal_virtual_101_SetSerializedProperty , SimNode_ExtFuncCall >(lib,"SetSerializedProperty","_dasUnreal_virtual_101_SetSerializedProperty")
		->args({"self","Inproperty"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2804:27
	makeExtern< FProperty * (*)(const FReferenceFinder &) , _dasUnreal_virtual_102_GetSerializedProperty , SimNode_ExtFuncCall >(lib,"GetSerializedProperty","_dasUnreal_virtual_102_GetSerializedProperty")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2879:14
	makeExtern< void (*)(const FCoreUObjectDelegates *,UObject *) , _dasUnreal_static_103_BroadcastOnObjectModified , SimNode_ExtFuncCall >(lib,"BroadcastOnObjectModified","_dasUnreal_static_103_BroadcastOnObjectModified")
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
	makeExtern< FString (*)(const FAssetMsg *,const wchar_t *) , _dasUnreal_static_104_FormatPathForAssetLog , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FormatPathForAssetLog","_dasUnreal_static_104_FormatPathForAssetLog")
		->args({"self","Path"})
		->arg_type(0,makeType<TExplicit<const FAssetMsg*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3157:33
	makeExtern< FString (*)(const FAssetMsg *,const UObject *) , _dasUnreal_static_105_FormatPathForAssetLog , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FormatPathForAssetLog","_dasUnreal_static_105_FormatPathForAssetLog")
		->args({"self","Object"})
		->arg_type(0,makeType<TExplicit<const FAssetMsg*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3159:33
	makeExtern< FString (*)(const FAssetMsg *,const wchar_t *,const FString &) , _dasUnreal_static_106_GetAssetLogString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAssetLogString","_dasUnreal_static_106_GetAssetLogString")
		->args({"self","Path","Message"})
		->arg_type(0,makeType<TExplicit<const FAssetMsg*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3161:33
	makeExtern< FString (*)(const FAssetMsg *,const UObject *,const FString &) , _dasUnreal_static_107_GetAssetLogString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAssetLogString","_dasUnreal_static_107_GetAssetLogString")
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
	using _method_291 = das::das_call_member< bool (UObjectBase::*)() const,&UObjectBase::IsValidLowLevel >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:122:23
	makeExtern<DAS_CALL_METHOD(_method_291), SimNode_ExtFuncCall >(lib,"IsValidLowLevel","das_call_member< bool (UObjectBase::*)() const , &UObjectBase::IsValidLowLevel >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_292 = das::das_call_member< bool (UObjectBase::*)(bool) const,&UObjectBase::IsValidLowLevelFast >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:131:23
	makeExtern<DAS_CALL_METHOD(_method_292), SimNode_ExtFuncCall >(lib,"IsValidLowLevelFast","das_call_member< bool (UObjectBase::*)(bool) const , &UObjectBase::IsValidLowLevelFast >::invoke")
		->args({"self","bRecursive"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_293 = das::das_call_member< unsigned int (UObjectBase::*)() const,&UObjectBase::GetUniqueID >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:137:21
	makeExtern<DAS_CALL_METHOD(_method_293), SimNode_ExtFuncCall >(lib,"GetUniqueID","das_call_member< unsigned int (UObjectBase::*)() const , &UObjectBase::GetUniqueID >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_294 = das::das_call_member< UClass * (UObjectBase::*)() const,&UObjectBase::GetClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:143:22
	makeExtern<DAS_CALL_METHOD(_method_294), SimNode_ExtFuncCall >(lib,"GetClass","das_call_member< UClass * (UObjectBase::*)() const , &UObjectBase::GetClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_295 = das::das_call_member< UObject * (UObjectBase::*)() const,&UObjectBase::GetOuter >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:149:23
	makeExtern<DAS_CALL_METHOD(_method_295), SimNode_ExtFuncCall >(lib,"GetOuter","das_call_member< UObject * (UObjectBase::*)() const , &UObjectBase::GetOuter >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_296 = das::das_call_member< FName (UObjectBase::*)() const,&UObjectBase::GetFName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:155:20
	makeExtern<DAS_CALL_METHOD(_method_296), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFName","das_call_member< FName (UObjectBase::*)() const , &UObjectBase::GetFName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:161:32
	makeExtern< FName (*)(const UObjectBase &) , _dasUnreal_virtual_108_GetFNameForStatID , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFNameForStatID","_dasUnreal_virtual_108_GetFNameForStatID")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:164:33
	makeExtern< FString (*)(const UObjectBase *,const wchar_t *) , _dasUnreal_static_109_RemoveClassPrefix , SimNode_ExtFuncCallAndCopyOrMove >(lib,"RemoveClassPrefix","_dasUnreal_static_109_RemoveClassPrefix")
		->args({"self","ClassName"})
		->arg_type(0,makeType<TExplicit<const UObjectBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_299 = das::das_call_member< UPackage * (UObjectBase::*)() const,&UObjectBase::GetExternalPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:167:28
	makeExtern<DAS_CALL_METHOD(_method_299), SimNode_ExtFuncCall >(lib,"GetExternalPackage","das_call_member< UPackage * (UObjectBase::*)() const , &UObjectBase::GetExternalPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_300 = das::das_call_member< void (UObjectBase::*)(UPackage *),&UObjectBase::SetExternalPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:170:23
	makeExtern<DAS_CALL_METHOD(_method_300), SimNode_ExtFuncCall >(lib,"SetExternalPackage","das_call_member< void (UObjectBase::*)(UPackage *) , &UObjectBase::SetExternalPackage >::invoke")
		->args({"self","InPackage"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_301 = das::das_call_member< UPackage * (UObjectBase::*)() const,&UObjectBase::GetExternalPackageInternal >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:173:28
	makeExtern<DAS_CALL_METHOD(_method_301), SimNode_ExtFuncCall >(lib,"GetExternalPackageInternal","das_call_member< UPackage * (UObjectBase::*)() const , &UObjectBase::GetExternalPackageInternal >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_302 = das::das_call_member< EObjectFlags (UObjectBase::*)() const,&UObjectBase::GetFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:193:27
	makeExtern<DAS_CALL_METHOD(_method_302), SimNode_ExtFuncCall >(lib,"GetFlags","das_call_member< EObjectFlags (UObjectBase::*)() const , &UObjectBase::GetFlags >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_303 = das::das_call_member< void (UObjectBase::*)(EObjectFlags),&UObjectBase::AtomicallySetFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:203:21
	makeExtern<DAS_CALL_METHOD(_method_303), SimNode_ExtFuncCall >(lib,"AtomicallySetFlags","das_call_member< void (UObjectBase::*)(EObjectFlags) , &UObjectBase::AtomicallySetFlags >::invoke")
		->args({"self","FlagsToAdd"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_304 = das::das_call_member< void (UObjectBase::*)(EObjectFlags),&UObjectBase::AtomicallyClearFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:220:21
	makeExtern<DAS_CALL_METHOD(_method_304), SimNode_ExtFuncCall >(lib,"AtomicallyClearFlags","das_call_member< void (UObjectBase::*)(EObjectFlags) , &UObjectBase::AtomicallyClearFlags >::invoke")
		->args({"self","FlagsToClear"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:234:14
	makeExtern< void (*)(const UObjectBase *,UObject *) , _dasUnreal_static_110_PrefetchClass , SimNode_ExtFuncCall >(lib,"PrefetchClass","_dasUnreal_static_110_PrefetchClass")
		->args({"self","Object"})
		->arg_type(0,makeType<TExplicit<const UObjectBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:235:14
	makeExtern< void (*)(const UObjectBase *,UObject *) , _dasUnreal_static_111_PrefetchOuter , SimNode_ExtFuncCall >(lib,"PrefetchOuter","_dasUnreal_static_111_PrefetchOuter")
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
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:446:38
	makeExtern< UScriptStruct * (*)(UObject *,const wchar_t *,SIZE_T,unsigned int,bool) , FindExistingStructIfHotReloadOrDynamic , SimNode_ExtFuncCall >(lib,"FindExistingStructIfHotReloadOrDynamic","FindExistingStructIfHotReloadOrDynamic")
		->args({"Outer","StructName","Size","Crc","bIsDynamic"})
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
	using _method_307 = das::das_call_member< void (UObjectBaseUtility::*)(EObjectFlags),&UObjectBaseUtility::SetFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:100:19
	makeExtern<DAS_CALL_METHOD(_method_307), SimNode_ExtFuncCall >(lib,"SetFlags","das_call_member< void (UObjectBaseUtility::*)(EObjectFlags) , &UObjectBaseUtility::SetFlags >::invoke")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_308 = das::das_call_member< void (UObjectBaseUtility::*)(EObjectFlags),&UObjectBaseUtility::ClearFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:108:19
	makeExtern<DAS_CALL_METHOD(_method_308), SimNode_ExtFuncCall >(lib,"ClearFlags","das_call_member< void (UObjectBaseUtility::*)(EObjectFlags) , &UObjectBaseUtility::ClearFlags >::invoke")
		->args({"self","FlagsToClear"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_309 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const,&UObjectBaseUtility::HasAnyFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:121:19
	makeExtern<DAS_CALL_METHOD(_method_309), SimNode_ExtFuncCall >(lib,"HasAnyFlags","das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const , &UObjectBaseUtility::HasAnyFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_310 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const,&UObjectBaseUtility::HasAllFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:133:19
	makeExtern<DAS_CALL_METHOD(_method_310), SimNode_ExtFuncCall >(lib,"HasAllFlags","das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const , &UObjectBaseUtility::HasAllFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_311 = das::das_call_member< EObjectFlags (UObjectBaseUtility::*)(EObjectFlags) const,&UObjectBaseUtility::GetMaskedFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:145:27
	makeExtern<DAS_CALL_METHOD(_method_311), SimNode_ExtFuncCall >(lib,"GetMaskedFlags","das_call_member< EObjectFlags (UObjectBaseUtility::*)(EObjectFlags) const , &UObjectBaseUtility::GetMaskedFlags >::invoke")
		->args({"self","Mask"})
		->arg_init(1,make_smart<ExprConstEnumeration>(-1,makeType<EObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_312 = das::das_call_member< void (UObjectBaseUtility::*)(EObjectMark) const,&UObjectBaseUtility::Mark >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:159:19
	makeExtern<DAS_CALL_METHOD(_method_312), SimNode_ExtFuncCall >(lib,"Mark","das_call_member< void (UObjectBaseUtility::*)(EObjectMark) const , &UObjectBaseUtility::Mark >::invoke")
		->args({"self","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_313 = das::das_call_member< void (UObjectBaseUtility::*)(EObjectMark) const,&UObjectBaseUtility::UnMark >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:169:19
	makeExtern<DAS_CALL_METHOD(_method_313), SimNode_ExtFuncCall >(lib,"UnMark","das_call_member< void (UObjectBaseUtility::*)(EObjectMark) const , &UObjectBaseUtility::UnMark >::invoke")
		->args({"self","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_314 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectMark) const,&UObjectBaseUtility::HasAnyMarks >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:180:19
	makeExtern<DAS_CALL_METHOD(_method_314), SimNode_ExtFuncCall >(lib,"HasAnyMarks","das_call_member< bool (UObjectBaseUtility::*)(EObjectMark) const , &UObjectBaseUtility::HasAnyMarks >::invoke")
		->args({"self","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_315 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectMark) const,&UObjectBaseUtility::HasAllMarks >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:191:19
	makeExtern<DAS_CALL_METHOD(_method_315), SimNode_ExtFuncCall >(lib,"HasAllMarks","das_call_member< bool (UObjectBaseUtility::*)(EObjectMark) const , &UObjectBaseUtility::HasAllMarks >::invoke")
		->args({"self","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_316 = das::das_call_member< EObjectMark (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetAllMarks >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:202:26
	makeExtern<DAS_CALL_METHOD(_method_316), SimNode_ExtFuncCall >(lib,"GetAllMarks","das_call_member< EObjectMark (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetAllMarks >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_317 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsPendingKill >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:211:19
	makeExtern<DAS_CALL_METHOD(_method_317), SimNode_ExtFuncCall >(lib,"IsPendingKill","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsPendingKill >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_318 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::MarkPendingKill >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:231:19
	makeExtern<DAS_CALL_METHOD(_method_318), SimNode_ExtFuncCall >(lib,"MarkPendingKill","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::MarkPendingKill >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_319 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::ClearPendingKill >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:248:19
	makeExtern<DAS_CALL_METHOD(_method_319), SimNode_ExtFuncCall >(lib,"ClearPendingKill","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::ClearPendingKill >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_320 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::MarkAsGarbage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:263:19
	makeExtern<DAS_CALL_METHOD(_method_320), SimNode_ExtFuncCall >(lib,"MarkAsGarbage","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::MarkAsGarbage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_321 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::ClearGarbage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:279:19
	makeExtern<DAS_CALL_METHOD(_method_321), SimNode_ExtFuncCall >(lib,"ClearGarbage","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::ClearGarbage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_322 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::AddToRoot >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:295:19
	makeExtern<DAS_CALL_METHOD(_method_322), SimNode_ExtFuncCall >(lib,"AddToRoot","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::AddToRoot >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_323 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::RemoveFromRoot >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:301:19
	makeExtern<DAS_CALL_METHOD(_method_323), SimNode_ExtFuncCall >(lib,"RemoveFromRoot","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::RemoveFromRoot >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_324 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsRooted >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:311:19
	makeExtern<DAS_CALL_METHOD(_method_324), SimNode_ExtFuncCall >(lib,"IsRooted","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsRooted >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_325 = das::das_call_member< bool (UObjectBaseUtility::*)(),&UObjectBaseUtility::ThisThreadAtomicallyClearedRFUnreachable >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:321:19
	makeExtern<DAS_CALL_METHOD(_method_325), SimNode_ExtFuncCall >(lib,"ThisThreadAtomicallyClearedRFUnreachable","das_call_member< bool (UObjectBaseUtility::*)() , &UObjectBaseUtility::ThisThreadAtomicallyClearedRFUnreachable >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_326 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsUnreachable >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:327:19
	makeExtern<DAS_CALL_METHOD(_method_326), SimNode_ExtFuncCall >(lib,"IsUnreachable","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsUnreachable >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_327 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsPendingKillOrUnreachable >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:334:19
	makeExtern<DAS_CALL_METHOD(_method_327), SimNode_ExtFuncCall >(lib,"IsPendingKillOrUnreachable","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsPendingKillOrUnreachable >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_328 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsNative >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:342:19
	makeExtern<DAS_CALL_METHOD(_method_328), SimNode_ExtFuncCall >(lib,"IsNative","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsNative >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_329 = das::das_call_member< void (UObjectBaseUtility::*)(EInternalObjectFlags) const,&UObjectBaseUtility::SetInternalFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:353:19
	makeExtern<DAS_CALL_METHOD(_method_329), SimNode_ExtFuncCall >(lib,"SetInternalFlags","das_call_member< void (UObjectBaseUtility::*)(EInternalObjectFlags) const , &UObjectBaseUtility::SetInternalFlags >::invoke")
		->args({"self","FlagsToSet"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_330 = das::das_call_member< EInternalObjectFlags (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetInternalFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:368:35
	makeExtern<DAS_CALL_METHOD(_method_330), SimNode_ExtFuncCall >(lib,"GetInternalFlags","das_call_member< EInternalObjectFlags (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetInternalFlags >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_331 = das::das_call_member< bool (UObjectBaseUtility::*)(EInternalObjectFlags) const,&UObjectBaseUtility::HasAnyInternalFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:379:19
	makeExtern<DAS_CALL_METHOD(_method_331), SimNode_ExtFuncCall >(lib,"HasAnyInternalFlags","das_call_member< bool (UObjectBaseUtility::*)(EInternalObjectFlags) const , &UObjectBaseUtility::HasAnyInternalFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_332 = das::das_call_member< void (UObjectBaseUtility::*)(EInternalObjectFlags) const,&UObjectBaseUtility::ClearInternalFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:390:19
	makeExtern<DAS_CALL_METHOD(_method_332), SimNode_ExtFuncCall >(lib,"ClearInternalFlags","das_call_member< void (UObjectBaseUtility::*)(EInternalObjectFlags) const , &UObjectBaseUtility::ClearInternalFlags >::invoke")
		->args({"self","FlagsToClear"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_333 = das::das_call_member< bool (UObjectBaseUtility::*)(EInternalObjectFlags) const,&UObjectBaseUtility::AtomicallyClearInternalFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:405:19
	makeExtern<DAS_CALL_METHOD(_method_333), SimNode_ExtFuncCall >(lib,"AtomicallyClearInternalFlags","das_call_member< bool (UObjectBaseUtility::*)(EInternalObjectFlags) const , &UObjectBaseUtility::AtomicallyClearInternalFlags >::invoke")
		->args({"self","FlagsToClear"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:467:21
	makeExtern< bool (*)(const UObjectBaseUtility *) , _dasUnreal_static_112_IsPendingKillEnabled , SimNode_ExtFuncCall >(lib,"IsPendingKillEnabled","_dasUnreal_static_112_IsPendingKillEnabled")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UObjectBaseUtility*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:473:42
	makeExtern< EInternalObjectFlags (*)(const UObjectBaseUtility *,const EInternalObjectFlags) , _dasUnreal_static_113_FixGarbageOrPendingKillInternalObjectFlags , SimNode_ExtFuncCall >(lib,"FixGarbageOrPendingKillInternalObjectFlags","_dasUnreal_static_113_FixGarbageOrPendingKillInternalObjectFlags")
		->args({"self","InFlags"})
		->arg_type(0,makeType<TExplicit<const UObjectBaseUtility*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:496:15
	makeExtern< bool (*)(const UObjectBaseUtility &) , _dasUnreal_virtual_114_CanBeClusterRoot , SimNode_ExtFuncCall >(lib,"CanBeClusterRoot","_dasUnreal_virtual_114_CanBeClusterRoot")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:506:31
	makeExtern< bool (*)(const UObjectBaseUtility &) , _dasUnreal_virtual_115_CanBeInCluster , SimNode_ExtFuncCall >(lib,"CanBeInCluster","_dasUnreal_virtual_115_CanBeInCluster")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:511:31
	makeExtern< void (*)(UObjectBaseUtility &) , _dasUnreal_virtual_116_CreateCluster , SimNode_ExtFuncCall >(lib,"CreateCluster","_dasUnreal_virtual_116_CreateCluster")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:516:15
	makeExtern< void (*)(UObjectBaseUtility &) , _dasUnreal_virtual_117_OnClusterMarkedAsPendingKill , SimNode_ExtFuncCall >(lib,"OnClusterMarkedAsPendingKill","_dasUnreal_virtual_117_OnClusterMarkedAsPendingKill")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_340 = das::das_call_member< void (UObjectBaseUtility::*)(UObjectBaseUtility *,bool),&UObjectBaseUtility::AddToCluster >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:523:23
	makeExtern<DAS_CALL_METHOD(_method_340), SimNode_ExtFuncCall >(lib,"AddToCluster","das_call_member< void (UObjectBaseUtility::*)(UObjectBaseUtility *,bool) , &UObjectBaseUtility::AddToCluster >::invoke")
		->args({"self","ClusterRootOrObjectFromCluster","bAddAsMutableObject"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_341 = das::das_call_member< FString (UObjectBaseUtility::*)(bool) const,&UObjectBaseUtility::GetFullGroupName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:536:26
	makeExtern<DAS_CALL_METHOD(_method_341), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFullGroupName","das_call_member< FString (UObjectBaseUtility::*)(bool) const , &UObjectBaseUtility::GetFullGroupName >::invoke")
		->args({"self","bStartWithOuter"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_342 = das::das_call_member< FString (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:543:22
	makeExtern<DAS_CALL_METHOD(_method_342), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetName","das_call_member< FString (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_343 = das::das_call_member< void (UObjectBaseUtility::*)(FString &) const,&UObjectBaseUtility::GetName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:549:19
	makeExtern<DAS_CALL_METHOD(_method_343), SimNode_ExtFuncCall >(lib,"GetName","das_call_member< void (UObjectBaseUtility::*)(FString &) const , &UObjectBaseUtility::GetName >::invoke")
		->args({"self","ResultString"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_344 = das::das_call_member< void (UObjectBaseUtility::*)(FString &) const,&UObjectBaseUtility::AppendName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:554:19
	makeExtern<DAS_CALL_METHOD(_method_344), SimNode_ExtFuncCall >(lib,"AppendName","das_call_member< void (UObjectBaseUtility::*)(FString &) const , &UObjectBaseUtility::AppendName >::invoke")
		->args({"self","ResultString"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

