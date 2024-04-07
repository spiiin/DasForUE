// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_5() {
	using _method_271 = das::das_call_member< void (UObjectBase::*)(EObjectFlags),&UObjectBase::AtomicallyClearFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:220:21
	makeExtern<DAS_CALL_METHOD(_method_271), SimNode_ExtFuncCall >(lib,"AtomicallyClearFlags","das_call_member< void (UObjectBase::*)(EObjectFlags) , &UObjectBase::AtomicallyClearFlags >::invoke")
		->args({"self","FlagsToClear"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:234:14
	makeExtern< void (*)(const UObjectBase *,UObject *) , _dasUnreal_static_97_PrefetchClass , SimNode_ExtFuncCall >(lib,"PrefetchClass","_dasUnreal_static_97_PrefetchClass")
		->args({"self","Object"})
		->arg_type(0,makeType<TExplicit<const UObjectBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:235:14
	makeExtern< void (*)(const UObjectBase *,UObject *) , _dasUnreal_static_98_PrefetchOuter , SimNode_ExtFuncCall >(lib,"PrefetchOuter","_dasUnreal_static_98_PrefetchOuter")
		->args({"self","Object"})
		->arg_type(0,makeType<TExplicit<const UObjectBase*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:276:22
	makeExtern< bool (*)() , UObjectInitialized , SimNode_ExtFuncCall >(lib,"UObjectInitialized","UObjectInitialized")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:281:22
	makeExtern< void (*)(UObjectBase *,bool) , UObjectForceRegistration , SimNode_ExtFuncCall >(lib,"UObjectForceRegistration","UObjectForceRegistration")
		->args({"Object","bCheckForModuleRelease"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:446:38
	makeExtern< UScriptStruct * (*)(UObject *,const wchar_t *,SIZE_T,unsigned int,bool) , FindExistingStructIfHotReloadOrDynamic , SimNode_ExtFuncCall >(lib,"FindExistingStructIfHotReloadOrDynamic","FindExistingStructIfHotReloadOrDynamic")
		->args({"Outer","StructName","Size","Crc","bIsDynamic"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:451:30
	makeExtern< UEnum * (*)(UObject *,const wchar_t *,SIZE_T,unsigned int,bool) , FindExistingEnumIfHotReloadOrDynamic , SimNode_ExtFuncCall >(lib,"FindExistingEnumIfHotReloadOrDynamic","FindExistingEnumIfHotReloadOrDynamic")
		->args({"Outer","EnumName","Size","Crc","bIsDynamic"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBase.h:454:22
	makeExtern< void (*)(FName,bool) , ProcessNewlyLoadedUObjects , SimNode_ExtFuncCall >(lib,"ProcessNewlyLoadedUObjects","ProcessNewlyLoadedUObjects")
		->args({"Package","bCanProcessNewlyLoadedObjects"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectMarks.h:67:22
	makeExtern< void (*)(const UObjectBase *,EObjectMark) , MarkObject , SimNode_ExtFuncCall >(lib,"MarkObject","MarkObject")
		->args({"Object","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectMarks.h:75:22
	makeExtern< void (*)(const UObjectBase *,EObjectMark) , UnMarkObject , SimNode_ExtFuncCall >(lib,"UnMarkObject","UnMarkObject")
		->args({"Object","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectMarks.h:83:22
	makeExtern< void (*)(EObjectMark) , MarkAllObjects , SimNode_ExtFuncCall >(lib,"MarkAllObjects","MarkAllObjects")
		->args({"Marks"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectMarks.h:91:22
	makeExtern< void (*)(EObjectMark) , UnMarkAllObjects , SimNode_ExtFuncCall >(lib,"UnMarkAllObjects","UnMarkAllObjects")
		->args({"Marks"})
		->arg_init(0,make_smart<ExprConstEnumeration>(-1,makeType<EObjectMark>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectMarks.h:100:22
	makeExtern< bool (*)(const UObjectBase *,EObjectMark) , ObjectHasAnyMarks , SimNode_ExtFuncCall >(lib,"ObjectHasAnyMarks","ObjectHasAnyMarks")
		->args({"Object","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectMarks.h:109:22
	makeExtern< bool (*)(const UObjectBase *,EObjectMark) , ObjectHasAllMarks , SimNode_ExtFuncCall >(lib,"ObjectHasAllMarks","ObjectHasAllMarks")
		->args({"Object","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectMarks.h:117:29
	makeExtern< EObjectMark (*)(const UObjectBase *) , ObjectGetAllMarks , SimNode_ExtFuncCall >(lib,"ObjectGetAllMarks","ObjectGetAllMarks")
		->args({"Object"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<UObjectBaseUtility>(*this,lib,"UObjectBaseUtility","UObjectBaseUtility");
	addCtorAndUsing<UObjectBaseUtility,EObjectFlags>(*this,lib,"UObjectBaseUtility","UObjectBaseUtility")
		->args({"InFlags"});
	using _method_274 = das::das_call_member< void (UObjectBaseUtility::*)(EObjectFlags),&UObjectBaseUtility::SetFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:100:19
	makeExtern<DAS_CALL_METHOD(_method_274), SimNode_ExtFuncCall >(lib,"SetFlags","das_call_member< void (UObjectBaseUtility::*)(EObjectFlags) , &UObjectBaseUtility::SetFlags >::invoke")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_275 = das::das_call_member< void (UObjectBaseUtility::*)(EObjectFlags),&UObjectBaseUtility::ClearFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:108:19
	makeExtern<DAS_CALL_METHOD(_method_275), SimNode_ExtFuncCall >(lib,"ClearFlags","das_call_member< void (UObjectBaseUtility::*)(EObjectFlags) , &UObjectBaseUtility::ClearFlags >::invoke")
		->args({"self","FlagsToClear"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_276 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const,&UObjectBaseUtility::HasAnyFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:121:19
	makeExtern<DAS_CALL_METHOD(_method_276), SimNode_ExtFuncCall >(lib,"HasAnyFlags","das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const , &UObjectBaseUtility::HasAnyFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_277 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const,&UObjectBaseUtility::HasAllFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:133:19
	makeExtern<DAS_CALL_METHOD(_method_277), SimNode_ExtFuncCall >(lib,"HasAllFlags","das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const , &UObjectBaseUtility::HasAllFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_278 = das::das_call_member< EObjectFlags (UObjectBaseUtility::*)(EObjectFlags) const,&UObjectBaseUtility::GetMaskedFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:145:27
	makeExtern<DAS_CALL_METHOD(_method_278), SimNode_ExtFuncCall >(lib,"GetMaskedFlags","das_call_member< EObjectFlags (UObjectBaseUtility::*)(EObjectFlags) const , &UObjectBaseUtility::GetMaskedFlags >::invoke")
		->args({"self","Mask"})
		->arg_init(1,make_smart<ExprConstEnumeration>(-1,makeType<EObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_279 = das::das_call_member< void (UObjectBaseUtility::*)(EObjectMark) const,&UObjectBaseUtility::Mark >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:159:19
	makeExtern<DAS_CALL_METHOD(_method_279), SimNode_ExtFuncCall >(lib,"Mark","das_call_member< void (UObjectBaseUtility::*)(EObjectMark) const , &UObjectBaseUtility::Mark >::invoke")
		->args({"self","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_280 = das::das_call_member< void (UObjectBaseUtility::*)(EObjectMark) const,&UObjectBaseUtility::UnMark >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:169:19
	makeExtern<DAS_CALL_METHOD(_method_280), SimNode_ExtFuncCall >(lib,"UnMark","das_call_member< void (UObjectBaseUtility::*)(EObjectMark) const , &UObjectBaseUtility::UnMark >::invoke")
		->args({"self","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_281 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectMark) const,&UObjectBaseUtility::HasAnyMarks >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:180:19
	makeExtern<DAS_CALL_METHOD(_method_281), SimNode_ExtFuncCall >(lib,"HasAnyMarks","das_call_member< bool (UObjectBaseUtility::*)(EObjectMark) const , &UObjectBaseUtility::HasAnyMarks >::invoke")
		->args({"self","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_282 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectMark) const,&UObjectBaseUtility::HasAllMarks >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:191:19
	makeExtern<DAS_CALL_METHOD(_method_282), SimNode_ExtFuncCall >(lib,"HasAllMarks","das_call_member< bool (UObjectBaseUtility::*)(EObjectMark) const , &UObjectBaseUtility::HasAllMarks >::invoke")
		->args({"self","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_283 = das::das_call_member< EObjectMark (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetAllMarks >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:202:26
	makeExtern<DAS_CALL_METHOD(_method_283), SimNode_ExtFuncCall >(lib,"GetAllMarks","das_call_member< EObjectMark (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetAllMarks >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_284 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsPendingKill >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:211:19
	makeExtern<DAS_CALL_METHOD(_method_284), SimNode_ExtFuncCall >(lib,"IsPendingKill","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsPendingKill >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_285 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::MarkPendingKill >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:231:19
	makeExtern<DAS_CALL_METHOD(_method_285), SimNode_ExtFuncCall >(lib,"MarkPendingKill","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::MarkPendingKill >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_286 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::ClearPendingKill >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:248:19
	makeExtern<DAS_CALL_METHOD(_method_286), SimNode_ExtFuncCall >(lib,"ClearPendingKill","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::ClearPendingKill >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_287 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::MarkAsGarbage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:263:19
	makeExtern<DAS_CALL_METHOD(_method_287), SimNode_ExtFuncCall >(lib,"MarkAsGarbage","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::MarkAsGarbage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_288 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::ClearGarbage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:279:19
	makeExtern<DAS_CALL_METHOD(_method_288), SimNode_ExtFuncCall >(lib,"ClearGarbage","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::ClearGarbage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_289 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::AddToRoot >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:295:19
	makeExtern<DAS_CALL_METHOD(_method_289), SimNode_ExtFuncCall >(lib,"AddToRoot","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::AddToRoot >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_290 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::RemoveFromRoot >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:301:19
	makeExtern<DAS_CALL_METHOD(_method_290), SimNode_ExtFuncCall >(lib,"RemoveFromRoot","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::RemoveFromRoot >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_291 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsRooted >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:311:19
	makeExtern<DAS_CALL_METHOD(_method_291), SimNode_ExtFuncCall >(lib,"IsRooted","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsRooted >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_292 = das::das_call_member< bool (UObjectBaseUtility::*)(),&UObjectBaseUtility::ThisThreadAtomicallyClearedRFUnreachable >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:321:19
	makeExtern<DAS_CALL_METHOD(_method_292), SimNode_ExtFuncCall >(lib,"ThisThreadAtomicallyClearedRFUnreachable","das_call_member< bool (UObjectBaseUtility::*)() , &UObjectBaseUtility::ThisThreadAtomicallyClearedRFUnreachable >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_293 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsUnreachable >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:327:19
	makeExtern<DAS_CALL_METHOD(_method_293), SimNode_ExtFuncCall >(lib,"IsUnreachable","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsUnreachable >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_294 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsPendingKillOrUnreachable >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:334:19
	makeExtern<DAS_CALL_METHOD(_method_294), SimNode_ExtFuncCall >(lib,"IsPendingKillOrUnreachable","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsPendingKillOrUnreachable >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_295 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsNative >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:342:19
	makeExtern<DAS_CALL_METHOD(_method_295), SimNode_ExtFuncCall >(lib,"IsNative","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsNative >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_296 = das::das_call_member< void (UObjectBaseUtility::*)(EInternalObjectFlags) const,&UObjectBaseUtility::SetInternalFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:353:19
	makeExtern<DAS_CALL_METHOD(_method_296), SimNode_ExtFuncCall >(lib,"SetInternalFlags","das_call_member< void (UObjectBaseUtility::*)(EInternalObjectFlags) const , &UObjectBaseUtility::SetInternalFlags >::invoke")
		->args({"self","FlagsToSet"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_297 = das::das_call_member< EInternalObjectFlags (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetInternalFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:368:35
	makeExtern<DAS_CALL_METHOD(_method_297), SimNode_ExtFuncCall >(lib,"GetInternalFlags","das_call_member< EInternalObjectFlags (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetInternalFlags >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_298 = das::das_call_member< bool (UObjectBaseUtility::*)(EInternalObjectFlags) const,&UObjectBaseUtility::HasAnyInternalFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:379:19
	makeExtern<DAS_CALL_METHOD(_method_298), SimNode_ExtFuncCall >(lib,"HasAnyInternalFlags","das_call_member< bool (UObjectBaseUtility::*)(EInternalObjectFlags) const , &UObjectBaseUtility::HasAnyInternalFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_299 = das::das_call_member< void (UObjectBaseUtility::*)(EInternalObjectFlags) const,&UObjectBaseUtility::ClearInternalFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:390:19
	makeExtern<DAS_CALL_METHOD(_method_299), SimNode_ExtFuncCall >(lib,"ClearInternalFlags","das_call_member< void (UObjectBaseUtility::*)(EInternalObjectFlags) const , &UObjectBaseUtility::ClearInternalFlags >::invoke")
		->args({"self","FlagsToClear"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_300 = das::das_call_member< bool (UObjectBaseUtility::*)(EInternalObjectFlags) const,&UObjectBaseUtility::AtomicallyClearInternalFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:405:19
	makeExtern<DAS_CALL_METHOD(_method_300), SimNode_ExtFuncCall >(lib,"AtomicallyClearInternalFlags","das_call_member< bool (UObjectBaseUtility::*)(EInternalObjectFlags) const , &UObjectBaseUtility::AtomicallyClearInternalFlags >::invoke")
		->args({"self","FlagsToClear"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:467:21
	makeExtern< bool (*)(const UObjectBaseUtility *) , _dasUnreal_static_99_IsPendingKillEnabled , SimNode_ExtFuncCall >(lib,"IsPendingKillEnabled","_dasUnreal_static_99_IsPendingKillEnabled")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UObjectBaseUtility*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:473:42
	makeExtern< EInternalObjectFlags (*)(const UObjectBaseUtility *,const EInternalObjectFlags) , _dasUnreal_static_100_FixGarbageOrPendingKillInternalObjectFlags , SimNode_ExtFuncCall >(lib,"FixGarbageOrPendingKillInternalObjectFlags","_dasUnreal_static_100_FixGarbageOrPendingKillInternalObjectFlags")
		->args({"self","InFlags"})
		->arg_type(0,makeType<TExplicit<const UObjectBaseUtility*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:496:15
	makeExtern< bool (*)(const UObjectBaseUtility &) , _dasUnreal_virtual_101_CanBeClusterRoot , SimNode_ExtFuncCall >(lib,"CanBeClusterRoot","_dasUnreal_virtual_101_CanBeClusterRoot")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:506:31
	makeExtern< bool (*)(const UObjectBaseUtility &) , _dasUnreal_virtual_102_CanBeInCluster , SimNode_ExtFuncCall >(lib,"CanBeInCluster","_dasUnreal_virtual_102_CanBeInCluster")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:511:31
	makeExtern< void (*)(UObjectBaseUtility &) , _dasUnreal_virtual_103_CreateCluster , SimNode_ExtFuncCall >(lib,"CreateCluster","_dasUnreal_virtual_103_CreateCluster")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:516:15
	makeExtern< void (*)(UObjectBaseUtility &) , _dasUnreal_virtual_104_OnClusterMarkedAsPendingKill , SimNode_ExtFuncCall >(lib,"OnClusterMarkedAsPendingKill","_dasUnreal_virtual_104_OnClusterMarkedAsPendingKill")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_307 = das::das_call_member< void (UObjectBaseUtility::*)(UObjectBaseUtility *,bool),&UObjectBaseUtility::AddToCluster >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:523:23
	makeExtern<DAS_CALL_METHOD(_method_307), SimNode_ExtFuncCall >(lib,"AddToCluster","das_call_member< void (UObjectBaseUtility::*)(UObjectBaseUtility *,bool) , &UObjectBaseUtility::AddToCluster >::invoke")
		->args({"self","ClusterRootOrObjectFromCluster","bAddAsMutableObject"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_308 = das::das_call_member< FString (UObjectBaseUtility::*)(bool) const,&UObjectBaseUtility::GetFullGroupName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:536:26
	makeExtern<DAS_CALL_METHOD(_method_308), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFullGroupName","das_call_member< FString (UObjectBaseUtility::*)(bool) const , &UObjectBaseUtility::GetFullGroupName >::invoke")
		->args({"self","bStartWithOuter"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_309 = das::das_call_member< FString (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:543:22
	makeExtern<DAS_CALL_METHOD(_method_309), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetName","das_call_member< FString (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_310 = das::das_call_member< void (UObjectBaseUtility::*)(FString &) const,&UObjectBaseUtility::GetName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:549:19
	makeExtern<DAS_CALL_METHOD(_method_310), SimNode_ExtFuncCall >(lib,"GetName","das_call_member< void (UObjectBaseUtility::*)(FString &) const , &UObjectBaseUtility::GetName >::invoke")
		->args({"self","ResultString"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_311 = das::das_call_member< void (UObjectBaseUtility::*)(FString &) const,&UObjectBaseUtility::AppendName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:554:19
	makeExtern<DAS_CALL_METHOD(_method_311), SimNode_ExtFuncCall >(lib,"AppendName","das_call_member< void (UObjectBaseUtility::*)(FString &) const , &UObjectBaseUtility::AppendName >::invoke")
		->args({"self","ResultString"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_312 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsPackageExternal >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:568:23
	makeExtern<DAS_CALL_METHOD(_method_312), SimNode_ExtFuncCall >(lib,"IsPackageExternal","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsPackageExternal >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_313 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::DetachExternalPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:574:23
	makeExtern<DAS_CALL_METHOD(_method_313), SimNode_ExtFuncCall >(lib,"DetachExternalPackage","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::DetachExternalPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_314 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::ReattachExternalPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:580:23
	makeExtern<DAS_CALL_METHOD(_method_314), SimNode_ExtFuncCall >(lib,"ReattachExternalPackage","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::ReattachExternalPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_315 = das::das_call_member< UObject * (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetOutermostObject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:587:27
	makeExtern<DAS_CALL_METHOD(_method_315), SimNode_ExtFuncCall >(lib,"GetOutermostObject","das_call_member< UObject * (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetOutermostObject >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_316 = das::das_call_member< UPackage * (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:594:28
	makeExtern<DAS_CALL_METHOD(_method_316), SimNode_ExtFuncCall >(lib,"GetPackage","das_call_member< UPackage * (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_317 = das::das_call_member< UPackage * (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetOutermost >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:611:28
	makeExtern<DAS_CALL_METHOD(_method_317), SimNode_ExtFuncCall >(lib,"GetOutermost","das_call_member< UPackage * (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetOutermost >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_318 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::MarkPackageDirty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:619:23
	makeExtern<DAS_CALL_METHOD(_method_318), SimNode_ExtFuncCall >(lib,"MarkPackageDirty","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::MarkPackageDirty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_319 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const,&UObjectBaseUtility::IsTemplate >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:626:23
	makeExtern<DAS_CALL_METHOD(_method_319), SimNode_ExtFuncCall >(lib,"IsTemplate","das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const , &UObjectBaseUtility::IsTemplate >::invoke")
		->args({"self","TemplateTypes"})
		->arg_init(1,make_smart<ExprConstEnumeration>(48,makeType<EObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_320 = das::das_call_member< UObject * (UObjectBaseUtility::*)(UClass *) const,&UObjectBaseUtility::GetTypedOuter >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:634:27
	makeExtern<DAS_CALL_METHOD(_method_320), SimNode_ExtFuncCall >(lib,"GetTypedOuter","das_call_member< UObject * (UObjectBaseUtility::*)(UClass *) const , &UObjectBaseUtility::GetTypedOuter >::invoke")
		->args({"self","Target"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_321 = das::das_call_member< UObjectBaseUtility * (UObjectBaseUtility::*)(const UClass *) const,&UObjectBaseUtility::GetImplementingOuterObject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:669:38
	makeExtern<DAS_CALL_METHOD(_method_321), SimNode_ExtFuncCall >(lib,"GetImplementingOuterObject","das_call_member< UObjectBaseUtility * (UObjectBaseUtility::*)(const UClass *) const , &UObjectBaseUtility::GetImplementingOuterObject >::invoke")
		->args({"self","InInterfaceClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_322 = das::das_call_member< bool (UObjectBaseUtility::*)(const UObject *) const,&UObjectBaseUtility::IsIn >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:675:23
	makeExtern<DAS_CALL_METHOD(_method_322), SimNode_ExtFuncCall >(lib,"IsIn","das_call_member< bool (UObjectBaseUtility::*)(const UObject *) const , &UObjectBaseUtility::IsIn >::invoke")
		->args({"self","SomeOuter"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_323 = das::das_call_member< bool (UObjectBaseUtility::*)(const UPackage *) const,&UObjectBaseUtility::IsIn >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:681:23
	makeExtern<DAS_CALL_METHOD(_method_323), SimNode_ExtFuncCall >(lib,"IsIn","das_call_member< bool (UObjectBaseUtility::*)(const UPackage *) const , &UObjectBaseUtility::IsIn >::invoke")
		->args({"self","SomePackage"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_324 = das::das_call_member< bool (UObjectBaseUtility::*)(const UObject *) const,&UObjectBaseUtility::IsInOuter >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:684:23
	makeExtern<DAS_CALL_METHOD(_method_324), SimNode_ExtFuncCall >(lib,"IsInOuter","das_call_member< bool (UObjectBaseUtility::*)(const UObject *) const , &UObjectBaseUtility::IsInOuter >::invoke")
		->args({"self","SomeOuter"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_325 = das::das_call_member< bool (UObjectBaseUtility::*)(const UPackage *) const,&UObjectBaseUtility::IsInPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:687:23
	makeExtern<DAS_CALL_METHOD(_method_325), SimNode_ExtFuncCall >(lib,"IsInPackage","das_call_member< bool (UObjectBaseUtility::*)(const UPackage *) const , &UObjectBaseUtility::IsInPackage >::invoke")
		->args({"self","SomePackage"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_326 = das::das_call_member< bool (UObjectBaseUtility::*)(const UClass *) const,&UObjectBaseUtility::IsInA >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:694:23
	makeExtern<DAS_CALL_METHOD(_method_326), SimNode_ExtFuncCall >(lib,"IsInA","das_call_member< bool (UObjectBaseUtility::*)(const UClass *) const , &UObjectBaseUtility::IsInA >::invoke")
		->args({"self","SomeBaseClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_327 = das::das_call_member< bool (UObjectBaseUtility::*)(unsigned int) const,&UObjectBaseUtility::RootPackageHasAnyFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:703:23
	makeExtern<DAS_CALL_METHOD(_method_327), SimNode_ExtFuncCall >(lib,"RootPackageHasAnyFlags","das_call_member< bool (UObjectBaseUtility::*)(unsigned int) const , &UObjectBaseUtility::RootPackageHasAnyFlags >::invoke")
		->args({"self","CheckFlagMask"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_328 = das::das_call_member< const UClass * (UObjectBaseUtility::*)(const UClass *) const,&UObjectBaseUtility::FindNearestCommonBaseClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:751:32
	makeExtern<DAS_CALL_METHOD(_method_328), SimNode_ExtFuncCall >(lib,"FindNearestCommonBaseClass","das_call_member< const UClass * (UObjectBaseUtility::*)(const UClass *) const , &UObjectBaseUtility::FindNearestCommonBaseClass >::invoke")
		->args({"self","TestClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_329 = das::das_call_member< void * (UObjectBaseUtility::*)(UClass *),&UObjectBaseUtility::GetInterfaceAddress >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:762:24
	makeExtern<DAS_CALL_METHOD(_method_329), SimNode_ExtFuncCall >(lib,"GetInterfaceAddress","das_call_member< void * (UObjectBaseUtility::*)(UClass *) , &UObjectBaseUtility::GetInterfaceAddress >::invoke")
		->args({"self","InterfaceClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_330 = das::das_call_member< void * (UObjectBaseUtility::*)(UClass *),&UObjectBaseUtility::GetNativeInterfaceAddress >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:768:24
	makeExtern<DAS_CALL_METHOD(_method_330), SimNode_ExtFuncCall >(lib,"GetNativeInterfaceAddress","das_call_member< void * (UObjectBaseUtility::*)(UClass *) , &UObjectBaseUtility::GetNativeInterfaceAddress >::invoke")
		->args({"self","InterfaceClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_331 = das::das_call_member< const void * (UObjectBaseUtility::*)(UClass *) const,&UObjectBaseUtility::GetNativeInterfaceAddress >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:774:14
	makeExtern<DAS_CALL_METHOD(_method_331), SimNode_ExtFuncCall >(lib,"GetNativeInterfaceAddress","das_call_member< const void * (UObjectBaseUtility::*)(UClass *) const , &UObjectBaseUtility::GetNativeInterfaceAddress >::invoke")
		->args({"self","InterfaceClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_332 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:784:23
	makeExtern<DAS_CALL_METHOD(_method_332), SimNode_ExtFuncCall >(lib,"IsDefaultSubobject","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsDefaultSubobject >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_333 = das::das_call_member< int (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetLinkerIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:804:24
	makeExtern<DAS_CALL_METHOD(_method_333), SimNode_ExtFuncCall >(lib,"GetLinkerIndex","das_call_member< int (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetLinkerIndex >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_334 = das::das_call_member< int (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetLinkerUE4Version >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:818:15
	makeExtern<DAS_CALL_METHOD(_method_334), SimNode_ExtFuncCall >(lib,"GetLinkerUE4Version","das_call_member< int (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetLinkerUE4Version >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_335 = das::das_call_member< int (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetLinkerLicenseeUEVersion >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:835:24
	makeExtern<DAS_CALL_METHOD(_method_335), SimNode_ExtFuncCall >(lib,"GetLinkerLicenseeUEVersion","das_call_member< int (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetLinkerLicenseeUEVersion >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_336 = das::das_call_member< int (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetLinkerLicenseeUE4Version >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:838:15
	makeExtern<DAS_CALL_METHOD(_method_336), SimNode_ExtFuncCall >(lib,"GetLinkerLicenseeUE4Version","das_call_member< int (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetLinkerLicenseeUE4Version >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_337 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::ResetStatID >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:868:19
	makeExtern<DAS_CALL_METHOD(_method_337), SimNode_ExtFuncCall >(lib,"ResetStatID","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::ResetStatID >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:918:18
	makeExtern< bool (*)(UObject *) , IsPossiblyAllocatedUObjectPointer , SimNode_ExtFuncCall >(lib,"IsPossiblyAllocatedUObjectPointer","IsPossiblyAllocatedUObjectPointer")
		->args({"Ptr"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:946:21
	makeExtern< FString (*)(const UObjectBaseUtility *) , GetNameSafe , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameSafe","GetNameSafe")
		->args({"Object"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:963:21
	makeExtern< FString (*)(const UObjectBaseUtility *) , GetPathNameSafe , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetPathNameSafe","GetPathNameSafe")
		->args({"Object"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:980:21
	makeExtern< FString (*)(const UObjectBaseUtility *) , GetFullNameSafe , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFullNameSafe","GetFullNameSafe")
		->args({"Object"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:996:25
	makeExtern< UClass * (*)(UClass *) , GetParentNativeClass , SimNode_ExtFuncCall >(lib,"GetParentNativeClass","GetParentNativeClass")
		->args({"Class"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:1003:23
	makeExtern< FName (*)(const UObjectBaseUtility *) , GetClassTraceScope , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetClassTraceScope","GetClassTraceScope")
		->args({"Object"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:88:2
	makeExtern< UClass * (*)(const UObject *) , _dasUnreal_static_105_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_105_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UObject*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:95:22
	makeExtern< const wchar_t * (*)(const UObject *) , _dasUnreal_static_106_StaticConfigName , SimNode_ExtFuncCall >(lib,"StaticConfigName","_dasUnreal_static_106_StaticConfigName")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UObject*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:99:14
	makeExtern< void (*)(const UObject *) , _dasUnreal_static_107_StaticRegisterNativesUObject , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUObject","_dasUnreal_static_107_StaticRegisterNativesUObject")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UObject*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_341 = das::das_call_member< UObject * (UObject::*)(FName,UClass *,UClass *,bool,bool),&UObject::CreateDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:120:27
	makeExtern<DAS_CALL_METHOD(_method_341), SimNode_ExtFuncCall >(lib,"CreateDefaultSubobject","das_call_member< UObject * (UObject::*)(FName,UClass *,UClass *,bool,bool) , &UObject::CreateDefaultSubobject >::invoke")
		->args({"self","SubobjectFName","ReturnType","ClassToCreateByDefault","bIsRequired","bIsTransient"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_342 = das::das_call_member< UObject * (UObject::*)(FName),&UObject::GetDefaultSubobjectByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:199:27
	makeExtern<DAS_CALL_METHOD(_method_342), SimNode_ExtFuncCall >(lib,"GetDefaultSubobjectByName","das_call_member< UObject * (UObject::*)(FName) , &UObject::GetDefaultSubobjectByName >::invoke")
		->args({"self","ToFind"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:217:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_108_PostInitProperties , SimNode_ExtFuncCall >(lib,"PostInitProperties","_dasUnreal_virtual_108_PostInitProperties")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:223:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_109_PostReinitProperties , SimNode_ExtFuncCall >(lib,"PostReinitProperties","_dasUnreal_virtual_109_PostReinitProperties")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:229:15
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_110_PostCDOContruct , SimNode_ExtFuncCall >(lib,"PostCDOContruct","_dasUnreal_virtual_110_PostCDOContruct")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:256:31
	makeExtern< bool (*)(UObject &,const wchar_t *) , _dasUnreal_virtual_111_PreSaveRoot , SimNode_ExtFuncCall >(lib,"PreSaveRoot","_dasUnreal_virtual_111_PreSaveRoot")
		->args({"self","Filename"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:268:31
	makeExtern< void (*)(UObject &,bool) , _dasUnreal_virtual_112_PostSaveRoot , SimNode_ExtFuncCall >(lib,"PostSaveRoot","_dasUnreal_virtual_112_PostSaveRoot")
		->args({"self","bCleanupIsRequired"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:299:31
	makeExtern< bool (*)(UObject &,bool) , _dasUnreal_virtual_113_Modify , SimNode_ExtFuncCall >(lib,"Modify","_dasUnreal_virtual_113_Modify")
		->args({"self","bAlwaysMarkDirty"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:302:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_114_IsCapturingAsRootObjectForTransaction , SimNode_ExtFuncCall >(lib,"IsCapturingAsRootObjectForTransaction","_dasUnreal_virtual_114_IsCapturingAsRootObjectForTransaction")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_350 = das::das_call_member< bool (UObject::*)() const,&UObject::CanModify >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:305:23
	makeExtern<DAS_CALL_METHOD(_method_350), SimNode_ExtFuncCall >(lib,"CanModify","das_call_member< bool (UObject::*)() const , &UObject::CanModify >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

