// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_13() {
	using _method_1016 = das::das_call_member< FProperty * (FProperty::*)(),&FProperty::GetRepOwner >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1114:25
	makeExtern<DAS_CALL_METHOD(_method_1016), SimNode_ExtFuncCall >(lib,"GetRepOwner","das_call_member< FProperty * (FProperty::*)() , &FProperty::GetRepOwner >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1017 = das::das_call_member< bool (FProperty::*)() const,&FProperty::IsEditorOnlyProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1126:19
	makeExtern<DAS_CALL_METHOD(_method_1017), SimNode_ExtFuncCall >(lib,"IsEditorOnlyProperty","das_call_member< bool (FProperty::*)() const , &FProperty::IsEditorOnlyProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1132:31
	makeExtern< bool (*)(const FProperty &,const FProperty *) , _dasUnreal_virtual_364_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_364_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1019 = das::das_call_member< ELifetimeCondition (FProperty::*)() const,&FProperty::GetBlueprintReplicationCondition >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1134:21
	makeExtern<DAS_CALL_METHOD(_method_1019), SimNode_ExtFuncCall >(lib,"GetBlueprintReplicationCondition","das_call_member< ELifetimeCondition (FProperty::*)() const , &FProperty::GetBlueprintReplicationCondition >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1020 = das::das_call_member< void (FProperty::*)(ELifetimeCondition),&FProperty::SetBlueprintReplicationCondition >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1135:7
	makeExtern<DAS_CALL_METHOD(_method_1020), SimNode_ExtFuncCall >(lib,"SetBlueprintReplicationCondition","das_call_member< void (FProperty::*)(ELifetimeCondition) , &FProperty::SetBlueprintReplicationCondition >::invoke")
		->args({"self","InBlueprintReplicationCondition"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FNumericProperty,EInternal,FFieldClass *>(*this,lib,"FNumericProperty","FNumericProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1488:2
	makeExtern< FFieldClass * (*)(const FNumericProperty *) , _dasUnreal_static_365_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_365_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FNumericProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1488:2
	makeExtern< FField * (*)(const FNumericProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_366_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_366_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FNumericProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1488:2
	makeExtern< uint64 (*)(const FNumericProperty *) , _dasUnreal_static_367_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_367_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FNumericProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1488:2
	makeExtern< uint64 (*)(const FNumericProperty *) , _dasUnreal_static_368_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_368_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FNumericProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FNumericProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FNumericProperty","FNumericProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FNumericProperty,FFieldVariant,const FName &,EObjectFlags,int,EPropertyFlags>(*this,lib,"FNumericProperty","FNumericProperty")
		->args({"InOwner","InName","InObjectFlags","InOffset","InFlags"});
	addCtorAndUsing<FNumericProperty,UField *>(*this,lib,"FNumericProperty","FNumericProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1518:23
	makeExtern< const wchar_t * (*)(const FNumericProperty &,const wchar_t *,void *,EPropertyPointerType,UObject *,int,FOutputDevice *) , _dasUnreal_virtual_369_ImportText_Internal , SimNode_ExtFuncCall >(lib,"ImportText_Internal","_dasUnreal_virtual_369_ImportText_Internal")
		->args({"self","Buffer","ContainerOrPropertyPtr","PropertyPointerType","Parent","PortFlags","ErrorText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1519:15
	makeExtern< void (*)(const FNumericProperty &,FString &,const void *,EPropertyPointerType,const void *,UObject *,int,UObject *) , _dasUnreal_virtual_370_ExportText_Internal , SimNode_ExtFuncCall >(lib,"ExportText_Internal","_dasUnreal_virtual_370_ExportText_Internal")
		->args({"self","ValueStr","PropertyValueOrContainer","PropertyPointerType","DefaultValue","Parent","PortFlags","ExportRootScope"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1525:15
	makeExtern< bool (*)(const FNumericProperty &) , _dasUnreal_virtual_371_IsFloatingPoint , SimNode_ExtFuncCall >(lib,"IsFloatingPoint","_dasUnreal_virtual_371_IsFloatingPoint")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1528:15
	makeExtern< bool (*)(const FNumericProperty &) , _dasUnreal_virtual_372_IsInteger , SimNode_ExtFuncCall >(lib,"IsInteger","_dasUnreal_virtual_372_IsInteger")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1029 = das::das_call_member< bool (FNumericProperty::*)() const,&FNumericProperty::IsEnum >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1549:19
	makeExtern<DAS_CALL_METHOD(_method_1029), SimNode_ExtFuncCall >(lib,"IsEnum","das_call_member< bool (FNumericProperty::*)() const , &FNumericProperty::IsEnum >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1555:17
	makeExtern< UEnum * (*)(const FNumericProperty &) , _dasUnreal_virtual_373_GetIntPropertyEnum , SimNode_ExtFuncCall >(lib,"GetIntPropertyEnum","_dasUnreal_virtual_373_GetIntPropertyEnum")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1562:15
	makeExtern< void (*)(const FNumericProperty &,void *,uint64) , _dasUnreal_virtual_374_SetIntPropertyValue , SimNode_ExtFuncCall >(lib,"SetIntPropertyValue","_dasUnreal_virtual_374_SetIntPropertyValue")
		->args({"self","Data","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1569:15
	makeExtern< void (*)(const FNumericProperty &,void *,int64) , _dasUnreal_virtual_375_SetIntPropertyValue , SimNode_ExtFuncCall >(lib,"SetIntPropertyValue","_dasUnreal_virtual_375_SetIntPropertyValue")
		->args({"self","Data","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1576:15
	makeExtern< void (*)(const FNumericProperty &,void *,double) , _dasUnreal_virtual_376_SetFloatingPointPropertyValue , SimNode_ExtFuncCall >(lib,"SetFloatingPointPropertyValue","_dasUnreal_virtual_376_SetFloatingPointPropertyValue")
		->args({"self","Data","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1584:15
	makeExtern< void (*)(const FNumericProperty &,void *,const wchar_t *) , _dasUnreal_virtual_377_SetNumericPropertyValueFromString , SimNode_ExtFuncCall >(lib,"SetNumericPropertyValueFromString","_dasUnreal_virtual_377_SetNumericPropertyValueFromString")
		->args({"self","Data","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1585:15
	makeExtern< void (*)(const FNumericProperty &,void *,const wchar_t *) , _dasUnreal_virtual_378_SetNumericPropertyValueFromString_InContainer , SimNode_ExtFuncCall >(lib,"SetNumericPropertyValueFromString_InContainer","_dasUnreal_virtual_378_SetNumericPropertyValueFromString_InContainer")
		->args({"self","Container","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1592:16
	makeExtern< int64 (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_379_GetSignedIntPropertyValue , SimNode_ExtFuncCall >(lib,"GetSignedIntPropertyValue","_dasUnreal_virtual_379_GetSignedIntPropertyValue")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1593:16
	makeExtern< int64 (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_380_GetSignedIntPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"GetSignedIntPropertyValue_InContainer","_dasUnreal_virtual_380_GetSignedIntPropertyValue_InContainer")
		->args({"self","Container"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1600:17
	makeExtern< uint64 (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_381_GetUnsignedIntPropertyValue , SimNode_ExtFuncCall >(lib,"GetUnsignedIntPropertyValue","_dasUnreal_virtual_381_GetUnsignedIntPropertyValue")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1601:17
	makeExtern< uint64 (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_382_GetUnsignedIntPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"GetUnsignedIntPropertyValue_InContainer","_dasUnreal_virtual_382_GetUnsignedIntPropertyValue_InContainer")
		->args({"self","Container"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1608:17
	makeExtern< double (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_383_GetFloatingPointPropertyValue , SimNode_ExtFuncCall >(lib,"GetFloatingPointPropertyValue","_dasUnreal_virtual_383_GetFloatingPointPropertyValue")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1616:18
	makeExtern< FString (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_384_GetNumericPropertyValueToString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNumericPropertyValueToString","_dasUnreal_virtual_384_GetNumericPropertyValueToString")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1617:18
	makeExtern< FString (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_385_GetNumericPropertyValueToString_InContainer , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNumericPropertyValueToString_InContainer","_dasUnreal_virtual_385_GetNumericPropertyValueToString_InContainer")
		->args({"self","Container"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FByteProperty,EInternal,FFieldClass *>(*this,lib,"FByteProperty","FByteProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1902:2
	makeExtern< FFieldClass * (*)(const FByteProperty *) , _dasUnreal_static_386_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_386_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FByteProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1902:2
	makeExtern< FField * (*)(const FByteProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_387_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_387_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FByteProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1902:2
	makeExtern< uint64 (*)(const FByteProperty *) , _dasUnreal_static_388_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_388_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FByteProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1902:2
	makeExtern< uint64 (*)(const FByteProperty *) , _dasUnreal_static_389_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_389_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FByteProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FByteProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FByteProperty","FByteProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FByteProperty,FFieldVariant,const FName &,EObjectFlags,int,EPropertyFlags,UEnum *>(*this,lib,"FByteProperty","FByteProperty")
		->args({"InOwner","InName","InObjectFlags","InOffset","InFlags","InEnum"})
		->arg_init(5,make_smart<ExprConstPtr>());
	addCtorAndUsing<FByteProperty,UField *>(*this,lib,"FByteProperty","FByteProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1934:15
	makeExtern< void (*)(FByteProperty &,FArchive &) , _dasUnreal_virtual_390_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_390_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1935:15
	makeExtern< void (*)(FByteProperty &,FReferenceCollector &) , _dasUnreal_virtual_391_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_391_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1940:15
	makeExtern< void (*)(FByteProperty &,const FField &) , _dasUnreal_virtual_392_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_392_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1943:18
	makeExtern< FString (*)(const FByteProperty &,FString *,unsigned int) , _dasUnreal_virtual_393_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_393_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->arg_init(2,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1955:15
	makeExtern< void (*)(const FByteProperty &,FBlake3 &,bool) , _dasUnreal_virtual_394_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_394_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1960:17
	makeExtern< UEnum * (*)(const FByteProperty &) , _dasUnreal_virtual_395_GetIntPropertyEnum , SimNode_ExtFuncCall >(lib,"GetIntPropertyEnum","_dasUnreal_virtual_395_GetIntPropertyEnum")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1053 = das::das_call_member< uint64 (FByteProperty::*)() const,&FByteProperty::GetMaxNetSerializeBits >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1964:9
	makeExtern<DAS_CALL_METHOD(_method_1053), SimNode_ExtFuncCall >(lib,"GetMaxNetSerializeBits","das_call_member< uint64 (FByteProperty::*)() const , &FByteProperty::GetMaxNetSerializeBits >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FInt8Property,EInternal,FFieldClass *>(*this,lib,"FInt8Property","FInt8Property")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1976:2
	makeExtern< FFieldClass * (*)(const FInt8Property *) , _dasUnreal_static_396_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_396_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt8Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1976:2
	makeExtern< FField * (*)(const FInt8Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_397_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_397_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FInt8Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1976:2
	makeExtern< uint64 (*)(const FInt8Property *) , _dasUnreal_static_398_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_398_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt8Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1976:2
	makeExtern< uint64 (*)(const FInt8Property *) , _dasUnreal_static_399_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_399_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt8Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FInt8Property,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FInt8Property","FInt8Property")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FInt8Property,FFieldVariant,const FName &,EObjectFlags,int,EPropertyFlags>(*this,lib,"FInt8Property","FInt8Property")
		->args({"InOwner","InName","InObjectFlags","InOffset","InFlags"});
	addCtorAndUsing<FInt8Property,UField *>(*this,lib,"FInt8Property","FInt8Property")
		->args({"InField"});
	addCtorAndUsing<FInt16Property,EInternal,FFieldClass *>(*this,lib,"FInt16Property","FInt16Property")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2015:2
	makeExtern< FFieldClass * (*)(const FInt16Property *) , _dasUnreal_static_400_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_400_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2015:2
	makeExtern< FField * (*)(const FInt16Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_401_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_401_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2015:2
	makeExtern< uint64 (*)(const FInt16Property *) , _dasUnreal_static_402_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_402_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2015:2
	makeExtern< uint64 (*)(const FInt16Property *) , _dasUnreal_static_403_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_403_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FInt16Property,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FInt16Property","FInt16Property")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FInt16Property,FFieldVariant,const FName &,EObjectFlags,int,EPropertyFlags>(*this,lib,"FInt16Property","FInt16Property")
		->args({"InOwner","InName","InObjectFlags","InOffset","InFlags"});
	addCtorAndUsing<FInt16Property,UField *>(*this,lib,"FInt16Property","FInt16Property")
		->args({"InField"});
	addCtorAndUsing<FIntProperty,EInternal,FFieldClass *>(*this,lib,"FIntProperty","FIntProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2055:2
	makeExtern< FFieldClass * (*)(const FIntProperty *) , _dasUnreal_static_404_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_404_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FIntProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2055:2
	makeExtern< FField * (*)(const FIntProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_405_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_405_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FIntProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2055:2
	makeExtern< uint64 (*)(const FIntProperty *) , _dasUnreal_static_406_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_406_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FIntProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2055:2
	makeExtern< uint64 (*)(const FIntProperty *) , _dasUnreal_static_407_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_407_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FIntProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FIntProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FIntProperty","FIntProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FIntProperty,FFieldVariant,const FName &,EObjectFlags,int,EPropertyFlags>(*this,lib,"FIntProperty","FIntProperty")
		->args({"InOwner","InName","InObjectFlags","InOffset","InFlags"});
	addCtorAndUsing<FIntProperty,UField *>(*this,lib,"FIntProperty","FIntProperty")
		->args({"InField"});
	addCtorAndUsing<FInt64Property,EInternal,FFieldClass *>(*this,lib,"FInt64Property","FInt64Property")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2094:2
	makeExtern< FFieldClass * (*)(const FInt64Property *) , _dasUnreal_static_408_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_408_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2094:2
	makeExtern< FField * (*)(const FInt64Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_409_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_409_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2094:2
	makeExtern< uint64 (*)(const FInt64Property *) , _dasUnreal_static_410_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_410_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2094:2
	makeExtern< uint64 (*)(const FInt64Property *) , _dasUnreal_static_411_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_411_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FInt64Property,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FInt64Property","FInt64Property")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FInt64Property,FFieldVariant,const FName &,EObjectFlags,int,EPropertyFlags>(*this,lib,"FInt64Property","FInt64Property")
		->args({"InOwner","InName","InObjectFlags","InOffset","InFlags"});
	addCtorAndUsing<FInt64Property,UField *>(*this,lib,"FInt64Property","FInt64Property")
		->args({"InField"});
	addCtorAndUsing<FUInt16Property,EInternal,FFieldClass *>(*this,lib,"FUInt16Property","FUInt16Property")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2133:2
	makeExtern< FFieldClass * (*)(const FUInt16Property *) , _dasUnreal_static_412_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_412_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2133:2
	makeExtern< FField * (*)(const FUInt16Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_413_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_413_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FUInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2133:2
	makeExtern< uint64 (*)(const FUInt16Property *) , _dasUnreal_static_414_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_414_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2133:2
	makeExtern< uint64 (*)(const FUInt16Property *) , _dasUnreal_static_415_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_415_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FUInt16Property,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FUInt16Property","FUInt16Property")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FUInt16Property,FFieldVariant,const FName &,EObjectFlags,int,EPropertyFlags>(*this,lib,"FUInt16Property","FUInt16Property")
		->args({"InOwner","InName","InObjectFlags","InOffset","InFlags"});
	addCtorAndUsing<FUInt16Property,UField *>(*this,lib,"FUInt16Property","FUInt16Property")
		->args({"InField"});
	addCtorAndUsing<FUInt32Property,EInternal,FFieldClass *>(*this,lib,"FUInt32Property","FUInt32Property")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2172:2
	makeExtern< FFieldClass * (*)(const FUInt32Property *) , _dasUnreal_static_416_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_416_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt32Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2172:2
	makeExtern< FField * (*)(const FUInt32Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_417_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_417_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FUInt32Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2172:2
	makeExtern< uint64 (*)(const FUInt32Property *) , _dasUnreal_static_418_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_418_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt32Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2172:2
	makeExtern< uint64 (*)(const FUInt32Property *) , _dasUnreal_static_419_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_419_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt32Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FUInt32Property,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FUInt32Property","FUInt32Property")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FUInt32Property,FFieldVariant,const FName &,EObjectFlags,int,EPropertyFlags>(*this,lib,"FUInt32Property","FUInt32Property")
		->args({"InOwner","InName","InObjectFlags","InOffset","InFlags"});
	addCtorAndUsing<FUInt32Property,UField *>(*this,lib,"FUInt32Property","FUInt32Property")
		->args({"InField"});
	addCtorAndUsing<FUInt64Property,EInternal,FFieldClass *>(*this,lib,"FUInt64Property","FUInt64Property")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2211:2
	makeExtern< FFieldClass * (*)(const FUInt64Property *) , _dasUnreal_static_420_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_420_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2211:2
	makeExtern< FField * (*)(const FUInt64Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_421_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_421_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FUInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2211:2
	makeExtern< uint64 (*)(const FUInt64Property *) , _dasUnreal_static_422_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_422_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2211:2
	makeExtern< uint64 (*)(const FUInt64Property *) , _dasUnreal_static_423_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_423_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FUInt64Property,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FUInt64Property","FUInt64Property")
		->args({"InOwner","InName","InObjectFlags"});
}
}

