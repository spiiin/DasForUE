// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_13() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2406:15
	makeExtern< void (*)(const FBoolProperty &,void *) , _dasUnreal_virtual_392_ClearValueInternal , SimNode_ExtFuncCall >(lib,"ClearValueInternal","_dasUnreal_virtual_392_ClearValueInternal")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2407:15
	makeExtern< void (*)(const FBoolProperty &,void *) , _dasUnreal_virtual_393_InitializeValueInternal , SimNode_ExtFuncCall >(lib,"InitializeValueInternal","_dasUnreal_virtual_393_InitializeValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2408:16
	makeExtern< int (*)(const FBoolProperty &) , _dasUnreal_virtual_394_GetMinAlignment , SimNode_ExtFuncCall >(lib,"GetMinAlignment","_dasUnreal_virtual_394_GetMinAlignment")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2411:15
	makeExtern< void (*)(const FBoolProperty &,FBlake3 &,bool) , _dasUnreal_virtual_395_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_395_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_991 = das::das_call_member< bool (FBoolProperty::*)(const void *) const,&FBoolProperty::GetPropertyValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2418:19
	makeExtern<DAS_CALL_METHOD(_method_991), SimNode_ExtFuncCall >(lib,"GetPropertyValue","das_call_member< bool (FBoolProperty::*)(const void *) const , &FBoolProperty::GetPropertyValue >::invoke")
		->args({"self","A"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_992 = das::das_call_member< bool (FBoolProperty::*)(const void *,int) const,&FBoolProperty::GetPropertyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2424:19
	makeExtern<DAS_CALL_METHOD(_method_992), SimNode_ExtFuncCall >(lib,"GetPropertyValue_InContainer","das_call_member< bool (FBoolProperty::*)(const void *,int) const , &FBoolProperty::GetPropertyValue_InContainer >::invoke")
		->args({"self","A","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2428:26
	makeExtern< bool (*)(const FBoolProperty *) , _dasUnreal_static_396_GetDefaultPropertyValue , SimNode_ExtFuncCall >(lib,"GetDefaultPropertyValue","_dasUnreal_static_396_GetDefaultPropertyValue")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_994 = das::das_call_member< bool (FBoolProperty::*)(const void *) const,&FBoolProperty::GetOptionalPropertyValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2432:19
	makeExtern<DAS_CALL_METHOD(_method_994), SimNode_ExtFuncCall >(lib,"GetOptionalPropertyValue","das_call_member< bool (FBoolProperty::*)(const void *) const , &FBoolProperty::GetOptionalPropertyValue >::invoke")
		->args({"self","B"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_995 = das::das_call_member< bool (FBoolProperty::*)(const void *,int) const,&FBoolProperty::GetOptionalPropertyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2436:19
	makeExtern<DAS_CALL_METHOD(_method_995), SimNode_ExtFuncCall >(lib,"GetOptionalPropertyValue_InContainer","das_call_member< bool (FBoolProperty::*)(const void *,int) const , &FBoolProperty::GetOptionalPropertyValue_InContainer >::invoke")
		->args({"self","B","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_996 = das::das_call_member< void (FBoolProperty::*)(void *,bool) const,&FBoolProperty::SetPropertyValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2440:19
	makeExtern<DAS_CALL_METHOD(_method_996), SimNode_ExtFuncCall >(lib,"SetPropertyValue","das_call_member< void (FBoolProperty::*)(void *,bool) const , &FBoolProperty::SetPropertyValue >::invoke")
		->args({"self","A","Value"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_997 = das::das_call_member< void (FBoolProperty::*)(void *,bool,int) const,&FBoolProperty::SetPropertyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2446:19
	makeExtern<DAS_CALL_METHOD(_method_997), SimNode_ExtFuncCall >(lib,"SetPropertyValue_InContainer","das_call_member< void (FBoolProperty::*)(void *,bool,int) const , &FBoolProperty::SetPropertyValue_InContainer >::invoke")
		->args({"self","A","Value","ArrayIndex"})
		->arg_init(3,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_998 = das::das_call_member< void (FBoolProperty::*)(const unsigned int,const bool,const unsigned int),&FBoolProperty::SetBoolSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2459:7
	makeExtern<DAS_CALL_METHOD(_method_998), SimNode_ExtFuncCall >(lib,"SetBoolSize","das_call_member< void (FBoolProperty::*)(const unsigned int,const bool,const unsigned int) , &FBoolProperty::SetBoolSize >::invoke")
		->args({"self","InSize","bIsNativeBool","InBitMask"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->arg_init(3,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_999 = das::das_call_member< bool (FBoolProperty::*)() const,&FBoolProperty::IsNativeBool >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2464:19
	makeExtern<DAS_CALL_METHOD(_method_999), SimNode_ExtFuncCall >(lib,"IsNativeBool","das_call_member< bool (FBoolProperty::*)() const , &FBoolProperty::IsNativeBool >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1000 = das::das_call_member< unsigned char (FBoolProperty::*)() const,&FBoolProperty::GetFieldMask >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2470:20
	makeExtern<DAS_CALL_METHOD(_method_1000), SimNode_ExtFuncCall >(lib,"GetFieldMask","das_call_member< unsigned char (FBoolProperty::*)() const , &FBoolProperty::GetFieldMask >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1001 = das::das_call_member< unsigned char (FBoolProperty::*)() const,&FBoolProperty::GetByteOffset >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2476:20
	makeExtern<DAS_CALL_METHOD(_method_1001), SimNode_ExtFuncCall >(lib,"GetByteOffset","das_call_member< unsigned char (FBoolProperty::*)() const , &FBoolProperty::GetByteOffset >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2481:9
	makeExtern< unsigned int (*)(const FBoolProperty &,const void *) , _dasUnreal_virtual_397_GetValueTypeHashInternal , SimNode_ExtFuncCall >(lib,"GetValueTypeHashInternal","_dasUnreal_virtual_397_GetValueTypeHashInternal")
		->args({"self","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FObjectPropertyBase,EInternal,FFieldClass *>(*this,lib,"FObjectPropertyBase","FObjectPropertyBase")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2493:2
	makeExtern< FFieldClass * (*)(const FObjectPropertyBase *) , _dasUnreal_static_398_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_398_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2493:2
	makeExtern< FField * (*)(const FObjectPropertyBase *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_399_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_399_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2493:2
	makeExtern< uint64 (*)(const FObjectPropertyBase *) , _dasUnreal_static_400_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_400_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2493:2
	makeExtern< uint64 (*)(const FObjectPropertyBase *) , _dasUnreal_static_401_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_401_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FObjectPropertyBase,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FObjectPropertyBase","FObjectPropertyBase")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FObjectPropertyBase,UField *>(*this,lib,"FObjectPropertyBase","FObjectPropertyBase")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2527:15
	makeExtern< void (*)(FObjectPropertyBase &,FReferenceCollector &) , _dasUnreal_virtual_402_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_402_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2528:15
	makeExtern< void (*)(FObjectPropertyBase &) , _dasUnreal_virtual_403_BeginDestroy , SimNode_ExtFuncCall >(lib,"BeginDestroy","_dasUnreal_virtual_403_BeginDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2532:15
	makeExtern< void (*)(FObjectPropertyBase &,const FField &) , _dasUnreal_virtual_404_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_404_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2535:15
	makeExtern< bool (*)(const FObjectPropertyBase &,const void *,const void *,unsigned int) , _dasUnreal_virtual_405_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_405_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2537:15
	makeExtern< bool (*)(const FObjectPropertyBase &) , _dasUnreal_virtual_406_SupportsNetSharedSerialization , SimNode_ExtFuncCall >(lib,"SupportsNetSharedSerialization","_dasUnreal_virtual_406_SupportsNetSharedSerialization")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2542:16
	makeExtern< FName (*)(const FObjectPropertyBase &) , _dasUnreal_virtual_407_GetID , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetID","_dasUnreal_virtual_407_GetID")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2544:15
	makeExtern< bool (*)(const FObjectPropertyBase &,const FProperty *) , _dasUnreal_virtual_408_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_408_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2550:18
	makeExtern< FString (*)(const FObjectPropertyBase &,FString *,unsigned int,const FString &) , _dasUnreal_virtual_409_GetCPPTypeCustom , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeCustom","_dasUnreal_virtual_409_GetCPPTypeCustom")
		->args({"self","ExtendedTypeText","CPPExportFlags","InnerNativeTypeName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2589:14
	makeExtern< bool (*)(const FObjectPropertyBase *,UObject *,UObject *,unsigned int) , _dasUnreal_static_410_StaticIdentical , SimNode_ExtFuncCall >(lib,"StaticIdentical","_dasUnreal_static_410_StaticIdentical")
		->args({"self","A","B","PortFlags"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2591:19
	makeExtern< UObject * (*)(const FObjectPropertyBase &,const void *) , _dasUnreal_virtual_411_LoadObjectPropertyValue , SimNode_ExtFuncCall >(lib,"LoadObjectPropertyValue","_dasUnreal_virtual_411_LoadObjectPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1017 = das::das_call_member< UObject * (FObjectPropertyBase::*)(const void *,int) const,&FObjectPropertyBase::LoadObjectPropertyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2595:23
	makeExtern<DAS_CALL_METHOD(_method_1017), SimNode_ExtFuncCall >(lib,"LoadObjectPropertyValue_InContainer","das_call_member< UObject * (FObjectPropertyBase::*)(const void *,int) const , &FObjectPropertyBase::LoadObjectPropertyValue_InContainer >::invoke")
		->args({"self","PropertyValueAddress","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2601:19
	makeExtern< UObject * (*)(const FObjectPropertyBase &,const void *) , _dasUnreal_virtual_412_GetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue","_dasUnreal_virtual_412_GetObjectPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2602:19
	makeExtern< UObject * (*)(const FObjectPropertyBase &,const void *,int) , _dasUnreal_virtual_413_GetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue_InContainer","_dasUnreal_virtual_413_GetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2603:15
	makeExtern< void (*)(const FObjectPropertyBase &,void *,UObject *) , _dasUnreal_virtual_414_SetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue","_dasUnreal_virtual_414_SetObjectPropertyValue")
		->args({"self","PropertyValueAddress","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2605:15
	makeExtern< void (*)(const FObjectPropertyBase &,void *,UObject *,int) , _dasUnreal_virtual_415_SetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue_InContainer","_dasUnreal_virtual_415_SetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","Value","ArrayIndex"})
		->arg_init(3,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1022 = das::das_call_member< void (FObjectPropertyBase::*)(UClass *),&FObjectPropertyBase::SetPropertyClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2617:7
	makeExtern<DAS_CALL_METHOD(_method_1022), SimNode_ExtFuncCall >(lib,"SetPropertyClass","das_call_member< void (FObjectPropertyBase::*)(UClass *) , &FObjectPropertyBase::SetPropertyClass >::invoke")
		->args({"self","NewPropertyClass"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStructProperty,EInternal,FFieldClass *>(*this,lib,"FStructProperty","FStructProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< FFieldClass * (*)(const FStructProperty *) , _dasUnreal_static_416_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_416_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< FField * (*)(const FStructProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_417_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_417_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< uint64 (*)(const FStructProperty *) , _dasUnreal_static_418_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_418_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< uint64 (*)(const FStructProperty *) , _dasUnreal_static_419_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_419_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStructProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FStructProperty","FStructProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FStructProperty,FFieldVariant,const UECodeGen_Private::FStructPropertyParams &>(*this,lib,"FStructProperty","FStructProperty")
		->args({"InOwner","Prop"});
	addCtorAndUsing<FStructProperty,UField *>(*this,lib,"FStructProperty","FStructProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5813:15
	makeExtern< void (*)(FStructProperty &,FReferenceCollector &) , _dasUnreal_virtual_420_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_420_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5818:15
	makeExtern< void (*)(FStructProperty &,const FField &) , _dasUnreal_virtual_421_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_421_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5821:18
	makeExtern< FString (*)(const FStructProperty &,FString &) , _dasUnreal_virtual_422_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_422_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5822:18
	makeExtern< FString (*)(const FStructProperty &,FString *,unsigned int) , _dasUnreal_virtual_423_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_423_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5823:18
	makeExtern< FString (*)(const FStructProperty &) , _dasUnreal_virtual_424_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_424_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5825:15
	makeExtern< bool (*)(const FStructProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_425_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_425_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5828:15
	makeExtern< bool (*)(const FStructProperty &) , _dasUnreal_virtual_426_SupportsNetSharedSerialization , SimNode_ExtFuncCall >(lib,"SupportsNetSharedSerialization","_dasUnreal_virtual_426_SupportsNetSharedSerialization")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5833:15
	makeExtern< void (*)(const FStructProperty &,void *,const void *,int) , _dasUnreal_virtual_427_CopyValuesInternal , SimNode_ExtFuncCall >(lib,"CopyValuesInternal","_dasUnreal_virtual_427_CopyValuesInternal")
		->args({"self","Dest","Src","Count"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5834:15
	makeExtern< void (*)(const FStructProperty &,void *) , _dasUnreal_virtual_428_ClearValueInternal , SimNode_ExtFuncCall >(lib,"ClearValueInternal","_dasUnreal_virtual_428_ClearValueInternal")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5835:15
	makeExtern< void (*)(const FStructProperty &,void *) , _dasUnreal_virtual_429_DestroyValueInternal , SimNode_ExtFuncCall >(lib,"DestroyValueInternal","_dasUnreal_virtual_429_DestroyValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5836:15
	makeExtern< void (*)(const FStructProperty &,void *) , _dasUnreal_virtual_430_InitializeValueInternal , SimNode_ExtFuncCall >(lib,"InitializeValueInternal","_dasUnreal_virtual_430_InitializeValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5838:16
	makeExtern< int (*)(const FStructProperty &) , _dasUnreal_virtual_431_GetMinAlignment , SimNode_ExtFuncCall >(lib,"GetMinAlignment","_dasUnreal_virtual_431_GetMinAlignment")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5841:15
	makeExtern< bool (*)(const FStructProperty &,const FProperty *) , _dasUnreal_virtual_432_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_432_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5844:15
	makeExtern< void (*)(const FStructProperty &,FBlake3 &,bool) , _dasUnreal_virtual_433_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_433_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1041 = das::das_call_member< bool (FStructProperty::*)(FName,const void *,const FProperty *&,const void *&) const,&FStructProperty::FindInnerPropertyInstance >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5849:7
	makeExtern<DAS_CALL_METHOD(_method_1041), SimNode_ExtFuncCall >(lib,"FindInnerPropertyInstance","das_call_member< bool (FStructProperty::*)(FName,const void *,const FProperty *&,const void *&) const , &FStructProperty::FindInnerPropertyInstance >::invoke")
		->args({"self","PropertyName","Data","OutProp","OutData"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FCustomPropertyListNode,FProperty *,int>(*this,lib,"FCustomPropertyListNode","FCustomPropertyListNode")
		->args({"InProperty","InArrayIndex"})
		->arg_init(0,make_smart<ExprConstPtr>())
		->arg_init(1,make_smart<ExprConstInt>(0));
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:6205:32
	makeExtern< FProperty * (*)(const FCustomPropertyListNode *,const FCustomPropertyListNode *&) , _dasUnreal_static_434_GetNextPropertyAndAdvance , SimNode_ExtFuncCall >(lib,"GetNextPropertyAndAdvance","_dasUnreal_static_434_GetNextPropertyAndAdvance")
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
	makeExtern< UClass * (*)(const UPropertyWrapper *) , _dasUnreal_static_435_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_435_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPropertyWrapper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:7166:2
	makeExtern< const wchar_t * (*)(const UPropertyWrapper *) , _dasUnreal_static_436_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_436_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPropertyWrapper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:7166:2
	makeExtern< void (*)(const UPropertyWrapper *) , _dasUnreal_static_437_StaticRegisterNativesUPropertyWrapper , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUPropertyWrapper","_dasUnreal_static_437_StaticRegisterNativesUPropertyWrapper")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPropertyWrapper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1046 = das::das_call_member< void (UPropertyWrapper::*)(FProperty *),&UPropertyWrapper::SetProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:7173:7
	makeExtern<DAS_CALL_METHOD(_method_1046), SimNode_ExtFuncCall >(lib,"SetProperty","das_call_member< void (UPropertyWrapper::*)(FProperty *) , &UPropertyWrapper::SetProperty >::invoke")
		->args({"self","InProperty"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

