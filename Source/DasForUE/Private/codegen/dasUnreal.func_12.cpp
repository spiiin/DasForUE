// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_12() {
	addCtorAndUsing<FByteProperty,EInternal,FFieldClass *>(*this,lib,"FByteProperty","FByteProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1902:2
	makeExtern< FFieldClass * (*)(const FByteProperty *) , _dasUnreal_static_337_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_337_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FByteProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1902:2
	makeExtern< FField * (*)(const FByteProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_338_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_338_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FByteProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1902:2
	makeExtern< uint64 (*)(const FByteProperty *) , _dasUnreal_static_339_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_339_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FByteProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1902:2
	makeExtern< uint64 (*)(const FByteProperty *) , _dasUnreal_static_340_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_340_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FByteProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FByteProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FByteProperty","FByteProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FByteProperty,FFieldVariant,const UECodeGen_Private::FBytePropertyParams &>(*this,lib,"FByteProperty","FByteProperty")
		->args({"InOwner","Prop"});
	addCtorAndUsing<FByteProperty,UField *>(*this,lib,"FByteProperty","FByteProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1935:15
	makeExtern< void (*)(FByteProperty &,FReferenceCollector &) , _dasUnreal_virtual_341_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_341_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1940:15
	makeExtern< void (*)(FByteProperty &,const FField &) , _dasUnreal_virtual_342_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_342_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1943:18
	makeExtern< FString (*)(const FByteProperty &,FString *,unsigned int) , _dasUnreal_virtual_343_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_343_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->arg_init(2,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1955:15
	makeExtern< void (*)(const FByteProperty &,FBlake3 &,bool) , _dasUnreal_virtual_344_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_344_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1960:17
	makeExtern< UEnum * (*)(const FByteProperty &) , _dasUnreal_virtual_345_GetIntPropertyEnum , SimNode_ExtFuncCall >(lib,"GetIntPropertyEnum","_dasUnreal_virtual_345_GetIntPropertyEnum")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_940 = das::das_call_member< uint64 (FByteProperty::*)() const,&FByteProperty::GetMaxNetSerializeBits >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1964:9
	makeExtern<DAS_CALL_METHOD(_method_940), SimNode_ExtFuncCall >(lib,"GetMaxNetSerializeBits","das_call_member< uint64 (FByteProperty::*)() const , &FByteProperty::GetMaxNetSerializeBits >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FInt8Property,EInternal,FFieldClass *>(*this,lib,"FInt8Property","FInt8Property")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1976:2
	makeExtern< FFieldClass * (*)(const FInt8Property *) , _dasUnreal_static_346_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_346_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt8Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1976:2
	makeExtern< FField * (*)(const FInt8Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_347_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_347_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FInt8Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1976:2
	makeExtern< uint64 (*)(const FInt8Property *) , _dasUnreal_static_348_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_348_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt8Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1976:2
	makeExtern< uint64 (*)(const FInt8Property *) , _dasUnreal_static_349_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_349_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt8Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FInt8Property,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FInt8Property","FInt8Property")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FInt8Property,FFieldVariant,const UECodeGen_Private::FGenericPropertyParams &>(*this,lib,"FInt8Property","FInt8Property")
		->args({"InOwner","Prop"});
	addCtorAndUsing<FInt8Property,UField *>(*this,lib,"FInt8Property","FInt8Property")
		->args({"InField"});
	addCtorAndUsing<FInt16Property,EInternal,FFieldClass *>(*this,lib,"FInt16Property","FInt16Property")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2015:2
	makeExtern< FFieldClass * (*)(const FInt16Property *) , _dasUnreal_static_350_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_350_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2015:2
	makeExtern< FField * (*)(const FInt16Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_351_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_351_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2015:2
	makeExtern< uint64 (*)(const FInt16Property *) , _dasUnreal_static_352_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_352_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2015:2
	makeExtern< uint64 (*)(const FInt16Property *) , _dasUnreal_static_353_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_353_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FInt16Property,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FInt16Property","FInt16Property")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FInt16Property,FFieldVariant,const UECodeGen_Private::FGenericPropertyParams &>(*this,lib,"FInt16Property","FInt16Property")
		->args({"InOwner","Prop"});
	addCtorAndUsing<FInt16Property,UField *>(*this,lib,"FInt16Property","FInt16Property")
		->args({"InField"});
	addCtorAndUsing<FIntProperty,EInternal,FFieldClass *>(*this,lib,"FIntProperty","FIntProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2055:2
	makeExtern< FFieldClass * (*)(const FIntProperty *) , _dasUnreal_static_354_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_354_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FIntProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2055:2
	makeExtern< FField * (*)(const FIntProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_355_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_355_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FIntProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2055:2
	makeExtern< uint64 (*)(const FIntProperty *) , _dasUnreal_static_356_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_356_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FIntProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2055:2
	makeExtern< uint64 (*)(const FIntProperty *) , _dasUnreal_static_357_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_357_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FIntProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FIntProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FIntProperty","FIntProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FIntProperty,FFieldVariant,const UECodeGen_Private::FGenericPropertyParams &>(*this,lib,"FIntProperty","FIntProperty")
		->args({"InOwner","Prop"});
	addCtorAndUsing<FIntProperty,UField *>(*this,lib,"FIntProperty","FIntProperty")
		->args({"InField"});
	addCtorAndUsing<FInt64Property,EInternal,FFieldClass *>(*this,lib,"FInt64Property","FInt64Property")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2094:2
	makeExtern< FFieldClass * (*)(const FInt64Property *) , _dasUnreal_static_358_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_358_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2094:2
	makeExtern< FField * (*)(const FInt64Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_359_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_359_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2094:2
	makeExtern< uint64 (*)(const FInt64Property *) , _dasUnreal_static_360_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_360_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2094:2
	makeExtern< uint64 (*)(const FInt64Property *) , _dasUnreal_static_361_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_361_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FInt64Property,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FInt64Property","FInt64Property")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FInt64Property,FFieldVariant,const UECodeGen_Private::FGenericPropertyParams &>(*this,lib,"FInt64Property","FInt64Property")
		->args({"InOwner","Prop"});
	addCtorAndUsing<FInt64Property,UField *>(*this,lib,"FInt64Property","FInt64Property")
		->args({"InField"});
	addCtorAndUsing<FUInt16Property,EInternal,FFieldClass *>(*this,lib,"FUInt16Property","FUInt16Property")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2133:2
	makeExtern< FFieldClass * (*)(const FUInt16Property *) , _dasUnreal_static_362_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_362_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2133:2
	makeExtern< FField * (*)(const FUInt16Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_363_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_363_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FUInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2133:2
	makeExtern< uint64 (*)(const FUInt16Property *) , _dasUnreal_static_364_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_364_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2133:2
	makeExtern< uint64 (*)(const FUInt16Property *) , _dasUnreal_static_365_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_365_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt16Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FUInt16Property,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FUInt16Property","FUInt16Property")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FUInt16Property,FFieldVariant,const UECodeGen_Private::FGenericPropertyParams &>(*this,lib,"FUInt16Property","FUInt16Property")
		->args({"InOwner","Prop"});
	addCtorAndUsing<FUInt16Property,UField *>(*this,lib,"FUInt16Property","FUInt16Property")
		->args({"InField"});
	addCtorAndUsing<FUInt32Property,EInternal,FFieldClass *>(*this,lib,"FUInt32Property","FUInt32Property")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2172:2
	makeExtern< FFieldClass * (*)(const FUInt32Property *) , _dasUnreal_static_366_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_366_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt32Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2172:2
	makeExtern< FField * (*)(const FUInt32Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_367_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_367_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FUInt32Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2172:2
	makeExtern< uint64 (*)(const FUInt32Property *) , _dasUnreal_static_368_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_368_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt32Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2172:2
	makeExtern< uint64 (*)(const FUInt32Property *) , _dasUnreal_static_369_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_369_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt32Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FUInt32Property,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FUInt32Property","FUInt32Property")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FUInt32Property,FFieldVariant,const UECodeGen_Private::FGenericPropertyParams &>(*this,lib,"FUInt32Property","FUInt32Property")
		->args({"InOwner","Prop"});
	addCtorAndUsing<FUInt32Property,UField *>(*this,lib,"FUInt32Property","FUInt32Property")
		->args({"InField"});
	addCtorAndUsing<FUInt64Property,EInternal,FFieldClass *>(*this,lib,"FUInt64Property","FUInt64Property")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2211:2
	makeExtern< FFieldClass * (*)(const FUInt64Property *) , _dasUnreal_static_370_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_370_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2211:2
	makeExtern< FField * (*)(const FUInt64Property *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_371_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_371_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FUInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2211:2
	makeExtern< uint64 (*)(const FUInt64Property *) , _dasUnreal_static_372_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_372_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2211:2
	makeExtern< uint64 (*)(const FUInt64Property *) , _dasUnreal_static_373_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_373_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FUInt64Property*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FUInt64Property,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FUInt64Property","FUInt64Property")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FUInt64Property,FFieldVariant,const UECodeGen_Private::FGenericPropertyParams &>(*this,lib,"FUInt64Property","FUInt64Property")
		->args({"InOwner","Prop"});
	addCtorAndUsing<FUInt64Property,UField *>(*this,lib,"FUInt64Property","FUInt64Property")
		->args({"InField"});
	addCtorAndUsing<FFloatProperty,EInternal,FFieldClass *>(*this,lib,"FFloatProperty","FFloatProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2251:2
	makeExtern< FFieldClass * (*)(const FFloatProperty *) , _dasUnreal_static_374_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_374_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FFloatProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2251:2
	makeExtern< FField * (*)(const FFloatProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_375_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_375_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FFloatProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2251:2
	makeExtern< uint64 (*)(const FFloatProperty *) , _dasUnreal_static_376_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_376_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FFloatProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2251:2
	makeExtern< uint64 (*)(const FFloatProperty *) , _dasUnreal_static_377_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_377_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FFloatProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FFloatProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FFloatProperty","FFloatProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FFloatProperty,FFieldVariant,const UECodeGen_Private::FGenericPropertyParams &>(*this,lib,"FFloatProperty","FFloatProperty")
		->args({"InOwner","Prop"});
	addCtorAndUsing<FFloatProperty,UField *>(*this,lib,"FFloatProperty","FFloatProperty")
		->args({"InField"});
	addCtorAndUsing<FDoubleProperty,EInternal,FFieldClass *>(*this,lib,"FDoubleProperty","FDoubleProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2290:2
	makeExtern< FFieldClass * (*)(const FDoubleProperty *) , _dasUnreal_static_378_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_378_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FDoubleProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2290:2
	makeExtern< FField * (*)(const FDoubleProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_379_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_379_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FDoubleProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2290:2
	makeExtern< uint64 (*)(const FDoubleProperty *) , _dasUnreal_static_380_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_380_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FDoubleProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2290:2
	makeExtern< uint64 (*)(const FDoubleProperty *) , _dasUnreal_static_381_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_381_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FDoubleProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FDoubleProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FDoubleProperty","FDoubleProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FDoubleProperty,FFieldVariant,const UECodeGen_Private::FGenericPropertyParams &>(*this,lib,"FDoubleProperty","FDoubleProperty")
		->args({"InOwner","Prop"});
	addCtorAndUsing<FDoubleProperty,UField *>(*this,lib,"FDoubleProperty","FDoubleProperty")
		->args({"InField"});
	addCtorAndUsing<FBoolProperty,EInternal,FFieldClass *>(*this,lib,"FBoolProperty","FBoolProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2340:2
	makeExtern< FFieldClass * (*)(const FBoolProperty *) , _dasUnreal_static_382_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_382_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2340:2
	makeExtern< FField * (*)(const FBoolProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_383_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_383_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2340:2
	makeExtern< uint64 (*)(const FBoolProperty *) , _dasUnreal_static_384_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_384_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBoolProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2340:2
	makeExtern< uint64 (*)(const FBoolProperty *) , _dasUnreal_static_385_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_385_StaticClassCastFlags")
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
	makeExtern< void (*)(FBoolProperty &,const FField &) , _dasUnreal_virtual_386_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_386_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2391:18
	makeExtern< FString (*)(const FBoolProperty &,FString *,unsigned int) , _dasUnreal_virtual_387_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_387_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2392:18
	makeExtern< FString (*)(const FBoolProperty &,FString &) , _dasUnreal_virtual_388_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_388_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2393:18
	makeExtern< FString (*)(const FBoolProperty &) , _dasUnreal_virtual_389_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_389_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2398:15
	makeExtern< bool (*)(const FBoolProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_390_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_390_Identical")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2405:15
	makeExtern< void (*)(const FBoolProperty &,void *,const void *,int) , _dasUnreal_virtual_391_CopyValuesInternal , SimNode_ExtFuncCall >(lib,"CopyValuesInternal","_dasUnreal_virtual_391_CopyValuesInternal")
		->args({"self","Dest","Src","Count"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

