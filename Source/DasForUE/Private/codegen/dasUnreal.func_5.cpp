// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_5() {
	using _method_259 = das::das_call_member< bool (UObject::*)(),&UObject::ConditionalFinishDestroy >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1109:23
	makeExtern<DAS_CALL_METHOD(_method_259), SimNode_ExtFuncCall >(lib,"ConditionalFinishDestroy","das_call_member< bool (UObject::*)() , &UObject::ConditionalFinishDestroy >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_260 = das::das_call_member< void (UObject::*)(),&UObject::ConditionalPostLoad >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1112:23
	makeExtern<DAS_CALL_METHOD(_method_260), SimNode_ExtFuncCall >(lib,"ConditionalPostLoad","das_call_member< void (UObject::*)() , &UObject::ConditionalPostLoad >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1143:15
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_90_WillNeverCacheCookedPlatformDataAgain , SimNode_ExtFuncCall >(lib,"WillNeverCacheCookedPlatformDataAgain","_dasUnreal_virtual_90_WillNeverCacheCookedPlatformDataAgain")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1157:15
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_91_ClearAllCachedCookedPlatformData , SimNode_ExtFuncCall >(lib,"ClearAllCachedCookedPlatformData","_dasUnreal_virtual_91_ClearAllCachedCookedPlatformData")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_263 = das::das_call_member< bool (UObject::*)(const UObject *const) const,&UObject::IsBasedOnArchetype >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1194:14
	makeExtern<DAS_CALL_METHOD(_method_263), SimNode_ExtFuncCall >(lib,"IsBasedOnArchetype","das_call_member< bool (UObject::*)(const UObject *const) const , &UObject::IsBasedOnArchetype >::invoke")
		->args({"self","SomeObject"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_264 = das::das_call_member< UFunction * (UObject::*)(FName) const,&UObject::FindFunction >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1197:29
	makeExtern<DAS_CALL_METHOD(_method_264), SimNode_ExtFuncCall >(lib,"FindFunction","das_call_member< UFunction * (UObject::*)(FName) const , &UObject::FindFunction >::invoke")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_265 = das::das_call_member< UFunction * (UObject::*)(FName) const,&UObject::FindFunctionChecked >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1200:29
	makeExtern<DAS_CALL_METHOD(_method_265), SimNode_ExtFuncCall >(lib,"FindFunctionChecked","das_call_member< UFunction * (UObject::*)(FName) const , &UObject::FindFunctionChecked >::invoke")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_266 = das::das_call_member< bool (UObject::*)(bool) const,&UObject::CheckDefaultSubobjects >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1225:23
	makeExtern<DAS_CALL_METHOD(_method_266), SimNode_ExtFuncCall >(lib,"CheckDefaultSubobjects","das_call_member< bool (UObject::*)(bool) const , &UObject::CheckDefaultSubobjects >::invoke")
		->args({"self","bForceCheck"})
		->arg_init(1,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_267 = das::das_call_member< void (UObject::*)(const FString &),&UObject::UpdateDefaultConfigFile >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1237:23
	makeExtern<DAS_CALL_METHOD(_method_267), SimNode_ExtFuncCall >(lib,"UpdateDefaultConfigFile","das_call_member< void (UObject::*)(const FString &) , &UObject::UpdateDefaultConfigFile >::invoke")
		->args({"self","SpecificFileLocation"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_268 = das::das_call_member< bool (UObject::*)(const FString &,bool),&UObject::TryUpdateDefaultConfigFile >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1246:23
	makeExtern<DAS_CALL_METHOD(_method_268), SimNode_ExtFuncCall >(lib,"TryUpdateDefaultConfigFile","das_call_member< bool (UObject::*)(const FString &,bool) , &UObject::TryUpdateDefaultConfigFile >::invoke")
		->args({"self","SpecificFileLocation","bWarnIfFail"})
		->arg_init(2,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_269 = das::das_call_member< void (UObject::*)(),&UObject::UpdateGlobalUserConfigFile >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1251:23
	makeExtern<DAS_CALL_METHOD(_method_269), SimNode_ExtFuncCall >(lib,"UpdateGlobalUserConfigFile","das_call_member< void (UObject::*)() , &UObject::UpdateGlobalUserConfigFile >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_270 = das::das_call_member< void (UObject::*)(),&UObject::UpdateProjectUserConfigFile >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1256:23
	makeExtern<DAS_CALL_METHOD(_method_270), SimNode_ExtFuncCall >(lib,"UpdateProjectUserConfigFile","das_call_member< void (UObject::*)() , &UObject::UpdateProjectUserConfigFile >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_271 = das::das_call_member< FString (UObject::*)() const,&UObject::GetDefaultConfigFilename >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1279:26
	makeExtern<DAS_CALL_METHOD(_method_271), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDefaultConfigFilename","das_call_member< FString (UObject::*)() const , &UObject::GetDefaultConfigFilename >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_272 = das::das_call_member< FString (UObject::*)() const,&UObject::GetGlobalUserConfigFilename >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1284:26
	makeExtern<DAS_CALL_METHOD(_method_272), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetGlobalUserConfigFilename","das_call_member< FString (UObject::*)() const , &UObject::GetGlobalUserConfigFilename >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_273 = das::das_call_member< FString (UObject::*)() const,&UObject::GetProjectUserConfigFilename >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1289:26
	makeExtern<DAS_CALL_METHOD(_method_273), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetProjectUserConfigFilename","das_call_member< FString (UObject::*)() const , &UObject::GetProjectUserConfigFilename >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1292:23
	makeExtern< const wchar_t * (*)(const UObject &) , _dasUnreal_virtual_92_GetConfigOverridePlatform , SimNode_ExtFuncCall >(lib,"GetConfigOverridePlatform","_dasUnreal_virtual_92_GetConfigOverridePlatform")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1299:15
	makeExtern< void (*)(UObject &,FString &) , _dasUnreal_virtual_93_OverrideConfigSection , SimNode_ExtFuncCall >(lib,"OverrideConfigSection","_dasUnreal_virtual_93_OverrideConfigSection")
		->args({"self","SectionName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1306:15
	makeExtern< void (*)(UObject &,FString &) , _dasUnreal_virtual_94_OverridePerObjectConfigSection , SimNode_ExtFuncCall >(lib,"OverridePerObjectConfigSection","_dasUnreal_virtual_94_OverridePerObjectConfigSection")
		->args({"self","SectionName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_277 = das::das_call_member< void (UObject::*)(const wchar_t *),&UObject::ParseParms >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1330:23
	makeExtern<DAS_CALL_METHOD(_method_277), SimNode_ExtFuncCall >(lib,"ParseParms","das_call_member< void (UObject::*)(const wchar_t *) , &UObject::ParseParms >::invoke")
		->args({"self","Parms"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1358:34
	makeExtern< UObject * (*)(const UObject *,const UClass *,const UObject *,FName,EObjectFlags) , _dasUnreal_static_95_GetArchetypeFromRequiredInfo , SimNode_ExtFuncCall >(lib,"GetArchetypeFromRequiredInfo","_dasUnreal_static_95_GetArchetypeFromRequiredInfo")
		->args({"self","Class","Outer","Name","ObjectFlags"})
		->arg_type(0,makeType<TExplicit<const UObject*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_279 = das::das_call_member< UObject * (UObject::*)() const,&UObject::GetArchetype >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1365:27
	makeExtern<DAS_CALL_METHOD(_method_279), SimNode_ExtFuncCall >(lib,"GetArchetype","das_call_member< UObject * (UObject::*)() const , &UObject::GetArchetype >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1391:31
	makeExtern< void (*)(UObject &,UFunction *,void *) , _dasUnreal_virtual_96_ProcessEvent , SimNode_ExtFuncCall >(lib,"ProcessEvent","_dasUnreal_virtual_96_ProcessEvent")
		->args({"self","Function","Parms"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1450:18
	makeExtern< UClass * (*)(UObject &,UClass *,UObject *) , _dasUnreal_virtual_97_RegenerateClass , SimNode_ExtFuncCall >(lib,"RegenerateClass","_dasUnreal_virtual_97_RegenerateClass")
		->args({"self","ClassToRegenerate","PreviousCDO"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_282 = das::das_call_member< bool (UObject::*)() const,&UObject::IsInBlueprint >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1455:23
	makeExtern<DAS_CALL_METHOD(_method_282), SimNode_ExtFuncCall >(lib,"IsInBlueprint","das_call_member< bool (UObject::*)() const , &UObject::IsInBlueprint >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_283 = das::das_call_member< void (UObject::*)(),&UObject::DestroyNonNativeProperties >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1460:23
	makeExtern<DAS_CALL_METHOD(_method_283), SimNode_ExtFuncCall >(lib,"DestroyNonNativeProperties","das_call_member< void (UObject::*)() , &UObject::DestroyNonNativeProperties >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1463:15
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_98_MarkAsEditorOnlySubobject , SimNode_ExtFuncCall >(lib,"MarkAsEditorOnlySubobject","_dasUnreal_virtual_98_MarkAsEditorOnlySubobject")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_285 = das::das_call_member< void (UObject::*)() const,&UObject::AbortInsideMemberFunction >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1468:23
	makeExtern<DAS_CALL_METHOD(_method_285), SimNode_ExtFuncCall >(lib,"AbortInsideMemberFunction","das_call_member< void (UObject::*)() const , &UObject::AbortInsideMemberFunction >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_286 = das::das_call_member< void (UObject::*)(int),&UObject::ExecuteUbergraph >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1664:7
	makeExtern<DAS_CALL_METHOD(_method_286), SimNode_ExtFuncCall >(lib,"ExecuteUbergraph","das_call_member< void (UObject::*)(int) , &UObject::ExecuteUbergraph >::invoke")
		->args({"self","EntryPoint"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1717:17
	makeExtern< uint64 (*)(const UObject &) , _dasUnreal_virtual_99_GetNetPushIdDynamic , SimNode_ExtFuncCall >(lib,"GetNetPushIdDynamic","_dasUnreal_virtual_99_GetNetPushIdDynamic")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1771:18
	makeExtern< bool (*)(const UObject *) , IsValid , SimNode_ExtFuncCall >(lib,"IsValid","IsValid")
		->args({"Test"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1782:18
	makeExtern< bool (*)(const UObject *) , IsValidChecked , SimNode_ExtFuncCall >(lib,"IsValidChecked","IsValidChecked")
		->args({"Test"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:157:2
	makeExtern< UClass * (*)(const UField *) , _dasUnreal_static_100_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_100_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UField*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:157:2
	makeExtern< const wchar_t * (*)(const UField *) , _dasUnreal_static_101_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_101_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UField*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:157:2
	makeExtern< void (*)(const UField *) , _dasUnreal_static_102_StaticRegisterNativesUField , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUField","_dasUnreal_static_102_StaticRegisterNativesUField")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UField*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:170:31
	makeExtern< void (*)(UField &) , _dasUnreal_virtual_103_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_103_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:171:31
	makeExtern< bool (*)(const UField &) , _dasUnreal_virtual_104_NeedsLoadForClient , SimNode_ExtFuncCall >(lib,"NeedsLoadForClient","_dasUnreal_virtual_104_NeedsLoadForClient")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:172:31
	makeExtern< bool (*)(const UField &) , _dasUnreal_virtual_105_NeedsLoadForServer , SimNode_ExtFuncCall >(lib,"NeedsLoadForServer","_dasUnreal_virtual_105_NeedsLoadForServer")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:176:31
	makeExtern< void (*)(UField &) , _dasUnreal_virtual_106_Bind , SimNode_ExtFuncCall >(lib,"Bind","_dasUnreal_virtual_106_Bind")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_295 = das::das_call_member< UClass * (UField::*)() const,&UField::GetOwnerClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:179:26
	makeExtern<DAS_CALL_METHOD(_method_295), SimNode_ExtFuncCall >(lib,"GetOwnerClass","das_call_member< UClass * (UField::*)() const , &UField::GetOwnerClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_296 = das::das_call_member< UStruct * (UField::*)() const,&UField::GetOwnerStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:182:27
	makeExtern<DAS_CALL_METHOD(_method_296), SimNode_ExtFuncCall >(lib,"GetOwnerStruct","das_call_member< UStruct * (UField::*)() const , &UField::GetOwnerStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_297 = das::das_call_member< FString (UField::*)() const,&UField::GetAuthoredName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:189:26
	makeExtern<DAS_CALL_METHOD(_method_297), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredName","das_call_member< FString (UField::*)() const , &UField::GetAuthoredName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_298 = das::das_call_member< FText (UField::*)() const,&UField::GetDisplayNameText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:197:24
	makeExtern<DAS_CALL_METHOD(_method_298), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDisplayNameText","das_call_member< FText (UField::*)() const , &UField::GetDisplayNameText >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_299 = das::das_call_member< FText (UField::*)(bool) const,&UField::GetToolTipText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:206:24
	makeExtern<DAS_CALL_METHOD(_method_299), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetToolTipText","das_call_member< FText (UField::*)(bool) const , &UField::GetToolTipText >::invoke")
		->args({"self","bShortTooltip"})
		->arg_init(1,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:214:30
	makeExtern< void (*)(const UField *,FString &,bool) , _dasUnreal_static_107_FormatNativeToolTip , SimNode_ExtFuncCall >(lib,"FormatNativeToolTip","_dasUnreal_static_107_FormatNativeToolTip")
		->args({"self","ToolTipString","bRemoveExtraSections"})
		->arg_type(0,makeType<TExplicit<const UField*> >(lib))
		->arg_init(2,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_301 = das::das_call_member< bool (UField::*)(const wchar_t *) const,&UField::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:222:7
	makeExtern<DAS_CALL_METHOD(_method_301), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (UField::*)(const wchar_t *) const , &UField::HasMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_302 = das::das_call_member< bool (UField::*)(const FName &) const,&UField::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:223:7
	makeExtern<DAS_CALL_METHOD(_method_302), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (UField::*)(const FName &) const , &UField::HasMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_303 = das::das_call_member< const FString * (UField::*)(const wchar_t *) const,&UField::FindMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:231:33
	makeExtern<DAS_CALL_METHOD(_method_303), SimNode_ExtFuncCall >(lib,"FindMetaData","das_call_member< const FString * (UField::*)(const wchar_t *) const , &UField::FindMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_304 = das::das_call_member< const FString * (UField::*)(const FName &) const,&UField::FindMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:232:33
	makeExtern<DAS_CALL_METHOD(_method_304), SimNode_ExtFuncCall >(lib,"FindMetaData","das_call_member< const FString * (UField::*)(const FName &) const , &UField::FindMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_305 = das::das_call_member< const FString & (UField::*)(const wchar_t *) const,&UField::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:240:33
	makeExtern<DAS_CALL_METHOD(_method_305), SimNode_ExtFuncCallRef >(lib,"GetMetaData","das_call_member< const FString & (UField::*)(const wchar_t *) const , &UField::GetMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_306 = das::das_call_member< const FString & (UField::*)(const FName &) const,&UField::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:241:33
	makeExtern<DAS_CALL_METHOD(_method_306), SimNode_ExtFuncCallRef >(lib,"GetMetaData","das_call_member< const FString & (UField::*)(const FName &) const , &UField::GetMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_307 = das::das_call_member< FText (UField::*)(const wchar_t *,const FString,const FString) const,&UField::GetMetaDataText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:251:24
	makeExtern<DAS_CALL_METHOD(_method_307), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetMetaDataText","das_call_member< FText (UField::*)(const wchar_t *,const FString,const FString) const , &UField::GetMetaDataText >::invoke")
		->args({"self","MetaDataKey","LocalizationNamespace","LocalizationKey"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_308 = das::das_call_member< FText (UField::*)(const FName &,const FString,const FString) const,&UField::GetMetaDataText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:252:24
	makeExtern<DAS_CALL_METHOD(_method_308), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetMetaDataText","das_call_member< FText (UField::*)(const FName &,const FString,const FString) const , &UField::GetMetaDataText >::invoke")
		->args({"self","MetaDataKey","LocalizationNamespace","LocalizationKey"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_309 = das::das_call_member< void (UField::*)(const wchar_t *,const wchar_t *),&UField::SetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:260:23
	makeExtern<DAS_CALL_METHOD(_method_309), SimNode_ExtFuncCall >(lib,"SetMetaData","das_call_member< void (UField::*)(const wchar_t *,const wchar_t *) , &UField::SetMetaData >::invoke")
		->args({"self","Key","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_310 = das::das_call_member< void (UField::*)(const FName &,const wchar_t *),&UField::SetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:261:23
	makeExtern<DAS_CALL_METHOD(_method_310), SimNode_ExtFuncCall >(lib,"SetMetaData","das_call_member< void (UField::*)(const FName &,const wchar_t *) , &UField::SetMetaData >::invoke")
		->args({"self","Key","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_311 = das::das_call_member< bool (UField::*)(const wchar_t *) const,&UField::GetBoolMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:269:7
	makeExtern<DAS_CALL_METHOD(_method_311), SimNode_ExtFuncCall >(lib,"GetBoolMetaData","das_call_member< bool (UField::*)(const wchar_t *) const , &UField::GetBoolMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_312 = das::das_call_member< bool (UField::*)(const FName &) const,&UField::GetBoolMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:275:7
	makeExtern<DAS_CALL_METHOD(_method_312), SimNode_ExtFuncCall >(lib,"GetBoolMetaData","das_call_member< bool (UField::*)(const FName &) const , &UField::GetBoolMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_313 = das::das_call_member< int (UField::*)(const wchar_t *) const,&UField::GetIntMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:288:8
	makeExtern<DAS_CALL_METHOD(_method_313), SimNode_ExtFuncCall >(lib,"GetIntMetaData","das_call_member< int (UField::*)(const wchar_t *) const , &UField::GetIntMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_314 = das::das_call_member< int (UField::*)(const FName &) const,&UField::GetIntMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:294:8
	makeExtern<DAS_CALL_METHOD(_method_314), SimNode_ExtFuncCall >(lib,"GetIntMetaData","das_call_member< int (UField::*)(const FName &) const , &UField::GetIntMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_315 = das::das_call_member< float (UField::*)(const wchar_t *) const,&UField::GetFloatMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:307:8
	makeExtern<DAS_CALL_METHOD(_method_315), SimNode_ExtFuncCall >(lib,"GetFloatMetaData","das_call_member< float (UField::*)(const wchar_t *) const , &UField::GetFloatMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_316 = das::das_call_member< float (UField::*)(const FName &) const,&UField::GetFloatMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:313:8
	makeExtern<DAS_CALL_METHOD(_method_316), SimNode_ExtFuncCall >(lib,"GetFloatMetaData","das_call_member< float (UField::*)(const FName &) const , &UField::GetFloatMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_317 = das::das_call_member< UClass * (UField::*)(const wchar_t *) const,&UField::GetClassMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:326:26
	makeExtern<DAS_CALL_METHOD(_method_317), SimNode_ExtFuncCall >(lib,"GetClassMetaData","das_call_member< UClass * (UField::*)(const wchar_t *) const , &UField::GetClassMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_318 = das::das_call_member< UClass * (UField::*)(const FName &) const,&UField::GetClassMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:327:26
	makeExtern<DAS_CALL_METHOD(_method_318), SimNode_ExtFuncCall >(lib,"GetClassMetaData","das_call_member< UClass * (UField::*)(const FName &) const , &UField::GetClassMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_319 = das::das_call_member< void (UField::*)(const wchar_t *),&UField::RemoveMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:330:23
	makeExtern<DAS_CALL_METHOD(_method_319), SimNode_ExtFuncCall >(lib,"RemoveMetaData","das_call_member< void (UField::*)(const wchar_t *) , &UField::RemoveMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_320 = das::das_call_member< void (UField::*)(const FName &),&UField::RemoveMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:331:23
	makeExtern<DAS_CALL_METHOD(_method_320), SimNode_ExtFuncCall >(lib,"RemoveMetaData","das_call_member< void (UField::*)(const FName &) , &UField::RemoveMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_321 = das::das_call_member< bool (UField::*)(const uint64) const,&UField::HasAnyCastFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:334:23
	makeExtern<DAS_CALL_METHOD(_method_321), SimNode_ExtFuncCall >(lib,"HasAnyCastFlags","das_call_member< bool (UField::*)(const uint64) const , &UField::HasAnyCastFlags >::invoke")
		->args({"self","InCastFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_322 = das::das_call_member< bool (UField::*)(const uint64) const,&UField::HasAllCastFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:335:23
	makeExtern<DAS_CALL_METHOD(_method_322), SimNode_ExtFuncCall >(lib,"HasAllCastFlags","das_call_member< bool (UField::*)(const uint64) const , &UField::HasAllCastFlags >::invoke")
		->args({"self","InCastFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:388:2
	makeExtern< UClass * (*)(const UStruct *) , _dasUnreal_static_108_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_108_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UStruct*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:388:2
	makeExtern< const wchar_t * (*)(const UStruct *) , _dasUnreal_static_109_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_109_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UStruct*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:388:2
	makeExtern< void (*)(const UStruct *) , _dasUnreal_static_110_StaticRegisterNativesUStruct , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUStruct","_dasUnreal_static_110_StaticRegisterNativesUStruct")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UStruct*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_326 = das::das_call_member< const FBlake3Hash & (UStruct::*)(bool) const,&UStruct::GetSchemaHash >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:441:37
	makeExtern<DAS_CALL_METHOD(_method_326), SimNode_ExtFuncCallRef >(lib,"GetSchemaHash","das_call_member< const FBlake3Hash & (UStruct::*)(bool) const , &UStruct::GetSchemaHash >::invoke")
		->args({"self","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:458:31
	makeExtern< void (*)(UStruct &) , _dasUnreal_virtual_111_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_111_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:459:31
	makeExtern< void (*)(UStruct &) , _dasUnreal_virtual_112_FinishDestroy , SimNode_ExtFuncCall >(lib,"FinishDestroy","_dasUnreal_virtual_112_FinishDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:460:31
	makeExtern< void (*)(UStruct &) , _dasUnreal_virtual_113_RegisterDependencies , SimNode_ExtFuncCall >(lib,"RegisterDependencies","_dasUnreal_virtual_113_RegisterDependencies")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:463:31
	makeExtern< void (*)(UStruct &,EObjectFlags) , _dasUnreal_virtual_114_TagSubobjects , SimNode_ExtFuncCall >(lib,"TagSubobjects","_dasUnreal_virtual_114_TagSubobjects")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:488:19
	makeExtern< UStruct * (*)(const UStruct &) , _dasUnreal_virtual_115_GetInheritanceSuper , SimNode_ExtFuncCall >(lib,"GetInheritanceSuper","_dasUnreal_virtual_115_GetInheritanceSuper")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_332 = das::das_call_member< void (UStruct::*)(bool),&UStruct::StaticLink >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:491:23
	makeExtern<DAS_CALL_METHOD(_method_332), SimNode_ExtFuncCall >(lib,"StaticLink","das_call_member< void (UStruct::*)(bool) , &UStruct::StaticLink >::invoke")
		->args({"self","bRelinkExistingProperties"})
		->arg_init(1,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:562:31
	makeExtern< void (*)(const UStruct &,void *,int) , _dasUnreal_virtual_116_InitializeStruct , SimNode_ExtFuncCall >(lib,"InitializeStruct","_dasUnreal_virtual_116_InitializeStruct")
		->args({"self","Dest","ArrayDim"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:571:31
	makeExtern< void (*)(const UStruct &,void *,int) , _dasUnreal_virtual_117_DestroyStruct , SimNode_ExtFuncCall >(lib,"DestroyStruct","_dasUnreal_virtual_117_DestroyStruct")
		->args({"self","Dest","ArrayDim"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:584:23
	makeExtern< const wchar_t * (*)(const UStruct &) , _dasUnreal_virtual_118_GetPrefixCPP , SimNode_ExtFuncCall >(lib,"GetPrefixCPP","_dasUnreal_virtual_118_GetPrefixCPP")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_336 = das::das_call_member< int (UStruct::*)() const,&UStruct::GetPropertiesSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:587:20
	makeExtern<DAS_CALL_METHOD(_method_336), SimNode_ExtFuncCall >(lib,"GetPropertiesSize","das_call_member< int (UStruct::*)() const , &UStruct::GetPropertiesSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_337 = das::das_call_member< int (UStruct::*)() const,&UStruct::GetMinAlignment >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:593:20
	makeExtern<DAS_CALL_METHOD(_method_337), SimNode_ExtFuncCall >(lib,"GetMinAlignment","das_call_member< int (UStruct::*)() const , &UStruct::GetMinAlignment >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_338 = das::das_call_member< int (UStruct::*)() const,&UStruct::GetStructureSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:599:20
	makeExtern<DAS_CALL_METHOD(_method_338), SimNode_ExtFuncCall >(lib,"GetStructureSize","das_call_member< int (UStruct::*)() const , &UStruct::GetStructureSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_339 = das::das_call_member< void (UStruct::*)(int),&UStruct::SetPropertiesSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:605:7
	makeExtern<DAS_CALL_METHOD(_method_339), SimNode_ExtFuncCall >(lib,"SetPropertiesSize","das_call_member< void (UStruct::*)(int) , &UStruct::SetPropertiesSize >::invoke")
		->args({"self","NewSize"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_340 = das::das_call_member< bool (UStruct::*)(const UStruct *) const,&UStruct::IsChildOf >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:619:23
	makeExtern<DAS_CALL_METHOD(_method_340), SimNode_ExtFuncCall >(lib,"IsChildOf","das_call_member< bool (UStruct::*)(const UStruct *) const , &UStruct::IsChildOf >::invoke")
		->args({"self","SomeBase"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_341 = das::das_call_member< UStruct * (UStruct::*)() const,&UStruct::GetSuperStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:628:11
	makeExtern<DAS_CALL_METHOD(_method_341), SimNode_ExtFuncCall >(lib,"GetSuperStruct","das_call_member< UStruct * (UStruct::*)() const , &UStruct::GetSuperStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:637:31
	makeExtern< void (*)(UStruct &,UStruct *) , _dasUnreal_virtual_119_SetSuperStruct , SimNode_ExtFuncCall >(lib,"SetSuperStruct","_dasUnreal_virtual_119_SetSuperStruct")
		->args({"self","NewSuperStruct"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:640:34
	makeExtern< FString (*)(const UStruct &,const UField *) , _dasUnreal_virtual_120_GetAuthoredNameForField , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredNameForField","_dasUnreal_virtual_120_GetAuthoredNameForField")
		->args({"self","Field"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:646:15
	makeExtern< bool (*)(const UStruct &) , _dasUnreal_virtual_121_IsStructTrashed , SimNode_ExtFuncCall >(lib,"IsStructTrashed","_dasUnreal_virtual_121_IsStructTrashed")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_345 = das::das_call_member< void (UStruct::*)(),&UStruct::DestroyChildPropertiesAndResetPropertyLinks >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:652:23
	makeExtern<DAS_CALL_METHOD(_method_345), SimNode_ExtFuncCall >(lib,"DestroyChildPropertiesAndResetPropertyLinks","das_call_member< void (UStruct::*)() , &UStruct::DestroyChildPropertiesAndResetPropertyLinks >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_346 = das::das_call_member< bool (UStruct::*)(const FName &) const,&UStruct::GetBoolMetaDataHierarchical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:656:23
	makeExtern<DAS_CALL_METHOD(_method_346), SimNode_ExtFuncCall >(lib,"GetBoolMetaDataHierarchical","das_call_member< bool (UStruct::*)(const FName &) const , &UStruct::GetBoolMetaDataHierarchical >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_347 = das::das_call_member< bool (UStruct::*)(const FName &,FString *) const,&UStruct::GetStringMetaDataHierarchical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:659:23
	makeExtern<DAS_CALL_METHOD(_method_347), SimNode_ExtFuncCall >(lib,"GetStringMetaDataHierarchical","das_call_member< bool (UStruct::*)(const FName &,FString *) const , &UStruct::GetStringMetaDataHierarchical >::invoke")
		->args({"self","Key","OutValue"})
		->arg_init(2,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_348 = das::das_call_member< const UStruct * (UStruct::*)(const FName &) const,&UStruct::HasMetaDataHierarchical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:667:33
	makeExtern<DAS_CALL_METHOD(_method_348), SimNode_ExtFuncCall >(lib,"HasMetaDataHierarchical","das_call_member< const UStruct * (UStruct::*)(const FName &) const , &UStruct::HasMetaDataHierarchical >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_349 = das::das_call_member< bool (UStruct::*)() const,&UStruct::HasAssetRegistrySearchableProperties >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:670:19
	makeExtern<DAS_CALL_METHOD(_method_349), SimNode_ExtFuncCall >(lib,"HasAssetRegistrySearchableProperties","das_call_member< bool (UStruct::*)() const , &UStruct::HasAssetRegistrySearchableProperties >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_350 = das::das_call_member< void (UStruct::*)(),&UStruct::DeleteUnresolvedScriptProperties >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:687:19
	makeExtern<DAS_CALL_METHOD(_method_350), SimNode_ExtFuncCall >(lib,"DeleteUnresolvedScriptProperties","das_call_member< void (UStruct::*)() , &UStruct::DeleteUnresolvedScriptProperties >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_351 = das::das_call_member< void (UStruct::*)(),&UStruct::CollectBytecodeAndPropertyReferencedObjects >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:709:23
	makeExtern<DAS_CALL_METHOD(_method_351), SimNode_ExtFuncCall >(lib,"CollectBytecodeAndPropertyReferencedObjects","das_call_member< void (UStruct::*)() , &UStruct::CollectBytecodeAndPropertyReferencedObjects >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_352 = das::das_call_member< void (UStruct::*)(),&UStruct::CollectBytecodeAndPropertyReferencedObjectsRecursively >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:713:23
	makeExtern<DAS_CALL_METHOD(_method_352), SimNode_ExtFuncCall >(lib,"CollectBytecodeAndPropertyReferencedObjectsRecursively","das_call_member< void (UStruct::*)() , &UStruct::CollectBytecodeAndPropertyReferencedObjectsRecursively >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_353 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasNoopConstructor >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:951:8
	makeExtern<DAS_CALL_METHOD(_method_353), SimNode_ExtFuncCall >(lib,"HasNoopConstructor","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasNoopConstructor >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_354 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasZeroConstructor >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:956:8
	makeExtern<DAS_CALL_METHOD(_method_354), SimNode_ExtFuncCall >(lib,"HasZeroConstructor","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasZeroConstructor >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_355 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasDestructor >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:965:8
	makeExtern<DAS_CALL_METHOD(_method_355), SimNode_ExtFuncCall >(lib,"HasDestructor","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasDestructor >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_356 = das::das_call_member< int (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::GetSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:972:21
	makeExtern<DAS_CALL_METHOD(_method_356), SimNode_ExtFuncCall >(lib,"GetSize","das_call_member< int (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::GetSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

