#include "DasCookedGameplayCommandlet.h"
#include "DasDSLService.h"
#include "DasProbeActor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/World.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

UDasCookedGameplayCommandlet::UDasCookedGameplayCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasCookedGameplayCommandlet::Main(const FString& Params){
 FString Id;FParse::Value(*Params,TEXT("ProbeId="),Id);FGuid Guid;if(!FGuid::ParseExact(Id,EGuidFormats::Digits,Guid))return 1;
 const FString Root=TEXT("/Game/DasCookedGameplay/")+Id;FString Error;UBlueprint *Gameplay=nullptr,*Native=nullptr,*Delegate=nullptr,*Single=nullptr,*Inherited=nullptr,*Life=nullptr,*TickOnly=nullptr;
 if(!FDasDSLService::Apply(TEXT("dsl_gameplay_runtime_probe.das"),Root+TEXT("/BP_Gameplay"),TEXT("gameplayprobe.GameplayRuntimeProbe"),Gameplay,Error) || !FDasDSLService::Apply(TEXT("dsl_cooked_native_owner.das"),Root+TEXT("/BP_NativeOwner"),TEXT("cookednative.CookedNativeOwner"),Native,Error) || !FDasDSLService::Apply(TEXT("dsl_delegate_actor.das"),Root+TEXT("/BP_Delegate"),TEXT("delegateexample.DelegateExample"),Delegate,Error) || !FDasDSLService::Apply(TEXT("dsl_single_delegate_actor.das"),Root+TEXT("/BP_SingleDelegate"),TEXT("singledelegate.SingleDelegateActor"),Single,Error)){UE_LOG(LogTemp,Error,TEXT("Cooked fixtures: %s"),*Error);return 1;}
 if(!FDasDSLService::Apply(TEXT("dsl_inheritance_child.das"),Root+TEXT("/BP_ChildCounter"),TEXT("inheritance.ChildCounter"),Inherited,Error)){UE_LOG(LogTemp,Error,TEXT("Cooked inheritance: %s"),*Error);return 1;}
 if(!FDasDSLService::Apply(TEXT("dsl_life_actor_child.das"),Root+TEXT("/BP_ChildLifeActor"),TEXT("lifefamily.ChildLifeActor"),Life,Error)){UE_LOG(LogTemp,Error,TEXT("Cooked lifecycle: %s"),*Error);return 1;}
 if(!FDasDSLService::Apply(TEXT("dsl_life_tick_child.das"),Root+TEXT("/BP_TickOnlyChild"),TEXT("lifetick.TickOnlyChild"),TickOnly,Error)){UE_LOG(LogTemp,Error,TEXT("Cooked tick-only: %s"),*Error);return 1;}
 auto Save=[](UObject* O){const FString File=FPackageName::LongPackageNameToFilename(O->GetOutermost()->GetName(),O->IsA<UWorld>()?FPackageName::GetMapPackageExtension():FPackageName::GetAssetPackageExtension());IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;return UPackage::SavePackage(O->GetOutermost(),O,*File,Args);};
 // Save generated component classes too; the map retains their cook dependencies.
 for(const auto* Name:{TEXT("BP_Gameplay"),TEXT("BP_NativeOwner"),TEXT("BP_Delegate"),TEXT("BP_CookedBehavior"),TEXT("BP_SingleDelegate"),TEXT("BP_ChildCounter"),TEXT("BP_BaseCounter"),TEXT("BP_BaseLifeActor"),TEXT("BP_ChildLifeActor"),TEXT("BP_BaseBehavior"),TEXT("BP_ChildBehavior"),TEXT("BP_TickOnlyBase"),TEXT("BP_TickOnlyChild")})if(auto* BP=FindObject<UBlueprint>(nullptr,*(Root+TEXT("/")+Name+TEXT(".")+Name)))if(!Save(BP))return 1;
 auto* World=UWorld::CreateWorld(EWorldType::Editor,false,TEXT("Map"),CreatePackage(*(Root+TEXT("/Map"))));World->SetFlags(RF_Public|RF_Standalone);World->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
 auto Spawn=[&](UBlueprint* BP,const TCHAR* Name){FActorSpawnParameters P;P.Name=Name;auto* A=World->SpawnActor<ADasProbeActor>(BP->GeneratedClass,FTransform::Identity,P);if(A)A->Tags.Add(Name);return A;};
 const bool Good=Spawn(Gameplay,TEXT("GameplayFirst")) && Spawn(Gameplay,TEXT("GameplaySecond")) && Spawn(Native,TEXT("NativeOwner")) && Spawn(Delegate,TEXT("DelegateOwner")) && Spawn(Single,TEXT("SingleOwner")) && Spawn(Inherited,TEXT("InheritedOwner")) && Spawn(Life,TEXT("LifeOwner")) && Spawn(TickOnly,TEXT("TickOnlyOwner")) && Save(World);
 World->DestroyWorld(false);if(!Good)return 1;UE_LOG(LogTemp,Display,TEXT("DAS_COOKED_GAMEPLAY_EXPORT=PASS map=%s/Map"),*Root);return 0;
}
