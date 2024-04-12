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
	addCtorAndUsing<FWeakObjectProperty,EInternal,FFieldClass *>(*this,lib,"FWeakObjectProperty","FWeakObjectProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2979:2
	makeExtern< FFieldClass * (*)(const FWeakObjectProperty *) , _dasUnreal_static_513_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_513_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FWeakObjectProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2979:2
	makeExtern< FField * (*)(const FWeakObjectProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_514_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_514_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FWeakObjectProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2979:2
	makeExtern< uint64 (*)(const FWeakObjectProperty *) , _dasUnreal_static_515_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_515_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FWeakObjectProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2979:2
	makeExtern< uint64 (*)(const FWeakObjectProperty *) , _dasUnreal_static_516_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_516_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FWeakObjectProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FWeakObjectProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FWeakObjectProperty","FWeakObjectProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FWeakObjectProperty,UField *>(*this,lib,"FWeakObjectProperty","FWeakObjectProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3009:18
	makeExtern< FString (*)(const FWeakObjectProperty &,FString &) , _dasUnreal_virtual_517_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_517_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3010:18
	makeExtern< FString (*)(const FWeakObjectProperty &,FString *,unsigned int) , _dasUnreal_virtual_518_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_518_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3011:18
	makeExtern< FString (*)(const FWeakObjectProperty &,FString *,unsigned int,const FString &) , _dasUnreal_virtual_519_GetCPPTypeCustom , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeCustom","_dasUnreal_virtual_519_GetCPPTypeCustom")
		->args({"self","ExtendedTypeText","CPPExportFlags","InnerNativeTypeName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3012:18
	makeExtern< FString (*)(const FWeakObjectProperty &) , _dasUnreal_virtual_520_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_520_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3022:15
	makeExtern< void (*)(const FWeakObjectProperty &,void *,const void *) , _dasUnreal_virtual_521_CopySingleValueToScriptVM , SimNode_ExtFuncCall >(lib,"CopySingleValueToScriptVM","_dasUnreal_virtual_521_CopySingleValueToScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3023:15
	makeExtern< void (*)(const FWeakObjectProperty &,void *,const void *) , _dasUnreal_virtual_522_CopySingleValueFromScriptVM , SimNode_ExtFuncCall >(lib,"CopySingleValueFromScriptVM","_dasUnreal_virtual_522_CopySingleValueFromScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3024:15
	makeExtern< void (*)(const FWeakObjectProperty &,void *,const void *) , _dasUnreal_virtual_523_CopyCompleteValueToScriptVM , SimNode_ExtFuncCall >(lib,"CopyCompleteValueToScriptVM","_dasUnreal_virtual_523_CopyCompleteValueToScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3025:15
	makeExtern< void (*)(const FWeakObjectProperty &,void *,const void *) , _dasUnreal_virtual_524_CopyCompleteValueFromScriptVM , SimNode_ExtFuncCall >(lib,"CopyCompleteValueFromScriptVM","_dasUnreal_virtual_524_CopyCompleteValueFromScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3026:15
	makeExtern< void (*)(const FWeakObjectProperty &,void *,const void *) , _dasUnreal_virtual_525_CopyCompleteValueToScriptVM_InContainer , SimNode_ExtFuncCall >(lib,"CopyCompleteValueToScriptVM_InContainer","_dasUnreal_virtual_525_CopyCompleteValueToScriptVM_InContainer")
		->args({"self","OutValue","InContainer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3027:15
	makeExtern< void (*)(const FWeakObjectProperty &,void *,const void *) , _dasUnreal_virtual_526_CopyCompleteValueFromScriptVM_InContainer , SimNode_ExtFuncCall >(lib,"CopyCompleteValueFromScriptVM_InContainer","_dasUnreal_virtual_526_CopyCompleteValueFromScriptVM_InContainer")
		->args({"self","OutContainer","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3031:30
	makeExtern< TObjectPtr_UObject (*)(const FWeakObjectProperty &,const void *) , _dasUnreal_virtual_527_GetObjectPtrPropertyValue , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetObjectPtrPropertyValue","_dasUnreal_virtual_527_GetObjectPtrPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3032:19
	makeExtern< UObject * (*)(const FWeakObjectProperty &,const void *) , _dasUnreal_virtual_528_GetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue","_dasUnreal_virtual_528_GetObjectPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3033:19
	makeExtern< UObject * (*)(const FWeakObjectProperty &,const void *,int) , _dasUnreal_virtual_529_GetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue_InContainer","_dasUnreal_virtual_529_GetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3034:15
	makeExtern< void (*)(const FWeakObjectProperty &,void *,UObject *) , _dasUnreal_virtual_530_SetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue","_dasUnreal_virtual_530_SetObjectPropertyValue")
		->args({"self","PropertyValueAddress","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3035:15
	makeExtern< void (*)(const FWeakObjectProperty &,void *,UObject *,int) , _dasUnreal_virtual_531_SetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue_InContainer","_dasUnreal_virtual_531_SetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","Value","ArrayIndex"})
		->arg_init(3,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FLazyObjectProperty,EInternal,FFieldClass *>(*this,lib,"FLazyObjectProperty","FLazyObjectProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3044:2
	makeExtern< FFieldClass * (*)(const FLazyObjectProperty *) , _dasUnreal_static_532_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_532_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FLazyObjectProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3044:2
	makeExtern< FField * (*)(const FLazyObjectProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_533_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_533_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FLazyObjectProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3044:2
	makeExtern< uint64 (*)(const FLazyObjectProperty *) , _dasUnreal_static_534_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_534_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FLazyObjectProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3044:2
	makeExtern< uint64 (*)(const FLazyObjectProperty *) , _dasUnreal_static_535_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_535_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FLazyObjectProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FLazyObjectProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FLazyObjectProperty","FLazyObjectProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FLazyObjectProperty,UField *>(*this,lib,"FLazyObjectProperty","FLazyObjectProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3074:18
	makeExtern< FString (*)(const FLazyObjectProperty &,FString &) , _dasUnreal_virtual_536_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_536_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3075:18
	makeExtern< FString (*)(const FLazyObjectProperty &,FString *,unsigned int) , _dasUnreal_virtual_537_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_537_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3076:18
	makeExtern< FString (*)(const FLazyObjectProperty &,FString *,unsigned int,const FString &) , _dasUnreal_virtual_538_GetCPPTypeCustom , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeCustom","_dasUnreal_virtual_538_GetCPPTypeCustom")
		->args({"self","ExtendedTypeText","CPPExportFlags","InnerNativeTypeName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3077:18
	makeExtern< FString (*)(const FLazyObjectProperty &) , _dasUnreal_virtual_539_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_539_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3082:16
	makeExtern< FName (*)(const FLazyObjectProperty &) , _dasUnreal_virtual_540_GetID , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetID","_dasUnreal_virtual_540_GetID")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3083:15
	makeExtern< bool (*)(const FLazyObjectProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_541_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_541_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3085:15
	makeExtern< void (*)(const FLazyObjectProperty &,void *,const void *) , _dasUnreal_virtual_542_CopySingleValueToScriptVM , SimNode_ExtFuncCall >(lib,"CopySingleValueToScriptVM","_dasUnreal_virtual_542_CopySingleValueToScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3086:15
	makeExtern< void (*)(const FLazyObjectProperty &,void *,const void *) , _dasUnreal_virtual_543_CopySingleValueFromScriptVM , SimNode_ExtFuncCall >(lib,"CopySingleValueFromScriptVM","_dasUnreal_virtual_543_CopySingleValueFromScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3087:15
	makeExtern< void (*)(const FLazyObjectProperty &,void *,const void *) , _dasUnreal_virtual_544_CopyCompleteValueToScriptVM , SimNode_ExtFuncCall >(lib,"CopyCompleteValueToScriptVM","_dasUnreal_virtual_544_CopyCompleteValueToScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3088:15
	makeExtern< void (*)(const FLazyObjectProperty &,void *,const void *) , _dasUnreal_virtual_545_CopyCompleteValueFromScriptVM , SimNode_ExtFuncCall >(lib,"CopyCompleteValueFromScriptVM","_dasUnreal_virtual_545_CopyCompleteValueFromScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3089:15
	makeExtern< void (*)(const FLazyObjectProperty &,void *,const void *) , _dasUnreal_virtual_546_CopyCompleteValueToScriptVM_InContainer , SimNode_ExtFuncCall >(lib,"CopyCompleteValueToScriptVM_InContainer","_dasUnreal_virtual_546_CopyCompleteValueToScriptVM_InContainer")
		->args({"self","OutValue","InContainer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3090:15
	makeExtern< void (*)(const FLazyObjectProperty &,void *,const void *) , _dasUnreal_virtual_547_CopyCompleteValueFromScriptVM_InContainer , SimNode_ExtFuncCall >(lib,"CopyCompleteValueFromScriptVM_InContainer","_dasUnreal_virtual_547_CopyCompleteValueFromScriptVM_InContainer")
		->args({"self","OutContainer","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3094:30
	makeExtern< TObjectPtr_UObject (*)(const FLazyObjectProperty &,const void *) , _dasUnreal_virtual_548_GetObjectPtrPropertyValue , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetObjectPtrPropertyValue","_dasUnreal_virtual_548_GetObjectPtrPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3095:19
	makeExtern< UObject * (*)(const FLazyObjectProperty &,const void *) , _dasUnreal_virtual_549_GetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue","_dasUnreal_virtual_549_GetObjectPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3096:19
	makeExtern< UObject * (*)(const FLazyObjectProperty &,const void *,int) , _dasUnreal_virtual_550_GetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue_InContainer","_dasUnreal_virtual_550_GetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3097:15
	makeExtern< void (*)(const FLazyObjectProperty &,void *,UObject *) , _dasUnreal_virtual_551_SetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue","_dasUnreal_virtual_551_SetObjectPropertyValue")
		->args({"self","PropertyValueAddress","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3098:15
	makeExtern< void (*)(const FLazyObjectProperty &,void *,UObject *,int) , _dasUnreal_virtual_552_SetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue_InContainer","_dasUnreal_virtual_552_SetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","Value","ArrayIndex"})
		->arg_init(3,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3099:15
	makeExtern< bool (*)(const FLazyObjectProperty &) , _dasUnreal_virtual_553_AllowCrossLevel , SimNode_ExtFuncCall >(lib,"AllowCrossLevel","_dasUnreal_virtual_553_AllowCrossLevel")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FSoftObjectProperty,EInternal,FFieldClass *>(*this,lib,"FSoftObjectProperty","FSoftObjectProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3111:2
	makeExtern< FFieldClass * (*)(const FSoftObjectProperty *) , _dasUnreal_static_554_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_554_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FSoftObjectProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3111:2
	makeExtern< FField * (*)(const FSoftObjectProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_555_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_555_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FSoftObjectProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3111:2
	makeExtern< uint64 (*)(const FSoftObjectProperty *) , _dasUnreal_static_556_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_556_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FSoftObjectProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3111:2
	makeExtern< uint64 (*)(const FSoftObjectProperty *) , _dasUnreal_static_557_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_557_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FSoftObjectProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FSoftObjectProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FSoftObjectProperty","FSoftObjectProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FSoftObjectProperty,UField *>(*this,lib,"FSoftObjectProperty","FSoftObjectProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3149:18
	makeExtern< FString (*)(const FSoftObjectProperty &,FString &) , _dasUnreal_virtual_558_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_558_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3150:18
	makeExtern< FString (*)(const FSoftObjectProperty &) , _dasUnreal_virtual_559_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_559_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3154:16
	makeExtern< FName (*)(const FSoftObjectProperty &) , _dasUnreal_virtual_560_GetID , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetID","_dasUnreal_virtual_560_GetID")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3155:15
	makeExtern< bool (*)(const FSoftObjectProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_561_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_561_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3167:19
	makeExtern< UObject * (*)(const FSoftObjectProperty &,const void *) , _dasUnreal_virtual_562_LoadObjectPropertyValue , SimNode_ExtFuncCall >(lib,"LoadObjectPropertyValue","_dasUnreal_virtual_562_LoadObjectPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3168:30
	makeExtern< TObjectPtr_UObject (*)(const FSoftObjectProperty &,const void *) , _dasUnreal_virtual_563_GetObjectPtrPropertyValue , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetObjectPtrPropertyValue","_dasUnreal_virtual_563_GetObjectPtrPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3169:19
	makeExtern< UObject * (*)(const FSoftObjectProperty &,const void *) , _dasUnreal_virtual_564_GetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue","_dasUnreal_virtual_564_GetObjectPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3170:19
	makeExtern< UObject * (*)(const FSoftObjectProperty &,const void *,int) , _dasUnreal_virtual_565_GetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"GetObjectPropertyValue_InContainer","_dasUnreal_virtual_565_GetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3171:15
	makeExtern< void (*)(const FSoftObjectProperty &,void *,UObject *) , _dasUnreal_virtual_566_SetObjectPropertyValue , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue","_dasUnreal_virtual_566_SetObjectPropertyValue")
		->args({"self","PropertyValueAddress","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3172:15
	makeExtern< void (*)(const FSoftObjectProperty &,void *,UObject *,int) , _dasUnreal_virtual_567_SetObjectPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"SetObjectPropertyValue_InContainer","_dasUnreal_virtual_567_SetObjectPropertyValue_InContainer")
		->args({"self","ContainerAddress","Value","ArrayIndex"})
		->arg_init(3,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3173:15
	makeExtern< bool (*)(const FSoftObjectProperty &) , _dasUnreal_virtual_568_AllowCrossLevel , SimNode_ExtFuncCall >(lib,"AllowCrossLevel","_dasUnreal_virtual_568_AllowCrossLevel")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3174:18
	makeExtern< FString (*)(const FSoftObjectProperty &,FString *,unsigned int,const FString &) , _dasUnreal_virtual_569_GetCPPTypeCustom , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeCustom","_dasUnreal_virtual_569_GetCPPTypeCustom")
		->args({"self","ExtendedTypeText","CPPExportFlags","InnerNativeTypeName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3176:18
	makeExtern< FString (*)(const FSoftObjectProperty &,FString *,unsigned int) , _dasUnreal_virtual_570_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_570_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FClassProperty,EInternal,FFieldClass *>(*this,lib,"FClassProperty","FClassProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3205:2
	makeExtern< FFieldClass * (*)(const FClassProperty *) , _dasUnreal_static_571_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_571_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FClassProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3205:2
	makeExtern< FField * (*)(const FClassProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_572_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_572_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FClassProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3205:2
	makeExtern< uint64 (*)(const FClassProperty *) , _dasUnreal_static_573_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_573_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FClassProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3205:2
	makeExtern< uint64 (*)(const FClassProperty *) , _dasUnreal_static_574_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_574_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FClassProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FClassProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FClassProperty","FClassProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FClassProperty,UField *>(*this,lib,"FClassProperty","FClassProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3238:15
	makeExtern< void (*)(FClassProperty &,FArchive &) , _dasUnreal_virtual_575_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_575_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3239:15
	makeExtern< void (*)(FClassProperty &,FReferenceCollector &) , _dasUnreal_virtual_576_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_576_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3240:15
	makeExtern< void (*)(FClassProperty &) , _dasUnreal_virtual_577_BeginDestroy , SimNode_ExtFuncCall >(lib,"BeginDestroy","_dasUnreal_virtual_577_BeginDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3244:15
	makeExtern< void (*)(FClassProperty &,const FField &) , _dasUnreal_virtual_578_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_578_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3247:18
	makeExtern< FString (*)(const FClassProperty &,FString *,unsigned int) , _dasUnreal_virtual_579_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_579_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3248:18
	makeExtern< FString (*)(const FClassProperty &,FString &) , _dasUnreal_virtual_580_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_580_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3249:18
	makeExtern< FString (*)(const FClassProperty &) , _dasUnreal_virtual_581_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_581_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3253:23
	makeExtern< const wchar_t * (*)(const FClassProperty &,const wchar_t *,void *,EPropertyPointerType,UObject *,int,FOutputDevice *) , _dasUnreal_virtual_582_ImportText_Internal , SimNode_ExtFuncCall >(lib,"ImportText_Internal","_dasUnreal_virtual_582_ImportText_Internal")
		->args({"self","Buffer","ContainerOrPropertyPtr","PropertyPointerType","OwnerObject","PortFlags","ErrorText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3254:15
	makeExtern< bool (*)(const FClassProperty &,const FProperty *) , _dasUnreal_virtual_583_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_583_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3255:15
	makeExtern< bool (*)(const FClassProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_584_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_584_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3257:15
	makeExtern< void (*)(const FClassProperty &,FBlake3 &,bool) , _dasUnreal_virtual_585_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_585_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3261:18
	makeExtern< FString (*)(const FClassProperty &,FString *,unsigned int,const FString &) , _dasUnreal_virtual_586_GetCPPTypeCustom , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeCustom","_dasUnreal_virtual_586_GetCPPTypeCustom")
		->args({"self","ExtendedTypeText","CPPExportFlags","InnerNativeTypeName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1259 = das::das_call_member< void (FClassProperty::*)(UClass *),&FClassProperty::SetMetaClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3273:7
	makeExtern<DAS_CALL_METHOD(_method_1259), SimNode_ExtFuncCall >(lib,"SetMetaClass","das_call_member< void (FClassProperty::*)(UClass *) , &FClassProperty::SetMetaClass >::invoke")
		->args({"self","NewMetaClass"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FClassPtrProperty,EInternal,FFieldClass *>(*this,lib,"FClassPtrProperty","FClassPtrProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3284:2
	makeExtern< FFieldClass * (*)(const FClassPtrProperty *) , _dasUnreal_static_587_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_587_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FClassPtrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:3284:2
	makeExtern< FField * (*)(const FClassPtrProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_588_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_588_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FClassPtrProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
}
}

