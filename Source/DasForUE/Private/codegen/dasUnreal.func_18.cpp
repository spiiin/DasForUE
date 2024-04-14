// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_18() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:277:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,int) , _dasUnreal_static_731_GenericArray_Remove , SimNode_ExtFuncCall >(lib,"GenericArray_Remove","_dasUnreal_static_731_GenericArray_Remove")
		->args({"self","TargetArray","ArrayProp","IndexToRemove"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:278:25
	makeExtern< bool (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,const void *) , _dasUnreal_static_732_GenericArray_RemoveItem , SimNode_ExtFuncCall >(lib,"GenericArray_RemoveItem","_dasUnreal_static_732_GenericArray_RemoveItem")
		->args({"self","TargetArray","ArrayProp","Item"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:279:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *) , _dasUnreal_static_733_GenericArray_Clear , SimNode_ExtFuncCall >(lib,"GenericArray_Clear","_dasUnreal_static_733_GenericArray_Clear")
		->args({"self","TargetArray","ArrayProp"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:280:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,int) , _dasUnreal_static_734_GenericArray_Resize , SimNode_ExtFuncCall >(lib,"GenericArray_Resize","_dasUnreal_static_734_GenericArray_Resize")
		->args({"self","TargetArray","ArrayProp","Size"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:281:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *) , _dasUnreal_static_735_GenericArray_Reverse , SimNode_ExtFuncCall >(lib,"GenericArray_Reverse","_dasUnreal_static_735_GenericArray_Reverse")
		->args({"self","TargetArray","ArrayProp"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:282:26
	makeExtern< int (*)(const UKismetArrayLibrary *,const void *,const FArrayProperty *) , _dasUnreal_static_736_GenericArray_Length , SimNode_ExtFuncCall >(lib,"GenericArray_Length","_dasUnreal_static_736_GenericArray_Length")
		->args({"self","TargetArray","ArrayProp"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:283:25
	makeExtern< bool (*)(const UKismetArrayLibrary *,const void *,const FArrayProperty *) , _dasUnreal_static_737_GenericArray_IsEmpty , SimNode_ExtFuncCall >(lib,"GenericArray_IsEmpty","_dasUnreal_static_737_GenericArray_IsEmpty")
		->args({"self","TargetArray","ArrayProp"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:284:25
	makeExtern< bool (*)(const UKismetArrayLibrary *,const void *,const FArrayProperty *) , _dasUnreal_static_738_GenericArray_IsNotEmpty , SimNode_ExtFuncCall >(lib,"GenericArray_IsNotEmpty","_dasUnreal_static_738_GenericArray_IsNotEmpty")
		->args({"self","TargetArray","ArrayProp"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:285:26
	makeExtern< int (*)(const UKismetArrayLibrary *,const void *,const FArrayProperty *) , _dasUnreal_static_739_GenericArray_LastIndex , SimNode_ExtFuncCall >(lib,"GenericArray_LastIndex","_dasUnreal_static_739_GenericArray_LastIndex")
		->args({"self","TargetArray","ArrayProp"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:286:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,int,void *) , _dasUnreal_static_740_GenericArray_Get , SimNode_ExtFuncCall >(lib,"GenericArray_Get","_dasUnreal_static_740_GenericArray_Get")
		->args({"self","TargetArray","ArrayProp","Index","Item"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:287:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,int,const void *,bool) , _dasUnreal_static_741_GenericArray_Set , SimNode_ExtFuncCall >(lib,"GenericArray_Set","_dasUnreal_static_741_GenericArray_Set")
		->args({"self","TargetArray","ArrayProp","Index","NewItem","bSizeToFit"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:288:25
	makeExtern< void (*)(const UKismetArrayLibrary *,const void *,const FArrayProperty *,int,int) , _dasUnreal_static_742_GenericArray_Swap , SimNode_ExtFuncCall >(lib,"GenericArray_Swap","_dasUnreal_static_742_GenericArray_Swap")
		->args({"self","TargetArray","ArrayProp","First","Second"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:289:26
	makeExtern< int (*)(const UKismetArrayLibrary *,const void *,const FArrayProperty *,const void *) , _dasUnreal_static_743_GenericArray_Find , SimNode_ExtFuncCall >(lib,"GenericArray_Find","_dasUnreal_static_743_GenericArray_Find")
		->args({"self","TargetArray","ArrayProperty","ItemToFind"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:290:25
	makeExtern< void (*)(const UKismetArrayLibrary *,UObject *,FName,const void *) , _dasUnreal_static_744_GenericArray_SetArrayPropertyByName , SimNode_ExtFuncCall >(lib,"GenericArray_SetArrayPropertyByName","_dasUnreal_static_744_GenericArray_SetArrayPropertyByName")
		->args({"self","OwnerObject","ArrayPropertyName","SrcArrayAddr"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:291:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,void *,int *) , _dasUnreal_static_745_GenericArray_Random , SimNode_ExtFuncCall >(lib,"GenericArray_Random","_dasUnreal_static_745_GenericArray_Random")
		->args({"self","TargetArray","ArrayProp","OutItem","OutIndex"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:293:25
	makeExtern< bool (*)(const UKismetArrayLibrary *,const void *,const FArrayProperty *,int) , _dasUnreal_static_746_GenericArray_IsValidIndex , SimNode_ExtFuncCall >(lib,"GenericArray_IsValidIndex","_dasUnreal_static_746_GenericArray_IsValidIndex")
		->args({"self","TargetArray","ArrayProp","IndexToTest"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1452 = das::das_call_member< bool (FAssetRegistryModule::*)() const,&FAssetRegistryModule::IsValid >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/AssetRegistry/Public\AssetRegistry/AssetRegistryModule.h:44:7
	makeExtern<DAS_CALL_METHOD(_method_1452), SimNode_ExtFuncCall >(lib,"IsValid","das_call_member< bool (FAssetRegistryModule::*)() const , &FAssetRegistryModule::IsValid >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/AssetRegistry/Public\AssetRegistry/AssetRegistryModule.h:60:14
	makeExtern< void (*)(const FAssetRegistryModule *,float) , _dasUnreal_static_747_TickAssetRegistry , SimNode_ExtFuncCall >(lib,"TickAssetRegistry","_dasUnreal_static_747_TickAssetRegistry")
		->args({"self","DeltaTime"})
		->arg_type(0,makeType<TExplicit<const FAssetRegistryModule*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/AssetRegistry/Public\AssetRegistry/AssetRegistryModule.h:65:14
	makeExtern< void (*)(const FAssetRegistryModule *,UObject *) , _dasUnreal_static_748_AssetCreated , SimNode_ExtFuncCall >(lib,"AssetCreated","_dasUnreal_static_748_AssetCreated")
		->args({"self","NewAsset"})
		->arg_type(0,makeType<TExplicit<const FAssetRegistryModule*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/AssetRegistry/Public\AssetRegistry/AssetRegistryModule.h:70:14
	makeExtern< void (*)(const FAssetRegistryModule *,UObject *) , _dasUnreal_static_749_AssetDeleted , SimNode_ExtFuncCall >(lib,"AssetDeleted","_dasUnreal_static_749_AssetDeleted")
		->args({"self","DeletedAsset"})
		->arg_type(0,makeType<TExplicit<const FAssetRegistryModule*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/AssetRegistry/Public\AssetRegistry/AssetRegistryModule.h:75:14
	makeExtern< void (*)(const FAssetRegistryModule *,const UObject *,const FString &) , _dasUnreal_static_750_AssetRenamed , SimNode_ExtFuncCall >(lib,"AssetRenamed","_dasUnreal_static_750_AssetRenamed")
		->args({"self","RenamedAsset","OldObjectPath"})
		->arg_type(0,makeType<TExplicit<const FAssetRegistryModule*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/AssetRegistry/Public\AssetRegistry/AssetRegistryModule.h:93:14
	makeExtern< void (*)(const FAssetRegistryModule *,UPackage *) , _dasUnreal_static_751_PackageDeleted , SimNode_ExtFuncCall >(lib,"PackageDeleted","_dasUnreal_static_751_PackageDeleted")
		->args({"self","DeletedPackage"})
		->arg_type(0,makeType<TExplicit<const FAssetRegistryModule*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
}
}

