// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_10() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:815:31
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_248_CopySingleValueToScriptVM , SimNode_ExtFuncCall >(lib,"CopySingleValueToScriptVM","_dasUnreal_virtual_248_CopySingleValueToScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:825:31
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_249_CopyCompleteValueToScriptVM , SimNode_ExtFuncCall >(lib,"CopyCompleteValueToScriptVM","_dasUnreal_virtual_249_CopyCompleteValueToScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:830:31
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_250_CopyCompleteValueToScriptVM_InContainer , SimNode_ExtFuncCall >(lib,"CopyCompleteValueToScriptVM_InContainer","_dasUnreal_virtual_250_CopyCompleteValueToScriptVM_InContainer")
		->args({"self","OutValue","InContainer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:831:31
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_251_CopyCompleteValueFromScriptVM_InContainer , SimNode_ExtFuncCall >(lib,"CopyCompleteValueFromScriptVM_InContainer","_dasUnreal_virtual_251_CopyCompleteValueFromScriptVM_InContainer")
		->args({"self","OutContainer","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:843:31
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_252_CopySingleValueFromScriptVM , SimNode_ExtFuncCall >(lib,"CopySingleValueFromScriptVM","_dasUnreal_virtual_252_CopySingleValueFromScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:853:31
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_253_CopyCompleteValueFromScriptVM , SimNode_ExtFuncCall >(lib,"CopyCompleteValueFromScriptVM","_dasUnreal_virtual_253_CopyCompleteValueFromScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_750 = das::das_call_member< void (FProperty::*)(void *) const,&FProperty::ClearValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:861:19
	makeExtern<DAS_CALL_METHOD(_method_750), SimNode_ExtFuncCall >(lib,"ClearValue","das_call_member< void (FProperty::*)(void *) const , &FProperty::ClearValue >::invoke")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_751 = das::das_call_member< void (FProperty::*)(void *,int) const,&FProperty::ClearValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:878:19
	makeExtern<DAS_CALL_METHOD(_method_751), SimNode_ExtFuncCall >(lib,"ClearValue_InContainer","das_call_member< void (FProperty::*)(void *,int) const , &FProperty::ClearValue_InContainer >::invoke")
		->args({"self","Data","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_752 = das::das_call_member< void (FProperty::*)(void *) const,&FProperty::DestroyValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:898:19
	makeExtern<DAS_CALL_METHOD(_method_752), SimNode_ExtFuncCall >(lib,"DestroyValue","das_call_member< void (FProperty::*)(void *) const , &FProperty::DestroyValue >::invoke")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_753 = das::das_call_member< void (FProperty::*)(void *) const,&FProperty::DestroyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:911:19
	makeExtern<DAS_CALL_METHOD(_method_753), SimNode_ExtFuncCall >(lib,"DestroyValue_InContainer","das_call_member< void (FProperty::*)(void *) const , &FProperty::DestroyValue_InContainer >::invoke")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_754 = das::das_call_member< void (FProperty::*)(void *) const,&FProperty::InitializeValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:929:19
	makeExtern<DAS_CALL_METHOD(_method_754), SimNode_ExtFuncCall >(lib,"InitializeValue","das_call_member< void (FProperty::*)(void *) const , &FProperty::InitializeValue >::invoke")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_755 = das::das_call_member< void (FProperty::*)(void *) const,&FProperty::InitializeValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:947:19
	makeExtern<DAS_CALL_METHOD(_method_755), SimNode_ExtFuncCall >(lib,"InitializeValue_InContainer","das_call_member< void (FProperty::*)(void *) const , &FProperty::InitializeValue_InContainer >::invoke")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_756 = das::das_call_member< bool (FProperty::*)(unsigned int) const,&FProperty::ShouldPort >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:971:23
	makeExtern<DAS_CALL_METHOD(_method_756), SimNode_ExtFuncCall >(lib,"ShouldPort","das_call_member< bool (FProperty::*)(unsigned int) const , &FProperty::ShouldPort >::invoke")
		->args({"self","PortFlags"})
		->arg_init(1,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:972:32
	makeExtern< FName (*)(const FProperty &) , _dasUnreal_virtual_254_GetID , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetID","_dasUnreal_virtual_254_GetID")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:984:32
	makeExtern< int (*)(const FProperty &) , _dasUnreal_virtual_255_GetMinAlignment , SimNode_ExtFuncCall >(lib,"GetMinAlignment","_dasUnreal_virtual_255_GetMinAlignment")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_759 = das::das_call_member< bool (FProperty::*)() const,&FProperty::ContainsWeakObjectReference >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1002:7
	makeExtern<DAS_CALL_METHOD(_method_759), SimNode_ExtFuncCall >(lib,"ContainsWeakObjectReference","das_call_member< bool (FProperty::*)() const , &FProperty::ContainsWeakObjectReference >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_760 = das::das_call_member< bool (FProperty::*)() const,&FProperty::ContainsInstancedObjectProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1014:19
	makeExtern<DAS_CALL_METHOD(_method_760), SimNode_ExtFuncCall >(lib,"ContainsInstancedObjectProperty","das_call_member< bool (FProperty::*)() const , &FProperty::ContainsInstancedObjectProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_761 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1027:20
	makeExtern<DAS_CALL_METHOD(_method_761), SimNode_ExtFuncCall >(lib,"GetSize","das_call_member< int (FProperty::*)() const , &FProperty::GetSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_762 = das::das_call_member< bool (FProperty::*)() const,&FProperty::ShouldDuplicateValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1038:7
	makeExtern<DAS_CALL_METHOD(_method_762), SimNode_ExtFuncCall >(lib,"ShouldDuplicateValue","das_call_member< bool (FProperty::*)() const , &FProperty::ShouldDuplicateValue >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_763 = das::das_call_member< FProperty * (FProperty::*)(),&FProperty::GetOwnerProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1046:13
	makeExtern<DAS_CALL_METHOD(_method_763), SimNode_ExtFuncCall >(lib,"GetOwnerProperty","das_call_member< FProperty * (FProperty::*)() , &FProperty::GetOwnerProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_764 = das::das_call_member< const FProperty * (FProperty::*)() const,&FProperty::GetOwnerProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1056:19
	makeExtern<DAS_CALL_METHOD(_method_764), SimNode_ExtFuncCall >(lib,"GetOwnerProperty","das_call_member< const FProperty * (FProperty::*)() const , &FProperty::GetOwnerProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_765 = das::das_call_member< bool (FProperty::*)(uint64) const,&FProperty::HasAnyPropertyFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1091:19
	makeExtern<DAS_CALL_METHOD(_method_765), SimNode_ExtFuncCall >(lib,"HasAnyPropertyFlags","das_call_member< bool (FProperty::*)(uint64) const , &FProperty::HasAnyPropertyFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_766 = das::das_call_member< bool (FProperty::*)(uint64) const,&FProperty::HasAllPropertyFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1105:19
	makeExtern<DAS_CALL_METHOD(_method_766), SimNode_ExtFuncCall >(lib,"HasAllPropertyFlags","das_call_member< bool (FProperty::*)(uint64) const , &FProperty::HasAllPropertyFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_767 = das::das_call_member< FProperty * (FProperty::*)(),&FProperty::GetRepOwner >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1114:25
	makeExtern<DAS_CALL_METHOD(_method_767), SimNode_ExtFuncCall >(lib,"GetRepOwner","das_call_member< FProperty * (FProperty::*)() , &FProperty::GetRepOwner >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_768 = das::das_call_member< bool (FProperty::*)() const,&FProperty::IsEditorOnlyProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1126:19
	makeExtern<DAS_CALL_METHOD(_method_768), SimNode_ExtFuncCall >(lib,"IsEditorOnlyProperty","das_call_member< bool (FProperty::*)() const , &FProperty::IsEditorOnlyProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1132:31
	makeExtern< bool (*)(const FProperty &,const FProperty *) , _dasUnreal_virtual_256_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_256_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_770 = das::das_call_member< ELifetimeCondition (FProperty::*)() const,&FProperty::GetBlueprintReplicationCondition >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1134:21
	makeExtern<DAS_CALL_METHOD(_method_770), SimNode_ExtFuncCall >(lib,"GetBlueprintReplicationCondition","das_call_member< ELifetimeCondition (FProperty::*)() const , &FProperty::GetBlueprintReplicationCondition >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_771 = das::das_call_member< void (FProperty::*)(ELifetimeCondition),&FProperty::SetBlueprintReplicationCondition >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1135:7
	makeExtern<DAS_CALL_METHOD(_method_771), SimNode_ExtFuncCall >(lib,"SetBlueprintReplicationCondition","das_call_member< void (FProperty::*)(ELifetimeCondition) , &FProperty::SetBlueprintReplicationCondition >::invoke")
		->args({"self","InBlueprintReplicationCondition"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FNumericProperty,EInternal,FFieldClass *>(*this,lib,"FNumericProperty","FNumericProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1488:2
	makeExtern< FFieldClass * (*)(const FNumericProperty *) , _dasUnreal_static_257_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_257_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FNumericProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1488:2
	makeExtern< FField * (*)(const FNumericProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_258_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_258_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FNumericProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1488:2
	makeExtern< uint64 (*)(const FNumericProperty *) , _dasUnreal_static_259_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_259_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FNumericProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1488:2
	makeExtern< uint64 (*)(const FNumericProperty *) , _dasUnreal_static_260_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_260_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FNumericProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FNumericProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FNumericProperty","FNumericProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FNumericProperty,UField *>(*this,lib,"FNumericProperty","FNumericProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1519:15
	makeExtern< void (*)(const FNumericProperty &,FString &,const void *,EPropertyPointerType,const void *,UObject *,int,UObject *) , _dasUnreal_virtual_261_ExportText_Internal , SimNode_ExtFuncCall >(lib,"ExportText_Internal","_dasUnreal_virtual_261_ExportText_Internal")
		->args({"self","ValueStr","PropertyValueOrContainer","PropertyPointerType","DefaultValue","Parent","PortFlags","ExportRootScope"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1525:15
	makeExtern< bool (*)(const FNumericProperty &) , _dasUnreal_virtual_262_IsFloatingPoint , SimNode_ExtFuncCall >(lib,"IsFloatingPoint","_dasUnreal_virtual_262_IsFloatingPoint")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1528:15
	makeExtern< bool (*)(const FNumericProperty &) , _dasUnreal_virtual_263_IsInteger , SimNode_ExtFuncCall >(lib,"IsInteger","_dasUnreal_virtual_263_IsInteger")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_779 = das::das_call_member< bool (FNumericProperty::*)() const,&FNumericProperty::IsEnum >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1549:19
	makeExtern<DAS_CALL_METHOD(_method_779), SimNode_ExtFuncCall >(lib,"IsEnum","das_call_member< bool (FNumericProperty::*)() const , &FNumericProperty::IsEnum >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1555:17
	makeExtern< UEnum * (*)(const FNumericProperty &) , _dasUnreal_virtual_264_GetIntPropertyEnum , SimNode_ExtFuncCall >(lib,"GetIntPropertyEnum","_dasUnreal_virtual_264_GetIntPropertyEnum")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1562:15
	makeExtern< void (*)(const FNumericProperty &,void *,uint64) , _dasUnreal_virtual_265_SetIntPropertyValue , SimNode_ExtFuncCall >(lib,"SetIntPropertyValue","_dasUnreal_virtual_265_SetIntPropertyValue")
		->args({"self","Data","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1569:15
	makeExtern< void (*)(const FNumericProperty &,void *,int64) , _dasUnreal_virtual_266_SetIntPropertyValue , SimNode_ExtFuncCall >(lib,"SetIntPropertyValue","_dasUnreal_virtual_266_SetIntPropertyValue")
		->args({"self","Data","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1576:15
	makeExtern< void (*)(const FNumericProperty &,void *,double) , _dasUnreal_virtual_267_SetFloatingPointPropertyValue , SimNode_ExtFuncCall >(lib,"SetFloatingPointPropertyValue","_dasUnreal_virtual_267_SetFloatingPointPropertyValue")
		->args({"self","Data","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1584:15
	makeExtern< void (*)(const FNumericProperty &,void *,const wchar_t *) , _dasUnreal_virtual_268_SetNumericPropertyValueFromString , SimNode_ExtFuncCall >(lib,"SetNumericPropertyValueFromString","_dasUnreal_virtual_268_SetNumericPropertyValueFromString")
		->args({"self","Data","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1585:15
	makeExtern< void (*)(const FNumericProperty &,void *,const wchar_t *) , _dasUnreal_virtual_269_SetNumericPropertyValueFromString_InContainer , SimNode_ExtFuncCall >(lib,"SetNumericPropertyValueFromString_InContainer","_dasUnreal_virtual_269_SetNumericPropertyValueFromString_InContainer")
		->args({"self","Container","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1592:16
	makeExtern< int64 (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_270_GetSignedIntPropertyValue , SimNode_ExtFuncCall >(lib,"GetSignedIntPropertyValue","_dasUnreal_virtual_270_GetSignedIntPropertyValue")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1593:16
	makeExtern< int64 (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_271_GetSignedIntPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"GetSignedIntPropertyValue_InContainer","_dasUnreal_virtual_271_GetSignedIntPropertyValue_InContainer")
		->args({"self","Container"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1600:17
	makeExtern< uint64 (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_272_GetUnsignedIntPropertyValue , SimNode_ExtFuncCall >(lib,"GetUnsignedIntPropertyValue","_dasUnreal_virtual_272_GetUnsignedIntPropertyValue")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1601:17
	makeExtern< uint64 (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_273_GetUnsignedIntPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"GetUnsignedIntPropertyValue_InContainer","_dasUnreal_virtual_273_GetUnsignedIntPropertyValue_InContainer")
		->args({"self","Container"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1608:17
	makeExtern< double (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_274_GetFloatingPointPropertyValue , SimNode_ExtFuncCall >(lib,"GetFloatingPointPropertyValue","_dasUnreal_virtual_274_GetFloatingPointPropertyValue")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1616:18
	makeExtern< FString (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_275_GetNumericPropertyValueToString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNumericPropertyValueToString","_dasUnreal_virtual_275_GetNumericPropertyValueToString")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1617:18
	makeExtern< FString (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_276_GetNumericPropertyValueToString_InContainer , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNumericPropertyValueToString_InContainer","_dasUnreal_virtual_276_GetNumericPropertyValueToString_InContainer")
		->args({"self","Container"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FBoolProperty,EInternal,FFieldClass *>(*this,lib,"FBoolProperty","FBoolProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2340:2
	makeExtern< FFieldClass * (*)(const FBoolProperty *) , _dasUnreal_static_277_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_277_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2340:2
	makeExtern< FField * (*)(const FBoolProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_278_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_278_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2340:2
	makeExtern< uint64 (*)(const FBoolProperty *) , _dasUnreal_static_279_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_279_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2340:2
	makeExtern< uint64 (*)(const FBoolProperty *) , _dasUnreal_static_280_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_280_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FBoolProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FBoolProperty","FBoolProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FBoolProperty,FFieldVariant,const UECodeGen_Private::FBoolPropertyParams &>(*this,lib,"FBoolProperty","FBoolProperty")
		->args({"InOwner","Prop"});
	addCtorAndUsing<FBoolProperty,UField *>(*this,lib,"FBoolProperty","FBoolProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2388:15
	makeExtern< void (*)(FBoolProperty &,const FField &) , _dasUnreal_virtual_281_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_281_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2391:18
	makeExtern< FString (*)(const FBoolProperty &,FString *,unsigned int) , _dasUnreal_virtual_282_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_282_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2392:18
	makeExtern< FString (*)(const FBoolProperty &,FString &) , _dasUnreal_virtual_283_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_283_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2393:18
	makeExtern< FString (*)(const FBoolProperty &) , _dasUnreal_virtual_284_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_284_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2398:15
	makeExtern< bool (*)(const FBoolProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_285_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_285_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2405:15
	makeExtern< void (*)(const FBoolProperty &,void *,const void *,int) , _dasUnreal_virtual_286_CopyValuesInternal , SimNode_ExtFuncCall >(lib,"CopyValuesInternal","_dasUnreal_virtual_286_CopyValuesInternal")
		->args({"self","Dest","Src","Count"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2406:15
	makeExtern< void (*)(const FBoolProperty &,void *) , _dasUnreal_virtual_287_ClearValueInternal , SimNode_ExtFuncCall >(lib,"ClearValueInternal","_dasUnreal_virtual_287_ClearValueInternal")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2407:15
	makeExtern< void (*)(const FBoolProperty &,void *) , _dasUnreal_virtual_288_InitializeValueInternal , SimNode_ExtFuncCall >(lib,"InitializeValueInternal","_dasUnreal_virtual_288_InitializeValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2408:16
	makeExtern< int (*)(const FBoolProperty &) , _dasUnreal_virtual_289_GetMinAlignment , SimNode_ExtFuncCall >(lib,"GetMinAlignment","_dasUnreal_virtual_289_GetMinAlignment")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2411:15
	makeExtern< void (*)(const FBoolProperty &,FBlake3 &,bool) , _dasUnreal_virtual_290_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_290_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_807 = das::das_call_member< bool (FBoolProperty::*)(const void *) const,&FBoolProperty::GetPropertyValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2418:19
	makeExtern<DAS_CALL_METHOD(_method_807), SimNode_ExtFuncCall >(lib,"GetPropertyValue","das_call_member< bool (FBoolProperty::*)(const void *) const , &FBoolProperty::GetPropertyValue >::invoke")
		->args({"self","A"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_808 = das::das_call_member< bool (FBoolProperty::*)(const void *,int) const,&FBoolProperty::GetPropertyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2424:19
	makeExtern<DAS_CALL_METHOD(_method_808), SimNode_ExtFuncCall >(lib,"GetPropertyValue_InContainer","das_call_member< bool (FBoolProperty::*)(const void *,int) const , &FBoolProperty::GetPropertyValue_InContainer >::invoke")
		->args({"self","A","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2428:26
	makeExtern< bool (*)(const FBoolProperty *) , _dasUnreal_static_291_GetDefaultPropertyValue , SimNode_ExtFuncCall >(lib,"GetDefaultPropertyValue","_dasUnreal_static_291_GetDefaultPropertyValue")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_810 = das::das_call_member< bool (FBoolProperty::*)(const void *) const,&FBoolProperty::GetOptionalPropertyValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2432:19
	makeExtern<DAS_CALL_METHOD(_method_810), SimNode_ExtFuncCall >(lib,"GetOptionalPropertyValue","das_call_member< bool (FBoolProperty::*)(const void *) const , &FBoolProperty::GetOptionalPropertyValue >::invoke")
		->args({"self","B"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_811 = das::das_call_member< bool (FBoolProperty::*)(const void *,int) const,&FBoolProperty::GetOptionalPropertyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2436:19
	makeExtern<DAS_CALL_METHOD(_method_811), SimNode_ExtFuncCall >(lib,"GetOptionalPropertyValue_InContainer","das_call_member< bool (FBoolProperty::*)(const void *,int) const , &FBoolProperty::GetOptionalPropertyValue_InContainer >::invoke")
		->args({"self","B","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_812 = das::das_call_member< void (FBoolProperty::*)(void *,bool) const,&FBoolProperty::SetPropertyValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2440:19
	makeExtern<DAS_CALL_METHOD(_method_812), SimNode_ExtFuncCall >(lib,"SetPropertyValue","das_call_member< void (FBoolProperty::*)(void *,bool) const , &FBoolProperty::SetPropertyValue >::invoke")
		->args({"self","A","Value"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_813 = das::das_call_member< void (FBoolProperty::*)(void *,bool,int) const,&FBoolProperty::SetPropertyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2446:19
	makeExtern<DAS_CALL_METHOD(_method_813), SimNode_ExtFuncCall >(lib,"SetPropertyValue_InContainer","das_call_member< void (FBoolProperty::*)(void *,bool,int) const , &FBoolProperty::SetPropertyValue_InContainer >::invoke")
		->args({"self","A","Value","ArrayIndex"})
		->arg_init(3,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_814 = das::das_call_member< void (FBoolProperty::*)(const unsigned int,const bool,const unsigned int),&FBoolProperty::SetBoolSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2459:7
	makeExtern<DAS_CALL_METHOD(_method_814), SimNode_ExtFuncCall >(lib,"SetBoolSize","das_call_member< void (FBoolProperty::*)(const unsigned int,const bool,const unsigned int) , &FBoolProperty::SetBoolSize >::invoke")
		->args({"self","InSize","bIsNativeBool","InBitMask"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->arg_init(3,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_815 = das::das_call_member< bool (FBoolProperty::*)() const,&FBoolProperty::IsNativeBool >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2464:19
	makeExtern<DAS_CALL_METHOD(_method_815), SimNode_ExtFuncCall >(lib,"IsNativeBool","das_call_member< bool (FBoolProperty::*)() const , &FBoolProperty::IsNativeBool >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_816 = das::das_call_member< unsigned char (FBoolProperty::*)() const,&FBoolProperty::GetFieldMask >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2470:20
	makeExtern<DAS_CALL_METHOD(_method_816), SimNode_ExtFuncCall >(lib,"GetFieldMask","das_call_member< unsigned char (FBoolProperty::*)() const , &FBoolProperty::GetFieldMask >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_817 = das::das_call_member< unsigned char (FBoolProperty::*)() const,&FBoolProperty::GetByteOffset >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2476:20
	makeExtern<DAS_CALL_METHOD(_method_817), SimNode_ExtFuncCall >(lib,"GetByteOffset","das_call_member< unsigned char (FBoolProperty::*)() const , &FBoolProperty::GetByteOffset >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2481:9
	makeExtern< unsigned int (*)(const FBoolProperty &,const void *) , _dasUnreal_virtual_292_GetValueTypeHashInternal , SimNode_ExtFuncCall >(lib,"GetValueTypeHashInternal","_dasUnreal_virtual_292_GetValueTypeHashInternal")
		->args({"self","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FObjectPropertyBase,EInternal,FFieldClass *>(*this,lib,"FObjectPropertyBase","FObjectPropertyBase")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2493:2
	makeExtern< FFieldClass * (*)(const FObjectPropertyBase *) , _dasUnreal_static_293_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_293_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2493:2
	makeExtern< FField * (*)(const FObjectPropertyBase *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_294_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_294_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2493:2
	makeExtern< uint64 (*)(const FObjectPropertyBase *) , _dasUnreal_static_295_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_295_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2493:2
	makeExtern< uint64 (*)(const FObjectPropertyBase *) , _dasUnreal_static_296_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_296_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FObjectPropertyBase,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FObjectPropertyBase","FObjectPropertyBase")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FObjectPropertyBase,UField *>(*this,lib,"FObjectPropertyBase","FObjectPropertyBase")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2527:15
	makeExtern< void (*)(FObjectPropertyBase &,FReferenceCollector &) , _dasUnreal_virtual_297_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_297_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2528:15
	makeExtern< void (*)(FObjectPropertyBase &) , _dasUnreal_virtual_298_BeginDestroy , SimNode_ExtFuncCall >(lib,"BeginDestroy","_dasUnreal_virtual_298_BeginDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2532:15
	makeExtern< void (*)(FObjectPropertyBase &,const FField &) , _dasUnreal_virtual_299_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_299_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2535:15
	makeExtern< bool (*)(const FObjectPropertyBase &,const void *,const void *,unsigned int) , _dasUnreal_virtual_300_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_300_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2537:15
	makeExtern< bool (*)(const FObjectPropertyBase &) , _dasUnreal_virtual_301_SupportsNetSharedSerialization , SimNode_ExtFuncCall >(lib,"SupportsNetSharedSerialization","_dasUnreal_virtual_301_SupportsNetSharedSerialization")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2542:16
	makeExtern< FName (*)(const FObjectPropertyBase &) , _dasUnreal_virtual_302_GetID , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetID","_dasUnreal_virtual_302_GetID")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2544:15
	makeExtern< bool (*)(const FObjectPropertyBase &,const FProperty *) , _dasUnreal_virtual_303_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_303_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2550:18
	makeExtern< FString (*)(const FObjectPropertyBase &,FString *,unsigned int,const FString &) , _dasUnreal_virtual_304_GetCPPTypeCustom , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeCustom","_dasUnreal_virtual_304_GetCPPTypeCustom")
		->args({"self","ExtendedTypeText","CPPExportFlags","InnerNativeTypeName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2589:14
	makeExtern< bool (*)(const FObjectPropertyBase *,UObject *,UObject *,unsigned int) , _dasUnreal_static_305_StaticIdentical , SimNode_ExtFuncCall >(lib,"StaticIdentical","_dasUnreal_static_305_StaticIdentical")
		->args({"self","A","B","PortFlags"})
		->arg_type(0,makeType<TExplicit<const FObjectPropertyBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2591:19
	makeExtern< UObject * (*)(const FObjectPropertyBase &,const void *) , _dasUnreal_virtual_306_LoadObjectPropertyValue , SimNode_ExtFuncCall >(lib,"LoadObjectPropertyValue","_dasUnreal_virtual_306_LoadObjectPropertyValue")
		->args({"self","PropertyValueAddress"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_833 = das::das_call_member< UObject * (FObjectPropertyBase::*)(const void *,int) const,&FObjectPropertyBase::LoadObjectPropertyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2595:23
	makeExtern<DAS_CALL_METHOD(_method_833), SimNode_ExtFuncCall >(lib,"LoadObjectPropertyValue_InContainer","das_call_member< UObject * (FObjectPropertyBase::*)(const void *,int) const , &FObjectPropertyBase::LoadObjectPropertyValue_InContainer >::invoke")
		->args({"self","PropertyValueAddress","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
}
}

