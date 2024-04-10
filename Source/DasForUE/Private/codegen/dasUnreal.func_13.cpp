// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_13() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1617:18
	makeExtern< FString (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_382_GetNumericPropertyValueToString_InContainer , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNumericPropertyValueToString_InContainer","_dasUnreal_virtual_382_GetNumericPropertyValueToString_InContainer")
		->args({"self","Container"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FByteProperty,EInternal,FFieldClass *>(*this,lib,"FByteProperty","FByteProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1902:2
	makeExtern< FFieldClass * (*)(const FByteProperty *) , _dasUnreal_static_383_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_383_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FByteProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1902:2
	makeExtern< FField * (*)(const FByteProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_384_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_384_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FByteProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1902:2
	makeExtern< uint64 (*)(const FByteProperty *) , _dasUnreal_static_385_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_385_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FByteProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1902:2
	makeExtern< uint64 (*)(const FByteProperty *) , _dasUnreal_static_386_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_386_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FByteProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FByteProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FByteProperty","FByteProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FByteProperty,UField *>(*this,lib,"FByteProperty","FByteProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1934:15
	makeExtern< void (*)(FByteProperty &,FArchive &) , _dasUnreal_virtual_387_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_387_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1935:15
	makeExtern< void (*)(FByteProperty &,FReferenceCollector &) , _dasUnreal_virtual_388_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_388_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1940:15
	makeExtern< void (*)(FByteProperty &,const FField &) , _dasUnreal_virtual_389_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_389_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1943:18
	makeExtern< FString (*)(const FByteProperty &,FString *,unsigned int) , _dasUnreal_virtual_390_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_390_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->arg_init(2,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1955:15
	makeExtern< void (*)(const FByteProperty &,FBlake3 &,bool) , _dasUnreal_virtual_391_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_391_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1960:17
	makeExtern< UEnum * (*)(const FByteProperty &) , _dasUnreal_virtual_392_GetIntPropertyEnum , SimNode_ExtFuncCall >(lib,"GetIntPropertyEnum","_dasUnreal_virtual_392_GetIntPropertyEnum")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1033 = das::das_call_member< uint64 (FByteProperty::*)() const,&FByteProperty::GetMaxNetSerializeBits >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1964:9
	makeExtern<DAS_CALL_METHOD(_method_1033), SimNode_ExtFuncCall >(lib,"GetMaxNetSerializeBits","das_call_member< uint64 (FByteProperty::*)() const , &FByteProperty::GetMaxNetSerializeBits >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FInt8Property,EInternal,FFieldClass *>(*this,lib,"FInt8Property","FInt8Property")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1976:2
	makeExtern< FFieldClass * (*)(const FInt8Property *) , _dasUnreal_static_393_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_393_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt8Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1976:2
	makeExtern< FField * (*)(const FInt8Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_394_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_394_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FInt8Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1976:2
	makeExtern< uint64 (*)(const FInt8Property *) , _dasUnreal_static_395_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_395_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt8Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1976:2
	makeExtern< uint64 (*)(const FInt8Property *) , _dasUnreal_static_396_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_396_StaticClassCastFlags")
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
	makeExtern< FFieldClass * (*)(const FInt16Property *) , _dasUnreal_static_397_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_397_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2015:2
	makeExtern< FField * (*)(const FInt16Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_398_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_398_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2015:2
	makeExtern< uint64 (*)(const FInt16Property *) , _dasUnreal_static_399_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_399_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2015:2
	makeExtern< uint64 (*)(const FInt16Property *) , _dasUnreal_static_400_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_400_StaticClassCastFlags")
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
	makeExtern< FFieldClass * (*)(const FIntProperty *) , _dasUnreal_static_401_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_401_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FIntProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2055:2
	makeExtern< FField * (*)(const FIntProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_402_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_402_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FIntProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2055:2
	makeExtern< uint64 (*)(const FIntProperty *) , _dasUnreal_static_403_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_403_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FIntProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2055:2
	makeExtern< uint64 (*)(const FIntProperty *) , _dasUnreal_static_404_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_404_StaticClassCastFlags")
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
	makeExtern< FFieldClass * (*)(const FInt64Property *) , _dasUnreal_static_405_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_405_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2094:2
	makeExtern< FField * (*)(const FInt64Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_406_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_406_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2094:2
	makeExtern< uint64 (*)(const FInt64Property *) , _dasUnreal_static_407_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_407_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2094:2
	makeExtern< uint64 (*)(const FInt64Property *) , _dasUnreal_static_408_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_408_StaticClassCastFlags")
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
	makeExtern< FFieldClass * (*)(const FUInt16Property *) , _dasUnreal_static_409_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_409_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2133:2
	makeExtern< FField * (*)(const FUInt16Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_410_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_410_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FUInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2133:2
	makeExtern< uint64 (*)(const FUInt16Property *) , _dasUnreal_static_411_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_411_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2133:2
	makeExtern< uint64 (*)(const FUInt16Property *) , _dasUnreal_static_412_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_412_StaticClassCastFlags")
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
	makeExtern< FFieldClass * (*)(const FUInt32Property *) , _dasUnreal_static_413_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_413_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt32Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2172:2
	makeExtern< FField * (*)(const FUInt32Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_414_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_414_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FUInt32Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2172:2
	makeExtern< uint64 (*)(const FUInt32Property *) , _dasUnreal_static_415_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_415_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt32Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2172:2
	makeExtern< uint64 (*)(const FUInt32Property *) , _dasUnreal_static_416_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_416_StaticClassCastFlags")
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
	makeExtern< FFieldClass * (*)(const FUInt64Property *) , _dasUnreal_static_417_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_417_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2211:2
	makeExtern< FField * (*)(const FUInt64Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_418_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_418_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FUInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2211:2
	makeExtern< uint64 (*)(const FUInt64Property *) , _dasUnreal_static_419_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_419_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2211:2
	makeExtern< uint64 (*)(const FUInt64Property *) , _dasUnreal_static_420_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_420_StaticClassCastFlags")
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
	makeExtern< FFieldClass * (*)(const FFloatProperty *) , _dasUnreal_static_421_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_421_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FFloatProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2251:2
	makeExtern< FField * (*)(const FFloatProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_422_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_422_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FFloatProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2251:2
	makeExtern< uint64 (*)(const FFloatProperty *) , _dasUnreal_static_423_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_423_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FFloatProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2251:2
	makeExtern< uint64 (*)(const FFloatProperty *) , _dasUnreal_static_424_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_424_StaticClassCastFlags")
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
	makeExtern< FFieldClass * (*)(const FDoubleProperty *) , _dasUnreal_static_425_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_425_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FDoubleProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2290:2
	makeExtern< FField * (*)(const FDoubleProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_426_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_426_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FDoubleProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2290:2
	makeExtern< uint64 (*)(const FDoubleProperty *) , _dasUnreal_static_427_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_427_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FDoubleProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2290:2
	makeExtern< uint64 (*)(const FDoubleProperty *) , _dasUnreal_static_428_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_428_StaticClassCastFlags")
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
	makeExtern< FFieldClass * (*)(const FBoolProperty *) , _dasUnreal_static_429_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_429_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2340:2
	makeExtern< FField * (*)(const FBoolProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_430_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_430_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2340:2
	makeExtern< uint64 (*)(const FBoolProperty *) , _dasUnreal_static_431_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_431_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2340:2
	makeExtern< uint64 (*)(const FBoolProperty *) , _dasUnreal_static_432_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_432_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FBoolProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FBoolProperty","FBoolProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FBoolProperty,UField *>(*this,lib,"FBoolProperty","FBoolProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2384:15
	makeExtern< void (*)(FBoolProperty &,FArchive &) , _dasUnreal_virtual_433_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_433_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2388:15
	makeExtern< void (*)(FBoolProperty &,const FField &) , _dasUnreal_virtual_434_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_434_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2391:18
	makeExtern< FString (*)(const FBoolProperty &,FString *,unsigned int) , _dasUnreal_virtual_435_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_435_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2392:18
	makeExtern< FString (*)(const FBoolProperty &,FString &) , _dasUnreal_virtual_436_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_436_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2393:18
	makeExtern< FString (*)(const FBoolProperty &) , _dasUnreal_virtual_437_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_437_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2397:15
	makeExtern< void (*)(FBoolProperty &,FArchive &) , _dasUnreal_virtual_438_LinkInternal , SimNode_ExtFuncCall >(lib,"LinkInternal","_dasUnreal_virtual_438_LinkInternal")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2398:15
	makeExtern< bool (*)(const FBoolProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_439_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_439_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2405:15
	makeExtern< void (*)(const FBoolProperty &,void *,const void *,int) , _dasUnreal_virtual_440_CopyValuesInternal , SimNode_ExtFuncCall >(lib,"CopyValuesInternal","_dasUnreal_virtual_440_CopyValuesInternal")
		->args({"self","Dest","Src","Count"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2406:15
	makeExtern< void (*)(const FBoolProperty &,void *) , _dasUnreal_virtual_441_ClearValueInternal , SimNode_ExtFuncCall >(lib,"ClearValueInternal","_dasUnreal_virtual_441_ClearValueInternal")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2407:15
	makeExtern< void (*)(const FBoolProperty &,void *) , _dasUnreal_virtual_442_InitializeValueInternal , SimNode_ExtFuncCall >(lib,"InitializeValueInternal","_dasUnreal_virtual_442_InitializeValueInternal")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2408:16
	makeExtern< int (*)(const FBoolProperty &) , _dasUnreal_virtual_443_GetMinAlignment , SimNode_ExtFuncCall >(lib,"GetMinAlignment","_dasUnreal_virtual_443_GetMinAlignment")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2411:15
	makeExtern< void (*)(const FBoolProperty &,FBlake3 &,bool) , _dasUnreal_virtual_444_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_444_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1086 = das::das_call_member< bool (FBoolProperty::*)(const void *) const,&FBoolProperty::GetPropertyValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2418:19
	makeExtern<DAS_CALL_METHOD(_method_1086), SimNode_ExtFuncCall >(lib,"GetPropertyValue","das_call_member< bool (FBoolProperty::*)(const void *) const , &FBoolProperty::GetPropertyValue >::invoke")
		->args({"self","A"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1087 = das::das_call_member< bool (FBoolProperty::*)(const void *,int) const,&FBoolProperty::GetPropertyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2424:19
	makeExtern<DAS_CALL_METHOD(_method_1087), SimNode_ExtFuncCall >(lib,"GetPropertyValue_InContainer","das_call_member< bool (FBoolProperty::*)(const void *,int) const , &FBoolProperty::GetPropertyValue_InContainer >::invoke")
		->args({"self","A","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2428:26
	makeExtern< bool (*)(const FBoolProperty *) , _dasUnreal_static_445_GetDefaultPropertyValue , SimNode_ExtFuncCall >(lib,"GetDefaultPropertyValue","_dasUnreal_static_445_GetDefaultPropertyValue")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
}
}

