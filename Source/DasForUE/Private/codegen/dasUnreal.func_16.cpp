// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_16() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3834:18
	makeExtern< FString (*)(const FMapProperty &,FString *,unsigned int) , _dasUnreal_virtual_587_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_587_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3835:18
	makeExtern< FString (*)(const FMapProperty &) , _dasUnreal_virtual_588_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_588_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3836:15
	makeExtern< void (*)(FMapProperty &,FArchive &) , _dasUnreal_virtual_589_LinkInternal , SimNode_ExtFuncCall >(lib,"LinkInternal","_dasUnreal_virtual_589_LinkInternal")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3837:15
	makeExtern< bool (*)(const FMapProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_590_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_590_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3844:15
	makeExtern< void (*)(const FMapProperty &,void *) , _dasUnreal_virtual_591_InitializeValueInternal , SimNode_ExtFuncCall >(lib,"InitializeValueInternal","_dasUnreal_virtual_591_InitializeValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3863:15
	makeExtern< void (*)(const FMapProperty &,void *,const void *,int) , _dasUnreal_virtual_592_CopyValuesInternal , SimNode_ExtFuncCall >(lib,"CopyValuesInternal","_dasUnreal_virtual_592_CopyValuesInternal")
		->args({"self","Dest","Src","Count"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3864:15
	makeExtern< void (*)(const FMapProperty &,void *) , _dasUnreal_virtual_593_ClearValueInternal , SimNode_ExtFuncCall >(lib,"ClearValueInternal","_dasUnreal_virtual_593_ClearValueInternal")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3865:15
	makeExtern< void (*)(const FMapProperty &,void *) , _dasUnreal_virtual_594_DestroyValueInternal , SimNode_ExtFuncCall >(lib,"DestroyValueInternal","_dasUnreal_virtual_594_DestroyValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3866:15
	makeExtern< bool (*)(const FMapProperty &) , _dasUnreal_virtual_595_PassCPPArgsByRef , SimNode_ExtFuncCall >(lib,"PassCPPArgsByRef","_dasUnreal_virtual_595_PassCPPArgsByRef")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3870:15
	makeExtern< bool (*)(const FMapProperty &,const FProperty *) , _dasUnreal_virtual_596_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_596_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3872:16
	makeExtern< void * (*)(const FMapProperty &,const FProperty *,void *,int) , _dasUnreal_virtual_597_GetValueAddressAtIndex_Direct , SimNode_ExtFuncCall >(lib,"GetValueAddressAtIndex_Direct","_dasUnreal_virtual_597_GetValueAddressAtIndex_Direct")
		->args({"self","Inner","InValueAddress","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1272 = das::das_call_member< FString (FMapProperty::*)(FString *,unsigned int,const FString &,const FString &,const FString &,const FString &) const,&FMapProperty::GetCPPTypeCustom >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3875:10
	makeExtern<DAS_CALL_METHOD(_method_1272), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeCustom","das_call_member< FString (FMapProperty::*)(FString *,unsigned int,const FString &,const FString &,const FString &,const FString &) const , &FMapProperty::GetCPPTypeCustom >::invoke")
		->args({"self","ExtendedTypeText","CPPExportFlags","KeyTypeText","InKeyExtendedTypeText","ValueTypeText","InValueExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1273 = das::das_call_member< int (FMapProperty::*)(void *) const,&FMapProperty::GetNum >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3881:8
	makeExtern<DAS_CALL_METHOD(_method_1273), SimNode_ExtFuncCall >(lib,"GetNum","das_call_member< int (FMapProperty::*)(void *) const , &FMapProperty::GetNum >::invoke")
		->args({"self","InMap"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1274 = das::das_call_member< int (FMapProperty::*)() const,&FMapProperty::GetPairStride >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3890:8
	makeExtern<DAS_CALL_METHOD(_method_1274), SimNode_ExtFuncCall >(lib,"GetPairStride","das_call_member< int (FMapProperty::*)() const , &FMapProperty::GetPairStride >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1275 = das::das_call_member< bool (FMapProperty::*)(void *,int) const,&FMapProperty::IsValidIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3899:7
	makeExtern<DAS_CALL_METHOD(_method_1275), SimNode_ExtFuncCall >(lib,"IsValidIndex","das_call_member< bool (FMapProperty::*)(void *,int) const , &FMapProperty::IsValidIndex >::invoke")
		->args({"self","InMap","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1276 = das::das_call_member< unsigned char * (FMapProperty::*)(void *,int) const,&FMapProperty::GetPairPtr >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3908:9
	makeExtern<DAS_CALL_METHOD(_method_1276), SimNode_ExtFuncCall >(lib,"GetPairPtr","das_call_member< unsigned char * (FMapProperty::*)(void *,int) const , &FMapProperty::GetPairPtr >::invoke")
		->args({"self","InMap","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1277 = das::das_call_member< const FProperty * (FMapProperty::*)() const,&FMapProperty::GetKeyProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3913:19
	makeExtern<DAS_CALL_METHOD(_method_1277), SimNode_ExtFuncCall >(lib,"GetKeyProperty","das_call_member< const FProperty * (FMapProperty::*)() const , &FMapProperty::GetKeyProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1278 = das::das_call_member< const FProperty * (FMapProperty::*)() const,&FMapProperty::GetValueProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3918:19
	makeExtern<DAS_CALL_METHOD(_method_1278), SimNode_ExtFuncCall >(lib,"GetValueProperty","das_call_member< const FProperty * (FMapProperty::*)() const , &FMapProperty::GetValueProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3923:15
	makeExtern< void (*)(const FMapProperty &,FBlake3 &,bool) , _dasUnreal_virtual_598_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_598_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FSetProperty,EInternal,FFieldClass *>(*this,lib,"FSetProperty","FSetProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3932:2
	makeExtern< FFieldClass * (*)(const FSetProperty *) , _dasUnreal_static_599_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_599_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FSetProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3932:2
	makeExtern< FField * (*)(const FSetProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_600_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_600_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FSetProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3932:2
	makeExtern< uint64 (*)(const FSetProperty *) , _dasUnreal_static_601_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_601_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FSetProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3932:2
	makeExtern< uint64 (*)(const FSetProperty *) , _dasUnreal_static_602_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_602_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FSetProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FSetProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FSetProperty","FSetProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FSetProperty,UField *>(*this,lib,"FSetProperty","FSetProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3961:15
	makeExtern< void (*)(FSetProperty &,FArchive &) , _dasUnreal_virtual_603_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_603_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3962:15
	makeExtern< void (*)(FSetProperty &,FReferenceCollector &) , _dasUnreal_virtual_604_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_604_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3967:15
	makeExtern< void (*)(FSetProperty &,const FField &) , _dasUnreal_virtual_605_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_605_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3968:18
	makeExtern< FField * (*)(FSetProperty &,const FName &) , _dasUnreal_virtual_606_GetInnerFieldByName , SimNode_ExtFuncCall >(lib,"GetInnerFieldByName","_dasUnreal_virtual_606_GetInnerFieldByName")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3972:15
	makeExtern< void (*)(FSetProperty &,FProperty *) , _dasUnreal_virtual_607_AddCppProperty , SimNode_ExtFuncCall >(lib,"AddCppProperty","_dasUnreal_virtual_607_AddCppProperty")
		->args({"self","Property"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3976:18
	makeExtern< FString (*)(const FSetProperty &,FString &) , _dasUnreal_virtual_608_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_608_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3977:18
	makeExtern< FString (*)(const FSetProperty &,FString *,unsigned int) , _dasUnreal_virtual_609_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_609_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3978:18
	makeExtern< FString (*)(const FSetProperty &) , _dasUnreal_virtual_610_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_610_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3979:15
	makeExtern< void (*)(FSetProperty &,FArchive &) , _dasUnreal_virtual_611_LinkInternal , SimNode_ExtFuncCall >(lib,"LinkInternal","_dasUnreal_virtual_611_LinkInternal")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3980:15
	makeExtern< bool (*)(const FSetProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_612_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_612_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3987:15
	makeExtern< void (*)(const FSetProperty &,void *,const void *,int) , _dasUnreal_virtual_613_CopyValuesInternal , SimNode_ExtFuncCall >(lib,"CopyValuesInternal","_dasUnreal_virtual_613_CopyValuesInternal")
		->args({"self","Dest","Src","Count"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3988:15
	makeExtern< void (*)(const FSetProperty &,void *) , _dasUnreal_virtual_614_ClearValueInternal , SimNode_ExtFuncCall >(lib,"ClearValueInternal","_dasUnreal_virtual_614_ClearValueInternal")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3989:15
	makeExtern< void (*)(const FSetProperty &,void *) , _dasUnreal_virtual_615_DestroyValueInternal , SimNode_ExtFuncCall >(lib,"DestroyValueInternal","_dasUnreal_virtual_615_DestroyValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3990:15
	makeExtern< bool (*)(const FSetProperty &) , _dasUnreal_virtual_616_PassCPPArgsByRef , SimNode_ExtFuncCall >(lib,"PassCPPArgsByRef","_dasUnreal_virtual_616_PassCPPArgsByRef")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3994:15
	makeExtern< bool (*)(const FSetProperty &,const FProperty *) , _dasUnreal_virtual_617_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_617_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3996:16
	makeExtern< void * (*)(const FSetProperty &,const FProperty *,void *,int) , _dasUnreal_virtual_618_GetValueAddressAtIndex_Direct , SimNode_ExtFuncCall >(lib,"GetValueAddressAtIndex_Direct","_dasUnreal_virtual_618_GetValueAddressAtIndex_Direct")
		->args({"self","Inner","InValueAddress","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1300 = das::das_call_member< FString (FSetProperty::*)(FString *,unsigned int,const FString &,const FString &) const,&FSetProperty::GetCPPTypeCustom >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3999:10
	makeExtern<DAS_CALL_METHOD(_method_1300), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeCustom","das_call_member< FString (FSetProperty::*)(FString *,unsigned int,const FString &,const FString &) const , &FSetProperty::GetCPPTypeCustom >::invoke")
		->args({"self","ExtendedTypeText","CPPExportFlags","ElementTypeText","InElementExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1301 = das::das_call_member< int (FSetProperty::*)(void *) const,&FSetProperty::GetNum >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:4005:8
	makeExtern<DAS_CALL_METHOD(_method_1301), SimNode_ExtFuncCall >(lib,"GetNum","das_call_member< int (FSetProperty::*)(void *) const , &FSetProperty::GetNum >::invoke")
		->args({"self","InSet"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1302 = das::das_call_member< int (FSetProperty::*)() const,&FSetProperty::GetStride >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:4015:8
	makeExtern<DAS_CALL_METHOD(_method_1302), SimNode_ExtFuncCall >(lib,"GetStride","das_call_member< int (FSetProperty::*)() const , &FSetProperty::GetStride >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1303 = das::das_call_member< bool (FSetProperty::*)(void *,int) const,&FSetProperty::IsValidIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:4024:7
	makeExtern<DAS_CALL_METHOD(_method_1303), SimNode_ExtFuncCall >(lib,"IsValidIndex","das_call_member< bool (FSetProperty::*)(void *,int) const , &FSetProperty::IsValidIndex >::invoke")
		->args({"self","InSet","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1304 = das::das_call_member< unsigned char * (FSetProperty::*)(void *,int) const,&FSetProperty::GetElementPtr >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:4034:9
	makeExtern<DAS_CALL_METHOD(_method_1304), SimNode_ExtFuncCall >(lib,"GetElementPtr","das_call_member< unsigned char * (FSetProperty::*)(void *,int) const , &FSetProperty::GetElementPtr >::invoke")
		->args({"self","InSet","Index"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:4040:15
	makeExtern< void (*)(const FSetProperty &,FBlake3 &,bool) , _dasUnreal_virtual_619_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_619_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStructProperty,EInternal,FFieldClass *>(*this,lib,"FStructProperty","FStructProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< FFieldClass * (*)(const FStructProperty *) , _dasUnreal_static_620_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_620_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< FField * (*)(const FStructProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_621_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_621_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< uint64 (*)(const FStructProperty *) , _dasUnreal_static_622_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_622_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< uint64 (*)(const FStructProperty *) , _dasUnreal_static_623_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_623_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStructProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FStructProperty","FStructProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FStructProperty,UField *>(*this,lib,"FStructProperty","FStructProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5812:15
	makeExtern< void (*)(FStructProperty &,FArchive &) , _dasUnreal_virtual_624_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_624_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5813:15
	makeExtern< void (*)(FStructProperty &,FReferenceCollector &) , _dasUnreal_virtual_625_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_625_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5818:15
	makeExtern< void (*)(FStructProperty &,const FField &) , _dasUnreal_virtual_626_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_626_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5821:18
	makeExtern< FString (*)(const FStructProperty &,FString &) , _dasUnreal_virtual_627_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_627_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5822:18
	makeExtern< FString (*)(const FStructProperty &,FString *,unsigned int) , _dasUnreal_virtual_628_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_628_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5823:18
	makeExtern< FString (*)(const FStructProperty &) , _dasUnreal_virtual_629_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_629_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5824:15
	makeExtern< void (*)(FStructProperty &,FArchive &) , _dasUnreal_virtual_630_LinkInternal , SimNode_ExtFuncCall >(lib,"LinkInternal","_dasUnreal_virtual_630_LinkInternal")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5825:15
	makeExtern< bool (*)(const FStructProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_631_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_631_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5828:15
	makeExtern< bool (*)(const FStructProperty &) , _dasUnreal_virtual_632_SupportsNetSharedSerialization , SimNode_ExtFuncCall >(lib,"SupportsNetSharedSerialization","_dasUnreal_virtual_632_SupportsNetSharedSerialization")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5833:15
	makeExtern< void (*)(const FStructProperty &,void *,const void *,int) , _dasUnreal_virtual_633_CopyValuesInternal , SimNode_ExtFuncCall >(lib,"CopyValuesInternal","_dasUnreal_virtual_633_CopyValuesInternal")
		->args({"self","Dest","Src","Count"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5834:15
	makeExtern< void (*)(const FStructProperty &,void *) , _dasUnreal_virtual_634_ClearValueInternal , SimNode_ExtFuncCall >(lib,"ClearValueInternal","_dasUnreal_virtual_634_ClearValueInternal")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5835:15
	makeExtern< void (*)(const FStructProperty &,void *) , _dasUnreal_virtual_635_DestroyValueInternal , SimNode_ExtFuncCall >(lib,"DestroyValueInternal","_dasUnreal_virtual_635_DestroyValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5836:15
	makeExtern< void (*)(const FStructProperty &,void *) , _dasUnreal_virtual_636_InitializeValueInternal , SimNode_ExtFuncCall >(lib,"InitializeValueInternal","_dasUnreal_virtual_636_InitializeValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5838:16
	makeExtern< int (*)(const FStructProperty &) , _dasUnreal_virtual_637_GetMinAlignment , SimNode_ExtFuncCall >(lib,"GetMinAlignment","_dasUnreal_virtual_637_GetMinAlignment")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5841:15
	makeExtern< bool (*)(const FStructProperty &,const FProperty *) , _dasUnreal_virtual_638_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_638_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5844:15
	makeExtern< void (*)(const FStructProperty &,FBlake3 &,bool) , _dasUnreal_virtual_639_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_639_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1326 = das::das_call_member< bool (FStructProperty::*)(const FArchive &) const,&FStructProperty::UseBinaryOrNativeSerialization >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5848:7
	makeExtern<DAS_CALL_METHOD(_method_1326), SimNode_ExtFuncCall >(lib,"UseBinaryOrNativeSerialization","das_call_member< bool (FStructProperty::*)(const FArchive &) const , &FStructProperty::UseBinaryOrNativeSerialization >::invoke")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1327 = das::das_call_member< bool (FStructProperty::*)(FName,const void *,const FProperty *&,const void *&) const,&FStructProperty::FindInnerPropertyInstance >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5849:7
	makeExtern<DAS_CALL_METHOD(_method_1327), SimNode_ExtFuncCall >(lib,"FindInnerPropertyInstance","das_call_member< bool (FStructProperty::*)(FName,const void *,const FProperty *&,const void *&) const , &FStructProperty::FindInnerPropertyInstance >::invoke")
		->args({"self","PropertyName","Data","OutProp","OutData"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FCustomPropertyListNode,FProperty *,int>(*this,lib,"FCustomPropertyListNode","FCustomPropertyListNode")
		->args({"InProperty","InArrayIndex"})
		->arg_init(0,make_smart<ExprConstPtr>())
		->arg_init(1,make_smart<ExprConstInt>(0));
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:6205:32
	makeExtern< FProperty * (*)(const FCustomPropertyListNode *,const FCustomPropertyListNode *&) , _dasUnreal_static_640_GetNextPropertyAndAdvance , SimNode_ExtFuncCall >(lib,"GetNextPropertyAndAdvance","_dasUnreal_static_640_GetNextPropertyAndAdvance")
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
	makeExtern< UClass * (*)(const UPropertyWrapper *) , _dasUnreal_static_641_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_641_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPropertyWrapper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:7166:2
	makeExtern< const wchar_t * (*)(const UPropertyWrapper *) , _dasUnreal_static_642_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_642_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPropertyWrapper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:7166:2
	makeExtern< void (*)(const UPropertyWrapper *) , _dasUnreal_static_643_StaticRegisterNativesUPropertyWrapper , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUPropertyWrapper","_dasUnreal_static_643_StaticRegisterNativesUPropertyWrapper")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPropertyWrapper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1332 = das::das_call_member< void (UPropertyWrapper::*)(FProperty *),&UPropertyWrapper::SetProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:7173:7
	makeExtern<DAS_CALL_METHOD(_method_1332), SimNode_ExtFuncCall >(lib,"SetProperty","das_call_member< void (UPropertyWrapper::*)(FProperty *) , &UPropertyWrapper::SetProperty >::invoke")
		->args({"self","InProperty"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FFrame,UObject *,UFunction *,void *,FFrame *,FField *>(*this,lib,"FFrame","FFrame")
		->args({"InObject","InNode","InLocals","InPreviousFrame","InPropertyChainForCompiledIn"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->arg_init(4,make_smart<ExprConstPtr>());
	using _method_1333 = das::das_call_member< void (FFrame::*)(UObject *,void *const),&FFrame::Step >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:208:23
	makeExtern<DAS_CALL_METHOD(_method_1333), SimNode_ExtFuncCall >(lib,"Step","das_call_member< void (FFrame::*)(UObject *,void *const) , &FFrame::Step >::invoke")
		->args({"self","Context","Z_Param__Result"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1334 = das::das_call_member< bool (FFrame::*)(UObject *,void *const),&FFrame::StepAndCheckMostRecentProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:211:30
	makeExtern<DAS_CALL_METHOD(_method_1334), SimNode_ExtFuncCall >(lib,"StepAndCheckMostRecentProperty","das_call_member< bool (FFrame::*)(UObject *,void *const) , &FFrame::StepAndCheckMostRecentProperty >::invoke")
		->args({"self","Context","Z_Param__Result"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1335 = das::das_call_member< void (FFrame::*)(void *const,FProperty *),&FFrame::StepExplicitProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:214:23
	makeExtern<DAS_CALL_METHOD(_method_1335), SimNode_ExtFuncCall >(lib,"StepExplicitProperty","das_call_member< void (FFrame::*)(void *const,FProperty *) , &FFrame::StepExplicitProperty >::invoke")
		->args({"self","Result","Property"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1336 = das::das_call_member< void (FFrame::*)(void *,const FFieldClass *),&FFrame::StepCompiledIn >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:219:30
	makeExtern<DAS_CALL_METHOD(_method_1336), SimNode_ExtFuncCall >(lib,"StepCompiledIn","das_call_member< void (FFrame::*)(void *,const FFieldClass *) , &FFrame::StepCompiledIn >::invoke")
		->args({"self","Result","ExpectedPropertyType"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:225:31
	makeExtern< void (*)(FFrame &,const wchar_t *,ELogVerbosity::Type,const FName &) , _dasUnreal_virtual_644_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_644_Serialize")
		->args({"self","V","Verbosity","Category"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:227:30
	makeExtern< void (*)(const FFrame *,const wchar_t *,ELogVerbosity::Type,FName) , _dasUnreal_static_645_KismetExecutionMessage , SimNode_ExtFuncCall >(lib,"KismetExecutionMessage","_dasUnreal_static_645_KismetExecutionMessage")
		->args({"self","Message","Verbosity","WarningId"})
		->arg_type(0,makeType<TExplicit<const FFrame*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1339 = das::das_call_member< const unsigned char (FFrame::*)() const,&FFrame::PeekCode >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:230:14
	makeExtern<DAS_CALL_METHOD(_method_1339), SimNode_ExtFuncCall >(lib,"PeekCode","das_call_member< const unsigned char (FFrame::*)() const , &FFrame::PeekCode >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1340 = das::das_call_member< void (FFrame::*)(const int),&FFrame::SkipCode >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:233:7
	makeExtern<DAS_CALL_METHOD(_method_1340), SimNode_ExtFuncCall >(lib,"SkipCode","das_call_member< void (FFrame::*)(const int) , &FFrame::SkipCode >::invoke")
		->args({"self","NumOps"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1341 = das::das_call_member< float (FFrame::*)(),&FFrame::ReadFloat >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:239:8
	makeExtern<DAS_CALL_METHOD(_method_1341), SimNode_ExtFuncCall >(lib,"ReadFloat","das_call_member< float (FFrame::*)() , &FFrame::ReadFloat >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1342 = das::das_call_member< double (FFrame::*)(),&FFrame::ReadDouble >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:240:9
	makeExtern<DAS_CALL_METHOD(_method_1342), SimNode_ExtFuncCall >(lib,"ReadDouble","das_call_member< double (FFrame::*)() , &FFrame::ReadDouble >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1343 = das::das_call_member< FName (FFrame::*)(),&FFrame::ReadName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:241:8
	makeExtern<DAS_CALL_METHOD(_method_1343), SimNode_ExtFuncCallAndCopyOrMove >(lib,"ReadName","das_call_member< FName (FFrame::*)() , &FFrame::ReadName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1344 = das::das_call_member< UObject * (FFrame::*)(),&FFrame::ReadObject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:242:11
	makeExtern<DAS_CALL_METHOD(_method_1344), SimNode_ExtFuncCall >(lib,"ReadObject","das_call_member< UObject * (FFrame::*)() , &FFrame::ReadObject >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1345 = das::das_call_member< int (FFrame::*)(),&FFrame::ReadWord >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:243:8
	makeExtern<DAS_CALL_METHOD(_method_1345), SimNode_ExtFuncCall >(lib,"ReadWord","das_call_member< int (FFrame::*)() , &FFrame::ReadWord >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1346 = das::das_call_member< FProperty * (FFrame::*)(),&FFrame::ReadProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:244:13
	makeExtern<DAS_CALL_METHOD(_method_1346), SimNode_ExtFuncCall >(lib,"ReadProperty","das_call_member< FProperty * (FFrame::*)() , &FFrame::ReadProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1347 = das::das_call_member< FProperty * (FFrame::*)(),&FFrame::ReadPropertyUnchecked >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:247:13
	makeExtern<DAS_CALL_METHOD(_method_1347), SimNode_ExtFuncCall >(lib,"ReadPropertyUnchecked","das_call_member< FProperty * (FFrame::*)() , &FFrame::ReadPropertyUnchecked >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1348 = das::das_call_member< unsigned int (FFrame::*)(),&FFrame::ReadCodeSkipCount >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:256:19
	makeExtern<DAS_CALL_METHOD(_method_1348), SimNode_ExtFuncCall >(lib,"ReadCodeSkipCount","das_call_member< unsigned int (FFrame::*)() , &FFrame::ReadCodeSkipCount >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

