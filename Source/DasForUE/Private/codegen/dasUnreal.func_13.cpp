// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_13() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1593:16
	makeExtern< int64 (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_378_GetSignedIntPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"GetSignedIntPropertyValue_InContainer","_dasUnreal_virtual_378_GetSignedIntPropertyValue_InContainer")
		->args({"self","Container"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1600:17
	makeExtern< uint64 (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_379_GetUnsignedIntPropertyValue , SimNode_ExtFuncCall >(lib,"GetUnsignedIntPropertyValue","_dasUnreal_virtual_379_GetUnsignedIntPropertyValue")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1601:17
	makeExtern< uint64 (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_380_GetUnsignedIntPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"GetUnsignedIntPropertyValue_InContainer","_dasUnreal_virtual_380_GetUnsignedIntPropertyValue_InContainer")
		->args({"self","Container"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1608:17
	makeExtern< double (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_381_GetFloatingPointPropertyValue , SimNode_ExtFuncCall >(lib,"GetFloatingPointPropertyValue","_dasUnreal_virtual_381_GetFloatingPointPropertyValue")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1616:18
	makeExtern< FString (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_382_GetNumericPropertyValueToString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNumericPropertyValueToString","_dasUnreal_virtual_382_GetNumericPropertyValueToString")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1617:18
	makeExtern< FString (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_383_GetNumericPropertyValueToString_InContainer , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNumericPropertyValueToString_InContainer","_dasUnreal_virtual_383_GetNumericPropertyValueToString_InContainer")
		->args({"self","Container"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FByteProperty,EInternal,FFieldClass *>(*this,lib,"FByteProperty","FByteProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1902:2
	makeExtern< FFieldClass * (*)(const FByteProperty *) , _dasUnreal_static_384_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_384_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FByteProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1902:2
	makeExtern< FField * (*)(const FByteProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_385_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_385_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FByteProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1902:2
	makeExtern< uint64 (*)(const FByteProperty *) , _dasUnreal_static_386_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_386_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FByteProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1902:2
	makeExtern< uint64 (*)(const FByteProperty *) , _dasUnreal_static_387_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_387_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FByteProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FByteProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FByteProperty","FByteProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FByteProperty,UField *>(*this,lib,"FByteProperty","FByteProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1934:15
	makeExtern< void (*)(FByteProperty &,FArchive &) , _dasUnreal_virtual_388_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_388_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1935:15
	makeExtern< void (*)(FByteProperty &,FReferenceCollector &) , _dasUnreal_virtual_389_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_389_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1940:15
	makeExtern< void (*)(FByteProperty &,const FField &) , _dasUnreal_virtual_390_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_390_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1943:18
	makeExtern< FString (*)(const FByteProperty &,FString *,unsigned int) , _dasUnreal_virtual_391_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_391_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->arg_init(2,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1955:15
	makeExtern< void (*)(const FByteProperty &,FBlake3 &,bool) , _dasUnreal_virtual_392_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_392_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1960:17
	makeExtern< UEnum * (*)(const FByteProperty &) , _dasUnreal_virtual_393_GetIntPropertyEnum , SimNode_ExtFuncCall >(lib,"GetIntPropertyEnum","_dasUnreal_virtual_393_GetIntPropertyEnum")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1036 = das::das_call_member< uint64 (FByteProperty::*)() const,&FByteProperty::GetMaxNetSerializeBits >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1964:9
	makeExtern<DAS_CALL_METHOD(_method_1036), SimNode_ExtFuncCall >(lib,"GetMaxNetSerializeBits","das_call_member< uint64 (FByteProperty::*)() const , &FByteProperty::GetMaxNetSerializeBits >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FInt8Property,EInternal,FFieldClass *>(*this,lib,"FInt8Property","FInt8Property")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1976:2
	makeExtern< FFieldClass * (*)(const FInt8Property *) , _dasUnreal_static_394_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_394_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt8Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1976:2
	makeExtern< FField * (*)(const FInt8Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_395_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_395_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FInt8Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1976:2
	makeExtern< uint64 (*)(const FInt8Property *) , _dasUnreal_static_396_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_396_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt8Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1976:2
	makeExtern< uint64 (*)(const FInt8Property *) , _dasUnreal_static_397_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_397_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt8Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FInt8Property,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FInt8Property","FInt8Property")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FInt8Property,UField *>(*this,lib,"FInt8Property","FInt8Property")
		->args({"InField"});
	addCtorAndUsing<FInt16Property,EInternal,FFieldClass *>(*this,lib,"FInt16Property","FInt16Property")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2015:2
	makeExtern< FFieldClass * (*)(const FInt16Property *) , _dasUnreal_static_398_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_398_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2015:2
	makeExtern< FField * (*)(const FInt16Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_399_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_399_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2015:2
	makeExtern< uint64 (*)(const FInt16Property *) , _dasUnreal_static_400_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_400_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2015:2
	makeExtern< uint64 (*)(const FInt16Property *) , _dasUnreal_static_401_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_401_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FInt16Property,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FInt16Property","FInt16Property")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FInt16Property,UField *>(*this,lib,"FInt16Property","FInt16Property")
		->args({"InField"});
	addCtorAndUsing<FIntProperty,EInternal,FFieldClass *>(*this,lib,"FIntProperty","FIntProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2055:2
	makeExtern< FFieldClass * (*)(const FIntProperty *) , _dasUnreal_static_402_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_402_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FIntProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2055:2
	makeExtern< FField * (*)(const FIntProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_403_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_403_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FIntProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2055:2
	makeExtern< uint64 (*)(const FIntProperty *) , _dasUnreal_static_404_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_404_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FIntProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2055:2
	makeExtern< uint64 (*)(const FIntProperty *) , _dasUnreal_static_405_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_405_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FIntProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FIntProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FIntProperty","FIntProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FIntProperty,UField *>(*this,lib,"FIntProperty","FIntProperty")
		->args({"InField"});
	addCtorAndUsing<FInt64Property,EInternal,FFieldClass *>(*this,lib,"FInt64Property","FInt64Property")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2094:2
	makeExtern< FFieldClass * (*)(const FInt64Property *) , _dasUnreal_static_406_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_406_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2094:2
	makeExtern< FField * (*)(const FInt64Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_407_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_407_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2094:2
	makeExtern< uint64 (*)(const FInt64Property *) , _dasUnreal_static_408_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_408_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2094:2
	makeExtern< uint64 (*)(const FInt64Property *) , _dasUnreal_static_409_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_409_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FInt64Property,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FInt64Property","FInt64Property")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FInt64Property,UField *>(*this,lib,"FInt64Property","FInt64Property")
		->args({"InField"});
	addCtorAndUsing<FUInt16Property,EInternal,FFieldClass *>(*this,lib,"FUInt16Property","FUInt16Property")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2133:2
	makeExtern< FFieldClass * (*)(const FUInt16Property *) , _dasUnreal_static_410_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_410_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2133:2
	makeExtern< FField * (*)(const FUInt16Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_411_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_411_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FUInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2133:2
	makeExtern< uint64 (*)(const FUInt16Property *) , _dasUnreal_static_412_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_412_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2133:2
	makeExtern< uint64 (*)(const FUInt16Property *) , _dasUnreal_static_413_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_413_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FUInt16Property,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FUInt16Property","FUInt16Property")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FUInt16Property,UField *>(*this,lib,"FUInt16Property","FUInt16Property")
		->args({"InField"});
	addCtorAndUsing<FUInt32Property,EInternal,FFieldClass *>(*this,lib,"FUInt32Property","FUInt32Property")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2172:2
	makeExtern< FFieldClass * (*)(const FUInt32Property *) , _dasUnreal_static_414_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_414_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt32Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2172:2
	makeExtern< FField * (*)(const FUInt32Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_415_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_415_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FUInt32Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2172:2
	makeExtern< uint64 (*)(const FUInt32Property *) , _dasUnreal_static_416_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_416_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt32Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2172:2
	makeExtern< uint64 (*)(const FUInt32Property *) , _dasUnreal_static_417_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_417_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt32Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FUInt32Property,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FUInt32Property","FUInt32Property")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FUInt32Property,UField *>(*this,lib,"FUInt32Property","FUInt32Property")
		->args({"InField"});
	addCtorAndUsing<FUInt64Property,EInternal,FFieldClass *>(*this,lib,"FUInt64Property","FUInt64Property")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2211:2
	makeExtern< FFieldClass * (*)(const FUInt64Property *) , _dasUnreal_static_418_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_418_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2211:2
	makeExtern< FField * (*)(const FUInt64Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_419_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_419_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FUInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2211:2
	makeExtern< uint64 (*)(const FUInt64Property *) , _dasUnreal_static_420_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_420_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2211:2
	makeExtern< uint64 (*)(const FUInt64Property *) , _dasUnreal_static_421_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_421_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FUInt64Property,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FUInt64Property","FUInt64Property")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FUInt64Property,UField *>(*this,lib,"FUInt64Property","FUInt64Property")
		->args({"InField"});
	addCtorAndUsing<FFloatProperty,EInternal,FFieldClass *>(*this,lib,"FFloatProperty","FFloatProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2251:2
	makeExtern< FFieldClass * (*)(const FFloatProperty *) , _dasUnreal_static_422_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_422_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FFloatProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2251:2
	makeExtern< FField * (*)(const FFloatProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_423_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_423_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FFloatProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2251:2
	makeExtern< uint64 (*)(const FFloatProperty *) , _dasUnreal_static_424_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_424_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FFloatProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2251:2
	makeExtern< uint64 (*)(const FFloatProperty *) , _dasUnreal_static_425_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_425_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FFloatProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FFloatProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FFloatProperty","FFloatProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FFloatProperty,UField *>(*this,lib,"FFloatProperty","FFloatProperty")
		->args({"InField"});
	addCtorAndUsing<FDoubleProperty,EInternal,FFieldClass *>(*this,lib,"FDoubleProperty","FDoubleProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2290:2
	makeExtern< FFieldClass * (*)(const FDoubleProperty *) , _dasUnreal_static_426_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_426_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FDoubleProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2290:2
	makeExtern< FField * (*)(const FDoubleProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_427_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_427_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FDoubleProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2290:2
	makeExtern< uint64 (*)(const FDoubleProperty *) , _dasUnreal_static_428_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_428_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FDoubleProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2290:2
	makeExtern< uint64 (*)(const FDoubleProperty *) , _dasUnreal_static_429_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_429_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FDoubleProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FDoubleProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FDoubleProperty","FDoubleProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FDoubleProperty,UField *>(*this,lib,"FDoubleProperty","FDoubleProperty")
		->args({"InField"});
	addCtorAndUsing<FBoolProperty,EInternal,FFieldClass *>(*this,lib,"FBoolProperty","FBoolProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2340:2
	makeExtern< FFieldClass * (*)(const FBoolProperty *) , _dasUnreal_static_430_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_430_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2340:2
	makeExtern< FField * (*)(const FBoolProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_431_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_431_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2340:2
	makeExtern< uint64 (*)(const FBoolProperty *) , _dasUnreal_static_432_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_432_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2340:2
	makeExtern< uint64 (*)(const FBoolProperty *) , _dasUnreal_static_433_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_433_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FBoolProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FBoolProperty","FBoolProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FBoolProperty,UField *>(*this,lib,"FBoolProperty","FBoolProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2384:15
	makeExtern< void (*)(FBoolProperty &,FArchive &) , _dasUnreal_virtual_434_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_434_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2388:15
	makeExtern< void (*)(FBoolProperty &,const FField &) , _dasUnreal_virtual_435_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_435_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2391:18
	makeExtern< FString (*)(const FBoolProperty &,FString *,unsigned int) , _dasUnreal_virtual_436_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_436_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2392:18
	makeExtern< FString (*)(const FBoolProperty &,FString &) , _dasUnreal_virtual_437_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_437_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2393:18
	makeExtern< FString (*)(const FBoolProperty &) , _dasUnreal_virtual_438_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_438_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2397:15
	makeExtern< void (*)(FBoolProperty &,FArchive &) , _dasUnreal_virtual_439_LinkInternal , SimNode_ExtFuncCall >(lib,"LinkInternal","_dasUnreal_virtual_439_LinkInternal")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2398:15
	makeExtern< bool (*)(const FBoolProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_440_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_440_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2405:15
	makeExtern< void (*)(const FBoolProperty &,void *,const void *,int) , _dasUnreal_virtual_441_CopyValuesInternal , SimNode_ExtFuncCall >(lib,"CopyValuesInternal","_dasUnreal_virtual_441_CopyValuesInternal")
		->args({"self","Dest","Src","Count"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2406:15
	makeExtern< void (*)(const FBoolProperty &,void *) , _dasUnreal_virtual_442_ClearValueInternal , SimNode_ExtFuncCall >(lib,"ClearValueInternal","_dasUnreal_virtual_442_ClearValueInternal")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2407:15
	makeExtern< void (*)(const FBoolProperty &,void *) , _dasUnreal_virtual_443_InitializeValueInternal , SimNode_ExtFuncCall >(lib,"InitializeValueInternal","_dasUnreal_virtual_443_InitializeValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

