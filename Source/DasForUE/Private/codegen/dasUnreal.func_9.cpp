// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_9() {
	addCtorAndUsing<FSavePackageResultStruct,ESavePackageResult>(*this,lib,"FSavePackageResultStruct","FSavePackageResultStruct")
		->args({"InResult"});
	addCtorAndUsing<FSavePackageResultStruct,ESavePackageResult,long long>(*this,lib,"FSavePackageResultStruct","FSavePackageResultStruct")
		->args({"InResult","InTotalFileSize"});
	using _method_644 = das::das_call_member< bool (FSavePackageResultStruct::*)() const,&FSavePackageResultStruct::IsSuccessful >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:129:7
	makeExtern<DAS_CALL_METHOD(_method_644), SimNode_ExtFuncCall >(lib,"IsSuccessful","das_call_member< bool (FSavePackageResultStruct::*)() const , &FSavePackageResultStruct::IsSuccessful >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:144:2
	makeExtern< UClass * (*)(const UPackage *) , _dasUnreal_static_213_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_213_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:144:2
	makeExtern< const wchar_t * (*)(const UPackage *) , _dasUnreal_static_214_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_214_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:144:2
	makeExtern< void (*)(const UPackage *) , _dasUnreal_static_215_StaticRegisterNativesUPackage , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUPackage","_dasUnreal_static_215_StaticRegisterNativesUPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_648 = das::das_call_member< bool (UPackage::*)() const,&UPackage::GetHasBeenEndLoaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:231:7
	makeExtern<DAS_CALL_METHOD(_method_648), SimNode_ExtFuncCall >(lib,"GetHasBeenEndLoaded","das_call_member< bool (UPackage::*)() const , &UPackage::GetHasBeenEndLoaded >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_649 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetHasBeenEndLoaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:235:7
	makeExtern<DAS_CALL_METHOD(_method_649), SimNode_ExtFuncCall >(lib,"SetHasBeenEndLoaded","das_call_member< void (UPackage::*)(bool) , &UPackage::SetHasBeenEndLoaded >::invoke")
		->args({"self","bValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:328:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_216_IsNameStableForNetworking , SimNode_ExtFuncCall >(lib,"IsNameStableForNetworking","_dasUnreal_virtual_216_IsNameStableForNetworking")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:329:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_217_NeedsLoadForClient , SimNode_ExtFuncCall >(lib,"NeedsLoadForClient","_dasUnreal_virtual_217_NeedsLoadForClient")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:330:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_218_NeedsLoadForServer , SimNode_ExtFuncCall >(lib,"NeedsLoadForServer","_dasUnreal_virtual_218_NeedsLoadForServer")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:331:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_219_IsPostLoadThreadSafe , SimNode_ExtFuncCall >(lib,"IsPostLoadThreadSafe","_dasUnreal_virtual_219_IsPostLoadThreadSafe")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:332:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_220_IsDestructionThreadSafe , SimNode_ExtFuncCall >(lib,"IsDestructionThreadSafe","_dasUnreal_virtual_220_IsDestructionThreadSafe")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_655 = das::das_call_member< void (UPackage::*)(bool,bool),&UPackage::SetLoadedByEditorPropertiesOnly >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:336:7
	makeExtern<DAS_CALL_METHOD(_method_655), SimNode_ExtFuncCall >(lib,"SetLoadedByEditorPropertiesOnly","das_call_member< void (UPackage::*)(bool,bool) , &UPackage::SetLoadedByEditorPropertiesOnly >::invoke")
		->args({"self","bIsEditorOnly","bRecursive"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_656 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsLoadedByEditorPropertiesOnly >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:338:7
	makeExtern<DAS_CALL_METHOD(_method_656), SimNode_ExtFuncCall >(lib,"IsLoadedByEditorPropertiesOnly","das_call_member< bool (UPackage::*)() const , &UPackage::IsLoadedByEditorPropertiesOnly >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_657 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetDynamicPIEPackagePending >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:341:7
	makeExtern<DAS_CALL_METHOD(_method_657), SimNode_ExtFuncCall >(lib,"SetDynamicPIEPackagePending","das_call_member< void (UPackage::*)(bool) , &UPackage::SetDynamicPIEPackagePending >::invoke")
		->args({"self","bInIsDynamicPIEPackagePending"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_658 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsDynamicPIEPackagePending >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:343:7
	makeExtern<DAS_CALL_METHOD(_method_658), SimNode_ExtFuncCall >(lib,"IsDynamicPIEPackagePending","das_call_member< bool (UPackage::*)() const , &UPackage::IsDynamicPIEPackagePending >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:350:15
	makeExtern< void (*)(UPackage &) , _dasUnreal_virtual_221_PostInitProperties , SimNode_ExtFuncCall >(lib,"PostInitProperties","_dasUnreal_virtual_221_PostInitProperties")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:352:15
	makeExtern< void (*)(UPackage &) , _dasUnreal_virtual_222_FinishDestroy , SimNode_ExtFuncCall >(lib,"FinishDestroy","_dasUnreal_virtual_222_FinishDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:358:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_223_IsAsset , SimNode_ExtFuncCall >(lib,"IsAsset","_dasUnreal_virtual_223_IsAsset")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_662 = das::das_call_member< int (UPackage::*)() const,&UPackage::GetPIEInstanceID >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:417:8
	makeExtern<DAS_CALL_METHOD(_method_662), SimNode_ExtFuncCall >(lib,"GetPIEInstanceID","das_call_member< int (UPackage::*)() const , &UPackage::GetPIEInstanceID >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_663 = das::das_call_member< void (UPackage::*)(int),&UPackage::SetPIEInstanceID >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:430:7
	makeExtern<DAS_CALL_METHOD(_method_663), SimNode_ExtFuncCall >(lib,"SetPIEInstanceID","das_call_member< void (UPackage::*)(int) , &UPackage::SetPIEInstanceID >::invoke")
		->args({"self","InPIEInstanceID"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_664 = das::das_call_member< int (UPackage::*)() const,&UPackage::GetLinkerLicenseeVersion >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:447:8
	makeExtern<DAS_CALL_METHOD(_method_664), SimNode_ExtFuncCall >(lib,"GetLinkerLicenseeVersion","das_call_member< int (UPackage::*)() const , &UPackage::GetLinkerLicenseeVersion >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_665 = das::das_call_member< void (UPackage::*)(float),&UPackage::SetLoadTime >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:461:7
	makeExtern<DAS_CALL_METHOD(_method_665), SimNode_ExtFuncCall >(lib,"SetLoadTime","das_call_member< void (UPackage::*)(float) , &UPackage::SetLoadTime >::invoke")
		->args({"self","InLoadTime"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_666 = das::das_call_member< float (UPackage::*)() const,&UPackage::GetLoadTime >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:473:8
	makeExtern<DAS_CALL_METHOD(_method_666), SimNode_ExtFuncCall >(lib,"GetLoadTime","das_call_member< float (UPackage::*)() const , &UPackage::GetLoadTime >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_667 = das::das_call_member< FName (UPackage::*)() const,&UPackage::GetFolderName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:488:8
	makeExtern<DAS_CALL_METHOD(_method_667), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFolderName","das_call_member< FName (UPackage::*)() const , &UPackage::GetFolderName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_668 = das::das_call_member< void (UPackage::*)(FName),&UPackage::SetFolderName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:497:7
	makeExtern<DAS_CALL_METHOD(_method_668), SimNode_ExtFuncCall >(lib,"SetFolderName","das_call_member< void (UPackage::*)(FName) , &UPackage::SetFolderName >::invoke")
		->args({"self","name"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_669 = das::das_call_member< void (UPackage::*)(),&UPackage::ClearDirtyFlag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:506:7
	makeExtern<DAS_CALL_METHOD(_method_669), SimNode_ExtFuncCall >(lib,"ClearDirtyFlag","das_call_member< void (UPackage::*)() , &UPackage::ClearDirtyFlag >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_670 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetDirtyFlag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:514:7
	makeExtern<DAS_CALL_METHOD(_method_670), SimNode_ExtFuncCall >(lib,"SetDirtyFlag","das_call_member< void (UPackage::*)(bool) , &UPackage::SetDirtyFlag >::invoke")
		->args({"self","bIsDirty"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_671 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsDirty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:521:7
	makeExtern<DAS_CALL_METHOD(_method_671), SimNode_ExtFuncCall >(lib,"IsDirty","das_call_member< bool (UPackage::*)() const , &UPackage::IsDirty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_672 = das::das_call_member< void (UPackage::*)(),&UPackage::MarkAsNewlyCreated >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:530:7
	makeExtern<DAS_CALL_METHOD(_method_672), SimNode_ExtFuncCall >(lib,"MarkAsNewlyCreated","das_call_member< void (UPackage::*)() , &UPackage::MarkAsNewlyCreated >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_673 = das::das_call_member< void (UPackage::*)(),&UPackage::MarkAsUnloaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:540:7
	makeExtern<DAS_CALL_METHOD(_method_673), SimNode_ExtFuncCall >(lib,"MarkAsUnloaded","das_call_member< void (UPackage::*)() , &UPackage::MarkAsUnloaded >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_674 = das::das_call_member< void (UPackage::*)(),&UPackage::MarkAsFullyLoaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:550:7
	makeExtern<DAS_CALL_METHOD(_method_674), SimNode_ExtFuncCall >(lib,"MarkAsFullyLoaded","das_call_member< void (UPackage::*)() , &UPackage::MarkAsFullyLoaded >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_675 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsFullyLoaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:560:7
	makeExtern<DAS_CALL_METHOD(_method_675), SimNode_ExtFuncCall >(lib,"IsFullyLoaded","das_call_member< bool (UPackage::*)() const , &UPackage::IsFullyLoaded >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_676 = das::das_call_member< void (UPackage::*)(),&UPackage::FullyLoad >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:565:7
	makeExtern<DAS_CALL_METHOD(_method_676), SimNode_ExtFuncCall >(lib,"FullyLoad","das_call_member< void (UPackage::*)() , &UPackage::FullyLoad >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_677 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetCanBeImportedFlag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:580:7
	makeExtern<DAS_CALL_METHOD(_method_677), SimNode_ExtFuncCall >(lib,"SetCanBeImportedFlag","das_call_member< void (UPackage::*)(bool) , &UPackage::SetCanBeImportedFlag >::invoke")
		->args({"self","bInCanBeImported"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_678 = das::das_call_member< bool (UPackage::*)() const,&UPackage::CanBeImported >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:590:7
	makeExtern<DAS_CALL_METHOD(_method_678), SimNode_ExtFuncCall >(lib,"CanBeImported","das_call_member< bool (UPackage::*)() const , &UPackage::CanBeImported >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:597:15
	makeExtern< void (*)(UPackage &,EObjectFlags) , _dasUnreal_virtual_224_TagSubobjects , SimNode_ExtFuncCall >(lib,"TagSubobjects","_dasUnreal_virtual_224_TagSubobjects")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_680 = das::das_call_member< void (UPackage::*)(),&UPackage::ThisContainsMap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:602:7
	makeExtern<DAS_CALL_METHOD(_method_680), SimNode_ExtFuncCall >(lib,"ThisContainsMap","das_call_member< void (UPackage::*)() , &UPackage::ThisContainsMap >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_681 = das::das_call_member< bool (UPackage::*)() const,&UPackage::ContainsMap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:612:7
	makeExtern<DAS_CALL_METHOD(_method_681), SimNode_ExtFuncCall >(lib,"ContainsMap","das_call_member< bool (UPackage::*)() const , &UPackage::ContainsMap >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_682 = das::das_call_member< void (UPackage::*)(bool),&UPackage::ThisRequiresLocalizationGather >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:620:7
	makeExtern<DAS_CALL_METHOD(_method_682), SimNode_ExtFuncCall >(lib,"ThisRequiresLocalizationGather","das_call_member< void (UPackage::*)(bool) , &UPackage::ThisRequiresLocalizationGather >::invoke")
		->args({"self","Value"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_683 = das::das_call_member< bool (UPackage::*)() const,&UPackage::RequiresLocalizationGather >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:637:7
	makeExtern<DAS_CALL_METHOD(_method_683), SimNode_ExtFuncCall >(lib,"RequiresLocalizationGather","das_call_member< bool (UPackage::*)() const , &UPackage::RequiresLocalizationGather >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_684 = das::das_call_member< void (UPackage::*)(unsigned int),&UPackage::SetPackageFlagsTo >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:645:7
	makeExtern<DAS_CALL_METHOD(_method_684), SimNode_ExtFuncCall >(lib,"SetPackageFlagsTo","das_call_member< void (UPackage::*)(unsigned int) , &UPackage::SetPackageFlagsTo >::invoke")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_685 = das::das_call_member< void (UPackage::*)(unsigned int),&UPackage::SetPackageFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:663:19
	makeExtern<DAS_CALL_METHOD(_method_685), SimNode_ExtFuncCall >(lib,"SetPackageFlags","das_call_member< void (UPackage::*)(unsigned int) , &UPackage::SetPackageFlags >::invoke")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_686 = das::das_call_member< void (UPackage::*)(unsigned int),&UPackage::ClearPackageFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:673:19
	makeExtern<DAS_CALL_METHOD(_method_686), SimNode_ExtFuncCall >(lib,"ClearPackageFlags","das_call_member< void (UPackage::*)(unsigned int) , &UPackage::ClearPackageFlags >::invoke")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_687 = das::das_call_member< bool (UPackage::*)(unsigned int) const,&UPackage::HasAnyPackageFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:686:19
	makeExtern<DAS_CALL_METHOD(_method_687), SimNode_ExtFuncCall >(lib,"HasAnyPackageFlags","das_call_member< bool (UPackage::*)(unsigned int) const , &UPackage::HasAnyPackageFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_688 = das::das_call_member< bool (UPackage::*)(unsigned int) const,&UPackage::HasAllPackagesFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:697:19
	makeExtern<DAS_CALL_METHOD(_method_688), SimNode_ExtFuncCall >(lib,"HasAllPackagesFlags","das_call_member< bool (UPackage::*)(unsigned int) const , &UPackage::HasAllPackagesFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_689 = das::das_call_member< unsigned int (UPackage::*)() const,&UPackage::GetPackageFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:707:21
	makeExtern<DAS_CALL_METHOD(_method_689), SimNode_ExtFuncCall >(lib,"GetPackageFlags","das_call_member< unsigned int (UPackage::*)() const , &UPackage::GetPackageFlags >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_690 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsExternallyReferenceable >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:715:19
	makeExtern<DAS_CALL_METHOD(_method_690), SimNode_ExtFuncCall >(lib,"IsExternallyReferenceable","das_call_member< bool (UPackage::*)() const , &UPackage::IsExternallyReferenceable >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_691 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetIsExternallyReferenceable >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:725:19
	makeExtern<DAS_CALL_METHOD(_method_691), SimNode_ExtFuncCall >(lib,"SetIsExternallyReferenceable","das_call_member< void (UPackage::*)(bool) , &UPackage::SetIsExternallyReferenceable >::invoke")
		->args({"self","bValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_692 = das::das_call_member< bool (UPackage::*)() const,&UPackage::HasThumbnailMap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:739:7
	makeExtern<DAS_CALL_METHOD(_method_692), SimNode_ExtFuncCall >(lib,"HasThumbnailMap","das_call_member< bool (UPackage::*)() const , &UPackage::HasThumbnailMap >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_693 = das::das_call_member< int64 (UPackage::*)() const,&UPackage::GetFileSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:832:20
	makeExtern<DAS_CALL_METHOD(_method_693), SimNode_ExtFuncCall >(lib,"GetFileSize","das_call_member< int64 (UPackage::*)() const , &UPackage::GetFileSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_694 = das::das_call_member< UObject * (UPackage::*)(EObjectFlags) const,&UPackage::FindAssetInPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:878:11
	makeExtern<DAS_CALL_METHOD(_method_694), SimNode_ExtFuncCall >(lib,"FindAssetInPackage","das_call_member< UObject * (UPackage::*)(EObjectFlags) const , &UPackage::FindAssetInPackage >::invoke")
		->args({"self","RequiredTopLevelFlags"})
		->arg_init(1,make_smart<ExprConstEnumeration>(0,makeType<EObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_695 = das::das_call_member< bool (UPackage::*)() const,&UPackage::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:893:7
	makeExtern<DAS_CALL_METHOD(_method_695), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (UPackage::*)() const , &UPackage::HasMetaData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_696 = das::das_call_member< UMetaData * (UPackage::*)(),&UPackage::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:909:19
	makeExtern<DAS_CALL_METHOD(_method_696), SimNode_ExtFuncCall >(lib,"GetMetaData","das_call_member< UMetaData * (UPackage::*)() , &UPackage::GetMetaData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:950:14
	makeExtern< void (*)(const UPackage *) , _dasUnreal_static_225_WaitForAsyncFileWrites , SimNode_ExtFuncCall >(lib,"WaitForAsyncFileWrites","_dasUnreal_static_225_WaitForAsyncFileWrites")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:953:14
	makeExtern< bool (*)(const UPackage *) , _dasUnreal_static_226_HasAsyncFileWrites , SimNode_ExtFuncCall >(lib,"HasAsyncFileWrites","_dasUnreal_static_226_HasAsyncFileWrites")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:962:14
	makeExtern< bool (*)(const UPackage *,UPackage *,const UObject *) , _dasUnreal_static_227_IsEmptyPackage , SimNode_ExtFuncCall >(lib,"IsEmptyPackage","_dasUnreal_static_227_IsEmptyPackage")
		->args({"self","Package","LastReferencer"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->arg_init(2,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:155:2
	makeExtern< FFieldClass * (*)(const FProperty *) , _dasUnreal_static_228_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_228_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:155:2
	makeExtern< uint64 (*)(const FProperty *) , _dasUnreal_static_229_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_229_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:155:2
	makeExtern< uint64 (*)(const FProperty *) , _dasUnreal_static_230_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_230_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:211:31
	makeExtern< void (*)(FProperty &,const FField &) , _dasUnreal_virtual_231_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_231_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:228:31
	makeExtern< FName (*)(const FProperty *,UStruct *,FName) , _dasUnreal_static_232_FindRedirectedPropertyName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FindRedirectedPropertyName","_dasUnreal_static_232_FindRedirectedPropertyName")
		->args({"self","ObjectStruct","OldName"})
		->arg_type(0,makeType<TExplicit<const FProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:233:34
	makeExtern< FString (*)(const FProperty &,FString &) , _dasUnreal_virtual_233_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_233_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:234:31
	makeExtern< bool (*)(const FProperty &) , _dasUnreal_virtual_234_PassCPPArgsByRef , SimNode_ExtFuncCall >(lib,"PassCPPArgsByRef","_dasUnreal_virtual_234_PassCPPArgsByRef")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_707 = das::das_call_member< FString (FProperty::*)() const,&FProperty::GetNameCPP >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:242:26
	makeExtern<DAS_CALL_METHOD(_method_707), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameCPP","das_call_member< FString (FProperty::*)() const , &FProperty::GetNameCPP >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:250:34
	makeExtern< FString (*)(const FProperty &,FString *,unsigned int) , _dasUnreal_virtual_235_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_235_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->arg_init(2,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:252:34
	makeExtern< FString (*)(const FProperty &) , _dasUnreal_virtual_236_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_236_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_710 = das::das_call_member< UPropertyWrapper * (FProperty::*)(),&FProperty::GetUPropertyWrapper >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:257:36
	makeExtern<DAS_CALL_METHOD(_method_710), SimNode_ExtFuncCall >(lib,"GetUPropertyWrapper","das_call_member< UPropertyWrapper * (FProperty::*)() , &FProperty::GetUPropertyWrapper >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:261:15
	makeExtern< bool (*)(const FProperty &) , _dasUnreal_virtual_237_HasSetter , SimNode_ExtFuncCall >(lib,"HasSetter","_dasUnreal_virtual_237_HasSetter")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:266:15
	makeExtern< bool (*)(const FProperty &) , _dasUnreal_virtual_238_HasGetter , SimNode_ExtFuncCall >(lib,"HasGetter","_dasUnreal_virtual_238_HasGetter")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:271:15
	makeExtern< bool (*)(const FProperty &) , _dasUnreal_virtual_239_HasSetterOrGetter , SimNode_ExtFuncCall >(lib,"HasSetterOrGetter","_dasUnreal_virtual_239_HasSetterOrGetter")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:280:15
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_240_CallSetter , SimNode_ExtFuncCall >(lib,"CallSetter","_dasUnreal_virtual_240_CallSetter")
		->args({"self","Container","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:289:15
	makeExtern< void (*)(const FProperty &,const void *,void *) , _dasUnreal_virtual_241_CallGetter , SimNode_ExtFuncCall >(lib,"CallGetter","_dasUnreal_virtual_241_CallGetter")
		->args({"self","Container","OutValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_716 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetOffset_ForDebug >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:314:20
	makeExtern<DAS_CALL_METHOD(_method_716), SimNode_ExtFuncCall >(lib,"GetOffset_ForDebug","das_call_member< int (FProperty::*)() const , &FProperty::GetOffset_ForDebug >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_717 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetOffset_ForUFunction >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:319:20
	makeExtern<DAS_CALL_METHOD(_method_717), SimNode_ExtFuncCall >(lib,"GetOffset_ForUFunction","das_call_member< int (FProperty::*)() const , &FProperty::GetOffset_ForUFunction >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_718 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetOffset_ForGC >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:324:20
	makeExtern<DAS_CALL_METHOD(_method_718), SimNode_ExtFuncCall >(lib,"GetOffset_ForGC","das_call_member< int (FProperty::*)() const , &FProperty::GetOffset_ForGC >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_719 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetOffset_ForInternal >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:329:20
	makeExtern<DAS_CALL_METHOD(_method_719), SimNode_ExtFuncCall >(lib,"GetOffset_ForInternal","das_call_member< int (FProperty::*)() const , &FProperty::GetOffset_ForInternal >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_720 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetOffset_ReplaceWith_ContainerPtrToValuePtr >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:334:20
	makeExtern<DAS_CALL_METHOD(_method_720), SimNode_ExtFuncCall >(lib,"GetOffset_ReplaceWith_ContainerPtrToValuePtr","das_call_member< int (FProperty::*)() const , &FProperty::GetOffset_ReplaceWith_ContainerPtrToValuePtr >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:379:31
	makeExtern< bool (*)(const FProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_242_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_242_Identical")
		->args({"self","A","B","PortFlags"})
		->arg_init(3,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_722 = das::das_call_member< bool (FProperty::*)(const void *,const void *,int,unsigned int) const,&FProperty::Identical_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:390:7
	makeExtern<DAS_CALL_METHOD(_method_722), SimNode_ExtFuncCall >(lib,"Identical_InContainer","das_call_member< bool (FProperty::*)(const void *,const void *,int,unsigned int) const , &FProperty::Identical_InContainer >::invoke")
		->args({"self","A","B","ArrayIndex","PortFlags"})
		->arg_init(3,make_smart<ExprConstInt>(0))
		->arg_init(4,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:447:31
	makeExtern< bool (*)(const FProperty &) , _dasUnreal_virtual_243_SupportsNetSharedSerialization , SimNode_ExtFuncCall >(lib,"SupportsNetSharedSerialization","_dasUnreal_virtual_243_SupportsNetSharedSerialization")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:450:15
	makeExtern< void (*)(const FProperty &,FString &,const void *,const void *,UObject *,int,UObject *) , _dasUnreal_virtual_244_ExportTextItem , SimNode_ExtFuncCall >(lib,"ExportTextItem","_dasUnreal_virtual_244_ExportTextItem")
		->args({"self","ValueStr","PropertyValue","DefaultValue","Parent","PortFlags","ExportRootScope"})
		->arg_init(6,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_725 = das::das_call_member< void (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const,&FProperty::ExportTextItem_Direct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:455:7
	makeExtern<DAS_CALL_METHOD(_method_725), SimNode_ExtFuncCall >(lib,"ExportTextItem_Direct","das_call_member< void (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const , &FProperty::ExportTextItem_Direct >::invoke")
		->args({"self","ValueStr","PropertyValue","DefaultValue","Parent","PortFlags","ExportRootScope"})
		->arg_init(6,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_726 = das::das_call_member< void (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const,&FProperty::ExportTextItem_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:460:7
	makeExtern<DAS_CALL_METHOD(_method_726), SimNode_ExtFuncCall >(lib,"ExportTextItem_InContainer","das_call_member< void (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const , &FProperty::ExportTextItem_InContainer >::invoke")
		->args({"self","ValueStr","Container","DefaultValue","Parent","PortFlags","ExportRootScope"})
		->arg_init(6,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_727 = das::das_call_member< void (FProperty::*)(void *,const void *) const,&FProperty::SetValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:509:19
	makeExtern<DAS_CALL_METHOD(_method_727), SimNode_ExtFuncCall >(lib,"SetValue_InContainer","das_call_member< void (FProperty::*)(void *,const void *) const , &FProperty::SetValue_InContainer >::invoke")
		->args({"self","OutContainer","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_728 = das::das_call_member< void (FProperty::*)(const void *,void *) const,&FProperty::GetValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:520:19
	makeExtern<DAS_CALL_METHOD(_method_728), SimNode_ExtFuncCall >(lib,"GetValue_InContainer","das_call_member< void (FProperty::*)(const void *,void *) const , &FProperty::GetValue_InContainer >::invoke")
		->args({"self","InContainer","OutValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_729 = das::das_call_member< void (FProperty::*)(void *,const void *,int) const,&FProperty::SetSingleValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:538:23
	makeExtern<DAS_CALL_METHOD(_method_729), SimNode_ExtFuncCall >(lib,"SetSingleValue_InContainer","das_call_member< void (FProperty::*)(void *,const void *,int) const , &FProperty::SetSingleValue_InContainer >::invoke")
		->args({"self","OutContainer","InValue","ArrayIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_730 = das::das_call_member< void (FProperty::*)(const void *,void *,int) const,&FProperty::GetSingleValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:546:23
	makeExtern<DAS_CALL_METHOD(_method_730), SimNode_ExtFuncCall >(lib,"GetSingleValue_InContainer","das_call_member< void (FProperty::*)(const void *,void *,int) const , &FProperty::GetSingleValue_InContainer >::invoke")
		->args({"self","InContainer","OutValue","ArrayIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_731 = das::das_call_member< void * (FProperty::*)() const,&FProperty::AllocateAndInitializeValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:549:24
	makeExtern<DAS_CALL_METHOD(_method_731), SimNode_ExtFuncCall >(lib,"AllocateAndInitializeValue","das_call_member< void * (FProperty::*)() const , &FProperty::AllocateAndInitializeValue >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_732 = das::das_call_member< void (FProperty::*)(void *) const,&FProperty::DestroyAndFreeValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:552:23
	makeExtern<DAS_CALL_METHOD(_method_732), SimNode_ExtFuncCall >(lib,"DestroyAndFreeValue","das_call_member< void (FProperty::*)(void *) const , &FProperty::DestroyAndFreeValue >::invoke")
		->args({"self","InMemory"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:576:32
	makeExtern< void * (*)(const FProperty &,const FProperty *,void *,int) , _dasUnreal_virtual_245_GetValueAddressAtIndex_Direct , SimNode_ExtFuncCall >(lib,"GetValueAddressAtIndex_Direct","_dasUnreal_virtual_245_GetValueAddressAtIndex_Direct")
		->args({"self","Inner","InValueAddress","Index"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:590:31
	makeExtern< void (*)(const FProperty &,FBlake3 &,bool) , _dasUnreal_virtual_246_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_246_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_735 = das::das_call_member< bool (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const,&FProperty::ExportText_Direct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:599:23
	makeExtern<DAS_CALL_METHOD(_method_735), SimNode_ExtFuncCall >(lib,"ExportText_Direct","das_call_member< bool (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const , &FProperty::ExportText_Direct >::invoke")
		->args({"self","ValueStr","Data","Delta","Parent","PortFlags","ExportRootScope"})
		->arg_init(6,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_736 = das::das_call_member< bool (FProperty::*)(int,FString &,const void *,const void *,UObject *,int,UObject *) const,&FProperty::ExportText_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:600:19
	makeExtern<DAS_CALL_METHOD(_method_736), SimNode_ExtFuncCall >(lib,"ExportText_InContainer","das_call_member< bool (FProperty::*)(int,FString &,const void *,const void *,UObject *,int,UObject *) const , &FProperty::ExportText_InContainer >::invoke")
		->args({"self","Index","ValueStr","Data","Delta","Parent","PortFlags","ExportRootScope"})
		->arg_init(7,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_737 = das::das_call_member< bool (FProperty::*)(int) const,&FProperty::IsInContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:731:19
	makeExtern<DAS_CALL_METHOD(_method_737), SimNode_ExtFuncCall >(lib,"IsInContainer","das_call_member< bool (FProperty::*)(int) const , &FProperty::IsInContainer >::invoke")
		->args({"self","ContainerSize"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_738 = das::das_call_member< bool (FProperty::*)(UStruct *) const,&FProperty::IsInContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:736:19
	makeExtern<DAS_CALL_METHOD(_method_738), SimNode_ExtFuncCall >(lib,"IsInContainer","das_call_member< bool (FProperty::*)(UStruct *) const , &FProperty::IsInContainer >::invoke")
		->args({"self","ContainerClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_739 = das::das_call_member< void (FProperty::*)(void *,const void *) const,&FProperty::CopySingleValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:752:19
	makeExtern<DAS_CALL_METHOD(_method_739), SimNode_ExtFuncCall >(lib,"CopySingleValue","das_call_member< void (FProperty::*)(void *,const void *) const , &FProperty::CopySingleValue >::invoke")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_740 = das::das_call_member< unsigned int (FProperty::*)(const void *) const,&FProperty::GetValueTypeHash >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:770:25
	makeExtern<DAS_CALL_METHOD(_method_740), SimNode_ExtFuncCall >(lib,"GetValueTypeHash","das_call_member< unsigned int (FProperty::*)(const void *) const , &FProperty::GetValueTypeHash >::invoke")
		->args({"self","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_741 = das::das_call_member< void (FProperty::*)(void *,const void *) const,&FProperty::CopyCompleteValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:786:19
	makeExtern<DAS_CALL_METHOD(_method_741), SimNode_ExtFuncCall >(lib,"CopyCompleteValue","das_call_member< void (FProperty::*)(void *,const void *) const , &FProperty::CopyCompleteValue >::invoke")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

