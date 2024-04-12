// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_4() {
	addCtorAndUsing<FPrimaryAssetType>(*this,lib,"FPrimaryAssetType","FPrimaryAssetType");
	addCtorAndUsing<FPrimaryAssetType,FName>(*this,lib,"FPrimaryAssetType","FPrimaryAssetType")
		->args({"InName"});
	addCtorAndUsing<FPrimaryAssetType,EName>(*this,lib,"FPrimaryAssetType","FPrimaryAssetType")
		->args({"InName"});
	addCtorAndUsing<FPrimaryAssetType,const char *>(*this,lib,"FPrimaryAssetType","FPrimaryAssetType")
		->args({"InName"});
	addCtorAndUsing<FPrimaryAssetType,const FPrimaryAssetType &>(*this,lib,"FPrimaryAssetType","FPrimaryAssetType")
		->args({""});
	using _method_242 = das::das_call_member< FName (FPrimaryAssetType::*)() const,&FPrimaryAssetType::GetName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:41:8
	makeExtern<DAS_CALL_METHOD(_method_242), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetName","das_call_member< FName (FPrimaryAssetType::*)() const , &FPrimaryAssetType::GetName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_243 = das::das_call_member< bool (FPrimaryAssetType::*)() const,&FPrimaryAssetType::IsValid >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:57:7
	makeExtern<DAS_CALL_METHOD(_method_243), SimNode_ExtFuncCall >(lib,"IsValid","das_call_member< bool (FPrimaryAssetType::*)() const , &FPrimaryAssetType::IsValid >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_244 = das::das_call_member< FString (FPrimaryAssetType::*)() const,&FPrimaryAssetType::ToString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:63:10
	makeExtern<DAS_CALL_METHOD(_method_244), SimNode_ExtFuncCallAndCopyOrMove >(lib,"ToString","das_call_member< FString (FPrimaryAssetType::*)() const , &FPrimaryAssetType::ToString >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_245 = das::das_call_member< bool (FPrimaryAssetType::*)(const FPrimaryAssetType &) const,&FPrimaryAssetType::LexicalLess >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:84:7
	makeExtern<DAS_CALL_METHOD(_method_245), SimNode_ExtFuncCall >(lib,"LexicalLess","das_call_member< bool (FPrimaryAssetType::*)(const FPrimaryAssetType &) const , &FPrimaryAssetType::LexicalLess >::invoke")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_246 = das::das_call_member< bool (FPrimaryAssetType::*)(const FPrimaryAssetType &) const,&FPrimaryAssetType::FastLess >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:89:7
	makeExtern<DAS_CALL_METHOD(_method_246), SimNode_ExtFuncCall >(lib,"FastLess","das_call_member< bool (FPrimaryAssetType::*)(const FPrimaryAssetType &) const , &FPrimaryAssetType::FastLess >::invoke")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FPrimaryAssetId>(*this,lib,"FPrimaryAssetId","FPrimaryAssetId");
	addCtorAndUsing<FPrimaryAssetId,FPrimaryAssetType,FName>(*this,lib,"FPrimaryAssetId","FPrimaryAssetId")
		->args({"InAssetType","InAssetName"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:122:41
	makeExtern< FPrimaryAssetId (*)(const FPrimaryAssetId *,const wchar_t *,unsigned int) , _dasUnreal_static_80_ParseTypeAndName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"ParseTypeAndName","_dasUnreal_static_80_ParseTypeAndName")
		->args({"self","TypeAndName","Len"})
		->arg_type(0,makeType<TExplicit<const FPrimaryAssetId*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:123:41
	makeExtern< FPrimaryAssetId (*)(const FPrimaryAssetId *,FName) , _dasUnreal_static_81_ParseTypeAndName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"ParseTypeAndName","_dasUnreal_static_81_ParseTypeAndName")
		->args({"self","TypeAndName"})
		->arg_type(0,makeType<TExplicit<const FPrimaryAssetId*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:124:25
	makeExtern< FPrimaryAssetId (*)(const FPrimaryAssetId *,const FString &) , _dasUnreal_static_82_ParseTypeAndName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"ParseTypeAndName","_dasUnreal_static_82_ParseTypeAndName")
		->args({"self","TypeAndName"})
		->arg_type(0,makeType<TExplicit<const FPrimaryAssetId*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FPrimaryAssetId,const FString &>(*this,lib,"FPrimaryAssetId","FPrimaryAssetId")
		->args({"TypeAndName"});
	addCtorAndUsing<FPrimaryAssetId,const FPrimaryAssetId &>(*this,lib,"FPrimaryAssetId","FPrimaryAssetId")
		->args({""});
	using _method_250 = das::das_call_member< bool (FPrimaryAssetId::*)() const,&FPrimaryAssetId::IsValid >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:139:7
	makeExtern<DAS_CALL_METHOD(_method_250), SimNode_ExtFuncCall >(lib,"IsValid","das_call_member< bool (FPrimaryAssetId::*)() const , &FPrimaryAssetId::IsValid >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_251 = das::das_call_member< FString (FPrimaryAssetId::*)() const,&FPrimaryAssetId::ToString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:145:10
	makeExtern<DAS_CALL_METHOD(_method_251), SimNode_ExtFuncCallAndCopyOrMove >(lib,"ToString","das_call_member< FString (FPrimaryAssetId::*)() const , &FPrimaryAssetId::ToString >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:164:25
	makeExtern< FPrimaryAssetId (*)(const FPrimaryAssetId *,const FString &) , _dasUnreal_static_83_FromString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FromString","_dasUnreal_static_83_FromString")
		->args({"self","String"})
		->arg_type(0,makeType<TExplicit<const FPrimaryAssetId*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:24:1
	makeExtern< const char * (*)(const FStatGroup_STATGROUP_Script *) , _dasUnreal_static_84_GetGroupName , SimNode_ExtFuncCall >(lib,"GetGroupName","_dasUnreal_static_84_GetGroupName")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStatGroup_STATGROUP_Script*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:24:1
	makeExtern< const char * (*)(const FStatGroup_STATGROUP_Script *) , _dasUnreal_static_85_GetGroupCategory , SimNode_ExtFuncCall >(lib,"GetGroupCategory","_dasUnreal_static_85_GetGroupCategory")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStatGroup_STATGROUP_Script*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:24:1
	makeExtern< const wchar_t * (*)(const FStatGroup_STATGROUP_Script *) , _dasUnreal_static_86_GetDescription , SimNode_ExtFuncCall >(lib,"GetDescription","_dasUnreal_static_86_GetDescription")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStatGroup_STATGROUP_Script*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:24:1
	makeExtern< bool (*)(const FStatGroup_STATGROUP_Script *) , _dasUnreal_static_87_IsDefaultEnabled , SimNode_ExtFuncCall >(lib,"IsDefaultEnabled","_dasUnreal_static_87_IsDefaultEnabled")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStatGroup_STATGROUP_Script*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:24:1
	makeExtern< bool (*)(const FStatGroup_STATGROUP_Script *) , _dasUnreal_static_88_IsCompileTimeEnable , SimNode_ExtFuncCall >(lib,"IsCompileTimeEnable","_dasUnreal_static_88_IsCompileTimeEnable")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStatGroup_STATGROUP_Script*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:24:1
	makeExtern< bool (*)(const FStatGroup_STATGROUP_Script *) , _dasUnreal_static_89_GetSortByName , SimNode_ExtFuncCall >(lib,"GetSortByName","_dasUnreal_static_89_GetSortByName")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FStatGroup_STATGROUP_Script*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:103:22
	makeExtern< const wchar_t * (*)(FunctionCallspace::Type) , FunctionCallspace::ToString , SimNode_ExtFuncCall >(lib,"ToString","FunctionCallspace::ToString")
		->args({"Callspace"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FBlueprintExceptionInfo,EBlueprintExceptionType::Type>(*this,lib,"FBlueprintExceptionInfo","FBlueprintExceptionInfo")
		->args({"InEventType"});
	addCtorAndUsing<FBlueprintExceptionInfo,EBlueprintExceptionType::Type,const FText &>(*this,lib,"FBlueprintExceptionInfo","FBlueprintExceptionInfo")
		->args({"InEventType","InDescription"});
	using _method_259 = das::das_call_member< EBlueprintExceptionType::Type (FBlueprintExceptionInfo::*)() const,&FBlueprintExceptionInfo::GetType >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:394:32
	makeExtern<DAS_CALL_METHOD(_method_259), SimNode_ExtFuncCall >(lib,"GetType","das_call_member< EBlueprintExceptionType::Type (FBlueprintExceptionInfo::*)() const , &FBlueprintExceptionInfo::GetType >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_260 = das::das_call_member< const FText & (FBlueprintExceptionInfo::*)() const,&FBlueprintExceptionInfo::GetDescription >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:399:15
	makeExtern<DAS_CALL_METHOD(_method_260), SimNode_ExtFuncCallRef >(lib,"GetDescription","das_call_member< const FText & (FBlueprintExceptionInfo::*)() const , &FBlueprintExceptionInfo::GetDescription >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FScriptInstrumentationSignal,EScriptInstrumentation::Type,const UObject *,const FFrame &,const FName>(*this,lib,"FScriptInstrumentationSignal","FScriptInstrumentationSignal")
		->args({"InEventType","InContextObject","InStackFrame","EventNameIn"});
	addCtorAndUsing<FScriptInstrumentationSignal,EScriptInstrumentation::Type,const UObject *,UFunction *,const int>(*this,lib,"FScriptInstrumentationSignal","FScriptInstrumentationSignal")
		->args({"InEventType","InContextObject","InFunction","LinkId"})
		->arg_init(3,make_smart<ExprConstInt>(-1));
	using _method_261 = das::das_call_member< EScriptInstrumentation::Type (FScriptInstrumentationSignal::*)() const,&FScriptInstrumentationSignal::GetType >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:426:31
	makeExtern<DAS_CALL_METHOD(_method_261), SimNode_ExtFuncCall >(lib,"GetType","das_call_member< EScriptInstrumentation::Type (FScriptInstrumentationSignal::*)() const , &FScriptInstrumentationSignal::GetType >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_262 = das::das_call_member< void (FScriptInstrumentationSignal::*)(EScriptInstrumentation::Type),&FScriptInstrumentationSignal::SetType >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:429:7
	makeExtern<DAS_CALL_METHOD(_method_262), SimNode_ExtFuncCall >(lib,"SetType","das_call_member< void (FScriptInstrumentationSignal::*)(EScriptInstrumentation::Type) , &FScriptInstrumentationSignal::SetType >::invoke")
		->args({"self","InType"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_263 = das::das_call_member< bool (FScriptInstrumentationSignal::*)() const,&FScriptInstrumentationSignal::IsContextObjectValid >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:432:7
	makeExtern<DAS_CALL_METHOD(_method_263), SimNode_ExtFuncCall >(lib,"IsContextObjectValid","das_call_member< bool (FScriptInstrumentationSignal::*)() const , &FScriptInstrumentationSignal::IsContextObjectValid >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_264 = das::das_call_member< const UObject * (FScriptInstrumentationSignal::*)() const,&FScriptInstrumentationSignal::GetContextObject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:435:17
	makeExtern<DAS_CALL_METHOD(_method_264), SimNode_ExtFuncCall >(lib,"GetContextObject","das_call_member< const UObject * (FScriptInstrumentationSignal::*)() const , &FScriptInstrumentationSignal::GetContextObject >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_265 = das::das_call_member< bool (FScriptInstrumentationSignal::*)() const,&FScriptInstrumentationSignal::IsStackFrameValid >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:438:7
	makeExtern<DAS_CALL_METHOD(_method_265), SimNode_ExtFuncCall >(lib,"IsStackFrameValid","das_call_member< bool (FScriptInstrumentationSignal::*)() const , &FScriptInstrumentationSignal::IsStackFrameValid >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_266 = das::das_call_member< const FFrame & (FScriptInstrumentationSignal::*)() const,&FScriptInstrumentationSignal::GetStackFrame >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:441:16
	makeExtern<DAS_CALL_METHOD(_method_266), SimNode_ExtFuncCallRef >(lib,"GetStackFrame","das_call_member< const FFrame & (FScriptInstrumentationSignal::*)() const , &FScriptInstrumentationSignal::GetStackFrame >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_267 = das::das_call_member< const UClass * (FScriptInstrumentationSignal::*)() const,&FScriptInstrumentationSignal::GetClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:444:32
	makeExtern<DAS_CALL_METHOD(_method_267), SimNode_ExtFuncCall >(lib,"GetClass","das_call_member< const UClass * (FScriptInstrumentationSignal::*)() const , &FScriptInstrumentationSignal::GetClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_268 = das::das_call_member< const UClass * (FScriptInstrumentationSignal::*)() const,&FScriptInstrumentationSignal::GetFunctionClassScope >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:447:32
	makeExtern<DAS_CALL_METHOD(_method_268), SimNode_ExtFuncCall >(lib,"GetFunctionClassScope","das_call_member< const UClass * (FScriptInstrumentationSignal::*)() const , &FScriptInstrumentationSignal::GetFunctionClassScope >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_269 = das::das_call_member< FName (FScriptInstrumentationSignal::*)() const,&FScriptInstrumentationSignal::GetFunctionName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:450:24
	makeExtern<DAS_CALL_METHOD(_method_269), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFunctionName","das_call_member< FName (FScriptInstrumentationSignal::*)() const , &FScriptInstrumentationSignal::GetFunctionName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_270 = das::das_call_member< int (FScriptInstrumentationSignal::*)() const,&FScriptInstrumentationSignal::GetScriptCodeOffset >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:453:24
	makeExtern<DAS_CALL_METHOD(_method_270), SimNode_ExtFuncCall >(lib,"GetScriptCodeOffset","das_call_member< int (FScriptInstrumentationSignal::*)() const , &FScriptInstrumentationSignal::GetScriptCodeOffset >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_271 = das::das_call_member< int (FScriptInstrumentationSignal::*)() const,&FScriptInstrumentationSignal::GetLatentLinkId >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:456:8
	makeExtern<DAS_CALL_METHOD(_method_271), SimNode_ExtFuncCall >(lib,"GetLatentLinkId","das_call_member< int (FScriptInstrumentationSignal::*)() const , &FScriptInstrumentationSignal::GetLatentLinkId >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:493:30
	makeExtern< void (*)(const FBlueprintCoreDelegates *,const UObject *,FFrame &,const FBlueprintExceptionInfo &) , _dasUnreal_static_90_ThrowScriptException , SimNode_ExtFuncCall >(lib,"ThrowScriptException","_dasUnreal_static_90_ThrowScriptException")
		->args({"self","ActiveObject","StackFrame","Info"})
		->arg_type(0,makeType<TExplicit<const FBlueprintCoreDelegates*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:494:30
	makeExtern< void (*)(const FBlueprintCoreDelegates *,const FScriptInstrumentationSignal &) , _dasUnreal_static_91_InstrumentScriptEvent , SimNode_ExtFuncCall >(lib,"InstrumentScriptEvent","_dasUnreal_static_91_InstrumentScriptEvent")
		->args({"self","Info"})
		->arg_type(0,makeType<TExplicit<const FBlueprintCoreDelegates*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:495:30
	makeExtern< void (*)(const FBlueprintCoreDelegates *,const int) , _dasUnreal_static_92_SetScriptMaximumLoopIterations , SimNode_ExtFuncCall >(lib,"SetScriptMaximumLoopIterations","_dasUnreal_static_92_SetScriptMaximumLoopIterations")
		->args({"self","MaximumLoopIterations"})
		->arg_type(0,makeType<TExplicit<const FBlueprintCoreDelegates*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:496:30
	makeExtern< bool (*)(const FBlueprintCoreDelegates *) , _dasUnreal_static_93_IsDebuggingEnabled , SimNode_ExtFuncCall >(lib,"IsDebuggingEnabled","_dasUnreal_static_93_IsDebuggingEnabled")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBlueprintCoreDelegates*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FBlueprintContextTracker>(*this,lib,"FBlueprintContextTracker","FBlueprintContextTracker");
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:514:35
	makeExtern< FBlueprintContextTracker & (*)(const FBlueprintContextTracker *) , _dasUnreal_static_94_Get , SimNode_ExtFuncCallRef >(lib,"Get","_dasUnreal_static_94_Get")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBlueprintContextTracker*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:517:41
	makeExtern< const FBlueprintContextTracker * (*)(const FBlueprintContextTracker *) , _dasUnreal_static_95_TryGet , SimNode_ExtFuncCall >(lib,"TryGet","_dasUnreal_static_95_TryGet")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FBlueprintContextTracker*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_278 = das::das_call_member< void (FBlueprintContextTracker::*)(),&FBlueprintContextTracker::ResetRunaway >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:520:7
	makeExtern<DAS_CALL_METHOD(_method_278), SimNode_ExtFuncCall >(lib,"ResetRunaway","das_call_member< void (FBlueprintContextTracker::*)() , &FBlueprintContextTracker::ResetRunaway >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_279 = das::das_call_member< void (FBlueprintContextTracker::*)(),&FBlueprintContextTracker::AddRunaway >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:523:19
	makeExtern<DAS_CALL_METHOD(_method_279), SimNode_ExtFuncCall >(lib,"AddRunaway","das_call_member< void (FBlueprintContextTracker::*)() , &FBlueprintContextTracker::AddRunaway >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_280 = das::das_call_member< void (FBlueprintContextTracker::*)(const UObject *,const UFunction *),&FBlueprintContextTracker::EnterScriptContext >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:529:7
	makeExtern<DAS_CALL_METHOD(_method_280), SimNode_ExtFuncCall >(lib,"EnterScriptContext","das_call_member< void (FBlueprintContextTracker::*)(const UObject *,const UFunction *) , &FBlueprintContextTracker::EnterScriptContext >::invoke")
		->args({"self","ContextObject","ContextFunction"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_281 = das::das_call_member< void (FBlueprintContextTracker::*)(),&FBlueprintContextTracker::ExitScriptContext >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:532:7
	makeExtern<DAS_CALL_METHOD(_method_281), SimNode_ExtFuncCall >(lib,"ExitScriptContext","das_call_member< void (FBlueprintContextTracker::*)() , &FBlueprintContextTracker::ExitScriptContext >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_282 = das::das_call_member< bool (FBlueprintContextTracker::*)(const UObject *),&FBlueprintContextTracker::RecordAccessViolation >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:535:7
	makeExtern<DAS_CALL_METHOD(_method_282), SimNode_ExtFuncCall >(lib,"RecordAccessViolation","das_call_member< bool (FBlueprintContextTracker::*)(const UObject *) , &FBlueprintContextTracker::RecordAccessViolation >::invoke")
		->args({"self","Object"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_283 = das::das_call_member< int (FBlueprintContextTracker::*)() const,&FBlueprintContextTracker::GetScriptEntryTag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:538:20
	makeExtern<DAS_CALL_METHOD(_method_283), SimNode_ExtFuncCall >(lib,"GetScriptEntryTag","das_call_member< int (FBlueprintContextTracker::*)() const , &FBlueprintContextTracker::GetScriptEntryTag >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FEditorScriptExecutionGuard>(*this,lib,"FEditorScriptExecutionGuard","FEditorScriptExecutionGuard");
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:672:22
	makeExtern< bool (*)(wchar_t) , IsValidCPPIdentifierChar , SimNode_ExtFuncCall >(lib,"IsValidCPPIdentifierChar","IsValidCPPIdentifierChar")
		->args({"Char"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:675:25
	makeExtern< FString (*)(wchar_t) , ToValidCPPIdentifierChars , SimNode_ExtFuncCallAndCopyOrMove >(lib,"ToValidCPPIdentifierChars","ToValidCPPIdentifierChars")
		->args({"Char"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Script.h:683:25
	makeExtern< FString (*)(const FString &,bool,const wchar_t *) , UnicodeToCPPIdentifier , SimNode_ExtFuncCallAndCopyOrMove >(lib,"UnicodeToCPPIdentifier","UnicodeToCPPIdentifier")
		->args({"InName","bDeprecated","Prefix"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FVTableHelper>(*this,lib,"FVTableHelper","FVTableHelper");
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:516:30
	makeExtern< const wchar_t * (*)(EPropertyObjectReferenceType) , LexToString , SimNode_ExtFuncCall >(lib,"LexToString","LexToString")
		->args({"Type"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:231:23
	makeExtern< bool (*)(UObject *) , UE::IsSavingPackage , SimNode_ExtFuncCall >(lib,"IsSavingPackage","UE::IsSavingPackage")
		->args({"InOuter"})
		->arg_init(0,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:240:27
	makeExtern< UPackage * (*)() , GetTransientPackage , SimNode_ExtFuncCall >(lib,"GetTransientPackage","GetTransientPackage")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:243:26
	makeExtern< UObject * (*)(UClass *) , GetTransientOuterForRename , SimNode_ExtFuncCall >(lib,"GetTransientOuterForRename","GetTransientOuterForRename")
		->args({"ForClass"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:250:32
	makeExtern< const FString * (*)(const FString &) , GetIniFilenameFromObjectsReference , SimNode_ExtFuncCall >(lib,"GetIniFilenameFromObjectsReference","GetIniFilenameFromObjectsReference")
		->args({"ObjectsReferenceString"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:261:25
	makeExtern< FString (*)(const FString &,const FString *,bool) , ResolveIniObjectsReference , SimNode_ExtFuncCallAndCopyOrMove >(lib,"ResolveIniObjectsReference","ResolveIniObjectsReference")
		->args({"ObjectReference","IniFilename","bThrow"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->arg_init(2,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:275:22
	makeExtern< bool (*)(UObject *&,FString &,bool,bool,unsigned int,const FLinkerInstancingContext *) , ResolveName , SimNode_ExtFuncCall >(lib,"ResolveName","ResolveName")
		->args({"Outer","ObjectsReferenceString","Create","Throw","LoadFlags","InstancingContext"})
		->arg_init(4,make_smart<ExprConstUInt>(0x0))
		->arg_init(5,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:278:22
	makeExtern< bool (*)(UObject *,unsigned int,const wchar_t *) , SafeLoadError , SimNode_ExtFuncCall >(lib,"SafeLoadError","SafeLoadError")
		->args({"Outer","LoadFlags","ErrorMessage"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:313:26
	makeExtern< UObject * (*)(UClass *,UObject *,FName,bool,EObjectFlags,EInternalObjectFlags) , StaticFindObjectFast , SimNode_ExtFuncCall >(lib,"StaticFindObjectFast","StaticFindObjectFast")
		->args({"Class","InOuter","InName","bExactClass","ExclusiveFlags","ExclusiveInternalFlags"})
		->arg_init(3,make_smart<ExprConstBool>(false))
		->arg_init(4,make_smart<ExprConstEnumeration>(0,makeType<EObjectFlags>(lib)))
		->arg_init(5,make_smart<ExprConstEnumeration>(0,makeType<EInternalObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:348:26
	makeExtern< UObject * (*)(UClass *,UObject *,FName,bool,EObjectFlags,EInternalObjectFlags) , StaticFindObjectFastSafe , SimNode_ExtFuncCall >(lib,"StaticFindObjectFastSafe","StaticFindObjectFastSafe")
		->args({"Class","InOuter","InName","bExactClass","ExclusiveFlags","ExclusiveInternalFlags"})
		->arg_init(3,make_smart<ExprConstBool>(false))
		->arg_init(4,make_smart<ExprConstEnumeration>(0,makeType<EObjectFlags>(lib)))
		->arg_init(5,make_smart<ExprConstEnumeration>(0,makeType<EInternalObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:360:26
	makeExtern< UObject * (*)(UClass *,UObject *,const wchar_t *,bool) , StaticFindObject , SimNode_ExtFuncCall >(lib,"StaticFindObject","StaticFindObject")
		->args({"Class","InOuter","Name","ExactClass"})
		->arg_init(3,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:374:26
	makeExtern< UObject * (*)(UClass *,UObject *,const wchar_t *,bool) , StaticFindObjectChecked , SimNode_ExtFuncCall >(lib,"StaticFindObjectChecked","StaticFindObjectChecked")
		->args({"Class","InOuter","Name","ExactClass"})
		->arg_init(3,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:377:26
	makeExtern< UObject * (*)(UClass *,UObject *,const wchar_t *,bool) , StaticFindObjectSafe , SimNode_ExtFuncCall >(lib,"StaticFindObjectSafe","StaticFindObjectSafe")
		->args({"Class","InOuter","Name","ExactClass"})
		->arg_init(3,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:483:26
	makeExtern< UObject * (*)(UClass *,const wchar_t *,EFindFirstObjectOptions,ELogVerbosity::Type,const wchar_t *) , StaticFindFirstObject , SimNode_ExtFuncCall >(lib,"StaticFindFirstObject","StaticFindFirstObject")
		->args({"Class","Name","Options","AmbiguousMessageVerbosity","InCurrentOperation"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EFindFirstObjectOptions>(lib)))
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<ELogVerbosity::Type>(lib)))
		->arg_init(4,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:499:26
	makeExtern< UObject * (*)(UClass *,const wchar_t *,EFindFirstObjectOptions,ELogVerbosity::Type,const wchar_t *) , StaticFindFirstObjectSafe , SimNode_ExtFuncCall >(lib,"StaticFindFirstObjectSafe","StaticFindFirstObjectSafe")
		->args({"Class","Name","Options","AmbiguousMessageVerbosity","InCurrentOperation"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EFindFirstObjectOptions>(lib)))
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<ELogVerbosity::Type>(lib)))
		->arg_init(4,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:523:22
	makeExtern< bool (*)(const wchar_t *,const wchar_t *,UClass *,UObject *&,UObject *,EParseObjectLoadingPolicy,bool *) , ParseObject , SimNode_ExtFuncCall >(lib,"ParseObject","ParseObject")
		->args({"Stream","Match","Class","DestRes","InParent","LoadingPolicy","bInvalidObject"})
		->arg_init(6,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:524:13
	makeExtern< bool (*)(const wchar_t *,const wchar_t *,UClass *,UObject *&,UObject *,bool *) , ParseObject , SimNode_ExtFuncCall >(lib,"ParseObject","ParseObject")
		->args({"Stream","Match","Class","DestRes","InParent","bInvalidObject"})
		->arg_init(5,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:544:26
	makeExtern< UObject * (*)(UClass *,UObject *,const wchar_t *,const wchar_t *,unsigned int,UPackageMap *,bool,const FLinkerInstancingContext *) , StaticLoadObject , SimNode_ExtFuncCall >(lib,"StaticLoadObject","StaticLoadObject")
		->args({"Class","InOuter","Name","Filename","LoadFlags","Sandbox","bAllowObjectReconciliation","InstancingContext"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->arg_init(4,make_smart<ExprConstUInt>(0x0))
		->arg_init(5,make_smart<ExprConstPtr>())
		->arg_init(6,make_smart<ExprConstBool>(true))
		->arg_init(7,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:547:25
	makeExtern< UClass * (*)(UClass *,UObject *,const wchar_t *,const wchar_t *,unsigned int,UPackageMap *) , StaticLoadClass , SimNode_ExtFuncCall >(lib,"StaticLoadClass","StaticLoadClass")
		->args({"BaseClass","InOuter","Name","Filename","LoadFlags","Sandbox"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->arg_init(4,make_smart<ExprConstUInt>(0x0))
		->arg_init(5,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:558:26
	makeExtern< UObject * (*)(const FStaticConstructObjectParameters &) , StaticConstructObject_Internal , SimNode_ExtFuncCall >(lib,"StaticConstructObject_Internal","StaticConstructObject_Internal")
		->args({"Params"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:577:26
	makeExtern< UObject * (*)(const UObject *,UObject *,const FName,EObjectFlags,UClass *,EDuplicateMode::Type,EInternalObjectFlags) , StaticDuplicateObject , SimNode_ExtFuncCall >(lib,"StaticDuplicateObject","StaticDuplicateObject")
		->args({"SourceObject","DestOuter","DestName","FlagMask","DestClass","DuplicateMode","InternalFlagsMask"})
		->arg_init(3,make_smart<ExprConstEnumeration>(-1,makeType<EObjectFlags>(lib)))
		->arg_init(4,make_smart<ExprConstPtr>())
		->arg_init(5,make_smart<ExprConstEnumeration>(0,makeType<EDuplicateMode::Type>(lib)))
		->arg_init(6,make_smart<ExprConstEnumeration>(-5242880,makeType<EInternalObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:613:22
	makeExtern< void (*)(float,bool,float) , StaticTick , SimNode_ExtFuncCall >(lib,"StaticTick","StaticTick")
		->args({"DeltaTime","bUseFullTimeLimit","AsyncLoadingTime"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->arg_init(2,make_smart<ExprConstFloat>(0.00499999988824129))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:627:27
	makeExtern< UPackage * (*)(UPackage *,const wchar_t *,unsigned int,FArchive *,const FLinkerInstancingContext *) , LoadPackage , SimNode_ExtFuncCall >(lib,"LoadPackage","LoadPackage")
		->args({"InOuter","InLongPackageName","LoadFlags","InReaderOverride","InstancingContext"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->arg_init(4,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:712:22
	makeExtern< void (*)() , CancelAsyncLoading , SimNode_ExtFuncCall >(lib,"CancelAsyncLoading","CancelAsyncLoading")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:724:22
	makeExtern< bool (*)() , IsEventDrivenLoaderEnabled , SimNode_ExtFuncCall >(lib,"IsEventDrivenLoaderEnabled","IsEventDrivenLoaderEnabled")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:733:23
	makeExtern< float (*)(const FName &) , GetAsyncLoadPercentage , SimNode_ExtFuncCall >(lib,"GetAsyncLoadPercentage","GetAsyncLoadPercentage")
		->args({"PackageName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:738:22
	makeExtern< bool (*)() , IsInGarbageCollectorThread , SimNode_ExtFuncCall >(lib,"IsInGarbageCollectorThread","IsInGarbageCollectorThread")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:746:22
	makeExtern< void (*)(EObjectFlags,bool) , CollectGarbage , SimNode_ExtFuncCall >(lib,"CollectGarbage","CollectGarbage")
		->args({"KeepFlags","bPerformFullPurge"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:754:22
	makeExtern< bool (*)(EObjectFlags,bool) , TryCollectGarbage , SimNode_ExtFuncCall >(lib,"TryCollectGarbage","TryCollectGarbage")
		->args({"KeepFlags","bPerformFullPurge"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:764:22
	makeExtern< bool (*)(bool,double) , UnhashUnreachableObjects , SimNode_ExtFuncCall >(lib,"UnhashUnreachableObjects","UnhashUnreachableObjects")
		->args({"bUseTimeLimit","TimeLimit"})
		->arg_init(1,make_smart<ExprConstDouble>(0.00000000000000000))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:771:22
	makeExtern< bool (*)() , IsIncrementalUnhashPending , SimNode_ExtFuncCall >(lib,"IsIncrementalUnhashPending","IsIncrementalUnhashPending")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:778:22
	makeExtern< bool (*)() , IsIncrementalPurgePending , SimNode_ExtFuncCall >(lib,"IsIncrementalPurgePending","IsIncrementalPurgePending")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:785:22
	makeExtern< void (*)(bool) , GatherUnreachableObjects , SimNode_ExtFuncCall >(lib,"GatherUnreachableObjects","GatherUnreachableObjects")
		->args({"bForceSingleThreaded"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:796:22
	makeExtern< void (*)(bool,double) , IncrementalPurgeGarbage , SimNode_ExtFuncCall >(lib,"IncrementalPurgeGarbage","IncrementalPurgeGarbage")
		->args({"bUseTimeLimit","TimeLimit"})
		->arg_init(1,make_smart<ExprConstDouble>(0.00200000000000000))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:818:23
	makeExtern< FName (*)(UObject *,const UClass *,FName,EUniqueObjectNameOptions) , MakeUniqueObjectName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"MakeUniqueObjectName","MakeUniqueObjectName")
		->args({"Outer","Class","BaseName","Options"})
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<EUniqueObjectNameOptions>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:832:23
	makeExtern< FName (*)(const FString &,const FName) , MakeObjectNameFromDisplayLabel , SimNode_ExtFuncCallAndCopyOrMove >(lib,"MakeObjectNameFromDisplayLabel","MakeObjectNameFromDisplayLabel")
		->args({"DisplayLabel","CurrentObjectName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:864:23
	makeExtern< int (*)() , GetNumAsyncPackages , SimNode_ExtFuncCall >(lib,"GetNumAsyncPackages","GetNumAsyncPackages")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:871:22
	makeExtern< bool (*)() , IsLoading , SimNode_ExtFuncCall >(lib,"IsLoading","IsLoading")
		->addToModule(*this, SideEffects::worstDefault);
}
}

