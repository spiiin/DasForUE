#include "DasPrimaryAssetsCommandlet.h"
#include "DasRuntime.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "UObject/Package.h"
#include "UObject/Linker.h"
#include "UObject/UObjectGlobals.h"
#include "Tickable.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
#include "Misc/PackageName.h"
#include "Misc/ScopeExit.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Async/TaskGraphInterfaces.h"
#include "Containers/Ticker.h"

UDasPrimaryAssetsCommandlet::UDasPrimaryAssetsCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasPrimaryAssetsCommandlet::Main(const FString& Params){
 auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));auto& M=UAssetManager::Get();
 int32 Checks=0,Failures=0;
 auto Check=[&](bool Ok,const TCHAR* Label){++Checks;if(!Ok)++Failures;UE_LOG(LogTemp,Display,TEXT("DAS_PRIMARY_ASSETS_%s %s"),Ok?TEXT("PASS"):TEXT("FAIL"),Label);return Ok;};
 FString ExampleError;if(!Check(R.ValidateIntMethod(R.ScriptPath(TEXT("primary_assets_example.das")),ExampleError),TEXT("public_example_compiles"))){UE_LOG(LogTemp,Display,TEXT("%s"),*ExampleError);return 1;}
 const FString Tag=FGuid::NewGuid().ToString(EGuidFormats::Digits),Type=TEXT("DasPrimaryProbe_")+Tag;
 const FString Mount=TEXT("/")+Type+TEXT("/"),Directory=FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()/TEXT("DasPrimaryAssets")/Tag)+TEXT("/");
 check(Directory.StartsWith(FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir())));
 const FString RootPackage=Mount+TEXT("Profile"),BundlePackage=Mount+TEXT("Icon"),RootPath=RootPackage+TEXT(".Profile"),BundlePath=BundlePackage+TEXT(".Icon");
 const FPrimaryAssetId Id(FPrimaryAssetType(*Type),FName(TEXT("Profile")));
 IFileManager::Get().MakeDirectory(*Directory,true);FPackageName::RegisterMountPoint(Mount,Directory);
 ON_SCOPE_EXIT{M.UnloadPrimaryAsset(Id);FlushAsyncLoading();M.RemovePrimaryAssetType(FPrimaryAssetType(*Type));if(auto* P=FindPackage(nullptr,*RootPackage))ResetLoaders(P);if(auto* P=FindPackage(nullptr,*BundlePackage))ResetLoaders(P);FPackageName::UnRegisterMountPoint(Mount,Directory);IFileManager::Get().Delete(*(Directory+TEXT("Profile.uasset")));IFileManager::Get().Delete(*(Directory+TEXT("Icon.uasset")));IFileManager::Get().DeleteDirectory(*Directory,false,false);};
 auto SaveFixture=[&](const FString& PackageName,FName Name,const FString& File){
  auto* P=CreatePackage(*PackageName);auto* A=NewObject<UDasPrimaryAssetFixture>(P,Name,RF_Public|RF_Standalone);
  FSavePackageArgs Save;Save.TopLevelFlags=RF_Public|RF_Standalone;
  const bool Ok=UPackage::SavePackage(P,A,*File,Save);A->ClearFlags(RF_Standalone);P->SetDirtyFlag(false);return Ok;
 };
 if(!Check(SaveFixture(RootPackage,TEXT("Profile"),Directory+TEXT("Profile.uasset")) && SaveFixture(BundlePackage,TEXT("Icon"),Directory+TEXT("Icon.uasset")),TEXT("temporary_real_asset_packages")))return 1;
 CollectGarbage(RF_NoFlags);
 Check(!FSoftObjectPath(RootPath).ResolveObject() && !FSoftObjectPath(BundlePath).ResolveObject(),TEXT("fixtures_unloaded_before_async_request"));
 FAssetBundleData Bundle;Bundle.AddBundleAsset(TEXT("UI"),FSoftObjectPath(BundlePath).GetAssetPath());
 if(!Check(M.AddDynamicAsset(Id,FSoftObjectPath(RootPath),Bundle),TEXT("register_primary_id_and_ui_bundle")))return 1;
 FString Template,Fixture;if(!FParse::Value(*Params,TEXT("Fixture="),Fixture) || !FFileHelper::LoadFileToString(Template,*Fixture))return 2;
 Template.ReplaceInline(TEXT("__TYPE__"),*Type);Template.ReplaceInline(TEXT("__PATH__"),*RootPath);
 const FString Path=R.ScriptPath(TEXT("primary_assets_probe_")+Tag+TEXT(".das"));
 if(!FFileHelper::SaveStringToFile(Template,*Path,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))return 2;
 ON_SCOPE_EXIT{IFileManager::Get().Delete(*Path);};
 TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game,false));GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
 TStrongObjectPtr<AActor> A(World->SpawnActor<AActor>()),B(World->SpawnActor<AActor>());
 ON_SCOPE_EXIT{R.ReleaseObject(A.Get());R.ReleaseObject(B.Get());GEngine->DestroyWorldContext(World.Get());World->DestroyWorld(false);};
 auto Call=[&](AActor* Owner,const TCHAR* Entry){auto V=R.RunNamedPersistent(Path,Entry,0,Owner);if(!V.bSucceeded)UE_LOG(LogTemp,Display,TEXT("Primary Asset VM: %s"),*V.Error);return V;};
 auto Pump=[&](TFunctionRef<bool()> Done){const double End=FPlatformTime::Seconds()+5.;while(FPlatformTime::Seconds()<End){++GFrameCounter;ProcessAsyncLoading(true,false,.005);FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread);FTickableGameObject::TickObjects(nullptr,LEVELTICK_All,false,.01f);FTSTicker::GetCoreTicker().Tick(.01f);if(Done())return true;FPlatformProcess::Sleep(.01f);}return false;};
 auto Start=Call(A.Get(),TEXT("start"));if(!Check(Start.bSucceeded && Start.Value==1,TEXT("find_value_ids_path_and_start_async_load")))return 1;
 if(!Check(Pump([&]{auto V=Call(A.Get(),TEXT("status"));return V.bSucceeded && V.Value==1;}),TEXT("callback_receives_loaded_asset_and_data")))return 1;
 Check(FSoftObjectPath(BundlePath).ResolveObject()!=nullptr,TEXT("ui_bundle_loaded_with_primary_asset"));
 CollectGarbage(RF_NoFlags);auto Read=Call(A.Get(),TEXT("retained_after_gc"));Check(Read.bSucceeded && Read.Value==1,TEXT("vm_lease_keeps_asset_alive_after_callback_and_gc"));
 Start=Call(B.Get(),TEXT("start"));Check(Start.bSucceeded && Start.Value==1 && Pump([&]{auto V=Call(B.Get(),TEXT("status"));return V.bSucceeded && V.Value==1;}),TEXT("second_owner_cached_load_callback"));
 const auto FirstUnload=Call(A.Get(),TEXT("unload"));CollectGarbage(RF_NoFlags);Read=Call(B.Get(),TEXT("retained_after_gc"));
 Check(FirstUnload.bSucceeded && FirstUnload.Value==1 && M.GetPrimaryAssetHandle(Id).IsValid() && Read.bSucceeded && Read.Value==1,TEXT("first_unload_preserves_second_owners_load"));
 const auto LastUnload=Call(B.Get(),TEXT("unload"));CollectGarbage(RF_NoFlags);
 Check(LastUnload.bSucceeded && LastUnload.Value==1 && !M.GetPrimaryAssetHandle(Id).IsValid() && !FSoftObjectPath(RootPath).ResolveObject() && !FSoftObjectPath(BundlePath).ResolveObject(),TEXT("last_unload_releases_root_and_bundle_without_hidden_pins"));
 const auto Missing=Call(A.Get(),TEXT("load_missing"));Check(Missing.bSucceeded && Missing.Value==1 && Pump([&]{auto V=Call(A.Get(),TEXT("status"));return V.bSucceeded && V.Value==2;}),TEXT("unknown_id_delivers_null_callback"));
 const auto Cancel=Call(A.Get(),TEXT("cancel_load"));Check(Cancel.bSucceeded && Cancel.Value==1 && Pump([&]{auto H=M.GetPrimaryAssetHandle(Id,true);return H.IsValid() && H->HasLoadCompleted();}),TEXT("cancel_delivery_retains_explicit_asset_lease"));
 const auto State=Call(A.Get(),TEXT("status"));Check(State.bSucceeded && State.Value==2,TEXT("cancelled_callback_is_suppressed"));
 R.ReleaseObject(A.Get());CollectGarbage(RF_NoFlags);Check(!M.GetPrimaryAssetHandle(Id).IsValid(),TEXT("session_release_unloads_last_managed_lease"));
 UE_LOG(LogTemp,Display,TEXT("DAS_PRIMARY_ASSETS_RESULT=%s checks=%d"),Failures?TEXT("FAIL"):TEXT("PASS"),Checks);return Failures?1:0;
}
