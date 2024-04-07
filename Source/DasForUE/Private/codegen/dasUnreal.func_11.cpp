// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_11() {
	using _method_834 = das::das_call_member< bool (UPackage::*)() const,&UPackage::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:893:7
	makeExtern<DAS_CALL_METHOD(_method_834), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (UPackage::*)() const , &UPackage::HasMetaData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_835 = das::das_call_member< UMetaData * (UPackage::*)(),&UPackage::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:909:19
	makeExtern<DAS_CALL_METHOD(_method_835), SimNode_ExtFuncCall >(lib,"GetMetaData","das_call_member< UMetaData * (UPackage::*)() , &UPackage::GetMetaData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:950:14
	makeExtern< void (*)(const UPackage *) , _dasUnreal_static_286_WaitForAsyncFileWrites , SimNode_ExtFuncCall >(lib,"WaitForAsyncFileWrites","_dasUnreal_static_286_WaitForAsyncFileWrites")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:953:14
	makeExtern< bool (*)(const UPackage *) , _dasUnreal_static_287_HasAsyncFileWrites , SimNode_ExtFuncCall >(lib,"HasAsyncFileWrites","_dasUnreal_static_287_HasAsyncFileWrites")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:962:14
	makeExtern< bool (*)(const UPackage *,UPackage *,const UObject *) , _dasUnreal_static_288_IsEmptyPackage , SimNode_ExtFuncCall >(lib,"IsEmptyPackage","_dasUnreal_static_288_IsEmptyPackage")
		->args({"self","Package","LastReferencer"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->arg_init(2,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:155:2
	makeExtern< FFieldClass * (*)(const FProperty *) , _dasUnreal_static_289_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_289_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:155:2
	makeExtern< uint64 (*)(const FProperty *) , _dasUnreal_static_290_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_290_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:155:2
	makeExtern< uint64 (*)(const FProperty *) , _dasUnreal_static_291_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_291_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:211:31
	makeExtern< void (*)(FProperty &,const FField &) , _dasUnreal_virtual_292_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_292_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:228:31
	makeExtern< FName (*)(const FProperty *,UStruct *,FName) , _dasUnreal_static_293_FindRedirectedPropertyName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FindRedirectedPropertyName","_dasUnreal_static_293_FindRedirectedPropertyName")
		->args({"self","ObjectStruct","OldName"})
		->arg_type(0,makeType<TExplicit<const FProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:233:34
	makeExtern< FString (*)(const FProperty &,FString &) , _dasUnreal_virtual_294_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_294_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:234:31
	makeExtern< bool (*)(const FProperty &) , _dasUnreal_virtual_295_PassCPPArgsByRef , SimNode_ExtFuncCall >(lib,"PassCPPArgsByRef","_dasUnreal_virtual_295_PassCPPArgsByRef")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_846 = das::das_call_member< FString (FProperty::*)() const,&FProperty::GetNameCPP >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:242:26
	makeExtern<DAS_CALL_METHOD(_method_846), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameCPP","das_call_member< FString (FProperty::*)() const , &FProperty::GetNameCPP >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:250:34
	makeExtern< FString (*)(const FProperty &,FString *,unsigned int) , _dasUnreal_virtual_296_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_296_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->arg_init(2,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:252:34
	makeExtern< FString (*)(const FProperty &) , _dasUnreal_virtual_297_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_297_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_849 = das::das_call_member< UPropertyWrapper * (FProperty::*)(),&FProperty::GetUPropertyWrapper >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:257:36
	makeExtern<DAS_CALL_METHOD(_method_849), SimNode_ExtFuncCall >(lib,"GetUPropertyWrapper","das_call_member< UPropertyWrapper * (FProperty::*)() , &FProperty::GetUPropertyWrapper >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:261:15
	makeExtern< bool (*)(const FProperty &) , _dasUnreal_virtual_298_HasSetter , SimNode_ExtFuncCall >(lib,"HasSetter","_dasUnreal_virtual_298_HasSetter")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:266:15
	makeExtern< bool (*)(const FProperty &) , _dasUnreal_virtual_299_HasGetter , SimNode_ExtFuncCall >(lib,"HasGetter","_dasUnreal_virtual_299_HasGetter")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:271:15
	makeExtern< bool (*)(const FProperty &) , _dasUnreal_virtual_300_HasSetterOrGetter , SimNode_ExtFuncCall >(lib,"HasSetterOrGetter","_dasUnreal_virtual_300_HasSetterOrGetter")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:280:15
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_301_CallSetter , SimNode_ExtFuncCall >(lib,"CallSetter","_dasUnreal_virtual_301_CallSetter")
		->args({"self","Container","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:289:15
	makeExtern< void (*)(const FProperty &,const void *,void *) , _dasUnreal_virtual_302_CallGetter , SimNode_ExtFuncCall >(lib,"CallGetter","_dasUnreal_virtual_302_CallGetter")
		->args({"self","Container","OutValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_855 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetOffset_ForDebug >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:314:20
	makeExtern<DAS_CALL_METHOD(_method_855), SimNode_ExtFuncCall >(lib,"GetOffset_ForDebug","das_call_member< int (FProperty::*)() const , &FProperty::GetOffset_ForDebug >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_856 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetOffset_ForUFunction >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:319:20
	makeExtern<DAS_CALL_METHOD(_method_856), SimNode_ExtFuncCall >(lib,"GetOffset_ForUFunction","das_call_member< int (FProperty::*)() const , &FProperty::GetOffset_ForUFunction >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_857 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetOffset_ForGC >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:324:20
	makeExtern<DAS_CALL_METHOD(_method_857), SimNode_ExtFuncCall >(lib,"GetOffset_ForGC","das_call_member< int (FProperty::*)() const , &FProperty::GetOffset_ForGC >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_858 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetOffset_ForInternal >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:329:20
	makeExtern<DAS_CALL_METHOD(_method_858), SimNode_ExtFuncCall >(lib,"GetOffset_ForInternal","das_call_member< int (FProperty::*)() const , &FProperty::GetOffset_ForInternal >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_859 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetOffset_ReplaceWith_ContainerPtrToValuePtr >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:334:20
	makeExtern<DAS_CALL_METHOD(_method_859), SimNode_ExtFuncCall >(lib,"GetOffset_ReplaceWith_ContainerPtrToValuePtr","das_call_member< int (FProperty::*)() const , &FProperty::GetOffset_ReplaceWith_ContainerPtrToValuePtr >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:379:31
	makeExtern< bool (*)(const FProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_303_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_303_Identical")
		->args({"self","A","B","PortFlags"})
		->arg_init(3,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_861 = das::das_call_member< bool (FProperty::*)(const void *,const void *,int,unsigned int) const,&FProperty::Identical_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:390:7
	makeExtern<DAS_CALL_METHOD(_method_861), SimNode_ExtFuncCall >(lib,"Identical_InContainer","das_call_member< bool (FProperty::*)(const void *,const void *,int,unsigned int) const , &FProperty::Identical_InContainer >::invoke")
		->args({"self","A","B","ArrayIndex","PortFlags"})
		->arg_init(3,make_smart<ExprConstInt>(0))
		->arg_init(4,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:447:31
	makeExtern< bool (*)(const FProperty &) , _dasUnreal_virtual_304_SupportsNetSharedSerialization , SimNode_ExtFuncCall >(lib,"SupportsNetSharedSerialization","_dasUnreal_virtual_304_SupportsNetSharedSerialization")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:450:15
	makeExtern< void (*)(const FProperty &,FString &,const void *,const void *,UObject *,int,UObject *) , _dasUnreal_virtual_305_ExportTextItem , SimNode_ExtFuncCall >(lib,"ExportTextItem","_dasUnreal_virtual_305_ExportTextItem")
		->args({"self","ValueStr","PropertyValue","DefaultValue","Parent","PortFlags","ExportRootScope"})
		->arg_init(6,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_864 = das::das_call_member< void (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const,&FProperty::ExportTextItem_Direct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:455:7
	makeExtern<DAS_CALL_METHOD(_method_864), SimNode_ExtFuncCall >(lib,"ExportTextItem_Direct","das_call_member< void (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const , &FProperty::ExportTextItem_Direct >::invoke")
		->args({"self","ValueStr","PropertyValue","DefaultValue","Parent","PortFlags","ExportRootScope"})
		->arg_init(6,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_865 = das::das_call_member< void (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const,&FProperty::ExportTextItem_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:460:7
	makeExtern<DAS_CALL_METHOD(_method_865), SimNode_ExtFuncCall >(lib,"ExportTextItem_InContainer","das_call_member< void (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const , &FProperty::ExportTextItem_InContainer >::invoke")
		->args({"self","ValueStr","Container","DefaultValue","Parent","PortFlags","ExportRootScope"})
		->arg_init(6,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_866 = das::das_call_member< void (FProperty::*)(void *,const void *) const,&FProperty::SetValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:509:19
	makeExtern<DAS_CALL_METHOD(_method_866), SimNode_ExtFuncCall >(lib,"SetValue_InContainer","das_call_member< void (FProperty::*)(void *,const void *) const , &FProperty::SetValue_InContainer >::invoke")
		->args({"self","OutContainer","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_867 = das::das_call_member< void (FProperty::*)(const void *,void *) const,&FProperty::GetValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:520:19
	makeExtern<DAS_CALL_METHOD(_method_867), SimNode_ExtFuncCall >(lib,"GetValue_InContainer","das_call_member< void (FProperty::*)(const void *,void *) const , &FProperty::GetValue_InContainer >::invoke")
		->args({"self","InContainer","OutValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_868 = das::das_call_member< void (FProperty::*)(void *,const void *,int) const,&FProperty::SetSingleValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:538:23
	makeExtern<DAS_CALL_METHOD(_method_868), SimNode_ExtFuncCall >(lib,"SetSingleValue_InContainer","das_call_member< void (FProperty::*)(void *,const void *,int) const , &FProperty::SetSingleValue_InContainer >::invoke")
		->args({"self","OutContainer","InValue","ArrayIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_869 = das::das_call_member< void (FProperty::*)(const void *,void *,int) const,&FProperty::GetSingleValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:546:23
	makeExtern<DAS_CALL_METHOD(_method_869), SimNode_ExtFuncCall >(lib,"GetSingleValue_InContainer","das_call_member< void (FProperty::*)(const void *,void *,int) const , &FProperty::GetSingleValue_InContainer >::invoke")
		->args({"self","InContainer","OutValue","ArrayIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_870 = das::das_call_member< void * (FProperty::*)() const,&FProperty::AllocateAndInitializeValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:549:24
	makeExtern<DAS_CALL_METHOD(_method_870), SimNode_ExtFuncCall >(lib,"AllocateAndInitializeValue","das_call_member< void * (FProperty::*)() const , &FProperty::AllocateAndInitializeValue >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_871 = das::das_call_member< void (FProperty::*)(void *) const,&FProperty::DestroyAndFreeValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:552:23
	makeExtern<DAS_CALL_METHOD(_method_871), SimNode_ExtFuncCall >(lib,"DestroyAndFreeValue","das_call_member< void (FProperty::*)(void *) const , &FProperty::DestroyAndFreeValue >::invoke")
		->args({"self","InMemory"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:576:32
	makeExtern< void * (*)(const FProperty &,const FProperty *,void *,int) , _dasUnreal_virtual_306_GetValueAddressAtIndex_Direct , SimNode_ExtFuncCall >(lib,"GetValueAddressAtIndex_Direct","_dasUnreal_virtual_306_GetValueAddressAtIndex_Direct")
		->args({"self","Inner","InValueAddress","Index"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:590:31
	makeExtern< void (*)(const FProperty &,FBlake3 &,bool) , _dasUnreal_virtual_307_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_307_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_874 = das::das_call_member< bool (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const,&FProperty::ExportText_Direct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:599:23
	makeExtern<DAS_CALL_METHOD(_method_874), SimNode_ExtFuncCall >(lib,"ExportText_Direct","das_call_member< bool (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const , &FProperty::ExportText_Direct >::invoke")
		->args({"self","ValueStr","Data","Delta","Parent","PortFlags","ExportRootScope"})
		->arg_init(6,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_875 = das::das_call_member< bool (FProperty::*)(int,FString &,const void *,const void *,UObject *,int,UObject *) const,&FProperty::ExportText_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:600:19
	makeExtern<DAS_CALL_METHOD(_method_875), SimNode_ExtFuncCall >(lib,"ExportText_InContainer","das_call_member< bool (FProperty::*)(int,FString &,const void *,const void *,UObject *,int,UObject *) const , &FProperty::ExportText_InContainer >::invoke")
		->args({"self","Index","ValueStr","Data","Delta","Parent","PortFlags","ExportRootScope"})
		->arg_init(7,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_876 = das::das_call_member< bool (FProperty::*)(int) const,&FProperty::IsInContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:731:19
	makeExtern<DAS_CALL_METHOD(_method_876), SimNode_ExtFuncCall >(lib,"IsInContainer","das_call_member< bool (FProperty::*)(int) const , &FProperty::IsInContainer >::invoke")
		->args({"self","ContainerSize"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_877 = das::das_call_member< bool (FProperty::*)(UStruct *) const,&FProperty::IsInContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:736:19
	makeExtern<DAS_CALL_METHOD(_method_877), SimNode_ExtFuncCall >(lib,"IsInContainer","das_call_member< bool (FProperty::*)(UStruct *) const , &FProperty::IsInContainer >::invoke")
		->args({"self","ContainerClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_878 = das::das_call_member< void (FProperty::*)(void *,const void *) const,&FProperty::CopySingleValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:752:19
	makeExtern<DAS_CALL_METHOD(_method_878), SimNode_ExtFuncCall >(lib,"CopySingleValue","das_call_member< void (FProperty::*)(void *,const void *) const , &FProperty::CopySingleValue >::invoke")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_879 = das::das_call_member< unsigned int (FProperty::*)(const void *) const,&FProperty::GetValueTypeHash >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:770:25
	makeExtern<DAS_CALL_METHOD(_method_879), SimNode_ExtFuncCall >(lib,"GetValueTypeHash","das_call_member< unsigned int (FProperty::*)(const void *) const , &FProperty::GetValueTypeHash >::invoke")
		->args({"self","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_880 = das::das_call_member< void (FProperty::*)(void *,const void *) const,&FProperty::CopyCompleteValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:786:19
	makeExtern<DAS_CALL_METHOD(_method_880), SimNode_ExtFuncCall >(lib,"CopyCompleteValue","das_call_member< void (FProperty::*)(void *,const void *) const , &FProperty::CopyCompleteValue >::invoke")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_881 = das::das_call_member< void (FProperty::*)(void *,const void *) const,&FProperty::CopyCompleteValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:800:19
	makeExtern<DAS_CALL_METHOD(_method_881), SimNode_ExtFuncCall >(lib,"CopyCompleteValue_InContainer","das_call_member< void (FProperty::*)(void *,const void *) const , &FProperty::CopyCompleteValue_InContainer >::invoke")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:815:31
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_308_CopySingleValueToScriptVM , SimNode_ExtFuncCall >(lib,"CopySingleValueToScriptVM","_dasUnreal_virtual_308_CopySingleValueToScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:825:31
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_309_CopyCompleteValueToScriptVM , SimNode_ExtFuncCall >(lib,"CopyCompleteValueToScriptVM","_dasUnreal_virtual_309_CopyCompleteValueToScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:830:31
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_310_CopyCompleteValueToScriptVM_InContainer , SimNode_ExtFuncCall >(lib,"CopyCompleteValueToScriptVM_InContainer","_dasUnreal_virtual_310_CopyCompleteValueToScriptVM_InContainer")
		->args({"self","OutValue","InContainer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:831:31
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_311_CopyCompleteValueFromScriptVM_InContainer , SimNode_ExtFuncCall >(lib,"CopyCompleteValueFromScriptVM_InContainer","_dasUnreal_virtual_311_CopyCompleteValueFromScriptVM_InContainer")
		->args({"self","OutContainer","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:843:31
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_312_CopySingleValueFromScriptVM , SimNode_ExtFuncCall >(lib,"CopySingleValueFromScriptVM","_dasUnreal_virtual_312_CopySingleValueFromScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:853:31
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_313_CopyCompleteValueFromScriptVM , SimNode_ExtFuncCall >(lib,"CopyCompleteValueFromScriptVM","_dasUnreal_virtual_313_CopyCompleteValueFromScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_888 = das::das_call_member< void (FProperty::*)(void *) const,&FProperty::ClearValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:861:19
	makeExtern<DAS_CALL_METHOD(_method_888), SimNode_ExtFuncCall >(lib,"ClearValue","das_call_member< void (FProperty::*)(void *) const , &FProperty::ClearValue >::invoke")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_889 = das::das_call_member< void (FProperty::*)(void *,int) const,&FProperty::ClearValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:878:19
	makeExtern<DAS_CALL_METHOD(_method_889), SimNode_ExtFuncCall >(lib,"ClearValue_InContainer","das_call_member< void (FProperty::*)(void *,int) const , &FProperty::ClearValue_InContainer >::invoke")
		->args({"self","Data","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_890 = das::das_call_member< void (FProperty::*)(void *) const,&FProperty::DestroyValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:898:19
	makeExtern<DAS_CALL_METHOD(_method_890), SimNode_ExtFuncCall >(lib,"DestroyValue","das_call_member< void (FProperty::*)(void *) const , &FProperty::DestroyValue >::invoke")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_891 = das::das_call_member< void (FProperty::*)(void *) const,&FProperty::DestroyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:911:19
	makeExtern<DAS_CALL_METHOD(_method_891), SimNode_ExtFuncCall >(lib,"DestroyValue_InContainer","das_call_member< void (FProperty::*)(void *) const , &FProperty::DestroyValue_InContainer >::invoke")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_892 = das::das_call_member< void (FProperty::*)(void *) const,&FProperty::InitializeValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:929:19
	makeExtern<DAS_CALL_METHOD(_method_892), SimNode_ExtFuncCall >(lib,"InitializeValue","das_call_member< void (FProperty::*)(void *) const , &FProperty::InitializeValue >::invoke")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_893 = das::das_call_member< void (FProperty::*)(void *) const,&FProperty::InitializeValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:947:19
	makeExtern<DAS_CALL_METHOD(_method_893), SimNode_ExtFuncCall >(lib,"InitializeValue_InContainer","das_call_member< void (FProperty::*)(void *) const , &FProperty::InitializeValue_InContainer >::invoke")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_894 = das::das_call_member< bool (FProperty::*)(unsigned int) const,&FProperty::ShouldPort >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:971:23
	makeExtern<DAS_CALL_METHOD(_method_894), SimNode_ExtFuncCall >(lib,"ShouldPort","das_call_member< bool (FProperty::*)(unsigned int) const , &FProperty::ShouldPort >::invoke")
		->args({"self","PortFlags"})
		->arg_init(1,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:972:32
	makeExtern< FName (*)(const FProperty &) , _dasUnreal_virtual_314_GetID , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetID","_dasUnreal_virtual_314_GetID")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:984:32
	makeExtern< int (*)(const FProperty &) , _dasUnreal_virtual_315_GetMinAlignment , SimNode_ExtFuncCall >(lib,"GetMinAlignment","_dasUnreal_virtual_315_GetMinAlignment")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_897 = das::das_call_member< bool (FProperty::*)() const,&FProperty::ContainsWeakObjectReference >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1002:7
	makeExtern<DAS_CALL_METHOD(_method_897), SimNode_ExtFuncCall >(lib,"ContainsWeakObjectReference","das_call_member< bool (FProperty::*)() const , &FProperty::ContainsWeakObjectReference >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_898 = das::das_call_member< bool (FProperty::*)() const,&FProperty::ContainsInstancedObjectProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1014:19
	makeExtern<DAS_CALL_METHOD(_method_898), SimNode_ExtFuncCall >(lib,"ContainsInstancedObjectProperty","das_call_member< bool (FProperty::*)() const , &FProperty::ContainsInstancedObjectProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_899 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1027:20
	makeExtern<DAS_CALL_METHOD(_method_899), SimNode_ExtFuncCall >(lib,"GetSize","das_call_member< int (FProperty::*)() const , &FProperty::GetSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_900 = das::das_call_member< bool (FProperty::*)() const,&FProperty::ShouldDuplicateValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1038:7
	makeExtern<DAS_CALL_METHOD(_method_900), SimNode_ExtFuncCall >(lib,"ShouldDuplicateValue","das_call_member< bool (FProperty::*)() const , &FProperty::ShouldDuplicateValue >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_901 = das::das_call_member< FProperty * (FProperty::*)(),&FProperty::GetOwnerProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1046:13
	makeExtern<DAS_CALL_METHOD(_method_901), SimNode_ExtFuncCall >(lib,"GetOwnerProperty","das_call_member< FProperty * (FProperty::*)() , &FProperty::GetOwnerProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_902 = das::das_call_member< const FProperty * (FProperty::*)() const,&FProperty::GetOwnerProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1056:19
	makeExtern<DAS_CALL_METHOD(_method_902), SimNode_ExtFuncCall >(lib,"GetOwnerProperty","das_call_member< const FProperty * (FProperty::*)() const , &FProperty::GetOwnerProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_903 = das::das_call_member< bool (FProperty::*)(uint64) const,&FProperty::HasAnyPropertyFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1091:19
	makeExtern<DAS_CALL_METHOD(_method_903), SimNode_ExtFuncCall >(lib,"HasAnyPropertyFlags","das_call_member< bool (FProperty::*)(uint64) const , &FProperty::HasAnyPropertyFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_904 = das::das_call_member< bool (FProperty::*)(uint64) const,&FProperty::HasAllPropertyFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1105:19
	makeExtern<DAS_CALL_METHOD(_method_904), SimNode_ExtFuncCall >(lib,"HasAllPropertyFlags","das_call_member< bool (FProperty::*)(uint64) const , &FProperty::HasAllPropertyFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_905 = das::das_call_member< FProperty * (FProperty::*)(),&FProperty::GetRepOwner >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1114:25
	makeExtern<DAS_CALL_METHOD(_method_905), SimNode_ExtFuncCall >(lib,"GetRepOwner","das_call_member< FProperty * (FProperty::*)() , &FProperty::GetRepOwner >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_906 = das::das_call_member< bool (FProperty::*)() const,&FProperty::IsEditorOnlyProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1126:19
	makeExtern<DAS_CALL_METHOD(_method_906), SimNode_ExtFuncCall >(lib,"IsEditorOnlyProperty","das_call_member< bool (FProperty::*)() const , &FProperty::IsEditorOnlyProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1132:31
	makeExtern< bool (*)(const FProperty &,const FProperty *) , _dasUnreal_virtual_316_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_316_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_908 = das::das_call_member< ELifetimeCondition (FProperty::*)() const,&FProperty::GetBlueprintReplicationCondition >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1134:21
	makeExtern<DAS_CALL_METHOD(_method_908), SimNode_ExtFuncCall >(lib,"GetBlueprintReplicationCondition","das_call_member< ELifetimeCondition (FProperty::*)() const , &FProperty::GetBlueprintReplicationCondition >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_909 = das::das_call_member< void (FProperty::*)(ELifetimeCondition),&FProperty::SetBlueprintReplicationCondition >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1135:7
	makeExtern<DAS_CALL_METHOD(_method_909), SimNode_ExtFuncCall >(lib,"SetBlueprintReplicationCondition","das_call_member< void (FProperty::*)(ELifetimeCondition) , &FProperty::SetBlueprintReplicationCondition >::invoke")
		->args({"self","InBlueprintReplicationCondition"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FNumericProperty,EInternal,FFieldClass *>(*this,lib,"FNumericProperty","FNumericProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1488:2
	makeExtern< FFieldClass * (*)(const FNumericProperty *) , _dasUnreal_static_317_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_317_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FNumericProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1488:2
	makeExtern< FField * (*)(const FNumericProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_318_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_318_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FNumericProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1488:2
	makeExtern< uint64 (*)(const FNumericProperty *) , _dasUnreal_static_319_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_319_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FNumericProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1488:2
	makeExtern< uint64 (*)(const FNumericProperty *) , _dasUnreal_static_320_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_320_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FNumericProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FNumericProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FNumericProperty","FNumericProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FNumericProperty,UField *>(*this,lib,"FNumericProperty","FNumericProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1519:15
	makeExtern< void (*)(const FNumericProperty &,FString &,const void *,EPropertyPointerType,const void *,UObject *,int,UObject *) , _dasUnreal_virtual_321_ExportText_Internal , SimNode_ExtFuncCall >(lib,"ExportText_Internal","_dasUnreal_virtual_321_ExportText_Internal")
		->args({"self","ValueStr","PropertyValueOrContainer","PropertyPointerType","DefaultValue","Parent","PortFlags","ExportRootScope"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1525:15
	makeExtern< bool (*)(const FNumericProperty &) , _dasUnreal_virtual_322_IsFloatingPoint , SimNode_ExtFuncCall >(lib,"IsFloatingPoint","_dasUnreal_virtual_322_IsFloatingPoint")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1528:15
	makeExtern< bool (*)(const FNumericProperty &) , _dasUnreal_virtual_323_IsInteger , SimNode_ExtFuncCall >(lib,"IsInteger","_dasUnreal_virtual_323_IsInteger")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_917 = das::das_call_member< bool (FNumericProperty::*)() const,&FNumericProperty::IsEnum >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1549:19
	makeExtern<DAS_CALL_METHOD(_method_917), SimNode_ExtFuncCall >(lib,"IsEnum","das_call_member< bool (FNumericProperty::*)() const , &FNumericProperty::IsEnum >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1555:17
	makeExtern< UEnum * (*)(const FNumericProperty &) , _dasUnreal_virtual_324_GetIntPropertyEnum , SimNode_ExtFuncCall >(lib,"GetIntPropertyEnum","_dasUnreal_virtual_324_GetIntPropertyEnum")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1562:15
	makeExtern< void (*)(const FNumericProperty &,void *,uint64) , _dasUnreal_virtual_325_SetIntPropertyValue , SimNode_ExtFuncCall >(lib,"SetIntPropertyValue","_dasUnreal_virtual_325_SetIntPropertyValue")
		->args({"self","Data","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1569:15
	makeExtern< void (*)(const FNumericProperty &,void *,int64) , _dasUnreal_virtual_326_SetIntPropertyValue , SimNode_ExtFuncCall >(lib,"SetIntPropertyValue","_dasUnreal_virtual_326_SetIntPropertyValue")
		->args({"self","Data","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1576:15
	makeExtern< void (*)(const FNumericProperty &,void *,double) , _dasUnreal_virtual_327_SetFloatingPointPropertyValue , SimNode_ExtFuncCall >(lib,"SetFloatingPointPropertyValue","_dasUnreal_virtual_327_SetFloatingPointPropertyValue")
		->args({"self","Data","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1584:15
	makeExtern< void (*)(const FNumericProperty &,void *,const wchar_t *) , _dasUnreal_virtual_328_SetNumericPropertyValueFromString , SimNode_ExtFuncCall >(lib,"SetNumericPropertyValueFromString","_dasUnreal_virtual_328_SetNumericPropertyValueFromString")
		->args({"self","Data","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1585:15
	makeExtern< void (*)(const FNumericProperty &,void *,const wchar_t *) , _dasUnreal_virtual_329_SetNumericPropertyValueFromString_InContainer , SimNode_ExtFuncCall >(lib,"SetNumericPropertyValueFromString_InContainer","_dasUnreal_virtual_329_SetNumericPropertyValueFromString_InContainer")
		->args({"self","Container","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1592:16
	makeExtern< int64 (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_330_GetSignedIntPropertyValue , SimNode_ExtFuncCall >(lib,"GetSignedIntPropertyValue","_dasUnreal_virtual_330_GetSignedIntPropertyValue")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1593:16
	makeExtern< int64 (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_331_GetSignedIntPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"GetSignedIntPropertyValue_InContainer","_dasUnreal_virtual_331_GetSignedIntPropertyValue_InContainer")
		->args({"self","Container"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1600:17
	makeExtern< uint64 (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_332_GetUnsignedIntPropertyValue , SimNode_ExtFuncCall >(lib,"GetUnsignedIntPropertyValue","_dasUnreal_virtual_332_GetUnsignedIntPropertyValue")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1601:17
	makeExtern< uint64 (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_333_GetUnsignedIntPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"GetUnsignedIntPropertyValue_InContainer","_dasUnreal_virtual_333_GetUnsignedIntPropertyValue_InContainer")
		->args({"self","Container"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1608:17
	makeExtern< double (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_334_GetFloatingPointPropertyValue , SimNode_ExtFuncCall >(lib,"GetFloatingPointPropertyValue","_dasUnreal_virtual_334_GetFloatingPointPropertyValue")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1616:18
	makeExtern< FString (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_335_GetNumericPropertyValueToString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNumericPropertyValueToString","_dasUnreal_virtual_335_GetNumericPropertyValueToString")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1617:18
	makeExtern< FString (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_336_GetNumericPropertyValueToString_InContainer , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNumericPropertyValueToString_InContainer","_dasUnreal_virtual_336_GetNumericPropertyValueToString_InContainer")
		->args({"self","Container"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

