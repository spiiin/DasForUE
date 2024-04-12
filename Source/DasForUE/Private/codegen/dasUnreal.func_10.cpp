// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_10() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1566:31
	makeExtern< void (*)(UScriptStruct &,FArchive &) , _dasUnreal_virtual_251_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_251_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1570:31
	makeExtern< void (*)(UScriptStruct &,FArchive &,bool) , _dasUnreal_virtual_252_Link , SimNode_ExtFuncCall >(lib,"Link","_dasUnreal_virtual_252_Link")
		->args({"self","Ar","bRelinkExistingProperties"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1571:31
	makeExtern< void (*)(const UScriptStruct &,void *,int) , _dasUnreal_virtual_253_InitializeStruct , SimNode_ExtFuncCall >(lib,"InitializeStruct","_dasUnreal_virtual_253_InitializeStruct")
		->args({"self","Dest","ArrayDim"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1572:31
	makeExtern< void (*)(const UScriptStruct &,void *,int) , _dasUnreal_virtual_254_DestroyStruct , SimNode_ExtFuncCall >(lib,"DestroyStruct","_dasUnreal_virtual_254_DestroyStruct")
		->args({"self","Dest","ArrayDim"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1573:31
	makeExtern< bool (*)(const UScriptStruct &) , _dasUnreal_virtual_255_IsStructTrashed , SimNode_ExtFuncCall >(lib,"IsStructTrashed","_dasUnreal_virtual_255_IsStructTrashed")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_730 = das::das_call_member< void (UScriptStruct::*)(bool),&UScriptStruct::SetStructTrashed >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1577:23
	makeExtern<DAS_CALL_METHOD(_method_730), SimNode_ExtFuncCall >(lib,"SetStructTrashed","das_call_member< void (UScriptStruct::*)(bool) , &UScriptStruct::SetStructTrashed >::invoke")
		->args({"self","bIsTrash"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1600:31
	makeExtern< void (*)(UScriptStruct &) , _dasUnreal_virtual_256_PrepareCppStructOps , SimNode_ExtFuncCall >(lib,"PrepareCppStructOps","_dasUnreal_virtual_256_PrepareCppStructOps")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_732 = das::das_call_member< UScriptStruct::ICppStructOps * (UScriptStruct::*)() const,&UScriptStruct::GetCppStructOps >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1603:29
	makeExtern<DAS_CALL_METHOD(_method_732), SimNode_ExtFuncCall >(lib,"GetCppStructOps","das_call_member< UScriptStruct::ICppStructOps * (UScriptStruct::*)() const , &UScriptStruct::GetCppStructOps >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_733 = das::das_call_member< void (UScriptStruct::*)(),&UScriptStruct::ClearCppStructOps >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1610:7
	makeExtern<DAS_CALL_METHOD(_method_733), SimNode_ExtFuncCall >(lib,"ClearCppStructOps","das_call_member< void (UScriptStruct::*)() , &UScriptStruct::ClearCppStructOps >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_734 = das::das_call_member< bool (UScriptStruct::*)() const,&UScriptStruct::HasDefaults >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1621:19
	makeExtern<DAS_CALL_METHOD(_method_734), SimNode_ExtFuncCall >(lib,"HasDefaults","das_call_member< bool (UScriptStruct::*)() const , &UScriptStruct::HasDefaults >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_735 = das::das_call_member< bool (UScriptStruct::*)() const,&UScriptStruct::UseNativeSerialization >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1643:7
	makeExtern<DAS_CALL_METHOD(_method_735), SimNode_ExtFuncCall >(lib,"UseNativeSerialization","das_call_member< bool (UScriptStruct::*)() const , &UScriptStruct::UseNativeSerialization >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_736 = das::das_call_member< void (UScriptStruct::*)(FString &,const void *,const void *,UObject *,int,UObject *,bool) const,&UScriptStruct::ExportText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1679:23
	makeExtern<DAS_CALL_METHOD(_method_736), SimNode_ExtFuncCall >(lib,"ExportText","das_call_member< void (UScriptStruct::*)(FString &,const void *,const void *,UObject *,int,UObject *,bool) const , &UScriptStruct::ExportText >::invoke")
		->args({"self","ValueStr","Value","Defaults","OwnerObject","PortFlags","ExportRootScope","bAllowNativeOverride"})
		->arg_init(7,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_737 = das::das_call_member< const wchar_t * (UScriptStruct::*)(const wchar_t *,void *,UObject *,int,FOutputDevice *,const FString &,bool),&UScriptStruct::ImportText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1693:31
	makeExtern<DAS_CALL_METHOD(_method_737), SimNode_ExtFuncCall >(lib,"ImportText","das_call_member< const wchar_t * (UScriptStruct::*)(const wchar_t *,void *,UObject *,int,FOutputDevice *,const FString &,bool) , &UScriptStruct::ImportText >::invoke")
		->args({"self","Buffer","Value","OwnerObject","PortFlags","ErrorText","StructName","bAllowNativeOverride"})
		->arg_init(7,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_738 = das::das_call_member< bool (UScriptStruct::*)(const void *,const void *,unsigned int) const,&UScriptStruct::CompareScriptStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1717:23
	makeExtern<DAS_CALL_METHOD(_method_738), SimNode_ExtFuncCall >(lib,"CompareScriptStruct","das_call_member< bool (UScriptStruct::*)(const void *,const void *,unsigned int) const , &UScriptStruct::CompareScriptStruct >::invoke")
		->args({"self","A","B","PortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_739 = das::das_call_member< void (UScriptStruct::*)(void *,const void *,int) const,&UScriptStruct::CopyScriptStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1727:23
	makeExtern<DAS_CALL_METHOD(_method_739), SimNode_ExtFuncCall >(lib,"CopyScriptStruct","das_call_member< void (UScriptStruct::*)(void *,const void *,int) const , &UScriptStruct::CopyScriptStruct >::invoke")
		->args({"self","Dest","Src","ArrayDim"})
		->arg_init(3,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_740 = das::das_call_member< void (UScriptStruct::*)(void *,int) const,&UScriptStruct::ClearScriptStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1736:23
	makeExtern<DAS_CALL_METHOD(_method_740), SimNode_ExtFuncCall >(lib,"ClearScriptStruct","das_call_member< void (UScriptStruct::*)(void *,int) const , &UScriptStruct::ClearScriptStruct >::invoke")
		->args({"self","Dest","ArrayDim"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1744:33
	makeExtern< unsigned int (*)(const UScriptStruct &,const void *) , _dasUnreal_virtual_257_GetStructTypeHash , SimNode_ExtFuncCall >(lib,"GetStructTypeHash","_dasUnreal_virtual_257_GetStructTypeHash")
		->args({"self","Src"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1747:31
	makeExtern< void (*)(UScriptStruct &) , _dasUnreal_virtual_258_RecursivelyPreload , SimNode_ExtFuncCall >(lib,"RecursivelyPreload","_dasUnreal_virtual_258_RecursivelyPreload")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1753:34
	makeExtern< FString (*)(const UScriptStruct &,unsigned int) , _dasUnreal_virtual_259_GetStructCPPName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetStructCPPName","_dasUnreal_virtual_259_GetStructCPPName")
		->args({"self","CPPExportFlags"})
		->arg_init(1,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1759:31
	makeExtern< void (*)(const UScriptStruct &,unsigned char *) , _dasUnreal_virtual_260_InitializeDefaultValue , SimNode_ExtFuncCall >(lib,"InitializeDefaultValue","_dasUnreal_virtual_260_InitializeDefaultValue")
		->args({"self","InStructData"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1782:2
	makeExtern< UClass * (*)(const UFunction *) , _dasUnreal_static_261_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_261_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UFunction*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1782:2
	makeExtern< const wchar_t * (*)(const UFunction *) , _dasUnreal_static_262_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_262_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UFunction*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1782:2
	makeExtern< void (*)(const UFunction *) , _dasUnreal_static_263_StaticRegisterNativesUFunction , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUFunction","_dasUnreal_static_263_StaticRegisterNativesUFunction")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UFunction*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_748 = das::das_call_member< UClass * (UFunction::*)() const,&UFunction::GetOuterUClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1783:2
	makeExtern<DAS_CALL_METHOD(_method_748), SimNode_ExtFuncCall >(lib,"GetOuterUClass","das_call_member< UClass * (UFunction::*)() const , &UFunction::GetOuterUClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_749 = das::das_call_member< void (UFunction::*)(UObject *,FFrame &,void *const),&UFunction::Invoke >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1851:23
	makeExtern<DAS_CALL_METHOD(_method_749), SimNode_ExtFuncCall >(lib,"Invoke","das_call_member< void (UFunction::*)(UObject *,FFrame &,void *const) , &UFunction::Invoke >::invoke")
		->args({"self","Obj","Stack","Z_Param__Result"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_750 = das::das_call_member< void (UFunction::*)(),&UFunction::InitializeDerivedMembers >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1858:23
	makeExtern<DAS_CALL_METHOD(_method_750), SimNode_ExtFuncCall >(lib,"InitializeDerivedMembers","das_call_member< void (UFunction::*)() , &UFunction::InitializeDerivedMembers >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1861:31
	makeExtern< void (*)(UFunction &,FArchive &) , _dasUnreal_virtual_264_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_264_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1862:31
	makeExtern< void (*)(UFunction &) , _dasUnreal_virtual_265_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_265_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1865:31
	makeExtern< void (*)(UFunction &) , _dasUnreal_virtual_266_Bind , SimNode_ExtFuncCall >(lib,"Bind","_dasUnreal_virtual_266_Bind")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1868:19
	makeExtern< UStruct * (*)(const UFunction &) , _dasUnreal_virtual_267_GetInheritanceSuper , SimNode_ExtFuncCall >(lib,"GetInheritanceSuper","_dasUnreal_virtual_267_GetInheritanceSuper")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1869:31
	makeExtern< void (*)(UFunction &,FArchive &,bool) , _dasUnreal_virtual_268_Link , SimNode_ExtFuncCall >(lib,"Link","_dasUnreal_virtual_268_Link")
		->args({"self","Ar","bRelinkExistingProperties"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_756 = das::das_call_member< UFunction * (UFunction::*)() const,&UFunction::GetSuperFunction >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1872:29
	makeExtern<DAS_CALL_METHOD(_method_756), SimNode_ExtFuncCall >(lib,"GetSuperFunction","das_call_member< UFunction * (UFunction::*)() const , &UFunction::GetSuperFunction >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_757 = das::das_call_member< FProperty * (UFunction::*)() const,&UFunction::GetReturnProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1875:29
	makeExtern<DAS_CALL_METHOD(_method_757), SimNode_ExtFuncCall >(lib,"GetReturnProperty","das_call_member< FProperty * (UFunction::*)() const , &UFunction::GetReturnProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_758 = das::das_call_member< UClass * (UFunction::*)() const,&UFunction::GetOuterUClassUnchecked >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1878:22
	makeExtern<DAS_CALL_METHOD(_method_758), SimNode_ExtFuncCall >(lib,"GetOuterUClassUnchecked","das_call_member< UClass * (UFunction::*)() const , &UFunction::GetOuterUClassUnchecked >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_759 = das::das_call_member< bool (UFunction::*)(EFunctionFlags) const,&UFunction::HasAnyFunctionFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1892:19
	makeExtern<DAS_CALL_METHOD(_method_759), SimNode_ExtFuncCall >(lib,"HasAnyFunctionFlags","das_call_member< bool (UFunction::*)(EFunctionFlags) const , &UFunction::HasAnyFunctionFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_760 = das::das_call_member< bool (UFunction::*)(EFunctionFlags) const,&UFunction::HasAllFunctionFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1903:19
	makeExtern<DAS_CALL_METHOD(_method_760), SimNode_ExtFuncCall >(lib,"HasAllFunctionFlags","das_call_member< bool (UFunction::*)(EFunctionFlags) const , &UFunction::HasAllFunctionFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1911:28
	makeExtern< uint64 (*)(const UFunction *) , _dasUnreal_static_269_GetDefaultIgnoredSignatureCompatibilityFlags , SimNode_ExtFuncCall >(lib,"GetDefaultIgnoredSignatureCompatibilityFlags","_dasUnreal_static_269_GetDefaultIgnoredSignatureCompatibilityFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UFunction*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_762 = das::das_call_member< bool (UFunction::*)(const UFunction *) const,&UFunction::IsSignatureCompatibleWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1929:23
	makeExtern<DAS_CALL_METHOD(_method_762), SimNode_ExtFuncCall >(lib,"IsSignatureCompatibleWith","das_call_member< bool (UFunction::*)(const UFunction *) const , &UFunction::IsSignatureCompatibleWith >::invoke")
		->args({"self","OtherFunction"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_763 = das::das_call_member< bool (UFunction::*)(const UFunction *,uint64) const,&UFunction::IsSignatureCompatibleWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1941:23
	makeExtern<DAS_CALL_METHOD(_method_763), SimNode_ExtFuncCall >(lib,"IsSignatureCompatibleWith","das_call_member< bool (UFunction::*)(const UFunction *,uint64) const , &UFunction::IsSignatureCompatibleWith >::invoke")
		->args({"self","OtherFunction","IgnoreFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2001:2
	makeExtern< UClass * (*)(const UEnum *) , _dasUnreal_static_270_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_270_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UEnum*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2001:2
	makeExtern< const wchar_t * (*)(const UEnum *) , _dasUnreal_static_271_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_271_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UEnum*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2001:2
	makeExtern< void (*)(const UEnum *) , _dasUnreal_static_272_StaticRegisterNativesUEnum , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUEnum","_dasUnreal_static_272_StaticRegisterNativesUEnum")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UEnum*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_767 = das::das_call_member< int (UEnum::*)(int64) const,&UEnum::GetIndexByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2022:20
	makeExtern<DAS_CALL_METHOD(_method_767), SimNode_ExtFuncCall >(lib,"GetIndexByValue","das_call_member< int (UEnum::*)(int64) const , &UEnum::GetIndexByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_768 = das::das_call_member< int64 (UEnum::*)(int) const,&UEnum::GetValueByIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2035:20
	makeExtern<DAS_CALL_METHOD(_method_768), SimNode_ExtFuncCall >(lib,"GetValueByIndex","das_call_member< int64 (UEnum::*)(int) const , &UEnum::GetValueByIndex >::invoke")
		->args({"self","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_769 = das::das_call_member< FName (UEnum::*)(int) const,&UEnum::GetNameByIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2042:24
	makeExtern<DAS_CALL_METHOD(_method_769), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameByIndex","das_call_member< FName (UEnum::*)(int) const , &UEnum::GetNameByIndex >::invoke")
		->args({"self","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_770 = das::das_call_member< int (UEnum::*)(FName,EGetByNameFlags) const,&UEnum::GetIndexByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2045:24
	makeExtern<DAS_CALL_METHOD(_method_770), SimNode_ExtFuncCall >(lib,"GetIndexByName","das_call_member< int (UEnum::*)(FName,EGetByNameFlags) const , &UEnum::GetIndexByName >::invoke")
		->args({"self","InName","Flags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EGetByNameFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_771 = das::das_call_member< FName (UEnum::*)(int64) const,&UEnum::GetNameByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2048:24
	makeExtern<DAS_CALL_METHOD(_method_771), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameByValue","das_call_member< FName (UEnum::*)(int64) const , &UEnum::GetNameByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_772 = das::das_call_member< int64 (UEnum::*)(FName,EGetByNameFlags) const,&UEnum::GetValueByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2051:24
	makeExtern<DAS_CALL_METHOD(_method_772), SimNode_ExtFuncCall >(lib,"GetValueByName","das_call_member< int64 (UEnum::*)(FName,EGetByNameFlags) const , &UEnum::GetValueByName >::invoke")
		->args({"self","InName","Flags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EGetByNameFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_773 = das::das_call_member< FString (UEnum::*)(int) const,&UEnum::GetNameStringByIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2054:26
	makeExtern<DAS_CALL_METHOD(_method_773), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameStringByIndex","das_call_member< FString (UEnum::*)(int) const , &UEnum::GetNameStringByIndex >::invoke")
		->args({"self","InIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_774 = das::das_call_member< int (UEnum::*)(const FString &,EGetByNameFlags) const,&UEnum::GetIndexByNameString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2057:24
	makeExtern<DAS_CALL_METHOD(_method_774), SimNode_ExtFuncCall >(lib,"GetIndexByNameString","das_call_member< int (UEnum::*)(const FString &,EGetByNameFlags) const , &UEnum::GetIndexByNameString >::invoke")
		->args({"self","SearchString","Flags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EGetByNameFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_775 = das::das_call_member< FString (UEnum::*)(int64) const,&UEnum::GetNameStringByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2060:26
	makeExtern<DAS_CALL_METHOD(_method_775), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameStringByValue","das_call_member< FString (UEnum::*)(int64) const , &UEnum::GetNameStringByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_776 = das::das_call_member< FString (UEnum::*)(int64) const,&UEnum::GetValueOrBitfieldAsString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2063:26
	makeExtern<DAS_CALL_METHOD(_method_776), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetValueOrBitfieldAsString","das_call_member< FString (UEnum::*)(int64) const , &UEnum::GetValueOrBitfieldAsString >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_777 = das::das_call_member< bool (UEnum::*)(FString &,int64) const,&UEnum::FindNameStringByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2066:23
	makeExtern<DAS_CALL_METHOD(_method_777), SimNode_ExtFuncCall >(lib,"FindNameStringByValue","das_call_member< bool (UEnum::*)(FString &,int64) const , &UEnum::FindNameStringByValue >::invoke")
		->args({"self","Out","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_778 = das::das_call_member< int64 (UEnum::*)(const FString &,EGetByNameFlags) const,&UEnum::GetValueByNameString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2069:24
	makeExtern<DAS_CALL_METHOD(_method_778), SimNode_ExtFuncCall >(lib,"GetValueByNameString","das_call_member< int64 (UEnum::*)(const FString &,EGetByNameFlags) const , &UEnum::GetValueByNameString >::invoke")
		->args({"self","SearchString","Flags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EGetByNameFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2079:32
	makeExtern< FText (*)(const UEnum &,int) , _dasUnreal_virtual_273_GetDisplayNameTextByIndex , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDisplayNameTextByIndex","_dasUnreal_virtual_273_GetDisplayNameTextByIndex")
		->args({"self","InIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_780 = das::das_call_member< FText (UEnum::*)(int64) const,&UEnum::GetDisplayNameTextByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2082:24
	makeExtern<DAS_CALL_METHOD(_method_780), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDisplayNameTextByValue","das_call_member< FText (UEnum::*)(int64) const , &UEnum::GetDisplayNameTextByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_781 = das::das_call_member< bool (UEnum::*)(FText &,int64) const,&UEnum::FindDisplayNameTextByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2085:23
	makeExtern<DAS_CALL_METHOD(_method_781), SimNode_ExtFuncCall >(lib,"FindDisplayNameTextByValue","das_call_member< bool (UEnum::*)(FText &,int64) const , &UEnum::FindDisplayNameTextByValue >::invoke")
		->args({"self","Out","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2096:34
	makeExtern< FString (*)(const UEnum &,int) , _dasUnreal_virtual_274_GetAuthoredNameStringByIndex , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredNameStringByIndex","_dasUnreal_virtual_274_GetAuthoredNameStringByIndex")
		->args({"self","InIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_783 = das::das_call_member< FString (UEnum::*)(int64) const,&UEnum::GetAuthoredNameStringByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2099:26
	makeExtern<DAS_CALL_METHOD(_method_783), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredNameStringByValue","das_call_member< FString (UEnum::*)(int64) const , &UEnum::GetAuthoredNameStringByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_784 = das::das_call_member< bool (UEnum::*)(FString &,int64) const,&UEnum::FindAuthoredNameStringByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2102:23
	makeExtern<DAS_CALL_METHOD(_method_784), SimNode_ExtFuncCall >(lib,"FindAuthoredNameStringByValue","das_call_member< bool (UEnum::*)(FString &,int64) const , &UEnum::FindAuthoredNameStringByValue >::invoke")
		->args({"self","Out","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_785 = das::das_call_member< int64 (UEnum::*)() const,&UEnum::GetMaxEnumValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2105:24
	makeExtern<DAS_CALL_METHOD(_method_785), SimNode_ExtFuncCall >(lib,"GetMaxEnumValue","das_call_member< int64 (UEnum::*)() const , &UEnum::GetMaxEnumValue >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_786 = das::das_call_member< bool (UEnum::*)(int64) const,&UEnum::IsValidEnumValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2108:23
	makeExtern<DAS_CALL_METHOD(_method_786), SimNode_ExtFuncCall >(lib,"IsValidEnumValue","das_call_member< bool (UEnum::*)(int64) const , &UEnum::IsValidEnumValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_787 = das::das_call_member< bool (UEnum::*)(FName) const,&UEnum::IsValidEnumName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2111:23
	makeExtern<DAS_CALL_METHOD(_method_787), SimNode_ExtFuncCall >(lib,"IsValidEnumName","das_call_member< bool (UEnum::*)(FName) const , &UEnum::IsValidEnumName >::invoke")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_788 = das::das_call_member< void (UEnum::*)(),&UEnum::RemoveNamesFromPrimaryList >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2121:23
	makeExtern<DAS_CALL_METHOD(_method_788), SimNode_ExtFuncCall >(lib,"RemoveNamesFromPrimaryList","das_call_member< void (UEnum::*)() , &UEnum::RemoveNamesFromPrimaryList >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_789 = das::das_call_member< UEnum::ECppForm (UEnum::*)() const,&UEnum::GetCppForm >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2137:11
	makeExtern<DAS_CALL_METHOD(_method_789), SimNode_ExtFuncCall >(lib,"GetCppForm","das_call_member< UEnum::ECppForm (UEnum::*)() const , &UEnum::GetCppForm >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_790 = das::das_call_member< void (UEnum::*)(EEnumFlags),&UEnum::SetEnumFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2142:7
	makeExtern<DAS_CALL_METHOD(_method_790), SimNode_ExtFuncCall >(lib,"SetEnumFlags","das_call_member< void (UEnum::*)(EEnumFlags) , &UEnum::SetEnumFlags >::invoke")
		->args({"self","FlagsToSet"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_791 = das::das_call_member< bool (UEnum::*)(EEnumFlags) const,&UEnum::HasAnyEnumFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2147:7
	makeExtern<DAS_CALL_METHOD(_method_791), SimNode_ExtFuncCall >(lib,"HasAnyEnumFlags","das_call_member< bool (UEnum::*)(EEnumFlags) const , &UEnum::HasAnyEnumFlags >::invoke")
		->args({"self","InFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2158:14
	makeExtern< bool (*)(const UEnum *,const wchar_t *) , _dasUnreal_static_275_IsFullEnumName , SimNode_ExtFuncCall >(lib,"IsFullEnumName","_dasUnreal_static_275_IsFullEnumName")
		->args({"self","InEnumName"})
		->arg_type(0,makeType<TExplicit<const UEnum*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2169:34
	makeExtern< FString (*)(const UEnum &,const wchar_t *) , _dasUnreal_virtual_276_GenerateFullEnumName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GenerateFullEnumName","_dasUnreal_virtual_276_GenerateFullEnumName")
		->args({"self","InEnumName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2213:31
	makeExtern< int64 (*)(const UEnum *,const wchar_t *&) , _dasUnreal_static_277_ParseEnum , SimNode_ExtFuncCall >(lib,"ParseEnum","_dasUnreal_static_277_ParseEnum")
		->args({"self","Str"})
		->arg_type(0,makeType<TExplicit<const UEnum*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_795 = das::das_call_member< bool (UEnum::*)() const,&UEnum::ContainsExistingMax >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2220:23
	makeExtern<DAS_CALL_METHOD(_method_795), SimNode_ExtFuncCall >(lib,"ContainsExistingMax","das_call_member< bool (UEnum::*)() const , &UEnum::ContainsExistingMax >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_796 = das::das_call_member< int (UEnum::*)() const,&UEnum::NumEnums >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2235:8
	makeExtern<DAS_CALL_METHOD(_method_796), SimNode_ExtFuncCall >(lib,"NumEnums","das_call_member< int (UEnum::*)() const , &UEnum::NumEnums >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_797 = das::das_call_member< FString (UEnum::*)() const,&UEnum::GenerateEnumPrefix >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2246:26
	makeExtern<DAS_CALL_METHOD(_method_797), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GenerateEnumPrefix","das_call_member< FString (UEnum::*)() const , &UEnum::GenerateEnumPrefix >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_798 = das::das_call_member< FText (UEnum::*)(int) const,&UEnum::GetToolTipTextByIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2256:24
	makeExtern<DAS_CALL_METHOD(_method_798), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetToolTipTextByIndex","das_call_member< FText (UEnum::*)(int) const , &UEnum::GetToolTipTextByIndex >::invoke")
		->args({"self","NameIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_799 = das::das_call_member< bool (UEnum::*)(const wchar_t *,int) const,&UEnum::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2268:23
	makeExtern<DAS_CALL_METHOD(_method_799), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (UEnum::*)(const wchar_t *,int) const , &UEnum::HasMetaData >::invoke")
		->args({"self","Key","NameIndex"})
		->arg_init(2,make_smart<ExprConstInt>(-1))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_800 = das::das_call_member< FString (UEnum::*)(const wchar_t *,int,bool) const,&UEnum::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2279:26
	makeExtern<DAS_CALL_METHOD(_method_800), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetMetaData","das_call_member< FString (UEnum::*)(const wchar_t *,int,bool) const , &UEnum::GetMetaData >::invoke")
		->args({"self","Key","NameIndex","bAllowRemap"})
		->arg_init(2,make_smart<ExprConstInt>(-1))
		->arg_init(3,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_801 = das::das_call_member< void (UEnum::*)(const wchar_t *,const wchar_t *,int) const,&UEnum::SetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2289:23
	makeExtern<DAS_CALL_METHOD(_method_801), SimNode_ExtFuncCall >(lib,"SetMetaData","das_call_member< void (UEnum::*)(const wchar_t *,const wchar_t *,int) const , &UEnum::SetMetaData >::invoke")
		->args({"self","Key","InValue","NameIndex"})
		->arg_init(3,make_smart<ExprConstInt>(-1))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_802 = das::das_call_member< void (UEnum::*)(const wchar_t *,int) const,&UEnum::RemoveMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2298:23
	makeExtern<DAS_CALL_METHOD(_method_802), SimNode_ExtFuncCall >(lib,"RemoveMetaData","das_call_member< void (UEnum::*)(const wchar_t *,int) const , &UEnum::RemoveMetaData >::invoke")
		->args({"self","Key","NameIndex"})
		->arg_init(2,make_smart<ExprConstInt>(-1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2511:31
	makeExtern< void (*)(UEnum &,FArchive &) , _dasUnreal_virtual_278_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_278_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2512:31
	makeExtern< void (*)(UEnum &) , _dasUnreal_virtual_279_BeginDestroy , SimNode_ExtFuncCall >(lib,"BeginDestroy","_dasUnreal_virtual_279_BeginDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2648:15
	makeExtern< bool (*)(const FCppClassTypeInfo &) , _dasUnreal_virtual_280_IsAbstract , SimNode_ExtFuncCall >(lib,"IsAbstract","_dasUnreal_virtual_280_IsAbstract")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FImplementedInterface>(*this,lib,"FImplementedInterface","FImplementedInterface");
	addCtorAndUsing<FImplementedInterface,UClass *,int,bool>(*this,lib,"FImplementedInterface","FImplementedInterface")
		->args({"InClass","InOffset","InImplementedByK2"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2728:2
	makeExtern< UClass * (*)(const UClass *) , _dasUnreal_static_281_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_281_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UClass*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2728:2
	makeExtern< const wchar_t * (*)(const UClass *) , _dasUnreal_static_282_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_282_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UClass*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2728:2
	makeExtern< void (*)(const UClass *) , _dasUnreal_static_283_StaticRegisterNativesUClass , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUClass","_dasUnreal_static_283_StaticRegisterNativesUClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UClass*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_809 = das::das_call_member< UPackage * (UClass::*)() const,&UClass::GetOuterUPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2729:2
	makeExtern<DAS_CALL_METHOD(_method_809), SimNode_ExtFuncCall >(lib,"GetOuterUPackage","das_call_member< UPackage * (UClass::*)() const , &UClass::GetOuterUPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_810 = das::das_call_member< void (UClass::*)(),&UClass::DestroyPropertiesPendingDestruction >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2772:23
	makeExtern<DAS_CALL_METHOD(_method_810), SimNode_ExtFuncCall >(lib,"DestroyPropertiesPendingDestruction","das_call_member< void (UClass::*)() , &UClass::DestroyPropertiesPendingDestruction >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2780:15
	makeExtern< void (*)(UClass &,FUObjectSerializeContext *) , _dasUnreal_virtual_284_ConditionalRecompileClass , SimNode_ExtFuncCall >(lib,"ConditionalRecompileClass","_dasUnreal_virtual_284_ConditionalRecompileClass")
		->args({"self","InLoadContext"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2781:15
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_285_FlushCompilationQueueForLevel , SimNode_ExtFuncCall >(lib,"FlushCompilationQueueForLevel","_dasUnreal_virtual_285_FlushCompilationQueueForLevel")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_813 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsFunctionHidden >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2796:23
	makeExtern<DAS_CALL_METHOD(_method_813), SimNode_ExtFuncCall >(lib,"IsFunctionHidden","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsFunctionHidden >::invoke")
		->args({"self","InFunction"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_814 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsAutoExpandCategory >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2798:23
	makeExtern<DAS_CALL_METHOD(_method_814), SimNode_ExtFuncCall >(lib,"IsAutoExpandCategory","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsAutoExpandCategory >::invoke")
		->args({"self","InCategory"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_815 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsPrioritizeCategory >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2800:23
	makeExtern<DAS_CALL_METHOD(_method_815), SimNode_ExtFuncCall >(lib,"IsPrioritizeCategory","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsPrioritizeCategory >::invoke")
		->args({"self","InCategory"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_816 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsAutoCollapseCategory >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2802:23
	makeExtern<DAS_CALL_METHOD(_method_816), SimNode_ExtFuncCall >(lib,"IsAutoCollapseCategory","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsAutoCollapseCategory >::invoke")
		->args({"self","InCategory"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_817 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsClassGroupName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2804:23
	makeExtern<DAS_CALL_METHOD(_method_817), SimNode_ExtFuncCall >(lib,"IsClassGroupName","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsClassGroupName >::invoke")
		->args({"self","InGroupName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_818 = das::das_call_member< void (UClass::*)(UObject *,FReferenceCollector &) const,&UClass::CallAddReferencedObjects >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2812:19
	makeExtern<DAS_CALL_METHOD(_method_818), SimNode_ExtFuncCall >(lib,"CallAddReferencedObjects","das_call_member< void (UClass::*)(UObject *,FReferenceCollector &) const , &UClass::CallAddReferencedObjects >::invoke")
		->args({"self","This","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_819 = das::das_call_member< void * (UClass::*)(),&UClass::GetOrCreateSparseClassData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2864:8
	makeExtern<DAS_CALL_METHOD(_method_819), SimNode_ExtFuncCall >(lib,"GetOrCreateSparseClassData","das_call_member< void * (UClass::*)() , &UClass::GetOrCreateSparseClassData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_820 = das::das_call_member< UScriptStruct * (UClass::*)() const,&UClass::GetSparseClassDataStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2869:33
	makeExtern<DAS_CALL_METHOD(_method_820), SimNode_ExtFuncCall >(lib,"GetSparseClassDataStruct","das_call_member< UScriptStruct * (UClass::*)() const , &UClass::GetSparseClassDataStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_821 = das::das_call_member< void (UClass::*)(UScriptStruct *),&UClass::SetSparseClassDataStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2871:23
	makeExtern<DAS_CALL_METHOD(_method_821), SimNode_ExtFuncCall >(lib,"SetSparseClassDataStruct","das_call_member< void (UClass::*)(UScriptStruct *) , &UClass::SetSparseClassDataStruct >::invoke")
		->args({"self","InSparseClassDataStruct"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_822 = das::das_call_member< void (UClass::*)(bool),&UClass::ClearSparseClassDataStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2877:23
	makeExtern<DAS_CALL_METHOD(_method_822), SimNode_ExtFuncCall >(lib,"ClearSparseClassDataStruct","das_call_member< void (UClass::*)(bool) , &UClass::ClearSparseClassDataStruct >::invoke")
		->args({"self","bInRecomplingOnLoad"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

