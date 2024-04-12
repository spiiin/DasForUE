// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_10() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1753:34
	makeExtern< FString (*)(const UScriptStruct &,unsigned int) , _dasUnreal_virtual_258_GetStructCPPName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetStructCPPName","_dasUnreal_virtual_258_GetStructCPPName")
		->args({"self","CPPExportFlags"})
		->arg_init(1,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1759:31
	makeExtern< void (*)(const UScriptStruct &,unsigned char *) , _dasUnreal_virtual_259_InitializeDefaultValue , SimNode_ExtFuncCall >(lib,"InitializeDefaultValue","_dasUnreal_virtual_259_InitializeDefaultValue")
		->args({"self","InStructData"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1782:2
	makeExtern< UClass * (*)(const UFunction *) , _dasUnreal_static_260_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_260_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UFunction*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1782:2
	makeExtern< const wchar_t * (*)(const UFunction *) , _dasUnreal_static_261_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_261_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UFunction*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1782:2
	makeExtern< void (*)(const UFunction *) , _dasUnreal_static_262_StaticRegisterNativesUFunction , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUFunction","_dasUnreal_static_262_StaticRegisterNativesUFunction")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UFunction*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_734 = das::das_call_member< UClass * (UFunction::*)() const,&UFunction::GetOuterUClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1783:2
	makeExtern<DAS_CALL_METHOD(_method_734), SimNode_ExtFuncCall >(lib,"GetOuterUClass","das_call_member< UClass * (UFunction::*)() const , &UFunction::GetOuterUClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_735 = das::das_call_member< void (UFunction::*)(UObject *,FFrame &,void *const),&UFunction::Invoke >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1851:23
	makeExtern<DAS_CALL_METHOD(_method_735), SimNode_ExtFuncCall >(lib,"Invoke","das_call_member< void (UFunction::*)(UObject *,FFrame &,void *const) , &UFunction::Invoke >::invoke")
		->args({"self","Obj","Stack","Z_Param__Result"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_736 = das::das_call_member< void (UFunction::*)(),&UFunction::InitializeDerivedMembers >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1858:23
	makeExtern<DAS_CALL_METHOD(_method_736), SimNode_ExtFuncCall >(lib,"InitializeDerivedMembers","das_call_member< void (UFunction::*)() , &UFunction::InitializeDerivedMembers >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1861:31
	makeExtern< void (*)(UFunction &,FArchive &) , _dasUnreal_virtual_263_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_263_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1862:31
	makeExtern< void (*)(UFunction &) , _dasUnreal_virtual_264_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_264_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1865:31
	makeExtern< void (*)(UFunction &) , _dasUnreal_virtual_265_Bind , SimNode_ExtFuncCall >(lib,"Bind","_dasUnreal_virtual_265_Bind")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1868:19
	makeExtern< UStruct * (*)(const UFunction &) , _dasUnreal_virtual_266_GetInheritanceSuper , SimNode_ExtFuncCall >(lib,"GetInheritanceSuper","_dasUnreal_virtual_266_GetInheritanceSuper")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1869:31
	makeExtern< void (*)(UFunction &,FArchive &,bool) , _dasUnreal_virtual_267_Link , SimNode_ExtFuncCall >(lib,"Link","_dasUnreal_virtual_267_Link")
		->args({"self","Ar","bRelinkExistingProperties"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_742 = das::das_call_member< UFunction * (UFunction::*)() const,&UFunction::GetSuperFunction >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1872:29
	makeExtern<DAS_CALL_METHOD(_method_742), SimNode_ExtFuncCall >(lib,"GetSuperFunction","das_call_member< UFunction * (UFunction::*)() const , &UFunction::GetSuperFunction >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_743 = das::das_call_member< FProperty * (UFunction::*)() const,&UFunction::GetReturnProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1875:29
	makeExtern<DAS_CALL_METHOD(_method_743), SimNode_ExtFuncCall >(lib,"GetReturnProperty","das_call_member< FProperty * (UFunction::*)() const , &UFunction::GetReturnProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_744 = das::das_call_member< UClass * (UFunction::*)() const,&UFunction::GetOuterUClassUnchecked >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1878:22
	makeExtern<DAS_CALL_METHOD(_method_744), SimNode_ExtFuncCall >(lib,"GetOuterUClassUnchecked","das_call_member< UClass * (UFunction::*)() const , &UFunction::GetOuterUClassUnchecked >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_745 = das::das_call_member< bool (UFunction::*)(EFunctionFlags) const,&UFunction::HasAnyFunctionFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1892:19
	makeExtern<DAS_CALL_METHOD(_method_745), SimNode_ExtFuncCall >(lib,"HasAnyFunctionFlags","das_call_member< bool (UFunction::*)(EFunctionFlags) const , &UFunction::HasAnyFunctionFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_746 = das::das_call_member< bool (UFunction::*)(EFunctionFlags) const,&UFunction::HasAllFunctionFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1903:19
	makeExtern<DAS_CALL_METHOD(_method_746), SimNode_ExtFuncCall >(lib,"HasAllFunctionFlags","das_call_member< bool (UFunction::*)(EFunctionFlags) const , &UFunction::HasAllFunctionFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1911:28
	makeExtern< uint64 (*)(const UFunction *) , _dasUnreal_static_268_GetDefaultIgnoredSignatureCompatibilityFlags , SimNode_ExtFuncCall >(lib,"GetDefaultIgnoredSignatureCompatibilityFlags","_dasUnreal_static_268_GetDefaultIgnoredSignatureCompatibilityFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UFunction*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_748 = das::das_call_member< bool (UFunction::*)(const UFunction *) const,&UFunction::IsSignatureCompatibleWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1929:23
	makeExtern<DAS_CALL_METHOD(_method_748), SimNode_ExtFuncCall >(lib,"IsSignatureCompatibleWith","das_call_member< bool (UFunction::*)(const UFunction *) const , &UFunction::IsSignatureCompatibleWith >::invoke")
		->args({"self","OtherFunction"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_749 = das::das_call_member< bool (UFunction::*)(const UFunction *,uint64) const,&UFunction::IsSignatureCompatibleWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1941:23
	makeExtern<DAS_CALL_METHOD(_method_749), SimNode_ExtFuncCall >(lib,"IsSignatureCompatibleWith","das_call_member< bool (UFunction::*)(const UFunction *,uint64) const , &UFunction::IsSignatureCompatibleWith >::invoke")
		->args({"self","OtherFunction","IgnoreFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2001:2
	makeExtern< UClass * (*)(const UEnum *) , _dasUnreal_static_269_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_269_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UEnum*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2001:2
	makeExtern< const wchar_t * (*)(const UEnum *) , _dasUnreal_static_270_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_270_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UEnum*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2001:2
	makeExtern< void (*)(const UEnum *) , _dasUnreal_static_271_StaticRegisterNativesUEnum , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUEnum","_dasUnreal_static_271_StaticRegisterNativesUEnum")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UEnum*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_753 = das::das_call_member< int (UEnum::*)(int64) const,&UEnum::GetIndexByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2022:20
	makeExtern<DAS_CALL_METHOD(_method_753), SimNode_ExtFuncCall >(lib,"GetIndexByValue","das_call_member< int (UEnum::*)(int64) const , &UEnum::GetIndexByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_754 = das::das_call_member< int64 (UEnum::*)(int) const,&UEnum::GetValueByIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2035:20
	makeExtern<DAS_CALL_METHOD(_method_754), SimNode_ExtFuncCall >(lib,"GetValueByIndex","das_call_member< int64 (UEnum::*)(int) const , &UEnum::GetValueByIndex >::invoke")
		->args({"self","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_755 = das::das_call_member< FName (UEnum::*)(int) const,&UEnum::GetNameByIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2042:24
	makeExtern<DAS_CALL_METHOD(_method_755), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameByIndex","das_call_member< FName (UEnum::*)(int) const , &UEnum::GetNameByIndex >::invoke")
		->args({"self","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_756 = das::das_call_member< int (UEnum::*)(FName,EGetByNameFlags) const,&UEnum::GetIndexByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2045:24
	makeExtern<DAS_CALL_METHOD(_method_756), SimNode_ExtFuncCall >(lib,"GetIndexByName","das_call_member< int (UEnum::*)(FName,EGetByNameFlags) const , &UEnum::GetIndexByName >::invoke")
		->args({"self","InName","Flags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EGetByNameFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_757 = das::das_call_member< FName (UEnum::*)(int64) const,&UEnum::GetNameByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2048:24
	makeExtern<DAS_CALL_METHOD(_method_757), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameByValue","das_call_member< FName (UEnum::*)(int64) const , &UEnum::GetNameByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_758 = das::das_call_member< int64 (UEnum::*)(FName,EGetByNameFlags) const,&UEnum::GetValueByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2051:24
	makeExtern<DAS_CALL_METHOD(_method_758), SimNode_ExtFuncCall >(lib,"GetValueByName","das_call_member< int64 (UEnum::*)(FName,EGetByNameFlags) const , &UEnum::GetValueByName >::invoke")
		->args({"self","InName","Flags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EGetByNameFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_759 = das::das_call_member< FString (UEnum::*)(int) const,&UEnum::GetNameStringByIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2054:26
	makeExtern<DAS_CALL_METHOD(_method_759), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameStringByIndex","das_call_member< FString (UEnum::*)(int) const , &UEnum::GetNameStringByIndex >::invoke")
		->args({"self","InIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_760 = das::das_call_member< int (UEnum::*)(const FString &,EGetByNameFlags) const,&UEnum::GetIndexByNameString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2057:24
	makeExtern<DAS_CALL_METHOD(_method_760), SimNode_ExtFuncCall >(lib,"GetIndexByNameString","das_call_member< int (UEnum::*)(const FString &,EGetByNameFlags) const , &UEnum::GetIndexByNameString >::invoke")
		->args({"self","SearchString","Flags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EGetByNameFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_761 = das::das_call_member< FString (UEnum::*)(int64) const,&UEnum::GetNameStringByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2060:26
	makeExtern<DAS_CALL_METHOD(_method_761), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameStringByValue","das_call_member< FString (UEnum::*)(int64) const , &UEnum::GetNameStringByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_762 = das::das_call_member< FString (UEnum::*)(int64) const,&UEnum::GetValueOrBitfieldAsString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2063:26
	makeExtern<DAS_CALL_METHOD(_method_762), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetValueOrBitfieldAsString","das_call_member< FString (UEnum::*)(int64) const , &UEnum::GetValueOrBitfieldAsString >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_763 = das::das_call_member< bool (UEnum::*)(FString &,int64) const,&UEnum::FindNameStringByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2066:23
	makeExtern<DAS_CALL_METHOD(_method_763), SimNode_ExtFuncCall >(lib,"FindNameStringByValue","das_call_member< bool (UEnum::*)(FString &,int64) const , &UEnum::FindNameStringByValue >::invoke")
		->args({"self","Out","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_764 = das::das_call_member< int64 (UEnum::*)(const FString &,EGetByNameFlags) const,&UEnum::GetValueByNameString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2069:24
	makeExtern<DAS_CALL_METHOD(_method_764), SimNode_ExtFuncCall >(lib,"GetValueByNameString","das_call_member< int64 (UEnum::*)(const FString &,EGetByNameFlags) const , &UEnum::GetValueByNameString >::invoke")
		->args({"self","SearchString","Flags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EGetByNameFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2079:32
	makeExtern< FText (*)(const UEnum &,int) , _dasUnreal_virtual_272_GetDisplayNameTextByIndex , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDisplayNameTextByIndex","_dasUnreal_virtual_272_GetDisplayNameTextByIndex")
		->args({"self","InIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_766 = das::das_call_member< FText (UEnum::*)(int64) const,&UEnum::GetDisplayNameTextByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2082:24
	makeExtern<DAS_CALL_METHOD(_method_766), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDisplayNameTextByValue","das_call_member< FText (UEnum::*)(int64) const , &UEnum::GetDisplayNameTextByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_767 = das::das_call_member< bool (UEnum::*)(FText &,int64) const,&UEnum::FindDisplayNameTextByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2085:23
	makeExtern<DAS_CALL_METHOD(_method_767), SimNode_ExtFuncCall >(lib,"FindDisplayNameTextByValue","das_call_member< bool (UEnum::*)(FText &,int64) const , &UEnum::FindDisplayNameTextByValue >::invoke")
		->args({"self","Out","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2096:34
	makeExtern< FString (*)(const UEnum &,int) , _dasUnreal_virtual_273_GetAuthoredNameStringByIndex , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredNameStringByIndex","_dasUnreal_virtual_273_GetAuthoredNameStringByIndex")
		->args({"self","InIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_769 = das::das_call_member< FString (UEnum::*)(int64) const,&UEnum::GetAuthoredNameStringByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2099:26
	makeExtern<DAS_CALL_METHOD(_method_769), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredNameStringByValue","das_call_member< FString (UEnum::*)(int64) const , &UEnum::GetAuthoredNameStringByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_770 = das::das_call_member< bool (UEnum::*)(FString &,int64) const,&UEnum::FindAuthoredNameStringByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2102:23
	makeExtern<DAS_CALL_METHOD(_method_770), SimNode_ExtFuncCall >(lib,"FindAuthoredNameStringByValue","das_call_member< bool (UEnum::*)(FString &,int64) const , &UEnum::FindAuthoredNameStringByValue >::invoke")
		->args({"self","Out","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_771 = das::das_call_member< int64 (UEnum::*)() const,&UEnum::GetMaxEnumValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2105:24
	makeExtern<DAS_CALL_METHOD(_method_771), SimNode_ExtFuncCall >(lib,"GetMaxEnumValue","das_call_member< int64 (UEnum::*)() const , &UEnum::GetMaxEnumValue >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_772 = das::das_call_member< bool (UEnum::*)(int64) const,&UEnum::IsValidEnumValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2108:23
	makeExtern<DAS_CALL_METHOD(_method_772), SimNode_ExtFuncCall >(lib,"IsValidEnumValue","das_call_member< bool (UEnum::*)(int64) const , &UEnum::IsValidEnumValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_773 = das::das_call_member< bool (UEnum::*)(FName) const,&UEnum::IsValidEnumName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2111:23
	makeExtern<DAS_CALL_METHOD(_method_773), SimNode_ExtFuncCall >(lib,"IsValidEnumName","das_call_member< bool (UEnum::*)(FName) const , &UEnum::IsValidEnumName >::invoke")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_774 = das::das_call_member< void (UEnum::*)(),&UEnum::RemoveNamesFromPrimaryList >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2121:23
	makeExtern<DAS_CALL_METHOD(_method_774), SimNode_ExtFuncCall >(lib,"RemoveNamesFromPrimaryList","das_call_member< void (UEnum::*)() , &UEnum::RemoveNamesFromPrimaryList >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_775 = das::das_call_member< UEnum::ECppForm (UEnum::*)() const,&UEnum::GetCppForm >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2137:11
	makeExtern<DAS_CALL_METHOD(_method_775), SimNode_ExtFuncCall >(lib,"GetCppForm","das_call_member< UEnum::ECppForm (UEnum::*)() const , &UEnum::GetCppForm >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_776 = das::das_call_member< void (UEnum::*)(EEnumFlags),&UEnum::SetEnumFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2142:7
	makeExtern<DAS_CALL_METHOD(_method_776), SimNode_ExtFuncCall >(lib,"SetEnumFlags","das_call_member< void (UEnum::*)(EEnumFlags) , &UEnum::SetEnumFlags >::invoke")
		->args({"self","FlagsToSet"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_777 = das::das_call_member< bool (UEnum::*)(EEnumFlags) const,&UEnum::HasAnyEnumFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2147:7
	makeExtern<DAS_CALL_METHOD(_method_777), SimNode_ExtFuncCall >(lib,"HasAnyEnumFlags","das_call_member< bool (UEnum::*)(EEnumFlags) const , &UEnum::HasAnyEnumFlags >::invoke")
		->args({"self","InFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2158:14
	makeExtern< bool (*)(const UEnum *,const wchar_t *) , _dasUnreal_static_274_IsFullEnumName , SimNode_ExtFuncCall >(lib,"IsFullEnumName","_dasUnreal_static_274_IsFullEnumName")
		->args({"self","InEnumName"})
		->arg_type(0,makeType<TExplicit<const UEnum*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2169:34
	makeExtern< FString (*)(const UEnum &,const wchar_t *) , _dasUnreal_virtual_275_GenerateFullEnumName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GenerateFullEnumName","_dasUnreal_virtual_275_GenerateFullEnumName")
		->args({"self","InEnumName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2213:31
	makeExtern< int64 (*)(const UEnum *,const wchar_t *&) , _dasUnreal_static_276_ParseEnum , SimNode_ExtFuncCall >(lib,"ParseEnum","_dasUnreal_static_276_ParseEnum")
		->args({"self","Str"})
		->arg_type(0,makeType<TExplicit<const UEnum*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_781 = das::das_call_member< bool (UEnum::*)() const,&UEnum::ContainsExistingMax >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2220:23
	makeExtern<DAS_CALL_METHOD(_method_781), SimNode_ExtFuncCall >(lib,"ContainsExistingMax","das_call_member< bool (UEnum::*)() const , &UEnum::ContainsExistingMax >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_782 = das::das_call_member< int (UEnum::*)() const,&UEnum::NumEnums >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2235:8
	makeExtern<DAS_CALL_METHOD(_method_782), SimNode_ExtFuncCall >(lib,"NumEnums","das_call_member< int (UEnum::*)() const , &UEnum::NumEnums >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_783 = das::das_call_member< FString (UEnum::*)() const,&UEnum::GenerateEnumPrefix >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2246:26
	makeExtern<DAS_CALL_METHOD(_method_783), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GenerateEnumPrefix","das_call_member< FString (UEnum::*)() const , &UEnum::GenerateEnumPrefix >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_784 = das::das_call_member< FText (UEnum::*)(int) const,&UEnum::GetToolTipTextByIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2256:24
	makeExtern<DAS_CALL_METHOD(_method_784), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetToolTipTextByIndex","das_call_member< FText (UEnum::*)(int) const , &UEnum::GetToolTipTextByIndex >::invoke")
		->args({"self","NameIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_785 = das::das_call_member< bool (UEnum::*)(const wchar_t *,int) const,&UEnum::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2268:23
	makeExtern<DAS_CALL_METHOD(_method_785), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (UEnum::*)(const wchar_t *,int) const , &UEnum::HasMetaData >::invoke")
		->args({"self","Key","NameIndex"})
		->arg_init(2,make_smart<ExprConstInt>(-1))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_786 = das::das_call_member< FString (UEnum::*)(const wchar_t *,int,bool) const,&UEnum::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2279:26
	makeExtern<DAS_CALL_METHOD(_method_786), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetMetaData","das_call_member< FString (UEnum::*)(const wchar_t *,int,bool) const , &UEnum::GetMetaData >::invoke")
		->args({"self","Key","NameIndex","bAllowRemap"})
		->arg_init(2,make_smart<ExprConstInt>(-1))
		->arg_init(3,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_787 = das::das_call_member< void (UEnum::*)(const wchar_t *,const wchar_t *,int) const,&UEnum::SetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2289:23
	makeExtern<DAS_CALL_METHOD(_method_787), SimNode_ExtFuncCall >(lib,"SetMetaData","das_call_member< void (UEnum::*)(const wchar_t *,const wchar_t *,int) const , &UEnum::SetMetaData >::invoke")
		->args({"self","Key","InValue","NameIndex"})
		->arg_init(3,make_smart<ExprConstInt>(-1))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_788 = das::das_call_member< void (UEnum::*)(const wchar_t *,int) const,&UEnum::RemoveMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2298:23
	makeExtern<DAS_CALL_METHOD(_method_788), SimNode_ExtFuncCall >(lib,"RemoveMetaData","das_call_member< void (UEnum::*)(const wchar_t *,int) const , &UEnum::RemoveMetaData >::invoke")
		->args({"self","Key","NameIndex"})
		->arg_init(2,make_smart<ExprConstInt>(-1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2511:31
	makeExtern< void (*)(UEnum &,FArchive &) , _dasUnreal_virtual_277_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_277_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2512:31
	makeExtern< void (*)(UEnum &) , _dasUnreal_virtual_278_BeginDestroy , SimNode_ExtFuncCall >(lib,"BeginDestroy","_dasUnreal_virtual_278_BeginDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2648:15
	makeExtern< bool (*)(const FCppClassTypeInfo &) , _dasUnreal_virtual_279_IsAbstract , SimNode_ExtFuncCall >(lib,"IsAbstract","_dasUnreal_virtual_279_IsAbstract")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FImplementedInterface>(*this,lib,"FImplementedInterface","FImplementedInterface");
	addCtorAndUsing<FImplementedInterface,UClass *,int,bool>(*this,lib,"FImplementedInterface","FImplementedInterface")
		->args({"InClass","InOffset","InImplementedByK2"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2728:2
	makeExtern< UClass * (*)(const UClass *) , _dasUnreal_static_280_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_280_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UClass*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2728:2
	makeExtern< const wchar_t * (*)(const UClass *) , _dasUnreal_static_281_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_281_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UClass*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2728:2
	makeExtern< void (*)(const UClass *) , _dasUnreal_static_282_StaticRegisterNativesUClass , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUClass","_dasUnreal_static_282_StaticRegisterNativesUClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UClass*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_795 = das::das_call_member< UPackage * (UClass::*)() const,&UClass::GetOuterUPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2729:2
	makeExtern<DAS_CALL_METHOD(_method_795), SimNode_ExtFuncCall >(lib,"GetOuterUPackage","das_call_member< UPackage * (UClass::*)() const , &UClass::GetOuterUPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_796 = das::das_call_member< void (UClass::*)(),&UClass::DestroyPropertiesPendingDestruction >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2772:23
	makeExtern<DAS_CALL_METHOD(_method_796), SimNode_ExtFuncCall >(lib,"DestroyPropertiesPendingDestruction","das_call_member< void (UClass::*)() , &UClass::DestroyPropertiesPendingDestruction >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2780:15
	makeExtern< void (*)(UClass &,FUObjectSerializeContext *) , _dasUnreal_virtual_283_ConditionalRecompileClass , SimNode_ExtFuncCall >(lib,"ConditionalRecompileClass","_dasUnreal_virtual_283_ConditionalRecompileClass")
		->args({"self","InLoadContext"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2781:15
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_284_FlushCompilationQueueForLevel , SimNode_ExtFuncCall >(lib,"FlushCompilationQueueForLevel","_dasUnreal_virtual_284_FlushCompilationQueueForLevel")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_799 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsFunctionHidden >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2796:23
	makeExtern<DAS_CALL_METHOD(_method_799), SimNode_ExtFuncCall >(lib,"IsFunctionHidden","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsFunctionHidden >::invoke")
		->args({"self","InFunction"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_800 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsAutoExpandCategory >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2798:23
	makeExtern<DAS_CALL_METHOD(_method_800), SimNode_ExtFuncCall >(lib,"IsAutoExpandCategory","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsAutoExpandCategory >::invoke")
		->args({"self","InCategory"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_801 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsPrioritizeCategory >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2800:23
	makeExtern<DAS_CALL_METHOD(_method_801), SimNode_ExtFuncCall >(lib,"IsPrioritizeCategory","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsPrioritizeCategory >::invoke")
		->args({"self","InCategory"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_802 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsAutoCollapseCategory >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2802:23
	makeExtern<DAS_CALL_METHOD(_method_802), SimNode_ExtFuncCall >(lib,"IsAutoCollapseCategory","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsAutoCollapseCategory >::invoke")
		->args({"self","InCategory"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_803 = das::das_call_member< bool (UClass::*)(const wchar_t *) const,&UClass::IsClassGroupName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2804:23
	makeExtern<DAS_CALL_METHOD(_method_803), SimNode_ExtFuncCall >(lib,"IsClassGroupName","das_call_member< bool (UClass::*)(const wchar_t *) const , &UClass::IsClassGroupName >::invoke")
		->args({"self","InGroupName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_804 = das::das_call_member< void (UClass::*)(UObject *,FReferenceCollector &) const,&UClass::CallAddReferencedObjects >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2812:19
	makeExtern<DAS_CALL_METHOD(_method_804), SimNode_ExtFuncCall >(lib,"CallAddReferencedObjects","das_call_member< void (UClass::*)(UObject *,FReferenceCollector &) const , &UClass::CallAddReferencedObjects >::invoke")
		->args({"self","This","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_805 = das::das_call_member< void * (UClass::*)(),&UClass::GetOrCreateSparseClassData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2864:8
	makeExtern<DAS_CALL_METHOD(_method_805), SimNode_ExtFuncCall >(lib,"GetOrCreateSparseClassData","das_call_member< void * (UClass::*)() , &UClass::GetOrCreateSparseClassData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_806 = das::das_call_member< UScriptStruct * (UClass::*)() const,&UClass::GetSparseClassDataStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2869:33
	makeExtern<DAS_CALL_METHOD(_method_806), SimNode_ExtFuncCall >(lib,"GetSparseClassDataStruct","das_call_member< UScriptStruct * (UClass::*)() const , &UClass::GetSparseClassDataStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_807 = das::das_call_member< void (UClass::*)(UScriptStruct *),&UClass::SetSparseClassDataStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2871:23
	makeExtern<DAS_CALL_METHOD(_method_807), SimNode_ExtFuncCall >(lib,"SetSparseClassDataStruct","das_call_member< void (UClass::*)(UScriptStruct *) , &UClass::SetSparseClassDataStruct >::invoke")
		->args({"self","InSparseClassDataStruct"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_808 = das::das_call_member< void (UClass::*)(bool),&UClass::ClearSparseClassDataStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2877:23
	makeExtern<DAS_CALL_METHOD(_method_808), SimNode_ExtFuncCall >(lib,"ClearSparseClassDataStruct","das_call_member< void (UClass::*)(bool) , &UClass::ClearSparseClassDataStruct >::invoke")
		->args({"self","bInRecomplingOnLoad"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2880:30
	makeExtern< void (*)(const UClass *) , _dasUnreal_static_285_AssembleReferenceTokenStreams , SimNode_ExtFuncCall >(lib,"AssembleReferenceTokenStreams","_dasUnreal_static_285_AssembleReferenceTokenStreams")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UClass*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2976:34
	makeExtern< UClass * (*)(UClass &) , _dasUnreal_virtual_286_GetAuthoritativeClass , SimNode_ExtFuncCall >(lib,"GetAuthoritativeClass","_dasUnreal_virtual_286_GetAuthoritativeClass")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_811 = das::das_call_member< const UClass * (UClass::*)() const,&UClass::GetAuthoritativeClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2977:16
	makeExtern<DAS_CALL_METHOD(_method_811), SimNode_ExtFuncCall >(lib,"GetAuthoritativeClass","das_call_member< const UClass * (UClass::*)() const , &UClass::GetAuthoritativeClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_812 = das::das_call_member< void (UClass::*)(UFunction *,FName),&UClass::AddFunctionToFunctionMap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2995:7
	makeExtern<DAS_CALL_METHOD(_method_812), SimNode_ExtFuncCall >(lib,"AddFunctionToFunctionMap","das_call_member< void (UClass::*)(UFunction *,FName) , &UClass::AddFunctionToFunctionMap >::invoke")
		->args({"self","Function","FuncName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_813 = das::das_call_member< void (UClass::*)(UFunction *),&UClass::RemoveFunctionFromFunctionMap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3011:7
	makeExtern<DAS_CALL_METHOD(_method_813), SimNode_ExtFuncCall >(lib,"RemoveFunctionFromFunctionMap","das_call_member< void (UClass::*)(UFunction *) , &UClass::RemoveFunctionFromFunctionMap >::invoke")
		->args({"self","Function"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_814 = das::das_call_member< void (UClass::*)(),&UClass::ClearFunctionMapsCaches >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3025:23
	makeExtern<DAS_CALL_METHOD(_method_814), SimNode_ExtFuncCall >(lib,"ClearFunctionMapsCaches","das_call_member< void (UClass::*)() , &UClass::ClearFunctionMapsCaches >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_815 = das::das_call_member< UFunction * (UClass::*)(FName,EIncludeSuperFlag::Type) const,&UClass::FindFunctionByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3028:29
	makeExtern<DAS_CALL_METHOD(_method_815), SimNode_ExtFuncCall >(lib,"FindFunctionByName","das_call_member< UFunction * (UClass::*)(FName,EIncludeSuperFlag::Type) const , &UClass::FindFunctionByName >::invoke")
		->args({"self","InName","IncludeSuper"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<EIncludeSuperFlag::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3031:31
	makeExtern< void (*)(UClass &,FArchive &) , _dasUnreal_virtual_287_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_287_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3032:31
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_288_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_288_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3033:31
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_289_FinishDestroy , SimNode_ExtFuncCall >(lib,"FinishDestroy","_dasUnreal_virtual_289_FinishDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3034:31
	makeExtern< void (*)(UClass &,UClass *,const wchar_t *,const wchar_t *) , _dasUnreal_virtual_290_DeferredRegister , SimNode_ExtFuncCall >(lib,"DeferredRegister","_dasUnreal_virtual_290_DeferredRegister")
		->args({"self","UClassStaticClass","PackageName","InName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3035:31
	makeExtern< bool (*)(UClass &,const wchar_t *,UObject *,unsigned int) , _dasUnreal_virtual_291_Rename , SimNode_ExtFuncCall >(lib,"Rename","_dasUnreal_virtual_291_Rename")
		->args({"self","NewName","NewOuter","Flags"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->arg_init(2,make_smart<ExprConstPtr>())
		->arg_init(3,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3036:31
	makeExtern< void (*)(UClass &,EObjectFlags) , _dasUnreal_virtual_292_TagSubobjects , SimNode_ExtFuncCall >(lib,"TagSubobjects","_dasUnreal_virtual_292_TagSubobjects")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3037:31
	makeExtern< void (*)(UClass &) , _dasUnreal_virtual_293_PostInitProperties , SimNode_ExtFuncCall >(lib,"PostInitProperties","_dasUnreal_virtual_293_PostInitProperties")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3038:30
	makeExtern< void (*)(const UClass *,UObject *,FReferenceCollector &) , _dasUnreal_static_294_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_static_294_AddReferencedObjects")
		->args({"self","InThis","Collector"})
		->arg_type(0,makeType<TExplicit<const UClass*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3039:55
	makeExtern< FRestoreForUObjectOverwrite * (*)(UClass &) , _dasUnreal_virtual_295_GetRestoreForUObjectOverwrite , SimNode_ExtFuncCall >(lib,"GetRestoreForUObjectOverwrite","_dasUnreal_virtual_295_GetRestoreForUObjectOverwrite")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3040:34
	makeExtern< FString (*)(UClass &) , _dasUnreal_virtual_296_GetDesc , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDesc","_dasUnreal_virtual_296_GetDesc")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3045:15
	makeExtern< bool (*)(const UClass &) , _dasUnreal_virtual_297_IsAsset , SimNode_ExtFuncCall >(lib,"IsAsset","_dasUnreal_virtual_297_IsAsset")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

