#include "DasRuntime.h"
#include "DasSoftReferences.h"
#include "DasNativeHashProbeComponent.h"
#include "DasSoftReferencesCommandlet.h"
#include "DasDSLService.h"
#include "DasTypeService.h"
#include "DasProbeActor.h"
#include "Engine/Blueprint.h"
#include "Engine/DataAsset.h"
#include "Curves/CurveFloat.h"
#include "Engine/World.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
UDasSoftReferencesCommandlet::UDasSoftReferencesCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasSoftReferencesCommandlet::Main(const FString& Params){
 if(FParse::Param(*Params,TEXT("UpcastsOnly"))){
  FString Error;if(!CheckDasSoftUpcasts(Error)){UE_LOG(LogTemp,Error,TEXT("DAS_SOFT_UPCASTS_RESULT=FAIL native: %s"),*Error);return 1;}
  auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
  for(const TCHAR* File:{TEXT("soft_upcasts_bad_downcast.das"),TEXT("soft_upcasts_bad_unrelated.das"),TEXT("soft_upcasts_bad_kind.das"),TEXT("soft_upcasts_bad_explicit_downcast.das"),TEXT("soft_upcasts_bad_explicit_kind.das"),TEXT("soft_upcasts_bad_ref.das"),TEXT("soft_upcasts_bad_implicit_ref.das"),TEXT("soft_upcasts_bad_native_ref.das"),TEXT("soft_upcasts_bad_array.das"),TEXT("soft_upcasts_bad_map.das"),TEXT("soft_upcasts_bad_ambiguous.das")}){Error.Reset();if(Runtime.ValidateIntMethod(Runtime.ScriptPath(File),Error) || Error.IsEmpty()){UE_LOG(LogTemp,Error,TEXT("DAS_SOFT_UPCASTS_RESULT=FAIL accepted %s"),File);return 1;}}
  auto* World=UWorld::CreateWorld(EWorldType::Game,false);if(!World)return 1;TStrongObjectPtr<UWorld> Keep(World);
  auto* Owner=World->SpawnActor<ADasProbeActor>();if(!Owner){World->DestroyWorld(false);return 1;}
  auto* Component=NewObject<UDasNativeHashProbeComponent>(Owner,TEXT("SoftUpcasts"));Owner->AddInstanceComponent(Component);Component->RegisterComponent();
  const auto Result=Runtime.RunFile(Runtime.ScriptPath(TEXT("soft_upcasts_probe.das")),0,false,Owner);Runtime.ReleaseObject(Owner);World->DestroyWorld(false);
  if(!Result.bSucceeded || Result.Value!=1){UE_LOG(LogTemp,Error,TEXT("DAS_SOFT_UPCASTS_RESULT=FAIL script: %s"),*Result.Error);return 1;}
  UE_LOG(LogTemp,Display,TEXT("DAS_SOFT_UPCASTS_RESULT=PASS native+script negatives=11"));return 0;
 }
 FString Id;FParse::Value(*Params,TEXT("ProbeId="),Id);FGuid Guid;if(!FGuid::ParseExact(Id,EGuidFormats::Digits,Guid))return 1;
 auto Save=[](UObject* O){const auto File=FPackageName::LongPackageNameToFilename(O->GetOutermost()->GetName(),O->IsA<UWorld>()?FPackageName::GetMapPackageExtension():FPackageName::GetAssetPackageExtension());IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;return UPackage::SavePackage(O->GetOutermost(),O,*File,Args);};
 const FString Root=TEXT("/Game/DasSoftReferences/")+Id;FString Error;UBlueprint* BP=nullptr;
 TArray<UObject*> Types;if(!FDasTypeService::Apply(TEXT("ue_soft_record.das"),TEXT("/Game/DasSoftReferences/Types"),Types,Error)){UE_LOG(LogTemp,Error,TEXT("Soft types: %s"),*Error);return 1;}for(auto* Type:Types)if(!Save(Type))return 1;
 if(!FDasDSLService::Apply(TEXT("dsl_soft_references.das"),Root+TEXT("/BP_SoftReferences"),TEXT("dsl_soft_references.SoftReferenceActor"),BP,Error)){UE_LOG(LogTemp,Error,TEXT("Soft export: %s"),*Error);return 1;}
 TStrongObjectPtr<UBlueprint> Keep(BP);auto* Lazy=NewObject<UCurveFloat>(CreatePackage(*(Root+TEXT("/LazyAsset"))),TEXT("LazyAsset"),RF_Public|RF_Standalone);if(!Save(Lazy))return 1;
 auto* P=FindFProperty<FSoftObjectProperty>(BP->GeneratedClass,TEXT("Lazy"));if(!P || !Save(BP))return 1;
 auto* World=UWorld::CreateWorld(EWorldType::Editor,false,TEXT("Map"),CreatePackage(*(Root+TEXT("/Map"))));if(!World)return 1;World->SetFlags(RF_Public|RF_Standalone);World->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
 FActorSpawnParameters Spawn;Spawn.Name=TEXT("SoftReferences");auto* Actor=World->SpawnActor<ADasProbeActor>(BP->GeneratedClass,FTransform::Identity,Spawn);if(Actor)P->SetPropertyValue_InContainer(Actor,FSoftObjectPtr(FSoftObjectPath(Lazy)));const bool Good=Actor && Save(World);World->DestroyWorld(false);
 if(!Good)return 1;UE_LOG(LogTemp,Display,TEXT("DAS_SOFT_REFERENCES_EXPORT=PASS map=%s/Map"),*Root);return 0;
}
