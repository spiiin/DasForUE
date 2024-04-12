// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_15() {
	using _method_1175 = das::das_call_member< int (FMapProperty::*)(void *) const,&FMapProperty::GetNum >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3881:8
	makeExtern<DAS_CALL_METHOD(_method_1175), SimNode_ExtFuncCall >(lib,"GetNum","das_call_member< int (FMapProperty::*)(void *) const , &FMapProperty::GetNum >::invoke")
		->args({"self","InMap"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1176 = das::das_call_member< int (FMapProperty::*)() const,&FMapProperty::GetPairStride >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3890:8
	makeExtern<DAS_CALL_METHOD(_method_1176), SimNode_ExtFuncCall >(lib,"GetPairStride","das_call_member< int (FMapProperty::*)() const , &FMapProperty::GetPairStride >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1177 = das::das_call_member< bool (FMapProperty::*)(void *,int) const,&FMapProperty::IsValidIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3899:7
	makeExtern<DAS_CALL_METHOD(_method_1177), SimNode_ExtFuncCall >(lib,"IsValidIndex","das_call_member< bool (FMapProperty::*)(void *,int) const , &FMapProperty::IsValidIndex >::invoke")
		->args({"self","InMap","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1178 = das::das_call_member< unsigned char * (FMapProperty::*)(void *,int) const,&FMapProperty::GetPairPtr >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3908:9
	makeExtern<DAS_CALL_METHOD(_method_1178), SimNode_ExtFuncCall >(lib,"GetPairPtr","das_call_member< unsigned char * (FMapProperty::*)(void *,int) const , &FMapProperty::GetPairPtr >::invoke")
		->args({"self","InMap","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1179 = das::das_call_member< const FProperty * (FMapProperty::*)() const,&FMapProperty::GetKeyProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3913:19
	makeExtern<DAS_CALL_METHOD(_method_1179), SimNode_ExtFuncCall >(lib,"GetKeyProperty","das_call_member< const FProperty * (FMapProperty::*)() const , &FMapProperty::GetKeyProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1180 = das::das_call_member< const FProperty * (FMapProperty::*)() const,&FMapProperty::GetValueProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3918:19
	makeExtern<DAS_CALL_METHOD(_method_1180), SimNode_ExtFuncCall >(lib,"GetValueProperty","das_call_member< const FProperty * (FMapProperty::*)() const , &FMapProperty::GetValueProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3923:15
	makeExtern< void (*)(const FMapProperty &,FBlake3 &,bool) , _dasUnreal_virtual_518_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_518_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FSetProperty,EInternal,FFieldClass *>(*this,lib,"FSetProperty","FSetProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3932:2
	makeExtern< FFieldClass * (*)(const FSetProperty *) , _dasUnreal_static_519_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_519_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FSetProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3932:2
	makeExtern< FField * (*)(const FSetProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_520_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_520_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FSetProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3932:2
	makeExtern< uint64 (*)(const FSetProperty *) , _dasUnreal_static_521_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_521_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FSetProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3932:2
	makeExtern< uint64 (*)(const FSetProperty *) , _dasUnreal_static_522_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_522_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FSetProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FSetProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FSetProperty","FSetProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FSetProperty,UField *>(*this,lib,"FSetProperty","FSetProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3961:15
	makeExtern< void (*)(FSetProperty &,FArchive &) , _dasUnreal_virtual_523_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_523_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3962:15
	makeExtern< void (*)(FSetProperty &,FReferenceCollector &) , _dasUnreal_virtual_524_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_524_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3967:15
	makeExtern< void (*)(FSetProperty &,const FField &) , _dasUnreal_virtual_525_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_525_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3968:18
	makeExtern< FField * (*)(FSetProperty &,const FName &) , _dasUnreal_virtual_526_GetInnerFieldByName , SimNode_ExtFuncCall >(lib,"GetInnerFieldByName","_dasUnreal_virtual_526_GetInnerFieldByName")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3972:15
	makeExtern< void (*)(FSetProperty &,FProperty *) , _dasUnreal_virtual_527_AddCppProperty , SimNode_ExtFuncCall >(lib,"AddCppProperty","_dasUnreal_virtual_527_AddCppProperty")
		->args({"self","Property"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3976:18
	makeExtern< FString (*)(const FSetProperty &,FString &) , _dasUnreal_virtual_528_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_528_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3977:18
	makeExtern< FString (*)(const FSetProperty &,FString *,unsigned int) , _dasUnreal_virtual_529_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_529_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3978:18
	makeExtern< FString (*)(const FSetProperty &) , _dasUnreal_virtual_530_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_530_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3979:15
	makeExtern< void (*)(FSetProperty &,FArchive &) , _dasUnreal_virtual_531_LinkInternal , SimNode_ExtFuncCall >(lib,"LinkInternal","_dasUnreal_virtual_531_LinkInternal")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3980:15
	makeExtern< bool (*)(const FSetProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_532_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_532_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3987:15
	makeExtern< void (*)(const FSetProperty &,void *,const void *,int) , _dasUnreal_virtual_533_CopyValuesInternal , SimNode_ExtFuncCall >(lib,"CopyValuesInternal","_dasUnreal_virtual_533_CopyValuesInternal")
		->args({"self","Dest","Src","Count"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3988:15
	makeExtern< void (*)(const FSetProperty &,void *) , _dasUnreal_virtual_534_ClearValueInternal , SimNode_ExtFuncCall >(lib,"ClearValueInternal","_dasUnreal_virtual_534_ClearValueInternal")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3989:15
	makeExtern< void (*)(const FSetProperty &,void *) , _dasUnreal_virtual_535_DestroyValueInternal , SimNode_ExtFuncCall >(lib,"DestroyValueInternal","_dasUnreal_virtual_535_DestroyValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3990:15
	makeExtern< bool (*)(const FSetProperty &) , _dasUnreal_virtual_536_PassCPPArgsByRef , SimNode_ExtFuncCall >(lib,"PassCPPArgsByRef","_dasUnreal_virtual_536_PassCPPArgsByRef")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3994:15
	makeExtern< bool (*)(const FSetProperty &,const FProperty *) , _dasUnreal_virtual_537_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_537_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3996:16
	makeExtern< void * (*)(const FSetProperty &,const FProperty *,void *,int) , _dasUnreal_virtual_538_GetValueAddressAtIndex_Direct , SimNode_ExtFuncCall >(lib,"GetValueAddressAtIndex_Direct","_dasUnreal_virtual_538_GetValueAddressAtIndex_Direct")
		->args({"self","Inner","InValueAddress","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1202 = das::das_call_member< FString (FSetProperty::*)(FString *,unsigned int,const FString &,const FString &) const,&FSetProperty::GetCPPTypeCustom >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3999:10
	makeExtern<DAS_CALL_METHOD(_method_1202), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeCustom","das_call_member< FString (FSetProperty::*)(FString *,unsigned int,const FString &,const FString &) const , &FSetProperty::GetCPPTypeCustom >::invoke")
		->args({"self","ExtendedTypeText","CPPExportFlags","ElementTypeText","InElementExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1203 = das::das_call_member< int (FSetProperty::*)(void *) const,&FSetProperty::GetNum >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:4005:8
	makeExtern<DAS_CALL_METHOD(_method_1203), SimNode_ExtFuncCall >(lib,"GetNum","das_call_member< int (FSetProperty::*)(void *) const , &FSetProperty::GetNum >::invoke")
		->args({"self","InSet"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1204 = das::das_call_member< int (FSetProperty::*)() const,&FSetProperty::GetStride >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:4015:8
	makeExtern<DAS_CALL_METHOD(_method_1204), SimNode_ExtFuncCall >(lib,"GetStride","das_call_member< int (FSetProperty::*)() const , &FSetProperty::GetStride >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1205 = das::das_call_member< bool (FSetProperty::*)(void *,int) const,&FSetProperty::IsValidIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:4024:7
	makeExtern<DAS_CALL_METHOD(_method_1205), SimNode_ExtFuncCall >(lib,"IsValidIndex","das_call_member< bool (FSetProperty::*)(void *,int) const , &FSetProperty::IsValidIndex >::invoke")
		->args({"self","InSet","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1206 = das::das_call_member< unsigned char * (FSetProperty::*)(void *,int) const,&FSetProperty::GetElementPtr >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:4034:9
	makeExtern<DAS_CALL_METHOD(_method_1206), SimNode_ExtFuncCall >(lib,"GetElementPtr","das_call_member< unsigned char * (FSetProperty::*)(void *,int) const , &FSetProperty::GetElementPtr >::invoke")
		->args({"self","InSet","Index"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:4040:15
	makeExtern< void (*)(const FSetProperty &,FBlake3 &,bool) , _dasUnreal_virtual_539_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_539_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStructProperty,EInternal,FFieldClass *>(*this,lib,"FStructProperty","FStructProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< FFieldClass * (*)(const FStructProperty *) , _dasUnreal_static_540_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_540_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< FField * (*)(const FStructProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_541_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_541_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< uint64 (*)(const FStructProperty *) , _dasUnreal_static_542_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_542_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< uint64 (*)(const FStructProperty *) , _dasUnreal_static_543_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_543_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStructProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FStructProperty","FStructProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FStructProperty,UField *>(*this,lib,"FStructProperty","FStructProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5812:15
	makeExtern< void (*)(FStructProperty &,FArchive &) , _dasUnreal_virtual_544_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_544_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5813:15
	makeExtern< void (*)(FStructProperty &,FReferenceCollector &) , _dasUnreal_virtual_545_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_545_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5818:15
	makeExtern< void (*)(FStructProperty &,const FField &) , _dasUnreal_virtual_546_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_546_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5821:18
	makeExtern< FString (*)(const FStructProperty &,FString &) , _dasUnreal_virtual_547_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_547_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5822:18
	makeExtern< FString (*)(const FStructProperty &,FString *,unsigned int) , _dasUnreal_virtual_548_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_548_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5823:18
	makeExtern< FString (*)(const FStructProperty &) , _dasUnreal_virtual_549_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_549_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5824:15
	makeExtern< void (*)(FStructProperty &,FArchive &) , _dasUnreal_virtual_550_LinkInternal , SimNode_ExtFuncCall >(lib,"LinkInternal","_dasUnreal_virtual_550_LinkInternal")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5825:15
	makeExtern< bool (*)(const FStructProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_551_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_551_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5828:15
	makeExtern< bool (*)(const FStructProperty &) , _dasUnreal_virtual_552_SupportsNetSharedSerialization , SimNode_ExtFuncCall >(lib,"SupportsNetSharedSerialization","_dasUnreal_virtual_552_SupportsNetSharedSerialization")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5833:15
	makeExtern< void (*)(const FStructProperty &,void *,const void *,int) , _dasUnreal_virtual_553_CopyValuesInternal , SimNode_ExtFuncCall >(lib,"CopyValuesInternal","_dasUnreal_virtual_553_CopyValuesInternal")
		->args({"self","Dest","Src","Count"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5834:15
	makeExtern< void (*)(const FStructProperty &,void *) , _dasUnreal_virtual_554_ClearValueInternal , SimNode_ExtFuncCall >(lib,"ClearValueInternal","_dasUnreal_virtual_554_ClearValueInternal")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5835:15
	makeExtern< void (*)(const FStructProperty &,void *) , _dasUnreal_virtual_555_DestroyValueInternal , SimNode_ExtFuncCall >(lib,"DestroyValueInternal","_dasUnreal_virtual_555_DestroyValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5836:15
	makeExtern< void (*)(const FStructProperty &,void *) , _dasUnreal_virtual_556_InitializeValueInternal , SimNode_ExtFuncCall >(lib,"InitializeValueInternal","_dasUnreal_virtual_556_InitializeValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5838:16
	makeExtern< int (*)(const FStructProperty &) , _dasUnreal_virtual_557_GetMinAlignment , SimNode_ExtFuncCall >(lib,"GetMinAlignment","_dasUnreal_virtual_557_GetMinAlignment")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5841:15
	makeExtern< bool (*)(const FStructProperty &,const FProperty *) , _dasUnreal_virtual_558_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_558_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5844:15
	makeExtern< void (*)(const FStructProperty &,FBlake3 &,bool) , _dasUnreal_virtual_559_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_559_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1228 = das::das_call_member< bool (FStructProperty::*)(const FArchive &) const,&FStructProperty::UseBinaryOrNativeSerialization >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5848:7
	makeExtern<DAS_CALL_METHOD(_method_1228), SimNode_ExtFuncCall >(lib,"UseBinaryOrNativeSerialization","das_call_member< bool (FStructProperty::*)(const FArchive &) const , &FStructProperty::UseBinaryOrNativeSerialization >::invoke")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1229 = das::das_call_member< bool (FStructProperty::*)(FName,const void *,const FProperty *&,const void *&) const,&FStructProperty::FindInnerPropertyInstance >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5849:7
	makeExtern<DAS_CALL_METHOD(_method_1229), SimNode_ExtFuncCall >(lib,"FindInnerPropertyInstance","das_call_member< bool (FStructProperty::*)(FName,const void *,const FProperty *&,const void *&) const , &FStructProperty::FindInnerPropertyInstance >::invoke")
		->args({"self","PropertyName","Data","OutProp","OutData"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FCustomPropertyListNode,FProperty *,int>(*this,lib,"FCustomPropertyListNode","FCustomPropertyListNode")
		->args({"InProperty","InArrayIndex"})
		->arg_init(0,make_smart<ExprConstPtr>())
		->arg_init(1,make_smart<ExprConstInt>(0));
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:6205:32
	makeExtern< FProperty * (*)(const FCustomPropertyListNode *,const FCustomPropertyListNode *&) , _dasUnreal_static_560_GetNextPropertyAndAdvance , SimNode_ExtFuncCall >(lib,"GetNextPropertyAndAdvance","_dasUnreal_static_560_GetNextPropertyAndAdvance")
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
	makeExtern< UClass * (*)(const UPropertyWrapper *) , _dasUnreal_static_561_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_561_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPropertyWrapper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:7166:2
	makeExtern< const wchar_t * (*)(const UPropertyWrapper *) , _dasUnreal_static_562_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_562_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPropertyWrapper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:7166:2
	makeExtern< void (*)(const UPropertyWrapper *) , _dasUnreal_static_563_StaticRegisterNativesUPropertyWrapper , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUPropertyWrapper","_dasUnreal_static_563_StaticRegisterNativesUPropertyWrapper")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPropertyWrapper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1234 = das::das_call_member< void (UPropertyWrapper::*)(FProperty *),&UPropertyWrapper::SetProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:7173:7
	makeExtern<DAS_CALL_METHOD(_method_1234), SimNode_ExtFuncCall >(lib,"SetProperty","das_call_member< void (UPropertyWrapper::*)(FProperty *) , &UPropertyWrapper::SetProperty >::invoke")
		->args({"self","InProperty"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FFrame,UObject *,UFunction *,void *,FFrame *,FField *>(*this,lib,"FFrame","FFrame")
		->args({"InObject","InNode","InLocals","InPreviousFrame","InPropertyChainForCompiledIn"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->arg_init(4,make_smart<ExprConstPtr>());
	using _method_1235 = das::das_call_member< void (FFrame::*)(UObject *,void *const),&FFrame::Step >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:208:23
	makeExtern<DAS_CALL_METHOD(_method_1235), SimNode_ExtFuncCall >(lib,"Step","das_call_member< void (FFrame::*)(UObject *,void *const) , &FFrame::Step >::invoke")
		->args({"self","Context","Z_Param__Result"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1236 = das::das_call_member< bool (FFrame::*)(UObject *,void *const),&FFrame::StepAndCheckMostRecentProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:211:30
	makeExtern<DAS_CALL_METHOD(_method_1236), SimNode_ExtFuncCall >(lib,"StepAndCheckMostRecentProperty","das_call_member< bool (FFrame::*)(UObject *,void *const) , &FFrame::StepAndCheckMostRecentProperty >::invoke")
		->args({"self","Context","Z_Param__Result"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1237 = das::das_call_member< void (FFrame::*)(void *const,FProperty *),&FFrame::StepExplicitProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:214:23
	makeExtern<DAS_CALL_METHOD(_method_1237), SimNode_ExtFuncCall >(lib,"StepExplicitProperty","das_call_member< void (FFrame::*)(void *const,FProperty *) , &FFrame::StepExplicitProperty >::invoke")
		->args({"self","Result","Property"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1238 = das::das_call_member< void (FFrame::*)(void *,const FFieldClass *),&FFrame::StepCompiledIn >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:219:30
	makeExtern<DAS_CALL_METHOD(_method_1238), SimNode_ExtFuncCall >(lib,"StepCompiledIn","das_call_member< void (FFrame::*)(void *,const FFieldClass *) , &FFrame::StepCompiledIn >::invoke")
		->args({"self","Result","ExpectedPropertyType"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:225:31
	makeExtern< void (*)(FFrame &,const wchar_t *,ELogVerbosity::Type,const FName &) , _dasUnreal_virtual_564_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_564_Serialize")
		->args({"self","V","Verbosity","Category"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:227:30
	makeExtern< void (*)(const FFrame *,const wchar_t *,ELogVerbosity::Type,FName) , _dasUnreal_static_565_KismetExecutionMessage , SimNode_ExtFuncCall >(lib,"KismetExecutionMessage","_dasUnreal_static_565_KismetExecutionMessage")
		->args({"self","Message","Verbosity","WarningId"})
		->arg_type(0,makeType<TExplicit<const FFrame*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1241 = das::das_call_member< const unsigned char (FFrame::*)() const,&FFrame::PeekCode >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:230:14
	makeExtern<DAS_CALL_METHOD(_method_1241), SimNode_ExtFuncCall >(lib,"PeekCode","das_call_member< const unsigned char (FFrame::*)() const , &FFrame::PeekCode >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1242 = das::das_call_member< void (FFrame::*)(const int),&FFrame::SkipCode >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:233:7
	makeExtern<DAS_CALL_METHOD(_method_1242), SimNode_ExtFuncCall >(lib,"SkipCode","das_call_member< void (FFrame::*)(const int) , &FFrame::SkipCode >::invoke")
		->args({"self","NumOps"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1243 = das::das_call_member< float (FFrame::*)(),&FFrame::ReadFloat >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:239:8
	makeExtern<DAS_CALL_METHOD(_method_1243), SimNode_ExtFuncCall >(lib,"ReadFloat","das_call_member< float (FFrame::*)() , &FFrame::ReadFloat >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1244 = das::das_call_member< double (FFrame::*)(),&FFrame::ReadDouble >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:240:9
	makeExtern<DAS_CALL_METHOD(_method_1244), SimNode_ExtFuncCall >(lib,"ReadDouble","das_call_member< double (FFrame::*)() , &FFrame::ReadDouble >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1245 = das::das_call_member< FName (FFrame::*)(),&FFrame::ReadName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:241:8
	makeExtern<DAS_CALL_METHOD(_method_1245), SimNode_ExtFuncCallAndCopyOrMove >(lib,"ReadName","das_call_member< FName (FFrame::*)() , &FFrame::ReadName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1246 = das::das_call_member< UObject * (FFrame::*)(),&FFrame::ReadObject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:242:11
	makeExtern<DAS_CALL_METHOD(_method_1246), SimNode_ExtFuncCall >(lib,"ReadObject","das_call_member< UObject * (FFrame::*)() , &FFrame::ReadObject >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1247 = das::das_call_member< int (FFrame::*)(),&FFrame::ReadWord >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:243:8
	makeExtern<DAS_CALL_METHOD(_method_1247), SimNode_ExtFuncCall >(lib,"ReadWord","das_call_member< int (FFrame::*)() , &FFrame::ReadWord >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1248 = das::das_call_member< FProperty * (FFrame::*)(),&FFrame::ReadProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:244:13
	makeExtern<DAS_CALL_METHOD(_method_1248), SimNode_ExtFuncCall >(lib,"ReadProperty","das_call_member< FProperty * (FFrame::*)() , &FFrame::ReadProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1249 = das::das_call_member< FProperty * (FFrame::*)(),&FFrame::ReadPropertyUnchecked >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:247:13
	makeExtern<DAS_CALL_METHOD(_method_1249), SimNode_ExtFuncCall >(lib,"ReadPropertyUnchecked","das_call_member< FProperty * (FFrame::*)() , &FFrame::ReadPropertyUnchecked >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1250 = das::das_call_member< unsigned int (FFrame::*)(),&FFrame::ReadCodeSkipCount >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:256:19
	makeExtern<DAS_CALL_METHOD(_method_1250), SimNode_ExtFuncCall >(lib,"ReadCodeSkipCount","das_call_member< unsigned int (FFrame::*)() , &FFrame::ReadCodeSkipCount >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1251 = das::das_call_member< unsigned short (FFrame::*)(FProperty **),&FFrame::ReadVariableSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:265:19
	makeExtern<DAS_CALL_METHOD(_method_1251), SimNode_ExtFuncCall >(lib,"ReadVariableSize","das_call_member< unsigned short (FFrame::*)(FProperty **) , &FFrame::ReadVariableSize >::invoke")
		->args({"self","ExpressionField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:313:33
	makeExtern< FFrame * (*)(const FFrame *,FFrame *) , _dasUnreal_static_566_PushThreadLocalTopStackFrame , SimNode_ExtFuncCall >(lib,"PushThreadLocalTopStackFrame","_dasUnreal_static_566_PushThreadLocalTopStackFrame")
		->args({"self","NewTopStackFrame"})
		->arg_type(0,makeType<TExplicit<const FFrame*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:314:30
	makeExtern< void (*)(const FFrame *,FFrame *) , _dasUnreal_static_567_PopThreadLocalTopStackFrame , SimNode_ExtFuncCall >(lib,"PopThreadLocalTopStackFrame","_dasUnreal_static_567_PopThreadLocalTopStackFrame")
		->args({"self","NewTopStackFrame"})
		->arg_type(0,makeType<TExplicit<const FFrame*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:315:33
	makeExtern< FFrame * (*)(const FFrame *) , _dasUnreal_static_568_GetThreadLocalTopStackFrame , SimNode_ExtFuncCall >(lib,"GetThreadLocalTopStackFrame","_dasUnreal_static_568_GetThreadLocalTopStackFrame")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FFrame*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:464:22
	makeExtern< void (*)() , GInitRunaway , SimNode_ExtFuncCall >(lib,"GInitRunaway","GInitRunaway")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/BlueprintFunctionLibrary.h:17:2
	makeExtern< UClass * (*)(const UBlueprintFunctionLibrary *) , _dasUnreal_static_569_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_569_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UBlueprintFunctionLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/BlueprintFunctionLibrary.h:17:2
	makeExtern< const wchar_t * (*)(const UBlueprintFunctionLibrary *) , _dasUnreal_static_570_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_570_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UBlueprintFunctionLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/BlueprintFunctionLibrary.h:20:27
	makeExtern< int (*)(UBlueprintFunctionLibrary &,UFunction *,FFrame *) , _dasUnreal_virtual_571_GetFunctionCallspace , SimNode_ExtFuncCall >(lib,"GetFunctionCallspace","_dasUnreal_virtual_571_GetFunctionCallspace")
		->args({"self","Function","Stack"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:21:2
	makeExtern< UClass * (*)(const UKismetArrayLibrary *) , _dasUnreal_static_572_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_572_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:21:2
	makeExtern< const wchar_t * (*)(const UKismetArrayLibrary *) , _dasUnreal_static_573_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_573_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:270:26
	makeExtern< int (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,const void *) , _dasUnreal_static_574_GenericArray_Add , SimNode_ExtFuncCall >(lib,"GenericArray_Add","_dasUnreal_static_574_GenericArray_Add")
		->args({"self","TargetArray","ArrayProp","NewItem"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:271:26
	makeExtern< int (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,const void *) , _dasUnreal_static_575_GenericArray_AddUnique , SimNode_ExtFuncCall >(lib,"GenericArray_AddUnique","_dasUnreal_static_575_GenericArray_AddUnique")
		->args({"self","TargetArray","ArrayProp","NewItem"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
}
}

