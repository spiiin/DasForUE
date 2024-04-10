// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_14() {
	using _method_1089 = das::das_call_member< bool (FBoolProperty::*)(const void *) const,&FBoolProperty::GetOptionalPropertyValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2432:19
	makeExtern<DAS_CALL_METHOD(_method_1089), SimNode_ExtFuncCall >(lib,"GetOptionalPropertyValue","das_call_member< bool (FBoolProperty::*)(const void *) const , &FBoolProperty::GetOptionalPropertyValue >::invoke")
		->args({"self","B"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1090 = das::das_call_member< bool (FBoolProperty::*)(const void *,int) const,&FBoolProperty::GetOptionalPropertyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2436:19
	makeExtern<DAS_CALL_METHOD(_method_1090), SimNode_ExtFuncCall >(lib,"GetOptionalPropertyValue_InContainer","das_call_member< bool (FBoolProperty::*)(const void *,int) const , &FBoolProperty::GetOptionalPropertyValue_InContainer >::invoke")
		->args({"self","B","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1091 = das::das_call_member< void (FBoolProperty::*)(void *,bool) const,&FBoolProperty::SetPropertyValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2440:19
	makeExtern<DAS_CALL_METHOD(_method_1091), SimNode_ExtFuncCall >(lib,"SetPropertyValue","das_call_member< void (FBoolProperty::*)(void *,bool) const , &FBoolProperty::SetPropertyValue >::invoke")
		->args({"self","A","Value"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1092 = das::das_call_member< void (FBoolProperty::*)(void *,bool,int) const,&FBoolProperty::SetPropertyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2446:19
	makeExtern<DAS_CALL_METHOD(_method_1092), SimNode_ExtFuncCall >(lib,"SetPropertyValue_InContainer","das_call_member< void (FBoolProperty::*)(void *,bool,int) const , &FBoolProperty::SetPropertyValue_InContainer >::invoke")
		->args({"self","A","Value","ArrayIndex"})
		->arg_init(3,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1093 = das::das_call_member< void (FBoolProperty::*)(const unsigned int,const bool,const unsigned int),&FBoolProperty::SetBoolSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2459:7
	makeExtern<DAS_CALL_METHOD(_method_1093), SimNode_ExtFuncCall >(lib,"SetBoolSize","das_call_member< void (FBoolProperty::*)(const unsigned int,const bool,const unsigned int) , &FBoolProperty::SetBoolSize >::invoke")
		->args({"self","InSize","bIsNativeBool","InBitMask"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->arg_init(3,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1094 = das::das_call_member< bool (FBoolProperty::*)() const,&FBoolProperty::IsNativeBool >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2464:19
	makeExtern<DAS_CALL_METHOD(_method_1094), SimNode_ExtFuncCall >(lib,"IsNativeBool","das_call_member< bool (FBoolProperty::*)() const , &FBoolProperty::IsNativeBool >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1095 = das::das_call_member< unsigned char (FBoolProperty::*)() const,&FBoolProperty::GetFieldMask >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2470:20
	makeExtern<DAS_CALL_METHOD(_method_1095), SimNode_ExtFuncCall >(lib,"GetFieldMask","das_call_member< unsigned char (FBoolProperty::*)() const , &FBoolProperty::GetFieldMask >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1096 = das::das_call_member< unsigned char (FBoolProperty::*)() const,&FBoolProperty::GetByteOffset >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2476:20
	makeExtern<DAS_CALL_METHOD(_method_1096), SimNode_ExtFuncCall >(lib,"GetByteOffset","das_call_member< unsigned char (FBoolProperty::*)() const , &FBoolProperty::GetByteOffset >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2481:9
	makeExtern< unsigned int (*)(const FBoolProperty &,const void *) , _dasUnreal_virtual_446_GetValueTypeHashInternal , SimNode_ExtFuncCall >(lib,"GetValueTypeHashInternal","_dasUnreal_virtual_446_GetValueTypeHashInternal")
		->args({"self","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FObjectPropertyBase,EInternal,FFieldClass *>(*this,lib,"FObjectPropertyBase","FObjectPropertyBase")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2493:2
	makeExtern< FFieldClass * (*)(const FObjectPropertyBase *) , _dasUnreal_static_447_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_447_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2493:2
	makeExtern< FField * (*)(const FObjectPropertyBase *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_448_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_448_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2493:2
	makeExtern< uint64 (*)(const FObjectPropertyBase *) , _dasUnreal_static_449_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_449_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2493:2
	makeExtern< uint64 (*)(const FObjectPropertyBase *) , _dasUnreal_static_450_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_450_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FObjectPropertyBase,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FObjectPropertyBase","FObjectPropertyBase")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FObjectPropertyBase,UField *>(*this,lib,"FObjectPropertyBase","FObjectPropertyBase")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2526:15
	makeExtern< void (*)(FObjectPropertyBase &,FArchive &) , _dasUnreal_virtual_451_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_451_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2527:15
	makeExtern< void (*)(FObjectPropertyBase &,FReferenceCollector &) , _dasUnreal_virtual_452_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_452_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2528:15
	makeExtern< void (*)(FObjectPropertyBase &) , _dasUnreal_virtual_453_BeginDestroy , SimNode_ExtFuncCall >(lib,"BeginDestroy","_dasUnreal_virtual_453_BeginDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2532:15
	makeExtern< void (*)(FObjectPropertyBase &,const FField &) , _dasUnreal_virtual_454_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_454_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2535:15
	makeExtern< bool (*)(const FObjectPropertyBase &,const void *,const void *,unsigned int) , _dasUnreal_virtual_455_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_455_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2537:15
	makeExtern< bool (*)(const FObjectPropertyBase &) , _dasUnreal_virtual_456_SupportsNetSharedSerialization , SimNode_ExtFuncCall >(lib,"SupportsNetSharedSerialization","_dasUnreal_virtual_456_SupportsNetSharedSerialization")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2542:16
	makeExtern< FName (*)(const FObjectPropertyBase &) , _dasUnreal_virtual_457_GetID , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetID","_dasUnreal_virtual_457_GetID")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2544:15
	makeExtern< bool (*)(const FObjectPropertyBase &,const FProperty *) , _dasUnreal_virtual_458_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_458_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2550:18
	makeExtern< FString (*)(const FObjectPropertyBase &,FString *,unsigned int,const FString &) , _dasUnreal_virtual_459_GetCPPTypeCustom , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeCustom","_dasUnreal_virtual_459_GetCPPTypeCustom")
		->args({"self","ExtendedTypeText","CPPExportFlags","InnerNativeTypeName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2589:14
	makeExtern< bool (*)(const FObjectPropertyBase *,UObject *,UObject *,unsigned int) , _dasUnreal_static_460_StaticIdentical , SimNode_ExtFuncCall >(lib,"StaticIdentical","_dasUnreal_static_460_StaticIdentical")
		->args({"self","A","B","PortFlags"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2591:19
	makeExtern< UObject * (*)(const FObjectPropertyBase &,const void *) , _dasUnreal_virtual_461_LoadObjectPropertyValue , SimNode_ExtFuncCall >(lib,"LoadObjectPropertyValue","_dasUnreal_virtual_461_LoadObjectPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1113 = das::das_call_member< UObject * (FObjectPropertyBase::*)(const void *,int) const,&FObjectPropertyBase::LoadObjectPropertyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2595:23
	makeExtern<DAS_CALL_METHOD(_method_1113), SimNode_ExtFuncCall >(lib,"LoadObjectPropertyValue_InContainer","das_call_member< UObject * (FObjectPropertyBase::*)(const void *,int) const , &FObjectPropertyBase::LoadObjectPropertyValue_InContainer >::invoke")
		->args({"self","PropertyValueAddress","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2601:19
	makeExtern< UObject * (*)(const FObjectPropertyBase &,const void *) , _dasUnreal_virtual_462_GetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue","_dasUnreal_virtual_462_GetObjectPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2602:19
	makeExtern< UObject * (*)(const FObjectPropertyBase &,const void *,int) , _dasUnreal_virtual_463_GetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue_InContainer","_dasUnreal_virtual_463_GetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2603:15
	makeExtern< void (*)(const FObjectPropertyBase &,void *,UObject *) , _dasUnreal_virtual_464_SetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue","_dasUnreal_virtual_464_SetObjectPropertyValue")
		->args({"self","PropertyValueAddress","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2605:15
	makeExtern< void (*)(const FObjectPropertyBase &,void *,UObject *,int) , _dasUnreal_virtual_465_SetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue_InContainer","_dasUnreal_virtual_465_SetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","Value","ArrayIndex"})
		->arg_init(3,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1118 = das::das_call_member< void (FObjectPropertyBase::*)(UClass *),&FObjectPropertyBase::SetPropertyClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2617:7
	makeExtern<DAS_CALL_METHOD(_method_1118), SimNode_ExtFuncCall >(lib,"SetPropertyClass","das_call_member< void (FObjectPropertyBase::*)(UClass *) , &FObjectPropertyBase::SetPropertyClass >::invoke")
		->args({"self","NewPropertyClass"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStrProperty,EInternal,FFieldClass *>(*this,lib,"FStrProperty","FStrProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3566:2
	makeExtern< FFieldClass * (*)(const FStrProperty *) , _dasUnreal_static_466_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_466_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3566:2
	makeExtern< FField * (*)(const FStrProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_467_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_467_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FStrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3566:2
	makeExtern< uint64 (*)(const FStrProperty *) , _dasUnreal_static_468_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_468_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3566:2
	makeExtern< uint64 (*)(const FStrProperty *) , _dasUnreal_static_469_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_469_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStrProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FStrProperty","FStrProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FStrProperty,UField *>(*this,lib,"FStrProperty","FStrProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3604:18
	makeExtern< FString (*)(const FStrProperty &) , _dasUnreal_virtual_470_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_470_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3605:9
	makeExtern< unsigned int (*)(const FStrProperty &,const void *) , _dasUnreal_virtual_471_GetValueTypeHashInternal , SimNode_ExtFuncCall >(lib,"GetValueTypeHashInternal","_dasUnreal_virtual_471_GetValueTypeHashInternal")
		->args({"self","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStructProperty,EInternal,FFieldClass *>(*this,lib,"FStructProperty","FStructProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< FFieldClass * (*)(const FStructProperty *) , _dasUnreal_static_472_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_472_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< FField * (*)(const FStructProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_473_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_473_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< uint64 (*)(const FStructProperty *) , _dasUnreal_static_474_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_474_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< uint64 (*)(const FStructProperty *) , _dasUnreal_static_475_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_475_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStructProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FStructProperty","FStructProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FStructProperty,UField *>(*this,lib,"FStructProperty","FStructProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5812:15
	makeExtern< void (*)(FStructProperty &,FArchive &) , _dasUnreal_virtual_476_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_476_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5813:15
	makeExtern< void (*)(FStructProperty &,FReferenceCollector &) , _dasUnreal_virtual_477_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_477_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5818:15
	makeExtern< void (*)(FStructProperty &,const FField &) , _dasUnreal_virtual_478_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_478_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5821:18
	makeExtern< FString (*)(const FStructProperty &,FString &) , _dasUnreal_virtual_479_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_479_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5822:18
	makeExtern< FString (*)(const FStructProperty &,FString *,unsigned int) , _dasUnreal_virtual_480_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_480_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5823:18
	makeExtern< FString (*)(const FStructProperty &) , _dasUnreal_virtual_481_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_481_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5824:15
	makeExtern< void (*)(FStructProperty &,FArchive &) , _dasUnreal_virtual_482_LinkInternal , SimNode_ExtFuncCall >(lib,"LinkInternal","_dasUnreal_virtual_482_LinkInternal")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5825:15
	makeExtern< bool (*)(const FStructProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_483_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_483_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5828:15
	makeExtern< bool (*)(const FStructProperty &) , _dasUnreal_virtual_484_SupportsNetSharedSerialization , SimNode_ExtFuncCall >(lib,"SupportsNetSharedSerialization","_dasUnreal_virtual_484_SupportsNetSharedSerialization")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5833:15
	makeExtern< void (*)(const FStructProperty &,void *,const void *,int) , _dasUnreal_virtual_485_CopyValuesInternal , SimNode_ExtFuncCall >(lib,"CopyValuesInternal","_dasUnreal_virtual_485_CopyValuesInternal")
		->args({"self","Dest","Src","Count"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5834:15
	makeExtern< void (*)(const FStructProperty &,void *) , _dasUnreal_virtual_486_ClearValueInternal , SimNode_ExtFuncCall >(lib,"ClearValueInternal","_dasUnreal_virtual_486_ClearValueInternal")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5835:15
	makeExtern< void (*)(const FStructProperty &,void *) , _dasUnreal_virtual_487_DestroyValueInternal , SimNode_ExtFuncCall >(lib,"DestroyValueInternal","_dasUnreal_virtual_487_DestroyValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5836:15
	makeExtern< void (*)(const FStructProperty &,void *) , _dasUnreal_virtual_488_InitializeValueInternal , SimNode_ExtFuncCall >(lib,"InitializeValueInternal","_dasUnreal_virtual_488_InitializeValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5838:16
	makeExtern< int (*)(const FStructProperty &) , _dasUnreal_virtual_489_GetMinAlignment , SimNode_ExtFuncCall >(lib,"GetMinAlignment","_dasUnreal_virtual_489_GetMinAlignment")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5841:15
	makeExtern< bool (*)(const FStructProperty &,const FProperty *) , _dasUnreal_virtual_490_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_490_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5844:15
	makeExtern< void (*)(const FStructProperty &,FBlake3 &,bool) , _dasUnreal_virtual_491_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_491_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1145 = das::das_call_member< bool (FStructProperty::*)(const FArchive &) const,&FStructProperty::UseBinaryOrNativeSerialization >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5848:7
	makeExtern<DAS_CALL_METHOD(_method_1145), SimNode_ExtFuncCall >(lib,"UseBinaryOrNativeSerialization","das_call_member< bool (FStructProperty::*)(const FArchive &) const , &FStructProperty::UseBinaryOrNativeSerialization >::invoke")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1146 = das::das_call_member< bool (FStructProperty::*)(FName,const void *,const FProperty *&,const void *&) const,&FStructProperty::FindInnerPropertyInstance >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5849:7
	makeExtern<DAS_CALL_METHOD(_method_1146), SimNode_ExtFuncCall >(lib,"FindInnerPropertyInstance","das_call_member< bool (FStructProperty::*)(FName,const void *,const FProperty *&,const void *&) const , &FStructProperty::FindInnerPropertyInstance >::invoke")
		->args({"self","PropertyName","Data","OutProp","OutData"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FCustomPropertyListNode,FProperty *,int>(*this,lib,"FCustomPropertyListNode","FCustomPropertyListNode")
		->args({"InProperty","InArrayIndex"})
		->arg_init(0,make_smart<ExprConstPtr>())
		->arg_init(1,make_smart<ExprConstInt>(0));
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:6205:32
	makeExtern< FProperty * (*)(const FCustomPropertyListNode *,const FCustomPropertyListNode *&) , _dasUnreal_static_492_GetNextPropertyAndAdvance , SimNode_ExtFuncCall >(lib,"GetNextPropertyAndAdvance","_dasUnreal_static_492_GetNextPropertyAndAdvance")
		->args({"self","Node"})
		->arg_type(0,makeType<TExplicit<const FCustomPropertyListNode*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:6529:16
	makeExtern< UField * (*)(const UStruct *) , GetChildFieldsFromStruct , SimNode_ExtFuncCall >(lib,"GetChildFieldsFromStruct","GetChildFieldsFromStruct")
		->args({"Owner"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:6535:16
	makeExtern< FField * (*)(const UStruct *) , GetChildFieldsFromStruct , SimNode_ExtFuncCall >(lib,"GetChildFieldsFromStruct_FField","GetChildFieldsFromStruct")
		->args({"Owner"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:6777:22
	makeExtern< FFieldVariant (*)(const UStruct *,FName,EFieldIterationFlags) , FindUFieldOrFProperty , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FindUFieldOrFProperty","FindUFieldOrFProperty")
		->args({"Owner","FieldName","IterationFlags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(3,makeType<EFieldIterationFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:6789:22
	makeExtern< FFieldVariant (*)(const UStruct *,const wchar_t *,EFieldIterationFlags) , FindUFieldOrFProperty , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FindUFieldOrFProperty","FindUFieldOrFProperty")
		->args({"Owner","FieldName","IterationFlags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(3,makeType<EFieldIterationFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:7166:2
	makeExtern< UClass * (*)(const UPropertyWrapper *) , _dasUnreal_static_493_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_493_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPropertyWrapper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:7166:2
	makeExtern< const wchar_t * (*)(const UPropertyWrapper *) , _dasUnreal_static_494_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_494_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPropertyWrapper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:7166:2
	makeExtern< void (*)(const UPropertyWrapper *) , _dasUnreal_static_495_StaticRegisterNativesUPropertyWrapper , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUPropertyWrapper","_dasUnreal_static_495_StaticRegisterNativesUPropertyWrapper")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPropertyWrapper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1151 = das::das_call_member< void (UPropertyWrapper::*)(FProperty *),&UPropertyWrapper::SetProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:7173:7
	makeExtern<DAS_CALL_METHOD(_method_1151), SimNode_ExtFuncCall >(lib,"SetProperty","das_call_member< void (UPropertyWrapper::*)(FProperty *) , &UPropertyWrapper::SetProperty >::invoke")
		->args({"self","InProperty"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FFrame,UObject *,UFunction *,void *,FFrame *,FField *>(*this,lib,"FFrame","FFrame")
		->args({"InObject","InNode","InLocals","InPreviousFrame","InPropertyChainForCompiledIn"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->arg_init(4,make_smart<ExprConstPtr>());
	using _method_1152 = das::das_call_member< void (FFrame::*)(UObject *,void *const),&FFrame::Step >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:208:23
	makeExtern<DAS_CALL_METHOD(_method_1152), SimNode_ExtFuncCall >(lib,"Step","das_call_member< void (FFrame::*)(UObject *,void *const) , &FFrame::Step >::invoke")
		->args({"self","Context","Z_Param__Result"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1153 = das::das_call_member< bool (FFrame::*)(UObject *,void *const),&FFrame::StepAndCheckMostRecentProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:211:30
	makeExtern<DAS_CALL_METHOD(_method_1153), SimNode_ExtFuncCall >(lib,"StepAndCheckMostRecentProperty","das_call_member< bool (FFrame::*)(UObject *,void *const) , &FFrame::StepAndCheckMostRecentProperty >::invoke")
		->args({"self","Context","Z_Param__Result"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1154 = das::das_call_member< void (FFrame::*)(void *const,FProperty *),&FFrame::StepExplicitProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:214:23
	makeExtern<DAS_CALL_METHOD(_method_1154), SimNode_ExtFuncCall >(lib,"StepExplicitProperty","das_call_member< void (FFrame::*)(void *const,FProperty *) , &FFrame::StepExplicitProperty >::invoke")
		->args({"self","Result","Property"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1155 = das::das_call_member< void (FFrame::*)(void *,const FFieldClass *),&FFrame::StepCompiledIn >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:219:30
	makeExtern<DAS_CALL_METHOD(_method_1155), SimNode_ExtFuncCall >(lib,"StepCompiledIn","das_call_member< void (FFrame::*)(void *,const FFieldClass *) , &FFrame::StepCompiledIn >::invoke")
		->args({"self","Result","ExpectedPropertyType"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:225:31
	makeExtern< void (*)(FFrame &,const wchar_t *,ELogVerbosity::Type,const FName &) , _dasUnreal_virtual_496_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_496_Serialize")
		->args({"self","V","Verbosity","Category"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:227:30
	makeExtern< void (*)(const FFrame *,const wchar_t *,ELogVerbosity::Type,FName) , _dasUnreal_static_497_KismetExecutionMessage , SimNode_ExtFuncCall >(lib,"KismetExecutionMessage","_dasUnreal_static_497_KismetExecutionMessage")
		->args({"self","Message","Verbosity","WarningId"})
		->arg_type(0,makeType<TExplicit<const FFrame*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1158 = das::das_call_member< const unsigned char (FFrame::*)() const,&FFrame::PeekCode >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:230:14
	makeExtern<DAS_CALL_METHOD(_method_1158), SimNode_ExtFuncCall >(lib,"PeekCode","das_call_member< const unsigned char (FFrame::*)() const , &FFrame::PeekCode >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1159 = das::das_call_member< void (FFrame::*)(const int),&FFrame::SkipCode >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:233:7
	makeExtern<DAS_CALL_METHOD(_method_1159), SimNode_ExtFuncCall >(lib,"SkipCode","das_call_member< void (FFrame::*)(const int) , &FFrame::SkipCode >::invoke")
		->args({"self","NumOps"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1160 = das::das_call_member< float (FFrame::*)(),&FFrame::ReadFloat >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:239:8
	makeExtern<DAS_CALL_METHOD(_method_1160), SimNode_ExtFuncCall >(lib,"ReadFloat","das_call_member< float (FFrame::*)() , &FFrame::ReadFloat >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1161 = das::das_call_member< double (FFrame::*)(),&FFrame::ReadDouble >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:240:9
	makeExtern<DAS_CALL_METHOD(_method_1161), SimNode_ExtFuncCall >(lib,"ReadDouble","das_call_member< double (FFrame::*)() , &FFrame::ReadDouble >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1162 = das::das_call_member< FName (FFrame::*)(),&FFrame::ReadName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:241:8
	makeExtern<DAS_CALL_METHOD(_method_1162), SimNode_ExtFuncCallAndCopyOrMove >(lib,"ReadName","das_call_member< FName (FFrame::*)() , &FFrame::ReadName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1163 = das::das_call_member< UObject * (FFrame::*)(),&FFrame::ReadObject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:242:11
	makeExtern<DAS_CALL_METHOD(_method_1163), SimNode_ExtFuncCall >(lib,"ReadObject","das_call_member< UObject * (FFrame::*)() , &FFrame::ReadObject >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1164 = das::das_call_member< int (FFrame::*)(),&FFrame::ReadWord >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:243:8
	makeExtern<DAS_CALL_METHOD(_method_1164), SimNode_ExtFuncCall >(lib,"ReadWord","das_call_member< int (FFrame::*)() , &FFrame::ReadWord >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1165 = das::das_call_member< FProperty * (FFrame::*)(),&FFrame::ReadProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:244:13
	makeExtern<DAS_CALL_METHOD(_method_1165), SimNode_ExtFuncCall >(lib,"ReadProperty","das_call_member< FProperty * (FFrame::*)() , &FFrame::ReadProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1166 = das::das_call_member< FProperty * (FFrame::*)(),&FFrame::ReadPropertyUnchecked >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:247:13
	makeExtern<DAS_CALL_METHOD(_method_1166), SimNode_ExtFuncCall >(lib,"ReadPropertyUnchecked","das_call_member< FProperty * (FFrame::*)() , &FFrame::ReadPropertyUnchecked >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1167 = das::das_call_member< unsigned int (FFrame::*)(),&FFrame::ReadCodeSkipCount >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:256:19
	makeExtern<DAS_CALL_METHOD(_method_1167), SimNode_ExtFuncCall >(lib,"ReadCodeSkipCount","das_call_member< unsigned int (FFrame::*)() , &FFrame::ReadCodeSkipCount >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1168 = das::das_call_member< unsigned short (FFrame::*)(FProperty **),&FFrame::ReadVariableSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:265:19
	makeExtern<DAS_CALL_METHOD(_method_1168), SimNode_ExtFuncCall >(lib,"ReadVariableSize","das_call_member< unsigned short (FFrame::*)(FProperty **) , &FFrame::ReadVariableSize >::invoke")
		->args({"self","ExpressionField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:313:33
	makeExtern< FFrame * (*)(const FFrame *,FFrame *) , _dasUnreal_static_498_PushThreadLocalTopStackFrame , SimNode_ExtFuncCall >(lib,"PushThreadLocalTopStackFrame","_dasUnreal_static_498_PushThreadLocalTopStackFrame")
		->args({"self","NewTopStackFrame"})
		->arg_type(0,makeType<TExplicit<const FFrame*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:314:30
	makeExtern< void (*)(const FFrame *,FFrame *) , _dasUnreal_static_499_PopThreadLocalTopStackFrame , SimNode_ExtFuncCall >(lib,"PopThreadLocalTopStackFrame","_dasUnreal_static_499_PopThreadLocalTopStackFrame")
		->args({"self","NewTopStackFrame"})
		->arg_type(0,makeType<TExplicit<const FFrame*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:315:33
	makeExtern< FFrame * (*)(const FFrame *) , _dasUnreal_static_500_GetThreadLocalTopStackFrame , SimNode_ExtFuncCall >(lib,"GetThreadLocalTopStackFrame","_dasUnreal_static_500_GetThreadLocalTopStackFrame")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FFrame*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:464:22
	makeExtern< void (*)() , GInitRunaway , SimNode_ExtFuncCall >(lib,"GInitRunaway","GInitRunaway")
		->addToModule(*this, SideEffects::worstDefault);
}
}

