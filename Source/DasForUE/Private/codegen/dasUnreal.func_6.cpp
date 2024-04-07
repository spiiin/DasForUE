// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_6() {
	using _method_345 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:784:23
	makeExtern<DAS_CALL_METHOD(_method_345), SimNode_ExtFuncCall >(lib,"IsDefaultSubobject","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsDefaultSubobject >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_346 = das::das_call_member< int (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetLinkerIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:804:24
	makeExtern<DAS_CALL_METHOD(_method_346), SimNode_ExtFuncCall >(lib,"GetLinkerIndex","das_call_member< int (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetLinkerIndex >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_347 = das::das_call_member< int (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetLinkerUE4Version >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:818:15
	makeExtern<DAS_CALL_METHOD(_method_347), SimNode_ExtFuncCall >(lib,"GetLinkerUE4Version","das_call_member< int (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetLinkerUE4Version >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_348 = das::das_call_member< int (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetLinkerLicenseeUEVersion >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:835:24
	makeExtern<DAS_CALL_METHOD(_method_348), SimNode_ExtFuncCall >(lib,"GetLinkerLicenseeUEVersion","das_call_member< int (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetLinkerLicenseeUEVersion >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_349 = das::das_call_member< int (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetLinkerLicenseeUE4Version >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:838:15
	makeExtern<DAS_CALL_METHOD(_method_349), SimNode_ExtFuncCall >(lib,"GetLinkerLicenseeUE4Version","das_call_member< int (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetLinkerLicenseeUE4Version >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_350 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::ResetStatID >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:868:19
	makeExtern<DAS_CALL_METHOD(_method_350), SimNode_ExtFuncCall >(lib,"ResetStatID","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::ResetStatID >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:918:18
	makeExtern< bool (*)(UObject *) , IsPossiblyAllocatedUObjectPointer , SimNode_ExtFuncCall >(lib,"IsPossiblyAllocatedUObjectPointer","IsPossiblyAllocatedUObjectPointer")
		->args({"Ptr"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:946:21
	makeExtern< FString (*)(const UObjectBaseUtility *) , GetNameSafe , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameSafe","GetNameSafe")
		->args({"Object"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:963:21
	makeExtern< FString (*)(const UObjectBaseUtility *) , GetPathNameSafe , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetPathNameSafe","GetPathNameSafe")
		->args({"Object"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:980:21
	makeExtern< FString (*)(const UObjectBaseUtility *) , GetFullNameSafe , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFullNameSafe","GetFullNameSafe")
		->args({"Object"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:996:25
	makeExtern< UClass * (*)(UClass *) , GetParentNativeClass , SimNode_ExtFuncCall >(lib,"GetParentNativeClass","GetParentNativeClass")
		->args({"Class"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:1003:23
	makeExtern< FName (*)(const UObjectBaseUtility *) , GetClassTraceScope , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetClassTraceScope","GetClassTraceScope")
		->args({"Object"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:88:2
	makeExtern< UClass * (*)(const UObject *) , _dasUnreal_static_109_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_109_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UObject*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:95:22
	makeExtern< const wchar_t * (*)(const UObject *) , _dasUnreal_static_110_StaticConfigName , SimNode_ExtFuncCall >(lib,"StaticConfigName","_dasUnreal_static_110_StaticConfigName")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UObject*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:99:14
	makeExtern< void (*)(const UObject *) , _dasUnreal_static_111_StaticRegisterNativesUObject , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUObject","_dasUnreal_static_111_StaticRegisterNativesUObject")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UObject*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_354 = das::das_call_member< UObject * (UObject::*)(FName,UClass *,UClass *,bool,bool),&UObject::CreateDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:120:27
	makeExtern<DAS_CALL_METHOD(_method_354), SimNode_ExtFuncCall >(lib,"CreateDefaultSubobject","das_call_member< UObject * (UObject::*)(FName,UClass *,UClass *,bool,bool) , &UObject::CreateDefaultSubobject >::invoke")
		->args({"self","SubobjectFName","ReturnType","ClassToCreateByDefault","bIsRequired","bIsTransient"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_355 = das::das_call_member< UObject * (UObject::*)(FName),&UObject::GetDefaultSubobjectByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:199:27
	makeExtern<DAS_CALL_METHOD(_method_355), SimNode_ExtFuncCall >(lib,"GetDefaultSubobjectByName","das_call_member< UObject * (UObject::*)(FName) , &UObject::GetDefaultSubobjectByName >::invoke")
		->args({"self","ToFind"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:217:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_112_PostInitProperties , SimNode_ExtFuncCall >(lib,"PostInitProperties","_dasUnreal_virtual_112_PostInitProperties")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:223:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_113_PostReinitProperties , SimNode_ExtFuncCall >(lib,"PostReinitProperties","_dasUnreal_virtual_113_PostReinitProperties")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:229:15
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_114_PostCDOContruct , SimNode_ExtFuncCall >(lib,"PostCDOContruct","_dasUnreal_virtual_114_PostCDOContruct")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:256:31
	makeExtern< bool (*)(UObject &,const wchar_t *) , _dasUnreal_virtual_115_PreSaveRoot , SimNode_ExtFuncCall >(lib,"PreSaveRoot","_dasUnreal_virtual_115_PreSaveRoot")
		->args({"self","Filename"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:268:31
	makeExtern< void (*)(UObject &,bool) , _dasUnreal_virtual_116_PostSaveRoot , SimNode_ExtFuncCall >(lib,"PostSaveRoot","_dasUnreal_virtual_116_PostSaveRoot")
		->args({"self","bCleanupIsRequired"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:299:31
	makeExtern< bool (*)(UObject &,bool) , _dasUnreal_virtual_117_Modify , SimNode_ExtFuncCall >(lib,"Modify","_dasUnreal_virtual_117_Modify")
		->args({"self","bAlwaysMarkDirty"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:302:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_118_IsCapturingAsRootObjectForTransaction , SimNode_ExtFuncCall >(lib,"IsCapturingAsRootObjectForTransaction","_dasUnreal_virtual_118_IsCapturingAsRootObjectForTransaction")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_363 = das::das_call_member< bool (UObject::*)() const,&UObject::CanModify >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:305:23
	makeExtern<DAS_CALL_METHOD(_method_363), SimNode_ExtFuncCall >(lib,"CanModify","das_call_member< bool (UObject::*)() const , &UObject::CanModify >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:315:15
	makeExtern< void (*)(UObject &,const FName &) , _dasUnreal_virtual_119_LoadedFromAnotherClass , SimNode_ExtFuncCall >(lib,"LoadedFromAnotherClass","_dasUnreal_virtual_119_LoadedFromAnotherClass")
		->args({"self","OldClassName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:330:15
	makeExtern< bool (*)(UObject &,const wchar_t *,UObject *&,bool) , _dasUnreal_virtual_120_ResolveSubobject , SimNode_ExtFuncCall >(lib,"ResolveSubobject","_dasUnreal_virtual_120_ResolveSubobject")
		->args({"self","SubObjectPath","OutObject","bLoadIfExists"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:335:15
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_121_IsReadyForAsyncPostLoad , SimNode_ExtFuncCall >(lib,"IsReadyForAsyncPostLoad","_dasUnreal_virtual_121_IsReadyForAsyncPostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:341:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_122_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_122_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:356:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_123_BeginDestroy , SimNode_ExtFuncCall >(lib,"BeginDestroy","_dasUnreal_virtual_123_BeginDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:363:15
	makeExtern< bool (*)(UObject &) , _dasUnreal_virtual_124_IsReadyForFinishDestroy , SimNode_ExtFuncCall >(lib,"IsReadyForFinishDestroy","_dasUnreal_virtual_124_IsReadyForFinishDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:369:15
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_125_PostLinkerChange , SimNode_ExtFuncCall >(lib,"PostLinkerChange","_dasUnreal_virtual_125_PostLinkerChange")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:377:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_126_FinishDestroy , SimNode_ExtFuncCall >(lib,"FinishDestroy","_dasUnreal_virtual_126_FinishDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:383:31
	makeExtern< void (*)(UObject &,FArchive &) , _dasUnreal_virtual_127_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_127_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:410:15
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_128_ShutdownAfterError , SimNode_ExtFuncCall >(lib,"ShutdownAfterError","_dasUnreal_virtual_128_ShutdownAfterError")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:413:15
	makeExtern< void (*)(UObject &,FProperty *) , _dasUnreal_virtual_129_PostInterpChange , SimNode_ExtFuncCall >(lib,"PostInterpChange","_dasUnreal_virtual_129_PostInterpChange")
		->args({"self","PropertyThatChanged"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_375 = das::das_call_member< void (UObject::*)(),&UObject::PostEditChange >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:456:23
	makeExtern<DAS_CALL_METHOD(_method_375), SimNode_ExtFuncCall >(lib,"PostEditChange","das_call_member< void (UObject::*)() , &UObject::PostEditChange >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_376 = das::das_call_member< void (UObject::*)(EEditChangePropagationFlags),&UObject::SetEditChangePropagationFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:472:23
	makeExtern<DAS_CALL_METHOD(_method_376), SimNode_ExtFuncCall >(lib,"SetEditChangePropagationFlags","das_call_member< void (UObject::*)(EEditChangePropagationFlags) , &UObject::SetEditChangePropagationFlags >::invoke")
		->args({"self","InFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:475:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_130_PreEditUndo , SimNode_ExtFuncCall >(lib,"PreEditUndo","_dasUnreal_virtual_130_PreEditUndo")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:513:15
	makeExtern< void (*)(UObject &,UObject *,const FName) , _dasUnreal_virtual_131_PostRename , SimNode_ExtFuncCall >(lib,"PostRename","_dasUnreal_virtual_131_PostRename")
		->args({"self","OldOuter","OldName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:528:15
	makeExtern< void (*)(UObject &,bool) , _dasUnreal_virtual_132_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_132_PostDuplicate")
		->args({"self","bDuplicateForPIE"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:529:15
	makeExtern< void (*)(UObject &,EDuplicateMode::Type) , _dasUnreal_virtual_133_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_133_PostDuplicate")
		->args({"self","DuplicateMode"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:540:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_134_NeedsLoadForClient , SimNode_ExtFuncCall >(lib,"NeedsLoadForClient","_dasUnreal_virtual_134_NeedsLoadForClient")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:548:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_135_NeedsLoadForServer , SimNode_ExtFuncCall >(lib,"NeedsLoadForServer","_dasUnreal_virtual_135_NeedsLoadForServer")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:564:15
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_136_NeedsLoadForEditorGame , SimNode_ExtFuncCall >(lib,"NeedsLoadForEditorGame","_dasUnreal_virtual_136_NeedsLoadForEditorGame")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:574:15
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_137_IsEditorOnly , SimNode_ExtFuncCall >(lib,"IsEditorOnly","_dasUnreal_virtual_137_IsEditorOnly")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:585:15
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_138_HasNonEditorOnlyReferences , SimNode_ExtFuncCall >(lib,"HasNonEditorOnlyReferences","_dasUnreal_virtual_138_HasNonEditorOnlyReferences")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:595:15
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_139_IsPostLoadThreadSafe , SimNode_ExtFuncCall >(lib,"IsPostLoadThreadSafe","_dasUnreal_virtual_139_IsPostLoadThreadSafe")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:605:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_140_IsDestructionThreadSafe , SimNode_ExtFuncCall >(lib,"IsDestructionThreadSafe","_dasUnreal_virtual_140_IsDestructionThreadSafe")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:637:30
	makeExtern< bool (*)(const UObject *,UObject *) , _dasUnreal_static_141_CanCreateInCurrentContext , SimNode_ExtFuncCall >(lib,"CanCreateInCurrentContext","_dasUnreal_static_141_CanCreateInCurrentContext")
		->args({"self","Template"})
		->arg_type(0,makeType<TExplicit<const UObject*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:666:15
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_142_PostEditImport , SimNode_ExtFuncCall >(lib,"PostEditImport","_dasUnreal_virtual_142_PostEditImport")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:671:15
	makeExtern< void (*)(UObject &,FProperty *) , _dasUnreal_virtual_143_PostReloadConfig , SimNode_ExtFuncCall >(lib,"PostReloadConfig","_dasUnreal_virtual_143_PostReloadConfig")
		->args({"self","PropertyThatWasLoaded"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:681:31
	makeExtern< bool (*)(UObject &,const wchar_t *,UObject *,unsigned int) , _dasUnreal_virtual_144_Rename , SimNode_ExtFuncCall >(lib,"Rename","_dasUnreal_virtual_144_Rename")
		->args({"self","NewName","NewOuter","Flags"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->arg_init(2,make_smart<ExprConstPtr>())
		->arg_init(3,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:684:18
	makeExtern< FString (*)(UObject &) , _dasUnreal_virtual_145_GetDesc , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDesc","_dasUnreal_virtual_145_GetDesc")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_393 = das::das_call_member< UScriptStruct * (UObject::*)() const,&UObject::GetSparseClassDataStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:687:33
	makeExtern<DAS_CALL_METHOD(_method_393), SimNode_ExtFuncCall >(lib,"GetSparseClassDataStruct","das_call_member< UScriptStruct * (UObject::*)() const , &UObject::GetSparseClassDataStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:690:15
	makeExtern< void (*)(const UObject &) , _dasUnreal_virtual_146_MoveDataToSparseClassDataStruct , SimNode_ExtFuncCall >(lib,"MoveDataToSparseClassDataStruct","_dasUnreal_virtual_146_MoveDataToSparseClassDataStruct")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:691:15
	makeExtern< bool (*)(const UObject &,UScriptStruct *,void *) , _dasUnreal_virtual_147_ConformSparseClassDataStruct , SimNode_ExtFuncCall >(lib,"ConformSparseClassDataStruct","_dasUnreal_virtual_147_ConformSparseClassDataStruct")
		->args({"self","SourceSparseClassDataStruct","SourceSparseClassData"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:706:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_148_ImplementsGetWorld , SimNode_ExtFuncCall >(lib,"ImplementsGetWorld","_dasUnreal_virtual_148_ImplementsGetWorld")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_397 = das::das_call_member< SIZE_T (UObject::*)(EResourceSizeMode::Type),&UObject::GetResourceSizeBytes >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:743:9
	makeExtern<DAS_CALL_METHOD(_method_397), SimNode_ExtFuncCall >(lib,"GetResourceSizeBytes","das_call_member< SIZE_T (UObject::*)(EResourceSizeMode::Type) , &UObject::GetResourceSizeBytes >::invoke")
		->args({"self","Mode"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:754:16
	makeExtern< FName (*)(UObject &) , _dasUnreal_virtual_149_GetExporterName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetExporterName","_dasUnreal_virtual_149_GetExporterName")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:766:30
	makeExtern< void (*)(const UObject *,UObject *,FReferenceCollector &) , _dasUnreal_static_150_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_static_150_AddReferencedObjects")
		->args({"self","InThis","Collector"})
		->arg_type(0,makeType<TExplicit<const UObject*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_400 = das::das_call_member< void (UObject::*)(FReferenceCollector &),&UObject::CallAddReferencedObjects >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:773:23
	makeExtern<DAS_CALL_METHOD(_method_400), SimNode_ExtFuncCall >(lib,"CallAddReferencedObjects","das_call_member< void (UObject::*)(FReferenceCollector &) , &UObject::CallAddReferencedObjects >::invoke")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:781:39
	makeExtern< FRestoreForUObjectOverwrite * (*)(UObject &) , _dasUnreal_virtual_151_GetRestoreForUObjectOverwrite , SimNode_ExtFuncCall >(lib,"GetRestoreForUObjectOverwrite","_dasUnreal_virtual_151_GetRestoreForUObjectOverwrite")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:793:15
	makeExtern< bool (*)(const UObject &,UObject *) , _dasUnreal_virtual_152_AreNativePropertiesIdenticalTo , SimNode_ExtFuncCall >(lib,"AreNativePropertiesIdenticalTo","_dasUnreal_virtual_152_AreNativePropertiesIdenticalTo")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_403 = das::das_call_member< void (UObject::*)(FAssetData &) const,&UObject::GetAssetRegistryTags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:881:23
	makeExtern<DAS_CALL_METHOD(_method_403), SimNode_ExtFuncCall >(lib,"GetAssetRegistryTags","das_call_member< void (UObject::*)(FAssetData &) const , &UObject::GetAssetRegistryTags >::invoke")
		->args({"self","Out"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:884:38
	makeExtern< const FName & (*)(const UObject *) , _dasUnreal_static_153_SourceFileTagName , SimNode_ExtFuncCallRef >(lib,"SourceFileTagName","_dasUnreal_static_153_SourceFileTagName")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UObject*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:888:38
	makeExtern< const FName & (*)(const UObject *) , _dasUnreal_static_154_AssetVersePathTagName , SimNode_ExtFuncCallRef >(lib,"AssetVersePathTagName","_dasUnreal_static_154_AssetVersePathTagName")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UObject*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_406 = das::das_call_member< UObject::FAssetRegistryTagMetadata & (UObject::FAssetRegistryTagMetadata::*)(const FText &),&UObject::FAssetRegistryTagMetadata::SetDisplayName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:914:30
	makeExtern<DAS_CALL_METHOD(_method_406), SimNode_ExtFuncCallRef >(lib,"SetDisplayName","das_call_member< UObject::FAssetRegistryTagMetadata & (UObject::FAssetRegistryTagMetadata::*)(const FText &) , &UObject::FAssetRegistryTagMetadata::SetDisplayName >::invoke")
		->args({"self","InDisplayName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_407 = das::das_call_member< UObject::FAssetRegistryTagMetadata & (UObject::FAssetRegistryTagMetadata::*)(const FText &),&UObject::FAssetRegistryTagMetadata::SetTooltip >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:921:30
	makeExtern<DAS_CALL_METHOD(_method_407), SimNode_ExtFuncCallRef >(lib,"SetTooltip","das_call_member< UObject::FAssetRegistryTagMetadata & (UObject::FAssetRegistryTagMetadata::*)(const FText &) , &UObject::FAssetRegistryTagMetadata::SetTooltip >::invoke")
		->args({"self","InTooltipText"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_408 = das::das_call_member< UObject::FAssetRegistryTagMetadata & (UObject::FAssetRegistryTagMetadata::*)(const FText &),&UObject::FAssetRegistryTagMetadata::SetSuffix >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:928:30
	makeExtern<DAS_CALL_METHOD(_method_408), SimNode_ExtFuncCallRef >(lib,"SetSuffix","das_call_member< UObject::FAssetRegistryTagMetadata & (UObject::FAssetRegistryTagMetadata::*)(const FText &) , &UObject::FAssetRegistryTagMetadata::SetSuffix >::invoke")
		->args({"self","InSuffix"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_409 = das::das_call_member< UObject::FAssetRegistryTagMetadata & (UObject::FAssetRegistryTagMetadata::*)(const FString &),&UObject::FAssetRegistryTagMetadata::SetImportantValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:935:30
	makeExtern<DAS_CALL_METHOD(_method_409), SimNode_ExtFuncCallRef >(lib,"SetImportantValue","das_call_member< UObject::FAssetRegistryTagMetadata & (UObject::FAssetRegistryTagMetadata::*)(const FString &) , &UObject::FAssetRegistryTagMetadata::SetImportantValue >::invoke")
		->args({"self","InImportantValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:951:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_155_IsAsset , SimNode_ExtFuncCall >(lib,"IsAsset","_dasUnreal_virtual_155_IsAsset")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:958:42
	makeExtern< FPrimaryAssetId (*)(const UObject &) , _dasUnreal_virtual_156_GetPrimaryAssetId , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetPrimaryAssetId","_dasUnreal_virtual_156_GetPrimaryAssetId")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:961:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_157_IsLocalizedResource , SimNode_ExtFuncCall >(lib,"IsLocalizedResource","_dasUnreal_virtual_157_IsLocalizedResource")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:964:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_158_IsSafeForRootSet , SimNode_ExtFuncCall >(lib,"IsSafeForRootSet","_dasUnreal_virtual_158_IsSafeForRootSet")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:971:31
	makeExtern< void (*)(UObject &,EObjectFlags) , _dasUnreal_virtual_159_TagSubobjects , SimNode_ExtFuncCall >(lib,"TagSubobjects","_dasUnreal_virtual_159_TagSubobjects")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:990:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_160_IsNameStableForNetworking , SimNode_ExtFuncCall >(lib,"IsNameStableForNetworking","_dasUnreal_virtual_160_IsNameStableForNetworking")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:993:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_161_IsFullNameStableForNetworking , SimNode_ExtFuncCall >(lib,"IsFullNameStableForNetworking","_dasUnreal_virtual_161_IsFullNameStableForNetworking")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:996:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_162_IsSupportedForNetworking , SimNode_ExtFuncCall >(lib,"IsSupportedForNetworking","_dasUnreal_virtual_162_IsSupportedForNetworking")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1002:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_163_PreNetReceive , SimNode_ExtFuncCall >(lib,"PreNetReceive","_dasUnreal_virtual_163_PreNetReceive")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1005:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_164_PostNetReceive , SimNode_ExtFuncCall >(lib,"PostNetReceive","_dasUnreal_virtual_164_PostNetReceive")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1008:15
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_165_PostRepNotifies , SimNode_ExtFuncCall >(lib,"PostRepNotifies","_dasUnreal_virtual_165_PostRepNotifies")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1011:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_166_PreDestroyFromReplication , SimNode_ExtFuncCall >(lib,"PreDestroyFromReplication","_dasUnreal_virtual_166_PreDestroyFromReplication")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_422 = das::das_call_member< bool (UObject::*)() const,&UObject::IsSelected >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1047:23
	makeExtern<DAS_CALL_METHOD(_method_422), SimNode_ExtFuncCall >(lib,"IsSelected","das_call_member< bool (UObject::*)() const , &UObject::IsSelected >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_423 = das::das_call_member< FString (UObject::*)() const,&UObject::GetDetailedInfo >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1100:26
	makeExtern<DAS_CALL_METHOD(_method_423), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDetailedInfo","das_call_member< FString (UObject::*)() const , &UObject::GetDetailedInfo >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_424 = das::das_call_member< bool (UObject::*)(),&UObject::ConditionalBeginDestroy >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1106:23
	makeExtern<DAS_CALL_METHOD(_method_424), SimNode_ExtFuncCall >(lib,"ConditionalBeginDestroy","das_call_member< bool (UObject::*)() , &UObject::ConditionalBeginDestroy >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_425 = das::das_call_member< bool (UObject::*)(),&UObject::ConditionalFinishDestroy >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1109:23
	makeExtern<DAS_CALL_METHOD(_method_425), SimNode_ExtFuncCall >(lib,"ConditionalFinishDestroy","das_call_member< bool (UObject::*)() , &UObject::ConditionalFinishDestroy >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_426 = das::das_call_member< void (UObject::*)(),&UObject::ConditionalPostLoad >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1112:23
	makeExtern<DAS_CALL_METHOD(_method_426), SimNode_ExtFuncCall >(lib,"ConditionalPostLoad","das_call_member< void (UObject::*)() , &UObject::ConditionalPostLoad >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1143:15
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_167_WillNeverCacheCookedPlatformDataAgain , SimNode_ExtFuncCall >(lib,"WillNeverCacheCookedPlatformDataAgain","_dasUnreal_virtual_167_WillNeverCacheCookedPlatformDataAgain")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1157:15
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_168_ClearAllCachedCookedPlatformData , SimNode_ExtFuncCall >(lib,"ClearAllCachedCookedPlatformData","_dasUnreal_virtual_168_ClearAllCachedCookedPlatformData")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_429 = das::das_call_member< bool (UObject::*)(const UObject *const) const,&UObject::IsBasedOnArchetype >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1194:14
	makeExtern<DAS_CALL_METHOD(_method_429), SimNode_ExtFuncCall >(lib,"IsBasedOnArchetype","das_call_member< bool (UObject::*)(const UObject *const) const , &UObject::IsBasedOnArchetype >::invoke")
		->args({"self","SomeObject"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_430 = das::das_call_member< UFunction * (UObject::*)(FName) const,&UObject::FindFunction >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1197:29
	makeExtern<DAS_CALL_METHOD(_method_430), SimNode_ExtFuncCall >(lib,"FindFunction","das_call_member< UFunction * (UObject::*)(FName) const , &UObject::FindFunction >::invoke")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_431 = das::das_call_member< UFunction * (UObject::*)(FName) const,&UObject::FindFunctionChecked >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1200:29
	makeExtern<DAS_CALL_METHOD(_method_431), SimNode_ExtFuncCall >(lib,"FindFunctionChecked","das_call_member< UFunction * (UObject::*)(FName) const , &UObject::FindFunctionChecked >::invoke")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_432 = das::das_call_member< bool (UObject::*)(bool) const,&UObject::CheckDefaultSubobjects >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1225:23
	makeExtern<DAS_CALL_METHOD(_method_432), SimNode_ExtFuncCall >(lib,"CheckDefaultSubobjects","das_call_member< bool (UObject::*)(bool) const , &UObject::CheckDefaultSubobjects >::invoke")
		->args({"self","bForceCheck"})
		->arg_init(1,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_433 = das::das_call_member< void (UObject::*)(const FString &),&UObject::UpdateDefaultConfigFile >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1237:23
	makeExtern<DAS_CALL_METHOD(_method_433), SimNode_ExtFuncCall >(lib,"UpdateDefaultConfigFile","das_call_member< void (UObject::*)(const FString &) , &UObject::UpdateDefaultConfigFile >::invoke")
		->args({"self","SpecificFileLocation"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_434 = das::das_call_member< bool (UObject::*)(const FString &,bool),&UObject::TryUpdateDefaultConfigFile >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1246:23
	makeExtern<DAS_CALL_METHOD(_method_434), SimNode_ExtFuncCall >(lib,"TryUpdateDefaultConfigFile","das_call_member< bool (UObject::*)(const FString &,bool) , &UObject::TryUpdateDefaultConfigFile >::invoke")
		->args({"self","SpecificFileLocation","bWarnIfFail"})
		->arg_init(2,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_435 = das::das_call_member< void (UObject::*)(),&UObject::UpdateGlobalUserConfigFile >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1251:23
	makeExtern<DAS_CALL_METHOD(_method_435), SimNode_ExtFuncCall >(lib,"UpdateGlobalUserConfigFile","das_call_member< void (UObject::*)() , &UObject::UpdateGlobalUserConfigFile >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_436 = das::das_call_member< void (UObject::*)(),&UObject::UpdateProjectUserConfigFile >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1256:23
	makeExtern<DAS_CALL_METHOD(_method_436), SimNode_ExtFuncCall >(lib,"UpdateProjectUserConfigFile","das_call_member< void (UObject::*)() , &UObject::UpdateProjectUserConfigFile >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_437 = das::das_call_member< FString (UObject::*)() const,&UObject::GetDefaultConfigFilename >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1279:26
	makeExtern<DAS_CALL_METHOD(_method_437), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDefaultConfigFilename","das_call_member< FString (UObject::*)() const , &UObject::GetDefaultConfigFilename >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_438 = das::das_call_member< FString (UObject::*)() const,&UObject::GetGlobalUserConfigFilename >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1284:26
	makeExtern<DAS_CALL_METHOD(_method_438), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetGlobalUserConfigFilename","das_call_member< FString (UObject::*)() const , &UObject::GetGlobalUserConfigFilename >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

