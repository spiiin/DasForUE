// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_4() {
	using _method_233 = das::das_call_member< void (UObject::*)(),&UObject::UpdateGlobalUserConfigFile >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1251:23
	makeExtern<DAS_CALL_METHOD(_method_233), SimNode_ExtFuncCall >(lib,"UpdateGlobalUserConfigFile","das_call_member< void (UObject::*)() , &UObject::UpdateGlobalUserConfigFile >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_234 = das::das_call_member< void (UObject::*)(),&UObject::UpdateProjectUserConfigFile >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1256:23
	makeExtern<DAS_CALL_METHOD(_method_234), SimNode_ExtFuncCall >(lib,"UpdateProjectUserConfigFile","das_call_member< void (UObject::*)() , &UObject::UpdateProjectUserConfigFile >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_235 = das::das_call_member< FString (UObject::*)() const,&UObject::GetDefaultConfigFilename >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1279:26
	makeExtern<DAS_CALL_METHOD(_method_235), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDefaultConfigFilename","das_call_member< FString (UObject::*)() const , &UObject::GetDefaultConfigFilename >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_236 = das::das_call_member< FString (UObject::*)() const,&UObject::GetGlobalUserConfigFilename >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1284:26
	makeExtern<DAS_CALL_METHOD(_method_236), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetGlobalUserConfigFilename","das_call_member< FString (UObject::*)() const , &UObject::GetGlobalUserConfigFilename >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_237 = das::das_call_member< FString (UObject::*)() const,&UObject::GetProjectUserConfigFilename >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1289:26
	makeExtern<DAS_CALL_METHOD(_method_237), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetProjectUserConfigFilename","das_call_member< FString (UObject::*)() const , &UObject::GetProjectUserConfigFilename >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1292:23
	makeExtern< const wchar_t * (*)(const UObject &) , _dasUnreal_virtual_71_GetConfigOverridePlatform , SimNode_ExtFuncCall >(lib,"GetConfigOverridePlatform","_dasUnreal_virtual_71_GetConfigOverridePlatform")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1299:15
	makeExtern< void (*)(UObject &,FString &) , _dasUnreal_virtual_72_OverrideConfigSection , SimNode_ExtFuncCall >(lib,"OverrideConfigSection","_dasUnreal_virtual_72_OverrideConfigSection")
		->args({"self","SectionName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1306:15
	makeExtern< void (*)(UObject &,FString &) , _dasUnreal_virtual_73_OverridePerObjectConfigSection , SimNode_ExtFuncCall >(lib,"OverridePerObjectConfigSection","_dasUnreal_virtual_73_OverridePerObjectConfigSection")
		->args({"self","SectionName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_241 = das::das_call_member< void (UObject::*)(const wchar_t *),&UObject::ParseParms >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1330:23
	makeExtern<DAS_CALL_METHOD(_method_241), SimNode_ExtFuncCall >(lib,"ParseParms","das_call_member< void (UObject::*)(const wchar_t *) , &UObject::ParseParms >::invoke")
		->args({"self","Parms"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1358:34
	makeExtern< UObject * (*)(const UObject &,const UClass *,const UObject *,FName,EObjectFlags) , _dasUnreal_static_74_GetArchetypeFromRequiredInfo , SimNode_ExtFuncCall >(lib,"GetArchetypeFromRequiredInfo","_dasUnreal_static_74_GetArchetypeFromRequiredInfo")
		->args({"self","Class","Outer","Name","ObjectFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_243 = das::das_call_member< UObject * (UObject::*)() const,&UObject::GetArchetype >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1365:27
	makeExtern<DAS_CALL_METHOD(_method_243), SimNode_ExtFuncCall >(lib,"GetArchetype","das_call_member< UObject * (UObject::*)() const , &UObject::GetArchetype >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1450:18
	makeExtern< UClass * (*)(UObject &,UClass *,UObject *) , _dasUnreal_virtual_75_RegenerateClass , SimNode_ExtFuncCall >(lib,"RegenerateClass","_dasUnreal_virtual_75_RegenerateClass")
		->args({"self","ClassToRegenerate","PreviousCDO"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_245 = das::das_call_member< bool (UObject::*)() const,&UObject::IsInBlueprint >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1455:23
	makeExtern<DAS_CALL_METHOD(_method_245), SimNode_ExtFuncCall >(lib,"IsInBlueprint","das_call_member< bool (UObject::*)() const , &UObject::IsInBlueprint >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_246 = das::das_call_member< void (UObject::*)(),&UObject::DestroyNonNativeProperties >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1460:23
	makeExtern<DAS_CALL_METHOD(_method_246), SimNode_ExtFuncCall >(lib,"DestroyNonNativeProperties","das_call_member< void (UObject::*)() , &UObject::DestroyNonNativeProperties >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1463:15
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_76_MarkAsEditorOnlySubobject , SimNode_ExtFuncCall >(lib,"MarkAsEditorOnlySubobject","_dasUnreal_virtual_76_MarkAsEditorOnlySubobject")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_248 = das::das_call_member< void (UObject::*)() const,&UObject::AbortInsideMemberFunction >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1468:23
	makeExtern<DAS_CALL_METHOD(_method_248), SimNode_ExtFuncCall >(lib,"AbortInsideMemberFunction","das_call_member< void (UObject::*)() const , &UObject::AbortInsideMemberFunction >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_249 = das::das_call_member< void (UObject::*)(int),&UObject::ExecuteUbergraph >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1664:7
	makeExtern<DAS_CALL_METHOD(_method_249), SimNode_ExtFuncCall >(lib,"ExecuteUbergraph","das_call_member< void (UObject::*)(int) , &UObject::ExecuteUbergraph >::invoke")
		->args({"self","EntryPoint"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1717:17
	makeExtern< uint64 (*)(const UObject &) , _dasUnreal_virtual_77_GetNetPushIdDynamic , SimNode_ExtFuncCall >(lib,"GetNetPushIdDynamic","_dasUnreal_virtual_77_GetNetPushIdDynamic")
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
	makeExtern< UClass * (*)(const UField &) , _dasUnreal_static_78_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_78_StaticClass")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:157:2
	makeExtern< const wchar_t * (*)(const UField &) , _dasUnreal_static_79_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_79_StaticPackage")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:157:2
	makeExtern< void (*)(const UField &) , _dasUnreal_static_80_StaticRegisterNativesUField , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUField","_dasUnreal_static_80_StaticRegisterNativesUField")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:170:31
	makeExtern< void (*)(UField &) , _dasUnreal_virtual_81_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_81_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:171:31
	makeExtern< bool (*)(const UField &) , _dasUnreal_virtual_82_NeedsLoadForClient , SimNode_ExtFuncCall >(lib,"NeedsLoadForClient","_dasUnreal_virtual_82_NeedsLoadForClient")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:172:31
	makeExtern< bool (*)(const UField &) , _dasUnreal_virtual_83_NeedsLoadForServer , SimNode_ExtFuncCall >(lib,"NeedsLoadForServer","_dasUnreal_virtual_83_NeedsLoadForServer")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:176:31
	makeExtern< void (*)(UField &) , _dasUnreal_virtual_84_Bind , SimNode_ExtFuncCall >(lib,"Bind","_dasUnreal_virtual_84_Bind")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_258 = das::das_call_member< UClass * (UField::*)() const,&UField::GetOwnerClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:179:26
	makeExtern<DAS_CALL_METHOD(_method_258), SimNode_ExtFuncCall >(lib,"GetOwnerClass","das_call_member< UClass * (UField::*)() const , &UField::GetOwnerClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_259 = das::das_call_member< UStruct * (UField::*)() const,&UField::GetOwnerStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:182:27
	makeExtern<DAS_CALL_METHOD(_method_259), SimNode_ExtFuncCall >(lib,"GetOwnerStruct","das_call_member< UStruct * (UField::*)() const , &UField::GetOwnerStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_260 = das::das_call_member< FString (UField::*)() const,&UField::GetAuthoredName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:189:26
	makeExtern<DAS_CALL_METHOD(_method_260), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredName","das_call_member< FString (UField::*)() const , &UField::GetAuthoredName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_261 = das::das_call_member< FText (UField::*)() const,&UField::GetDisplayNameText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:197:24
	makeExtern<DAS_CALL_METHOD(_method_261), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDisplayNameText","das_call_member< FText (UField::*)() const , &UField::GetDisplayNameText >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_262 = das::das_call_member< FText (UField::*)(bool) const,&UField::GetToolTipText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:206:24
	makeExtern<DAS_CALL_METHOD(_method_262), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetToolTipText","das_call_member< FText (UField::*)(bool) const , &UField::GetToolTipText >::invoke")
		->args({"self","bShortTooltip"})
		->arg_init(1,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:214:30
	makeExtern< void (*)(const UField &,FString &,bool) , _dasUnreal_static_85_FormatNativeToolTip , SimNode_ExtFuncCall >(lib,"FormatNativeToolTip","_dasUnreal_static_85_FormatNativeToolTip")
		->args({"self","ToolTipString","bRemoveExtraSections"})
		->arg_init(2,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_264 = das::das_call_member< bool (UField::*)(const wchar_t *) const,&UField::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:222:7
	makeExtern<DAS_CALL_METHOD(_method_264), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (UField::*)(const wchar_t *) const , &UField::HasMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_265 = das::das_call_member< bool (UField::*)(const FName &) const,&UField::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:223:7
	makeExtern<DAS_CALL_METHOD(_method_265), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (UField::*)(const FName &) const , &UField::HasMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_266 = das::das_call_member< const FString * (UField::*)(const wchar_t *) const,&UField::FindMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:231:33
	makeExtern<DAS_CALL_METHOD(_method_266), SimNode_ExtFuncCall >(lib,"FindMetaData","das_call_member< const FString * (UField::*)(const wchar_t *) const , &UField::FindMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_267 = das::das_call_member< const FString * (UField::*)(const FName &) const,&UField::FindMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:232:33
	makeExtern<DAS_CALL_METHOD(_method_267), SimNode_ExtFuncCall >(lib,"FindMetaData","das_call_member< const FString * (UField::*)(const FName &) const , &UField::FindMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_268 = das::das_call_member< const FString & (UField::*)(const wchar_t *) const,&UField::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:240:33
	makeExtern<DAS_CALL_METHOD(_method_268), SimNode_ExtFuncCallRef >(lib,"GetMetaData","das_call_member< const FString & (UField::*)(const wchar_t *) const , &UField::GetMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_269 = das::das_call_member< const FString & (UField::*)(const FName &) const,&UField::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:241:33
	makeExtern<DAS_CALL_METHOD(_method_269), SimNode_ExtFuncCallRef >(lib,"GetMetaData","das_call_member< const FString & (UField::*)(const FName &) const , &UField::GetMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_270 = das::das_call_member< FText (UField::*)(const wchar_t *,const FString,const FString) const,&UField::GetMetaDataText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:251:24
	makeExtern<DAS_CALL_METHOD(_method_270), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetMetaDataText","das_call_member< FText (UField::*)(const wchar_t *,const FString,const FString) const , &UField::GetMetaDataText >::invoke")
		->args({"self","MetaDataKey","LocalizationNamespace","LocalizationKey"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_271 = das::das_call_member< FText (UField::*)(const FName &,const FString,const FString) const,&UField::GetMetaDataText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:252:24
	makeExtern<DAS_CALL_METHOD(_method_271), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetMetaDataText","das_call_member< FText (UField::*)(const FName &,const FString,const FString) const , &UField::GetMetaDataText >::invoke")
		->args({"self","MetaDataKey","LocalizationNamespace","LocalizationKey"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_272 = das::das_call_member< void (UField::*)(const wchar_t *,const wchar_t *),&UField::SetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:260:23
	makeExtern<DAS_CALL_METHOD(_method_272), SimNode_ExtFuncCall >(lib,"SetMetaData","das_call_member< void (UField::*)(const wchar_t *,const wchar_t *) , &UField::SetMetaData >::invoke")
		->args({"self","Key","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_273 = das::das_call_member< void (UField::*)(const FName &,const wchar_t *),&UField::SetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:261:23
	makeExtern<DAS_CALL_METHOD(_method_273), SimNode_ExtFuncCall >(lib,"SetMetaData","das_call_member< void (UField::*)(const FName &,const wchar_t *) , &UField::SetMetaData >::invoke")
		->args({"self","Key","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_274 = das::das_call_member< bool (UField::*)(const wchar_t *) const,&UField::GetBoolMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:269:7
	makeExtern<DAS_CALL_METHOD(_method_274), SimNode_ExtFuncCall >(lib,"GetBoolMetaData","das_call_member< bool (UField::*)(const wchar_t *) const , &UField::GetBoolMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_275 = das::das_call_member< bool (UField::*)(const FName &) const,&UField::GetBoolMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:275:7
	makeExtern<DAS_CALL_METHOD(_method_275), SimNode_ExtFuncCall >(lib,"GetBoolMetaData","das_call_member< bool (UField::*)(const FName &) const , &UField::GetBoolMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_276 = das::das_call_member< int (UField::*)(const wchar_t *) const,&UField::GetIntMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:288:8
	makeExtern<DAS_CALL_METHOD(_method_276), SimNode_ExtFuncCall >(lib,"GetIntMetaData","das_call_member< int (UField::*)(const wchar_t *) const , &UField::GetIntMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_277 = das::das_call_member< int (UField::*)(const FName &) const,&UField::GetIntMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:294:8
	makeExtern<DAS_CALL_METHOD(_method_277), SimNode_ExtFuncCall >(lib,"GetIntMetaData","das_call_member< int (UField::*)(const FName &) const , &UField::GetIntMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_278 = das::das_call_member< float (UField::*)(const wchar_t *) const,&UField::GetFloatMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:307:8
	makeExtern<DAS_CALL_METHOD(_method_278), SimNode_ExtFuncCall >(lib,"GetFloatMetaData","das_call_member< float (UField::*)(const wchar_t *) const , &UField::GetFloatMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_279 = das::das_call_member< float (UField::*)(const FName &) const,&UField::GetFloatMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:313:8
	makeExtern<DAS_CALL_METHOD(_method_279), SimNode_ExtFuncCall >(lib,"GetFloatMetaData","das_call_member< float (UField::*)(const FName &) const , &UField::GetFloatMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_280 = das::das_call_member< UClass * (UField::*)(const wchar_t *) const,&UField::GetClassMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:326:26
	makeExtern<DAS_CALL_METHOD(_method_280), SimNode_ExtFuncCall >(lib,"GetClassMetaData","das_call_member< UClass * (UField::*)(const wchar_t *) const , &UField::GetClassMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_281 = das::das_call_member< UClass * (UField::*)(const FName &) const,&UField::GetClassMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:327:26
	makeExtern<DAS_CALL_METHOD(_method_281), SimNode_ExtFuncCall >(lib,"GetClassMetaData","das_call_member< UClass * (UField::*)(const FName &) const , &UField::GetClassMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_282 = das::das_call_member< void (UField::*)(const wchar_t *),&UField::RemoveMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:330:23
	makeExtern<DAS_CALL_METHOD(_method_282), SimNode_ExtFuncCall >(lib,"RemoveMetaData","das_call_member< void (UField::*)(const wchar_t *) , &UField::RemoveMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_283 = das::das_call_member< void (UField::*)(const FName &),&UField::RemoveMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:331:23
	makeExtern<DAS_CALL_METHOD(_method_283), SimNode_ExtFuncCall >(lib,"RemoveMetaData","das_call_member< void (UField::*)(const FName &) , &UField::RemoveMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_284 = das::das_call_member< bool (UField::*)(const uint64) const,&UField::HasAnyCastFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:334:23
	makeExtern<DAS_CALL_METHOD(_method_284), SimNode_ExtFuncCall >(lib,"HasAnyCastFlags","das_call_member< bool (UField::*)(const uint64) const , &UField::HasAnyCastFlags >::invoke")
		->args({"self","InCastFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_285 = das::das_call_member< bool (UField::*)(const uint64) const,&UField::HasAllCastFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:335:23
	makeExtern<DAS_CALL_METHOD(_method_285), SimNode_ExtFuncCall >(lib,"HasAllCastFlags","das_call_member< bool (UField::*)(const uint64) const , &UField::HasAllCastFlags >::invoke")
		->args({"self","InCastFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:388:2
	makeExtern< UClass * (*)(const UStruct &) , _dasUnreal_static_86_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_86_StaticClass")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:388:2
	makeExtern< const wchar_t * (*)(const UStruct &) , _dasUnreal_static_87_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_87_StaticPackage")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:388:2
	makeExtern< void (*)(const UStruct &) , _dasUnreal_static_88_StaticRegisterNativesUStruct , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUStruct","_dasUnreal_static_88_StaticRegisterNativesUStruct")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_289 = das::das_call_member< const FBlake3Hash & (UStruct::*)(bool) const,&UStruct::GetSchemaHash >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:441:37
	makeExtern<DAS_CALL_METHOD(_method_289), SimNode_ExtFuncCallRef >(lib,"GetSchemaHash","das_call_member< const FBlake3Hash & (UStruct::*)(bool) const , &UStruct::GetSchemaHash >::invoke")
		->args({"self","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:458:31
	makeExtern< void (*)(UStruct &) , _dasUnreal_virtual_89_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_89_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:459:31
	makeExtern< void (*)(UStruct &) , _dasUnreal_virtual_90_FinishDestroy , SimNode_ExtFuncCall >(lib,"FinishDestroy","_dasUnreal_virtual_90_FinishDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:460:31
	makeExtern< void (*)(UStruct &) , _dasUnreal_virtual_91_RegisterDependencies , SimNode_ExtFuncCall >(lib,"RegisterDependencies","_dasUnreal_virtual_91_RegisterDependencies")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:463:31
	makeExtern< void (*)(UStruct &,EObjectFlags) , _dasUnreal_virtual_92_TagSubobjects , SimNode_ExtFuncCall >(lib,"TagSubobjects","_dasUnreal_virtual_92_TagSubobjects")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:488:19
	makeExtern< UStruct * (*)(const UStruct &) , _dasUnreal_virtual_93_GetInheritanceSuper , SimNode_ExtFuncCall >(lib,"GetInheritanceSuper","_dasUnreal_virtual_93_GetInheritanceSuper")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_295 = das::das_call_member< void (UStruct::*)(bool),&UStruct::StaticLink >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:491:23
	makeExtern<DAS_CALL_METHOD(_method_295), SimNode_ExtFuncCall >(lib,"StaticLink","das_call_member< void (UStruct::*)(bool) , &UStruct::StaticLink >::invoke")
		->args({"self","bRelinkExistingProperties"})
		->arg_init(1,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:562:31
	makeExtern< void (*)(const UStruct &,void *,int) , _dasUnreal_virtual_94_InitializeStruct , SimNode_ExtFuncCall >(lib,"InitializeStruct","_dasUnreal_virtual_94_InitializeStruct")
		->args({"self","Dest","ArrayDim"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:571:31
	makeExtern< void (*)(const UStruct &,void *,int) , _dasUnreal_virtual_95_DestroyStruct , SimNode_ExtFuncCall >(lib,"DestroyStruct","_dasUnreal_virtual_95_DestroyStruct")
		->args({"self","Dest","ArrayDim"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:584:23
	makeExtern< const wchar_t * (*)(const UStruct &) , _dasUnreal_virtual_96_GetPrefixCPP , SimNode_ExtFuncCall >(lib,"GetPrefixCPP","_dasUnreal_virtual_96_GetPrefixCPP")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_299 = das::das_call_member< int (UStruct::*)() const,&UStruct::GetPropertiesSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:587:20
	makeExtern<DAS_CALL_METHOD(_method_299), SimNode_ExtFuncCall >(lib,"GetPropertiesSize","das_call_member< int (UStruct::*)() const , &UStruct::GetPropertiesSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_300 = das::das_call_member< int (UStruct::*)() const,&UStruct::GetMinAlignment >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:593:20
	makeExtern<DAS_CALL_METHOD(_method_300), SimNode_ExtFuncCall >(lib,"GetMinAlignment","das_call_member< int (UStruct::*)() const , &UStruct::GetMinAlignment >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_301 = das::das_call_member< int (UStruct::*)() const,&UStruct::GetStructureSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:599:20
	makeExtern<DAS_CALL_METHOD(_method_301), SimNode_ExtFuncCall >(lib,"GetStructureSize","das_call_member< int (UStruct::*)() const , &UStruct::GetStructureSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_302 = das::das_call_member< void (UStruct::*)(int),&UStruct::SetPropertiesSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:605:7
	makeExtern<DAS_CALL_METHOD(_method_302), SimNode_ExtFuncCall >(lib,"SetPropertiesSize","das_call_member< void (UStruct::*)(int) , &UStruct::SetPropertiesSize >::invoke")
		->args({"self","NewSize"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_303 = das::das_call_member< bool (UStruct::*)(const UStruct *) const,&UStruct::IsChildOf >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:619:23
	makeExtern<DAS_CALL_METHOD(_method_303), SimNode_ExtFuncCall >(lib,"IsChildOf","das_call_member< bool (UStruct::*)(const UStruct *) const , &UStruct::IsChildOf >::invoke")
		->args({"self","SomeBase"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_304 = das::das_call_member< UStruct * (UStruct::*)() const,&UStruct::GetSuperStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:628:11
	makeExtern<DAS_CALL_METHOD(_method_304), SimNode_ExtFuncCall >(lib,"GetSuperStruct","das_call_member< UStruct * (UStruct::*)() const , &UStruct::GetSuperStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:637:31
	makeExtern< void (*)(UStruct &,UStruct *) , _dasUnreal_virtual_97_SetSuperStruct , SimNode_ExtFuncCall >(lib,"SetSuperStruct","_dasUnreal_virtual_97_SetSuperStruct")
		->args({"self","NewSuperStruct"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:640:34
	makeExtern< FString (*)(const UStruct &,const UField *) , _dasUnreal_virtual_98_GetAuthoredNameForField , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredNameForField","_dasUnreal_virtual_98_GetAuthoredNameForField")
		->args({"self","Field"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:646:15
	makeExtern< bool (*)(const UStruct &) , _dasUnreal_virtual_99_IsStructTrashed , SimNode_ExtFuncCall >(lib,"IsStructTrashed","_dasUnreal_virtual_99_IsStructTrashed")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_308 = das::das_call_member< void (UStruct::*)(),&UStruct::DestroyChildPropertiesAndResetPropertyLinks >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:652:23
	makeExtern<DAS_CALL_METHOD(_method_308), SimNode_ExtFuncCall >(lib,"DestroyChildPropertiesAndResetPropertyLinks","das_call_member< void (UStruct::*)() , &UStruct::DestroyChildPropertiesAndResetPropertyLinks >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_309 = das::das_call_member< bool (UStruct::*)(const FName &) const,&UStruct::GetBoolMetaDataHierarchical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:656:23
	makeExtern<DAS_CALL_METHOD(_method_309), SimNode_ExtFuncCall >(lib,"GetBoolMetaDataHierarchical","das_call_member< bool (UStruct::*)(const FName &) const , &UStruct::GetBoolMetaDataHierarchical >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_310 = das::das_call_member< bool (UStruct::*)(const FName &,FString *) const,&UStruct::GetStringMetaDataHierarchical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:659:23
	makeExtern<DAS_CALL_METHOD(_method_310), SimNode_ExtFuncCall >(lib,"GetStringMetaDataHierarchical","das_call_member< bool (UStruct::*)(const FName &,FString *) const , &UStruct::GetStringMetaDataHierarchical >::invoke")
		->args({"self","Key","OutValue"})
		->arg_init(2,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_311 = das::das_call_member< const UStruct * (UStruct::*)(const FName &) const,&UStruct::HasMetaDataHierarchical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:667:33
	makeExtern<DAS_CALL_METHOD(_method_311), SimNode_ExtFuncCall >(lib,"HasMetaDataHierarchical","das_call_member< const UStruct * (UStruct::*)(const FName &) const , &UStruct::HasMetaDataHierarchical >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_312 = das::das_call_member< bool (UStruct::*)() const,&UStruct::HasAssetRegistrySearchableProperties >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:670:19
	makeExtern<DAS_CALL_METHOD(_method_312), SimNode_ExtFuncCall >(lib,"HasAssetRegistrySearchableProperties","das_call_member< bool (UStruct::*)() const , &UStruct::HasAssetRegistrySearchableProperties >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_313 = das::das_call_member< void (UStruct::*)(),&UStruct::DeleteUnresolvedScriptProperties >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:687:19
	makeExtern<DAS_CALL_METHOD(_method_313), SimNode_ExtFuncCall >(lib,"DeleteUnresolvedScriptProperties","das_call_member< void (UStruct::*)() , &UStruct::DeleteUnresolvedScriptProperties >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_314 = das::das_call_member< void (UStruct::*)(),&UStruct::CollectBytecodeAndPropertyReferencedObjects >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:709:23
	makeExtern<DAS_CALL_METHOD(_method_314), SimNode_ExtFuncCall >(lib,"CollectBytecodeAndPropertyReferencedObjects","das_call_member< void (UStruct::*)() , &UStruct::CollectBytecodeAndPropertyReferencedObjects >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_315 = das::das_call_member< void (UStruct::*)(),&UStruct::CollectBytecodeAndPropertyReferencedObjectsRecursively >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:713:23
	makeExtern<DAS_CALL_METHOD(_method_315), SimNode_ExtFuncCall >(lib,"CollectBytecodeAndPropertyReferencedObjectsRecursively","das_call_member< void (UStruct::*)() , &UStruct::CollectBytecodeAndPropertyReferencedObjectsRecursively >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2001:2
	makeExtern< UClass * (*)(const UEnum &) , _dasUnreal_static_100_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_100_StaticClass")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2001:2
	makeExtern< const wchar_t * (*)(const UEnum &) , _dasUnreal_static_101_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_101_StaticPackage")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2001:2
	makeExtern< void (*)(const UEnum &) , _dasUnreal_static_102_StaticRegisterNativesUEnum , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUEnum","_dasUnreal_static_102_StaticRegisterNativesUEnum")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_319 = das::das_call_member< int (UEnum::*)(int64) const,&UEnum::GetIndexByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2022:20
	makeExtern<DAS_CALL_METHOD(_method_319), SimNode_ExtFuncCall >(lib,"GetIndexByValue","das_call_member< int (UEnum::*)(int64) const , &UEnum::GetIndexByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_320 = das::das_call_member< int64 (UEnum::*)(int) const,&UEnum::GetValueByIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2035:20
	makeExtern<DAS_CALL_METHOD(_method_320), SimNode_ExtFuncCall >(lib,"GetValueByIndex","das_call_member< int64 (UEnum::*)(int) const , &UEnum::GetValueByIndex >::invoke")
		->args({"self","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_321 = das::das_call_member< FName (UEnum::*)(int) const,&UEnum::GetNameByIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2042:24
	makeExtern<DAS_CALL_METHOD(_method_321), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameByIndex","das_call_member< FName (UEnum::*)(int) const , &UEnum::GetNameByIndex >::invoke")
		->args({"self","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_322 = das::das_call_member< int (UEnum::*)(FName,EGetByNameFlags) const,&UEnum::GetIndexByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2045:24
	makeExtern<DAS_CALL_METHOD(_method_322), SimNode_ExtFuncCall >(lib,"GetIndexByName","das_call_member< int (UEnum::*)(FName,EGetByNameFlags) const , &UEnum::GetIndexByName >::invoke")
		->args({"self","InName","Flags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EGetByNameFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_323 = das::das_call_member< FName (UEnum::*)(int64) const,&UEnum::GetNameByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2048:24
	makeExtern<DAS_CALL_METHOD(_method_323), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameByValue","das_call_member< FName (UEnum::*)(int64) const , &UEnum::GetNameByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_324 = das::das_call_member< int64 (UEnum::*)(FName,EGetByNameFlags) const,&UEnum::GetValueByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2051:24
	makeExtern<DAS_CALL_METHOD(_method_324), SimNode_ExtFuncCall >(lib,"GetValueByName","das_call_member< int64 (UEnum::*)(FName,EGetByNameFlags) const , &UEnum::GetValueByName >::invoke")
		->args({"self","InName","Flags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EGetByNameFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_325 = das::das_call_member< FString (UEnum::*)(int) const,&UEnum::GetNameStringByIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2054:26
	makeExtern<DAS_CALL_METHOD(_method_325), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameStringByIndex","das_call_member< FString (UEnum::*)(int) const , &UEnum::GetNameStringByIndex >::invoke")
		->args({"self","InIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_326 = das::das_call_member< int (UEnum::*)(const FString &,EGetByNameFlags) const,&UEnum::GetIndexByNameString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2057:24
	makeExtern<DAS_CALL_METHOD(_method_326), SimNode_ExtFuncCall >(lib,"GetIndexByNameString","das_call_member< int (UEnum::*)(const FString &,EGetByNameFlags) const , &UEnum::GetIndexByNameString >::invoke")
		->args({"self","SearchString","Flags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EGetByNameFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_327 = das::das_call_member< FString (UEnum::*)(int64) const,&UEnum::GetNameStringByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2060:26
	makeExtern<DAS_CALL_METHOD(_method_327), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameStringByValue","das_call_member< FString (UEnum::*)(int64) const , &UEnum::GetNameStringByValue >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_328 = das::das_call_member< FString (UEnum::*)(int64) const,&UEnum::GetValueOrBitfieldAsString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2063:26
	makeExtern<DAS_CALL_METHOD(_method_328), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetValueOrBitfieldAsString","das_call_member< FString (UEnum::*)(int64) const , &UEnum::GetValueOrBitfieldAsString >::invoke")
		->args({"self","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_329 = das::das_call_member< bool (UEnum::*)(FString &,int64) const,&UEnum::FindNameStringByValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2066:23
	makeExtern<DAS_CALL_METHOD(_method_329), SimNode_ExtFuncCall >(lib,"FindNameStringByValue","das_call_member< bool (UEnum::*)(FString &,int64) const , &UEnum::FindNameStringByValue >::invoke")
		->args({"self","Out","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_330 = das::das_call_member< int64 (UEnum::*)(const FString &,EGetByNameFlags) const,&UEnum::GetValueByNameString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2069:24
	makeExtern<DAS_CALL_METHOD(_method_330), SimNode_ExtFuncCall >(lib,"GetValueByNameString","das_call_member< int64 (UEnum::*)(const FString &,EGetByNameFlags) const , &UEnum::GetValueByNameString >::invoke")
		->args({"self","SearchString","Flags"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EGetByNameFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
}
}

