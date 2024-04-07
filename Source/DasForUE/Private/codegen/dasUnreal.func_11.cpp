// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_11() {
	using _method_822 = das::das_call_member< bool (UPackage::*)() const,&UPackage::GetHasBeenEndLoaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:231:7
	makeExtern<DAS_CALL_METHOD(_method_822), SimNode_ExtFuncCall >(lib,"GetHasBeenEndLoaded","das_call_member< bool (UPackage::*)() const , &UPackage::GetHasBeenEndLoaded >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_823 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetHasBeenEndLoaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:235:7
	makeExtern<DAS_CALL_METHOD(_method_823), SimNode_ExtFuncCall >(lib,"SetHasBeenEndLoaded","das_call_member< void (UPackage::*)(bool) , &UPackage::SetHasBeenEndLoaded >::invoke")
		->args({"self","bValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:328:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_300_IsNameStableForNetworking , SimNode_ExtFuncCall >(lib,"IsNameStableForNetworking","_dasUnreal_virtual_300_IsNameStableForNetworking")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:329:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_301_NeedsLoadForClient , SimNode_ExtFuncCall >(lib,"NeedsLoadForClient","_dasUnreal_virtual_301_NeedsLoadForClient")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:330:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_302_NeedsLoadForServer , SimNode_ExtFuncCall >(lib,"NeedsLoadForServer","_dasUnreal_virtual_302_NeedsLoadForServer")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:331:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_303_IsPostLoadThreadSafe , SimNode_ExtFuncCall >(lib,"IsPostLoadThreadSafe","_dasUnreal_virtual_303_IsPostLoadThreadSafe")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:332:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_304_IsDestructionThreadSafe , SimNode_ExtFuncCall >(lib,"IsDestructionThreadSafe","_dasUnreal_virtual_304_IsDestructionThreadSafe")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_829 = das::das_call_member< void (UPackage::*)(bool,bool),&UPackage::SetLoadedByEditorPropertiesOnly >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:336:7
	makeExtern<DAS_CALL_METHOD(_method_829), SimNode_ExtFuncCall >(lib,"SetLoadedByEditorPropertiesOnly","das_call_member< void (UPackage::*)(bool,bool) , &UPackage::SetLoadedByEditorPropertiesOnly >::invoke")
		->args({"self","bIsEditorOnly","bRecursive"})
		->arg_init(2,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_830 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsLoadedByEditorPropertiesOnly >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:338:7
	makeExtern<DAS_CALL_METHOD(_method_830), SimNode_ExtFuncCall >(lib,"IsLoadedByEditorPropertiesOnly","das_call_member< bool (UPackage::*)() const , &UPackage::IsLoadedByEditorPropertiesOnly >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_831 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetDynamicPIEPackagePending >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:341:7
	makeExtern<DAS_CALL_METHOD(_method_831), SimNode_ExtFuncCall >(lib,"SetDynamicPIEPackagePending","das_call_member< void (UPackage::*)(bool) , &UPackage::SetDynamicPIEPackagePending >::invoke")
		->args({"self","bInIsDynamicPIEPackagePending"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_832 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsDynamicPIEPackagePending >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:343:7
	makeExtern<DAS_CALL_METHOD(_method_832), SimNode_ExtFuncCall >(lib,"IsDynamicPIEPackagePending","das_call_member< bool (UPackage::*)() const , &UPackage::IsDynamicPIEPackagePending >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:350:15
	makeExtern< void (*)(UPackage &) , _dasUnreal_virtual_305_PostInitProperties , SimNode_ExtFuncCall >(lib,"PostInitProperties","_dasUnreal_virtual_305_PostInitProperties")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:352:15
	makeExtern< void (*)(UPackage &) , _dasUnreal_virtual_306_FinishDestroy , SimNode_ExtFuncCall >(lib,"FinishDestroy","_dasUnreal_virtual_306_FinishDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:355:15
	makeExtern< void (*)(UPackage &,FArchive &) , _dasUnreal_virtual_307_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_307_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:358:15
	makeExtern< bool (*)(const UPackage &) , _dasUnreal_virtual_308_IsAsset , SimNode_ExtFuncCall >(lib,"IsAsset","_dasUnreal_virtual_308_IsAsset")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:369:14
	makeExtern< void (*)(const UPackage *,UObject *,FReferenceCollector &) , _dasUnreal_static_309_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_static_309_AddReferencedObjects")
		->args({"self","InThis","Collector"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_838 = das::das_call_member< int (UPackage::*)() const,&UPackage::GetPIEInstanceID >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:417:8
	makeExtern<DAS_CALL_METHOD(_method_838), SimNode_ExtFuncCall >(lib,"GetPIEInstanceID","das_call_member< int (UPackage::*)() const , &UPackage::GetPIEInstanceID >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_839 = das::das_call_member< void (UPackage::*)(int),&UPackage::SetPIEInstanceID >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:430:7
	makeExtern<DAS_CALL_METHOD(_method_839), SimNode_ExtFuncCall >(lib,"SetPIEInstanceID","das_call_member< void (UPackage::*)(int) , &UPackage::SetPIEInstanceID >::invoke")
		->args({"self","InPIEInstanceID"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_840 = das::das_call_member< int (UPackage::*)() const,&UPackage::GetLinkerLicenseeVersion >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:447:8
	makeExtern<DAS_CALL_METHOD(_method_840), SimNode_ExtFuncCall >(lib,"GetLinkerLicenseeVersion","das_call_member< int (UPackage::*)() const , &UPackage::GetLinkerLicenseeVersion >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_841 = das::das_call_member< void (UPackage::*)(float),&UPackage::SetLoadTime >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:461:7
	makeExtern<DAS_CALL_METHOD(_method_841), SimNode_ExtFuncCall >(lib,"SetLoadTime","das_call_member< void (UPackage::*)(float) , &UPackage::SetLoadTime >::invoke")
		->args({"self","InLoadTime"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_842 = das::das_call_member< float (UPackage::*)() const,&UPackage::GetLoadTime >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:473:8
	makeExtern<DAS_CALL_METHOD(_method_842), SimNode_ExtFuncCall >(lib,"GetLoadTime","das_call_member< float (UPackage::*)() const , &UPackage::GetLoadTime >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_843 = das::das_call_member< FName (UPackage::*)() const,&UPackage::GetFolderName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:488:8
	makeExtern<DAS_CALL_METHOD(_method_843), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFolderName","das_call_member< FName (UPackage::*)() const , &UPackage::GetFolderName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_844 = das::das_call_member< void (UPackage::*)(FName),&UPackage::SetFolderName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:497:7
	makeExtern<DAS_CALL_METHOD(_method_844), SimNode_ExtFuncCall >(lib,"SetFolderName","das_call_member< void (UPackage::*)(FName) , &UPackage::SetFolderName >::invoke")
		->args({"self","name"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_845 = das::das_call_member< void (UPackage::*)(),&UPackage::ClearDirtyFlag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:506:7
	makeExtern<DAS_CALL_METHOD(_method_845), SimNode_ExtFuncCall >(lib,"ClearDirtyFlag","das_call_member< void (UPackage::*)() , &UPackage::ClearDirtyFlag >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_846 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetDirtyFlag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:514:7
	makeExtern<DAS_CALL_METHOD(_method_846), SimNode_ExtFuncCall >(lib,"SetDirtyFlag","das_call_member< void (UPackage::*)(bool) , &UPackage::SetDirtyFlag >::invoke")
		->args({"self","bIsDirty"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_847 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsDirty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:521:7
	makeExtern<DAS_CALL_METHOD(_method_847), SimNode_ExtFuncCall >(lib,"IsDirty","das_call_member< bool (UPackage::*)() const , &UPackage::IsDirty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_848 = das::das_call_member< void (UPackage::*)(),&UPackage::MarkAsNewlyCreated >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:530:7
	makeExtern<DAS_CALL_METHOD(_method_848), SimNode_ExtFuncCall >(lib,"MarkAsNewlyCreated","das_call_member< void (UPackage::*)() , &UPackage::MarkAsNewlyCreated >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_849 = das::das_call_member< void (UPackage::*)(),&UPackage::MarkAsUnloaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:540:7
	makeExtern<DAS_CALL_METHOD(_method_849), SimNode_ExtFuncCall >(lib,"MarkAsUnloaded","das_call_member< void (UPackage::*)() , &UPackage::MarkAsUnloaded >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_850 = das::das_call_member< void (UPackage::*)(),&UPackage::MarkAsFullyLoaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:550:7
	makeExtern<DAS_CALL_METHOD(_method_850), SimNode_ExtFuncCall >(lib,"MarkAsFullyLoaded","das_call_member< void (UPackage::*)() , &UPackage::MarkAsFullyLoaded >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_851 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsFullyLoaded >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:560:7
	makeExtern<DAS_CALL_METHOD(_method_851), SimNode_ExtFuncCall >(lib,"IsFullyLoaded","das_call_member< bool (UPackage::*)() const , &UPackage::IsFullyLoaded >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_852 = das::das_call_member< void (UPackage::*)(),&UPackage::FullyLoad >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:565:7
	makeExtern<DAS_CALL_METHOD(_method_852), SimNode_ExtFuncCall >(lib,"FullyLoad","das_call_member< void (UPackage::*)() , &UPackage::FullyLoad >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_853 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetCanBeImportedFlag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:580:7
	makeExtern<DAS_CALL_METHOD(_method_853), SimNode_ExtFuncCall >(lib,"SetCanBeImportedFlag","das_call_member< void (UPackage::*)(bool) , &UPackage::SetCanBeImportedFlag >::invoke")
		->args({"self","bInCanBeImported"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_854 = das::das_call_member< bool (UPackage::*)() const,&UPackage::CanBeImported >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:590:7
	makeExtern<DAS_CALL_METHOD(_method_854), SimNode_ExtFuncCall >(lib,"CanBeImported","das_call_member< bool (UPackage::*)() const , &UPackage::CanBeImported >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:597:15
	makeExtern< void (*)(UPackage &,EObjectFlags) , _dasUnreal_virtual_310_TagSubobjects , SimNode_ExtFuncCall >(lib,"TagSubobjects","_dasUnreal_virtual_310_TagSubobjects")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_856 = das::das_call_member< void (UPackage::*)(),&UPackage::ThisContainsMap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:602:7
	makeExtern<DAS_CALL_METHOD(_method_856), SimNode_ExtFuncCall >(lib,"ThisContainsMap","das_call_member< void (UPackage::*)() , &UPackage::ThisContainsMap >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_857 = das::das_call_member< bool (UPackage::*)() const,&UPackage::ContainsMap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:612:7
	makeExtern<DAS_CALL_METHOD(_method_857), SimNode_ExtFuncCall >(lib,"ContainsMap","das_call_member< bool (UPackage::*)() const , &UPackage::ContainsMap >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_858 = das::das_call_member< void (UPackage::*)(bool),&UPackage::ThisRequiresLocalizationGather >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:620:7
	makeExtern<DAS_CALL_METHOD(_method_858), SimNode_ExtFuncCall >(lib,"ThisRequiresLocalizationGather","das_call_member< void (UPackage::*)(bool) , &UPackage::ThisRequiresLocalizationGather >::invoke")
		->args({"self","Value"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_859 = das::das_call_member< bool (UPackage::*)() const,&UPackage::RequiresLocalizationGather >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:637:7
	makeExtern<DAS_CALL_METHOD(_method_859), SimNode_ExtFuncCall >(lib,"RequiresLocalizationGather","das_call_member< bool (UPackage::*)() const , &UPackage::RequiresLocalizationGather >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_860 = das::das_call_member< void (UPackage::*)(unsigned int),&UPackage::SetPackageFlagsTo >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:645:7
	makeExtern<DAS_CALL_METHOD(_method_860), SimNode_ExtFuncCall >(lib,"SetPackageFlagsTo","das_call_member< void (UPackage::*)(unsigned int) , &UPackage::SetPackageFlagsTo >::invoke")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_861 = das::das_call_member< void (UPackage::*)(unsigned int),&UPackage::SetPackageFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:663:19
	makeExtern<DAS_CALL_METHOD(_method_861), SimNode_ExtFuncCall >(lib,"SetPackageFlags","das_call_member< void (UPackage::*)(unsigned int) , &UPackage::SetPackageFlags >::invoke")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_862 = das::das_call_member< void (UPackage::*)(unsigned int),&UPackage::ClearPackageFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:673:19
	makeExtern<DAS_CALL_METHOD(_method_862), SimNode_ExtFuncCall >(lib,"ClearPackageFlags","das_call_member< void (UPackage::*)(unsigned int) , &UPackage::ClearPackageFlags >::invoke")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_863 = das::das_call_member< bool (UPackage::*)(unsigned int) const,&UPackage::HasAnyPackageFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:686:19
	makeExtern<DAS_CALL_METHOD(_method_863), SimNode_ExtFuncCall >(lib,"HasAnyPackageFlags","das_call_member< bool (UPackage::*)(unsigned int) const , &UPackage::HasAnyPackageFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_864 = das::das_call_member< bool (UPackage::*)(unsigned int) const,&UPackage::HasAllPackagesFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:697:19
	makeExtern<DAS_CALL_METHOD(_method_864), SimNode_ExtFuncCall >(lib,"HasAllPackagesFlags","das_call_member< bool (UPackage::*)(unsigned int) const , &UPackage::HasAllPackagesFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_865 = das::das_call_member< unsigned int (UPackage::*)() const,&UPackage::GetPackageFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:707:21
	makeExtern<DAS_CALL_METHOD(_method_865), SimNode_ExtFuncCall >(lib,"GetPackageFlags","das_call_member< unsigned int (UPackage::*)() const , &UPackage::GetPackageFlags >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_866 = das::das_call_member< bool (UPackage::*)() const,&UPackage::IsExternallyReferenceable >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:715:19
	makeExtern<DAS_CALL_METHOD(_method_866), SimNode_ExtFuncCall >(lib,"IsExternallyReferenceable","das_call_member< bool (UPackage::*)() const , &UPackage::IsExternallyReferenceable >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_867 = das::das_call_member< void (UPackage::*)(bool),&UPackage::SetIsExternallyReferenceable >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:725:19
	makeExtern<DAS_CALL_METHOD(_method_867), SimNode_ExtFuncCall >(lib,"SetIsExternallyReferenceable","das_call_member< void (UPackage::*)(bool) , &UPackage::SetIsExternallyReferenceable >::invoke")
		->args({"self","bValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_868 = das::das_call_member< bool (UPackage::*)() const,&UPackage::HasThumbnailMap >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:739:7
	makeExtern<DAS_CALL_METHOD(_method_868), SimNode_ExtFuncCall >(lib,"HasThumbnailMap","das_call_member< bool (UPackage::*)() const , &UPackage::HasThumbnailMap >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_869 = das::das_call_member< int64 (UPackage::*)() const,&UPackage::GetFileSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:832:20
	makeExtern<DAS_CALL_METHOD(_method_869), SimNode_ExtFuncCall >(lib,"GetFileSize","das_call_member< int64 (UPackage::*)() const , &UPackage::GetFileSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_870 = das::das_call_member< UObject * (UPackage::*)(EObjectFlags) const,&UPackage::FindAssetInPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:878:11
	makeExtern<DAS_CALL_METHOD(_method_870), SimNode_ExtFuncCall >(lib,"FindAssetInPackage","das_call_member< UObject * (UPackage::*)(EObjectFlags) const , &UPackage::FindAssetInPackage >::invoke")
		->args({"self","RequiredTopLevelFlags"})
		->arg_init(1,make_smart<ExprConstEnumeration>(0,makeType<EObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_871 = das::das_call_member< bool (UPackage::*)() const,&UPackage::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:893:7
	makeExtern<DAS_CALL_METHOD(_method_871), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (UPackage::*)() const , &UPackage::HasMetaData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_872 = das::das_call_member< UMetaData * (UPackage::*)(),&UPackage::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:909:19
	makeExtern<DAS_CALL_METHOD(_method_872), SimNode_ExtFuncCall >(lib,"GetMetaData","das_call_member< UMetaData * (UPackage::*)() , &UPackage::GetMetaData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:950:14
	makeExtern< void (*)(const UPackage *) , _dasUnreal_static_311_WaitForAsyncFileWrites , SimNode_ExtFuncCall >(lib,"WaitForAsyncFileWrites","_dasUnreal_static_311_WaitForAsyncFileWrites")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:953:14
	makeExtern< bool (*)(const UPackage *) , _dasUnreal_static_312_HasAsyncFileWrites , SimNode_ExtFuncCall >(lib,"HasAsyncFileWrites","_dasUnreal_static_312_HasAsyncFileWrites")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:962:14
	makeExtern< bool (*)(const UPackage *,UPackage *,const UObject *) , _dasUnreal_static_313_IsEmptyPackage , SimNode_ExtFuncCall >(lib,"IsEmptyPackage","_dasUnreal_static_313_IsEmptyPackage")
		->args({"self","Package","LastReferencer"})
		->arg_type(0,makeType<TExplicit<const UPackage*> >(lib))
		->arg_init(2,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:155:2
	makeExtern< FFieldClass * (*)(const FProperty *) , _dasUnreal_static_314_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_314_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:155:2
	makeExtern< uint64 (*)(const FProperty *) , _dasUnreal_static_315_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_315_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:155:2
	makeExtern< uint64 (*)(const FProperty *) , _dasUnreal_static_316_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_316_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:207:31
	makeExtern< void (*)(FProperty &,FArchive &) , _dasUnreal_virtual_317_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_317_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:211:31
	makeExtern< void (*)(FProperty &,const FField &) , _dasUnreal_virtual_318_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_318_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:228:31
	makeExtern< FName (*)(const FProperty *,UStruct *,FName) , _dasUnreal_static_319_FindRedirectedPropertyName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FindRedirectedPropertyName","_dasUnreal_static_319_FindRedirectedPropertyName")
		->args({"self","ObjectStruct","OldName"})
		->arg_type(0,makeType<TExplicit<const FProperty*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:233:34
	makeExtern< FString (*)(const FProperty &,FString &) , _dasUnreal_virtual_320_GetCPPMacroType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPMacroType","_dasUnreal_virtual_320_GetCPPMacroType")
		->args({"self","ExtendedTypeText"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:234:31
	makeExtern< bool (*)(const FProperty &) , _dasUnreal_virtual_321_PassCPPArgsByRef , SimNode_ExtFuncCall >(lib,"PassCPPArgsByRef","_dasUnreal_virtual_321_PassCPPArgsByRef")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_884 = das::das_call_member< FString (FProperty::*)() const,&FProperty::GetNameCPP >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:242:26
	makeExtern<DAS_CALL_METHOD(_method_884), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameCPP","das_call_member< FString (FProperty::*)() const , &FProperty::GetNameCPP >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:250:34
	makeExtern< FString (*)(const FProperty &,FString *,unsigned int) , _dasUnreal_virtual_322_GetCPPType , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPType","_dasUnreal_virtual_322_GetCPPType")
		->args({"self","ExtendedTypeText","CPPExportFlags"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->arg_init(2,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:252:34
	makeExtern< FString (*)(const FProperty &) , _dasUnreal_virtual_323_GetCPPTypeForwardDeclaration , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetCPPTypeForwardDeclaration","_dasUnreal_virtual_323_GetCPPTypeForwardDeclaration")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_887 = das::das_call_member< UPropertyWrapper * (FProperty::*)(),&FProperty::GetUPropertyWrapper >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:257:36
	makeExtern<DAS_CALL_METHOD(_method_887), SimNode_ExtFuncCall >(lib,"GetUPropertyWrapper","das_call_member< UPropertyWrapper * (FProperty::*)() , &FProperty::GetUPropertyWrapper >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:261:15
	makeExtern< bool (*)(const FProperty &) , _dasUnreal_virtual_324_HasSetter , SimNode_ExtFuncCall >(lib,"HasSetter","_dasUnreal_virtual_324_HasSetter")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:266:15
	makeExtern< bool (*)(const FProperty &) , _dasUnreal_virtual_325_HasGetter , SimNode_ExtFuncCall >(lib,"HasGetter","_dasUnreal_virtual_325_HasGetter")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:271:15
	makeExtern< bool (*)(const FProperty &) , _dasUnreal_virtual_326_HasSetterOrGetter , SimNode_ExtFuncCall >(lib,"HasSetterOrGetter","_dasUnreal_virtual_326_HasSetterOrGetter")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:280:15
	makeExtern< void (*)(const FProperty &,void *,const void *) , _dasUnreal_virtual_327_CallSetter , SimNode_ExtFuncCall >(lib,"CallSetter","_dasUnreal_virtual_327_CallSetter")
		->args({"self","Container","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:289:15
	makeExtern< void (*)(const FProperty &,const void *,void *) , _dasUnreal_virtual_328_CallGetter , SimNode_ExtFuncCall >(lib,"CallGetter","_dasUnreal_virtual_328_CallGetter")
		->args({"self","Container","OutValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_893 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetOffset_ForDebug >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:314:20
	makeExtern<DAS_CALL_METHOD(_method_893), SimNode_ExtFuncCall >(lib,"GetOffset_ForDebug","das_call_member< int (FProperty::*)() const , &FProperty::GetOffset_ForDebug >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_894 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetOffset_ForUFunction >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:319:20
	makeExtern<DAS_CALL_METHOD(_method_894), SimNode_ExtFuncCall >(lib,"GetOffset_ForUFunction","das_call_member< int (FProperty::*)() const , &FProperty::GetOffset_ForUFunction >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_895 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetOffset_ForGC >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:324:20
	makeExtern<DAS_CALL_METHOD(_method_895), SimNode_ExtFuncCall >(lib,"GetOffset_ForGC","das_call_member< int (FProperty::*)() const , &FProperty::GetOffset_ForGC >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_896 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetOffset_ForInternal >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:329:20
	makeExtern<DAS_CALL_METHOD(_method_896), SimNode_ExtFuncCall >(lib,"GetOffset_ForInternal","das_call_member< int (FProperty::*)() const , &FProperty::GetOffset_ForInternal >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_897 = das::das_call_member< int (FProperty::*)() const,&FProperty::GetOffset_ReplaceWith_ContainerPtrToValuePtr >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:334:20
	makeExtern<DAS_CALL_METHOD(_method_897), SimNode_ExtFuncCall >(lib,"GetOffset_ReplaceWith_ContainerPtrToValuePtr","das_call_member< int (FProperty::*)() const , &FProperty::GetOffset_ReplaceWith_ContainerPtrToValuePtr >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_898 = das::das_call_member< void (FProperty::*)(FArchive &),&FProperty::LinkWithoutChangingOffset >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:339:7
	makeExtern<DAS_CALL_METHOD(_method_898), SimNode_ExtFuncCall >(lib,"LinkWithoutChangingOffset","das_call_member< void (FProperty::*)(FArchive &) , &FProperty::LinkWithoutChangingOffset >::invoke")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_899 = das::das_call_member< int (FProperty::*)(FArchive &),&FProperty::Link >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:344:8
	makeExtern<DAS_CALL_METHOD(_method_899), SimNode_ExtFuncCall >(lib,"Link","das_call_member< int (FProperty::*)(FArchive &) , &FProperty::Link >::invoke")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:379:31
	makeExtern< bool (*)(const FProperty &,const void *,const void *,unsigned int) , _dasUnreal_virtual_329_Identical , SimNode_ExtFuncCall >(lib,"Identical","_dasUnreal_virtual_329_Identical")
		->args({"self","A","B","PortFlags"})
		->arg_init(3,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_901 = das::das_call_member< bool (FProperty::*)(const void *,const void *,int,unsigned int) const,&FProperty::Identical_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:390:7
	makeExtern<DAS_CALL_METHOD(_method_901), SimNode_ExtFuncCall >(lib,"Identical_InContainer","das_call_member< bool (FProperty::*)(const void *,const void *,int,unsigned int) const , &FProperty::Identical_InContainer >::invoke")
		->args({"self","A","B","ArrayIndex","PortFlags"})
		->arg_init(3,make_smart<ExprConstInt>(0))
		->arg_init(4,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:447:31
	makeExtern< bool (*)(const FProperty &) , _dasUnreal_virtual_330_SupportsNetSharedSerialization , SimNode_ExtFuncCall >(lib,"SupportsNetSharedSerialization","_dasUnreal_virtual_330_SupportsNetSharedSerialization")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:450:15
	makeExtern< void (*)(const FProperty &,FString &,const void *,const void *,UObject *,int,UObject *) , _dasUnreal_virtual_331_ExportTextItem , SimNode_ExtFuncCall >(lib,"ExportTextItem","_dasUnreal_virtual_331_ExportTextItem")
		->args({"self","ValueStr","PropertyValue","DefaultValue","Parent","PortFlags","ExportRootScope"})
		->arg_init(6,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_904 = das::das_call_member< void (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const,&FProperty::ExportTextItem_Direct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:455:7
	makeExtern<DAS_CALL_METHOD(_method_904), SimNode_ExtFuncCall >(lib,"ExportTextItem_Direct","das_call_member< void (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const , &FProperty::ExportTextItem_Direct >::invoke")
		->args({"self","ValueStr","PropertyValue","DefaultValue","Parent","PortFlags","ExportRootScope"})
		->arg_init(6,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_905 = das::das_call_member< void (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const,&FProperty::ExportTextItem_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:460:7
	makeExtern<DAS_CALL_METHOD(_method_905), SimNode_ExtFuncCall >(lib,"ExportTextItem_InContainer","das_call_member< void (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const , &FProperty::ExportTextItem_InContainer >::invoke")
		->args({"self","ValueStr","Container","DefaultValue","Parent","PortFlags","ExportRootScope"})
		->arg_init(6,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_906 = das::das_call_member< void (FProperty::*)(void *,const void *) const,&FProperty::SetValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:509:19
	makeExtern<DAS_CALL_METHOD(_method_906), SimNode_ExtFuncCall >(lib,"SetValue_InContainer","das_call_member< void (FProperty::*)(void *,const void *) const , &FProperty::SetValue_InContainer >::invoke")
		->args({"self","OutContainer","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_907 = das::das_call_member< void (FProperty::*)(const void *,void *) const,&FProperty::GetValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:520:19
	makeExtern<DAS_CALL_METHOD(_method_907), SimNode_ExtFuncCall >(lib,"GetValue_InContainer","das_call_member< void (FProperty::*)(const void *,void *) const , &FProperty::GetValue_InContainer >::invoke")
		->args({"self","InContainer","OutValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_908 = das::das_call_member< void (FProperty::*)(void *,const void *,int) const,&FProperty::SetSingleValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:538:23
	makeExtern<DAS_CALL_METHOD(_method_908), SimNode_ExtFuncCall >(lib,"SetSingleValue_InContainer","das_call_member< void (FProperty::*)(void *,const void *,int) const , &FProperty::SetSingleValue_InContainer >::invoke")
		->args({"self","OutContainer","InValue","ArrayIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_909 = das::das_call_member< void (FProperty::*)(const void *,void *,int) const,&FProperty::GetSingleValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:546:23
	makeExtern<DAS_CALL_METHOD(_method_909), SimNode_ExtFuncCall >(lib,"GetSingleValue_InContainer","das_call_member< void (FProperty::*)(const void *,void *,int) const , &FProperty::GetSingleValue_InContainer >::invoke")
		->args({"self","InContainer","OutValue","ArrayIndex"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_910 = das::das_call_member< void * (FProperty::*)() const,&FProperty::AllocateAndInitializeValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:549:24
	makeExtern<DAS_CALL_METHOD(_method_910), SimNode_ExtFuncCall >(lib,"AllocateAndInitializeValue","das_call_member< void * (FProperty::*)() const , &FProperty::AllocateAndInitializeValue >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_911 = das::das_call_member< void (FProperty::*)(void *) const,&FProperty::DestroyAndFreeValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:552:23
	makeExtern<DAS_CALL_METHOD(_method_911), SimNode_ExtFuncCall >(lib,"DestroyAndFreeValue","das_call_member< void (FProperty::*)(void *) const , &FProperty::DestroyAndFreeValue >::invoke")
		->args({"self","InMemory"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:576:32
	makeExtern< void * (*)(const FProperty &,const FProperty *,void *,int) , _dasUnreal_virtual_332_GetValueAddressAtIndex_Direct , SimNode_ExtFuncCall >(lib,"GetValueAddressAtIndex_Direct","_dasUnreal_virtual_332_GetValueAddressAtIndex_Direct")
		->args({"self","Inner","InValueAddress","Index"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:590:31
	makeExtern< void (*)(const FProperty &,FBlake3 &,bool) , _dasUnreal_virtual_333_AppendSchemaHash , SimNode_ExtFuncCall >(lib,"AppendSchemaHash","_dasUnreal_virtual_333_AppendSchemaHash")
		->args({"self","Builder","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_914 = das::das_call_member< bool (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const,&FProperty::ExportText_Direct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:599:23
	makeExtern<DAS_CALL_METHOD(_method_914), SimNode_ExtFuncCall >(lib,"ExportText_Direct","das_call_member< bool (FProperty::*)(FString &,const void *,const void *,UObject *,int,UObject *) const , &FProperty::ExportText_Direct >::invoke")
		->args({"self","ValueStr","Data","Delta","Parent","PortFlags","ExportRootScope"})
		->arg_init(6,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_915 = das::das_call_member< bool (FProperty::*)(int,FString &,const void *,const void *,UObject *,int,UObject *) const,&FProperty::ExportText_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:600:19
	makeExtern<DAS_CALL_METHOD(_method_915), SimNode_ExtFuncCall >(lib,"ExportText_InContainer","das_call_member< bool (FProperty::*)(int,FString &,const void *,const void *,UObject *,int,UObject *) const , &FProperty::ExportText_InContainer >::invoke")
		->args({"self","Index","ValueStr","Data","Delta","Parent","PortFlags","ExportRootScope"})
		->arg_init(7,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_916 = das::das_call_member< bool (FProperty::*)(int) const,&FProperty::IsInContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:731:19
	makeExtern<DAS_CALL_METHOD(_method_916), SimNode_ExtFuncCall >(lib,"IsInContainer","das_call_member< bool (FProperty::*)(int) const , &FProperty::IsInContainer >::invoke")
		->args({"self","ContainerSize"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_917 = das::das_call_member< bool (FProperty::*)(UStruct *) const,&FProperty::IsInContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:736:19
	makeExtern<DAS_CALL_METHOD(_method_917), SimNode_ExtFuncCall >(lib,"IsInContainer","das_call_member< bool (FProperty::*)(UStruct *) const , &FProperty::IsInContainer >::invoke")
		->args({"self","ContainerClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_918 = das::das_call_member< void (FProperty::*)(void *,const void *) const,&FProperty::CopySingleValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:752:19
	makeExtern<DAS_CALL_METHOD(_method_918), SimNode_ExtFuncCall >(lib,"CopySingleValue","das_call_member< void (FProperty::*)(void *,const void *) const , &FProperty::CopySingleValue >::invoke")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_919 = das::das_call_member< unsigned int (FProperty::*)(const void *) const,&FProperty::GetValueTypeHash >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:770:25
	makeExtern<DAS_CALL_METHOD(_method_919), SimNode_ExtFuncCall >(lib,"GetValueTypeHash","das_call_member< unsigned int (FProperty::*)(const void *) const , &FProperty::GetValueTypeHash >::invoke")
		->args({"self","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_920 = das::das_call_member< void (FProperty::*)(void *,const void *) const,&FProperty::CopyCompleteValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:786:19
	makeExtern<DAS_CALL_METHOD(_method_920), SimNode_ExtFuncCall >(lib,"CopyCompleteValue","das_call_member< void (FProperty::*)(void *,const void *) const , &FProperty::CopyCompleteValue >::invoke")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_921 = das::das_call_member< void (FProperty::*)(void *,const void *) const,&FProperty::CopyCompleteValue_InContainer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:800:19
	makeExtern<DAS_CALL_METHOD(_method_921), SimNode_ExtFuncCall >(lib,"CopyCompleteValue_InContainer","das_call_member< void (FProperty::*)(void *,const void *) const , &FProperty::CopyCompleteValue_InContainer >::invoke")
		->args({"self","Dest","Src"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

