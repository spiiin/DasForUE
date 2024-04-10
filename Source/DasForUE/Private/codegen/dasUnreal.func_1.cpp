// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_1() {
	addCtorAndUsing<FEngineVersionBase>(*this,lib,"FEngineVersionBase","FEngineVersionBase");
	addCtorAndUsing<FEngineVersionBase,unsigned short,unsigned short,unsigned short,unsigned int>(*this,lib,"FEngineVersionBase","FEngineVersionBase")
		->args({"InMajor","InMinor","InPatch","InChangelist"})
		->arg_init(3,make_smart<ExprConstUInt>(0x0));
	using _method_1 = das::das_call_member< unsigned int (FEngineVersionBase::*)() const,&FEngineVersionBase::GetChangelist >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/EngineVersionBase.h:39:18
	makeExtern<DAS_CALL_METHOD(_method_1), SimNode_ExtFuncCall >(lib,"GetChangelist","das_call_member< unsigned int (FEngineVersionBase::*)() const , &FEngineVersionBase::GetChangelist >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_2 = das::das_call_member< unsigned short (FEngineVersionBase::*)() const,&FEngineVersionBase::GetMajor >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/EngineVersionBase.h:42:21
	makeExtern<DAS_CALL_METHOD(_method_2), SimNode_ExtFuncCall >(lib,"GetMajor","das_call_member< unsigned short (FEngineVersionBase::*)() const , &FEngineVersionBase::GetMajor >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_3 = das::das_call_member< unsigned short (FEngineVersionBase::*)() const,&FEngineVersionBase::GetMinor >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/EngineVersionBase.h:48:21
	makeExtern<DAS_CALL_METHOD(_method_3), SimNode_ExtFuncCall >(lib,"GetMinor","das_call_member< unsigned short (FEngineVersionBase::*)() const , &FEngineVersionBase::GetMinor >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_4 = das::das_call_member< unsigned short (FEngineVersionBase::*)() const,&FEngineVersionBase::GetPatch >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/EngineVersionBase.h:54:21
	makeExtern<DAS_CALL_METHOD(_method_4), SimNode_ExtFuncCall >(lib,"GetPatch","das_call_member< unsigned short (FEngineVersionBase::*)() const , &FEngineVersionBase::GetPatch >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_5 = das::das_call_member< bool (FEngineVersionBase::*)() const,&FEngineVersionBase::IsLicenseeVersion >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/EngineVersionBase.h:60:16
	makeExtern<DAS_CALL_METHOD(_method_5), SimNode_ExtFuncCall >(lib,"IsLicenseeVersion","das_call_member< bool (FEngineVersionBase::*)() const , &FEngineVersionBase::IsLicenseeVersion >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_6 = das::das_call_member< bool (FEngineVersionBase::*)() const,&FEngineVersionBase::IsEmpty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/EngineVersionBase.h:63:16
	makeExtern<DAS_CALL_METHOD(_method_6), SimNode_ExtFuncCall >(lib,"IsEmpty","das_call_member< bool (FEngineVersionBase::*)() const , &FEngineVersionBase::IsEmpty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_7 = das::das_call_member< bool (FEngineVersionBase::*)() const,&FEngineVersionBase::HasChangelist >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/EngineVersionBase.h:66:16
	makeExtern<DAS_CALL_METHOD(_method_7), SimNode_ExtFuncCall >(lib,"HasChangelist","das_call_member< bool (FEngineVersionBase::*)() const , &FEngineVersionBase::HasChangelist >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/EngineVersionBase.h:69:37
	makeExtern< EVersionComparison (*)(const FEngineVersionBase *,const FEngineVersionBase &,const FEngineVersionBase &,EVersionComponent *) , _dasUnreal_static_0_GetNewest , SimNode_ExtFuncCall >(lib,"GetNewest","_dasUnreal_static_0_GetNewest")
		->args({"self","First","Second","OutComponent"})
		->arg_type(0,makeType<TExplicit<const FEngineVersionBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/EngineVersionBase.h:72:25
	makeExtern< unsigned int (*)(const FEngineVersionBase *,unsigned int) , _dasUnreal_static_1_EncodeLicenseeChangelist , SimNode_ExtFuncCall >(lib,"EncodeLicenseeChangelist","_dasUnreal_static_1_EncodeLicenseeChangelist")
		->args({"self","Changelist"})
		->arg_type(0,makeType<TExplicit<const FEngineVersionBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Logging/LogVerbosity.h:72:23
	makeExtern< const wchar_t * (*)(ELogVerbosity::Type) , ToString , SimNode_ExtFuncCall >(lib,"ToString","ToString")
		->args({"Verbosity"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Logging/LogVerbosity.h:79:37
	makeExtern< ELogVerbosity::Type (*)(const FString &) , ParseLogVerbosityFromString , SimNode_ExtFuncCall >(lib,"ParseLogVerbosityFromString","ParseLogVerbosityFromString")
		->args({"VerbosityString"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:146:15
	makeExtern< void (*)(FOutputDevice &,const wchar_t *,ELogVerbosity::Type,const FName &,const double) , _dasUnreal_virtual_2_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_2_Serialize")
		->args({"self","V","Verbosity","Category","Time"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:151:24
	makeExtern< void (*)(FOutputDevice &,const UE::FLogRecord &) , _dasUnreal_virtual_3_SerializeRecord , SimNode_ExtFuncCall >(lib,"SerializeRecord","_dasUnreal_virtual_3_SerializeRecord")
		->args({"self","Record"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:153:15
	makeExtern< void (*)(FOutputDevice &) , _dasUnreal_virtual_4_Flush , SimNode_ExtFuncCall >(lib,"Flush","_dasUnreal_virtual_4_Flush")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:162:15
	makeExtern< void (*)(FOutputDevice &) , _dasUnreal_virtual_5_TearDown , SimNode_ExtFuncCall >(lib,"TearDown","_dasUnreal_virtual_5_TearDown")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_14 = das::das_call_member< void (FOutputDevice::*)(bool),&FOutputDevice::SetSuppressEventTag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:166:7
	makeExtern<DAS_CALL_METHOD(_method_14), SimNode_ExtFuncCall >(lib,"SetSuppressEventTag","das_call_member< void (FOutputDevice::*)(bool) , &FOutputDevice::SetSuppressEventTag >::invoke")
		->args({"self","bInSuppressEventTag"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_15 = das::das_call_member< bool (FOutputDevice::*)() const,&FOutputDevice::GetSuppressEventTag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:170:19
	makeExtern<DAS_CALL_METHOD(_method_15), SimNode_ExtFuncCall >(lib,"GetSuppressEventTag","das_call_member< bool (FOutputDevice::*)() const , &FOutputDevice::GetSuppressEventTag >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_16 = das::das_call_member< void (FOutputDevice::*)(bool),&FOutputDevice::SetAutoEmitLineTerminator >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:171:7
	makeExtern<DAS_CALL_METHOD(_method_16), SimNode_ExtFuncCall >(lib,"SetAutoEmitLineTerminator","das_call_member< void (FOutputDevice::*)(bool) , &FOutputDevice::SetAutoEmitLineTerminator >::invoke")
		->args({"self","bInAutoEmitLineTerminator"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_17 = das::das_call_member< bool (FOutputDevice::*)() const,&FOutputDevice::GetAutoEmitLineTerminator >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:175:19
	makeExtern<DAS_CALL_METHOD(_method_17), SimNode_ExtFuncCall >(lib,"GetAutoEmitLineTerminator","das_call_member< bool (FOutputDevice::*)() const , &FOutputDevice::GetAutoEmitLineTerminator >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:181:15
	makeExtern< void (*)(FOutputDevice &,FArchive &) , _dasUnreal_virtual_6_Dump , SimNode_ExtFuncCall >(lib,"Dump","_dasUnreal_virtual_6_Dump")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:188:15
	makeExtern< bool (*)(const FOutputDevice &) , _dasUnreal_virtual_7_IsMemoryOnly , SimNode_ExtFuncCall >(lib,"IsMemoryOnly","_dasUnreal_virtual_7_IsMemoryOnly")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:196:15
	makeExtern< bool (*)(const FOutputDevice &) , _dasUnreal_virtual_8_CanBeUsedOnAnyThread , SimNode_ExtFuncCall >(lib,"CanBeUsedOnAnyThread","_dasUnreal_virtual_8_CanBeUsedOnAnyThread")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:204:15
	makeExtern< bool (*)(const FOutputDevice &) , _dasUnreal_virtual_9_CanBeUsedOnMultipleThreads , SimNode_ExtFuncCall >(lib,"CanBeUsedOnMultipleThreads","_dasUnreal_virtual_9_CanBeUsedOnMultipleThreads")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:213:15
	makeExtern< bool (*)(const FOutputDevice &) , _dasUnreal_virtual_10_CanBeUsedOnPanicThread , SimNode_ExtFuncCall >(lib,"CanBeUsedOnPanicThread","_dasUnreal_virtual_10_CanBeUsedOnPanicThread")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_23 = das::das_call_member< void (FOutputDevice::*)(const wchar_t *),&FOutputDevice::Log >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:219:16
	makeExtern<DAS_CALL_METHOD(_method_23), SimNode_ExtFuncCall >(lib,"Log","das_call_member< void (FOutputDevice::*)(const wchar_t *) , &FOutputDevice::Log >::invoke")
		->args({"self","S"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_24 = das::das_call_member< void (FOutputDevice::*)(ELogVerbosity::Type,const wchar_t *),&FOutputDevice::Log >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:220:16
	makeExtern<DAS_CALL_METHOD(_method_24), SimNode_ExtFuncCall >(lib,"Log","das_call_member< void (FOutputDevice::*)(ELogVerbosity::Type,const wchar_t *) , &FOutputDevice::Log >::invoke")
		->args({"self","Verbosity","S"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_25 = das::das_call_member< void (FOutputDevice::*)(const FName &,ELogVerbosity::Type,const wchar_t *),&FOutputDevice::Log >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:221:16
	makeExtern<DAS_CALL_METHOD(_method_25), SimNode_ExtFuncCall >(lib,"Log","das_call_member< void (FOutputDevice::*)(const FName &,ELogVerbosity::Type,const wchar_t *) , &FOutputDevice::Log >::invoke")
		->args({"self","Category","Verbosity","Str"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_26 = das::das_call_member< void (FOutputDevice::*)(const FString &),&FOutputDevice::Log >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:222:16
	makeExtern<DAS_CALL_METHOD(_method_26), SimNode_ExtFuncCall >(lib,"Log","das_call_member< void (FOutputDevice::*)(const FString &) , &FOutputDevice::Log >::invoke")
		->args({"self","S"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_27 = das::das_call_member< void (FOutputDevice::*)(const FText &),&FOutputDevice::Log >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:223:16
	makeExtern<DAS_CALL_METHOD(_method_27), SimNode_ExtFuncCall >(lib,"Log","das_call_member< void (FOutputDevice::*)(const FText &) , &FOutputDevice::Log >::invoke")
		->args({"self","S"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_28 = das::das_call_member< void (FOutputDevice::*)(ELogVerbosity::Type,const FString &),&FOutputDevice::Log >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:224:16
	makeExtern<DAS_CALL_METHOD(_method_28), SimNode_ExtFuncCall >(lib,"Log","das_call_member< void (FOutputDevice::*)(ELogVerbosity::Type,const FString &) , &FOutputDevice::Log >::invoke")
		->args({"self","Verbosity","S"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_29 = das::das_call_member< void (FOutputDevice::*)(const FName &,ELogVerbosity::Type,const FString &),&FOutputDevice::Log >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:225:16
	makeExtern<DAS_CALL_METHOD(_method_29), SimNode_ExtFuncCall >(lib,"Log","das_call_member< void (FOutputDevice::*)(const FName &,ELogVerbosity::Type,const FString &) , &FOutputDevice::Log >::invoke")
		->args({"self","Category","Verbosity","S"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:93:25
	makeExtern< FArchiveState & (*)(FArchiveState &) , _dasUnreal_virtual_11_GetInnermostState , SimNode_ExtFuncCallRef >(lib,"GetInnermostState","_dasUnreal_virtual_11_GetInnermostState")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_31 = das::das_call_member< void (FArchiveState::*)(const FArchiveState &),&FArchiveState::SetArchiveState >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:101:16
	makeExtern<DAS_CALL_METHOD(_method_31), SimNode_ExtFuncCall >(lib,"SetArchiveState","das_call_member< void (FArchiveState::*)(const FArchiveState &) , &FArchiveState::SetArchiveState >::invoke")
		->args({"self","InState"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_32 = das::das_call_member< void (FArchiveState::*)(),&FArchiveState::SetError >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:106:16
	makeExtern<DAS_CALL_METHOD(_method_32), SimNode_ExtFuncCall >(lib,"SetError","das_call_member< void (FArchiveState::*)() , &FArchiveState::SetError >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_33 = das::das_call_member< void (FArchiveState::*)(),&FArchiveState::ClearError >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:111:16
	makeExtern<DAS_CALL_METHOD(_method_33), SimNode_ExtFuncCall >(lib,"ClearError","das_call_member< void (FArchiveState::*)() , &FArchiveState::ClearError >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_34 = das::das_call_member< void (FArchiveState::*)(),&FArchiveState::SetCriticalError >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:116:16
	makeExtern<DAS_CALL_METHOD(_method_34), SimNode_ExtFuncCall >(lib,"SetCriticalError","das_call_member< void (FArchiveState::*)() , &FArchiveState::SetCriticalError >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:121:15
	makeExtern< void (*)(FArchiveState &,SIZE_T,SIZE_T) , _dasUnreal_virtual_12_CountBytes , SimNode_ExtFuncCall >(lib,"CountBytes","_dasUnreal_virtual_12_CountBytes")
		->args({"self","InNum","InMax"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:129:27
	makeExtern< FString (*)(const FArchiveState &) , _dasUnreal_virtual_13_GetArchiveName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetArchiveName","_dasUnreal_virtual_13_GetArchiveName")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:136:19
	makeExtern< FLinker * (*)(FArchiveState &) , _dasUnreal_virtual_14_GetLinker , SimNode_ExtFuncCall >(lib,"GetLinker","_dasUnreal_virtual_14_GetLinker")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:145:16
	makeExtern< int64 (*)(FArchiveState &) , _dasUnreal_virtual_15_Tell , SimNode_ExtFuncCall >(lib,"Tell","_dasUnreal_virtual_15_Tell")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:151:16
	makeExtern< int64 (*)(FArchiveState &) , _dasUnreal_virtual_16_TotalSize , SimNode_ExtFuncCall >(lib,"TotalSize","_dasUnreal_virtual_16_TotalSize")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:157:15
	makeExtern< bool (*)(FArchiveState &) , _dasUnreal_virtual_17_AtEnd , SimNode_ExtFuncCall >(lib,"AtEnd","_dasUnreal_virtual_17_AtEnd")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_41 = das::das_call_member< bool (FArchiveState::*)(),&FArchiveState::IsByteSwapping >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:165:19
	makeExtern<DAS_CALL_METHOD(_method_41), SimNode_ExtFuncCall >(lib,"IsByteSwapping","das_call_member< bool (FArchiveState::*)() , &FArchiveState::IsByteSwapping >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_42 = das::das_call_member< void (FArchiveState::*)(),&FArchiveState::ThisContainsCode >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:176:7
	makeExtern<DAS_CALL_METHOD(_method_42), SimNode_ExtFuncCall >(lib,"ThisContainsCode","das_call_member< void (FArchiveState::*)() , &FArchiveState::ThisContainsCode >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_43 = das::das_call_member< void (FArchiveState::*)(),&FArchiveState::ThisContainsMap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:182:7
	makeExtern<DAS_CALL_METHOD(_method_43), SimNode_ExtFuncCall >(lib,"ThisContainsMap","das_call_member< void (FArchiveState::*)() , &FArchiveState::ThisContainsMap >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_44 = das::das_call_member< void (FArchiveState::*)(),&FArchiveState::ThisRequiresLocalizationGather >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:188:7
	makeExtern<DAS_CALL_METHOD(_method_44), SimNode_ExtFuncCall >(lib,"ThisRequiresLocalizationGather","das_call_member< void (FArchiveState::*)() , &FArchiveState::ThisRequiresLocalizationGather >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:197:19
	makeExtern< UObject * (*)(FArchiveState &,const UObject *) , _dasUnreal_virtual_18_GetArchetypeFromLoader , SimNode_ExtFuncCall >(lib,"GetArchetypeFromLoader","_dasUnreal_virtual_18_GetArchetypeFromLoader")
		->args({"self","Obj"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_46 = das::das_call_member< int (FArchiveState::*)() const,&FArchiveState::LicenseeUEVer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:209:20
	makeExtern<DAS_CALL_METHOD(_method_46), SimNode_ExtFuncCall >(lib,"LicenseeUEVer","das_call_member< int (FArchiveState::*)() const , &FArchiveState::LicenseeUEVer >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_47 = das::das_call_member< FEngineVersionBase (FArchiveState::*)() const,&FArchiveState::EngineVer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:229:33
	makeExtern<DAS_CALL_METHOD(_method_47), SimNode_ExtFuncCallAndCopyOrMove >(lib,"EngineVer","das_call_member< FEngineVersionBase (FArchiveState::*)() const , &FArchiveState::EngineVer >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:235:26
	makeExtern< unsigned int (*)(const FArchiveState &) , _dasUnreal_virtual_19_EngineNetVer , SimNode_ExtFuncCall >(lib,"EngineNetVer","_dasUnreal_virtual_19_EngineNetVer")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:238:26
	makeExtern< unsigned int (*)(const FArchiveState &) , _dasUnreal_virtual_20_GameNetVer , SimNode_ExtFuncCall >(lib,"GameNetVer","_dasUnreal_virtual_20_GameNetVer")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_50 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsLoading >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:249:19
	makeExtern<DAS_CALL_METHOD(_method_50), SimNode_ExtFuncCall >(lib,"IsLoading","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsLoading >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_51 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsLoadingFromCookedPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:255:20
	makeExtern<DAS_CALL_METHOD(_method_51), SimNode_ExtFuncCall >(lib,"IsLoadingFromCookedPackage","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsLoadingFromCookedPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_52 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsSaving >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:261:19
	makeExtern<DAS_CALL_METHOD(_method_52), SimNode_ExtFuncCall >(lib,"IsSaving","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsSaving >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_53 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsTransacting >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:267:19
	makeExtern<DAS_CALL_METHOD(_method_53), SimNode_ExtFuncCall >(lib,"IsTransacting","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsTransacting >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_54 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsTextFormat >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:283:19
	makeExtern<DAS_CALL_METHOD(_method_54), SimNode_ExtFuncCall >(lib,"IsTextFormat","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsTextFormat >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_55 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::WantBinaryPropertySerialization >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:289:19
	makeExtern<DAS_CALL_METHOD(_method_55), SimNode_ExtFuncCall >(lib,"WantBinaryPropertySerialization","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::WantBinaryPropertySerialization >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_56 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::UseUnversionedPropertySerialization >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:298:19
	makeExtern<DAS_CALL_METHOD(_method_56), SimNode_ExtFuncCall >(lib,"UseUnversionedPropertySerialization","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::UseUnversionedPropertySerialization >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_57 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsForcingUnicode >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:304:19
	makeExtern<DAS_CALL_METHOD(_method_57), SimNode_ExtFuncCall >(lib,"IsForcingUnicode","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsForcingUnicode >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_58 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsPersistent >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:313:19
	makeExtern<DAS_CALL_METHOD(_method_58), SimNode_ExtFuncCall >(lib,"IsPersistent","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsPersistent >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_59 = das::das_call_member< void (FArchiveState::*)(bool),&FArchiveState::SetShouldSkipCompilingAssets >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:323:7
	makeExtern<DAS_CALL_METHOD(_method_59), SimNode_ExtFuncCall >(lib,"SetShouldSkipCompilingAssets","das_call_member< void (FArchiveState::*)(bool) , &FArchiveState::SetShouldSkipCompilingAssets >::invoke")
		->args({"self","Enabled"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_60 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::ShouldSkipCompilingAssets >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:331:19
	makeExtern<DAS_CALL_METHOD(_method_60), SimNode_ExtFuncCall >(lib,"ShouldSkipCompilingAssets","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::ShouldSkipCompilingAssets >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_61 = das::das_call_member< void (FArchiveState::*)(bool),&FArchiveState::SetShouldSkipUpdateCustomVersion >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:341:16
	makeExtern<DAS_CALL_METHOD(_method_61), SimNode_ExtFuncCall >(lib,"SetShouldSkipUpdateCustomVersion","das_call_member< void (FArchiveState::*)(bool) , &FArchiveState::SetShouldSkipUpdateCustomVersion >::invoke")
		->args({"self","bShouldSkip"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_62 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::ShouldSkipUpdateCustomVersion >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:344:19
	makeExtern<DAS_CALL_METHOD(_method_62), SimNode_ExtFuncCall >(lib,"ShouldSkipUpdateCustomVersion","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::ShouldSkipUpdateCustomVersion >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_63 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsError >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:350:19
	makeExtern<DAS_CALL_METHOD(_method_63), SimNode_ExtFuncCall >(lib,"IsError","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsError >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_64 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::GetError >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:355:19
	makeExtern<DAS_CALL_METHOD(_method_64), SimNode_ExtFuncCall >(lib,"GetError","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::GetError >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_65 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsCriticalError >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:361:19
	makeExtern<DAS_CALL_METHOD(_method_65), SimNode_ExtFuncCall >(lib,"IsCriticalError","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsCriticalError >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_66 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::ContainsCode >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:367:19
	makeExtern<DAS_CALL_METHOD(_method_66), SimNode_ExtFuncCall >(lib,"ContainsCode","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::ContainsCode >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_67 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::ContainsMap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:373:19
	makeExtern<DAS_CALL_METHOD(_method_67), SimNode_ExtFuncCall >(lib,"ContainsMap","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::ContainsMap >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_68 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::RequiresLocalizationGather >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:379:19
	makeExtern<DAS_CALL_METHOD(_method_68), SimNode_ExtFuncCall >(lib,"RequiresLocalizationGather","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::RequiresLocalizationGather >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_69 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::ForceByteSwapping >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:385:19
	makeExtern<DAS_CALL_METHOD(_method_69), SimNode_ExtFuncCall >(lib,"ForceByteSwapping","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::ForceByteSwapping >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_70 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsSerializingDefaults >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:391:19
	makeExtern<DAS_CALL_METHOD(_method_70), SimNode_ExtFuncCall >(lib,"IsSerializingDefaults","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsSerializingDefaults >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_71 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsIgnoringArchetypeRef >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:397:19
	makeExtern<DAS_CALL_METHOD(_method_71), SimNode_ExtFuncCall >(lib,"IsIgnoringArchetypeRef","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsIgnoringArchetypeRef >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_72 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::DoDelta >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:403:19
	makeExtern<DAS_CALL_METHOD(_method_72), SimNode_ExtFuncCall >(lib,"DoDelta","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::DoDelta >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_73 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::DoIntraPropertyDelta >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:409:19
	makeExtern<DAS_CALL_METHOD(_method_73), SimNode_ExtFuncCall >(lib,"DoIntraPropertyDelta","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::DoIntraPropertyDelta >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_74 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsIgnoringOuterRef >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:415:19
	makeExtern<DAS_CALL_METHOD(_method_74), SimNode_ExtFuncCall >(lib,"IsIgnoringOuterRef","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsIgnoringOuterRef >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_75 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsIgnoringClassGeneratedByRef >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:421:19
	makeExtern<DAS_CALL_METHOD(_method_75), SimNode_ExtFuncCall >(lib,"IsIgnoringClassGeneratedByRef","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsIgnoringClassGeneratedByRef >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_76 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsIgnoringClassRef >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:427:19
	makeExtern<DAS_CALL_METHOD(_method_76), SimNode_ExtFuncCall >(lib,"IsIgnoringClassRef","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsIgnoringClassRef >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_77 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsAllowingLazyLoading >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:433:19
	makeExtern<DAS_CALL_METHOD(_method_77), SimNode_ExtFuncCall >(lib,"IsAllowingLazyLoading","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsAllowingLazyLoading >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_78 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsObjectReferenceCollector >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:442:19
	makeExtern<DAS_CALL_METHOD(_method_78), SimNode_ExtFuncCall >(lib,"IsObjectReferenceCollector","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsObjectReferenceCollector >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_79 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsModifyingWeakAndStrongReferences >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:448:19
	makeExtern<DAS_CALL_METHOD(_method_79), SimNode_ExtFuncCall >(lib,"IsModifyingWeakAndStrongReferences","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsModifyingWeakAndStrongReferences >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_80 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsCountingMemory >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:454:19
	makeExtern<DAS_CALL_METHOD(_method_80), SimNode_ExtFuncCall >(lib,"IsCountingMemory","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsCountingMemory >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_81 = das::das_call_member< unsigned int (FArchiveState::*)() const,&FArchiveState::GetPortFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:460:21
	makeExtern<DAS_CALL_METHOD(_method_81), SimNode_ExtFuncCall >(lib,"GetPortFlags","das_call_member< unsigned int (FArchiveState::*)() const , &FArchiveState::GetPortFlags >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_82 = das::das_call_member< bool (FArchiveState::*)(unsigned int) const,&FArchiveState::HasAnyPortFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:466:19
	makeExtern<DAS_CALL_METHOD(_method_82), SimNode_ExtFuncCall >(lib,"HasAnyPortFlags","das_call_member< bool (FArchiveState::*)(unsigned int) const , &FArchiveState::HasAnyPortFlags >::invoke")
		->args({"self","Flags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_83 = das::das_call_member< bool (FArchiveState::*)(unsigned int) const,&FArchiveState::HasAllPortFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:472:19
	makeExtern<DAS_CALL_METHOD(_method_83), SimNode_ExtFuncCall >(lib,"HasAllPortFlags","das_call_member< bool (FArchiveState::*)(unsigned int) const , &FArchiveState::HasAllPortFlags >::invoke")
		->args({"self","Flags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_84 = das::das_call_member< unsigned int (FArchiveState::*)() const,&FArchiveState::GetDebugSerializationFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:478:21
	makeExtern<DAS_CALL_METHOD(_method_84), SimNode_ExtFuncCall >(lib,"GetDebugSerializationFlags","das_call_member< unsigned int (FArchiveState::*)() const , &FArchiveState::GetDebugSerializationFlags >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_85 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::ShouldSkipBulkData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:488:19
	makeExtern<DAS_CALL_METHOD(_method_85), SimNode_ExtFuncCall >(lib,"ShouldSkipBulkData","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::ShouldSkipBulkData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_86 = das::das_call_member< int64 (FArchiveState::*)() const,&FArchiveState::GetMaxSerializeSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:494:20
	makeExtern<DAS_CALL_METHOD(_method_86), SimNode_ExtFuncCall >(lib,"GetMaxSerializeSize","das_call_member< int64 (FArchiveState::*)() const , &FArchiveState::GetMaxSerializeSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:515:24
	makeExtern< void (*)(FArchiveState &) , _dasUnreal_virtual_21_ResetCustomVersions , SimNode_ExtFuncCall >(lib,"ResetCustomVersions","_dasUnreal_virtual_21_ResetCustomVersions")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_88 = das::das_call_member< void (FArchiveState::*)(bool),&FArchiveState::SetByteSwapping >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:532:7
	makeExtern<DAS_CALL_METHOD(_method_88), SimNode_ExtFuncCall >(lib,"SetByteSwapping","das_call_member< void (FArchiveState::*)(bool) , &FArchiveState::SetByteSwapping >::invoke")
		->args({"self","Enabled"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_89 = das::das_call_member< void (FArchiveState::*)(unsigned int),&FArchiveState::SetPortFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:542:7
	makeExtern<DAS_CALL_METHOD(_method_89), SimNode_ExtFuncCall >(lib,"SetPortFlags","das_call_member< void (FArchiveState::*)(unsigned int) , &FArchiveState::SetPortFlags >::invoke")
		->args({"self","InPortFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_90 = das::das_call_member< void (FArchiveState::*)(unsigned int),&FArchiveState::SetDebugSerializationFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:552:7
	makeExtern<DAS_CALL_METHOD(_method_90), SimNode_ExtFuncCall >(lib,"SetDebugSerializationFlags","das_call_member< void (FArchiveState::*)(unsigned int) , &FArchiveState::SetDebugSerializationFlags >::invoke")
		->args({"self","InCustomFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_91 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsFilterEditorOnly >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:564:7
	makeExtern<DAS_CALL_METHOD(_method_91), SimNode_ExtFuncCall >(lib,"IsFilterEditorOnly","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsFilterEditorOnly >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:574:15
	makeExtern< void (*)(FArchiveState &,bool) , _dasUnreal_virtual_22_SetFilterEditorOnly , SimNode_ExtFuncCall >(lib,"SetFilterEditorOnly","_dasUnreal_virtual_22_SetFilterEditorOnly")
		->args({"self","InFilterEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_93 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsSaveGame >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:585:7
	makeExtern<DAS_CALL_METHOD(_method_93), SimNode_ExtFuncCall >(lib,"IsSaveGame","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsSaveGame >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_94 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsNetArchive >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:593:19
	makeExtern<DAS_CALL_METHOD(_method_94), SimNode_ExtFuncCall >(lib,"IsNetArchive","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsNetArchive >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_95 = das::das_call_member< bool (FArchiveState::*)() const,&FArchiveState::IsCooking >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:603:19
	makeExtern<DAS_CALL_METHOD(_method_95), SimNode_ExtFuncCall >(lib,"IsCooking","das_call_member< bool (FArchiveState::*)() const , &FArchiveState::IsCooking >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_96 = das::das_call_member< FArchiveCookContext * (FArchiveState::*)(),&FArchiveState::GetCookContext >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:629:35
	makeExtern<DAS_CALL_METHOD(_method_96), SimNode_ExtFuncCall >(lib,"GetCookContext","das_call_member< FArchiveCookContext * (FArchiveState::*)() , &FArchiveState::GetCookContext >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

