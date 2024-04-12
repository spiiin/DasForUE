// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_17() {
	using _method_1349 = das::das_call_member< const FProperty * (FMapProperty::*)() const,&FMapProperty::GetKeyProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3913:19
	makeExtern<DAS_CALL_METHOD(_method_1349), SimNode_ExtFuncCall >(lib,"GetKeyProperty","das_call_member< const FProperty * (FMapProperty::*)() const , &FMapProperty::GetKeyProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1350 = das::das_call_member< const FProperty * (FMapProperty::*)() const,&FMapProperty::GetValueProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3918:19
	makeExtern<DAS_CALL_METHOD(_method_1350), SimNode_ExtFuncCall >(lib,"GetValueProperty","das_call_member< const FProperty * (FMapProperty::*)() const , &FMapProperty::GetValueProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3923:15
	makeExtern< void (*)(const FMapProperty &,FBlake3 &,bool) , _dasUnreal_virtual_669_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_669_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FSetProperty,EInternal,FFieldClass *>(*this,lib,"FSetProperty","FSetProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3932:2
	makeExtern< FFieldClass * (*)(const FSetProperty *) , _dasUnreal_static_670_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_670_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FSetProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3932:2
	makeExtern< FField * (*)(const FSetProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_671_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_671_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FSetProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3932:2
	makeExtern< uint64 (*)(const FSetProperty *) , _dasUnreal_static_672_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_672_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FSetProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3932:2
	makeExtern< uint64 (*)(const FSetProperty *) , _dasUnreal_static_673_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_673_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FSetProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FSetProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FSetProperty","FSetProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FSetProperty,UField *>(*this,lib,"FSetProperty","FSetProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3961:15
	makeExtern< void (*)(FSetProperty &,FArchive &) , _dasUnreal_virtual_674_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_674_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3962:15
	makeExtern< void (*)(FSetProperty &,FReferenceCollector &) , _dasUnreal_virtual_675_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_675_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3967:15
	makeExtern< void (*)(FSetProperty &,const FField &) , _dasUnreal_virtual_676_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_676_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3968:18
	makeExtern< FField * (*)(FSetProperty &,const FName &) , _dasUnreal_virtual_677_GetInnerFieldByName , SimNode_ExtFuncCall >(lib,"GetInnerFieldByName","_dasUnreal_virtual_677_GetInnerFieldByName")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3972:15
	makeExtern< void (*)(FSetProperty &,FProperty *) , _dasUnreal_virtual_678_AddCppProperty , SimNode_ExtFuncCall >(lib,"AddCppProperty","_dasUnreal_virtual_678_AddCppProperty")
		->args({"self","Property"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3976:18
	makeExtern< FString (*)(const FSetProperty &,FString &) , _dasUnreal_virtual_679_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_679_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3977:18
	makeExtern< FString (*)(const FSetProperty &,FString *,unsigned int) , _dasUnreal_virtual_680_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_680_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3978:18
	makeExtern< FString (*)(const FSetProperty &) , _dasUnreal_virtual_681_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_681_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3979:15
	makeExtern< void (*)(FSetProperty &,FArchive &) , _dasUnreal_virtual_682_LinkInternal , SimNode_ExtFuncCall >(lib,"LinkInternal","_dasUnreal_virtual_682_LinkInternal")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3980:15
	makeExtern< bool (*)(const FSetProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_683_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_683_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3987:15
	makeExtern< void (*)(const FSetProperty &,void *,const void *,int) , _dasUnreal_virtual_684_CopyValuesInternal , SimNode_ExtFuncCall >(lib,"CopyValuesInternal","_dasUnreal_virtual_684_CopyValuesInternal")
		->args({"self","Dest","Src","Count"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3988:15
	makeExtern< void (*)(const FSetProperty &,void *) , _dasUnreal_virtual_685_ClearValueInternal , SimNode_ExtFuncCall >(lib,"ClearValueInternal","_dasUnreal_virtual_685_ClearValueInternal")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3989:15
	makeExtern< void (*)(const FSetProperty &,void *) , _dasUnreal_virtual_686_DestroyValueInternal , SimNode_ExtFuncCall >(lib,"DestroyValueInternal","_dasUnreal_virtual_686_DestroyValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3990:15
	makeExtern< bool (*)(const FSetProperty &) , _dasUnreal_virtual_687_PassCPPArgsByRef , SimNode_ExtFuncCall >(lib,"PassCPPArgsByRef","_dasUnreal_virtual_687_PassCPPArgsByRef")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3994:15
	makeExtern< bool (*)(const FSetProperty &,const FProperty *) , _dasUnreal_virtual_688_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_688_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3996:16
	makeExtern< void * (*)(const FSetProperty &,const FProperty *,void *,int) , _dasUnreal_virtual_689_GetValueAddressAtIndex_Direct , SimNode_ExtFuncCall >(lib,"GetValueAddressAtIndex_Direct","_dasUnreal_virtual_689_GetValueAddressAtIndex_Direct")
		->args({"self","Inner","InValueAddress","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1372 = das::das_call_member< FString (FSetProperty::*)(FString *,unsigned int,const FString &,const FString &) const,&FSetProperty::GetCPPTypeCustom >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3999:10
	makeExtern<DAS_CALL_METHOD(_method_1372), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeCustom","das_call_member< FString (FSetProperty::*)(FString *,unsigned int,const FString &,const FString &) const , &FSetProperty::GetCPPTypeCustom >::invoke")
		->args({"self","ExtendedTypeText","CPPExportFlags","ElementTypeText","InElementExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1373 = das::das_call_member< int (FSetProperty::*)(void *) const,&FSetProperty::GetNum >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:4005:8
	makeExtern<DAS_CALL_METHOD(_method_1373), SimNode_ExtFuncCall >(lib,"GetNum","das_call_member< int (FSetProperty::*)(void *) const , &FSetProperty::GetNum >::invoke")
		->args({"self","InSet"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1374 = das::das_call_member< int (FSetProperty::*)() const,&FSetProperty::GetStride >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:4015:8
	makeExtern<DAS_CALL_METHOD(_method_1374), SimNode_ExtFuncCall >(lib,"GetStride","das_call_member< int (FSetProperty::*)() const , &FSetProperty::GetStride >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1375 = das::das_call_member< bool (FSetProperty::*)(void *,int) const,&FSetProperty::IsValidIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:4024:7
	makeExtern<DAS_CALL_METHOD(_method_1375), SimNode_ExtFuncCall >(lib,"IsValidIndex","das_call_member< bool (FSetProperty::*)(void *,int) const , &FSetProperty::IsValidIndex >::invoke")
		->args({"self","InSet","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1376 = das::das_call_member< unsigned char * (FSetProperty::*)(void *,int) const,&FSetProperty::GetElementPtr >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:4034:9
	makeExtern<DAS_CALL_METHOD(_method_1376), SimNode_ExtFuncCall >(lib,"GetElementPtr","das_call_member< unsigned char * (FSetProperty::*)(void *,int) const , &FSetProperty::GetElementPtr >::invoke")
		->args({"self","InSet","Index"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:4040:15
	makeExtern< void (*)(const FSetProperty &,FBlake3 &,bool) , _dasUnreal_virtual_690_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_690_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStructProperty,EInternal,FFieldClass *>(*this,lib,"FStructProperty","FStructProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< FFieldClass * (*)(const FStructProperty *) , _dasUnreal_static_691_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_691_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< FField * (*)(const FStructProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_692_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_692_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< uint64 (*)(const FStructProperty *) , _dasUnreal_static_693_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_693_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< uint64 (*)(const FStructProperty *) , _dasUnreal_static_694_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_694_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStructProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FStructProperty","FStructProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FStructProperty,UField *>(*this,lib,"FStructProperty","FStructProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5812:15
	makeExtern< void (*)(FStructProperty &,FArchive &) , _dasUnreal_virtual_695_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_695_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5813:15
	makeExtern< void (*)(FStructProperty &,FReferenceCollector &) , _dasUnreal_virtual_696_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_696_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5818:15
	makeExtern< void (*)(FStructProperty &,const FField &) , _dasUnreal_virtual_697_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_697_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5821:18
	makeExtern< FString (*)(const FStructProperty &,FString &) , _dasUnreal_virtual_698_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_698_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5822:18
	makeExtern< FString (*)(const FStructProperty &,FString *,unsigned int) , _dasUnreal_virtual_699_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_699_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5823:18
	makeExtern< FString (*)(const FStructProperty &) , _dasUnreal_virtual_700_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_700_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5824:15
	makeExtern< void (*)(FStructProperty &,FArchive &) , _dasUnreal_virtual_701_LinkInternal , SimNode_ExtFuncCall >(lib,"LinkInternal","_dasUnreal_virtual_701_LinkInternal")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5825:15
	makeExtern< bool (*)(const FStructProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_702_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_702_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5828:15
	makeExtern< bool (*)(const FStructProperty &) , _dasUnreal_virtual_703_SupportsNetSharedSerialization , SimNode_ExtFuncCall >(lib,"SupportsNetSharedSerialization","_dasUnreal_virtual_703_SupportsNetSharedSerialization")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5833:15
	makeExtern< void (*)(const FStructProperty &,void *,const void *,int) , _dasUnreal_virtual_704_CopyValuesInternal , SimNode_ExtFuncCall >(lib,"CopyValuesInternal","_dasUnreal_virtual_704_CopyValuesInternal")
		->args({"self","Dest","Src","Count"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5834:15
	makeExtern< void (*)(const FStructProperty &,void *) , _dasUnreal_virtual_705_ClearValueInternal , SimNode_ExtFuncCall >(lib,"ClearValueInternal","_dasUnreal_virtual_705_ClearValueInternal")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5835:15
	makeExtern< void (*)(const FStructProperty &,void *) , _dasUnreal_virtual_706_DestroyValueInternal , SimNode_ExtFuncCall >(lib,"DestroyValueInternal","_dasUnreal_virtual_706_DestroyValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5836:15
	makeExtern< void (*)(const FStructProperty &,void *) , _dasUnreal_virtual_707_InitializeValueInternal , SimNode_ExtFuncCall >(lib,"InitializeValueInternal","_dasUnreal_virtual_707_InitializeValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5838:16
	makeExtern< int (*)(const FStructProperty &) , _dasUnreal_virtual_708_GetMinAlignment , SimNode_ExtFuncCall >(lib,"GetMinAlignment","_dasUnreal_virtual_708_GetMinAlignment")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5841:15
	makeExtern< bool (*)(const FStructProperty &,const FProperty *) , _dasUnreal_virtual_709_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_709_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5844:15
	makeExtern< void (*)(const FStructProperty &,FBlake3 &,bool) , _dasUnreal_virtual_710_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_710_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1398 = das::das_call_member< bool (FStructProperty::*)(const FArchive &) const,&FStructProperty::UseBinaryOrNativeSerialization >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5848:7
	makeExtern<DAS_CALL_METHOD(_method_1398), SimNode_ExtFuncCall >(lib,"UseBinaryOrNativeSerialization","das_call_member< bool (FStructProperty::*)(const FArchive &) const , &FStructProperty::UseBinaryOrNativeSerialization >::invoke")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1399 = das::das_call_member< bool (FStructProperty::*)(FName,const void *,const FProperty *&,const void *&) const,&FStructProperty::FindInnerPropertyInstance >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5849:7
	makeExtern<DAS_CALL_METHOD(_method_1399), SimNode_ExtFuncCall >(lib,"FindInnerPropertyInstance","das_call_member< bool (FStructProperty::*)(FName,const void *,const FProperty *&,const void *&) const , &FStructProperty::FindInnerPropertyInstance >::invoke")
		->args({"self","PropertyName","Data","OutProp","OutData"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FCustomPropertyListNode,FProperty *,int>(*this,lib,"FCustomPropertyListNode","FCustomPropertyListNode")
		->args({"InProperty","InArrayIndex"})
		->arg_init(0,make_smart<ExprConstPtr>())
		->arg_init(1,make_smart<ExprConstInt>(0));
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:6205:32
	makeExtern< FProperty * (*)(const FCustomPropertyListNode *,const FCustomPropertyListNode *&) , _dasUnreal_static_711_GetNextPropertyAndAdvance , SimNode_ExtFuncCall >(lib,"GetNextPropertyAndAdvance","_dasUnreal_static_711_GetNextPropertyAndAdvance")
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
	makeExtern< UClass * (*)(const UPropertyWrapper *) , _dasUnreal_static_712_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_712_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPropertyWrapper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:7166:2
	makeExtern< const wchar_t * (*)(const UPropertyWrapper *) , _dasUnreal_static_713_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_713_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPropertyWrapper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:7166:2
	makeExtern< void (*)(const UPropertyWrapper *) , _dasUnreal_static_714_StaticRegisterNativesUPropertyWrapper , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUPropertyWrapper","_dasUnreal_static_714_StaticRegisterNativesUPropertyWrapper")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPropertyWrapper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1404 = das::das_call_member< void (UPropertyWrapper::*)(FProperty *),&UPropertyWrapper::SetProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:7173:7
	makeExtern<DAS_CALL_METHOD(_method_1404), SimNode_ExtFuncCall >(lib,"SetProperty","das_call_member< void (UPropertyWrapper::*)(FProperty *) , &UPropertyWrapper::SetProperty >::invoke")
		->args({"self","InProperty"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FFrame,UObject *,UFunction *,void *,FFrame *,FField *>(*this,lib,"FFrame","FFrame")
		->args({"InObject","InNode","InLocals","InPreviousFrame","InPropertyChainForCompiledIn"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->arg_init(4,make_smart<ExprConstPtr>());
	using _method_1405 = das::das_call_member< void (FFrame::*)(UObject *,void *const),&FFrame::Step >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:208:23
	makeExtern<DAS_CALL_METHOD(_method_1405), SimNode_ExtFuncCall >(lib,"Step","das_call_member< void (FFrame::*)(UObject *,void *const) , &FFrame::Step >::invoke")
		->args({"self","Context","Z_Param__Result"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1406 = das::das_call_member< bool (FFrame::*)(UObject *,void *const),&FFrame::StepAndCheckMostRecentProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:211:30
	makeExtern<DAS_CALL_METHOD(_method_1406), SimNode_ExtFuncCall >(lib,"StepAndCheckMostRecentProperty","das_call_member< bool (FFrame::*)(UObject *,void *const) , &FFrame::StepAndCheckMostRecentProperty >::invoke")
		->args({"self","Context","Z_Param__Result"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1407 = das::das_call_member< void (FFrame::*)(void *const,FProperty *),&FFrame::StepExplicitProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:214:23
	makeExtern<DAS_CALL_METHOD(_method_1407), SimNode_ExtFuncCall >(lib,"StepExplicitProperty","das_call_member< void (FFrame::*)(void *const,FProperty *) , &FFrame::StepExplicitProperty >::invoke")
		->args({"self","Result","Property"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1408 = das::das_call_member< void (FFrame::*)(void *,const FFieldClass *),&FFrame::StepCompiledIn >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:219:30
	makeExtern<DAS_CALL_METHOD(_method_1408), SimNode_ExtFuncCall >(lib,"StepCompiledIn","das_call_member< void (FFrame::*)(void *,const FFieldClass *) , &FFrame::StepCompiledIn >::invoke")
		->args({"self","Result","ExpectedPropertyType"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:225:31
	makeExtern< void (*)(FFrame &,const wchar_t *,ELogVerbosity::Type,const FName &) , _dasUnreal_virtual_715_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_715_Serialize")
		->args({"self","V","Verbosity","Category"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:227:30
	makeExtern< void (*)(const FFrame *,const wchar_t *,ELogVerbosity::Type,FName) , _dasUnreal_static_716_KismetExecutionMessage , SimNode_ExtFuncCall >(lib,"KismetExecutionMessage","_dasUnreal_static_716_KismetExecutionMessage")
		->args({"self","Message","Verbosity","WarningId"})
		->arg_type(0,makeType<TExplicit<const FFrame*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1411 = das::das_call_member< const unsigned char (FFrame::*)() const,&FFrame::PeekCode >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:230:14
	makeExtern<DAS_CALL_METHOD(_method_1411), SimNode_ExtFuncCall >(lib,"PeekCode","das_call_member< const unsigned char (FFrame::*)() const , &FFrame::PeekCode >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1412 = das::das_call_member< void (FFrame::*)(const int),&FFrame::SkipCode >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:233:7
	makeExtern<DAS_CALL_METHOD(_method_1412), SimNode_ExtFuncCall >(lib,"SkipCode","das_call_member< void (FFrame::*)(const int) , &FFrame::SkipCode >::invoke")
		->args({"self","NumOps"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1413 = das::das_call_member< float (FFrame::*)(),&FFrame::ReadFloat >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:239:8
	makeExtern<DAS_CALL_METHOD(_method_1413), SimNode_ExtFuncCall >(lib,"ReadFloat","das_call_member< float (FFrame::*)() , &FFrame::ReadFloat >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1414 = das::das_call_member< double (FFrame::*)(),&FFrame::ReadDouble >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:240:9
	makeExtern<DAS_CALL_METHOD(_method_1414), SimNode_ExtFuncCall >(lib,"ReadDouble","das_call_member< double (FFrame::*)() , &FFrame::ReadDouble >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1415 = das::das_call_member< FName (FFrame::*)(),&FFrame::ReadName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:241:8
	makeExtern<DAS_CALL_METHOD(_method_1415), SimNode_ExtFuncCallAndCopyOrMove >(lib,"ReadName","das_call_member< FName (FFrame::*)() , &FFrame::ReadName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1416 = das::das_call_member< UObject * (FFrame::*)(),&FFrame::ReadObject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:242:11
	makeExtern<DAS_CALL_METHOD(_method_1416), SimNode_ExtFuncCall >(lib,"ReadObject","das_call_member< UObject * (FFrame::*)() , &FFrame::ReadObject >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1417 = das::das_call_member< int (FFrame::*)(),&FFrame::ReadWord >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:243:8
	makeExtern<DAS_CALL_METHOD(_method_1417), SimNode_ExtFuncCall >(lib,"ReadWord","das_call_member< int (FFrame::*)() , &FFrame::ReadWord >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1418 = das::das_call_member< FProperty * (FFrame::*)(),&FFrame::ReadProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:244:13
	makeExtern<DAS_CALL_METHOD(_method_1418), SimNode_ExtFuncCall >(lib,"ReadProperty","das_call_member< FProperty * (FFrame::*)() , &FFrame::ReadProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1419 = das::das_call_member< FProperty * (FFrame::*)(),&FFrame::ReadPropertyUnchecked >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:247:13
	makeExtern<DAS_CALL_METHOD(_method_1419), SimNode_ExtFuncCall >(lib,"ReadPropertyUnchecked","das_call_member< FProperty * (FFrame::*)() , &FFrame::ReadPropertyUnchecked >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1420 = das::das_call_member< unsigned int (FFrame::*)(),&FFrame::ReadCodeSkipCount >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:256:19
	makeExtern<DAS_CALL_METHOD(_method_1420), SimNode_ExtFuncCall >(lib,"ReadCodeSkipCount","das_call_member< unsigned int (FFrame::*)() , &FFrame::ReadCodeSkipCount >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1421 = das::das_call_member< unsigned short (FFrame::*)(FProperty **),&FFrame::ReadVariableSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:265:19
	makeExtern<DAS_CALL_METHOD(_method_1421), SimNode_ExtFuncCall >(lib,"ReadVariableSize","das_call_member< unsigned short (FFrame::*)(FProperty **) , &FFrame::ReadVariableSize >::invoke")
		->args({"self","ExpressionField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:313:33
	makeExtern< FFrame * (*)(const FFrame *,FFrame *) , _dasUnreal_static_717_PushThreadLocalTopStackFrame , SimNode_ExtFuncCall >(lib,"PushThreadLocalTopStackFrame","_dasUnreal_static_717_PushThreadLocalTopStackFrame")
		->args({"self","NewTopStackFrame"})
		->arg_type(0,makeType<TExplicit<const FFrame*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:314:30
	makeExtern< void (*)(const FFrame *,FFrame *) , _dasUnreal_static_718_PopThreadLocalTopStackFrame , SimNode_ExtFuncCall >(lib,"PopThreadLocalTopStackFrame","_dasUnreal_static_718_PopThreadLocalTopStackFrame")
		->args({"self","NewTopStackFrame"})
		->arg_type(0,makeType<TExplicit<const FFrame*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:315:33
	makeExtern< FFrame * (*)(const FFrame *) , _dasUnreal_static_719_GetThreadLocalTopStackFrame , SimNode_ExtFuncCall >(lib,"GetThreadLocalTopStackFrame","_dasUnreal_static_719_GetThreadLocalTopStackFrame")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FFrame*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:464:22
	makeExtern< void (*)() , GInitRunaway , SimNode_ExtFuncCall >(lib,"GInitRunaway","GInitRunaway")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/BlueprintFunctionLibrary.h:17:2
	makeExtern< UClass * (*)(const UBlueprintFunctionLibrary *) , _dasUnreal_static_720_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_720_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UBlueprintFunctionLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/BlueprintFunctionLibrary.h:17:2
	makeExtern< const wchar_t * (*)(const UBlueprintFunctionLibrary *) , _dasUnreal_static_721_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_721_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UBlueprintFunctionLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/BlueprintFunctionLibrary.h:20:27
	makeExtern< int (*)(UBlueprintFunctionLibrary &,UFunction *,FFrame *) , _dasUnreal_virtual_722_GetFunctionCallspace , SimNode_ExtFuncCall >(lib,"GetFunctionCallspace","_dasUnreal_virtual_722_GetFunctionCallspace")
		->args({"self","Function","Stack"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:21:2
	makeExtern< UClass * (*)(const UKismetArrayLibrary *) , _dasUnreal_static_723_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_723_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:21:2
	makeExtern< const wchar_t * (*)(const UKismetArrayLibrary *) , _dasUnreal_static_724_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_724_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:270:26
	makeExtern< int (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,const void *) , _dasUnreal_static_725_GenericArray_Add , SimNode_ExtFuncCall >(lib,"GenericArray_Add","_dasUnreal_static_725_GenericArray_Add")
		->args({"self","TargetArray","ArrayProp","NewItem"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:271:26
	makeExtern< int (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,const void *) , _dasUnreal_static_726_GenericArray_AddUnique , SimNode_ExtFuncCall >(lib,"GenericArray_AddUnique","_dasUnreal_static_726_GenericArray_AddUnique")
		->args({"self","TargetArray","ArrayProp","NewItem"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:272:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *) , _dasUnreal_static_727_GenericArray_Shuffle , SimNode_ExtFuncCall >(lib,"GenericArray_Shuffle","_dasUnreal_static_727_GenericArray_Shuffle")
		->args({"self","TargetArray","ArrayProp"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:274:25
	makeExtern< bool (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,void *,const FArrayProperty *) , _dasUnreal_static_728_GenericArray_Identical , SimNode_ExtFuncCall >(lib,"GenericArray_Identical","_dasUnreal_static_728_GenericArray_Identical")
		->args({"self","ArrayA","ArrayAProp","ArrayB","ArrayBProperty"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:275:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,void *,const FArrayProperty *) , _dasUnreal_static_729_GenericArray_Append , SimNode_ExtFuncCall >(lib,"GenericArray_Append","_dasUnreal_static_729_GenericArray_Append")
		->args({"self","TargetArray","TargetArrayProp","SourceArray","SourceArrayProperty"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:276:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,const void *,int) , _dasUnreal_static_730_GenericArray_Insert , SimNode_ExtFuncCall >(lib,"GenericArray_Insert","_dasUnreal_static_730_GenericArray_Insert")
		->args({"self","TargetArray","ArrayProp","NewItem","Index"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
}
}

