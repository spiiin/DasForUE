// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_15() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2948:15
	makeExtern< void (*)(const FObjectPtrProperty &,void *,const void *) , _dasUnreal_virtual_504_CopyCompleteValueFromScriptVM , SimNode_ExtFuncCall >(lib,"CopyCompleteValueFromScriptVM","_dasUnreal_virtual_504_CopyCompleteValueFromScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2949:15
	makeExtern< void (*)(const FObjectPtrProperty &,void *,const void *) , _dasUnreal_virtual_505_CopyCompleteValueToScriptVM_InContainer , SimNode_ExtFuncCall >(lib,"CopyCompleteValueToScriptVM_InContainer","_dasUnreal_virtual_505_CopyCompleteValueToScriptVM_InContainer")
		->args({"self","OutValue","InContainer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2950:15
	makeExtern< void (*)(const FObjectPtrProperty &,void *,const void *) , _dasUnreal_virtual_506_CopyCompleteValueFromScriptVM_InContainer , SimNode_ExtFuncCall >(lib,"CopyCompleteValueFromScriptVM_InContainer","_dasUnreal_virtual_506_CopyCompleteValueFromScriptVM_InContainer")
		->args({"self","OutContainer","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1178 = das::das_call_member< FObjectPtr & (FObjectPtrProperty::*)(const void *) const,&FObjectPtrProperty::GetObjectPropertyValueAsPtr >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2959:14
	makeExtern<DAS_CALL_METHOD(_method_1178), SimNode_ExtFuncCallRef >(lib,"GetObjectPropertyValueAsPtr","das_call_member< FObjectPtr & (FObjectPtrProperty::*)(const void *) const , &FObjectPtrProperty::GetObjectPropertyValueAsPtr >::invoke")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2962:30
	makeExtern< TObjectPtr_UObject (*)(const FObjectPtrProperty &,const void *) , _dasUnreal_virtual_507_GetObjectPtrPropertyValue , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetObjectPtrPropertyValue","_dasUnreal_virtual_507_GetObjectPtrPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2963:19
	makeExtern< UObject * (*)(const FObjectPtrProperty &,const void *) , _dasUnreal_virtual_508_GetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue","_dasUnreal_virtual_508_GetObjectPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2964:19
	makeExtern< UObject * (*)(const FObjectPtrProperty &,const void *,int) , _dasUnreal_virtual_509_GetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue_InContainer","_dasUnreal_virtual_509_GetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2965:15
	makeExtern< void (*)(const FObjectPtrProperty &,void *,UObject *) , _dasUnreal_virtual_510_SetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue","_dasUnreal_virtual_510_SetObjectPropertyValue")
		->args({"self","PropertyValueAddress","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2966:15
	makeExtern< void (*)(const FObjectPtrProperty &,void *,UObject *,int) , _dasUnreal_virtual_511_SetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue_InContainer","_dasUnreal_virtual_511_SetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","Value","ArrayIndex"})
		->arg_init(3,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2967:15
	makeExtern< bool (*)(const FObjectPtrProperty &,const FObjectPropertyBase *) , _dasUnreal_virtual_512_AllowObjectTypeReinterpretationTo , SimNode_ExtFuncCall >(lib,"AllowObjectTypeReinterpretationTo","_dasUnreal_virtual_512_AllowObjectTypeReinterpretationTo")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FClassProperty,EInternal,FFieldClass *>(*this,lib,"FClassProperty","FClassProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3205:2
	makeExtern< FFieldClass * (*)(const FClassProperty *) , _dasUnreal_static_513_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_513_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FClassProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3205:2
	makeExtern< FField * (*)(const FClassProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_514_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_514_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FClassProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3205:2
	makeExtern< uint64 (*)(const FClassProperty *) , _dasUnreal_static_515_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_515_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FClassProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3205:2
	makeExtern< uint64 (*)(const FClassProperty *) , _dasUnreal_static_516_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_516_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FClassProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FClassProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FClassProperty","FClassProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FClassProperty,UField *>(*this,lib,"FClassProperty","FClassProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3238:15
	makeExtern< void (*)(FClassProperty &,FArchive &) , _dasUnreal_virtual_517_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_517_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3239:15
	makeExtern< void (*)(FClassProperty &,FReferenceCollector &) , _dasUnreal_virtual_518_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_518_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3240:15
	makeExtern< void (*)(FClassProperty &) , _dasUnreal_virtual_519_BeginDestroy , SimNode_ExtFuncCall >(lib,"BeginDestroy","_dasUnreal_virtual_519_BeginDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3244:15
	makeExtern< void (*)(FClassProperty &,const FField &) , _dasUnreal_virtual_520_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_520_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3247:18
	makeExtern< FString (*)(const FClassProperty &,FString *,unsigned int) , _dasUnreal_virtual_521_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_521_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3248:18
	makeExtern< FString (*)(const FClassProperty &,FString &) , _dasUnreal_virtual_522_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_522_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3249:18
	makeExtern< FString (*)(const FClassProperty &) , _dasUnreal_virtual_523_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_523_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3253:23
	makeExtern< const wchar_t * (*)(const FClassProperty &,const wchar_t *,void *,EPropertyPointerType,UObject *,int,FOutputDevice *) , _dasUnreal_virtual_524_ImportText_Internal , SimNode_ExtFuncCall >(lib,"ImportText_Internal","_dasUnreal_virtual_524_ImportText_Internal")
		->args({"self","Buffer","ContainerOrPropertyPtr","PropertyPointerType","OwnerObject","PortFlags","ErrorText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3254:15
	makeExtern< bool (*)(const FClassProperty &,const FProperty *) , _dasUnreal_virtual_525_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_525_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3255:15
	makeExtern< bool (*)(const FClassProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_526_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_526_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3257:15
	makeExtern< void (*)(const FClassProperty &,FBlake3 &,bool) , _dasUnreal_virtual_527_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_527_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3261:18
	makeExtern< FString (*)(const FClassProperty &,FString *,unsigned int,const FString &) , _dasUnreal_virtual_528_GetCPPTypeCustom , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeCustom","_dasUnreal_virtual_528_GetCPPTypeCustom")
		->args({"self","ExtendedTypeText","CPPExportFlags","InnerNativeTypeName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1201 = das::das_call_member< void (FClassProperty::*)(UClass *),&FClassProperty::SetMetaClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3273:7
	makeExtern<DAS_CALL_METHOD(_method_1201), SimNode_ExtFuncCall >(lib,"SetMetaClass","das_call_member< void (FClassProperty::*)(UClass *) , &FClassProperty::SetMetaClass >::invoke")
		->args({"self","NewMetaClass"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FClassPtrProperty,EInternal,FFieldClass *>(*this,lib,"FClassPtrProperty","FClassPtrProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3284:2
	makeExtern< FFieldClass * (*)(const FClassPtrProperty *) , _dasUnreal_static_529_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_529_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FClassPtrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3284:2
	makeExtern< FField * (*)(const FClassPtrProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_530_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_530_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FClassPtrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3284:2
	makeExtern< uint64 (*)(const FClassPtrProperty *) , _dasUnreal_static_531_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_531_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FClassPtrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3284:2
	makeExtern< uint64 (*)(const FClassPtrProperty *) , _dasUnreal_static_532_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_532_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FClassPtrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FClassPtrProperty,UField *>(*this,lib,"FClassPtrProperty","FClassPtrProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3296:18
	makeExtern< FString (*)(const FClassPtrProperty &,FString &) , _dasUnreal_virtual_533_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_533_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3297:18
	makeExtern< FString (*)(const FClassPtrProperty &,FString *,unsigned int) , _dasUnreal_virtual_534_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_534_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3301:15
	makeExtern< bool (*)(const FClassPtrProperty &,const FProperty *) , _dasUnreal_virtual_535_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_535_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3302:15
	makeExtern< bool (*)(const FClassPtrProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_536_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_536_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3304:15
	makeExtern< void (*)(const FClassPtrProperty &,void *,const void *) , _dasUnreal_virtual_537_CopySingleValueToScriptVM , SimNode_ExtFuncCall >(lib,"CopySingleValueToScriptVM","_dasUnreal_virtual_537_CopySingleValueToScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3305:15
	makeExtern< void (*)(const FClassPtrProperty &,void *,const void *) , _dasUnreal_virtual_538_CopySingleValueFromScriptVM , SimNode_ExtFuncCall >(lib,"CopySingleValueFromScriptVM","_dasUnreal_virtual_538_CopySingleValueFromScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3306:15
	makeExtern< void (*)(const FClassPtrProperty &,void *,const void *) , _dasUnreal_virtual_539_CopyCompleteValueToScriptVM , SimNode_ExtFuncCall >(lib,"CopyCompleteValueToScriptVM","_dasUnreal_virtual_539_CopyCompleteValueToScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3307:15
	makeExtern< void (*)(const FClassPtrProperty &,void *,const void *) , _dasUnreal_virtual_540_CopyCompleteValueFromScriptVM , SimNode_ExtFuncCall >(lib,"CopyCompleteValueFromScriptVM","_dasUnreal_virtual_540_CopyCompleteValueFromScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3308:15
	makeExtern< void (*)(const FClassPtrProperty &,void *,const void *) , _dasUnreal_virtual_541_CopyCompleteValueToScriptVM_InContainer , SimNode_ExtFuncCall >(lib,"CopyCompleteValueToScriptVM_InContainer","_dasUnreal_virtual_541_CopyCompleteValueToScriptVM_InContainer")
		->args({"self","OutValue","InContainer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3309:15
	makeExtern< void (*)(const FClassPtrProperty &,void *,const void *) , _dasUnreal_virtual_542_CopyCompleteValueFromScriptVM_InContainer , SimNode_ExtFuncCall >(lib,"CopyCompleteValueFromScriptVM_InContainer","_dasUnreal_virtual_542_CopyCompleteValueFromScriptVM_InContainer")
		->args({"self","OutContainer","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3313:19
	makeExtern< UObject * (*)(const FClassPtrProperty &,const void *) , _dasUnreal_virtual_543_GetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue","_dasUnreal_virtual_543_GetObjectPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3314:19
	makeExtern< UObject * (*)(const FClassPtrProperty &,const void *,int) , _dasUnreal_virtual_544_GetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue_InContainer","_dasUnreal_virtual_544_GetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3315:15
	makeExtern< void (*)(const FClassPtrProperty &,void *,UObject *) , _dasUnreal_virtual_545_SetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue","_dasUnreal_virtual_545_SetObjectPropertyValue")
		->args({"self","PropertyValueAddress","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3316:15
	makeExtern< void (*)(const FClassPtrProperty &,void *,UObject *,int) , _dasUnreal_virtual_546_SetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue_InContainer","_dasUnreal_virtual_546_SetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","Value","ArrayIndex"})
		->arg_init(3,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStrProperty,EInternal,FFieldClass *>(*this,lib,"FStrProperty","FStrProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3566:2
	makeExtern< FFieldClass * (*)(const FStrProperty *) , _dasUnreal_static_547_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_547_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3566:2
	makeExtern< FField * (*)(const FStrProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_548_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_548_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FStrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3566:2
	makeExtern< uint64 (*)(const FStrProperty *) , _dasUnreal_static_549_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_549_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3566:2
	makeExtern< uint64 (*)(const FStrProperty *) , _dasUnreal_static_550_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_550_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FStrProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FStrProperty","FStrProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FStrProperty,UField *>(*this,lib,"FStrProperty","FStrProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3604:18
	makeExtern< FString (*)(const FStrProperty &) , _dasUnreal_virtual_551_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_551_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3605:9
	makeExtern< unsigned int (*)(const FStrProperty &,const void *) , _dasUnreal_virtual_552_GetValueTypeHashInternal , SimNode_ExtFuncCall >(lib,"GetValueTypeHashInternal","_dasUnreal_virtual_552_GetValueTypeHashInternal")
		->args({"self","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FArrayProperty,EInternal,FFieldClass *>(*this,lib,"FArrayProperty","FArrayProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3630:2
	makeExtern< FFieldClass * (*)(const FArrayProperty *) , _dasUnreal_static_553_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_553_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FArrayProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3630:2
	makeExtern< FField * (*)(const FArrayProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_554_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_554_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FArrayProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3630:2
	makeExtern< uint64 (*)(const FArrayProperty *) , _dasUnreal_static_555_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_555_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FArrayProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3630:2
	makeExtern< uint64 (*)(const FArrayProperty *) , _dasUnreal_static_556_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_556_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FArrayProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FArrayProperty,FFieldVariant,const FName &,EObjectFlags,EArrayPropertyFlags>(*this,lib,"FArrayProperty","FArrayProperty")
		->args({"InOwner","InName","InObjectFlags","InArrayPropertyFlags"})
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<EArrayPropertyFlags>(lib)));
	addCtorAndUsing<FArrayProperty,UField *>(*this,lib,"FArrayProperty","FArrayProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3681:15
	makeExtern< void (*)(FArrayProperty &,FArchive &) , _dasUnreal_virtual_557_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_557_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3682:15
	makeExtern< void (*)(FArrayProperty &,FReferenceCollector &) , _dasUnreal_virtual_558_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_558_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3687:15
	makeExtern< void (*)(FArrayProperty &,const FField &) , _dasUnreal_virtual_559_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_559_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3690:15
	makeExtern< void (*)(FArrayProperty &,FProperty *) , _dasUnreal_virtual_560_AddCppProperty , SimNode_ExtFuncCall >(lib,"AddCppProperty","_dasUnreal_virtual_560_AddCppProperty")
		->args({"self","Property"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3691:18
	makeExtern< FField * (*)(FArrayProperty &,const FName &) , _dasUnreal_virtual_561_GetInnerFieldByName , SimNode_ExtFuncCall >(lib,"GetInnerFieldByName","_dasUnreal_virtual_561_GetInnerFieldByName")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3697:18
	makeExtern< FString (*)(const FArrayProperty &,FString &) , _dasUnreal_virtual_562_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_562_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3698:18
	makeExtern< FString (*)(const FArrayProperty &,FString *,unsigned int) , _dasUnreal_virtual_563_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_563_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3699:18
	makeExtern< FString (*)(const FArrayProperty &) , _dasUnreal_virtual_564_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_564_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3700:15
	makeExtern< void (*)(FArrayProperty &,FArchive &) , _dasUnreal_virtual_565_LinkInternal , SimNode_ExtFuncCall >(lib,"LinkInternal","_dasUnreal_virtual_565_LinkInternal")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3701:15
	makeExtern< bool (*)(const FArrayProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_566_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_566_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3708:15
	makeExtern< void (*)(const FArrayProperty &,void *) , _dasUnreal_virtual_567_InitializeValueInternal , SimNode_ExtFuncCall >(lib,"InitializeValueInternal","_dasUnreal_virtual_567_InitializeValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3727:15
	makeExtern< void (*)(const FArrayProperty &,void *,const void *,int) , _dasUnreal_virtual_568_CopyValuesInternal , SimNode_ExtFuncCall >(lib,"CopyValuesInternal","_dasUnreal_virtual_568_CopyValuesInternal")
		->args({"self","Dest","Src","Count"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3728:15
	makeExtern< void (*)(const FArrayProperty &,void *) , _dasUnreal_virtual_569_ClearValueInternal , SimNode_ExtFuncCall >(lib,"ClearValueInternal","_dasUnreal_virtual_569_ClearValueInternal")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3729:15
	makeExtern< void (*)(const FArrayProperty &,void *) , _dasUnreal_virtual_570_DestroyValueInternal , SimNode_ExtFuncCall >(lib,"DestroyValueInternal","_dasUnreal_virtual_570_DestroyValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3730:15
	makeExtern< bool (*)(const FArrayProperty &) , _dasUnreal_virtual_571_PassCPPArgsByRef , SimNode_ExtFuncCall >(lib,"PassCPPArgsByRef","_dasUnreal_virtual_571_PassCPPArgsByRef")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3734:15
	makeExtern< bool (*)(const FArrayProperty &,const FProperty *) , _dasUnreal_virtual_572_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_572_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3737:16
	makeExtern< int (*)(const FArrayProperty &) , _dasUnreal_virtual_573_GetMinAlignment , SimNode_ExtFuncCall >(lib,"GetMinAlignment","_dasUnreal_virtual_573_GetMinAlignment")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3743:16
	makeExtern< void * (*)(const FArrayProperty &,const FProperty *,void *,int) , _dasUnreal_virtual_574_GetValueAddressAtIndex_Direct , SimNode_ExtFuncCall >(lib,"GetValueAddressAtIndex_Direct","_dasUnreal_virtual_574_GetValueAddressAtIndex_Direct")
		->args({"self","Inner","InValueAddress","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1248 = das::das_call_member< FString (FArrayProperty::*)(FString *,unsigned int,const FString &,const FString &) const,&FArrayProperty::GetCPPTypeCustom >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3746:10
	makeExtern<DAS_CALL_METHOD(_method_1248), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeCustom","das_call_member< FString (FArrayProperty::*)(FString *,unsigned int,const FString &,const FString &) const , &FArrayProperty::GetCPPTypeCustom >::invoke")
		->args({"self","ExtendedTypeText","CPPExportFlags","InnerTypeText","InInnerExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3749:14
	makeExtern< void (*)(const FArrayProperty *,FString &,const FProperty *,const void *,int,const void *,int,UObject *,int,UObject *) , _dasUnreal_static_575_ExportTextInnerItem , SimNode_ExtFuncCall >(lib,"ExportTextInnerItem","_dasUnreal_static_575_ExportTextInnerItem")
		->args({"self","ValueStr","Inner","PropertyValue","PropertySize","DefaultValue","DefaultSize","Parent","PortFlags","ExportRootScope"})
		->arg_type(0,makeType<TExplicit<const FArrayProperty*> >(lib))
		->arg_init(7,make_smart<ExprConstPtr>())
		->arg_init(8,make_smart<ExprConstInt>(0))
		->arg_init(9,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3754:15
	makeExtern< void (*)(const FArrayProperty &,FBlake3 &,bool) , _dasUnreal_virtual_576_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_576_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FMapProperty,EInternal,FFieldClass *>(*this,lib,"FMapProperty","FMapProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3774:2
	makeExtern< FFieldClass * (*)(const FMapProperty *) , _dasUnreal_static_577_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_577_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FMapProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3774:2
	makeExtern< FField * (*)(const FMapProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_578_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_578_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FMapProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3774:2
	makeExtern< uint64 (*)(const FMapProperty *) , _dasUnreal_static_579_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_579_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FMapProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3774:2
	makeExtern< uint64 (*)(const FMapProperty *) , _dasUnreal_static_580_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_580_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FMapProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FMapProperty,FFieldVariant,const FName &,EObjectFlags,EMapPropertyFlags>(*this,lib,"FMapProperty","FMapProperty")
		->args({"InOwner","InName","InObjectFlags","InMapFlags"})
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<EMapPropertyFlags>(lib)));
	addCtorAndUsing<FMapProperty,UField *>(*this,lib,"FMapProperty","FMapProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3818:15
	makeExtern< void (*)(FMapProperty &,FArchive &) , _dasUnreal_virtual_581_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_581_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3819:15
	makeExtern< void (*)(FMapProperty &,FReferenceCollector &) , _dasUnreal_virtual_582_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_582_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3824:15
	makeExtern< void (*)(FMapProperty &,const FField &) , _dasUnreal_virtual_583_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_583_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3825:18
	makeExtern< FField * (*)(FMapProperty &,const FName &) , _dasUnreal_virtual_584_GetInnerFieldByName , SimNode_ExtFuncCall >(lib,"GetInnerFieldByName","_dasUnreal_virtual_584_GetInnerFieldByName")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3829:15
	makeExtern< void (*)(FMapProperty &,FProperty *) , _dasUnreal_virtual_585_AddCppProperty , SimNode_ExtFuncCall >(lib,"AddCppProperty","_dasUnreal_virtual_585_AddCppProperty")
		->args({"self","Property"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3833:18
	makeExtern< FString (*)(const FMapProperty &,FString &) , _dasUnreal_virtual_586_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_586_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

