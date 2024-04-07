// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_7() {
	using _method_450 = das::das_call_member< FProperty * (UStruct::*)(FName) const,&UStruct::FindPropertyByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:474:29
	makeExtern<DAS_CALL_METHOD(_method_450), SimNode_ExtFuncCall >(lib,"FindPropertyByName","das_call_member< FProperty * (UStruct::*)(FName) const , &UStruct::FindPropertyByName >::invoke")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:488:19
	makeExtern< UStruct * (*)(const UStruct &) , _dasUnreal_virtual_142_GetInheritanceSuper , SimNode_ExtFuncCall >(lib,"GetInheritanceSuper","_dasUnreal_virtual_142_GetInheritanceSuper")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_452 = das::das_call_member< void (UStruct::*)(bool),&UStruct::StaticLink >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:491:23
	makeExtern<DAS_CALL_METHOD(_method_452), SimNode_ExtFuncCall >(lib,"StaticLink","das_call_member< void (UStruct::*)(bool) , &UStruct::StaticLink >::invoke")
		->args({"self","bRelinkExistingProperties"})
		->arg_init(1,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:562:31
	makeExtern< void (*)(const UStruct &,void *,int) , _dasUnreal_virtual_143_InitializeStruct , SimNode_ExtFuncCall >(lib,"InitializeStruct","_dasUnreal_virtual_143_InitializeStruct")
		->args({"self","Dest","ArrayDim"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:571:31
	makeExtern< void (*)(const UStruct &,void *,int) , _dasUnreal_virtual_144_DestroyStruct , SimNode_ExtFuncCall >(lib,"DestroyStruct","_dasUnreal_virtual_144_DestroyStruct")
		->args({"self","Dest","ArrayDim"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:574:21
	makeExtern< FProperty * (*)(const UStruct &,const FName) , _dasUnreal_virtual_145_CustomFindProperty , SimNode_ExtFuncCall >(lib,"CustomFindProperty","_dasUnreal_virtual_145_CustomFindProperty")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:584:23
	makeExtern< const wchar_t * (*)(const UStruct &) , _dasUnreal_virtual_146_GetPrefixCPP , SimNode_ExtFuncCall >(lib,"GetPrefixCPP","_dasUnreal_virtual_146_GetPrefixCPP")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_457 = das::das_call_member< int (UStruct::*)() const,&UStruct::GetPropertiesSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:587:20
	makeExtern<DAS_CALL_METHOD(_method_457), SimNode_ExtFuncCall >(lib,"GetPropertiesSize","das_call_member< int (UStruct::*)() const , &UStruct::GetPropertiesSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_458 = das::das_call_member< int (UStruct::*)() const,&UStruct::GetMinAlignment >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:593:20
	makeExtern<DAS_CALL_METHOD(_method_458), SimNode_ExtFuncCall >(lib,"GetMinAlignment","das_call_member< int (UStruct::*)() const , &UStruct::GetMinAlignment >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_459 = das::das_call_member< int (UStruct::*)() const,&UStruct::GetStructureSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:599:20
	makeExtern<DAS_CALL_METHOD(_method_459), SimNode_ExtFuncCall >(lib,"GetStructureSize","das_call_member< int (UStruct::*)() const , &UStruct::GetStructureSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_460 = das::das_call_member< void (UStruct::*)(int),&UStruct::SetPropertiesSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:605:7
	makeExtern<DAS_CALL_METHOD(_method_460), SimNode_ExtFuncCall >(lib,"SetPropertiesSize","das_call_member< void (UStruct::*)(int) , &UStruct::SetPropertiesSize >::invoke")
		->args({"self","NewSize"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_461 = das::das_call_member< bool (UStruct::*)(const UStruct *) const,&UStruct::IsChildOf >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:619:23
	makeExtern<DAS_CALL_METHOD(_method_461), SimNode_ExtFuncCall >(lib,"IsChildOf","das_call_member< bool (UStruct::*)(const UStruct *) const , &UStruct::IsChildOf >::invoke")
		->args({"self","SomeBase"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_462 = das::das_call_member< UStruct * (UStruct::*)() const,&UStruct::GetSuperStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:628:11
	makeExtern<DAS_CALL_METHOD(_method_462), SimNode_ExtFuncCall >(lib,"GetSuperStruct","das_call_member< UStruct * (UStruct::*)() const , &UStruct::GetSuperStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:637:31
	makeExtern< void (*)(UStruct &,UStruct *) , _dasUnreal_virtual_147_SetSuperStruct , SimNode_ExtFuncCall >(lib,"SetSuperStruct","_dasUnreal_virtual_147_SetSuperStruct")
		->args({"self","NewSuperStruct"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:640:34
	makeExtern< FString (*)(const UStruct &,const UField *) , _dasUnreal_virtual_148_GetAuthoredNameForField , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredNameForField","_dasUnreal_virtual_148_GetAuthoredNameForField")
		->args({"self","Field"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:643:34
	makeExtern< FString (*)(const UStruct &,const FField *) , _dasUnreal_virtual_149_GetAuthoredNameForField , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredNameForField","_dasUnreal_virtual_149_GetAuthoredNameForField")
		->args({"self","Field"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:646:15
	makeExtern< bool (*)(const UStruct &) , _dasUnreal_virtual_150_IsStructTrashed , SimNode_ExtFuncCall >(lib,"IsStructTrashed","_dasUnreal_virtual_150_IsStructTrashed")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_467 = das::das_call_member< void (UStruct::*)(),&UStruct::DestroyChildPropertiesAndResetPropertyLinks >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:652:23
	makeExtern<DAS_CALL_METHOD(_method_467), SimNode_ExtFuncCall >(lib,"DestroyChildPropertiesAndResetPropertyLinks","das_call_member< void (UStruct::*)() , &UStruct::DestroyChildPropertiesAndResetPropertyLinks >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_468 = das::das_call_member< bool (UStruct::*)(const FName &) const,&UStruct::GetBoolMetaDataHierarchical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:656:23
	makeExtern<DAS_CALL_METHOD(_method_468), SimNode_ExtFuncCall >(lib,"GetBoolMetaDataHierarchical","das_call_member< bool (UStruct::*)(const FName &) const , &UStruct::GetBoolMetaDataHierarchical >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_469 = das::das_call_member< bool (UStruct::*)(const FName &,FString *) const,&UStruct::GetStringMetaDataHierarchical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:659:23
	makeExtern<DAS_CALL_METHOD(_method_469), SimNode_ExtFuncCall >(lib,"GetStringMetaDataHierarchical","das_call_member< bool (UStruct::*)(const FName &,FString *) const , &UStruct::GetStringMetaDataHierarchical >::invoke")
		->args({"self","Key","OutValue"})
		->arg_init(2,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_470 = das::das_call_member< const UStruct * (UStruct::*)(const FName &) const,&UStruct::HasMetaDataHierarchical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:667:33
	makeExtern<DAS_CALL_METHOD(_method_470), SimNode_ExtFuncCall >(lib,"HasMetaDataHierarchical","das_call_member< const UStruct * (UStruct::*)(const FName &) const , &UStruct::HasMetaDataHierarchical >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_471 = das::das_call_member< bool (UStruct::*)() const,&UStruct::HasAssetRegistrySearchableProperties >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:670:19
	makeExtern<DAS_CALL_METHOD(_method_471), SimNode_ExtFuncCall >(lib,"HasAssetRegistrySearchableProperties","das_call_member< bool (UStruct::*)() const , &UStruct::HasAssetRegistrySearchableProperties >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_472 = das::das_call_member< void (UStruct::*)(),&UStruct::DeleteUnresolvedScriptProperties >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:687:19
	makeExtern<DAS_CALL_METHOD(_method_472), SimNode_ExtFuncCall >(lib,"DeleteUnresolvedScriptProperties","das_call_member< void (UStruct::*)() , &UStruct::DeleteUnresolvedScriptProperties >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_473 = das::das_call_member< void (UStruct::*)(),&UStruct::CollectBytecodeAndPropertyReferencedObjects >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:709:23
	makeExtern<DAS_CALL_METHOD(_method_473), SimNode_ExtFuncCall >(lib,"CollectBytecodeAndPropertyReferencedObjects","das_call_member< void (UStruct::*)() , &UStruct::CollectBytecodeAndPropertyReferencedObjects >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_474 = das::das_call_member< void (UStruct::*)(),&UStruct::CollectBytecodeAndPropertyReferencedObjectsRecursively >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:713:23
	makeExtern<DAS_CALL_METHOD(_method_474), SimNode_ExtFuncCall >(lib,"CollectBytecodeAndPropertyReferencedObjectsRecursively","das_call_member< void (UStruct::*)() , &UStruct::CollectBytecodeAndPropertyReferencedObjectsRecursively >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_475 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasNoopConstructor >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:951:8
	makeExtern<DAS_CALL_METHOD(_method_475), SimNode_ExtFuncCall >(lib,"HasNoopConstructor","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasNoopConstructor >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_476 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasZeroConstructor >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:956:8
	makeExtern<DAS_CALL_METHOD(_method_476), SimNode_ExtFuncCall >(lib,"HasZeroConstructor","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasZeroConstructor >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_477 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasDestructor >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:965:8
	makeExtern<DAS_CALL_METHOD(_method_477), SimNode_ExtFuncCall >(lib,"HasDestructor","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasDestructor >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_478 = das::das_call_member< int (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::GetSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:972:21
	makeExtern<DAS_CALL_METHOD(_method_478), SimNode_ExtFuncCall >(lib,"GetSize","das_call_member< int (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::GetSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_479 = das::das_call_member< int (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::GetAlignment >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:977:21
	makeExtern<DAS_CALL_METHOD(_method_479), SimNode_ExtFuncCall >(lib,"GetAlignment","das_call_member< int (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::GetAlignment >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_480 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasSerializer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:983:8
	makeExtern<DAS_CALL_METHOD(_method_480), SimNode_ExtFuncCall >(lib,"HasSerializer","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasSerializer >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_481 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasStructuredSerializer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:988:8
	makeExtern<DAS_CALL_METHOD(_method_481), SimNode_ExtFuncCall >(lib,"HasStructuredSerializer","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasStructuredSerializer >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_482 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasPostSerialize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1000:8
	makeExtern<DAS_CALL_METHOD(_method_482), SimNode_ExtFuncCall >(lib,"HasPostSerialize","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasPostSerialize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_483 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasNetSerializer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1008:8
	makeExtern<DAS_CALL_METHOD(_method_483), SimNode_ExtFuncCall >(lib,"HasNetSerializer","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasNetSerializer >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_484 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasNetSharedSerialization >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1014:8
	makeExtern<DAS_CALL_METHOD(_method_484), SimNode_ExtFuncCall >(lib,"HasNetSharedSerialization","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasNetSharedSerialization >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_485 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasNetDeltaSerializer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1025:8
	makeExtern<DAS_CALL_METHOD(_method_485), SimNode_ExtFuncCall >(lib,"HasNetDeltaSerializer","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasNetDeltaSerializer >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_486 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasPostScriptConstruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1036:8
	makeExtern<DAS_CALL_METHOD(_method_486), SimNode_ExtFuncCall >(lib,"HasPostScriptConstruct","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasPostScriptConstruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_487 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::IsPlainOldData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1047:8
	makeExtern<DAS_CALL_METHOD(_method_487), SimNode_ExtFuncCall >(lib,"IsPlainOldData","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::IsPlainOldData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_488 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::IsUECoreType >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1053:8
	makeExtern<DAS_CALL_METHOD(_method_488), SimNode_ExtFuncCall >(lib,"IsUECoreType","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::IsUECoreType >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_489 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::IsUECoreVariant >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1059:8
	makeExtern<DAS_CALL_METHOD(_method_489), SimNode_ExtFuncCall >(lib,"IsUECoreVariant","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::IsUECoreVariant >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_490 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasCopy >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1065:8
	makeExtern<DAS_CALL_METHOD(_method_490), SimNode_ExtFuncCall >(lib,"HasCopy","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasCopy >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_491 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasIdentical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1076:8
	makeExtern<DAS_CALL_METHOD(_method_491), SimNode_ExtFuncCall >(lib,"HasIdentical","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasIdentical >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_492 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasExportTextItem >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1087:8
	makeExtern<DAS_CALL_METHOD(_method_492), SimNode_ExtFuncCall >(lib,"HasExportTextItem","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasExportTextItem >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_493 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasImportTextItem >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1098:8
	makeExtern<DAS_CALL_METHOD(_method_493), SimNode_ExtFuncCall >(lib,"HasImportTextItem","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasImportTextItem >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_494 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasFindInnerPropertyInstance >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1108:8
	makeExtern<DAS_CALL_METHOD(_method_494), SimNode_ExtFuncCall >(lib,"HasFindInnerPropertyInstance","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasFindInnerPropertyInstance >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_495 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasAddStructReferencedObjects >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1115:8
	makeExtern<DAS_CALL_METHOD(_method_495), SimNode_ExtFuncCall >(lib,"HasAddStructReferencedObjects","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasAddStructReferencedObjects >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_496 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)(EPropertyObjectReferenceType) const,&UScriptStruct::ICppStructOps::HasSerializerObjectReferences >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1120:8
	makeExtern<DAS_CALL_METHOD(_method_496), SimNode_ExtFuncCall >(lib,"HasSerializerObjectReferences","das_call_member< bool (UScriptStruct::ICppStructOps::*)(EPropertyObjectReferenceType) const , &UScriptStruct::ICppStructOps::HasSerializerObjectReferences >::invoke")
		->args({"self","Type"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_497 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasSerializeFromMismatchedTag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1132:8
	makeExtern<DAS_CALL_METHOD(_method_497), SimNode_ExtFuncCall >(lib,"HasSerializeFromMismatchedTag","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasSerializeFromMismatchedTag >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_498 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasStructuredSerializeFromMismatchedTag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1136:8
	makeExtern<DAS_CALL_METHOD(_method_498), SimNode_ExtFuncCall >(lib,"HasStructuredSerializeFromMismatchedTag","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasStructuredSerializeFromMismatchedTag >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_499 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasGetTypeHash >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1149:8
	makeExtern<DAS_CALL_METHOD(_method_499), SimNode_ExtFuncCall >(lib,"HasGetTypeHash","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasGetTypeHash >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_500 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::IsAbstract >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1164:8
	makeExtern<DAS_CALL_METHOD(_method_500), SimNode_ExtFuncCall >(lib,"IsAbstract","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::IsAbstract >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_501 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasCanEditChange >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1171:8
	makeExtern<DAS_CALL_METHOD(_method_501), SimNode_ExtFuncCall >(lib,"HasCanEditChange","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasCanEditChange >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1541:2
	makeExtern< UClass * (*)(const UScriptStruct *) , _dasUnreal_static_151_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_151_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UScriptStruct*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1541:2
	makeExtern< const wchar_t * (*)(const UScriptStruct *) , _dasUnreal_static_152_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_152_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UScriptStruct*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1541:2
	makeExtern< void (*)(const UScriptStruct *) , _dasUnreal_static_153_StaticRegisterNativesUScriptStruct , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUScriptStruct","_dasUnreal_static_153_StaticRegisterNativesUScriptStruct")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UScriptStruct*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1571:31
	makeExtern< void (*)(const UScriptStruct &,void *,int) , _dasUnreal_virtual_154_InitializeStruct , SimNode_ExtFuncCall >(lib,"InitializeStruct","_dasUnreal_virtual_154_InitializeStruct")
		->args({"self","Dest","ArrayDim"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1572:31
	makeExtern< void (*)(const UScriptStruct &,void *,int) , _dasUnreal_virtual_155_DestroyStruct , SimNode_ExtFuncCall >(lib,"DestroyStruct","_dasUnreal_virtual_155_DestroyStruct")
		->args({"self","Dest","ArrayDim"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1573:31
	makeExtern< bool (*)(const UScriptStruct &) , _dasUnreal_virtual_156_IsStructTrashed , SimNode_ExtFuncCall >(lib,"IsStructTrashed","_dasUnreal_virtual_156_IsStructTrashed")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_508 = das::das_call_member< void (UScriptStruct::*)(bool),&UScriptStruct::SetStructTrashed >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1577:23
	makeExtern<DAS_CALL_METHOD(_method_508), SimNode_ExtFuncCall >(lib,"SetStructTrashed","das_call_member< void (UScriptStruct::*)(bool) , &UScriptStruct::SetStructTrashed >::invoke")
		->args({"self","bIsTrash"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1600:31
	makeExtern< void (*)(UScriptStruct &) , _dasUnreal_virtual_157_PrepareCppStructOps , SimNode_ExtFuncCall >(lib,"PrepareCppStructOps","_dasUnreal_virtual_157_PrepareCppStructOps")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_510 = das::das_call_member< UScriptStruct::ICppStructOps * (UScriptStruct::*)() const,&UScriptStruct::GetCppStructOps >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1603:29
	makeExtern<DAS_CALL_METHOD(_method_510), SimNode_ExtFuncCall >(lib,"GetCppStructOps","das_call_member< UScriptStruct::ICppStructOps * (UScriptStruct::*)() const , &UScriptStruct::GetCppStructOps >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_511 = das::das_call_member< void (UScriptStruct::*)(),&UScriptStruct::ClearCppStructOps >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1610:7
	makeExtern<DAS_CALL_METHOD(_method_511), SimNode_ExtFuncCall >(lib,"ClearCppStructOps","das_call_member< void (UScriptStruct::*)() , &UScriptStruct::ClearCppStructOps >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_512 = das::das_call_member< bool (UScriptStruct::*)() const,&UScriptStruct::HasDefaults >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1621:19
	makeExtern<DAS_CALL_METHOD(_method_512), SimNode_ExtFuncCall >(lib,"HasDefaults","das_call_member< bool (UScriptStruct::*)() const , &UScriptStruct::HasDefaults >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_513 = das::das_call_member< bool (UScriptStruct::*)() const,&UScriptStruct::UseNativeSerialization >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1643:7
	makeExtern<DAS_CALL_METHOD(_method_513), SimNode_ExtFuncCall >(lib,"UseNativeSerialization","das_call_member< bool (UScriptStruct::*)() const , &UScriptStruct::UseNativeSerialization >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_514 = das::das_call_member< void (UScriptStruct::*)(FString &,const void *,const void *,UObject *,int,UObject *,bool) const,&UScriptStruct::ExportText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1679:23
	makeExtern<DAS_CALL_METHOD(_method_514), SimNode_ExtFuncCall >(lib,"ExportText","das_call_member< void (UScriptStruct::*)(FString &,const void *,const void *,UObject *,int,UObject *,bool) const , &UScriptStruct::ExportText >::invoke")
		->args({"self","ValueStr","Value","Defaults","OwnerObject","PortFlags","ExportRootScope","bAllowNativeOverride"})
		->arg_init(7,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_515 = das::das_call_member< bool (UScriptStruct::*)(const void *,const void *,unsigned int) const,&UScriptStruct::CompareScriptStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1717:23
	makeExtern<DAS_CALL_METHOD(_method_515), SimNode_ExtFuncCall >(lib,"CompareScriptStruct","das_call_member< bool (UScriptStruct::*)(const void *,const void *,unsigned int) const , &UScriptStruct::CompareScriptStruct >::invoke")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_516 = das::das_call_member< void (UScriptStruct::*)(void *,const void *,int) const,&UScriptStruct::CopyScriptStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1727:23
	makeExtern<DAS_CALL_METHOD(_method_516), SimNode_ExtFuncCall >(lib,"CopyScriptStruct","das_call_member< void (UScriptStruct::*)(void *,const void *,int) const , &UScriptStruct::CopyScriptStruct >::invoke")
		->args({"self","Dest","Src","ArrayDim"})
		->arg_init(3,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_517 = das::das_call_member< void (UScriptStruct::*)(void *,int) const,&UScriptStruct::ClearScriptStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1736:23
	makeExtern<DAS_CALL_METHOD(_method_517), SimNode_ExtFuncCall >(lib,"ClearScriptStruct","das_call_member< void (UScriptStruct::*)(void *,int) const , &UScriptStruct::ClearScriptStruct >::invoke")
		->args({"self","Dest","ArrayDim"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1744:33
	makeExtern< unsigned int (*)(const UScriptStruct &,const void *) , _dasUnreal_virtual_158_GetStructTypeHash , SimNode_ExtFuncCall >(lib,"GetStructTypeHash","_dasUnreal_virtual_158_GetStructTypeHash")
		->args({"self","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1747:31
	makeExtern< void (*)(UScriptStruct &) , _dasUnreal_virtual_159_RecursivelyPreload , SimNode_ExtFuncCall >(lib,"RecursivelyPreload","_dasUnreal_virtual_159_RecursivelyPreload")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1753:34
	makeExtern< FString (*)(const UScriptStruct &,unsigned int) , _dasUnreal_virtual_160_GetStructCPPName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetStructCPPName","_dasUnreal_virtual_160_GetStructCPPName")
		->args({"self","CPPExportFlags"})
		->arg_init(1,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1759:31
	makeExtern< void (*)(const UScriptStruct &,unsigned char *) , _dasUnreal_virtual_161_InitializeDefaultValue , SimNode_ExtFuncCall >(lib,"InitializeDefaultValue","_dasUnreal_virtual_161_InitializeDefaultValue")
		->args({"self","InStructData"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1782:2
	makeExtern< UClass * (*)(const UFunction *) , _dasUnreal_static_162_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_162_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UFunction*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1782:2
	makeExtern< const wchar_t * (*)(const UFunction *) , _dasUnreal_static_163_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_163_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UFunction*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1782:2
	makeExtern< void (*)(const UFunction *) , _dasUnreal_static_164_StaticRegisterNativesUFunction , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUFunction","_dasUnreal_static_164_StaticRegisterNativesUFunction")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UFunction*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_525 = das::das_call_member< UClass * (UFunction::*)() const,&UFunction::GetOuterUClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1783:2
	makeExtern<DAS_CALL_METHOD(_method_525), SimNode_ExtFuncCall >(lib,"GetOuterUClass","das_call_member< UClass * (UFunction::*)() const , &UFunction::GetOuterUClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_526 = das::das_call_member< void (UFunction::*)(),&UFunction::InitializeDerivedMembers >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1858:23
	makeExtern<DAS_CALL_METHOD(_method_526), SimNode_ExtFuncCall >(lib,"InitializeDerivedMembers","das_call_member< void (UFunction::*)() , &UFunction::InitializeDerivedMembers >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1862:31
	makeExtern< void (*)(UFunction &) , _dasUnreal_virtual_165_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_165_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1865:31
	makeExtern< void (*)(UFunction &) , _dasUnreal_virtual_166_Bind , SimNode_ExtFuncCall >(lib,"Bind","_dasUnreal_virtual_166_Bind")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1868:19
	makeExtern< UStruct * (*)(const UFunction &) , _dasUnreal_virtual_167_GetInheritanceSuper , SimNode_ExtFuncCall >(lib,"GetInheritanceSuper","_dasUnreal_virtual_167_GetInheritanceSuper")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_530 = das::das_call_member< UFunction * (UFunction::*)() const,&UFunction::GetSuperFunction >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1872:29
	makeExtern<DAS_CALL_METHOD(_method_530), SimNode_ExtFuncCall >(lib,"GetSuperFunction","das_call_member< UFunction * (UFunction::*)() const , &UFunction::GetSuperFunction >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_531 = das::das_call_member< FProperty * (UFunction::*)() const,&UFunction::GetReturnProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1875:29
	makeExtern<DAS_CALL_METHOD(_method_531), SimNode_ExtFuncCall >(lib,"GetReturnProperty","das_call_member< FProperty * (UFunction::*)() const , &UFunction::GetReturnProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_532 = das::das_call_member< UClass * (UFunction::*)() const,&UFunction::GetOuterUClassUnchecked >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1878:22
	makeExtern<DAS_CALL_METHOD(_method_532), SimNode_ExtFuncCall >(lib,"GetOuterUClassUnchecked","das_call_member< UClass * (UFunction::*)() const , &UFunction::GetOuterUClassUnchecked >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1911:28
	makeExtern< uint64 (*)(const UFunction *) , _dasUnreal_static_168_GetDefaultIgnoredSignatureCompatibilityFlags , SimNode_ExtFuncCall >(lib,"GetDefaultIgnoredSignatureCompatibilityFlags","_dasUnreal_static_168_GetDefaultIgnoredSignatureCompatibilityFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UFunction*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_534 = das::das_call_member< bool (UFunction::*)(const UFunction *) const,&UFunction::IsSignatureCompatibleWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1929:23
	makeExtern<DAS_CALL_METHOD(_method_534), SimNode_ExtFuncCall >(lib,"IsSignatureCompatibleWith","das_call_member< bool (UFunction::*)(const UFunction *) const , &UFunction::IsSignatureCompatibleWith >::invoke")
		->args({"self","OtherFunction"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_535 = das::das_call_member< bool (UFunction::*)(const UFunction *,uint64) const,&UFunction::IsSignatureCompatibleWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1941:23
	makeExtern<DAS_CALL_METHOD(_method_535), SimNode_ExtFuncCall >(lib,"IsSignatureCompatibleWith","das_call_member< bool (UFunction::*)(const UFunction *,uint64) const , &UFunction::IsSignatureCompatibleWith >::invoke")
		->args({"self","OtherFunction","IgnoreFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2001:2
	makeExtern< UClass * (*)(const UEnum *) , _dasUnreal_static_169_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_169_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UEnum*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2001:2
	makeExtern< const wchar_t * (*)(const UEnum *) , _dasUnreal_static_170_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_170_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UEnum*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2001:2
	makeExtern< void (*)(const UEnum *) , _dasUnreal_static_171_StaticRegisterNativesUEnum , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUEnum","_dasUnreal_static_171_StaticRegisterNativesUEnum")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UEnum*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_539 = das::das_call_member< int (UEnum::*)(int64) const,&UEnum::GetIndexByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2022:20
	makeExtern<DAS_CALL_METHOD(_method_539), SimNode_ExtFuncCall >(lib,"GetIndexByValue","das_call_member< int (UEnum::*)(int64) const , &UEnum::GetIndexByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_540 = das::das_call_member< int64 (UEnum::*)(int) const,&UEnum::GetValueByIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2035:20
	makeExtern<DAS_CALL_METHOD(_method_540), SimNode_ExtFuncCall >(lib,"GetValueByIndex","das_call_member< int64 (UEnum::*)(int) const , &UEnum::GetValueByIndex >::invoke")
		->args({"self","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_541 = das::das_call_member< FName (UEnum::*)(int) const,&UEnum::GetNameByIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2042:24
	makeExtern<DAS_CALL_METHOD(_method_541), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameByIndex","das_call_member< FName (UEnum::*)(int) const , &UEnum::GetNameByIndex >::invoke")
		->args({"self","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_542 = das::das_call_member< int (UEnum::*)(FName,EGetByNameFlags) const,&UEnum::GetIndexByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2045:24
	makeExtern<DAS_CALL_METHOD(_method_542), SimNode_ExtFuncCall >(lib,"GetIndexByName","das_call_member< int (UEnum::*)(FName,EGetByNameFlags) const , &UEnum::GetIndexByName >::invoke")
		->args({"self","InName","Flags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EGetByNameFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_543 = das::das_call_member< FName (UEnum::*)(int64) const,&UEnum::GetNameByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2048:24
	makeExtern<DAS_CALL_METHOD(_method_543), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameByValue","das_call_member< FName (UEnum::*)(int64) const , &UEnum::GetNameByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_544 = das::das_call_member< int64 (UEnum::*)(FName,EGetByNameFlags) const,&UEnum::GetValueByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2051:24
	makeExtern<DAS_CALL_METHOD(_method_544), SimNode_ExtFuncCall >(lib,"GetValueByName","das_call_member< int64 (UEnum::*)(FName,EGetByNameFlags) const , &UEnum::GetValueByName >::invoke")
		->args({"self","InName","Flags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EGetByNameFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_545 = das::das_call_member< FString (UEnum::*)(int) const,&UEnum::GetNameStringByIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2054:26
	makeExtern<DAS_CALL_METHOD(_method_545), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameStringByIndex","das_call_member< FString (UEnum::*)(int) const , &UEnum::GetNameStringByIndex >::invoke")
		->args({"self","InIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_546 = das::das_call_member< int (UEnum::*)(const FString &,EGetByNameFlags) const,&UEnum::GetIndexByNameString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2057:24
	makeExtern<DAS_CALL_METHOD(_method_546), SimNode_ExtFuncCall >(lib,"GetIndexByNameString","das_call_member< int (UEnum::*)(const FString &,EGetByNameFlags) const , &UEnum::GetIndexByNameString >::invoke")
		->args({"self","SearchString","Flags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EGetByNameFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_547 = das::das_call_member< FString (UEnum::*)(int64) const,&UEnum::GetNameStringByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2060:26
	makeExtern<DAS_CALL_METHOD(_method_547), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameStringByValue","das_call_member< FString (UEnum::*)(int64) const , &UEnum::GetNameStringByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_548 = das::das_call_member< FString (UEnum::*)(int64) const,&UEnum::GetValueOrBitfieldAsString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2063:26
	makeExtern<DAS_CALL_METHOD(_method_548), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetValueOrBitfieldAsString","das_call_member< FString (UEnum::*)(int64) const , &UEnum::GetValueOrBitfieldAsString >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_549 = das::das_call_member< bool (UEnum::*)(FString &,int64) const,&UEnum::FindNameStringByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2066:23
	makeExtern<DAS_CALL_METHOD(_method_549), SimNode_ExtFuncCall >(lib,"FindNameStringByValue","das_call_member< bool (UEnum::*)(FString &,int64) const , &UEnum::FindNameStringByValue >::invoke")
		->args({"self","Out","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

