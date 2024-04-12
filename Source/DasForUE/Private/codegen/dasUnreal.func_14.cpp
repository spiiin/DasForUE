// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_14() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2408:16
	makeExtern< int (*)(const FBoolProperty &) , _dasUnreal_virtual_444_GetMinAlignment , SimNode_ExtFuncCall >(lib,"GetMinAlignment","_dasUnreal_virtual_444_GetMinAlignment")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2411:15
	makeExtern< void (*)(const FBoolProperty &,FBlake3 &,bool) , _dasUnreal_virtual_445_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_445_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1089 = das::das_call_member< bool (FBoolProperty::*)(const void *) const,&FBoolProperty::GetPropertyValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2418:19
	makeExtern<DAS_CALL_METHOD(_method_1089), SimNode_ExtFuncCall >(lib,"GetPropertyValue","das_call_member< bool (FBoolProperty::*)(const void *) const , &FBoolProperty::GetPropertyValue >::invoke")
		->args({"self","A"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1090 = das::das_call_member< bool (FBoolProperty::*)(const void *,int) const,&FBoolProperty::GetPropertyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2424:19
	makeExtern<DAS_CALL_METHOD(_method_1090), SimNode_ExtFuncCall >(lib,"GetPropertyValue_InContainer","das_call_member< bool (FBoolProperty::*)(const void *,int) const , &FBoolProperty::GetPropertyValue_InContainer >::invoke")
		->args({"self","A","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2428:26
	makeExtern< bool (*)(const FBoolProperty *) , _dasUnreal_static_446_GetDefaultPropertyValue , SimNode_ExtFuncCall >(lib,"GetDefaultPropertyValue","_dasUnreal_static_446_GetDefaultPropertyValue")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1092 = das::das_call_member< bool (FBoolProperty::*)(const void *) const,&FBoolProperty::GetOptionalPropertyValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2432:19
	makeExtern<DAS_CALL_METHOD(_method_1092), SimNode_ExtFuncCall >(lib,"GetOptionalPropertyValue","das_call_member< bool (FBoolProperty::*)(const void *) const , &FBoolProperty::GetOptionalPropertyValue >::invoke")
		->args({"self","B"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1093 = das::das_call_member< bool (FBoolProperty::*)(const void *,int) const,&FBoolProperty::GetOptionalPropertyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2436:19
	makeExtern<DAS_CALL_METHOD(_method_1093), SimNode_ExtFuncCall >(lib,"GetOptionalPropertyValue_InContainer","das_call_member< bool (FBoolProperty::*)(const void *,int) const , &FBoolProperty::GetOptionalPropertyValue_InContainer >::invoke")
		->args({"self","B","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1094 = das::das_call_member< void (FBoolProperty::*)(void *,bool) const,&FBoolProperty::SetPropertyValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2440:19
	makeExtern<DAS_CALL_METHOD(_method_1094), SimNode_ExtFuncCall >(lib,"SetPropertyValue","das_call_member< void (FBoolProperty::*)(void *,bool) const , &FBoolProperty::SetPropertyValue >::invoke")
		->args({"self","A","Value"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1095 = das::das_call_member< void (FBoolProperty::*)(void *,bool,int) const,&FBoolProperty::SetPropertyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2446:19
	makeExtern<DAS_CALL_METHOD(_method_1095), SimNode_ExtFuncCall >(lib,"SetPropertyValue_InContainer","das_call_member< void (FBoolProperty::*)(void *,bool,int) const , &FBoolProperty::SetPropertyValue_InContainer >::invoke")
		->args({"self","A","Value","ArrayIndex"})
		->arg_init(3,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1096 = das::das_call_member< void (FBoolProperty::*)(const unsigned int,const bool,const unsigned int),&FBoolProperty::SetBoolSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2459:7
	makeExtern<DAS_CALL_METHOD(_method_1096), SimNode_ExtFuncCall >(lib,"SetBoolSize","das_call_member< void (FBoolProperty::*)(const unsigned int,const bool,const unsigned int) , &FBoolProperty::SetBoolSize >::invoke")
		->args({"self","InSize","bIsNativeBool","InBitMask"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->arg_init(3,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1097 = das::das_call_member< bool (FBoolProperty::*)() const,&FBoolProperty::IsNativeBool >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2464:19
	makeExtern<DAS_CALL_METHOD(_method_1097), SimNode_ExtFuncCall >(lib,"IsNativeBool","das_call_member< bool (FBoolProperty::*)() const , &FBoolProperty::IsNativeBool >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1098 = das::das_call_member< unsigned char (FBoolProperty::*)() const,&FBoolProperty::GetFieldMask >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2470:20
	makeExtern<DAS_CALL_METHOD(_method_1098), SimNode_ExtFuncCall >(lib,"GetFieldMask","das_call_member< unsigned char (FBoolProperty::*)() const , &FBoolProperty::GetFieldMask >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1099 = das::das_call_member< unsigned char (FBoolProperty::*)() const,&FBoolProperty::GetByteOffset >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2476:20
	makeExtern<DAS_CALL_METHOD(_method_1099), SimNode_ExtFuncCall >(lib,"GetByteOffset","das_call_member< unsigned char (FBoolProperty::*)() const , &FBoolProperty::GetByteOffset >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2481:9
	makeExtern< unsigned int (*)(const FBoolProperty &,const void *) , _dasUnreal_virtual_447_GetValueTypeHashInternal , SimNode_ExtFuncCall >(lib,"GetValueTypeHashInternal","_dasUnreal_virtual_447_GetValueTypeHashInternal")
		->args({"self","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FObjectPropertyBase,EInternal,FFieldClass *>(*this,lib,"FObjectPropertyBase","FObjectPropertyBase")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2493:2
	makeExtern< FFieldClass * (*)(const FObjectPropertyBase *) , _dasUnreal_static_448_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_448_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2493:2
	makeExtern< FField * (*)(const FObjectPropertyBase *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_449_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_449_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2493:2
	makeExtern< uint64 (*)(const FObjectPropertyBase *) , _dasUnreal_static_450_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_450_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2493:2
	makeExtern< uint64 (*)(const FObjectPropertyBase *) , _dasUnreal_static_451_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_451_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FObjectPropertyBase,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FObjectPropertyBase","FObjectPropertyBase")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FObjectPropertyBase,UField *>(*this,lib,"FObjectPropertyBase","FObjectPropertyBase")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2526:15
	makeExtern< void (*)(FObjectPropertyBase &,FArchive &) , _dasUnreal_virtual_452_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_452_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2527:15
	makeExtern< void (*)(FObjectPropertyBase &,FReferenceCollector &) , _dasUnreal_virtual_453_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_453_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2528:15
	makeExtern< void (*)(FObjectPropertyBase &) , _dasUnreal_virtual_454_BeginDestroy , SimNode_ExtFuncCall >(lib,"BeginDestroy","_dasUnreal_virtual_454_BeginDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2532:15
	makeExtern< void (*)(FObjectPropertyBase &,const FField &) , _dasUnreal_virtual_455_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_455_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2535:15
	makeExtern< bool (*)(const FObjectPropertyBase &,const void *,const void *,unsigned int) , _dasUnreal_virtual_456_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_456_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2537:15
	makeExtern< bool (*)(const FObjectPropertyBase &) , _dasUnreal_virtual_457_SupportsNetSharedSerialization , SimNode_ExtFuncCall >(lib,"SupportsNetSharedSerialization","_dasUnreal_virtual_457_SupportsNetSharedSerialization")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2542:16
	makeExtern< FName (*)(const FObjectPropertyBase &) , _dasUnreal_virtual_458_GetID , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetID","_dasUnreal_virtual_458_GetID")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2544:15
	makeExtern< bool (*)(const FObjectPropertyBase &,const FProperty *) , _dasUnreal_virtual_459_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_459_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2550:18
	makeExtern< FString (*)(const FObjectPropertyBase &,FString *,unsigned int,const FString &) , _dasUnreal_virtual_460_GetCPPTypeCustom , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeCustom","_dasUnreal_virtual_460_GetCPPTypeCustom")
		->args({"self","ExtendedTypeText","CPPExportFlags","InnerNativeTypeName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2589:14
	makeExtern< bool (*)(const FObjectPropertyBase *,UObject *,UObject *,unsigned int) , _dasUnreal_static_461_StaticIdentical , SimNode_ExtFuncCall >(lib,"StaticIdentical","_dasUnreal_static_461_StaticIdentical")
		->args({"self","A","B","PortFlags"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2591:19
	makeExtern< UObject * (*)(const FObjectPropertyBase &,const void *) , _dasUnreal_virtual_462_LoadObjectPropertyValue , SimNode_ExtFuncCall >(lib,"LoadObjectPropertyValue","_dasUnreal_virtual_462_LoadObjectPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1116 = das::das_call_member< UObject * (FObjectPropertyBase::*)(const void *,int) const,&FObjectPropertyBase::LoadObjectPropertyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2595:23
	makeExtern<DAS_CALL_METHOD(_method_1116), SimNode_ExtFuncCall >(lib,"LoadObjectPropertyValue_InContainer","das_call_member< UObject * (FObjectPropertyBase::*)(const void *,int) const , &FObjectPropertyBase::LoadObjectPropertyValue_InContainer >::invoke")
		->args({"self","PropertyValueAddress","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2601:19
	makeExtern< UObject * (*)(const FObjectPropertyBase &,const void *) , _dasUnreal_virtual_463_GetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue","_dasUnreal_virtual_463_GetObjectPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2602:19
	makeExtern< UObject * (*)(const FObjectPropertyBase &,const void *,int) , _dasUnreal_virtual_464_GetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue_InContainer","_dasUnreal_virtual_464_GetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2603:15
	makeExtern< void (*)(const FObjectPropertyBase &,void *,UObject *) , _dasUnreal_virtual_465_SetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue","_dasUnreal_virtual_465_SetObjectPropertyValue")
		->args({"self","PropertyValueAddress","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2605:15
	makeExtern< void (*)(const FObjectPropertyBase &,void *,UObject *,int) , _dasUnreal_virtual_466_SetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue_InContainer","_dasUnreal_virtual_466_SetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","Value","ArrayIndex"})
		->arg_init(3,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1121 = das::das_call_member< void (FObjectPropertyBase::*)(UClass *),&FObjectPropertyBase::SetPropertyClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2617:7
	makeExtern<DAS_CALL_METHOD(_method_1121), SimNode_ExtFuncCall >(lib,"SetPropertyClass","das_call_member< void (FObjectPropertyBase::*)(UClass *) , &FObjectPropertyBase::SetPropertyClass >::invoke")
		->args({"self","NewPropertyClass"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStrProperty,EInternal,FFieldClass *>(*this,lib,"FStrProperty","FStrProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3566:2
	makeExtern< FFieldClass * (*)(const FStrProperty *) , _dasUnreal_static_467_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_467_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3566:2
	makeExtern< FField * (*)(const FStrProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_468_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_468_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FStrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3566:2
	makeExtern< uint64 (*)(const FStrProperty *) , _dasUnreal_static_469_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_469_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3566:2
	makeExtern< uint64 (*)(const FStrProperty *) , _dasUnreal_static_470_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_470_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStrProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FStrProperty","FStrProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FStrProperty,UField *>(*this,lib,"FStrProperty","FStrProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3604:18
	makeExtern< FString (*)(const FStrProperty &) , _dasUnreal_virtual_471_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_471_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3605:9
	makeExtern< unsigned int (*)(const FStrProperty &,const void *) , _dasUnreal_virtual_472_GetValueTypeHashInternal , SimNode_ExtFuncCall >(lib,"GetValueTypeHashInternal","_dasUnreal_virtual_472_GetValueTypeHashInternal")
		->args({"self","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FArrayProperty,EInternal,FFieldClass *>(*this,lib,"FArrayProperty","FArrayProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3630:2
	makeExtern< FFieldClass * (*)(const FArrayProperty *) , _dasUnreal_static_473_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_473_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FArrayProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3630:2
	makeExtern< FField * (*)(const FArrayProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_474_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_474_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FArrayProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3630:2
	makeExtern< uint64 (*)(const FArrayProperty *) , _dasUnreal_static_475_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_475_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FArrayProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3630:2
	makeExtern< uint64 (*)(const FArrayProperty *) , _dasUnreal_static_476_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_476_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FArrayProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FArrayProperty,FFieldVariant,const FName &,EObjectFlags,EArrayPropertyFlags>(*this,lib,"FArrayProperty","FArrayProperty")
		->args({"InOwner","InName","InObjectFlags","InArrayPropertyFlags"})
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<EArrayPropertyFlags>(lib)));
	addCtorAndUsing<FArrayProperty,UField *>(*this,lib,"FArrayProperty","FArrayProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3681:15
	makeExtern< void (*)(FArrayProperty &,FArchive &) , _dasUnreal_virtual_477_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_477_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3682:15
	makeExtern< void (*)(FArrayProperty &,FReferenceCollector &) , _dasUnreal_virtual_478_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_478_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3687:15
	makeExtern< void (*)(FArrayProperty &,const FField &) , _dasUnreal_virtual_479_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_479_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3690:15
	makeExtern< void (*)(FArrayProperty &,FProperty *) , _dasUnreal_virtual_480_AddCppProperty , SimNode_ExtFuncCall >(lib,"AddCppProperty","_dasUnreal_virtual_480_AddCppProperty")
		->args({"self","Property"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3691:18
	makeExtern< FField * (*)(FArrayProperty &,const FName &) , _dasUnreal_virtual_481_GetInnerFieldByName , SimNode_ExtFuncCall >(lib,"GetInnerFieldByName","_dasUnreal_virtual_481_GetInnerFieldByName")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3697:18
	makeExtern< FString (*)(const FArrayProperty &,FString &) , _dasUnreal_virtual_482_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_482_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3698:18
	makeExtern< FString (*)(const FArrayProperty &,FString *,unsigned int) , _dasUnreal_virtual_483_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_483_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3699:18
	makeExtern< FString (*)(const FArrayProperty &) , _dasUnreal_virtual_484_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_484_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3700:15
	makeExtern< void (*)(FArrayProperty &,FArchive &) , _dasUnreal_virtual_485_LinkInternal , SimNode_ExtFuncCall >(lib,"LinkInternal","_dasUnreal_virtual_485_LinkInternal")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3701:15
	makeExtern< bool (*)(const FArrayProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_486_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_486_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3708:15
	makeExtern< void (*)(const FArrayProperty &,void *) , _dasUnreal_virtual_487_InitializeValueInternal , SimNode_ExtFuncCall >(lib,"InitializeValueInternal","_dasUnreal_virtual_487_InitializeValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3727:15
	makeExtern< void (*)(const FArrayProperty &,void *,const void *,int) , _dasUnreal_virtual_488_CopyValuesInternal , SimNode_ExtFuncCall >(lib,"CopyValuesInternal","_dasUnreal_virtual_488_CopyValuesInternal")
		->args({"self","Dest","Src","Count"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3728:15
	makeExtern< void (*)(const FArrayProperty &,void *) , _dasUnreal_virtual_489_ClearValueInternal , SimNode_ExtFuncCall >(lib,"ClearValueInternal","_dasUnreal_virtual_489_ClearValueInternal")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3729:15
	makeExtern< void (*)(const FArrayProperty &,void *) , _dasUnreal_virtual_490_DestroyValueInternal , SimNode_ExtFuncCall >(lib,"DestroyValueInternal","_dasUnreal_virtual_490_DestroyValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3730:15
	makeExtern< bool (*)(const FArrayProperty &) , _dasUnreal_virtual_491_PassCPPArgsByRef , SimNode_ExtFuncCall >(lib,"PassCPPArgsByRef","_dasUnreal_virtual_491_PassCPPArgsByRef")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3734:15
	makeExtern< bool (*)(const FArrayProperty &,const FProperty *) , _dasUnreal_virtual_492_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_492_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3737:16
	makeExtern< int (*)(const FArrayProperty &) , _dasUnreal_virtual_493_GetMinAlignment , SimNode_ExtFuncCall >(lib,"GetMinAlignment","_dasUnreal_virtual_493_GetMinAlignment")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3743:16
	makeExtern< void * (*)(const FArrayProperty &,const FProperty *,void *,int) , _dasUnreal_virtual_494_GetValueAddressAtIndex_Direct , SimNode_ExtFuncCall >(lib,"GetValueAddressAtIndex_Direct","_dasUnreal_virtual_494_GetValueAddressAtIndex_Direct")
		->args({"self","Inner","InValueAddress","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1150 = das::das_call_member< FString (FArrayProperty::*)(FString *,unsigned int,const FString &,const FString &) const,&FArrayProperty::GetCPPTypeCustom >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3746:10
	makeExtern<DAS_CALL_METHOD(_method_1150), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeCustom","das_call_member< FString (FArrayProperty::*)(FString *,unsigned int,const FString &,const FString &) const , &FArrayProperty::GetCPPTypeCustom >::invoke")
		->args({"self","ExtendedTypeText","CPPExportFlags","InnerTypeText","InInnerExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3749:14
	makeExtern< void (*)(const FArrayProperty *,FString &,const FProperty *,const void *,int,const void *,int,UObject *,int,UObject *) , _dasUnreal_static_495_ExportTextInnerItem , SimNode_ExtFuncCall >(lib,"ExportTextInnerItem","_dasUnreal_static_495_ExportTextInnerItem")
		->args({"self","ValueStr","Inner","PropertyValue","PropertySize","DefaultValue","DefaultSize","Parent","PortFlags","ExportRootScope"})
		->arg_type(0,makeType<TExplicit<const FArrayProperty*> >(lib))
		->arg_init(7,make_smart<ExprConstPtr>())
		->arg_init(8,make_smart<ExprConstInt>(0))
		->arg_init(9,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3754:15
	makeExtern< void (*)(const FArrayProperty &,FBlake3 &,bool) , _dasUnreal_virtual_496_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_496_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FMapProperty,EInternal,FFieldClass *>(*this,lib,"FMapProperty","FMapProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3774:2
	makeExtern< FFieldClass * (*)(const FMapProperty *) , _dasUnreal_static_497_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_497_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FMapProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3774:2
	makeExtern< FField * (*)(const FMapProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_498_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_498_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FMapProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3774:2
	makeExtern< uint64 (*)(const FMapProperty *) , _dasUnreal_static_499_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_499_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FMapProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3774:2
	makeExtern< uint64 (*)(const FMapProperty *) , _dasUnreal_static_500_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_500_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FMapProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FMapProperty,FFieldVariant,const FName &,EObjectFlags,EMapPropertyFlags>(*this,lib,"FMapProperty","FMapProperty")
		->args({"InOwner","InName","InObjectFlags","InMapFlags"})
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<EMapPropertyFlags>(lib)));
	addCtorAndUsing<FMapProperty,UField *>(*this,lib,"FMapProperty","FMapProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3818:15
	makeExtern< void (*)(FMapProperty &,FArchive &) , _dasUnreal_virtual_501_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_501_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3819:15
	makeExtern< void (*)(FMapProperty &,FReferenceCollector &) , _dasUnreal_virtual_502_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_502_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3824:15
	makeExtern< void (*)(FMapProperty &,const FField &) , _dasUnreal_virtual_503_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_503_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3825:18
	makeExtern< FField * (*)(FMapProperty &,const FName &) , _dasUnreal_virtual_504_GetInnerFieldByName , SimNode_ExtFuncCall >(lib,"GetInnerFieldByName","_dasUnreal_virtual_504_GetInnerFieldByName")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3829:15
	makeExtern< void (*)(FMapProperty &,FProperty *) , _dasUnreal_virtual_505_AddCppProperty , SimNode_ExtFuncCall >(lib,"AddCppProperty","_dasUnreal_virtual_505_AddCppProperty")
		->args({"self","Property"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3833:18
	makeExtern< FString (*)(const FMapProperty &,FString &) , _dasUnreal_virtual_506_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_506_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3834:18
	makeExtern< FString (*)(const FMapProperty &,FString *,unsigned int) , _dasUnreal_virtual_507_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_507_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3835:18
	makeExtern< FString (*)(const FMapProperty &) , _dasUnreal_virtual_508_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_508_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3836:15
	makeExtern< void (*)(FMapProperty &,FArchive &) , _dasUnreal_virtual_509_LinkInternal , SimNode_ExtFuncCall >(lib,"LinkInternal","_dasUnreal_virtual_509_LinkInternal")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3837:15
	makeExtern< bool (*)(const FMapProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_510_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_510_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3844:15
	makeExtern< void (*)(const FMapProperty &,void *) , _dasUnreal_virtual_511_InitializeValueInternal , SimNode_ExtFuncCall >(lib,"InitializeValueInternal","_dasUnreal_virtual_511_InitializeValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3863:15
	makeExtern< void (*)(const FMapProperty &,void *,const void *,int) , _dasUnreal_virtual_512_CopyValuesInternal , SimNode_ExtFuncCall >(lib,"CopyValuesInternal","_dasUnreal_virtual_512_CopyValuesInternal")
		->args({"self","Dest","Src","Count"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3864:15
	makeExtern< void (*)(const FMapProperty &,void *) , _dasUnreal_virtual_513_ClearValueInternal , SimNode_ExtFuncCall >(lib,"ClearValueInternal","_dasUnreal_virtual_513_ClearValueInternal")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3865:15
	makeExtern< void (*)(const FMapProperty &,void *) , _dasUnreal_virtual_514_DestroyValueInternal , SimNode_ExtFuncCall >(lib,"DestroyValueInternal","_dasUnreal_virtual_514_DestroyValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3866:15
	makeExtern< bool (*)(const FMapProperty &) , _dasUnreal_virtual_515_PassCPPArgsByRef , SimNode_ExtFuncCall >(lib,"PassCPPArgsByRef","_dasUnreal_virtual_515_PassCPPArgsByRef")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3870:15
	makeExtern< bool (*)(const FMapProperty &,const FProperty *) , _dasUnreal_virtual_516_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_516_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3872:16
	makeExtern< void * (*)(const FMapProperty &,const FProperty *,void *,int) , _dasUnreal_virtual_517_GetValueAddressAtIndex_Direct , SimNode_ExtFuncCall >(lib,"GetValueAddressAtIndex_Direct","_dasUnreal_virtual_517_GetValueAddressAtIndex_Direct")
		->args({"self","Inner","InValueAddress","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1174 = das::das_call_member< FString (FMapProperty::*)(FString *,unsigned int,const FString &,const FString &,const FString &,const FString &) const,&FMapProperty::GetCPPTypeCustom >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3875:10
	makeExtern<DAS_CALL_METHOD(_method_1174), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeCustom","das_call_member< FString (FMapProperty::*)(FString *,unsigned int,const FString &,const FString &,const FString &,const FString &) const , &FMapProperty::GetCPPTypeCustom >::invoke")
		->args({"self","ExtendedTypeText","CPPExportFlags","KeyTypeText","InKeyExtendedTypeText","ValueTypeText","InValueExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

