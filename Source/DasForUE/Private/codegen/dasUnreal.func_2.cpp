// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_2() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:905:24
	makeExtern< void (*)(FArchiveState &,bool) , _dasUnreal_virtual_28_SetIsTextFormat , SimNode_ExtFuncCall >(lib,"SetIsTextFormat","_dasUnreal_virtual_28_SetIsTextFormat")
		->args({"self","bInIsTextFormat"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:912:24
	makeExtern< void (*)(FArchiveState &,bool) , _dasUnreal_virtual_29_SetWantBinaryPropertySerialization , SimNode_ExtFuncCall >(lib,"SetWantBinaryPropertySerialization","_dasUnreal_virtual_29_SetWantBinaryPropertySerialization")
		->args({"self","bInWantBinaryPropertySerialization"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:915:24
	makeExtern< void (*)(FArchiveState &,bool) , _dasUnreal_virtual_30_SetUseUnversionedPropertySerialization , SimNode_ExtFuncCall >(lib,"SetUseUnversionedPropertySerialization","_dasUnreal_virtual_30_SetUseUnversionedPropertySerialization")
		->args({"self","bInUseUnversioned"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:923:24
	makeExtern< void (*)(FArchiveState &,bool) , _dasUnreal_virtual_31_SetForceUnicode , SimNode_ExtFuncCall >(lib,"SetForceUnicode","_dasUnreal_virtual_31_SetForceUnicode")
		->args({"self","bInForceUnicode"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:930:24
	makeExtern< void (*)(FArchiveState &,bool) , _dasUnreal_virtual_32_SetIsPersistent , SimNode_ExtFuncCall >(lib,"SetIsPersistent","_dasUnreal_virtual_32_SetIsPersistent")
		->args({"self","bInIsPersistent"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_101 = das::das_call_member< void (FArchiveState::*)(int),&FArchiveState::SetUE4Ver >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:941:19
	makeExtern<DAS_CALL_METHOD(_method_101), SimNode_ExtFuncCall >(lib,"SetUE4Ver","das_call_member< void (FArchiveState::*)(int) , &FArchiveState::SetUE4Ver >::invoke")
		->args({"self","InVer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:953:24
	makeExtern< void (*)(FArchiveState &,int) , _dasUnreal_virtual_33_SetLicenseeUEVer , SimNode_ExtFuncCall >(lib,"SetLicenseeUEVer","_dasUnreal_virtual_33_SetLicenseeUEVer")
		->args({"self","InVer"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_103 = das::das_call_member< void (FArchiveState::*)(int),&FArchiveState::SetLicenseeUE4Ver >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:956:19
	makeExtern<DAS_CALL_METHOD(_method_103), SimNode_ExtFuncCall >(lib,"SetLicenseeUE4Ver","das_call_member< void (FArchiveState::*)(int) , &FArchiveState::SetLicenseeUE4Ver >::invoke")
		->args({"self","InVer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:967:24
	makeExtern< void (*)(FArchiveState &,const FEngineVersionBase &) , _dasUnreal_virtual_34_SetEngineVer , SimNode_ExtFuncCall >(lib,"SetEngineVer","_dasUnreal_virtual_34_SetEngineVer")
		->args({"self","InVer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:972:24
	makeExtern< void (*)(FArchiveState &,const unsigned int) , _dasUnreal_virtual_35_SetEngineNetVer , SimNode_ExtFuncCall >(lib,"SetEngineNetVer","_dasUnreal_virtual_35_SetEngineNetVer")
		->args({"self","InEngineNetVer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:977:24
	makeExtern< void (*)(FArchiveState &,const unsigned int) , _dasUnreal_virtual_36_SetGameNetVer , SimNode_ExtFuncCall >(lib,"SetGameNetVer","_dasUnreal_virtual_36_SetGameNetVer")
		->args({"self","InGameNetVer"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FArchive>(*this,lib,"FArchive","FArchive");
	addCtorAndUsing<FArchive,const FArchive &>(*this,lib,"FArchive","FArchive")
		->args({""});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1291:15
	makeExtern< void (*)(FArchive &) , _dasUnreal_virtual_37_ForceBlueprintFinalization , SimNode_ExtFuncCall >(lib,"ForceBlueprintFinalization","_dasUnreal_virtual_37_ForceBlueprintFinalization")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1579:15
	makeExtern< void (*)(FArchive &,void *,int64) , _dasUnreal_virtual_38_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_38_Serialize")
		->args({"self","V","Length"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1590:15
	makeExtern< void (*)(FArchive &,void *,int64) , _dasUnreal_virtual_39_SerializeBits , SimNode_ExtFuncCall >(lib,"SerializeBits","_dasUnreal_virtual_39_SerializeBits")
		->args({"self","V","LengthBits"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1600:15
	makeExtern< void (*)(FArchive &,unsigned int &,unsigned int) , _dasUnreal_virtual_40_SerializeInt , SimNode_ExtFuncCall >(lib,"SerializeInt","_dasUnreal_virtual_40_SerializeInt")
		->args({"self","Value","Max"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1606:24
	makeExtern< void (*)(FArchive &,unsigned int &) , _dasUnreal_virtual_41_SerializeIntPacked , SimNode_ExtFuncCall >(lib,"SerializeIntPacked","_dasUnreal_virtual_41_SerializeIntPacked")
		->args({"self","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1607:24
	makeExtern< void (*)(FArchive &,unsigned long long &) , _dasUnreal_virtual_42_SerializeIntPacked64 , SimNode_ExtFuncCall >(lib,"SerializeIntPacked64","_dasUnreal_virtual_42_SerializeIntPacked64")
		->args({"self","Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1610:15
	makeExtern< void (*)(FArchive &,UObject *) , _dasUnreal_virtual_43_Preload , SimNode_ExtFuncCall >(lib,"Preload","_dasUnreal_virtual_43_Preload")
		->args({"self","Object"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_114 = das::das_call_member< FArchiveState & (FArchive::*)(),&FArchive::GetArchiveState >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1613:17
	makeExtern<DAS_CALL_METHOD(_method_114), SimNode_ExtFuncCallRef >(lib,"GetArchiveState","das_call_member< FArchiveState & (FArchive::*)() , &FArchive::GetArchiveState >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_115 = das::das_call_member< const FArchiveState & (FArchive::*)() const,&FArchive::GetArchiveState >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1618:23
	makeExtern<DAS_CALL_METHOD(_method_115), SimNode_ExtFuncCallRef >(lib,"GetArchiveState","das_call_member< const FArchiveState & (FArchive::*)() const , &FArchive::GetArchiveState >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1636:15
	makeExtern< void (*)(FArchive &,int64) , _dasUnreal_virtual_44_Seek , SimNode_ExtFuncCall >(lib,"Seek","_dasUnreal_virtual_44_Seek")
		->args({"self","InPos"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1645:15
	makeExtern< void (*)(FArchive &,UE::Serialization::FEditorBulkData *) , _dasUnreal_virtual_45_AttachBulkData , SimNode_ExtFuncCall >(lib,"AttachBulkData","_dasUnreal_virtual_45_AttachBulkData")
		->args({"self","BulkData"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1654:15
	makeExtern< void (*)(FArchive &,UE::Serialization::FEditorBulkData *,bool) , _dasUnreal_virtual_46_DetachBulkData , SimNode_ExtFuncCall >(lib,"DetachBulkData","_dasUnreal_virtual_46_DetachBulkData")
		->args({"self","BulkData","bEnsureBulkDataIsLoaded"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1677:15
	makeExtern< bool (*)(const FArchive &,FArchive *) , _dasUnreal_virtual_47_IsProxyOf , SimNode_ExtFuncCall >(lib,"IsProxyOf","_dasUnreal_virtual_47_IsProxyOf")
		->args({"self","InOther"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1697:15
	makeExtern< bool (*)(FArchive &,int64,int64) , _dasUnreal_virtual_48_Precache , SimNode_ExtFuncCall >(lib,"Precache","_dasUnreal_virtual_48_Precache")
		->args({"self","PrecacheOffset","PrecacheSize"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1705:15
	makeExtern< void (*)(FArchive &) , _dasUnreal_virtual_49_FlushCache , SimNode_ExtFuncCall >(lib,"FlushCache","_dasUnreal_virtual_49_FlushCache")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1725:15
	makeExtern< void (*)(FArchive &) , _dasUnreal_virtual_50_Flush , SimNode_ExtFuncCall >(lib,"Flush","_dasUnreal_virtual_50_Flush")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1730:15
	makeExtern< bool (*)(FArchive &) , _dasUnreal_virtual_51_Close , SimNode_ExtFuncCall >(lib,"Close","_dasUnreal_virtual_51_Close")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_124 = das::das_call_member< void (FArchive::*)(void *,int64,FName,ECompressionFlags,bool),&FArchive::SerializeCompressed >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1747:16
	makeExtern<DAS_CALL_METHOD(_method_124), SimNode_ExtFuncCall >(lib,"SerializeCompressed","das_call_member< void (FArchive::*)(void *,int64,FName,ECompressionFlags,bool) , &FArchive::SerializeCompressed >::invoke")
		->args({"self","V","Length","CompressionFormatCannotChange","Flags","bTreatBufferAsFileReader"})
		->arg_init(4,make_smart<ExprConstEnumeration>(0,makeType<ECompressionFlags>(lib)))
		->arg_init(5,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_125 = das::das_call_member< void (FArchive::*)(void *,int64,FName,FName,ECompressionFlags,bool,long long *),&FArchive::SerializeCompressedNew >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1767:16
	makeExtern<DAS_CALL_METHOD(_method_125), SimNode_ExtFuncCall >(lib,"SerializeCompressedNew","das_call_member< void (FArchive::*)(void *,int64,FName,FName,ECompressionFlags,bool,long long *) , &FArchive::SerializeCompressedNew >::invoke")
		->args({"self","V","Length","CompressionFormatToEncode","CompressionFormatToDecodeOldV1Files","Flags","bTreatBufferAsFileReader","OutPartialReadLength"})
		->arg_init(5,make_smart<ExprConstEnumeration>(0,makeType<ECompressionFlags>(lib)))
		->arg_init(6,make_smart<ExprConstBool>(false))
		->arg_init(7,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_126 = das::das_call_member< void (FArchive::*)(void *,int64),&FArchive::SerializeCompressedNew >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1779:16
	makeExtern<DAS_CALL_METHOD(_method_126), SimNode_ExtFuncCall >(lib,"SerializeCompressedNew","das_call_member< void (FArchive::*)(void *,int64) , &FArchive::SerializeCompressedNew >::invoke")
		->args({"self","V","Length"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_127 = das::das_call_member< void (FArchive::*)(void *,int),&FArchive::ByteSwap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1785:16
	makeExtern<DAS_CALL_METHOD(_method_127), SimNode_ExtFuncCall >(lib,"ByteSwap","das_call_member< void (FArchive::*)(void *,int) , &FArchive::ByteSwap >::invoke")
		->args({"self","V","Length"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_128 = das::das_call_member< FArchive & (FArchive::*)(void *,int),&FArchive::ByteOrderSerialize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1788:24
	makeExtern<DAS_CALL_METHOD(_method_128), SimNode_ExtFuncCallRef >(lib,"ByteOrderSerialize","das_call_member< FArchive & (FArchive::*)(void *,int) , &FArchive::ByteOrderSerialize >::invoke")
		->args({"self","V","Length"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_129 = das::das_call_member< void (FArchive::*)(),&FArchive::StartSerializingDefaults >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1803:7
	makeExtern<DAS_CALL_METHOD(_method_129), SimNode_ExtFuncCall >(lib,"StartSerializingDefaults","das_call_member< void (FArchive::*)() , &FArchive::StartSerializingDefaults >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_130 = das::das_call_member< void (FArchive::*)(),&FArchive::StopSerializingDefaults >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1809:7
	makeExtern<DAS_CALL_METHOD(_method_130), SimNode_ExtFuncCall >(lib,"StopSerializingDefaults","das_call_member< void (FArchive::*)() , &FArchive::StopSerializingDefaults >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1817:15
	makeExtern< void (*)(FArchive &,const UObject *) , _dasUnreal_virtual_52_MarkScriptSerializationStart , SimNode_ExtFuncCall >(lib,"MarkScriptSerializationStart","_dasUnreal_virtual_52_MarkScriptSerializationStart")
		->args({"self","Obj"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1822:15
	makeExtern< void (*)(FArchive &,const UObject *) , _dasUnreal_virtual_53_MarkScriptSerializationEnd , SimNode_ExtFuncCall >(lib,"MarkScriptSerializationEnd","_dasUnreal_virtual_53_MarkScriptSerializationEnd")
		->args({"self","Obj"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1868:20
	makeExtern< FArchive * (*)(FArchive &) , _dasUnreal_virtual_54_GetCacheableArchive , SimNode_ExtFuncCall >(lib,"GetCacheableArchive","_dasUnreal_virtual_54_GetCacheableArchive")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1935:24
	makeExtern< void (*)(FArchive &,FProperty *,const bool) , _dasUnreal_virtual_55_PushSerializedProperty , SimNode_ExtFuncCall >(lib,"PushSerializedProperty","_dasUnreal_virtual_55_PushSerializedProperty")
		->args({"self","InProperty","bIsEditorOnlyProperty"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:1943:24
	makeExtern< void (*)(FArchive &,FProperty *,const bool) , _dasUnreal_virtual_56_PopSerializedProperty , SimNode_ExtFuncCall >(lib,"PopSerializedProperty","_dasUnreal_virtual_56_PopSerializedProperty")
		->args({"self","InProperty","bIsEditorOnlyProperty"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FArchive::FScopeSetDebugSerializationFlags,FArchive &,unsigned int,bool>(*this,lib,"FScopeSetDebugSerializationFlags","FArchive::FScopeSetDebugSerializationFlags")
		->args({"InAr","NewFlags","Remove"})
		->arg_init(2,make_smart<ExprConstBool>(false));
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:2161:24
	makeExtern< void (*)(FArchive &,const FName &) , _dasUnreal_virtual_57_PushDebugDataString , SimNode_ExtFuncCall >(lib,"PushDebugDataString","_dasUnreal_virtual_57_PushDebugDataString")
		->args({"self","DebugData"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:2162:15
	makeExtern< void (*)(FArchive &) , _dasUnreal_virtual_58_PopDebugDataString , SimNode_ExtFuncCall >(lib,"PopDebugDataString","_dasUnreal_virtual_58_PopDebugDataString")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FArchive::FScopeAddDebugData,FArchive &,const FName &>(*this,lib,"FScopeAddDebugData","FArchive::FScopeAddDebugData")
		->args({"InAr","DebugData"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:2203:15
	makeExtern< void (*)(FArchive &,EFileRegionType) , _dasUnreal_virtual_59_PushFileRegionType , SimNode_ExtFuncCall >(lib,"PushFileRegionType","_dasUnreal_virtual_59_PushFileRegionType")
		->args({"self","Type"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:2204:15
	makeExtern< void (*)(FArchive &) , _dasUnreal_virtual_60_PopFileRegionType , SimNode_ExtFuncCall >(lib,"PopFileRegionType","_dasUnreal_virtual_60_PopFileRegionType")
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
	using _method_140 = das::das_call_member< SIZE_T (FString::*)() const,&FString::GetAllocatedSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:266:21
	makeExtern<DAS_CALL_METHOD(_method_140), SimNode_ExtFuncCall >(lib,"GetAllocatedSize","das_call_member< SIZE_T (FString::*)() const , &FString::GetAllocatedSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_141 = das::das_call_member< void (FString::*)() const,&FString::CheckInvariants >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:274:19
	makeExtern<DAS_CALL_METHOD(_method_141), SimNode_ExtFuncCall >(lib,"CheckInvariants","das_call_member< void (FString::*)() const , &FString::CheckInvariants >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_142 = das::das_call_member< void (FString::*)(),&FString::Empty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:287:16
	makeExtern<DAS_CALL_METHOD(_method_142), SimNode_ExtFuncCall >(lib,"Empty","das_call_member< void (FString::*)() , &FString::Empty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_143 = das::das_call_member< void (FString::*)(int),&FString::Empty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:288:16
	makeExtern<DAS_CALL_METHOD(_method_143), SimNode_ExtFuncCall >(lib,"Empty","das_call_member< void (FString::*)(int) , &FString::Empty >::invoke")
		->args({"self","Slack"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_144 = das::das_call_member< bool (FString::*)() const,&FString::IsEmpty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:295:32
	makeExtern<DAS_CALL_METHOD(_method_144), SimNode_ExtFuncCall >(lib,"IsEmpty","das_call_member< bool (FString::*)() const , &FString::IsEmpty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_145 = das::das_call_member< void (FString::*)(int),&FString::Reset >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:305:16
	makeExtern<DAS_CALL_METHOD(_method_145), SimNode_ExtFuncCall >(lib,"Reset","das_call_member< void (FString::*)(int) , &FString::Reset >::invoke")
		->args({"self","NewReservedSize"})
		->arg_init(1,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_146 = das::das_call_member< void (FString::*)(),&FString::Shrink >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:310:16
	makeExtern<DAS_CALL_METHOD(_method_146), SimNode_ExtFuncCall >(lib,"Shrink","das_call_member< void (FString::*)() , &FString::Shrink >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_147 = das::das_call_member< bool (FString::*)(int) const,&FString::IsValidIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:319:32
	makeExtern<DAS_CALL_METHOD(_method_147), SimNode_ExtFuncCall >(lib,"IsValidIndex","das_call_member< bool (FString::*)(int) const , &FString::IsValidIndex >::invoke")
		->args({"self","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_148 = das::das_call_member< void (FString::*)(const char *,int),&FString::AppendChars >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:361:16
	makeExtern<DAS_CALL_METHOD(_method_148), SimNode_ExtFuncCall >(lib,"AppendChars","das_call_member< void (FString::*)(const char *,int) , &FString::AppendChars >::invoke")
		->args({"self","Str","Count"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_149 = das::das_call_member< FString & (FString::*)(wchar_t),&FString::AppendChar >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:398:20
	makeExtern<DAS_CALL_METHOD(_method_149), SimNode_ExtFuncCallRef >(lib,"AppendChar","das_call_member< FString & (FString::*)(wchar_t) , &FString::AppendChar >::invoke")
		->args({"self","InChar"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_150 = das::das_call_member< void (FString::*)(int,wchar_t),&FString::InsertAt >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:425:16
	makeExtern<DAS_CALL_METHOD(_method_150), SimNode_ExtFuncCall >(lib,"InsertAt","das_call_member< void (FString::*)(int,wchar_t) , &FString::InsertAt >::invoke")
		->args({"self","Index","Character"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_151 = das::das_call_member< void (FString::*)(int,const FString &),&FString::InsertAt >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:426:16
	makeExtern<DAS_CALL_METHOD(_method_151), SimNode_ExtFuncCall >(lib,"InsertAt","das_call_member< void (FString::*)(int,const FString &) , &FString::InsertAt >::invoke")
		->args({"self","Index","Characters"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_152 = das::das_call_member< void (FString::*)(int,int,bool),&FString::RemoveAt >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:435:16
	makeExtern<DAS_CALL_METHOD(_method_152), SimNode_ExtFuncCall >(lib,"RemoveAt","das_call_member< void (FString::*)(int,int,bool) , &FString::RemoveAt >::invoke")
		->args({"self","Index","Count","bAllowShrinking"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->arg_init(3,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_153 = das::das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type),&FString::RemoveFromStart >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:456:7
	makeExtern<DAS_CALL_METHOD(_method_153), SimNode_ExtFuncCall >(lib,"RemoveFromStart","das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) , &FString::RemoveFromStart >::invoke")
		->args({"self","InPrefix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_154 = das::das_call_member< bool (FString::*)(const FString &,ESearchCase::Type),&FString::RemoveFromStart >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:467:7
	makeExtern<DAS_CALL_METHOD(_method_154), SimNode_ExtFuncCall >(lib,"RemoveFromStart","das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) , &FString::RemoveFromStart >::invoke")
		->args({"self","InPrefix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_155 = das::das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type),&FString::RemoveFromStart >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:479:16
	makeExtern<DAS_CALL_METHOD(_method_155), SimNode_ExtFuncCall >(lib,"RemoveFromStart","das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) , &FString::RemoveFromStart >::invoke")
		->args({"self","InPrefix","InPrefixLen","SearchCase"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_156 = das::das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type),&FString::RemoveFromEnd >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:500:7
	makeExtern<DAS_CALL_METHOD(_method_156), SimNode_ExtFuncCall >(lib,"RemoveFromEnd","das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) , &FString::RemoveFromEnd >::invoke")
		->args({"self","InSuffix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_157 = das::das_call_member< bool (FString::*)(const FString &,ESearchCase::Type),&FString::RemoveFromEnd >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:511:7
	makeExtern<DAS_CALL_METHOD(_method_157), SimNode_ExtFuncCall >(lib,"RemoveFromEnd","das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) , &FString::RemoveFromEnd >::invoke")
		->args({"self","InSuffix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_158 = das::das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type),&FString::RemoveFromEnd >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:523:16
	makeExtern<DAS_CALL_METHOD(_method_158), SimNode_ExtFuncCall >(lib,"RemoveFromEnd","das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) , &FString::RemoveFromEnd >::invoke")
		->args({"self","InSuffix","InSuffixLen","SearchCase"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_159 = das::das_call_member< void (FString::*)(const wchar_t *,int),&FString::PathAppend >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:531:16
	makeExtern<DAS_CALL_METHOD(_method_159), SimNode_ExtFuncCall >(lib,"PathAppend","das_call_member< void (FString::*)(const wchar_t *,int) , &FString::PathAppend >::invoke")
		->args({"self","Str","StrLength"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_160 = das::das_call_member< int (FString::*)() const,&FString::Len >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:976:33
	makeExtern<DAS_CALL_METHOD(_method_160), SimNode_ExtFuncCall >(lib,"Len","das_call_member< int (FString::*)() const , &FString::Len >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_161 = das::das_call_member< void (FString::*)(int,bool),&FString::LeftInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:994:19
	makeExtern<DAS_CALL_METHOD(_method_161), SimNode_ExtFuncCall >(lib,"LeftInline","das_call_member< void (FString::*)(int,bool) , &FString::LeftInline >::invoke")
		->args({"self","Count","bAllowShrinking"})
		->arg_init(2,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_162 = das::das_call_member< void (FString::*)(int,bool),&FString::LeftChopInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1015:19
	makeExtern<DAS_CALL_METHOD(_method_162), SimNode_ExtFuncCall >(lib,"LeftChopInline","das_call_member< void (FString::*)(int,bool) , &FString::LeftChopInline >::invoke")
		->args({"self","Count","bAllowShrinking"})
		->arg_init(2,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_163 = das::das_call_member< void (FString::*)(int,bool),&FString::RightInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1035:19
	makeExtern<DAS_CALL_METHOD(_method_163), SimNode_ExtFuncCall >(lib,"RightInline","das_call_member< void (FString::*)(int,bool) , &FString::RightInline >::invoke")
		->args({"self","Count","bAllowShrinking"})
		->arg_init(2,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_164 = das::das_call_member< void (FString::*)(int,bool),&FString::RightChopInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1051:19
	makeExtern<DAS_CALL_METHOD(_method_164), SimNode_ExtFuncCall >(lib,"RightChopInline","das_call_member< void (FString::*)(int,bool) , &FString::RightChopInline >::invoke")
		->args({"self","Count","bAllowShrinking"})
		->arg_init(2,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_165 = das::das_call_member< void (FString::*)(int,int,bool),&FString::MidInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1065:19
	makeExtern<DAS_CALL_METHOD(_method_165), SimNode_ExtFuncCall >(lib,"MidInline","das_call_member< void (FString::*)(int,int,bool) , &FString::MidInline >::invoke")
		->args({"self","Start","Count","bAllowShrinking"})
		->arg_init(2,make_smart<ExprConstInt>(2147483647))
		->arg_init(3,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_166 = das::das_call_member< int (FString::*)(const wchar_t *,ESearchCase::Type,ESearchDir::Type,int) const,&FString::Find >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1107:21
	makeExtern<DAS_CALL_METHOD(_method_166), SimNode_ExtFuncCall >(lib,"Find","das_call_member< int (FString::*)(const wchar_t *,ESearchCase::Type,ESearchDir::Type,int) const , &FString::Find >::invoke")
		->args({"self","SubStr","SearchCase","SearchDir","StartPosition"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<ESearchDir::Type>(lib)))
		->arg_init(4,make_smart<ExprConstInt>(-1))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_167 = das::das_call_member< int (FString::*)(const FString &,ESearchCase::Type,ESearchDir::Type,int) const,&FString::Find >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1129:21
	makeExtern<DAS_CALL_METHOD(_method_167), SimNode_ExtFuncCall >(lib,"Find","das_call_member< int (FString::*)(const FString &,ESearchCase::Type,ESearchDir::Type,int) const , &FString::Find >::invoke")
		->args({"self","SubStr","SearchCase","SearchDir","StartPosition"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<ESearchDir::Type>(lib)))
		->arg_init(4,make_smart<ExprConstInt>(-1))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_168 = das::das_call_member< int (FString::*)(const wchar_t *,int,ESearchCase::Type,ESearchDir::Type,int) const,&FString::Find >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1152:30
	makeExtern<DAS_CALL_METHOD(_method_168), SimNode_ExtFuncCall >(lib,"Find","das_call_member< int (FString::*)(const wchar_t *,int,ESearchCase::Type,ESearchDir::Type,int) const , &FString::Find >::invoke")
		->args({"self","SubStr","InSubStrLen","SearchCase","SearchDir","StartPosition"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->arg_init(4,make_smart<ExprConstEnumeration>(0,makeType<ESearchDir::Type>(lib)))
		->arg_init(5,make_smart<ExprConstInt>(-1))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_169 = das::das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type,ESearchDir::Type) const,&FString::Contains >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1180:32
	makeExtern<DAS_CALL_METHOD(_method_169), SimNode_ExtFuncCall >(lib,"Contains","das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type,ESearchDir::Type) const , &FString::Contains >::invoke")
		->args({"self","SubStr","SearchCase","SearchDir"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<ESearchDir::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_170 = das::das_call_member< bool (FString::*)(const FString &,ESearchCase::Type,ESearchDir::Type) const,&FString::Contains >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1194:32
	makeExtern<DAS_CALL_METHOD(_method_170), SimNode_ExtFuncCall >(lib,"Contains","das_call_member< bool (FString::*)(const FString &,ESearchCase::Type,ESearchDir::Type) const , &FString::Contains >::invoke")
		->args({"self","SubStr","SearchCase","SearchDir"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<ESearchDir::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_171 = das::das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type,ESearchDir::Type) const,&FString::Contains >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1209:32
	makeExtern<DAS_CALL_METHOD(_method_171), SimNode_ExtFuncCall >(lib,"Contains","das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type,ESearchDir::Type) const , &FString::Contains >::invoke")
		->args({"self","SubStr","SubStrLen","SearchCase","SearchDir"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->arg_init(4,make_smart<ExprConstEnumeration>(0,makeType<ESearchDir::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_172 = das::das_call_member< bool (FString::*)(wchar_t,int &) const,&FString::FindChar >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1222:19
	makeExtern<DAS_CALL_METHOD(_method_172), SimNode_ExtFuncCall >(lib,"FindChar","das_call_member< bool (FString::*)(wchar_t,int &) const , &FString::FindChar >::invoke")
		->args({"self","InChar","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_173 = das::das_call_member< bool (FString::*)(wchar_t,int &) const,&FString::FindLastChar >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1234:19
	makeExtern<DAS_CALL_METHOD(_method_173), SimNode_ExtFuncCall >(lib,"FindLastChar","das_call_member< bool (FString::*)(wchar_t,int &) const , &FString::FindLastChar >::invoke")
		->args({"self","InChar","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_174 = das::das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const,&FString::Equals >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1275:32
	makeExtern<DAS_CALL_METHOD(_method_174), SimNode_ExtFuncCall >(lib,"Equals","das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const , &FString::Equals >::invoke")
		->args({"self","Other","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_175 = das::das_call_member< int (FString::*)(const FString &,ESearchCase::Type) const,&FString::Compare >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1307:33
	makeExtern<DAS_CALL_METHOD(_method_175), SimNode_ExtFuncCall >(lib,"Compare","das_call_member< int (FString::*)(const FString &,ESearchCase::Type) const , &FString::Compare >::invoke")
		->args({"self","Other","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_176 = das::das_call_member< bool (FString::*)(const FString &,FString *,FString *,ESearchCase::Type,ESearchDir::Type) const,&FString::Split >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1329:16
	makeExtern<DAS_CALL_METHOD(_method_176), SimNode_ExtFuncCall >(lib,"Split","das_call_member< bool (FString::*)(const FString &,FString *,FString *,ESearchCase::Type,ESearchDir::Type) const , &FString::Split >::invoke")
		->args({"self","InS","LeftS","RightS","SearchCase","SearchDir"})
		->arg_init(5,make_smart<ExprConstEnumeration>(0,makeType<ESearchDir::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_177 = das::das_call_member< bool (FString::*)(const FString &,FString *,FString *) const,&FString::Split >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1333:16
	makeExtern<DAS_CALL_METHOD(_method_177), SimNode_ExtFuncCall >(lib,"Split","das_call_member< bool (FString::*)(const FString &,FString *,FString *) const , &FString::Split >::invoke")
		->args({"self","InS","LeftS","RightS"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_178 = das::das_call_member< void (FString::*)(),&FString::ToUpperInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1345:16
	makeExtern<DAS_CALL_METHOD(_method_178), SimNode_ExtFuncCall >(lib,"ToUpperInline","das_call_member< void (FString::*)() , &FString::ToUpperInline >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_179 = das::das_call_member< void (FString::*)(),&FString::ToLowerInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1357:16
	makeExtern<DAS_CALL_METHOD(_method_179), SimNode_ExtFuncCall >(lib,"ToLowerInline","das_call_member< void (FString::*)() , &FString::ToLowerInline >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_180 = das::das_call_member< FString (FString::*)(int) const,&FString::LeftPad >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1360:32
	makeExtern<DAS_CALL_METHOD(_method_180), SimNode_ExtFuncCallAndCopyOrMove >(lib,"LeftPad","das_call_member< FString (FString::*)(int) const , &FString::LeftPad >::invoke")
		->args({"self","ChCount"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_181 = das::das_call_member< FString (FString::*)(int) const,&FString::RightPad >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1363:32
	makeExtern<DAS_CALL_METHOD(_method_181), SimNode_ExtFuncCallAndCopyOrMove >(lib,"RightPad","das_call_member< FString (FString::*)(int) const , &FString::RightPad >::invoke")
		->args({"self","ChCount"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_182 = das::das_call_member< bool (FString::*)() const,&FString::IsNumeric >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1366:29
	makeExtern<DAS_CALL_METHOD(_method_182), SimNode_ExtFuncCall >(lib,"IsNumeric","das_call_member< bool (FString::*)() const , &FString::IsNumeric >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

