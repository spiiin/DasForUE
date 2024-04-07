// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_11() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2535:15
	makeExtern< bool (*)(const FObjectPropertyBase &,const void *,const void *,unsigned int) , _dasUnreal_virtual_304_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_304_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2537:15
	makeExtern< bool (*)(const FObjectPropertyBase &) , _dasUnreal_virtual_305_SupportsNetSharedSerialization , SimNode_ExtFuncCall >(lib,"SupportsNetSharedSerialization","_dasUnreal_virtual_305_SupportsNetSharedSerialization")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2542:16
	makeExtern< FName (*)(const FObjectPropertyBase &) , _dasUnreal_virtual_306_GetID , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetID","_dasUnreal_virtual_306_GetID")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2544:15
	makeExtern< bool (*)(const FObjectPropertyBase &,const FProperty *) , _dasUnreal_virtual_307_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_307_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2550:18
	makeExtern< FString (*)(const FObjectPropertyBase &,FString *,unsigned int,const FString &) , _dasUnreal_virtual_308_GetCPPTypeCustom , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeCustom","_dasUnreal_virtual_308_GetCPPTypeCustom")
		->args({"self","ExtendedTypeText","CPPExportFlags","InnerNativeTypeName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2589:14
	makeExtern< bool (*)(const FObjectPropertyBase *,UObject *,UObject *,unsigned int) , _dasUnreal_static_309_StaticIdentical , SimNode_ExtFuncCall >(lib,"StaticIdentical","_dasUnreal_static_309_StaticIdentical")
		->args({"self","A","B","PortFlags"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2591:19
	makeExtern< UObject * (*)(const FObjectPropertyBase &,const void *) , _dasUnreal_virtual_310_LoadObjectPropertyValue , SimNode_ExtFuncCall >(lib,"LoadObjectPropertyValue","_dasUnreal_virtual_310_LoadObjectPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_837 = das::das_call_member< UObject * (FObjectPropertyBase::*)(const void *,int) const,&FObjectPropertyBase::LoadObjectPropertyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2595:23
	makeExtern<DAS_CALL_METHOD(_method_837), SimNode_ExtFuncCall >(lib,"LoadObjectPropertyValue_InContainer","das_call_member< UObject * (FObjectPropertyBase::*)(const void *,int) const , &FObjectPropertyBase::LoadObjectPropertyValue_InContainer >::invoke")
		->args({"self","PropertyValueAddress","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2601:19
	makeExtern< UObject * (*)(const FObjectPropertyBase &,const void *) , _dasUnreal_virtual_311_GetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue","_dasUnreal_virtual_311_GetObjectPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2602:19
	makeExtern< UObject * (*)(const FObjectPropertyBase &,const void *,int) , _dasUnreal_virtual_312_GetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue_InContainer","_dasUnreal_virtual_312_GetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2603:15
	makeExtern< void (*)(const FObjectPropertyBase &,void *,UObject *) , _dasUnreal_virtual_313_SetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue","_dasUnreal_virtual_313_SetObjectPropertyValue")
		->args({"self","PropertyValueAddress","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2605:15
	makeExtern< void (*)(const FObjectPropertyBase &,void *,UObject *,int) , _dasUnreal_virtual_314_SetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue_InContainer","_dasUnreal_virtual_314_SetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","Value","ArrayIndex"})
		->arg_init(3,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_842 = das::das_call_member< void (FObjectPropertyBase::*)(UClass *),&FObjectPropertyBase::SetPropertyClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2617:7
	makeExtern<DAS_CALL_METHOD(_method_842), SimNode_ExtFuncCall >(lib,"SetPropertyClass","das_call_member< void (FObjectPropertyBase::*)(UClass *) , &FObjectPropertyBase::SetPropertyClass >::invoke")
		->args({"self","NewPropertyClass"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStructProperty,EInternal,FFieldClass *>(*this,lib,"FStructProperty","FStructProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< FFieldClass * (*)(const FStructProperty *) , _dasUnreal_static_315_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_315_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< FField * (*)(const FStructProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_316_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_316_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< uint64 (*)(const FStructProperty *) , _dasUnreal_static_317_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_317_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStructProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5790:2
	makeExtern< uint64 (*)(const FStructProperty *) , _dasUnreal_static_318_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_318_StaticClassCastFlags")
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
	makeExtern< void (*)(FStructProperty &,FReferenceCollector &) , _dasUnreal_virtual_319_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_319_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5818:15
	makeExtern< void (*)(FStructProperty &,const FField &) , _dasUnreal_virtual_320_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_320_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5821:18
	makeExtern< FString (*)(const FStructProperty &,FString &) , _dasUnreal_virtual_321_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_321_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5822:18
	makeExtern< FString (*)(const FStructProperty &,FString *,unsigned int) , _dasUnreal_virtual_322_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_322_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5823:18
	makeExtern< FString (*)(const FStructProperty &) , _dasUnreal_virtual_323_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_323_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5825:15
	makeExtern< bool (*)(const FStructProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_324_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_324_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5828:15
	makeExtern< bool (*)(const FStructProperty &) , _dasUnreal_virtual_325_SupportsNetSharedSerialization , SimNode_ExtFuncCall >(lib,"SupportsNetSharedSerialization","_dasUnreal_virtual_325_SupportsNetSharedSerialization")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5833:15
	makeExtern< void (*)(const FStructProperty &,void *,const void *,int) , _dasUnreal_virtual_326_CopyValuesInternal , SimNode_ExtFuncCall >(lib,"CopyValuesInternal","_dasUnreal_virtual_326_CopyValuesInternal")
		->args({"self","Dest","Src","Count"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5834:15
	makeExtern< void (*)(const FStructProperty &,void *) , _dasUnreal_virtual_327_ClearValueInternal , SimNode_ExtFuncCall >(lib,"ClearValueInternal","_dasUnreal_virtual_327_ClearValueInternal")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5835:15
	makeExtern< void (*)(const FStructProperty &,void *) , _dasUnreal_virtual_328_DestroyValueInternal , SimNode_ExtFuncCall >(lib,"DestroyValueInternal","_dasUnreal_virtual_328_DestroyValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5836:15
	makeExtern< void (*)(const FStructProperty &,void *) , _dasUnreal_virtual_329_InitializeValueInternal , SimNode_ExtFuncCall >(lib,"InitializeValueInternal","_dasUnreal_virtual_329_InitializeValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5838:16
	makeExtern< int (*)(const FStructProperty &) , _dasUnreal_virtual_330_GetMinAlignment , SimNode_ExtFuncCall >(lib,"GetMinAlignment","_dasUnreal_virtual_330_GetMinAlignment")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5841:15
	makeExtern< bool (*)(const FStructProperty &,const FProperty *) , _dasUnreal_virtual_331_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_331_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5844:15
	makeExtern< void (*)(const FStructProperty &,FBlake3 &,bool) , _dasUnreal_virtual_332_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_332_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_861 = das::das_call_member< bool (FStructProperty::*)(FName,const void *,const FProperty *&,const void *&) const,&FStructProperty::FindInnerPropertyInstance >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:5849:7
	makeExtern<DAS_CALL_METHOD(_method_861), SimNode_ExtFuncCall >(lib,"FindInnerPropertyInstance","das_call_member< bool (FStructProperty::*)(FName,const void *,const FProperty *&,const void *&) const , &FStructProperty::FindInnerPropertyInstance >::invoke")
		->args({"self","PropertyName","Data","OutProp","OutData"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FCustomPropertyListNode,FProperty *,int>(*this,lib,"FCustomPropertyListNode","FCustomPropertyListNode")
		->args({"InProperty","InArrayIndex"})
		->arg_init(0,make_smart<ExprConstPtr>())
		->arg_init(1,make_smart<ExprConstInt>(0));
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:6205:32
	makeExtern< FProperty * (*)(const FCustomPropertyListNode *,const FCustomPropertyListNode *&) , _dasUnreal_static_333_GetNextPropertyAndAdvance , SimNode_ExtFuncCall >(lib,"GetNextPropertyAndAdvance","_dasUnreal_static_333_GetNextPropertyAndAdvance")
		->args({"self","Node"})
		->arg_type(0,makeType<TExplicit<const FCustomPropertyListNode*> >(lib))
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
	makeExtern< UClass * (*)(const UPropertyWrapper *) , _dasUnreal_static_334_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_334_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPropertyWrapper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:7166:2
	makeExtern< const wchar_t * (*)(const UPropertyWrapper *) , _dasUnreal_static_335_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_335_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPropertyWrapper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:7166:2
	makeExtern< void (*)(const UPropertyWrapper *) , _dasUnreal_static_336_StaticRegisterNativesUPropertyWrapper , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUPropertyWrapper","_dasUnreal_static_336_StaticRegisterNativesUPropertyWrapper")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPropertyWrapper*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_866 = das::das_call_member< void (UPropertyWrapper::*)(FProperty *),&UPropertyWrapper::SetProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:7173:7
	makeExtern<DAS_CALL_METHOD(_method_866), SimNode_ExtFuncCall >(lib,"SetProperty","das_call_member< void (UPropertyWrapper::*)(FProperty *) , &UPropertyWrapper::SetProperty >::invoke")
		->args({"self","InProperty"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

