// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_16() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3284:2
	makeExtern< uint64 (*)(const FClassPtrProperty *) , _dasUnreal_static_589_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_589_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FClassPtrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3284:2
	makeExtern< uint64 (*)(const FClassPtrProperty *) , _dasUnreal_static_590_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_590_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FClassPtrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FClassPtrProperty,UField *>(*this,lib,"FClassPtrProperty","FClassPtrProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3296:18
	makeExtern< FString (*)(const FClassPtrProperty &,FString &) , _dasUnreal_virtual_591_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_591_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3297:18
	makeExtern< FString (*)(const FClassPtrProperty &,FString *,unsigned int) , _dasUnreal_virtual_592_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_592_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3301:15
	makeExtern< bool (*)(const FClassPtrProperty &,const FProperty *) , _dasUnreal_virtual_593_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_593_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3302:15
	makeExtern< bool (*)(const FClassPtrProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_594_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_594_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3304:15
	makeExtern< void (*)(const FClassPtrProperty &,void *,const void *) , _dasUnreal_virtual_595_CopySingleValueToScriptVM , SimNode_ExtFuncCall >(lib,"CopySingleValueToScriptVM","_dasUnreal_virtual_595_CopySingleValueToScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3305:15
	makeExtern< void (*)(const FClassPtrProperty &,void *,const void *) , _dasUnreal_virtual_596_CopySingleValueFromScriptVM , SimNode_ExtFuncCall >(lib,"CopySingleValueFromScriptVM","_dasUnreal_virtual_596_CopySingleValueFromScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3306:15
	makeExtern< void (*)(const FClassPtrProperty &,void *,const void *) , _dasUnreal_virtual_597_CopyCompleteValueToScriptVM , SimNode_ExtFuncCall >(lib,"CopyCompleteValueToScriptVM","_dasUnreal_virtual_597_CopyCompleteValueToScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3307:15
	makeExtern< void (*)(const FClassPtrProperty &,void *,const void *) , _dasUnreal_virtual_598_CopyCompleteValueFromScriptVM , SimNode_ExtFuncCall >(lib,"CopyCompleteValueFromScriptVM","_dasUnreal_virtual_598_CopyCompleteValueFromScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3308:15
	makeExtern< void (*)(const FClassPtrProperty &,void *,const void *) , _dasUnreal_virtual_599_CopyCompleteValueToScriptVM_InContainer , SimNode_ExtFuncCall >(lib,"CopyCompleteValueToScriptVM_InContainer","_dasUnreal_virtual_599_CopyCompleteValueToScriptVM_InContainer")
		->args({"self","OutValue","InContainer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3309:15
	makeExtern< void (*)(const FClassPtrProperty &,void *,const void *) , _dasUnreal_virtual_600_CopyCompleteValueFromScriptVM_InContainer , SimNode_ExtFuncCall >(lib,"CopyCompleteValueFromScriptVM_InContainer","_dasUnreal_virtual_600_CopyCompleteValueFromScriptVM_InContainer")
		->args({"self","OutContainer","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3313:19
	makeExtern< UObject * (*)(const FClassPtrProperty &,const void *) , _dasUnreal_virtual_601_GetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue","_dasUnreal_virtual_601_GetObjectPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3314:19
	makeExtern< UObject * (*)(const FClassPtrProperty &,const void *,int) , _dasUnreal_virtual_602_GetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue_InContainer","_dasUnreal_virtual_602_GetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3315:15
	makeExtern< void (*)(const FClassPtrProperty &,void *,UObject *) , _dasUnreal_virtual_603_SetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue","_dasUnreal_virtual_603_SetObjectPropertyValue")
		->args({"self","PropertyValueAddress","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3316:15
	makeExtern< void (*)(const FClassPtrProperty &,void *,UObject *,int) , _dasUnreal_virtual_604_SetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue_InContainer","_dasUnreal_virtual_604_SetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","Value","ArrayIndex"})
		->arg_init(3,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FSoftClassProperty,EInternal,FFieldClass *>(*this,lib,"FSoftClassProperty","FSoftClassProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3332:2
	makeExtern< FFieldClass * (*)(const FSoftClassProperty *) , _dasUnreal_static_605_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_605_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FSoftClassProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3332:2
	makeExtern< FField * (*)(const FSoftClassProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_606_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_606_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FSoftClassProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3332:2
	makeExtern< uint64 (*)(const FSoftClassProperty *) , _dasUnreal_static_607_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_607_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FSoftClassProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3332:2
	makeExtern< uint64 (*)(const FSoftClassProperty *) , _dasUnreal_static_608_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_608_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FSoftClassProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FSoftClassProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FSoftClassProperty","FSoftClassProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FSoftClassProperty,UField *>(*this,lib,"FSoftClassProperty","FSoftClassProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3363:18
	makeExtern< FString (*)(const FSoftClassProperty &,FString *,unsigned int) , _dasUnreal_virtual_609_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_609_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3364:18
	makeExtern< FString (*)(const FSoftClassProperty &,FString &) , _dasUnreal_virtual_610_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_610_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3365:18
	makeExtern< FString (*)(const FSoftClassProperty &) , _dasUnreal_virtual_611_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_611_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3369:15
	makeExtern< void (*)(FSoftClassProperty &,const FField &) , _dasUnreal_virtual_612_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_612_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3372:15
	makeExtern< void (*)(FSoftClassProperty &,FArchive &) , _dasUnreal_virtual_613_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_613_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3373:15
	makeExtern< void (*)(FSoftClassProperty &,FReferenceCollector &) , _dasUnreal_virtual_614_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_614_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3374:15
	makeExtern< void (*)(FSoftClassProperty &) , _dasUnreal_virtual_615_BeginDestroy , SimNode_ExtFuncCall >(lib,"BeginDestroy","_dasUnreal_virtual_615_BeginDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3378:15
	makeExtern< bool (*)(const FSoftClassProperty &,const FProperty *) , _dasUnreal_virtual_616_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_616_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3381:18
	makeExtern< FString (*)(const FSoftClassProperty &,FString *,unsigned int,const FString &) , _dasUnreal_virtual_617_GetCPPTypeCustom , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeCustom","_dasUnreal_virtual_617_GetCPPTypeCustom")
		->args({"self","ExtendedTypeText","CPPExportFlags","InnerNativeTypeName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1291 = das::das_call_member< void (FSoftClassProperty::*)(UClass *),&FSoftClassProperty::SetMetaClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3393:7
	makeExtern<DAS_CALL_METHOD(_method_1291), SimNode_ExtFuncCall >(lib,"SetMetaClass","das_call_member< void (FSoftClassProperty::*)(UClass *) , &FSoftClassProperty::SetMetaClass >::invoke")
		->args({"self","NewMetaClass"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStrProperty,EInternal,FFieldClass *>(*this,lib,"FStrProperty","FStrProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3566:2
	makeExtern< FFieldClass * (*)(const FStrProperty *) , _dasUnreal_static_618_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_618_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3566:2
	makeExtern< FField * (*)(const FStrProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_619_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_619_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FStrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3566:2
	makeExtern< uint64 (*)(const FStrProperty *) , _dasUnreal_static_620_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_620_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3566:2
	makeExtern< uint64 (*)(const FStrProperty *) , _dasUnreal_static_621_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_621_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStrProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FStrProperty","FStrProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FStrProperty,UField *>(*this,lib,"FStrProperty","FStrProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3604:18
	makeExtern< FString (*)(const FStrProperty &) , _dasUnreal_virtual_622_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_622_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3605:9
	makeExtern< unsigned int (*)(const FStrProperty &,const void *) , _dasUnreal_virtual_623_GetValueTypeHashInternal , SimNode_ExtFuncCall >(lib,"GetValueTypeHashInternal","_dasUnreal_virtual_623_GetValueTypeHashInternal")
		->args({"self","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FArrayProperty,EInternal,FFieldClass *>(*this,lib,"FArrayProperty","FArrayProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3630:2
	makeExtern< FFieldClass * (*)(const FArrayProperty *) , _dasUnreal_static_624_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_624_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FArrayProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3630:2
	makeExtern< FField * (*)(const FArrayProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_625_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_625_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FArrayProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3630:2
	makeExtern< uint64 (*)(const FArrayProperty *) , _dasUnreal_static_626_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_626_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FArrayProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3630:2
	makeExtern< uint64 (*)(const FArrayProperty *) , _dasUnreal_static_627_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_627_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FArrayProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FArrayProperty,FFieldVariant,const FName &,EObjectFlags,EArrayPropertyFlags>(*this,lib,"FArrayProperty","FArrayProperty")
		->args({"InOwner","InName","InObjectFlags","InArrayPropertyFlags"})
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<EArrayPropertyFlags>(lib)));
	addCtorAndUsing<FArrayProperty,UField *>(*this,lib,"FArrayProperty","FArrayProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3681:15
	makeExtern< void (*)(FArrayProperty &,FArchive &) , _dasUnreal_virtual_628_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_628_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3682:15
	makeExtern< void (*)(FArrayProperty &,FReferenceCollector &) , _dasUnreal_virtual_629_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_629_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3687:15
	makeExtern< void (*)(FArrayProperty &,const FField &) , _dasUnreal_virtual_630_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_630_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3690:15
	makeExtern< void (*)(FArrayProperty &,FProperty *) , _dasUnreal_virtual_631_AddCppProperty , SimNode_ExtFuncCall >(lib,"AddCppProperty","_dasUnreal_virtual_631_AddCppProperty")
		->args({"self","Property"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3691:18
	makeExtern< FField * (*)(FArrayProperty &,const FName &) , _dasUnreal_virtual_632_GetInnerFieldByName , SimNode_ExtFuncCall >(lib,"GetInnerFieldByName","_dasUnreal_virtual_632_GetInnerFieldByName")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3697:18
	makeExtern< FString (*)(const FArrayProperty &,FString &) , _dasUnreal_virtual_633_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_633_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3698:18
	makeExtern< FString (*)(const FArrayProperty &,FString *,unsigned int) , _dasUnreal_virtual_634_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_634_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3699:18
	makeExtern< FString (*)(const FArrayProperty &) , _dasUnreal_virtual_635_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_635_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3700:15
	makeExtern< void (*)(FArrayProperty &,FArchive &) , _dasUnreal_virtual_636_LinkInternal , SimNode_ExtFuncCall >(lib,"LinkInternal","_dasUnreal_virtual_636_LinkInternal")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3701:15
	makeExtern< bool (*)(const FArrayProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_637_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_637_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3708:15
	makeExtern< void (*)(const FArrayProperty &,void *) , _dasUnreal_virtual_638_InitializeValueInternal , SimNode_ExtFuncCall >(lib,"InitializeValueInternal","_dasUnreal_virtual_638_InitializeValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3727:15
	makeExtern< void (*)(const FArrayProperty &,void *,const void *,int) , _dasUnreal_virtual_639_CopyValuesInternal , SimNode_ExtFuncCall >(lib,"CopyValuesInternal","_dasUnreal_virtual_639_CopyValuesInternal")
		->args({"self","Dest","Src","Count"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3728:15
	makeExtern< void (*)(const FArrayProperty &,void *) , _dasUnreal_virtual_640_ClearValueInternal , SimNode_ExtFuncCall >(lib,"ClearValueInternal","_dasUnreal_virtual_640_ClearValueInternal")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3729:15
	makeExtern< void (*)(const FArrayProperty &,void *) , _dasUnreal_virtual_641_DestroyValueInternal , SimNode_ExtFuncCall >(lib,"DestroyValueInternal","_dasUnreal_virtual_641_DestroyValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3730:15
	makeExtern< bool (*)(const FArrayProperty &) , _dasUnreal_virtual_642_PassCPPArgsByRef , SimNode_ExtFuncCall >(lib,"PassCPPArgsByRef","_dasUnreal_virtual_642_PassCPPArgsByRef")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3734:15
	makeExtern< bool (*)(const FArrayProperty &,const FProperty *) , _dasUnreal_virtual_643_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_643_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3737:16
	makeExtern< int (*)(const FArrayProperty &) , _dasUnreal_virtual_644_GetMinAlignment , SimNode_ExtFuncCall >(lib,"GetMinAlignment","_dasUnreal_virtual_644_GetMinAlignment")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3743:16
	makeExtern< void * (*)(const FArrayProperty &,const FProperty *,void *,int) , _dasUnreal_virtual_645_GetValueAddressAtIndex_Direct , SimNode_ExtFuncCall >(lib,"GetValueAddressAtIndex_Direct","_dasUnreal_virtual_645_GetValueAddressAtIndex_Direct")
		->args({"self","Inner","InValueAddress","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1320 = das::das_call_member< FString (FArrayProperty::*)(FString *,unsigned int,const FString &,const FString &) const,&FArrayProperty::GetCPPTypeCustom >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3746:10
	makeExtern<DAS_CALL_METHOD(_method_1320), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeCustom","das_call_member< FString (FArrayProperty::*)(FString *,unsigned int,const FString &,const FString &) const , &FArrayProperty::GetCPPTypeCustom >::invoke")
		->args({"self","ExtendedTypeText","CPPExportFlags","InnerTypeText","InInnerExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3749:14
	makeExtern< void (*)(const FArrayProperty *,FString &,const FProperty *,const void *,int,const void *,int,UObject *,int,UObject *) , _dasUnreal_static_646_ExportTextInnerItem , SimNode_ExtFuncCall >(lib,"ExportTextInnerItem","_dasUnreal_static_646_ExportTextInnerItem")
		->args({"self","ValueStr","Inner","PropertyValue","PropertySize","DefaultValue","DefaultSize","Parent","PortFlags","ExportRootScope"})
		->arg_type(0,makeType<TExplicit<const FArrayProperty*> >(lib))
		->arg_init(7,make_smart<ExprConstPtr>())
		->arg_init(8,make_smart<ExprConstInt>(0))
		->arg_init(9,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3754:15
	makeExtern< void (*)(const FArrayProperty &,FBlake3 &,bool) , _dasUnreal_virtual_647_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_647_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FMapProperty,EInternal,FFieldClass *>(*this,lib,"FMapProperty","FMapProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3774:2
	makeExtern< FFieldClass * (*)(const FMapProperty *) , _dasUnreal_static_648_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_648_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FMapProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3774:2
	makeExtern< FField * (*)(const FMapProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_649_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_649_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FMapProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3774:2
	makeExtern< uint64 (*)(const FMapProperty *) , _dasUnreal_static_650_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_650_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FMapProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3774:2
	makeExtern< uint64 (*)(const FMapProperty *) , _dasUnreal_static_651_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_651_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FMapProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FMapProperty,FFieldVariant,const FName &,EObjectFlags,EMapPropertyFlags>(*this,lib,"FMapProperty","FMapProperty")
		->args({"InOwner","InName","InObjectFlags","InMapFlags"})
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<EMapPropertyFlags>(lib)));
	addCtorAndUsing<FMapProperty,UField *>(*this,lib,"FMapProperty","FMapProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3818:15
	makeExtern< void (*)(FMapProperty &,FArchive &) , _dasUnreal_virtual_652_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_652_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3819:15
	makeExtern< void (*)(FMapProperty &,FReferenceCollector &) , _dasUnreal_virtual_653_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_653_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3824:15
	makeExtern< void (*)(FMapProperty &,const FField &) , _dasUnreal_virtual_654_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_654_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3825:18
	makeExtern< FField * (*)(FMapProperty &,const FName &) , _dasUnreal_virtual_655_GetInnerFieldByName , SimNode_ExtFuncCall >(lib,"GetInnerFieldByName","_dasUnreal_virtual_655_GetInnerFieldByName")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3829:15
	makeExtern< void (*)(FMapProperty &,FProperty *) , _dasUnreal_virtual_656_AddCppProperty , SimNode_ExtFuncCall >(lib,"AddCppProperty","_dasUnreal_virtual_656_AddCppProperty")
		->args({"self","Property"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3833:18
	makeExtern< FString (*)(const FMapProperty &,FString &) , _dasUnreal_virtual_657_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_657_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3834:18
	makeExtern< FString (*)(const FMapProperty &,FString *,unsigned int) , _dasUnreal_virtual_658_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_658_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3835:18
	makeExtern< FString (*)(const FMapProperty &) , _dasUnreal_virtual_659_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_659_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3836:15
	makeExtern< void (*)(FMapProperty &,FArchive &) , _dasUnreal_virtual_660_LinkInternal , SimNode_ExtFuncCall >(lib,"LinkInternal","_dasUnreal_virtual_660_LinkInternal")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3837:15
	makeExtern< bool (*)(const FMapProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_661_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_661_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3844:15
	makeExtern< void (*)(const FMapProperty &,void *) , _dasUnreal_virtual_662_InitializeValueInternal , SimNode_ExtFuncCall >(lib,"InitializeValueInternal","_dasUnreal_virtual_662_InitializeValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3863:15
	makeExtern< void (*)(const FMapProperty &,void *,const void *,int) , _dasUnreal_virtual_663_CopyValuesInternal , SimNode_ExtFuncCall >(lib,"CopyValuesInternal","_dasUnreal_virtual_663_CopyValuesInternal")
		->args({"self","Dest","Src","Count"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3864:15
	makeExtern< void (*)(const FMapProperty &,void *) , _dasUnreal_virtual_664_ClearValueInternal , SimNode_ExtFuncCall >(lib,"ClearValueInternal","_dasUnreal_virtual_664_ClearValueInternal")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3865:15
	makeExtern< void (*)(const FMapProperty &,void *) , _dasUnreal_virtual_665_DestroyValueInternal , SimNode_ExtFuncCall >(lib,"DestroyValueInternal","_dasUnreal_virtual_665_DestroyValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3866:15
	makeExtern< bool (*)(const FMapProperty &) , _dasUnreal_virtual_666_PassCPPArgsByRef , SimNode_ExtFuncCall >(lib,"PassCPPArgsByRef","_dasUnreal_virtual_666_PassCPPArgsByRef")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3870:15
	makeExtern< bool (*)(const FMapProperty &,const FProperty *) , _dasUnreal_virtual_667_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_667_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3872:16
	makeExtern< void * (*)(const FMapProperty &,const FProperty *,void *,int) , _dasUnreal_virtual_668_GetValueAddressAtIndex_Direct , SimNode_ExtFuncCall >(lib,"GetValueAddressAtIndex_Direct","_dasUnreal_virtual_668_GetValueAddressAtIndex_Direct")
		->args({"self","Inner","InValueAddress","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1344 = das::das_call_member< FString (FMapProperty::*)(FString *,unsigned int,const FString &,const FString &,const FString &,const FString &) const,&FMapProperty::GetCPPTypeCustom >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3875:10
	makeExtern<DAS_CALL_METHOD(_method_1344), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeCustom","das_call_member< FString (FMapProperty::*)(FString *,unsigned int,const FString &,const FString &,const FString &,const FString &) const , &FMapProperty::GetCPPTypeCustom >::invoke")
		->args({"self","ExtendedTypeText","CPPExportFlags","KeyTypeText","InKeyExtendedTypeText","ValueTypeText","InValueExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1345 = das::das_call_member< int (FMapProperty::*)(void *) const,&FMapProperty::GetNum >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3881:8
	makeExtern<DAS_CALL_METHOD(_method_1345), SimNode_ExtFuncCall >(lib,"GetNum","das_call_member< int (FMapProperty::*)(void *) const , &FMapProperty::GetNum >::invoke")
		->args({"self","InMap"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1346 = das::das_call_member< int (FMapProperty::*)() const,&FMapProperty::GetPairStride >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3890:8
	makeExtern<DAS_CALL_METHOD(_method_1346), SimNode_ExtFuncCall >(lib,"GetPairStride","das_call_member< int (FMapProperty::*)() const , &FMapProperty::GetPairStride >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1347 = das::das_call_member< bool (FMapProperty::*)(void *,int) const,&FMapProperty::IsValidIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3899:7
	makeExtern<DAS_CALL_METHOD(_method_1347), SimNode_ExtFuncCall >(lib,"IsValidIndex","das_call_member< bool (FMapProperty::*)(void *,int) const , &FMapProperty::IsValidIndex >::invoke")
		->args({"self","InMap","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1348 = das::das_call_member< unsigned char * (FMapProperty::*)(void *,int) const,&FMapProperty::GetPairPtr >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3908:9
	makeExtern<DAS_CALL_METHOD(_method_1348), SimNode_ExtFuncCall >(lib,"GetPairPtr","das_call_member< unsigned char * (FMapProperty::*)(void *,int) const , &FMapProperty::GetPairPtr >::invoke")
		->args({"self","InMap","Index"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

