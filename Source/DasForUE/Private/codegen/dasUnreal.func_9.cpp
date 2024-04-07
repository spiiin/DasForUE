// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_9() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:640:34
	makeExtern< FString (*)(const UStruct &,const UField *) , _dasUnreal_virtual_223_GetAuthoredNameForField , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredNameForField","_dasUnreal_virtual_223_GetAuthoredNameForField")
		->args({"self","Field"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:643:34
	makeExtern< FString (*)(const UStruct &,const FField *) , _dasUnreal_virtual_224_GetAuthoredNameForField , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredNameForField","_dasUnreal_virtual_224_GetAuthoredNameForField")
		->args({"self","Field"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:646:15
	makeExtern< bool (*)(const UStruct &) , _dasUnreal_virtual_225_IsStructTrashed , SimNode_ExtFuncCall >(lib,"IsStructTrashed","_dasUnreal_virtual_225_IsStructTrashed")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_631 = das::das_call_member< void (UStruct::*)(),&UStruct::DestroyChildPropertiesAndResetPropertyLinks >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:652:23
	makeExtern<DAS_CALL_METHOD(_method_631), SimNode_ExtFuncCall >(lib,"DestroyChildPropertiesAndResetPropertyLinks","das_call_member< void (UStruct::*)() , &UStruct::DestroyChildPropertiesAndResetPropertyLinks >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_632 = das::das_call_member< bool (UStruct::*)(const FName &) const,&UStruct::GetBoolMetaDataHierarchical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:656:23
	makeExtern<DAS_CALL_METHOD(_method_632), SimNode_ExtFuncCall >(lib,"GetBoolMetaDataHierarchical","das_call_member< bool (UStruct::*)(const FName &) const , &UStruct::GetBoolMetaDataHierarchical >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_633 = das::das_call_member< bool (UStruct::*)(const FName &,FString *) const,&UStruct::GetStringMetaDataHierarchical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:659:23
	makeExtern<DAS_CALL_METHOD(_method_633), SimNode_ExtFuncCall >(lib,"GetStringMetaDataHierarchical","das_call_member< bool (UStruct::*)(const FName &,FString *) const , &UStruct::GetStringMetaDataHierarchical >::invoke")
		->args({"self","Key","OutValue"})
		->arg_init(2,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_634 = das::das_call_member< const UStruct * (UStruct::*)(const FName &) const,&UStruct::HasMetaDataHierarchical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:667:33
	makeExtern<DAS_CALL_METHOD(_method_634), SimNode_ExtFuncCall >(lib,"HasMetaDataHierarchical","das_call_member< const UStruct * (UStruct::*)(const FName &) const , &UStruct::HasMetaDataHierarchical >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_635 = das::das_call_member< bool (UStruct::*)() const,&UStruct::HasAssetRegistrySearchableProperties >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:670:19
	makeExtern<DAS_CALL_METHOD(_method_635), SimNode_ExtFuncCall >(lib,"HasAssetRegistrySearchableProperties","das_call_member< bool (UStruct::*)() const , &UStruct::HasAssetRegistrySearchableProperties >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_636 = das::das_call_member< void (UStruct::*)(),&UStruct::DeleteUnresolvedScriptProperties >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:687:19
	makeExtern<DAS_CALL_METHOD(_method_636), SimNode_ExtFuncCall >(lib,"DeleteUnresolvedScriptProperties","das_call_member< void (UStruct::*)() , &UStruct::DeleteUnresolvedScriptProperties >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_637 = das::das_call_member< void (UStruct::*)(),&UStruct::CollectBytecodeAndPropertyReferencedObjects >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:709:23
	makeExtern<DAS_CALL_METHOD(_method_637), SimNode_ExtFuncCall >(lib,"CollectBytecodeAndPropertyReferencedObjects","das_call_member< void (UStruct::*)() , &UStruct::CollectBytecodeAndPropertyReferencedObjects >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_638 = das::das_call_member< void (UStruct::*)(),&UStruct::CollectBytecodeAndPropertyReferencedObjectsRecursively >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:713:23
	makeExtern<DAS_CALL_METHOD(_method_638), SimNode_ExtFuncCall >(lib,"CollectBytecodeAndPropertyReferencedObjectsRecursively","das_call_member< void (UStruct::*)() , &UStruct::CollectBytecodeAndPropertyReferencedObjectsRecursively >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_639 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasNoopConstructor >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:951:8
	makeExtern<DAS_CALL_METHOD(_method_639), SimNode_ExtFuncCall >(lib,"HasNoopConstructor","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasNoopConstructor >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_640 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasZeroConstructor >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:956:8
	makeExtern<DAS_CALL_METHOD(_method_640), SimNode_ExtFuncCall >(lib,"HasZeroConstructor","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasZeroConstructor >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_641 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasDestructor >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:965:8
	makeExtern<DAS_CALL_METHOD(_method_641), SimNode_ExtFuncCall >(lib,"HasDestructor","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasDestructor >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_642 = das::das_call_member< int (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::GetSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:972:21
	makeExtern<DAS_CALL_METHOD(_method_642), SimNode_ExtFuncCall >(lib,"GetSize","das_call_member< int (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::GetSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_643 = das::das_call_member< int (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::GetAlignment >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:977:21
	makeExtern<DAS_CALL_METHOD(_method_643), SimNode_ExtFuncCall >(lib,"GetAlignment","das_call_member< int (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::GetAlignment >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_644 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasSerializer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:983:8
	makeExtern<DAS_CALL_METHOD(_method_644), SimNode_ExtFuncCall >(lib,"HasSerializer","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasSerializer >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_645 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasStructuredSerializer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:988:8
	makeExtern<DAS_CALL_METHOD(_method_645), SimNode_ExtFuncCall >(lib,"HasStructuredSerializer","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasStructuredSerializer >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_646 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasPostSerialize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1000:8
	makeExtern<DAS_CALL_METHOD(_method_646), SimNode_ExtFuncCall >(lib,"HasPostSerialize","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasPostSerialize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_647 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasNetSerializer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1008:8
	makeExtern<DAS_CALL_METHOD(_method_647), SimNode_ExtFuncCall >(lib,"HasNetSerializer","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasNetSerializer >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_648 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasNetSharedSerialization >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1014:8
	makeExtern<DAS_CALL_METHOD(_method_648), SimNode_ExtFuncCall >(lib,"HasNetSharedSerialization","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasNetSharedSerialization >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_649 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasNetDeltaSerializer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1025:8
	makeExtern<DAS_CALL_METHOD(_method_649), SimNode_ExtFuncCall >(lib,"HasNetDeltaSerializer","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasNetDeltaSerializer >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_650 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasPostScriptConstruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1036:8
	makeExtern<DAS_CALL_METHOD(_method_650), SimNode_ExtFuncCall >(lib,"HasPostScriptConstruct","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasPostScriptConstruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_651 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::IsPlainOldData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1047:8
	makeExtern<DAS_CALL_METHOD(_method_651), SimNode_ExtFuncCall >(lib,"IsPlainOldData","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::IsPlainOldData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_652 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::IsUECoreType >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1053:8
	makeExtern<DAS_CALL_METHOD(_method_652), SimNode_ExtFuncCall >(lib,"IsUECoreType","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::IsUECoreType >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_653 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::IsUECoreVariant >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1059:8
	makeExtern<DAS_CALL_METHOD(_method_653), SimNode_ExtFuncCall >(lib,"IsUECoreVariant","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::IsUECoreVariant >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_654 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasCopy >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1065:8
	makeExtern<DAS_CALL_METHOD(_method_654), SimNode_ExtFuncCall >(lib,"HasCopy","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasCopy >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_655 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasIdentical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1076:8
	makeExtern<DAS_CALL_METHOD(_method_655), SimNode_ExtFuncCall >(lib,"HasIdentical","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasIdentical >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_656 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasExportTextItem >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1087:8
	makeExtern<DAS_CALL_METHOD(_method_656), SimNode_ExtFuncCall >(lib,"HasExportTextItem","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasExportTextItem >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_657 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasImportTextItem >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1098:8
	makeExtern<DAS_CALL_METHOD(_method_657), SimNode_ExtFuncCall >(lib,"HasImportTextItem","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasImportTextItem >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_658 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasFindInnerPropertyInstance >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1108:8
	makeExtern<DAS_CALL_METHOD(_method_658), SimNode_ExtFuncCall >(lib,"HasFindInnerPropertyInstance","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasFindInnerPropertyInstance >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_659 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasAddStructReferencedObjects >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1115:8
	makeExtern<DAS_CALL_METHOD(_method_659), SimNode_ExtFuncCall >(lib,"HasAddStructReferencedObjects","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasAddStructReferencedObjects >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_660 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)(EPropertyObjectReferenceType) const,&UScriptStruct::ICppStructOps::HasSerializerObjectReferences >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1120:8
	makeExtern<DAS_CALL_METHOD(_method_660), SimNode_ExtFuncCall >(lib,"HasSerializerObjectReferences","das_call_member< bool (UScriptStruct::ICppStructOps::*)(EPropertyObjectReferenceType) const , &UScriptStruct::ICppStructOps::HasSerializerObjectReferences >::invoke")
		->args({"self","Type"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_661 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasSerializeFromMismatchedTag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1132:8
	makeExtern<DAS_CALL_METHOD(_method_661), SimNode_ExtFuncCall >(lib,"HasSerializeFromMismatchedTag","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasSerializeFromMismatchedTag >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_662 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasStructuredSerializeFromMismatchedTag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1136:8
	makeExtern<DAS_CALL_METHOD(_method_662), SimNode_ExtFuncCall >(lib,"HasStructuredSerializeFromMismatchedTag","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasStructuredSerializeFromMismatchedTag >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_663 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasGetTypeHash >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1149:8
	makeExtern<DAS_CALL_METHOD(_method_663), SimNode_ExtFuncCall >(lib,"HasGetTypeHash","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasGetTypeHash >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_664 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::IsAbstract >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1164:8
	makeExtern<DAS_CALL_METHOD(_method_664), SimNode_ExtFuncCall >(lib,"IsAbstract","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::IsAbstract >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_665 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasCanEditChange >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1171:8
	makeExtern<DAS_CALL_METHOD(_method_665), SimNode_ExtFuncCall >(lib,"HasCanEditChange","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasCanEditChange >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1541:2
	makeExtern< UClass * (*)(const UScriptStruct *) , _dasUnreal_static_226_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_226_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UScriptStruct*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1541:2
	makeExtern< const wchar_t * (*)(const UScriptStruct *) , _dasUnreal_static_227_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_227_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UScriptStruct*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1541:2
	makeExtern< void (*)(const UScriptStruct *) , _dasUnreal_static_228_StaticRegisterNativesUScriptStruct , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUScriptStruct","_dasUnreal_static_228_StaticRegisterNativesUScriptStruct")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UScriptStruct*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1566:31
	makeExtern< void (*)(UScriptStruct &,FArchive &) , _dasUnreal_virtual_229_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_229_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1570:31
	makeExtern< void (*)(UScriptStruct &,FArchive &,bool) , _dasUnreal_virtual_230_Link , SimNode_ExtFuncCall >(lib,"Link","_dasUnreal_virtual_230_Link")
		->args({"self","Ar","bRelinkExistingProperties"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1571:31
	makeExtern< void (*)(const UScriptStruct &,void *,int) , _dasUnreal_virtual_231_InitializeStruct , SimNode_ExtFuncCall >(lib,"InitializeStruct","_dasUnreal_virtual_231_InitializeStruct")
		->args({"self","Dest","ArrayDim"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1572:31
	makeExtern< void (*)(const UScriptStruct &,void *,int) , _dasUnreal_virtual_232_DestroyStruct , SimNode_ExtFuncCall >(lib,"DestroyStruct","_dasUnreal_virtual_232_DestroyStruct")
		->args({"self","Dest","ArrayDim"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1573:31
	makeExtern< bool (*)(const UScriptStruct &) , _dasUnreal_virtual_233_IsStructTrashed , SimNode_ExtFuncCall >(lib,"IsStructTrashed","_dasUnreal_virtual_233_IsStructTrashed")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_674 = das::das_call_member< void (UScriptStruct::*)(bool),&UScriptStruct::SetStructTrashed >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1577:23
	makeExtern<DAS_CALL_METHOD(_method_674), SimNode_ExtFuncCall >(lib,"SetStructTrashed","das_call_member< void (UScriptStruct::*)(bool) , &UScriptStruct::SetStructTrashed >::invoke")
		->args({"self","bIsTrash"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1600:31
	makeExtern< void (*)(UScriptStruct &) , _dasUnreal_virtual_234_PrepareCppStructOps , SimNode_ExtFuncCall >(lib,"PrepareCppStructOps","_dasUnreal_virtual_234_PrepareCppStructOps")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_676 = das::das_call_member< UScriptStruct::ICppStructOps * (UScriptStruct::*)() const,&UScriptStruct::GetCppStructOps >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1603:29
	makeExtern<DAS_CALL_METHOD(_method_676), SimNode_ExtFuncCall >(lib,"GetCppStructOps","das_call_member< UScriptStruct::ICppStructOps * (UScriptStruct::*)() const , &UScriptStruct::GetCppStructOps >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_677 = das::das_call_member< void (UScriptStruct::*)(),&UScriptStruct::ClearCppStructOps >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1610:7
	makeExtern<DAS_CALL_METHOD(_method_677), SimNode_ExtFuncCall >(lib,"ClearCppStructOps","das_call_member< void (UScriptStruct::*)() , &UScriptStruct::ClearCppStructOps >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_678 = das::das_call_member< bool (UScriptStruct::*)() const,&UScriptStruct::HasDefaults >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1621:19
	makeExtern<DAS_CALL_METHOD(_method_678), SimNode_ExtFuncCall >(lib,"HasDefaults","das_call_member< bool (UScriptStruct::*)() const , &UScriptStruct::HasDefaults >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_679 = das::das_call_member< bool (UScriptStruct::*)() const,&UScriptStruct::UseNativeSerialization >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1643:7
	makeExtern<DAS_CALL_METHOD(_method_679), SimNode_ExtFuncCall >(lib,"UseNativeSerialization","das_call_member< bool (UScriptStruct::*)() const , &UScriptStruct::UseNativeSerialization >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_680 = das::das_call_member< void (UScriptStruct::*)(FString &,const void *,const void *,UObject *,int,UObject *,bool) const,&UScriptStruct::ExportText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1679:23
	makeExtern<DAS_CALL_METHOD(_method_680), SimNode_ExtFuncCall >(lib,"ExportText","das_call_member< void (UScriptStruct::*)(FString &,const void *,const void *,UObject *,int,UObject *,bool) const , &UScriptStruct::ExportText >::invoke")
		->args({"self","ValueStr","Value","Defaults","OwnerObject","PortFlags","ExportRootScope","bAllowNativeOverride"})
		->arg_init(7,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_681 = das::das_call_member< bool (UScriptStruct::*)(const void *,const void *,unsigned int) const,&UScriptStruct::CompareScriptStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1717:23
	makeExtern<DAS_CALL_METHOD(_method_681), SimNode_ExtFuncCall >(lib,"CompareScriptStruct","das_call_member< bool (UScriptStruct::*)(const void *,const void *,unsigned int) const , &UScriptStruct::CompareScriptStruct >::invoke")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_682 = das::das_call_member< void (UScriptStruct::*)(void *,const void *,int) const,&UScriptStruct::CopyScriptStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1727:23
	makeExtern<DAS_CALL_METHOD(_method_682), SimNode_ExtFuncCall >(lib,"CopyScriptStruct","das_call_member< void (UScriptStruct::*)(void *,const void *,int) const , &UScriptStruct::CopyScriptStruct >::invoke")
		->args({"self","Dest","Src","ArrayDim"})
		->arg_init(3,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_683 = das::das_call_member< void (UScriptStruct::*)(void *,int) const,&UScriptStruct::ClearScriptStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1736:23
	makeExtern<DAS_CALL_METHOD(_method_683), SimNode_ExtFuncCall >(lib,"ClearScriptStruct","das_call_member< void (UScriptStruct::*)(void *,int) const , &UScriptStruct::ClearScriptStruct >::invoke")
		->args({"self","Dest","ArrayDim"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1744:33
	makeExtern< unsigned int (*)(const UScriptStruct &,const void *) , _dasUnreal_virtual_235_GetStructTypeHash , SimNode_ExtFuncCall >(lib,"GetStructTypeHash","_dasUnreal_virtual_235_GetStructTypeHash")
		->args({"self","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1747:31
	makeExtern< void (*)(UScriptStruct &) , _dasUnreal_virtual_236_RecursivelyPreload , SimNode_ExtFuncCall >(lib,"RecursivelyPreload","_dasUnreal_virtual_236_RecursivelyPreload")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1753:34
	makeExtern< FString (*)(const UScriptStruct &,unsigned int) , _dasUnreal_virtual_237_GetStructCPPName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetStructCPPName","_dasUnreal_virtual_237_GetStructCPPName")
		->args({"self","CPPExportFlags"})
		->arg_init(1,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1759:31
	makeExtern< void (*)(const UScriptStruct &,unsigned char *) , _dasUnreal_virtual_238_InitializeDefaultValue , SimNode_ExtFuncCall >(lib,"InitializeDefaultValue","_dasUnreal_virtual_238_InitializeDefaultValue")
		->args({"self","InStructData"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1782:2
	makeExtern< UClass * (*)(const UFunction *) , _dasUnreal_static_239_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_239_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UFunction*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1782:2
	makeExtern< const wchar_t * (*)(const UFunction *) , _dasUnreal_static_240_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_240_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UFunction*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1782:2
	makeExtern< void (*)(const UFunction *) , _dasUnreal_static_241_StaticRegisterNativesUFunction , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUFunction","_dasUnreal_static_241_StaticRegisterNativesUFunction")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UFunction*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_691 = das::das_call_member< UClass * (UFunction::*)() const,&UFunction::GetOuterUClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1783:2
	makeExtern<DAS_CALL_METHOD(_method_691), SimNode_ExtFuncCall >(lib,"GetOuterUClass","das_call_member< UClass * (UFunction::*)() const , &UFunction::GetOuterUClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_692 = das::das_call_member< void (UFunction::*)(),&UFunction::InitializeDerivedMembers >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1858:23
	makeExtern<DAS_CALL_METHOD(_method_692), SimNode_ExtFuncCall >(lib,"InitializeDerivedMembers","das_call_member< void (UFunction::*)() , &UFunction::InitializeDerivedMembers >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1861:31
	makeExtern< void (*)(UFunction &,FArchive &) , _dasUnreal_virtual_242_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_242_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1862:31
	makeExtern< void (*)(UFunction &) , _dasUnreal_virtual_243_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_243_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1865:31
	makeExtern< void (*)(UFunction &) , _dasUnreal_virtual_244_Bind , SimNode_ExtFuncCall >(lib,"Bind","_dasUnreal_virtual_244_Bind")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1868:19
	makeExtern< UStruct * (*)(const UFunction &) , _dasUnreal_virtual_245_GetInheritanceSuper , SimNode_ExtFuncCall >(lib,"GetInheritanceSuper","_dasUnreal_virtual_245_GetInheritanceSuper")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1869:31
	makeExtern< void (*)(UFunction &,FArchive &,bool) , _dasUnreal_virtual_246_Link , SimNode_ExtFuncCall >(lib,"Link","_dasUnreal_virtual_246_Link")
		->args({"self","Ar","bRelinkExistingProperties"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_698 = das::das_call_member< UFunction * (UFunction::*)() const,&UFunction::GetSuperFunction >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1872:29
	makeExtern<DAS_CALL_METHOD(_method_698), SimNode_ExtFuncCall >(lib,"GetSuperFunction","das_call_member< UFunction * (UFunction::*)() const , &UFunction::GetSuperFunction >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_699 = das::das_call_member< FProperty * (UFunction::*)() const,&UFunction::GetReturnProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1875:29
	makeExtern<DAS_CALL_METHOD(_method_699), SimNode_ExtFuncCall >(lib,"GetReturnProperty","das_call_member< FProperty * (UFunction::*)() const , &UFunction::GetReturnProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_700 = das::das_call_member< UClass * (UFunction::*)() const,&UFunction::GetOuterUClassUnchecked >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1878:22
	makeExtern<DAS_CALL_METHOD(_method_700), SimNode_ExtFuncCall >(lib,"GetOuterUClassUnchecked","das_call_member< UClass * (UFunction::*)() const , &UFunction::GetOuterUClassUnchecked >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1911:28
	makeExtern< uint64 (*)(const UFunction *) , _dasUnreal_static_247_GetDefaultIgnoredSignatureCompatibilityFlags , SimNode_ExtFuncCall >(lib,"GetDefaultIgnoredSignatureCompatibilityFlags","_dasUnreal_static_247_GetDefaultIgnoredSignatureCompatibilityFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UFunction*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_702 = das::das_call_member< bool (UFunction::*)(const UFunction *) const,&UFunction::IsSignatureCompatibleWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1929:23
	makeExtern<DAS_CALL_METHOD(_method_702), SimNode_ExtFuncCall >(lib,"IsSignatureCompatibleWith","das_call_member< bool (UFunction::*)(const UFunction *) const , &UFunction::IsSignatureCompatibleWith >::invoke")
		->args({"self","OtherFunction"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_703 = das::das_call_member< bool (UFunction::*)(const UFunction *,uint64) const,&UFunction::IsSignatureCompatibleWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1941:23
	makeExtern<DAS_CALL_METHOD(_method_703), SimNode_ExtFuncCall >(lib,"IsSignatureCompatibleWith","das_call_member< bool (UFunction::*)(const UFunction *,uint64) const , &UFunction::IsSignatureCompatibleWith >::invoke")
		->args({"self","OtherFunction","IgnoreFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2001:2
	makeExtern< UClass * (*)(const UEnum *) , _dasUnreal_static_248_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_248_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UEnum*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2001:2
	makeExtern< const wchar_t * (*)(const UEnum *) , _dasUnreal_static_249_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_249_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UEnum*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2001:2
	makeExtern< void (*)(const UEnum *) , _dasUnreal_static_250_StaticRegisterNativesUEnum , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUEnum","_dasUnreal_static_250_StaticRegisterNativesUEnum")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UEnum*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_707 = das::das_call_member< int (UEnum::*)(int64) const,&UEnum::GetIndexByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2022:20
	makeExtern<DAS_CALL_METHOD(_method_707), SimNode_ExtFuncCall >(lib,"GetIndexByValue","das_call_member< int (UEnum::*)(int64) const , &UEnum::GetIndexByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_708 = das::das_call_member< int64 (UEnum::*)(int) const,&UEnum::GetValueByIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2035:20
	makeExtern<DAS_CALL_METHOD(_method_708), SimNode_ExtFuncCall >(lib,"GetValueByIndex","das_call_member< int64 (UEnum::*)(int) const , &UEnum::GetValueByIndex >::invoke")
		->args({"self","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_709 = das::das_call_member< FName (UEnum::*)(int) const,&UEnum::GetNameByIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2042:24
	makeExtern<DAS_CALL_METHOD(_method_709), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameByIndex","das_call_member< FName (UEnum::*)(int) const , &UEnum::GetNameByIndex >::invoke")
		->args({"self","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_710 = das::das_call_member< int (UEnum::*)(FName,EGetByNameFlags) const,&UEnum::GetIndexByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2045:24
	makeExtern<DAS_CALL_METHOD(_method_710), SimNode_ExtFuncCall >(lib,"GetIndexByName","das_call_member< int (UEnum::*)(FName,EGetByNameFlags) const , &UEnum::GetIndexByName >::invoke")
		->args({"self","InName","Flags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EGetByNameFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_711 = das::das_call_member< FName (UEnum::*)(int64) const,&UEnum::GetNameByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2048:24
	makeExtern<DAS_CALL_METHOD(_method_711), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameByValue","das_call_member< FName (UEnum::*)(int64) const , &UEnum::GetNameByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_712 = das::das_call_member< int64 (UEnum::*)(FName,EGetByNameFlags) const,&UEnum::GetValueByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2051:24
	makeExtern<DAS_CALL_METHOD(_method_712), SimNode_ExtFuncCall >(lib,"GetValueByName","das_call_member< int64 (UEnum::*)(FName,EGetByNameFlags) const , &UEnum::GetValueByName >::invoke")
		->args({"self","InName","Flags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EGetByNameFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_713 = das::das_call_member< FString (UEnum::*)(int) const,&UEnum::GetNameStringByIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2054:26
	makeExtern<DAS_CALL_METHOD(_method_713), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameStringByIndex","das_call_member< FString (UEnum::*)(int) const , &UEnum::GetNameStringByIndex >::invoke")
		->args({"self","InIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_714 = das::das_call_member< int (UEnum::*)(const FString &,EGetByNameFlags) const,&UEnum::GetIndexByNameString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2057:24
	makeExtern<DAS_CALL_METHOD(_method_714), SimNode_ExtFuncCall >(lib,"GetIndexByNameString","das_call_member< int (UEnum::*)(const FString &,EGetByNameFlags) const , &UEnum::GetIndexByNameString >::invoke")
		->args({"self","SearchString","Flags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EGetByNameFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_715 = das::das_call_member< FString (UEnum::*)(int64) const,&UEnum::GetNameStringByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2060:26
	makeExtern<DAS_CALL_METHOD(_method_715), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameStringByValue","das_call_member< FString (UEnum::*)(int64) const , &UEnum::GetNameStringByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_716 = das::das_call_member< FString (UEnum::*)(int64) const,&UEnum::GetValueOrBitfieldAsString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2063:26
	makeExtern<DAS_CALL_METHOD(_method_716), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetValueOrBitfieldAsString","das_call_member< FString (UEnum::*)(int64) const , &UEnum::GetValueOrBitfieldAsString >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_717 = das::das_call_member< bool (UEnum::*)(FString &,int64) const,&UEnum::FindNameStringByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2066:23
	makeExtern<DAS_CALL_METHOD(_method_717), SimNode_ExtFuncCall >(lib,"FindNameStringByValue","das_call_member< bool (UEnum::*)(FString &,int64) const , &UEnum::FindNameStringByValue >::invoke")
		->args({"self","Out","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_718 = das::das_call_member< int64 (UEnum::*)(const FString &,EGetByNameFlags) const,&UEnum::GetValueByNameString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2069:24
	makeExtern<DAS_CALL_METHOD(_method_718), SimNode_ExtFuncCall >(lib,"GetValueByNameString","das_call_member< int64 (UEnum::*)(const FString &,EGetByNameFlags) const , &UEnum::GetValueByNameString >::invoke")
		->args({"self","SearchString","Flags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EGetByNameFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2079:32
	makeExtern< FText (*)(const UEnum &,int) , _dasUnreal_virtual_251_GetDisplayNameTextByIndex , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDisplayNameTextByIndex","_dasUnreal_virtual_251_GetDisplayNameTextByIndex")
		->args({"self","InIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_720 = das::das_call_member< FText (UEnum::*)(int64) const,&UEnum::GetDisplayNameTextByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2082:24
	makeExtern<DAS_CALL_METHOD(_method_720), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDisplayNameTextByValue","das_call_member< FText (UEnum::*)(int64) const , &UEnum::GetDisplayNameTextByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_721 = das::das_call_member< bool (UEnum::*)(FText &,int64) const,&UEnum::FindDisplayNameTextByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2085:23
	makeExtern<DAS_CALL_METHOD(_method_721), SimNode_ExtFuncCall >(lib,"FindDisplayNameTextByValue","das_call_member< bool (UEnum::*)(FText &,int64) const , &UEnum::FindDisplayNameTextByValue >::invoke")
		->args({"self","Out","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2096:34
	makeExtern< FString (*)(const UEnum &,int) , _dasUnreal_virtual_252_GetAuthoredNameStringByIndex , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredNameStringByIndex","_dasUnreal_virtual_252_GetAuthoredNameStringByIndex")
		->args({"self","InIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_723 = das::das_call_member< FString (UEnum::*)(int64) const,&UEnum::GetAuthoredNameStringByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2099:26
	makeExtern<DAS_CALL_METHOD(_method_723), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredNameStringByValue","das_call_member< FString (UEnum::*)(int64) const , &UEnum::GetAuthoredNameStringByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_724 = das::das_call_member< bool (UEnum::*)(FString &,int64) const,&UEnum::FindAuthoredNameStringByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2102:23
	makeExtern<DAS_CALL_METHOD(_method_724), SimNode_ExtFuncCall >(lib,"FindAuthoredNameStringByValue","das_call_member< bool (UEnum::*)(FString &,int64) const , &UEnum::FindAuthoredNameStringByValue >::invoke")
		->args({"self","Out","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_725 = das::das_call_member< int64 (UEnum::*)() const,&UEnum::GetMaxEnumValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2105:24
	makeExtern<DAS_CALL_METHOD(_method_725), SimNode_ExtFuncCall >(lib,"GetMaxEnumValue","das_call_member< int64 (UEnum::*)() const , &UEnum::GetMaxEnumValue >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_726 = das::das_call_member< bool (UEnum::*)(int64) const,&UEnum::IsValidEnumValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2108:23
	makeExtern<DAS_CALL_METHOD(_method_726), SimNode_ExtFuncCall >(lib,"IsValidEnumValue","das_call_member< bool (UEnum::*)(int64) const , &UEnum::IsValidEnumValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_727 = das::das_call_member< bool (UEnum::*)(FName) const,&UEnum::IsValidEnumName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2111:23
	makeExtern<DAS_CALL_METHOD(_method_727), SimNode_ExtFuncCall >(lib,"IsValidEnumName","das_call_member< bool (UEnum::*)(FName) const , &UEnum::IsValidEnumName >::invoke")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

