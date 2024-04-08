// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_14() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2251:2
	makeExtern< uint64 (*)(const FFloatProperty *) , _dasUnreal_static_426_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_426_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FFloatProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2251:2
	makeExtern< uint64 (*)(const FFloatProperty *) , _dasUnreal_static_427_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_427_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FFloatProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FFloatProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FFloatProperty","FFloatProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FFloatProperty,FFieldVariant,const UECodeGen_Private::FGenericPropertyParams &>(*this,lib,"FFloatProperty","FFloatProperty")
		->args({"InOwner","Prop"});
	addCtorAndUsing<FFloatProperty,UField *>(*this,lib,"FFloatProperty","FFloatProperty")
		->args({"InField"});
	addCtorAndUsing<FDoubleProperty,EInternal,FFieldClass *>(*this,lib,"FDoubleProperty","FDoubleProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2290:2
	makeExtern< FFieldClass * (*)(const FDoubleProperty *) , _dasUnreal_static_428_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_428_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FDoubleProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2290:2
	makeExtern< FField * (*)(const FDoubleProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_429_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_429_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FDoubleProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2290:2
	makeExtern< uint64 (*)(const FDoubleProperty *) , _dasUnreal_static_430_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_430_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FDoubleProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2290:2
	makeExtern< uint64 (*)(const FDoubleProperty *) , _dasUnreal_static_431_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_431_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FDoubleProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FDoubleProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FDoubleProperty","FDoubleProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FDoubleProperty,FFieldVariant,const UECodeGen_Private::FGenericPropertyParams &>(*this,lib,"FDoubleProperty","FDoubleProperty")
		->args({"InOwner","Prop"});
	addCtorAndUsing<FDoubleProperty,UField *>(*this,lib,"FDoubleProperty","FDoubleProperty")
		->args({"InField"});
	addCtorAndUsing<FBoolProperty,EInternal,FFieldClass *>(*this,lib,"FBoolProperty","FBoolProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2340:2
	makeExtern< FFieldClass * (*)(const FBoolProperty *) , _dasUnreal_static_432_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_432_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2340:2
	makeExtern< FField * (*)(const FBoolProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_433_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_433_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2340:2
	makeExtern< uint64 (*)(const FBoolProperty *) , _dasUnreal_static_434_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_434_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2340:2
	makeExtern< uint64 (*)(const FBoolProperty *) , _dasUnreal_static_435_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_435_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FBoolProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FBoolProperty","FBoolProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FBoolProperty,FFieldVariant,const UECodeGen_Private::FBoolPropertyParams &>(*this,lib,"FBoolProperty","FBoolProperty")
		->args({"InOwner","Prop"});
	addCtorAndUsing<FBoolProperty,UField *>(*this,lib,"FBoolProperty","FBoolProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2384:15
	makeExtern< void (*)(FBoolProperty &,FArchive &) , _dasUnreal_virtual_436_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_436_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2388:15
	makeExtern< void (*)(FBoolProperty &,const FField &) , _dasUnreal_virtual_437_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_437_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2391:18
	makeExtern< FString (*)(const FBoolProperty &,FString *,unsigned int) , _dasUnreal_virtual_438_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_438_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2392:18
	makeExtern< FString (*)(const FBoolProperty &,FString &) , _dasUnreal_virtual_439_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_439_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2393:18
	makeExtern< FString (*)(const FBoolProperty &) , _dasUnreal_virtual_440_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_440_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2397:15
	makeExtern< void (*)(FBoolProperty &,FArchive &) , _dasUnreal_virtual_441_LinkInternal , SimNode_ExtFuncCall >(lib,"LinkInternal","_dasUnreal_virtual_441_LinkInternal")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2398:15
	makeExtern< bool (*)(const FBoolProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_442_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_442_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2405:15
	makeExtern< void (*)(const FBoolProperty &,void *,const void *,int) , _dasUnreal_virtual_443_CopyValuesInternal , SimNode_ExtFuncCall >(lib,"CopyValuesInternal","_dasUnreal_virtual_443_CopyValuesInternal")
		->args({"self","Dest","Src","Count"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2406:15
	makeExtern< void (*)(const FBoolProperty &,void *) , _dasUnreal_virtual_444_ClearValueInternal , SimNode_ExtFuncCall >(lib,"ClearValueInternal","_dasUnreal_virtual_444_ClearValueInternal")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2407:15
	makeExtern< void (*)(const FBoolProperty &,void *) , _dasUnreal_virtual_445_InitializeValueInternal , SimNode_ExtFuncCall >(lib,"InitializeValueInternal","_dasUnreal_virtual_445_InitializeValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2408:16
	makeExtern< int (*)(const FBoolProperty &) , _dasUnreal_virtual_446_GetMinAlignment , SimNode_ExtFuncCall >(lib,"GetMinAlignment","_dasUnreal_virtual_446_GetMinAlignment")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2411:15
	makeExtern< void (*)(const FBoolProperty &,FBlake3 &,bool) , _dasUnreal_virtual_447_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_447_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1098 = das::das_call_member< bool (FBoolProperty::*)(const void *) const,&FBoolProperty::GetPropertyValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2418:19
	makeExtern<DAS_CALL_METHOD(_method_1098), SimNode_ExtFuncCall >(lib,"GetPropertyValue","das_call_member< bool (FBoolProperty::*)(const void *) const , &FBoolProperty::GetPropertyValue >::invoke")
		->args({"self","A"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1099 = das::das_call_member< bool (FBoolProperty::*)(const void *,int) const,&FBoolProperty::GetPropertyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2424:19
	makeExtern<DAS_CALL_METHOD(_method_1099), SimNode_ExtFuncCall >(lib,"GetPropertyValue_InContainer","das_call_member< bool (FBoolProperty::*)(const void *,int) const , &FBoolProperty::GetPropertyValue_InContainer >::invoke")
		->args({"self","A","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2428:26
	makeExtern< bool (*)(const FBoolProperty *) , _dasUnreal_static_448_GetDefaultPropertyValue , SimNode_ExtFuncCall >(lib,"GetDefaultPropertyValue","_dasUnreal_static_448_GetDefaultPropertyValue")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1101 = das::das_call_member< bool (FBoolProperty::*)(const void *) const,&FBoolProperty::GetOptionalPropertyValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2432:19
	makeExtern<DAS_CALL_METHOD(_method_1101), SimNode_ExtFuncCall >(lib,"GetOptionalPropertyValue","das_call_member< bool (FBoolProperty::*)(const void *) const , &FBoolProperty::GetOptionalPropertyValue >::invoke")
		->args({"self","B"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1102 = das::das_call_member< bool (FBoolProperty::*)(const void *,int) const,&FBoolProperty::GetOptionalPropertyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2436:19
	makeExtern<DAS_CALL_METHOD(_method_1102), SimNode_ExtFuncCall >(lib,"GetOptionalPropertyValue_InContainer","das_call_member< bool (FBoolProperty::*)(const void *,int) const , &FBoolProperty::GetOptionalPropertyValue_InContainer >::invoke")
		->args({"self","B","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1103 = das::das_call_member< void (FBoolProperty::*)(void *,bool) const,&FBoolProperty::SetPropertyValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2440:19
	makeExtern<DAS_CALL_METHOD(_method_1103), SimNode_ExtFuncCall >(lib,"SetPropertyValue","das_call_member< void (FBoolProperty::*)(void *,bool) const , &FBoolProperty::SetPropertyValue >::invoke")
		->args({"self","A","Value"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1104 = das::das_call_member< void (FBoolProperty::*)(void *,bool,int) const,&FBoolProperty::SetPropertyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2446:19
	makeExtern<DAS_CALL_METHOD(_method_1104), SimNode_ExtFuncCall >(lib,"SetPropertyValue_InContainer","das_call_member< void (FBoolProperty::*)(void *,bool,int) const , &FBoolProperty::SetPropertyValue_InContainer >::invoke")
		->args({"self","A","Value","ArrayIndex"})
		->arg_init(3,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1105 = das::das_call_member< void (FBoolProperty::*)(const unsigned int,const bool,const unsigned int),&FBoolProperty::SetBoolSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2459:7
	makeExtern<DAS_CALL_METHOD(_method_1105), SimNode_ExtFuncCall >(lib,"SetBoolSize","das_call_member< void (FBoolProperty::*)(const unsigned int,const bool,const unsigned int) , &FBoolProperty::SetBoolSize >::invoke")
		->args({"self","InSize","bIsNativeBool","InBitMask"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->arg_init(3,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1106 = das::das_call_member< bool (FBoolProperty::*)() const,&FBoolProperty::IsNativeBool >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2464:19
	makeExtern<DAS_CALL_METHOD(_method_1106), SimNode_ExtFuncCall >(lib,"IsNativeBool","das_call_member< bool (FBoolProperty::*)() const , &FBoolProperty::IsNativeBool >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1107 = das::das_call_member< unsigned char (FBoolProperty::*)() const,&FBoolProperty::GetFieldMask >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2470:20
	makeExtern<DAS_CALL_METHOD(_method_1107), SimNode_ExtFuncCall >(lib,"GetFieldMask","das_call_member< unsigned char (FBoolProperty::*)() const , &FBoolProperty::GetFieldMask >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1108 = das::das_call_member< unsigned char (FBoolProperty::*)() const,&FBoolProperty::GetByteOffset >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2476:20
	makeExtern<DAS_CALL_METHOD(_method_1108), SimNode_ExtFuncCall >(lib,"GetByteOffset","das_call_member< unsigned char (FBoolProperty::*)() const , &FBoolProperty::GetByteOffset >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2481:9
	makeExtern< unsigned int (*)(const FBoolProperty &,const void *) , _dasUnreal_virtual_449_GetValueTypeHashInternal , SimNode_ExtFuncCall >(lib,"GetValueTypeHashInternal","_dasUnreal_virtual_449_GetValueTypeHashInternal")
		->args({"self","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FObjectPropertyBase,EInternal,FFieldClass *>(*this,lib,"FObjectPropertyBase","FObjectPropertyBase")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2493:2
	makeExtern< FFieldClass * (*)(const FObjectPropertyBase *) , _dasUnreal_static_450_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_450_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2493:2
	makeExtern< FField * (*)(const FObjectPropertyBase *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_451_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_451_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2493:2
	makeExtern< uint64 (*)(const FObjectPropertyBase *) , _dasUnreal_static_452_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_452_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2493:2
	makeExtern< uint64 (*)(const FObjectPropertyBase *) , _dasUnreal_static_453_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_453_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FObjectPropertyBase,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FObjectPropertyBase","FObjectPropertyBase")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FObjectPropertyBase,UField *>(*this,lib,"FObjectPropertyBase","FObjectPropertyBase")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2526:15
	makeExtern< void (*)(FObjectPropertyBase &,FArchive &) , _dasUnreal_virtual_454_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_454_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2527:15
	makeExtern< void (*)(FObjectPropertyBase &,FReferenceCollector &) , _dasUnreal_virtual_455_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_455_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2528:15
	makeExtern< void (*)(FObjectPropertyBase &) , _dasUnreal_virtual_456_BeginDestroy , SimNode_ExtFuncCall >(lib,"BeginDestroy","_dasUnreal_virtual_456_BeginDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2532:15
	makeExtern< void (*)(FObjectPropertyBase &,const FField &) , _dasUnreal_virtual_457_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_457_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2535:15
	makeExtern< bool (*)(const FObjectPropertyBase &,const void *,const void *,unsigned int) , _dasUnreal_virtual_458_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_458_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2537:15
	makeExtern< bool (*)(const FObjectPropertyBase &) , _dasUnreal_virtual_459_SupportsNetSharedSerialization , SimNode_ExtFuncCall >(lib,"SupportsNetSharedSerialization","_dasUnreal_virtual_459_SupportsNetSharedSerialization")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2542:16
	makeExtern< FName (*)(const FObjectPropertyBase &) , _dasUnreal_virtual_460_GetID , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetID","_dasUnreal_virtual_460_GetID")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2544:15
	makeExtern< bool (*)(const FObjectPropertyBase &,const FProperty *) , _dasUnreal_virtual_461_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_461_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2550:18
	makeExtern< FString (*)(const FObjectPropertyBase &,FString *,unsigned int,const FString &) , _dasUnreal_virtual_462_GetCPPTypeCustom , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeCustom","_dasUnreal_virtual_462_GetCPPTypeCustom")
		->args({"self","ExtendedTypeText","CPPExportFlags","InnerNativeTypeName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2589:14
	makeExtern< bool (*)(const FObjectPropertyBase *,UObject *,UObject *,unsigned int) , _dasUnreal_static_463_StaticIdentical , SimNode_ExtFuncCall >(lib,"StaticIdentical","_dasUnreal_static_463_StaticIdentical")
		->args({"self","A","B","PortFlags"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2591:19
	makeExtern< UObject * (*)(const FObjectPropertyBase &,const void *) , _dasUnreal_virtual_464_LoadObjectPropertyValue , SimNode_ExtFuncCall >(lib,"LoadObjectPropertyValue","_dasUnreal_virtual_464_LoadObjectPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1125 = das::das_call_member< UObject * (FObjectPropertyBase::*)(const void *,int) const,&FObjectPropertyBase::LoadObjectPropertyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2595:23
	makeExtern<DAS_CALL_METHOD(_method_1125), SimNode_ExtFuncCall >(lib,"LoadObjectPropertyValue_InContainer","das_call_member< UObject * (FObjectPropertyBase::*)(const void *,int) const , &FObjectPropertyBase::LoadObjectPropertyValue_InContainer >::invoke")
		->args({"self","PropertyValueAddress","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2601:19
	makeExtern< UObject * (*)(const FObjectPropertyBase &,const void *) , _dasUnreal_virtual_465_GetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue","_dasUnreal_virtual_465_GetObjectPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2602:19
	makeExtern< UObject * (*)(const FObjectPropertyBase &,const void *,int) , _dasUnreal_virtual_466_GetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue_InContainer","_dasUnreal_virtual_466_GetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2603:15
	makeExtern< void (*)(const FObjectPropertyBase &,void *,UObject *) , _dasUnreal_virtual_467_SetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue","_dasUnreal_virtual_467_SetObjectPropertyValue")
		->args({"self","PropertyValueAddress","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2605:15
	makeExtern< void (*)(const FObjectPropertyBase &,void *,UObject *,int) , _dasUnreal_virtual_468_SetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue_InContainer","_dasUnreal_virtual_468_SetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","Value","ArrayIndex"})
		->arg_init(3,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1130 = das::das_call_member< void (FObjectPropertyBase::*)(UClass *),&FObjectPropertyBase::SetPropertyClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2617:7
	makeExtern<DAS_CALL_METHOD(_method_1130), SimNode_ExtFuncCall >(lib,"SetPropertyClass","das_call_member< void (FObjectPropertyBase::*)(UClass *) , &FObjectPropertyBase::SetPropertyClass >::invoke")
		->args({"self","NewPropertyClass"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStrProperty,EInternal,FFieldClass *>(*this,lib,"FStrProperty","FStrProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3566:2
	makeExtern< FFieldClass * (*)(const FStrProperty *) , _dasUnreal_static_469_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_469_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3566:2
	makeExtern< FField * (*)(const FStrProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_470_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_470_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FStrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3566:2
	makeExtern< uint64 (*)(const FStrProperty *) , _dasUnreal_static_471_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_471_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3566:2
	makeExtern< uint64 (*)(const FStrProperty *) , _dasUnreal_static_472_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_472_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStrProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FStrProperty","FStrProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FStrProperty,FFieldVariant,const UECodeGen_Private::FGenericPropertyParams &>(*this,lib,"FStrProperty","FStrProperty")
		->args({"InOwner","Prop"});
	addCtorAndUsing<FStrProperty,UField *>(*this,lib,"FStrProperty","FStrProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3604:18
	makeExtern< FString (*)(const FStrProperty &) , _dasUnreal_virtual_473_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_473_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3605:9
	makeExtern< unsigned int (*)(const FStrProperty &,const void *) , _dasUnreal_virtual_474_GetValueTypeHashInternal , SimNode_ExtFuncCall >(lib,"GetValueTypeHashInternal","_dasUnreal_virtual_474_GetValueTypeHashInternal")
		->args({"self","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStructProperty,EInternal,FFieldClass *>(*this,lib,"FStructProperty","FStructProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< FFieldClass * (*)(const FStructProperty *) , _dasUnreal_static_475_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_475_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< FField * (*)(const FStructProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_476_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_476_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< uint64 (*)(const FStructProperty *) , _dasUnreal_static_477_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_477_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< uint64 (*)(const FStructProperty *) , _dasUnreal_static_478_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_478_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStructProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FStructProperty","FStructProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FStructProperty,FFieldVariant,const UECodeGen_Private::FStructPropertyParams &>(*this,lib,"FStructProperty","FStructProperty")
		->args({"InOwner","Prop"});
	addCtorAndUsing<FStructProperty,UField *>(*this,lib,"FStructProperty","FStructProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5812:15
	makeExtern< void (*)(FStructProperty &,FArchive &) , _dasUnreal_virtual_479_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_479_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5813:15
	makeExtern< void (*)(FStructProperty &,FReferenceCollector &) , _dasUnreal_virtual_480_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_480_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5818:15
	makeExtern< void (*)(FStructProperty &,const FField &) , _dasUnreal_virtual_481_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_481_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5821:18
	makeExtern< FString (*)(const FStructProperty &,FString &) , _dasUnreal_virtual_482_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_482_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5822:18
	makeExtern< FString (*)(const FStructProperty &,FString *,unsigned int) , _dasUnreal_virtual_483_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_483_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5823:18
	makeExtern< FString (*)(const FStructProperty &) , _dasUnreal_virtual_484_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_484_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5824:15
	makeExtern< void (*)(FStructProperty &,FArchive &) , _dasUnreal_virtual_485_LinkInternal , SimNode_ExtFuncCall >(lib,"LinkInternal","_dasUnreal_virtual_485_LinkInternal")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5825:15
	makeExtern< bool (*)(const FStructProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_486_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_486_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5828:15
	makeExtern< bool (*)(const FStructProperty &) , _dasUnreal_virtual_487_SupportsNetSharedSerialization , SimNode_ExtFuncCall >(lib,"SupportsNetSharedSerialization","_dasUnreal_virtual_487_SupportsNetSharedSerialization")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5833:15
	makeExtern< void (*)(const FStructProperty &,void *,const void *,int) , _dasUnreal_virtual_488_CopyValuesInternal , SimNode_ExtFuncCall >(lib,"CopyValuesInternal","_dasUnreal_virtual_488_CopyValuesInternal")
		->args({"self","Dest","Src","Count"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5834:15
	makeExtern< void (*)(const FStructProperty &,void *) , _dasUnreal_virtual_489_ClearValueInternal , SimNode_ExtFuncCall >(lib,"ClearValueInternal","_dasUnreal_virtual_489_ClearValueInternal")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5835:15
	makeExtern< void (*)(const FStructProperty &,void *) , _dasUnreal_virtual_490_DestroyValueInternal , SimNode_ExtFuncCall >(lib,"DestroyValueInternal","_dasUnreal_virtual_490_DestroyValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5836:15
	makeExtern< void (*)(const FStructProperty &,void *) , _dasUnreal_virtual_491_InitializeValueInternal , SimNode_ExtFuncCall >(lib,"InitializeValueInternal","_dasUnreal_virtual_491_InitializeValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

