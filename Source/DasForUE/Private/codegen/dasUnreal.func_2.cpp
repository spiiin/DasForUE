// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_2() {
	using _method_97 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsCooking >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:603:19
	makeExtern<DAS_CALL_METHOD(_method_97), SimNode_ExtFuncCall >(lib,"IsCooking","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsCooking >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_98 = das::das_call_member< FArchiveCookContext * (FArchiveState::*)(),&FArchiveState::GetCookContext >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:629:35
	makeExtern<DAS_CALL_METHOD(_method_98), SimNode_ExtFuncCall >(lib,"GetCookContext","das_call_member< FArchiveCookContext * (FArchiveState::*)() , &FArchiveState::GetCookContext >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:653:15
	makeExtern< bool (*)(const FArchiveState &) , _dasUnreal_virtual_23_UseToResolveEnumerators , SimNode_ExtFuncCall >(lib,"UseToResolveEnumerators","_dasUnreal_virtual_23_UseToResolveEnumerators")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:661:15
	makeExtern< bool (*)(const FArchiveState &,const FProperty *) , _dasUnreal_virtual_24_ShouldSkipProperty , SimNode_ExtFuncCall >(lib,"ShouldSkipProperty","_dasUnreal_virtual_24_ShouldSkipProperty")
		->args({"self","InProperty"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:672:15
	makeExtern< void (*)(FArchiveState &,FProperty *) , _dasUnreal_virtual_25_SetSerializedProperty , SimNode_ExtFuncCall >(lib,"SetSerializedProperty","_dasUnreal_virtual_25_SetSerializedProperty")
		->args({"self","InProperty"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_102 = das::das_call_member< FProperty * (FArchiveState::*)() const,&FArchiveState::GetSerializedProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:682:31
	makeExtern<DAS_CALL_METHOD(_method_102), SimNode_ExtFuncCall >(lib,"GetSerializedProperty","das_call_member< FProperty * (FArchiveState::*)() const , &FArchiveState::GetSerializedProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_103 = das::das_call_member< const FArchiveSerializedPropertyChain * (FArchiveState::*)() const,&FArchiveState::GetSerializedPropertyChain >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:697:53
	makeExtern<DAS_CALL_METHOD(_method_103), SimNode_ExtFuncCall >(lib,"GetSerializedPropertyChain","das_call_member< const FArchiveSerializedPropertyChain * (FArchiveState::*)() const , &FArchiveState::GetSerializedPropertyChain >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:705:24
	makeExtern< void (*)(FArchiveState &,const FArchiveSerializedPropertyChain *,FProperty *) , _dasUnreal_virtual_26_SetSerializedPropertyChain , SimNode_ExtFuncCall >(lib,"SetSerializedPropertyChain","_dasUnreal_virtual_26_SetSerializedPropertyChain")
		->args({"self","InSerializedPropertyChain","InSerializedPropertyOverride"})
		->arg_init(2,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:709:24
	makeExtern< bool (*)(const FArchiveState &) , _dasUnreal_virtual_27_IsEditorOnlyPropertyOnTheStack , SimNode_ExtFuncCall >(lib,"IsEditorOnlyPropertyOnTheStack","_dasUnreal_virtual_27_IsEditorOnlyPropertyOnTheStack")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:713:15
	makeExtern< void (*)(FArchiveState &,FUObjectSerializeContext *) , _dasUnreal_virtual_28_SetSerializeContext , SimNode_ExtFuncCall >(lib,"SetSerializeContext","_dasUnreal_virtual_28_SetSerializeContext")
		->args({"self","InLoadContext"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:716:36
	makeExtern< FUObjectSerializeContext * (*)(FArchiveState &) , _dasUnreal_virtual_29_GetSerializeContext , SimNode_ExtFuncCall >(lib,"GetSerializeContext","_dasUnreal_virtual_29_GetSerializeContext")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:723:24
	makeExtern< void (*)(FArchiveState &,const FString &) , _dasUnreal_virtual_30_SetLocalizationNamespace , SimNode_ExtFuncCall >(lib,"SetLocalizationNamespace","_dasUnreal_virtual_30_SetLocalizationNamespace")
		->args({"self","InLocalizationNamespace"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:729:27
	makeExtern< FString (*)(const FArchiveState &) , _dasUnreal_virtual_31_GetLocalizationNamespace , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetLocalizationNamespace","_dasUnreal_virtual_31_GetLocalizationNamespace")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:733:24
	makeExtern< void (*)(FArchiveState &) , _dasUnreal_virtual_32_Reset , SimNode_ExtFuncCall >(lib,"Reset","_dasUnreal_virtual_32_Reset")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FArchiveState::FFastPathLoadBuffer>(*this,lib,"FFastPathLoadBuffer","FArchiveState::FFastPathLoadBuffer");
	using _method_111 = das::das_call_member< void (FArchiveState::FFastPathLoadBuffer::*)(),&FArchiveState::FFastPathLoadBuffer::Reset >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:746:20
	makeExtern<DAS_CALL_METHOD(_method_111), SimNode_ExtFuncCall >(lib,"Reset","das_call_member< void (FArchiveState::FFastPathLoadBuffer::*)() , &FArchiveState::FFastPathLoadBuffer::Reset >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:878:24
	makeExtern< void (*)(FArchiveState &,bool) , _dasUnreal_virtual_33_SetIsLoading , SimNode_ExtFuncCall >(lib,"SetIsLoading","_dasUnreal_virtual_33_SetIsLoading")
		->args({"self","bInIsLoading"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:884:24
	makeExtern< void (*)(FArchiveState &,bool) , _dasUnreal_virtual_34_SetIsLoadingFromCookedPackage , SimNode_ExtFuncCall >(lib,"SetIsLoadingFromCookedPackage","_dasUnreal_virtual_34_SetIsLoadingFromCookedPackage")
		->args({"self","bInIsLoadingFromCookedPackage"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:891:24
	makeExtern< void (*)(FArchiveState &,bool) , _dasUnreal_virtual_35_SetIsSaving , SimNode_ExtFuncCall >(lib,"SetIsSaving","_dasUnreal_virtual_35_SetIsSaving")
		->args({"self","bInIsSaving"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:898:24
	makeExtern< void (*)(FArchiveState &,bool) , _dasUnreal_virtual_36_SetIsTransacting , SimNode_ExtFuncCall >(lib,"SetIsTransacting","_dasUnreal_virtual_36_SetIsTransacting")
		->args({"self","bInIsTransacting"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:905:24
	makeExtern< void (*)(FArchiveState &,bool) , _dasUnreal_virtual_37_SetIsTextFormat , SimNode_ExtFuncCall >(lib,"SetIsTextFormat","_dasUnreal_virtual_37_SetIsTextFormat")
		->args({"self","bInIsTextFormat"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:912:24
	makeExtern< void (*)(FArchiveState &,bool) , _dasUnreal_virtual_38_SetWantBinaryPropertySerialization , SimNode_ExtFuncCall >(lib,"SetWantBinaryPropertySerialization","_dasUnreal_virtual_38_SetWantBinaryPropertySerialization")
		->args({"self","bInWantBinaryPropertySerialization"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:915:24
	makeExtern< void (*)(FArchiveState &,bool) , _dasUnreal_virtual_39_SetUseUnversionedPropertySerialization , SimNode_ExtFuncCall >(lib,"SetUseUnversionedPropertySerialization","_dasUnreal_virtual_39_SetUseUnversionedPropertySerialization")
		->args({"self","bInUseUnversioned"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:923:24
	makeExtern< void (*)(FArchiveState &,bool) , _dasUnreal_virtual_40_SetForceUnicode , SimNode_ExtFuncCall >(lib,"SetForceUnicode","_dasUnreal_virtual_40_SetForceUnicode")
		->args({"self","bInForceUnicode"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:930:24
	makeExtern< void (*)(FArchiveState &,bool) , _dasUnreal_virtual_41_SetIsPersistent , SimNode_ExtFuncCall >(lib,"SetIsPersistent","_dasUnreal_virtual_41_SetIsPersistent")
		->args({"self","bInIsPersistent"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_121 = das::das_call_member< void (FArchiveState::*)(int),&FArchiveState::SetUE4Ver >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:941:19
	makeExtern<DAS_CALL_METHOD(_method_121), SimNode_ExtFuncCall >(lib,"SetUE4Ver","das_call_member< void (FArchiveState::*)(int) , &FArchiveState::SetUE4Ver >::invoke")
		->args({"self","InVer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:953:24
	makeExtern< void (*)(FArchiveState &,int) , _dasUnreal_virtual_42_SetLicenseeUEVer , SimNode_ExtFuncCall >(lib,"SetLicenseeUEVer","_dasUnreal_virtual_42_SetLicenseeUEVer")
		->args({"self","InVer"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_123 = das::das_call_member< void (FArchiveState::*)(int),&FArchiveState::SetLicenseeUE4Ver >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:956:19
	makeExtern<DAS_CALL_METHOD(_method_123), SimNode_ExtFuncCall >(lib,"SetLicenseeUE4Ver","das_call_member< void (FArchiveState::*)(int) , &FArchiveState::SetLicenseeUE4Ver >::invoke")
		->args({"self","InVer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:967:24
	makeExtern< void (*)(FArchiveState &,const FEngineVersionBase &) , _dasUnreal_virtual_43_SetEngineVer , SimNode_ExtFuncCall >(lib,"SetEngineVer","_dasUnreal_virtual_43_SetEngineVer")
		->args({"self","InVer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:972:24
	makeExtern< void (*)(FArchiveState &,const unsigned int) , _dasUnreal_virtual_44_SetEngineNetVer , SimNode_ExtFuncCall >(lib,"SetEngineNetVer","_dasUnreal_virtual_44_SetEngineNetVer")
		->args({"self","InEngineNetVer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:977:24
	makeExtern< void (*)(FArchiveState &,const unsigned int) , _dasUnreal_virtual_45_SetGameNetVer , SimNode_ExtFuncCall >(lib,"SetGameNetVer","_dasUnreal_virtual_45_SetGameNetVer")
		->args({"self","InGameNetVer"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FArchive>(*this,lib,"FArchive","FArchive");
	addCtorAndUsing<FArchive,const FArchive &>(*this,lib,"FArchive","FArchive")
		->args({""});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1291:15
	makeExtern< void (*)(FArchive &) , _dasUnreal_virtual_46_ForceBlueprintFinalization , SimNode_ExtFuncCall >(lib,"ForceBlueprintFinalization","_dasUnreal_virtual_46_ForceBlueprintFinalization")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1579:15
	makeExtern< void (*)(FArchive &,void *,int64) , _dasUnreal_virtual_47_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_47_Serialize")
		->args({"self","V","Length"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1590:15
	makeExtern< void (*)(FArchive &,void *,int64) , _dasUnreal_virtual_48_SerializeBits , SimNode_ExtFuncCall >(lib,"SerializeBits","_dasUnreal_virtual_48_SerializeBits")
		->args({"self","V","LengthBits"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1600:15
	makeExtern< void (*)(FArchive &,unsigned int &,unsigned int) , _dasUnreal_virtual_49_SerializeInt , SimNode_ExtFuncCall >(lib,"SerializeInt","_dasUnreal_virtual_49_SerializeInt")
		->args({"self","Value","Max"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1606:24
	makeExtern< void (*)(FArchive &,unsigned int &) , _dasUnreal_virtual_50_SerializeIntPacked , SimNode_ExtFuncCall >(lib,"SerializeIntPacked","_dasUnreal_virtual_50_SerializeIntPacked")
		->args({"self","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1607:24
	makeExtern< void (*)(FArchive &,unsigned long long &) , _dasUnreal_virtual_51_SerializeIntPacked64 , SimNode_ExtFuncCall >(lib,"SerializeIntPacked64","_dasUnreal_virtual_51_SerializeIntPacked64")
		->args({"self","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1610:15
	makeExtern< void (*)(FArchive &,UObject *) , _dasUnreal_virtual_52_Preload , SimNode_ExtFuncCall >(lib,"Preload","_dasUnreal_virtual_52_Preload")
		->args({"self","Object"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_134 = das::das_call_member< FArchiveState & (FArchive::*)(),&FArchive::GetArchiveState >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1613:17
	makeExtern<DAS_CALL_METHOD(_method_134), SimNode_ExtFuncCallRef >(lib,"GetArchiveState","das_call_member< FArchiveState & (FArchive::*)() , &FArchive::GetArchiveState >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_135 = das::das_call_member< const FArchiveState & (FArchive::*)() const,&FArchive::GetArchiveState >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1618:23
	makeExtern<DAS_CALL_METHOD(_method_135), SimNode_ExtFuncCallRef >(lib,"GetArchiveState","das_call_member< const FArchiveState & (FArchive::*)() const , &FArchive::GetArchiveState >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1636:15
	makeExtern< void (*)(FArchive &,int64) , _dasUnreal_virtual_53_Seek , SimNode_ExtFuncCall >(lib,"Seek","_dasUnreal_virtual_53_Seek")
		->args({"self","InPos"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1645:15
	makeExtern< void (*)(FArchive &,UE::Serialization::FEditorBulkData *) , _dasUnreal_virtual_54_AttachBulkData , SimNode_ExtFuncCall >(lib,"AttachBulkData","_dasUnreal_virtual_54_AttachBulkData")
		->args({"self","BulkData"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1654:15
	makeExtern< void (*)(FArchive &,UE::Serialization::FEditorBulkData *,bool) , _dasUnreal_virtual_55_DetachBulkData , SimNode_ExtFuncCall >(lib,"DetachBulkData","_dasUnreal_virtual_55_DetachBulkData")
		->args({"self","BulkData","bEnsureBulkDataIsLoaded"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1677:15
	makeExtern< bool (*)(const FArchive &,FArchive *) , _dasUnreal_virtual_56_IsProxyOf , SimNode_ExtFuncCall >(lib,"IsProxyOf","_dasUnreal_virtual_56_IsProxyOf")
		->args({"self","InOther"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1697:15
	makeExtern< bool (*)(FArchive &,int64,int64) , _dasUnreal_virtual_57_Precache , SimNode_ExtFuncCall >(lib,"Precache","_dasUnreal_virtual_57_Precache")
		->args({"self","PrecacheOffset","PrecacheSize"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1705:15
	makeExtern< void (*)(FArchive &) , _dasUnreal_virtual_58_FlushCache , SimNode_ExtFuncCall >(lib,"FlushCache","_dasUnreal_virtual_58_FlushCache")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1725:15
	makeExtern< void (*)(FArchive &) , _dasUnreal_virtual_59_Flush , SimNode_ExtFuncCall >(lib,"Flush","_dasUnreal_virtual_59_Flush")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1730:15
	makeExtern< bool (*)(FArchive &) , _dasUnreal_virtual_60_Close , SimNode_ExtFuncCall >(lib,"Close","_dasUnreal_virtual_60_Close")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_144 = das::das_call_member< void (FArchive::*)(void *,int64,FName,ECompressionFlags,bool),&FArchive::SerializeCompressed >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1747:16
	makeExtern<DAS_CALL_METHOD(_method_144), SimNode_ExtFuncCall >(lib,"SerializeCompressed","das_call_member< void (FArchive::*)(void *,int64,FName,ECompressionFlags,bool) , &FArchive::SerializeCompressed >::invoke")
		->args({"self","V","Length","CompressionFormatCannotChange","Flags","bTreatBufferAsFileReader"})
		->arg_init(4,make_smart<ExprConstEnumeration>(0,makeType<ECompressionFlags>(lib)))
		->arg_init(5,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_145 = das::das_call_member< void (FArchive::*)(void *,int64,FName,FName,ECompressionFlags,bool,long long *),&FArchive::SerializeCompressedNew >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1767:16
	makeExtern<DAS_CALL_METHOD(_method_145), SimNode_ExtFuncCall >(lib,"SerializeCompressedNew","das_call_member< void (FArchive::*)(void *,int64,FName,FName,ECompressionFlags,bool,long long *) , &FArchive::SerializeCompressedNew >::invoke")
		->args({"self","V","Length","CompressionFormatToEncode","CompressionFormatToDecodeOldV1Files","Flags","bTreatBufferAsFileReader","OutPartialReadLength"})
		->arg_init(5,make_smart<ExprConstEnumeration>(0,makeType<ECompressionFlags>(lib)))
		->arg_init(6,make_smart<ExprConstBool>(false))
		->arg_init(7,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_146 = das::das_call_member< void (FArchive::*)(void *,int64),&FArchive::SerializeCompressedNew >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1779:16
	makeExtern<DAS_CALL_METHOD(_method_146), SimNode_ExtFuncCall >(lib,"SerializeCompressedNew","das_call_member< void (FArchive::*)(void *,int64) , &FArchive::SerializeCompressedNew >::invoke")
		->args({"self","V","Length"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_147 = das::das_call_member< void (FArchive::*)(void *,int),&FArchive::ByteSwap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1785:16
	makeExtern<DAS_CALL_METHOD(_method_147), SimNode_ExtFuncCall >(lib,"ByteSwap","das_call_member< void (FArchive::*)(void *,int) , &FArchive::ByteSwap >::invoke")
		->args({"self","V","Length"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_148 = das::das_call_member< FArchive & (FArchive::*)(void *,int),&FArchive::ByteOrderSerialize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1788:24
	makeExtern<DAS_CALL_METHOD(_method_148), SimNode_ExtFuncCallRef >(lib,"ByteOrderSerialize","das_call_member< FArchive & (FArchive::*)(void *,int) , &FArchive::ByteOrderSerialize >::invoke")
		->args({"self","V","Length"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_149 = das::das_call_member< void (FArchive::*)(),&FArchive::StartSerializingDefaults >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1803:7
	makeExtern<DAS_CALL_METHOD(_method_149), SimNode_ExtFuncCall >(lib,"StartSerializingDefaults","das_call_member< void (FArchive::*)() , &FArchive::StartSerializingDefaults >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_150 = das::das_call_member< void (FArchive::*)(),&FArchive::StopSerializingDefaults >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1809:7
	makeExtern<DAS_CALL_METHOD(_method_150), SimNode_ExtFuncCall >(lib,"StopSerializingDefaults","das_call_member< void (FArchive::*)() , &FArchive::StopSerializingDefaults >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1817:15
	makeExtern< void (*)(FArchive &,const UObject *) , _dasUnreal_virtual_61_MarkScriptSerializationStart , SimNode_ExtFuncCall >(lib,"MarkScriptSerializationStart","_dasUnreal_virtual_61_MarkScriptSerializationStart")
		->args({"self","Obj"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1822:15
	makeExtern< void (*)(FArchive &,const UObject *) , _dasUnreal_virtual_62_MarkScriptSerializationEnd , SimNode_ExtFuncCall >(lib,"MarkScriptSerializationEnd","_dasUnreal_virtual_62_MarkScriptSerializationEnd")
		->args({"self","Obj"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1868:20
	makeExtern< FArchive * (*)(FArchive &) , _dasUnreal_virtual_63_GetCacheableArchive , SimNode_ExtFuncCall >(lib,"GetCacheableArchive","_dasUnreal_virtual_63_GetCacheableArchive")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1935:24
	makeExtern< void (*)(FArchive &,FProperty *,const bool) , _dasUnreal_virtual_64_PushSerializedProperty , SimNode_ExtFuncCall >(lib,"PushSerializedProperty","_dasUnreal_virtual_64_PushSerializedProperty")
		->args({"self","InProperty","bIsEditorOnlyProperty"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1943:24
	makeExtern< void (*)(FArchive &,FProperty *,const bool) , _dasUnreal_virtual_65_PopSerializedProperty , SimNode_ExtFuncCall >(lib,"PopSerializedProperty","_dasUnreal_virtual_65_PopSerializedProperty")
		->args({"self","InProperty","bIsEditorOnlyProperty"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FArchive::FScopeSetDebugSerializationFlags,FArchive &,unsigned int,bool>(*this,lib,"FScopeSetDebugSerializationFlags","FArchive::FScopeSetDebugSerializationFlags")
		->args({"InAr","NewFlags","Remove"})
		->arg_init(2,make_smart<ExprConstBool>(false));
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:2161:24
	makeExtern< void (*)(FArchive &,const FName &) , _dasUnreal_virtual_66_PushDebugDataString , SimNode_ExtFuncCall >(lib,"PushDebugDataString","_dasUnreal_virtual_66_PushDebugDataString")
		->args({"self","DebugData"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:2162:15
	makeExtern< void (*)(FArchive &) , _dasUnreal_virtual_67_PopDebugDataString , SimNode_ExtFuncCall >(lib,"PopDebugDataString","_dasUnreal_virtual_67_PopDebugDataString")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FArchive::FScopeAddDebugData,FArchive &,const FName &>(*this,lib,"FScopeAddDebugData","FArchive::FScopeAddDebugData")
		->args({"InAr","DebugData"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:2203:15
	makeExtern< void (*)(FArchive &,EFileRegionType) , _dasUnreal_virtual_68_PushFileRegionType , SimNode_ExtFuncCall >(lib,"PushFileRegionType","_dasUnreal_virtual_68_PushFileRegionType")
		->args({"self","Type"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:2204:15
	makeExtern< void (*)(FArchive &) , _dasUnreal_virtual_69_PopFileRegionType , SimNode_ExtFuncCall >(lib,"PopFileRegionType","_dasUnreal_virtual_69_PopFileRegionType")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:41:14
	makeExtern< wchar_t * (*)(FString &) , GetData , SimNode_ExtFuncCall >(lib,"GetData","GetData")
		->args({""})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:42:14
	makeExtern< const wchar_t * (*)(const FString &) , GetData , SimNode_ExtFuncCall >(lib,"GetData","GetData")
		->args({""})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:43:14
	makeExtern< int (*)(const FString &) , GetNum , SimNode_ExtFuncCall >(lib,"GetNum","GetNum")
		->args({"String"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FString>(*this,lib,"FString","FString");
	addCtorAndUsing<FString,const FString &>(*this,lib,"FString","FString")
		->args({""});
	addCtorAndUsing<FString,const FString &,int>(*this,lib,"FString","FString")
		->args({"Other","ExtraSlack"});
	addCtorAndUsing<FString,const char *>(*this,lib,"FString","FString")
		->args({"Str"});
	addCtorAndUsing<FString,int,const char *>(*this,lib,"FString","FString")
		->args({"Len","Str"});
	addCtorAndUsing<FString,const char *,int>(*this,lib,"FString","FString")
		->args({"Str","ExtraSlack"});
	using _method_160 = das::das_call_member< SIZE_T (FString::*)() const,&FString::GetAllocatedSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:266:21
	makeExtern<DAS_CALL_METHOD(_method_160), SimNode_ExtFuncCall >(lib,"GetAllocatedSize","das_call_member< SIZE_T (FString::*)() const , &FString::GetAllocatedSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_161 = das::das_call_member< void (FString::*)() const,&FString::CheckInvariants >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:274:19
	makeExtern<DAS_CALL_METHOD(_method_161), SimNode_ExtFuncCall >(lib,"CheckInvariants","das_call_member< void (FString::*)() const , &FString::CheckInvariants >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_162 = das::das_call_member< void (FString::*)(),&FString::Empty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:287:16
	makeExtern<DAS_CALL_METHOD(_method_162), SimNode_ExtFuncCall >(lib,"Empty","das_call_member< void (FString::*)() , &FString::Empty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_163 = das::das_call_member< void (FString::*)(int),&FString::Empty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:288:16
	makeExtern<DAS_CALL_METHOD(_method_163), SimNode_ExtFuncCall >(lib,"Empty","das_call_member< void (FString::*)(int) , &FString::Empty >::invoke")
		->args({"self","Slack"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_164 = das::das_call_member< bool (FString::*)() const,&FString::IsEmpty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:295:32
	makeExtern<DAS_CALL_METHOD(_method_164), SimNode_ExtFuncCall >(lib,"IsEmpty","das_call_member< bool (FString::*)() const , &FString::IsEmpty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_165 = das::das_call_member< void (FString::*)(int),&FString::Reset >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:305:16
	makeExtern<DAS_CALL_METHOD(_method_165), SimNode_ExtFuncCall >(lib,"Reset","das_call_member< void (FString::*)(int) , &FString::Reset >::invoke")
		->args({"self","NewReservedSize"})
		->arg_init(1,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_166 = das::das_call_member< void (FString::*)(),&FString::Shrink >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:310:16
	makeExtern<DAS_CALL_METHOD(_method_166), SimNode_ExtFuncCall >(lib,"Shrink","das_call_member< void (FString::*)() , &FString::Shrink >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_167 = das::das_call_member< bool (FString::*)(int) const,&FString::IsValidIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:319:32
	makeExtern<DAS_CALL_METHOD(_method_167), SimNode_ExtFuncCall >(lib,"IsValidIndex","das_call_member< bool (FString::*)(int) const , &FString::IsValidIndex >::invoke")
		->args({"self","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_168 = das::das_call_member< void (FString::*)(const char *,int),&FString::AppendChars >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:361:16
	makeExtern<DAS_CALL_METHOD(_method_168), SimNode_ExtFuncCall >(lib,"AppendChars","das_call_member< void (FString::*)(const char *,int) , &FString::AppendChars >::invoke")
		->args({"self","Str","Count"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_169 = das::das_call_member< FString & (FString::*)(wchar_t),&FString::AppendChar >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:398:20
	makeExtern<DAS_CALL_METHOD(_method_169), SimNode_ExtFuncCallRef >(lib,"AppendChar","das_call_member< FString & (FString::*)(wchar_t) , &FString::AppendChar >::invoke")
		->args({"self","InChar"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_170 = das::das_call_member< void (FString::*)(int,wchar_t),&FString::InsertAt >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:425:16
	makeExtern<DAS_CALL_METHOD(_method_170), SimNode_ExtFuncCall >(lib,"InsertAt","das_call_member< void (FString::*)(int,wchar_t) , &FString::InsertAt >::invoke")
		->args({"self","Index","Character"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_171 = das::das_call_member< void (FString::*)(int,const FString &),&FString::InsertAt >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:426:16
	makeExtern<DAS_CALL_METHOD(_method_171), SimNode_ExtFuncCall >(lib,"InsertAt","das_call_member< void (FString::*)(int,const FString &) , &FString::InsertAt >::invoke")
		->args({"self","Index","Characters"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_172 = das::das_call_member< void (FString::*)(int,int,bool),&FString::RemoveAt >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:435:16
	makeExtern<DAS_CALL_METHOD(_method_172), SimNode_ExtFuncCall >(lib,"RemoveAt","das_call_member< void (FString::*)(int,int,bool) , &FString::RemoveAt >::invoke")
		->args({"self","Index","Count","bAllowShrinking"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->arg_init(3,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_173 = das::das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type),&FString::RemoveFromStart >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:456:7
	makeExtern<DAS_CALL_METHOD(_method_173), SimNode_ExtFuncCall >(lib,"RemoveFromStart","das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) , &FString::RemoveFromStart >::invoke")
		->args({"self","InPrefix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_174 = das::das_call_member< bool (FString::*)(const FString &,ESearchCase::Type),&FString::RemoveFromStart >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:467:7
	makeExtern<DAS_CALL_METHOD(_method_174), SimNode_ExtFuncCall >(lib,"RemoveFromStart","das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) , &FString::RemoveFromStart >::invoke")
		->args({"self","InPrefix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_175 = das::das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type),&FString::RemoveFromStart >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:479:16
	makeExtern<DAS_CALL_METHOD(_method_175), SimNode_ExtFuncCall >(lib,"RemoveFromStart","das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) , &FString::RemoveFromStart >::invoke")
		->args({"self","InPrefix","InPrefixLen","SearchCase"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_176 = das::das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type),&FString::RemoveFromEnd >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:500:7
	makeExtern<DAS_CALL_METHOD(_method_176), SimNode_ExtFuncCall >(lib,"RemoveFromEnd","das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) , &FString::RemoveFromEnd >::invoke")
		->args({"self","InSuffix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_177 = das::das_call_member< bool (FString::*)(const FString &,ESearchCase::Type),&FString::RemoveFromEnd >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:511:7
	makeExtern<DAS_CALL_METHOD(_method_177), SimNode_ExtFuncCall >(lib,"RemoveFromEnd","das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) , &FString::RemoveFromEnd >::invoke")
		->args({"self","InSuffix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_178 = das::das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type),&FString::RemoveFromEnd >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:523:16
	makeExtern<DAS_CALL_METHOD(_method_178), SimNode_ExtFuncCall >(lib,"RemoveFromEnd","das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) , &FString::RemoveFromEnd >::invoke")
		->args({"self","InSuffix","InSuffixLen","SearchCase"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_179 = das::das_call_member< void (FString::*)(const wchar_t *,int),&FString::PathAppend >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:531:16
	makeExtern<DAS_CALL_METHOD(_method_179), SimNode_ExtFuncCall >(lib,"PathAppend","das_call_member< void (FString::*)(const wchar_t *,int) , &FString::PathAppend >::invoke")
		->args({"self","Str","StrLength"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_180 = das::das_call_member< int (FString::*)() const,&FString::Len >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:976:33
	makeExtern<DAS_CALL_METHOD(_method_180), SimNode_ExtFuncCall >(lib,"Len","das_call_member< int (FString::*)() const , &FString::Len >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_181 = das::das_call_member< void (FString::*)(int,bool),&FString::LeftInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:994:19
	makeExtern<DAS_CALL_METHOD(_method_181), SimNode_ExtFuncCall >(lib,"LeftInline","das_call_member< void (FString::*)(int,bool) , &FString::LeftInline >::invoke")
		->args({"self","Count","bAllowShrinking"})
		->arg_init(2,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_182 = das::das_call_member< void (FString::*)(int,bool),&FString::LeftChopInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1015:19
	makeExtern<DAS_CALL_METHOD(_method_182), SimNode_ExtFuncCall >(lib,"LeftChopInline","das_call_member< void (FString::*)(int,bool) , &FString::LeftChopInline >::invoke")
		->args({"self","Count","bAllowShrinking"})
		->arg_init(2,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
}
}

