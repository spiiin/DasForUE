// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_12() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:228:31
	makeExtern< FName (*)(const FProperty *,UStruct *,FName) , _dasUnreal_static_338_FindRedirectedPropertyName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FindRedirectedPropertyName","_dasUnreal_static_338_FindRedirectedPropertyName")
		->args({"self","ObjectStruct","OldName"})
		->arg_type(0,makeType<TExplicit<const FProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:233:34
	makeExtern< FString (*)(const FProperty &,FString &) , _dasUnreal_virtual_339_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_339_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:234:31
	makeExtern< bool (*)(const FProperty &) , _dasUnreal_virtual_340_PassCPPArgsByRef , SimNode_ExtFuncCall >(lib,"PassCPPArgsByRef","_dasUnreal_virtual_340_PassCPPArgsByRef")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_928 = das::das_call_member< FString (FProperty::*)() const,&FProperty::GetNameCPP >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:242:26
	makeExtern<DAS_CALL_METHOD(_method_928), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameCPP","das_call_member< FString (FProperty::*)() const , &FProperty::GetNameCPP >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:250:34
	makeExtern< FString (*)(const FProperty &,FString *,unsigned int) , _dasUnreal_virtual_341_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_341_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->arg_init(2,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:252:34
	makeExtern< FString (*)(const FProperty &) , _dasUnreal_virtual_342_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_342_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_931 = das::das_call_member< UPropertyWrapper * (FProperty::*)(),&FProperty::GetUPropertyWrapper >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:257:36
	makeExtern<DAS_CALL_METHOD(_method_931), SimNode_ExtFuncCall >(lib,"GetUPropertyWrapper","das_call_member< UPropertyWrapper * (FProperty::*)() , &FProperty::GetUPropertyWrapper >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:261:15
	makeExtern< bool (*)(const FProperty &) , _dasUnreal_virtual_343_HasSetter , SimNode_ExtFuncCall >(lib,"HasSetter","_dasUnreal_virtual_343_HasSetter")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:266:15
	makeExtern< bool (*)(const FProperty &) , _dasUnreal_virtual_344_HasGetter , SimNode_ExtFuncCall >(lib,"HasGetter","_dasUnreal_virtual_344_HasGetter")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:271:15
	makeExtern< bool (*)(const FProperty &) , _dasUnreal_virtual_345_HasSetterOrGetter , SimNode_ExtFuncCall >(lib,"HasSetterOrGetter","_dasUnreal_virtual_345_HasSetterOrGetter")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:280:15
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_346_CallSetter , SimNode_ExtFuncCall >(lib,"CallSetter","_dasUnreal_virtual_346_CallSetter")
		->args({"self","Container","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:289:15
	makeExtern< void (*)(const FProperty &,const void *,void *) , _dasUnreal_virtual_347_CallGetter , SimNode_ExtFuncCall >(lib,"CallGetter","_dasUnreal_virtual_347_CallGetter")
		->args({"self","Container","OutValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_937 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetOffset_ForDebug >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:314:20
	makeExtern<DAS_CALL_METHOD(_method_937), SimNode_ExtFuncCall >(lib,"GetOffset_ForDebug","das_call_member< int (FProperty::*)() const , &FProperty::GetOffset_ForDebug >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_938 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetOffset_ForUFunction >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:319:20
	makeExtern<DAS_CALL_METHOD(_method_938), SimNode_ExtFuncCall >(lib,"GetOffset_ForUFunction","das_call_member< int (FProperty::*)() const , &FProperty::GetOffset_ForUFunction >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_939 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetOffset_ForGC >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:324:20
	makeExtern<DAS_CALL_METHOD(_method_939), SimNode_ExtFuncCall >(lib,"GetOffset_ForGC","das_call_member< int (FProperty::*)() const , &FProperty::GetOffset_ForGC >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_940 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetOffset_ForInternal >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:329:20
	makeExtern<DAS_CALL_METHOD(_method_940), SimNode_ExtFuncCall >(lib,"GetOffset_ForInternal","das_call_member< int (FProperty::*)() const , &FProperty::GetOffset_ForInternal >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_941 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetOffset_ReplaceWith_ContainerPtrToValuePtr >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:334:20
	makeExtern<DAS_CALL_METHOD(_method_941), SimNode_ExtFuncCall >(lib,"GetOffset_ReplaceWith_ContainerPtrToValuePtr","das_call_member< int (FProperty::*)() const , &FProperty::GetOffset_ReplaceWith_ContainerPtrToValuePtr >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_942 = das::das_call_member< void (FProperty::*)(FArchive &),&FProperty::LinkWithoutChangingOffset >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:339:7
	makeExtern<DAS_CALL_METHOD(_method_942), SimNode_ExtFuncCall >(lib,"LinkWithoutChangingOffset","das_call_member< void (FProperty::*)(FArchive &) , &FProperty::LinkWithoutChangingOffset >::invoke")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_943 = das::das_call_member< int (FProperty::*)(FArchive &),&FProperty::Link >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:344:8
	makeExtern<DAS_CALL_METHOD(_method_943), SimNode_ExtFuncCall >(lib,"Link","das_call_member< int (FProperty::*)(FArchive &) , &FProperty::Link >::invoke")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:379:31
	makeExtern< bool (*)(const FProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_348_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_348_Identical")
		->args({"self","A","B","PortFlags"})
		->arg_init(3,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_945 = das::das_call_member< bool (FProperty::*)(const void *,const void *,int,unsigned int) const,&FProperty::Identical_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:390:7
	makeExtern<DAS_CALL_METHOD(_method_945), SimNode_ExtFuncCall >(lib,"Identical_InContainer","das_call_member< bool (FProperty::*)(const void *,const void *,int,unsigned int) const , &FProperty::Identical_InContainer >::invoke")
		->args({"self","A","B","ArrayIndex","PortFlags"})
		->arg_init(3,make_smart<ExprConstInt>(0))
		->arg_init(4,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:447:31
	makeExtern< bool (*)(const FProperty &) , _dasUnreal_virtual_349_SupportsNetSharedSerialization , SimNode_ExtFuncCall >(lib,"SupportsNetSharedSerialization","_dasUnreal_virtual_349_SupportsNetSharedSerialization")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_947 = das::das_call_member< void (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const,&FProperty::ExportTextItem_Direct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:455:7
	makeExtern<DAS_CALL_METHOD(_method_947), SimNode_ExtFuncCall >(lib,"ExportTextItem_Direct","das_call_member< void (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const , &FProperty::ExportTextItem_Direct >::invoke")
		->args({"self","ValueStr","PropertyValue","DefaultValue","Parent","PortFlags","ExportRootScope"})
		->arg_init(6,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_948 = das::das_call_member< void (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const,&FProperty::ExportTextItem_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:460:7
	makeExtern<DAS_CALL_METHOD(_method_948), SimNode_ExtFuncCall >(lib,"ExportTextItem_InContainer","das_call_member< void (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const , &FProperty::ExportTextItem_InContainer >::invoke")
		->args({"self","ValueStr","Container","DefaultValue","Parent","PortFlags","ExportRootScope"})
		->arg_init(6,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_949 = das::das_call_member< const wchar_t * (FProperty::*)(const wchar_t *,void *,UObject *,int,FOutputDevice *) const,&FProperty::ImportText_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:480:15
	makeExtern<DAS_CALL_METHOD(_method_949), SimNode_ExtFuncCall >(lib,"ImportText_InContainer","das_call_member< const wchar_t * (FProperty::*)(const wchar_t *,void *,UObject *,int,FOutputDevice *) const , &FProperty::ImportText_InContainer >::invoke")
		->args({"self","Buffer","Container","OwnerObject","PortFlags","ErrorText"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_950 = das::das_call_member< const wchar_t * (FProperty::*)(const wchar_t *,void *,UObject *,int,FOutputDevice *) const,&FProperty::ImportText_Direct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:499:15
	makeExtern<DAS_CALL_METHOD(_method_950), SimNode_ExtFuncCall >(lib,"ImportText_Direct","das_call_member< const wchar_t * (FProperty::*)(const wchar_t *,void *,UObject *,int,FOutputDevice *) const , &FProperty::ImportText_Direct >::invoke")
		->args({"self","Buffer","PropertyPtr","OwnerObject","PortFlags","ErrorText"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_951 = das::das_call_member< void (FProperty::*)(void *,const void *) const,&FProperty::SetValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:509:19
	makeExtern<DAS_CALL_METHOD(_method_951), SimNode_ExtFuncCall >(lib,"SetValue_InContainer","das_call_member< void (FProperty::*)(void *,const void *) const , &FProperty::SetValue_InContainer >::invoke")
		->args({"self","OutContainer","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_952 = das::das_call_member< void (FProperty::*)(const void *,void *) const,&FProperty::GetValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:520:19
	makeExtern<DAS_CALL_METHOD(_method_952), SimNode_ExtFuncCall >(lib,"GetValue_InContainer","das_call_member< void (FProperty::*)(const void *,void *) const , &FProperty::GetValue_InContainer >::invoke")
		->args({"self","InContainer","OutValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_953 = das::das_call_member< void (FProperty::*)(void *,const void *,int) const,&FProperty::SetSingleValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:538:23
	makeExtern<DAS_CALL_METHOD(_method_953), SimNode_ExtFuncCall >(lib,"SetSingleValue_InContainer","das_call_member< void (FProperty::*)(void *,const void *,int) const , &FProperty::SetSingleValue_InContainer >::invoke")
		->args({"self","OutContainer","InValue","ArrayIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_954 = das::das_call_member< void (FProperty::*)(const void *,void *,int) const,&FProperty::GetSingleValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:546:23
	makeExtern<DAS_CALL_METHOD(_method_954), SimNode_ExtFuncCall >(lib,"GetSingleValue_InContainer","das_call_member< void (FProperty::*)(const void *,void *,int) const , &FProperty::GetSingleValue_InContainer >::invoke")
		->args({"self","InContainer","OutValue","ArrayIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_955 = das::das_call_member< void * (FProperty::*)() const,&FProperty::AllocateAndInitializeValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:549:24
	makeExtern<DAS_CALL_METHOD(_method_955), SimNode_ExtFuncCall >(lib,"AllocateAndInitializeValue","das_call_member< void * (FProperty::*)() const , &FProperty::AllocateAndInitializeValue >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_956 = das::das_call_member< void (FProperty::*)(void *) const,&FProperty::DestroyAndFreeValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:552:23
	makeExtern<DAS_CALL_METHOD(_method_956), SimNode_ExtFuncCall >(lib,"DestroyAndFreeValue","das_call_member< void (FProperty::*)(void *) const , &FProperty::DestroyAndFreeValue >::invoke")
		->args({"self","InMemory"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:576:32
	makeExtern< void * (*)(const FProperty &,const FProperty *,void *,int) , _dasUnreal_virtual_350_GetValueAddressAtIndex_Direct , SimNode_ExtFuncCall >(lib,"GetValueAddressAtIndex_Direct","_dasUnreal_virtual_350_GetValueAddressAtIndex_Direct")
		->args({"self","Inner","InValueAddress","Index"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:590:31
	makeExtern< void (*)(const FProperty &,FBlake3 &,bool) , _dasUnreal_virtual_351_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_351_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_959 = das::das_call_member< bool (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const,&FProperty::ExportText_Direct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:599:23
	makeExtern<DAS_CALL_METHOD(_method_959), SimNode_ExtFuncCall >(lib,"ExportText_Direct","das_call_member< bool (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const , &FProperty::ExportText_Direct >::invoke")
		->args({"self","ValueStr","Data","Delta","Parent","PortFlags","ExportRootScope"})
		->arg_init(6,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_960 = das::das_call_member< bool (FProperty::*)(int,FString &,const void *,const void *,UObject *,int,UObject *) const,&FProperty::ExportText_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:600:19
	makeExtern<DAS_CALL_METHOD(_method_960), SimNode_ExtFuncCall >(lib,"ExportText_InContainer","das_call_member< bool (FProperty::*)(int,FString &,const void *,const void *,UObject *,int,UObject *) const , &FProperty::ExportText_InContainer >::invoke")
		->args({"self","Index","ValueStr","Data","Delta","Parent","PortFlags","ExportRootScope"})
		->arg_init(7,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_961 = das::das_call_member< bool (FProperty::*)(int) const,&FProperty::IsInContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:731:19
	makeExtern<DAS_CALL_METHOD(_method_961), SimNode_ExtFuncCall >(lib,"IsInContainer","das_call_member< bool (FProperty::*)(int) const , &FProperty::IsInContainer >::invoke")
		->args({"self","ContainerSize"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_962 = das::das_call_member< bool (FProperty::*)(UStruct *) const,&FProperty::IsInContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:736:19
	makeExtern<DAS_CALL_METHOD(_method_962), SimNode_ExtFuncCall >(lib,"IsInContainer","das_call_member< bool (FProperty::*)(UStruct *) const , &FProperty::IsInContainer >::invoke")
		->args({"self","ContainerClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_963 = das::das_call_member< void (FProperty::*)(void *,const void *) const,&FProperty::CopySingleValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:752:19
	makeExtern<DAS_CALL_METHOD(_method_963), SimNode_ExtFuncCall >(lib,"CopySingleValue","das_call_member< void (FProperty::*)(void *,const void *) const , &FProperty::CopySingleValue >::invoke")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_964 = das::das_call_member< unsigned int (FProperty::*)(const void *) const,&FProperty::GetValueTypeHash >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:770:25
	makeExtern<DAS_CALL_METHOD(_method_964), SimNode_ExtFuncCall >(lib,"GetValueTypeHash","das_call_member< unsigned int (FProperty::*)(const void *) const , &FProperty::GetValueTypeHash >::invoke")
		->args({"self","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_965 = das::das_call_member< void (FProperty::*)(void *,const void *) const,&FProperty::CopyCompleteValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:786:19
	makeExtern<DAS_CALL_METHOD(_method_965), SimNode_ExtFuncCall >(lib,"CopyCompleteValue","das_call_member< void (FProperty::*)(void *,const void *) const , &FProperty::CopyCompleteValue >::invoke")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_966 = das::das_call_member< void (FProperty::*)(void *,const void *) const,&FProperty::CopyCompleteValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:800:19
	makeExtern<DAS_CALL_METHOD(_method_966), SimNode_ExtFuncCall >(lib,"CopyCompleteValue_InContainer","das_call_member< void (FProperty::*)(void *,const void *) const , &FProperty::CopyCompleteValue_InContainer >::invoke")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:815:31
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_352_CopySingleValueToScriptVM , SimNode_ExtFuncCall >(lib,"CopySingleValueToScriptVM","_dasUnreal_virtual_352_CopySingleValueToScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:825:31
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_353_CopyCompleteValueToScriptVM , SimNode_ExtFuncCall >(lib,"CopyCompleteValueToScriptVM","_dasUnreal_virtual_353_CopyCompleteValueToScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:830:31
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_354_CopyCompleteValueToScriptVM_InContainer , SimNode_ExtFuncCall >(lib,"CopyCompleteValueToScriptVM_InContainer","_dasUnreal_virtual_354_CopyCompleteValueToScriptVM_InContainer")
		->args({"self","OutValue","InContainer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:831:31
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_355_CopyCompleteValueFromScriptVM_InContainer , SimNode_ExtFuncCall >(lib,"CopyCompleteValueFromScriptVM_InContainer","_dasUnreal_virtual_355_CopyCompleteValueFromScriptVM_InContainer")
		->args({"self","OutContainer","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:843:31
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_356_CopySingleValueFromScriptVM , SimNode_ExtFuncCall >(lib,"CopySingleValueFromScriptVM","_dasUnreal_virtual_356_CopySingleValueFromScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:853:31
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_357_CopyCompleteValueFromScriptVM , SimNode_ExtFuncCall >(lib,"CopyCompleteValueFromScriptVM","_dasUnreal_virtual_357_CopyCompleteValueFromScriptVM")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_973 = das::das_call_member< void (FProperty::*)(void *) const,&FProperty::ClearValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:861:19
	makeExtern<DAS_CALL_METHOD(_method_973), SimNode_ExtFuncCall >(lib,"ClearValue","das_call_member< void (FProperty::*)(void *) const , &FProperty::ClearValue >::invoke")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_974 = das::das_call_member< void (FProperty::*)(void *,int) const,&FProperty::ClearValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:878:19
	makeExtern<DAS_CALL_METHOD(_method_974), SimNode_ExtFuncCall >(lib,"ClearValue_InContainer","das_call_member< void (FProperty::*)(void *,int) const , &FProperty::ClearValue_InContainer >::invoke")
		->args({"self","Data","ArrayIndex"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_975 = das::das_call_member< void (FProperty::*)(void *) const,&FProperty::DestroyValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:898:19
	makeExtern<DAS_CALL_METHOD(_method_975), SimNode_ExtFuncCall >(lib,"DestroyValue","das_call_member< void (FProperty::*)(void *) const , &FProperty::DestroyValue >::invoke")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_976 = das::das_call_member< void (FProperty::*)(void *) const,&FProperty::DestroyValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:911:19
	makeExtern<DAS_CALL_METHOD(_method_976), SimNode_ExtFuncCall >(lib,"DestroyValue_InContainer","das_call_member< void (FProperty::*)(void *) const , &FProperty::DestroyValue_InContainer >::invoke")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_977 = das::das_call_member< void (FProperty::*)(void *) const,&FProperty::InitializeValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:929:19
	makeExtern<DAS_CALL_METHOD(_method_977), SimNode_ExtFuncCall >(lib,"InitializeValue","das_call_member< void (FProperty::*)(void *) const , &FProperty::InitializeValue >::invoke")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_978 = das::das_call_member< void (FProperty::*)(void *) const,&FProperty::InitializeValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:947:19
	makeExtern<DAS_CALL_METHOD(_method_978), SimNode_ExtFuncCall >(lib,"InitializeValue_InContainer","das_call_member< void (FProperty::*)(void *) const , &FProperty::InitializeValue_InContainer >::invoke")
		->args({"self","Dest"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_979 = das::das_call_member< bool (FProperty::*)(unsigned int,FOutputDevice *) const,&FProperty::ValidateImportFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:970:23
	makeExtern<DAS_CALL_METHOD(_method_979), SimNode_ExtFuncCall >(lib,"ValidateImportFlags","das_call_member< bool (FProperty::*)(unsigned int,FOutputDevice *) const , &FProperty::ValidateImportFlags >::invoke")
		->args({"self","PortFlags","ErrorText"})
		->arg_init(2,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_980 = das::das_call_member< bool (FProperty::*)(unsigned int) const,&FProperty::ShouldPort >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:971:23
	makeExtern<DAS_CALL_METHOD(_method_980), SimNode_ExtFuncCall >(lib,"ShouldPort","das_call_member< bool (FProperty::*)(unsigned int) const , &FProperty::ShouldPort >::invoke")
		->args({"self","PortFlags"})
		->arg_init(1,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:972:32
	makeExtern< FName (*)(const FProperty &) , _dasUnreal_virtual_358_GetID , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetID","_dasUnreal_virtual_358_GetID")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:984:32
	makeExtern< int (*)(const FProperty &) , _dasUnreal_virtual_359_GetMinAlignment , SimNode_ExtFuncCall >(lib,"GetMinAlignment","_dasUnreal_virtual_359_GetMinAlignment")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_983 = das::das_call_member< bool (FProperty::*)() const,&FProperty::ContainsWeakObjectReference >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1002:7
	makeExtern<DAS_CALL_METHOD(_method_983), SimNode_ExtFuncCall >(lib,"ContainsWeakObjectReference","das_call_member< bool (FProperty::*)() const , &FProperty::ContainsWeakObjectReference >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_984 = das::das_call_member< bool (FProperty::*)() const,&FProperty::ContainsInstancedObjectProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1014:19
	makeExtern<DAS_CALL_METHOD(_method_984), SimNode_ExtFuncCall >(lib,"ContainsInstancedObjectProperty","das_call_member< bool (FProperty::*)() const , &FProperty::ContainsInstancedObjectProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_985 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1027:20
	makeExtern<DAS_CALL_METHOD(_method_985), SimNode_ExtFuncCall >(lib,"GetSize","das_call_member< int (FProperty::*)() const , &FProperty::GetSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_986 = das::das_call_member< bool (FProperty::*)(FArchive &) const,&FProperty::ShouldSerializeValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1031:23
	makeExtern<DAS_CALL_METHOD(_method_986), SimNode_ExtFuncCall >(lib,"ShouldSerializeValue","das_call_member< bool (FProperty::*)(FArchive &) const , &FProperty::ShouldSerializeValue >::invoke")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_987 = das::das_call_member< bool (FProperty::*)() const,&FProperty::ShouldDuplicateValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1038:7
	makeExtern<DAS_CALL_METHOD(_method_987), SimNode_ExtFuncCall >(lib,"ShouldDuplicateValue","das_call_member< bool (FProperty::*)() const , &FProperty::ShouldDuplicateValue >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_988 = das::das_call_member< FProperty * (FProperty::*)(),&FProperty::GetOwnerProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1046:13
	makeExtern<DAS_CALL_METHOD(_method_988), SimNode_ExtFuncCall >(lib,"GetOwnerProperty","das_call_member< FProperty * (FProperty::*)() , &FProperty::GetOwnerProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_989 = das::das_call_member< const FProperty * (FProperty::*)() const,&FProperty::GetOwnerProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1056:19
	makeExtern<DAS_CALL_METHOD(_method_989), SimNode_ExtFuncCall >(lib,"GetOwnerProperty","das_call_member< const FProperty * (FProperty::*)() const , &FProperty::GetOwnerProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_990 = das::das_call_member< EPropertyFlags (FProperty::*)() const,&FProperty::GetPropertyFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1069:29
	makeExtern<DAS_CALL_METHOD(_method_990), SimNode_ExtFuncCall >(lib,"GetPropertyFlags","das_call_member< EPropertyFlags (FProperty::*)() const , &FProperty::GetPropertyFlags >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_991 = das::das_call_member< void (FProperty::*)(EPropertyFlags),&FProperty::SetPropertyFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1073:19
	makeExtern<DAS_CALL_METHOD(_method_991), SimNode_ExtFuncCall >(lib,"SetPropertyFlags","das_call_member< void (FProperty::*)(EPropertyFlags) , &FProperty::SetPropertyFlags >::invoke")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_992 = das::das_call_member< void (FProperty::*)(EPropertyFlags),&FProperty::ClearPropertyFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1077:19
	makeExtern<DAS_CALL_METHOD(_method_992), SimNode_ExtFuncCall >(lib,"ClearPropertyFlags","das_call_member< void (FProperty::*)(EPropertyFlags) , &FProperty::ClearPropertyFlags >::invoke")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_993 = das::das_call_member< bool (FProperty::*)(uint64) const,&FProperty::HasAnyPropertyFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1091:19
	makeExtern<DAS_CALL_METHOD(_method_993), SimNode_ExtFuncCall >(lib,"HasAnyPropertyFlags","das_call_member< bool (FProperty::*)(uint64) const , &FProperty::HasAnyPropertyFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_994 = das::das_call_member< bool (FProperty::*)(uint64) const,&FProperty::HasAllPropertyFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1105:19
	makeExtern<DAS_CALL_METHOD(_method_994), SimNode_ExtFuncCall >(lib,"HasAllPropertyFlags","das_call_member< bool (FProperty::*)(uint64) const , &FProperty::HasAllPropertyFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_995 = das::das_call_member< FProperty * (FProperty::*)(),&FProperty::GetRepOwner >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1114:25
	makeExtern<DAS_CALL_METHOD(_method_995), SimNode_ExtFuncCall >(lib,"GetRepOwner","das_call_member< FProperty * (FProperty::*)() , &FProperty::GetRepOwner >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_996 = das::das_call_member< bool (FProperty::*)() const,&FProperty::IsEditorOnlyProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1126:19
	makeExtern<DAS_CALL_METHOD(_method_996), SimNode_ExtFuncCall >(lib,"IsEditorOnlyProperty","das_call_member< bool (FProperty::*)() const , &FProperty::IsEditorOnlyProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1132:31
	makeExtern< bool (*)(const FProperty &,const FProperty *) , _dasUnreal_virtual_360_SameType , SimNode_ExtFuncCall >(lib,"SameType","_dasUnreal_virtual_360_SameType")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_998 = das::das_call_member< ELifetimeCondition (FProperty::*)() const,&FProperty::GetBlueprintReplicationCondition >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1134:21
	makeExtern<DAS_CALL_METHOD(_method_998), SimNode_ExtFuncCall >(lib,"GetBlueprintReplicationCondition","das_call_member< ELifetimeCondition (FProperty::*)() const , &FProperty::GetBlueprintReplicationCondition >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_999 = das::das_call_member< void (FProperty::*)(ELifetimeCondition),&FProperty::SetBlueprintReplicationCondition >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1135:7
	makeExtern<DAS_CALL_METHOD(_method_999), SimNode_ExtFuncCall >(lib,"SetBlueprintReplicationCondition","das_call_member< void (FProperty::*)(ELifetimeCondition) , &FProperty::SetBlueprintReplicationCondition >::invoke")
		->args({"self","InBlueprintReplicationCondition"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FNumericProperty,EInternal,FFieldClass *>(*this,lib,"FNumericProperty","FNumericProperty")
		->args({"InInernal","InClass"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1488:2
	makeExtern< FFieldClass * (*)(const FNumericProperty *) , _dasUnreal_static_361_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_361_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FNumericProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1488:2
	makeExtern< FField * (*)(const FNumericProperty *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_362_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_362_Construct")
		->args({"self","InOwner","InName","InObjectFlags"})
		->arg_type(0,makeType<TExplicit<const FNumericProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1488:2
	makeExtern< uint64 (*)(const FNumericProperty *) , _dasUnreal_static_363_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_363_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FNumericProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1488:2
	makeExtern< uint64 (*)(const FNumericProperty *) , _dasUnreal_static_364_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_364_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FNumericProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FNumericProperty,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FNumericProperty","FNumericProperty")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FNumericProperty,UField *>(*this,lib,"FNumericProperty","FNumericProperty")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1518:23
	makeExtern< const wchar_t * (*)(const FNumericProperty &,const wchar_t *,void *,EPropertyPointerType,UObject *,int,FOutputDevice *) , _dasUnreal_virtual_365_ImportText_Internal , SimNode_ExtFuncCall >(lib,"ImportText_Internal","_dasUnreal_virtual_365_ImportText_Internal")
		->args({"self","Buffer","ContainerOrPropertyPtr","PropertyPointerType","Parent","PortFlags","ErrorText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1519:15
	makeExtern< void (*)(const FNumericProperty &,FString &,const void *,EPropertyPointerType,const void *,UObject *,int,UObject *) , _dasUnreal_virtual_366_ExportText_Internal , SimNode_ExtFuncCall >(lib,"ExportText_Internal","_dasUnreal_virtual_366_ExportText_Internal")
		->args({"self","ValueStr","PropertyValueOrContainer","PropertyPointerType","DefaultValue","Parent","PortFlags","ExportRootScope"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1525:15
	makeExtern< bool (*)(const FNumericProperty &) , _dasUnreal_virtual_367_IsFloatingPoint , SimNode_ExtFuncCall >(lib,"IsFloatingPoint","_dasUnreal_virtual_367_IsFloatingPoint")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1528:15
	makeExtern< bool (*)(const FNumericProperty &) , _dasUnreal_virtual_368_IsInteger , SimNode_ExtFuncCall >(lib,"IsInteger","_dasUnreal_virtual_368_IsInteger")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1008 = das::das_call_member< bool (FNumericProperty::*)() const,&FNumericProperty::IsEnum >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1549:19
	makeExtern<DAS_CALL_METHOD(_method_1008), SimNode_ExtFuncCall >(lib,"IsEnum","das_call_member< bool (FNumericProperty::*)() const , &FNumericProperty::IsEnum >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1555:17
	makeExtern< UEnum * (*)(const FNumericProperty &) , _dasUnreal_virtual_369_GetIntPropertyEnum , SimNode_ExtFuncCall >(lib,"GetIntPropertyEnum","_dasUnreal_virtual_369_GetIntPropertyEnum")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1562:15
	makeExtern< void (*)(const FNumericProperty &,void *,uint64) , _dasUnreal_virtual_370_SetIntPropertyValue , SimNode_ExtFuncCall >(lib,"SetIntPropertyValue","_dasUnreal_virtual_370_SetIntPropertyValue")
		->args({"self","Data","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1569:15
	makeExtern< void (*)(const FNumericProperty &,void *,int64) , _dasUnreal_virtual_371_SetIntPropertyValue , SimNode_ExtFuncCall >(lib,"SetIntPropertyValue","_dasUnreal_virtual_371_SetIntPropertyValue")
		->args({"self","Data","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1576:15
	makeExtern< void (*)(const FNumericProperty &,void *,double) , _dasUnreal_virtual_372_SetFloatingPointPropertyValue , SimNode_ExtFuncCall >(lib,"SetFloatingPointPropertyValue","_dasUnreal_virtual_372_SetFloatingPointPropertyValue")
		->args({"self","Data","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1584:15
	makeExtern< void (*)(const FNumericProperty &,void *,const wchar_t *) , _dasUnreal_virtual_373_SetNumericPropertyValueFromString , SimNode_ExtFuncCall >(lib,"SetNumericPropertyValueFromString","_dasUnreal_virtual_373_SetNumericPropertyValueFromString")
		->args({"self","Data","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1585:15
	makeExtern< void (*)(const FNumericProperty &,void *,const wchar_t *) , _dasUnreal_virtual_374_SetNumericPropertyValueFromString_InContainer , SimNode_ExtFuncCall >(lib,"SetNumericPropertyValueFromString_InContainer","_dasUnreal_virtual_374_SetNumericPropertyValueFromString_InContainer")
		->args({"self","Container","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1592:16
	makeExtern< int64 (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_375_GetSignedIntPropertyValue , SimNode_ExtFuncCall >(lib,"GetSignedIntPropertyValue","_dasUnreal_virtual_375_GetSignedIntPropertyValue")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1593:16
	makeExtern< int64 (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_376_GetSignedIntPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"GetSignedIntPropertyValue_InContainer","_dasUnreal_virtual_376_GetSignedIntPropertyValue_InContainer")
		->args({"self","Container"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1600:17
	makeExtern< uint64 (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_377_GetUnsignedIntPropertyValue , SimNode_ExtFuncCall >(lib,"GetUnsignedIntPropertyValue","_dasUnreal_virtual_377_GetUnsignedIntPropertyValue")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1601:17
	makeExtern< uint64 (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_378_GetUnsignedIntPropertyValue_InContainer , SimNode_ExtFuncCall >(lib,"GetUnsignedIntPropertyValue_InContainer","_dasUnreal_virtual_378_GetUnsignedIntPropertyValue_InContainer")
		->args({"self","Container"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1608:17
	makeExtern< double (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_379_GetFloatingPointPropertyValue , SimNode_ExtFuncCall >(lib,"GetFloatingPointPropertyValue","_dasUnreal_virtual_379_GetFloatingPointPropertyValue")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1616:18
	makeExtern< FString (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_380_GetNumericPropertyValueToString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNumericPropertyValueToString","_dasUnreal_virtual_380_GetNumericPropertyValueToString")
		->args({"self","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:1617:18
	makeExtern< FString (*)(const FNumericProperty &,const void *) , _dasUnreal_virtual_381_GetNumericPropertyValueToString_InContainer , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNumericPropertyValueToString_InContainer","_dasUnreal_virtual_381_GetNumericPropertyValueToString_InContainer")
		->args({"self","Container"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

