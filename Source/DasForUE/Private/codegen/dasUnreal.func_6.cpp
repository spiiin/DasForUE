// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_6() {
	using _method_339 = das::das_call_member< void (UObjectBaseUtility::*)(EObjectFlags),&UObjectBaseUtility::ClearFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:108:19
	makeExtern<DAS_CALL_METHOD(_method_339), SimNode_ExtFuncCall >(lib,"ClearFlags","das_call_member< void (UObjectBaseUtility::*)(EObjectFlags) , &UObjectBaseUtility::ClearFlags >::invoke")
		->args({"self","FlagsToClear"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_340 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const,&UObjectBaseUtility::HasAnyFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:121:19
	makeExtern<DAS_CALL_METHOD(_method_340), SimNode_ExtFuncCall >(lib,"HasAnyFlags","das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const , &UObjectBaseUtility::HasAnyFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_341 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const,&UObjectBaseUtility::HasAllFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:133:19
	makeExtern<DAS_CALL_METHOD(_method_341), SimNode_ExtFuncCall >(lib,"HasAllFlags","das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const , &UObjectBaseUtility::HasAllFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_342 = das::das_call_member< EObjectFlags (UObjectBaseUtility::*)(EObjectFlags) const,&UObjectBaseUtility::GetMaskedFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:145:27
	makeExtern<DAS_CALL_METHOD(_method_342), SimNode_ExtFuncCall >(lib,"GetMaskedFlags","das_call_member< EObjectFlags (UObjectBaseUtility::*)(EObjectFlags) const , &UObjectBaseUtility::GetMaskedFlags >::invoke")
		->args({"self","Mask"})
		->arg_init(1,make_smart<ExprConstEnumeration>(-1,makeType<EObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_343 = das::das_call_member< void (UObjectBaseUtility::*)(EObjectMark) const,&UObjectBaseUtility::Mark >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:159:19
	makeExtern<DAS_CALL_METHOD(_method_343), SimNode_ExtFuncCall >(lib,"Mark","das_call_member< void (UObjectBaseUtility::*)(EObjectMark) const , &UObjectBaseUtility::Mark >::invoke")
		->args({"self","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_344 = das::das_call_member< void (UObjectBaseUtility::*)(EObjectMark) const,&UObjectBaseUtility::UnMark >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:169:19
	makeExtern<DAS_CALL_METHOD(_method_344), SimNode_ExtFuncCall >(lib,"UnMark","das_call_member< void (UObjectBaseUtility::*)(EObjectMark) const , &UObjectBaseUtility::UnMark >::invoke")
		->args({"self","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_345 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectMark) const,&UObjectBaseUtility::HasAnyMarks >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:180:19
	makeExtern<DAS_CALL_METHOD(_method_345), SimNode_ExtFuncCall >(lib,"HasAnyMarks","das_call_member< bool (UObjectBaseUtility::*)(EObjectMark) const , &UObjectBaseUtility::HasAnyMarks >::invoke")
		->args({"self","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_346 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectMark) const,&UObjectBaseUtility::HasAllMarks >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:191:19
	makeExtern<DAS_CALL_METHOD(_method_346), SimNode_ExtFuncCall >(lib,"HasAllMarks","das_call_member< bool (UObjectBaseUtility::*)(EObjectMark) const , &UObjectBaseUtility::HasAllMarks >::invoke")
		->args({"self","Marks"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_347 = das::das_call_member< EObjectMark (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetAllMarks >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:202:26
	makeExtern<DAS_CALL_METHOD(_method_347), SimNode_ExtFuncCall >(lib,"GetAllMarks","das_call_member< EObjectMark (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetAllMarks >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_348 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsPendingKill >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:211:19
	makeExtern<DAS_CALL_METHOD(_method_348), SimNode_ExtFuncCall >(lib,"IsPendingKill","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsPendingKill >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_349 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::MarkPendingKill >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:231:19
	makeExtern<DAS_CALL_METHOD(_method_349), SimNode_ExtFuncCall >(lib,"MarkPendingKill","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::MarkPendingKill >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_350 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::ClearPendingKill >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:248:19
	makeExtern<DAS_CALL_METHOD(_method_350), SimNode_ExtFuncCall >(lib,"ClearPendingKill","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::ClearPendingKill >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_351 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::MarkAsGarbage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:263:19
	makeExtern<DAS_CALL_METHOD(_method_351), SimNode_ExtFuncCall >(lib,"MarkAsGarbage","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::MarkAsGarbage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_352 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::ClearGarbage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:279:19
	makeExtern<DAS_CALL_METHOD(_method_352), SimNode_ExtFuncCall >(lib,"ClearGarbage","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::ClearGarbage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_353 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::AddToRoot >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:295:19
	makeExtern<DAS_CALL_METHOD(_method_353), SimNode_ExtFuncCall >(lib,"AddToRoot","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::AddToRoot >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_354 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::RemoveFromRoot >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:301:19
	makeExtern<DAS_CALL_METHOD(_method_354), SimNode_ExtFuncCall >(lib,"RemoveFromRoot","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::RemoveFromRoot >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_355 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsRooted >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:311:19
	makeExtern<DAS_CALL_METHOD(_method_355), SimNode_ExtFuncCall >(lib,"IsRooted","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsRooted >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_356 = das::das_call_member< bool (UObjectBaseUtility::*)(),&UObjectBaseUtility::ThisThreadAtomicallyClearedRFUnreachable >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:321:19
	makeExtern<DAS_CALL_METHOD(_method_356), SimNode_ExtFuncCall >(lib,"ThisThreadAtomicallyClearedRFUnreachable","das_call_member< bool (UObjectBaseUtility::*)() , &UObjectBaseUtility::ThisThreadAtomicallyClearedRFUnreachable >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_357 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsUnreachable >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:327:19
	makeExtern<DAS_CALL_METHOD(_method_357), SimNode_ExtFuncCall >(lib,"IsUnreachable","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsUnreachable >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_358 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsPendingKillOrUnreachable >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:334:19
	makeExtern<DAS_CALL_METHOD(_method_358), SimNode_ExtFuncCall >(lib,"IsPendingKillOrUnreachable","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsPendingKillOrUnreachable >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_359 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsNative >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:342:19
	makeExtern<DAS_CALL_METHOD(_method_359), SimNode_ExtFuncCall >(lib,"IsNative","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsNative >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_360 = das::das_call_member< void (UObjectBaseUtility::*)(EInternalObjectFlags) const,&UObjectBaseUtility::SetInternalFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:353:19
	makeExtern<DAS_CALL_METHOD(_method_360), SimNode_ExtFuncCall >(lib,"SetInternalFlags","das_call_member< void (UObjectBaseUtility::*)(EInternalObjectFlags) const , &UObjectBaseUtility::SetInternalFlags >::invoke")
		->args({"self","FlagsToSet"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_361 = das::das_call_member< EInternalObjectFlags (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetInternalFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:368:35
	makeExtern<DAS_CALL_METHOD(_method_361), SimNode_ExtFuncCall >(lib,"GetInternalFlags","das_call_member< EInternalObjectFlags (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetInternalFlags >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_362 = das::das_call_member< bool (UObjectBaseUtility::*)(EInternalObjectFlags) const,&UObjectBaseUtility::HasAnyInternalFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:379:19
	makeExtern<DAS_CALL_METHOD(_method_362), SimNode_ExtFuncCall >(lib,"HasAnyInternalFlags","das_call_member< bool (UObjectBaseUtility::*)(EInternalObjectFlags) const , &UObjectBaseUtility::HasAnyInternalFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_363 = das::das_call_member< void (UObjectBaseUtility::*)(EInternalObjectFlags) const,&UObjectBaseUtility::ClearInternalFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:390:19
	makeExtern<DAS_CALL_METHOD(_method_363), SimNode_ExtFuncCall >(lib,"ClearInternalFlags","das_call_member< void (UObjectBaseUtility::*)(EInternalObjectFlags) const , &UObjectBaseUtility::ClearInternalFlags >::invoke")
		->args({"self","FlagsToClear"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_364 = das::das_call_member< bool (UObjectBaseUtility::*)(EInternalObjectFlags) const,&UObjectBaseUtility::AtomicallyClearInternalFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:405:19
	makeExtern<DAS_CALL_METHOD(_method_364), SimNode_ExtFuncCall >(lib,"AtomicallyClearInternalFlags","das_call_member< bool (UObjectBaseUtility::*)(EInternalObjectFlags) const , &UObjectBaseUtility::AtomicallyClearInternalFlags >::invoke")
		->args({"self","FlagsToClear"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:467:21
	makeExtern< bool (*)(const UObjectBaseUtility *) , _dasUnreal_static_124_IsPendingKillEnabled , SimNode_ExtFuncCall >(lib,"IsPendingKillEnabled","_dasUnreal_static_124_IsPendingKillEnabled")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UObjectBaseUtility*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:473:42
	makeExtern< EInternalObjectFlags (*)(const UObjectBaseUtility *,const EInternalObjectFlags) , _dasUnreal_static_125_FixGarbageOrPendingKillInternalObjectFlags , SimNode_ExtFuncCall >(lib,"FixGarbageOrPendingKillInternalObjectFlags","_dasUnreal_static_125_FixGarbageOrPendingKillInternalObjectFlags")
		->args({"self","InFlags"})
		->arg_type(0,makeType<TExplicit<const UObjectBaseUtility*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:496:15
	makeExtern< bool (*)(const UObjectBaseUtility &) , _dasUnreal_virtual_126_CanBeClusterRoot , SimNode_ExtFuncCall >(lib,"CanBeClusterRoot","_dasUnreal_virtual_126_CanBeClusterRoot")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:506:31
	makeExtern< bool (*)(const UObjectBaseUtility &) , _dasUnreal_virtual_127_CanBeInCluster , SimNode_ExtFuncCall >(lib,"CanBeInCluster","_dasUnreal_virtual_127_CanBeInCluster")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:511:31
	makeExtern< void (*)(UObjectBaseUtility &) , _dasUnreal_virtual_128_CreateCluster , SimNode_ExtFuncCall >(lib,"CreateCluster","_dasUnreal_virtual_128_CreateCluster")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:516:15
	makeExtern< void (*)(UObjectBaseUtility &) , _dasUnreal_virtual_129_OnClusterMarkedAsPendingKill , SimNode_ExtFuncCall >(lib,"OnClusterMarkedAsPendingKill","_dasUnreal_virtual_129_OnClusterMarkedAsPendingKill")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_371 = das::das_call_member< void (UObjectBaseUtility::*)(UObjectBaseUtility *,bool),&UObjectBaseUtility::AddToCluster >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:523:23
	makeExtern<DAS_CALL_METHOD(_method_371), SimNode_ExtFuncCall >(lib,"AddToCluster","das_call_member< void (UObjectBaseUtility::*)(UObjectBaseUtility *,bool) , &UObjectBaseUtility::AddToCluster >::invoke")
		->args({"self","ClusterRootOrObjectFromCluster","bAddAsMutableObject"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_372 = das::das_call_member< FString (UObjectBaseUtility::*)(bool) const,&UObjectBaseUtility::GetFullGroupName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:536:26
	makeExtern<DAS_CALL_METHOD(_method_372), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFullGroupName","das_call_member< FString (UObjectBaseUtility::*)(bool) const , &UObjectBaseUtility::GetFullGroupName >::invoke")
		->args({"self","bStartWithOuter"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_373 = das::das_call_member< FString (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:543:22
	makeExtern<DAS_CALL_METHOD(_method_373), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetName","das_call_member< FString (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_374 = das::das_call_member< void (UObjectBaseUtility::*)(FString &) const,&UObjectBaseUtility::GetName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:549:19
	makeExtern<DAS_CALL_METHOD(_method_374), SimNode_ExtFuncCall >(lib,"GetName","das_call_member< void (UObjectBaseUtility::*)(FString &) const , &UObjectBaseUtility::GetName >::invoke")
		->args({"self","ResultString"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_375 = das::das_call_member< void (UObjectBaseUtility::*)(FString &) const,&UObjectBaseUtility::AppendName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:554:19
	makeExtern<DAS_CALL_METHOD(_method_375), SimNode_ExtFuncCall >(lib,"AppendName","das_call_member< void (UObjectBaseUtility::*)(FString &) const , &UObjectBaseUtility::AppendName >::invoke")
		->args({"self","ResultString"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_376 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsPackageExternal >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:568:23
	makeExtern<DAS_CALL_METHOD(_method_376), SimNode_ExtFuncCall >(lib,"IsPackageExternal","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsPackageExternal >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_377 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::DetachExternalPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:574:23
	makeExtern<DAS_CALL_METHOD(_method_377), SimNode_ExtFuncCall >(lib,"DetachExternalPackage","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::DetachExternalPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_378 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::ReattachExternalPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:580:23
	makeExtern<DAS_CALL_METHOD(_method_378), SimNode_ExtFuncCall >(lib,"ReattachExternalPackage","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::ReattachExternalPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_379 = das::das_call_member< UObject * (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetOutermostObject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:587:27
	makeExtern<DAS_CALL_METHOD(_method_379), SimNode_ExtFuncCall >(lib,"GetOutermostObject","das_call_member< UObject * (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetOutermostObject >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_380 = das::das_call_member< UPackage * (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:594:28
	makeExtern<DAS_CALL_METHOD(_method_380), SimNode_ExtFuncCall >(lib,"GetPackage","das_call_member< UPackage * (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_381 = das::das_call_member< UPackage * (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetOutermost >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:611:28
	makeExtern<DAS_CALL_METHOD(_method_381), SimNode_ExtFuncCall >(lib,"GetOutermost","das_call_member< UPackage * (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetOutermost >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_382 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::MarkPackageDirty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:619:23
	makeExtern<DAS_CALL_METHOD(_method_382), SimNode_ExtFuncCall >(lib,"MarkPackageDirty","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::MarkPackageDirty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_383 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const,&UObjectBaseUtility::IsTemplate >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:626:23
	makeExtern<DAS_CALL_METHOD(_method_383), SimNode_ExtFuncCall >(lib,"IsTemplate","das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const , &UObjectBaseUtility::IsTemplate >::invoke")
		->args({"self","TemplateTypes"})
		->arg_init(1,make_smart<ExprConstEnumeration>(48,makeType<EObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_384 = das::das_call_member< UObject * (UObjectBaseUtility::*)(UClass *) const,&UObjectBaseUtility::GetTypedOuter >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:634:27
	makeExtern<DAS_CALL_METHOD(_method_384), SimNode_ExtFuncCall >(lib,"GetTypedOuter","das_call_member< UObject * (UObjectBaseUtility::*)(UClass *) const , &UObjectBaseUtility::GetTypedOuter >::invoke")
		->args({"self","Target"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_385 = das::das_call_member< UObjectBaseUtility * (UObjectBaseUtility::*)(const UClass *) const,&UObjectBaseUtility::GetImplementingOuterObject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:669:38
	makeExtern<DAS_CALL_METHOD(_method_385), SimNode_ExtFuncCall >(lib,"GetImplementingOuterObject","das_call_member< UObjectBaseUtility * (UObjectBaseUtility::*)(const UClass *) const , &UObjectBaseUtility::GetImplementingOuterObject >::invoke")
		->args({"self","InInterfaceClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_386 = das::das_call_member< bool (UObjectBaseUtility::*)(const UObject *) const,&UObjectBaseUtility::IsIn >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:675:23
	makeExtern<DAS_CALL_METHOD(_method_386), SimNode_ExtFuncCall >(lib,"IsIn","das_call_member< bool (UObjectBaseUtility::*)(const UObject *) const , &UObjectBaseUtility::IsIn >::invoke")
		->args({"self","SomeOuter"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_387 = das::das_call_member< bool (UObjectBaseUtility::*)(const UPackage *) const,&UObjectBaseUtility::IsIn >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:681:23
	makeExtern<DAS_CALL_METHOD(_method_387), SimNode_ExtFuncCall >(lib,"IsIn","das_call_member< bool (UObjectBaseUtility::*)(const UPackage *) const , &UObjectBaseUtility::IsIn >::invoke")
		->args({"self","SomePackage"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_388 = das::das_call_member< bool (UObjectBaseUtility::*)(const UObject *) const,&UObjectBaseUtility::IsInOuter >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:684:23
	makeExtern<DAS_CALL_METHOD(_method_388), SimNode_ExtFuncCall >(lib,"IsInOuter","das_call_member< bool (UObjectBaseUtility::*)(const UObject *) const , &UObjectBaseUtility::IsInOuter >::invoke")
		->args({"self","SomeOuter"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_389 = das::das_call_member< bool (UObjectBaseUtility::*)(const UPackage *) const,&UObjectBaseUtility::IsInPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:687:23
	makeExtern<DAS_CALL_METHOD(_method_389), SimNode_ExtFuncCall >(lib,"IsInPackage","das_call_member< bool (UObjectBaseUtility::*)(const UPackage *) const , &UObjectBaseUtility::IsInPackage >::invoke")
		->args({"self","SomePackage"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_390 = das::das_call_member< bool (UObjectBaseUtility::*)(const UClass *) const,&UObjectBaseUtility::IsInA >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:694:23
	makeExtern<DAS_CALL_METHOD(_method_390), SimNode_ExtFuncCall >(lib,"IsInA","das_call_member< bool (UObjectBaseUtility::*)(const UClass *) const , &UObjectBaseUtility::IsInA >::invoke")
		->args({"self","SomeBaseClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_391 = das::das_call_member< bool (UObjectBaseUtility::*)(unsigned int) const,&UObjectBaseUtility::RootPackageHasAnyFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:703:23
	makeExtern<DAS_CALL_METHOD(_method_391), SimNode_ExtFuncCall >(lib,"RootPackageHasAnyFlags","das_call_member< bool (UObjectBaseUtility::*)(unsigned int) const , &UObjectBaseUtility::RootPackageHasAnyFlags >::invoke")
		->args({"self","CheckFlagMask"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_392 = das::das_call_member< const UClass * (UObjectBaseUtility::*)(const UClass *) const,&UObjectBaseUtility::FindNearestCommonBaseClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:751:32
	makeExtern<DAS_CALL_METHOD(_method_392), SimNode_ExtFuncCall >(lib,"FindNearestCommonBaseClass","das_call_member< const UClass * (UObjectBaseUtility::*)(const UClass *) const , &UObjectBaseUtility::FindNearestCommonBaseClass >::invoke")
		->args({"self","TestClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_393 = das::das_call_member< void * (UObjectBaseUtility::*)(UClass *),&UObjectBaseUtility::GetInterfaceAddress >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:762:24
	makeExtern<DAS_CALL_METHOD(_method_393), SimNode_ExtFuncCall >(lib,"GetInterfaceAddress","das_call_member< void * (UObjectBaseUtility::*)(UClass *) , &UObjectBaseUtility::GetInterfaceAddress >::invoke")
		->args({"self","InterfaceClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_394 = das::das_call_member< void * (UObjectBaseUtility::*)(UClass *),&UObjectBaseUtility::GetNativeInterfaceAddress >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:768:24
	makeExtern<DAS_CALL_METHOD(_method_394), SimNode_ExtFuncCall >(lib,"GetNativeInterfaceAddress","das_call_member< void * (UObjectBaseUtility::*)(UClass *) , &UObjectBaseUtility::GetNativeInterfaceAddress >::invoke")
		->args({"self","InterfaceClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_395 = das::das_call_member< const void * (UObjectBaseUtility::*)(UClass *) const,&UObjectBaseUtility::GetNativeInterfaceAddress >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:774:14
	makeExtern<DAS_CALL_METHOD(_method_395), SimNode_ExtFuncCall >(lib,"GetNativeInterfaceAddress","das_call_member< const void * (UObjectBaseUtility::*)(UClass *) const , &UObjectBaseUtility::GetNativeInterfaceAddress >::invoke")
		->args({"self","InterfaceClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_396 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:784:23
	makeExtern<DAS_CALL_METHOD(_method_396), SimNode_ExtFuncCall >(lib,"IsDefaultSubobject","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsDefaultSubobject >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_397 = das::das_call_member< int (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetLinkerIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:804:24
	makeExtern<DAS_CALL_METHOD(_method_397), SimNode_ExtFuncCall >(lib,"GetLinkerIndex","das_call_member< int (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetLinkerIndex >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_398 = das::das_call_member< int (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetLinkerUE4Version >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:818:15
	makeExtern<DAS_CALL_METHOD(_method_398), SimNode_ExtFuncCall >(lib,"GetLinkerUE4Version","das_call_member< int (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetLinkerUE4Version >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_399 = das::das_call_member< int (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetLinkerLicenseeUEVersion >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:835:24
	makeExtern<DAS_CALL_METHOD(_method_399), SimNode_ExtFuncCall >(lib,"GetLinkerLicenseeUEVersion","das_call_member< int (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetLinkerLicenseeUEVersion >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_400 = das::das_call_member< int (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetLinkerLicenseeUE4Version >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:838:15
	makeExtern<DAS_CALL_METHOD(_method_400), SimNode_ExtFuncCall >(lib,"GetLinkerLicenseeUE4Version","das_call_member< int (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetLinkerLicenseeUE4Version >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_401 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::ResetStatID >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:868:19
	makeExtern<DAS_CALL_METHOD(_method_401), SimNode_ExtFuncCall >(lib,"ResetStatID","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::ResetStatID >::invoke")
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
	makeExtern< UClass * (*)(const UObject *) , _dasUnreal_static_130_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_130_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UObject*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:95:22
	makeExtern< const wchar_t * (*)(const UObject *) , _dasUnreal_static_131_StaticConfigName , SimNode_ExtFuncCall >(lib,"StaticConfigName","_dasUnreal_static_131_StaticConfigName")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UObject*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:99:14
	makeExtern< void (*)(const UObject *) , _dasUnreal_static_132_StaticRegisterNativesUObject , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUObject","_dasUnreal_static_132_StaticRegisterNativesUObject")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UObject*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_405 = das::das_call_member< UObject * (UObject::*)(FName,UClass *,UClass *,bool,bool),&UObject::CreateDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:120:27
	makeExtern<DAS_CALL_METHOD(_method_405), SimNode_ExtFuncCall >(lib,"CreateDefaultSubobject","das_call_member< UObject * (UObject::*)(FName,UClass *,UClass *,bool,bool) , &UObject::CreateDefaultSubobject >::invoke")
		->args({"self","SubobjectFName","ReturnType","ClassToCreateByDefault","bIsRequired","bIsTransient"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_406 = das::das_call_member< UObject * (UObject::*)(FName),&UObject::GetDefaultSubobjectByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:199:27
	makeExtern<DAS_CALL_METHOD(_method_406), SimNode_ExtFuncCall >(lib,"GetDefaultSubobjectByName","das_call_member< UObject * (UObject::*)(FName) , &UObject::GetDefaultSubobjectByName >::invoke")
		->args({"self","ToFind"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:217:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_133_PostInitProperties , SimNode_ExtFuncCall >(lib,"PostInitProperties","_dasUnreal_virtual_133_PostInitProperties")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:223:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_134_PostReinitProperties , SimNode_ExtFuncCall >(lib,"PostReinitProperties","_dasUnreal_virtual_134_PostReinitProperties")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:229:15
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_135_PostCDOContruct , SimNode_ExtFuncCall >(lib,"PostCDOContruct","_dasUnreal_virtual_135_PostCDOContruct")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:256:31
	makeExtern< bool (*)(UObject &,const wchar_t *) , _dasUnreal_virtual_136_PreSaveRoot , SimNode_ExtFuncCall >(lib,"PreSaveRoot","_dasUnreal_virtual_136_PreSaveRoot")
		->args({"self","Filename"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:268:31
	makeExtern< void (*)(UObject &,bool) , _dasUnreal_virtual_137_PostSaveRoot , SimNode_ExtFuncCall >(lib,"PostSaveRoot","_dasUnreal_virtual_137_PostSaveRoot")
		->args({"self","bCleanupIsRequired"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:299:31
	makeExtern< bool (*)(UObject &,bool) , _dasUnreal_virtual_138_Modify , SimNode_ExtFuncCall >(lib,"Modify","_dasUnreal_virtual_138_Modify")
		->args({"self","bAlwaysMarkDirty"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:302:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_139_IsCapturingAsRootObjectForTransaction , SimNode_ExtFuncCall >(lib,"IsCapturingAsRootObjectForTransaction","_dasUnreal_virtual_139_IsCapturingAsRootObjectForTransaction")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_414 = das::das_call_member< bool (UObject::*)() const,&UObject::CanModify >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:305:23
	makeExtern<DAS_CALL_METHOD(_method_414), SimNode_ExtFuncCall >(lib,"CanModify","das_call_member< bool (UObject::*)() const , &UObject::CanModify >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:315:15
	makeExtern< void (*)(UObject &,const FName &) , _dasUnreal_virtual_140_LoadedFromAnotherClass , SimNode_ExtFuncCall >(lib,"LoadedFromAnotherClass","_dasUnreal_virtual_140_LoadedFromAnotherClass")
		->args({"self","OldClassName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:330:15
	makeExtern< bool (*)(UObject &,const wchar_t *,UObject *&,bool) , _dasUnreal_virtual_141_ResolveSubobject , SimNode_ExtFuncCall >(lib,"ResolveSubobject","_dasUnreal_virtual_141_ResolveSubobject")
		->args({"self","SubObjectPath","OutObject","bLoadIfExists"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:335:15
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_142_IsReadyForAsyncPostLoad , SimNode_ExtFuncCall >(lib,"IsReadyForAsyncPostLoad","_dasUnreal_virtual_142_IsReadyForAsyncPostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:341:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_143_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_143_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:356:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_144_BeginDestroy , SimNode_ExtFuncCall >(lib,"BeginDestroy","_dasUnreal_virtual_144_BeginDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:363:15
	makeExtern< bool (*)(UObject &) , _dasUnreal_virtual_145_IsReadyForFinishDestroy , SimNode_ExtFuncCall >(lib,"IsReadyForFinishDestroy","_dasUnreal_virtual_145_IsReadyForFinishDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:369:15
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_146_PostLinkerChange , SimNode_ExtFuncCall >(lib,"PostLinkerChange","_dasUnreal_virtual_146_PostLinkerChange")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:377:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_147_FinishDestroy , SimNode_ExtFuncCall >(lib,"FinishDestroy","_dasUnreal_virtual_147_FinishDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:383:31
	makeExtern< void (*)(UObject &,FArchive &) , _dasUnreal_virtual_148_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_148_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:410:15
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_149_ShutdownAfterError , SimNode_ExtFuncCall >(lib,"ShutdownAfterError","_dasUnreal_virtual_149_ShutdownAfterError")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:413:15
	makeExtern< void (*)(UObject &,FProperty *) , _dasUnreal_virtual_150_PostInterpChange , SimNode_ExtFuncCall >(lib,"PostInterpChange","_dasUnreal_virtual_150_PostInterpChange")
		->args({"self","PropertyThatChanged"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_426 = das::das_call_member< void (UObject::*)(),&UObject::PostEditChange >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:456:23
	makeExtern<DAS_CALL_METHOD(_method_426), SimNode_ExtFuncCall >(lib,"PostEditChange","das_call_member< void (UObject::*)() , &UObject::PostEditChange >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_427 = das::das_call_member< void (UObject::*)(EEditChangePropagationFlags),&UObject::SetEditChangePropagationFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:472:23
	makeExtern<DAS_CALL_METHOD(_method_427), SimNode_ExtFuncCall >(lib,"SetEditChangePropagationFlags","das_call_member< void (UObject::*)(EEditChangePropagationFlags) , &UObject::SetEditChangePropagationFlags >::invoke")
		->args({"self","InFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:475:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_151_PreEditUndo , SimNode_ExtFuncCall >(lib,"PreEditUndo","_dasUnreal_virtual_151_PreEditUndo")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:513:15
	makeExtern< void (*)(UObject &,UObject *,const FName) , _dasUnreal_virtual_152_PostRename , SimNode_ExtFuncCall >(lib,"PostRename","_dasUnreal_virtual_152_PostRename")
		->args({"self","OldOuter","OldName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:528:15
	makeExtern< void (*)(UObject &,bool) , _dasUnreal_virtual_153_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_153_PostDuplicate")
		->args({"self","bDuplicateForPIE"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:529:15
	makeExtern< void (*)(UObject &,EDuplicateMode::Type) , _dasUnreal_virtual_154_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_154_PostDuplicate")
		->args({"self","DuplicateMode"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:540:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_155_NeedsLoadForClient , SimNode_ExtFuncCall >(lib,"NeedsLoadForClient","_dasUnreal_virtual_155_NeedsLoadForClient")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

