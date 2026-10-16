#include "DasDefaultsMergeCommandlet.h"
#include "DasDefaultsMigration.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasProbeActor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "Engine/InheritableComponentHandler.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/UnrealType.h"
#include "UObject/StructOnScope.h"
#include "UObject/SavePackage.h"
#include "UObject/Script.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"
#include "HAL/IConsoleManager.h"

namespace {
int32 Int(UObject* O,const TCHAR* N){return FindFProperty<FIntProperty>(O->GetClass(),N)->GetPropertyValue_InContainer(O);}
void SetInt(UObject* O,const TCHAR* N,int32 V){FindFProperty<FIntProperty>(O->GetClass(),N)->SetPropertyValue_InContainer(O,V);}
UObject* Ref(UObject* O,const TCHAR* N){return FindFProperty<FObjectPropertyBase>(O->GetClass(),N)->GetObjectPropertyValue_InContainer(O);}
FVector& Vector(UObject* O,const TCHAR* N){return *FindFProperty<FStructProperty>(O->GetClass(),N)->ContainerPtrToValuePtr<FVector>(O);}
UObject* Template(UBlueprint* BP,const TCHAR* Name){for(auto* N:BP->SimpleConstructionScript->GetAllNodes())if(N->GetVariableName()==Name)return N->ComponentTemplate.Get();return nullptr;}
int32 Call(ADasProbeActor* A){auto* F=A->FindFunction(TEXT("Read"));FStructOnScope Frame(F);FindFProperty<FIntProperty>(F,TEXT("input"))->SetPropertyValue_InContainer(Frame.GetStructMemory(),0);A->ProcessEvent(F,Frame.GetStructMemory());return CastFieldChecked<FIntProperty>(F->GetReturnProperty())->GetPropertyValue_InContainer(Frame.GetStructMemory());}
bool Save(UObject* O){const FString File=FPackageName::LongPackageNameToFilename(O->GetOutermost()->GetName(),O->IsA<UWorld>()?FPackageName::GetMapPackageExtension():FPackageName::GetAssetPackageExtension());IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;return UPackage::SavePackage(O->GetOutermost(),O,*File,Args);}
}
UDasDefaultsMergeCommandlet::UDasDefaultsMergeCommandlet(){IsEditor=true;IsClient=false;IsServer=false;LogToConsole=true;}
int32 UDasDefaultsMergeCommandlet::Main(const FString& Params){
 FString Id,Phase;FParse::Value(*Params,TEXT("ProbeId="),Id);FParse::Value(*Params,TEXT("Phase="),Phase);FGuid Guid;if(!FGuid::ParseExact(Id,EGuidFormats::Digits,Guid))return 1;
 FEditorScriptExecutionGuard Guard;const FString Root=TEXT("/Game/DasDefaultsMerge/")+Id;FString Error;
 auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));const FString Path=R.ScriptPath(TEXT("dsl_defaults_merge.das"));FString Final;if(!FFileHelper::LoadFileToString(Final,*Path))return 1;
 ON_SCOPE_EXIT{FFileHelper::SaveStringToFile(Final,*Path,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);};
 auto Check=[&](bool OK,const TCHAR* Name){UE_LOG(LogTemp,Display,TEXT("DAS_DEFAULTS_TEST %s %s %s"),OK?TEXT("PASS"):TEXT("FAIL"),Name,*Error);return OK;};
 UBlueprint* BP=nullptr;UBlueprint* Child=nullptr;UWorld* W=nullptr;ADasProbeActor* A=nullptr;ADasProbeActor* M=nullptr;
 ON_SCOPE_EXIT{if(W && W->IsInitialized())W->DestroyWorld(false);};
 if(Phase==TEXT("Unopened")){
  BP=LoadObject<UBlueprint>(nullptr,*(Root+TEXT("/BP_Merge.BP_Merge")));if(!BP)return 1;const FString Prior=BP->GetOutermost()->GetMetaData().GetValue(BP,TEXT("DasDSLModel"));
  const FString Changed=Final.Replace(TEXT("var Auto:int=20"),TEXT("var Auto:int=30"));FFileHelper::SaveStringToFile(Changed,*Path,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
  const bool Rejected=!FDasDSLService::Reimport(BP,Error) && Error.Contains(TEXT("Open/initialize"));const bool Same=Int(BP->GeneratedClass->GetDefaultObject(),TEXT("Auto"))==20 && Prior==BP->GetOutermost()->GetMetaData().GetValue(BP,TEXT("DasDSLModel"));
  if(auto* Map=FindObject<UWorld>(nullptr,*(Root+TEXT("/Map.Map"))))Map->DestroyWorld(false);
  if(!Check(Rejected && Same,TEXT("unopened_component_map_rejected_before_mutation")))return 1;UE_LOG(LogTemp,Display,TEXT("DAS_DEFAULTS_MERGE_RESULT=PASS phase=Unopened"));return 0;
 }
 if(Phase==TEXT("Create")){
  FString Old=Final.Replace(TEXT("int=20"),TEXT("int=10")).Replace(TEXT("4.0lf,5.0lf,6.0lf"),TEXT("1.0lf,2.0lf,3.0lf")).Replace(TEXT("4.0lf,5.0lf,20.0lf"),TEXT("1.0lf,2.0lf,10.0lf")).Replace(TEXT("Sphere.Sphere"),TEXT("Cube.Cube")).Replace(TEXT("/Script/Engine.Pawn"),TEXT("/Script/Engine.Actor")).Replace(TEXT("Bonus=52"),TEXT("Bonus=42"));
  if(!FFileHelper::SaveStringToFile(Old,*Path,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) || !Check(FDasDSLService::Apply(TEXT("dsl_defaults_merge.das"),Root+TEXT("/BP_Merge"),TEXT("defaultsmerge.MergeActor"),BP,Error),TEXT("import_old")))return 1;
  Child=FKismetEditorUtilities::CreateBlueprint(BP->GeneratedClass,CreatePackage(*(Root+TEXT("/BP_Child"))),TEXT("BP_Child"),BPTYPE_Normal,UBlueprint::StaticClass(),UBlueprintGeneratedClass::StaticClass());if(!Child)return 1;FKismetEditorUtilities::CompileBlueprint(Child);
  SetInt(Child->GeneratedClass->GetDefaultObject(),TEXT("Manual"),55);
  for(auto* Node:BP->SimpleConstructionScript->GetAllNodes())if(Node->GetVariableName()==TEXT("Pivot")){auto* Override=Child->GetInheritableComponentHandler(true)->CreateOverridenComponentTemplate(FComponentKey(Node));if(!Override)return 1;Vector(Override,TEXT("RelativeLocation")).Y=66;}

  auto* CDO=BP->GeneratedClass->GetDefaultObject();SetInt(CDO,TEXT("Manual"),77);Vector(CDO,TEXT("Position")).X=99;
  Vector(Template(BP,TEXT("Pivot")),TEXT("RelativeLocation")).X=99;SetInt(Template(BP,TEXT("Worker")),TEXT("Bonus"),88);
  W=UWorld::CreateWorld(EWorldType::Editor,false,TEXT("Map"),CreatePackage(*(Root+TEXT("/Map"))));if(!W)return 1;W->SetFlags(RF_Public|RF_Standalone);W->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
  A=W->SpawnActor<ADasProbeActor>(BP->GeneratedClass);M=W->SpawnActor<ADasProbeActor>(BP->GeneratedClass);if(!A || !M)return 1;A->Tags.Add(TEXT("MergeInherited"));M->Tags.Add(TEXT("MergeManual"));
  SetInt(M,TEXT("Auto"),111);Vector(M,TEXT("Position")).Y=222;Vector(Ref(M,TEXT("Pivot")),TEXT("RelativeLocation")).Z=777;SetInt(Ref(M,TEXT("OtherWorker")),TEXT("Bonus"),123);
  CastChecked<UStaticMeshComponent>(Ref(M,TEXT("Mesh")))->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cylinder.Cylinder")));
  // A no-change merge registers the source baseline without erasing manual CDO edits.
  if(!Check(FDasDSLService::Reimport(BP,Error),TEXT("bootstrap_manual_CDO")) || !Check(Call(A)==87 && A->LastDasError.IsEmpty(),TEXT("persistent_VM_seed")))return 1;
  {auto* Game=UWorld::CreateWorld(EWorldType::Game,false);auto* Playing=Game->SpawnActor<ADasProbeActor>(BP->GeneratedClass);const bool Rejected=Playing && !FDasDSLService::Reimport(BP,Error) && Error.Contains(TEXT("PIE/game"));Game->DestroyActor(Playing);Game->DestroyWorld(false);if(!Check(Rejected,TEXT("game_world_rejected_before_mutation")))return 1;}
  {const FString Bad=Old.Replace(TEXT("var Auto:int=10"),TEXT("var Auto:int=10\n        var Extra:int=1"));FFileHelper::SaveStringToFile(Bad,*Path,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);const bool Rejected=!FDasDSLService::Reimport(BP,Error,EDasDefaultMigrationPolicy::PreserveOverrides);FFileHelper::SaveStringToFile(Old,*Path,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);if(!Check(Rejected && Int(CDO,TEXT("Auto"))==10,TEXT("layout_change_rejected_before_mutation")))return 1;}
  auto* OriginalClass=BP->GeneratedClass.Get();const int32 Contexts=R.PersistentContextCount();const FString Metadata=BP->GetOutermost()->GetMetaData().GetValue(BP,TEXT("DasDSLModel"));
  if(!Save(BP) || !Save(Child) || !Save(W))return 1;
  if(!FFileHelper::SaveStringToFile(Final,*Path,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))return 1;
#if WITH_DEV_AUTOMATION_TESTS
  for(int32 Stage:{1,2}){DasFailDefaultsMergeForTesting(Stage);const bool Merged=FDasDSLService::Reimport(BP,Error);DasFailDefaultsMergeForTesting(0);
   if(!Check(!Merged && Error.Contains(TEXT("rolled back")) && BP->GeneratedClass.Get()==OriginalClass && Int(CDO,TEXT("Auto"))==10 && Int(CDO,TEXT("Manual"))==77 && Int(A,TEXT("Auto"))==10 && Int(M,TEXT("Auto"))==111 && Int(Template(BP,TEXT("OtherWorker")),TEXT("Bonus"))==42 && BP->GetOutermost()->GetMetaData().GetValue(BP,TEXT("DasDSLModel"))==Metadata && !BP->GetOutermost()->IsDirty() && !W->GetOutermost()->IsDirty() && R.PersistentContextCount()==Contexts,TEXT("atomic_rollback")))return 1;
  }
#endif
  if(!Check(IConsoleManager::Get().ProcessUserConsoleInput(*(TEXT("Das.DSL.Reimport ")+Root+TEXT("/BP_Merge")),*GLog,W) && Int(CDO,TEXT("Auto"))==20,TEXT("console_automatic_merge_new_defaults")) || !Check(BP->GeneratedClass.Get()==OriginalClass && R.PersistentContextCount()==Contexts,TEXT("class_and_context_identity")))return 1;
 }else{
  BP=LoadObject<UBlueprint>(nullptr,*(Root+TEXT("/BP_Merge.BP_Merge")));Child=LoadObject<UBlueprint>(nullptr,*(Root+TEXT("/BP_Child.BP_Child")));W=LoadObject<UWorld>(nullptr,*(Root+TEXT("/Map.Map")));if(!BP || !Child || !W)return 1; W->WorldType=EWorldType::Editor; if(!W->IsInitialized())W->InitWorld();W->UpdateWorldComponents(true,false);
  for(const auto& Actor:W->PersistentLevel->Actors)if(auto* P=Cast<ADasProbeActor>(Actor)){if(P->Tags.Contains(TEXT("MergeInherited")))A=P;if(P->Tags.Contains(TEXT("MergeManual")))M=P;}
 }
 if(Phase==TEXT("Unopened"))return 1;
 if(!A || !M)return 1;auto* CDO=BP->GeneratedClass->GetDefaultObject();auto* CCDO=Child->GeneratedClass->GetDefaultObject();
 UE_LOG(LogTemp,Display,TEXT("DAS_DEFAULTS_VALUES pivot template=%s A=%s M=%s worker=%d otherA=%d otherM=%d"),*Vector(Template(BP,TEXT("Pivot")),TEXT("RelativeLocation")).ToString(),*Vector(Ref(A,TEXT("Pivot")),TEXT("RelativeLocation")).ToString(),*Vector(Ref(M,TEXT("Pivot")),TEXT("RelativeLocation")).ToString(),Int(Ref(A,TEXT("Worker")),TEXT("Bonus")),Int(Ref(A,TEXT("OtherWorker")),TEXT("Bonus")),Int(Ref(M,TEXT("OtherWorker")),TEXT("Bonus")));
 if(!Check(Int(CDO,TEXT("Auto"))==20 && Int(CDO,TEXT("Manual"))==77,TEXT("CDO_source_and_manual")) ||
 !Check(Int(CCDO,TEXT("Auto"))==20 && Int(CCDO,TEXT("Manual"))==55,TEXT("child_CDO_override")) ||
 !Check(Int(A,TEXT("Auto"))==20 && Int(M,TEXT("Auto"))==111,TEXT("placed_instance_override")) ||
 !Check(Vector(CDO,TEXT("Position"))==FVector(99,5,6) && Vector(A,TEXT("Position"))==FVector(99,5,6) && Vector(M,TEXT("Position"))==FVector(99,222,6),TEXT("struct_member_merge")) ||
 !Check(Vector(Ref(A,TEXT("Pivot")),TEXT("RelativeLocation"))==FVector(99,5,20) && Vector(Ref(M,TEXT("Pivot")),TEXT("RelativeLocation"))==FVector(99,5,777),TEXT("scene_template_and_instance")) ||
 !Check(Int(Template(BP,TEXT("Worker")),TEXT("Bonus"))==88 && Int(Ref(A,TEXT("Worker")),TEXT("Bonus"))==88 && Int(Ref(A,TEXT("OtherWorker")),TEXT("Bonus"))==52 && Int(Ref(M,TEXT("OtherWorker")),TEXT("Bonus"))==123,TEXT("component_template_sibling_overrides")) ||
 !Check(Ref(A,TEXT("Asset"))->GetPathName()==TEXT("/Engine/BasicShapes/Sphere.Sphere") && Ref(A,TEXT("SpawnType"))->GetPathName()==TEXT("/Script/Engine.Pawn"),TEXT("asset_and_class_defaults")) ||
 !Check(Ref(Ref(A,TEXT("Mesh")),TEXT("StaticMesh"))->GetPathName()==TEXT("/Engine/BasicShapes/Sphere.Sphere") && Ref(Ref(M,TEXT("Mesh")),TEXT("StaticMesh"))->GetPathName()==TEXT("/Engine/BasicShapes/Cylinder.Cylinder"),TEXT("native_asset_manual_override")) ||
 !Check(Call(A)==97 && A->LastDasError.IsEmpty(),TEXT("reflected_call_after_reload")))return 1;
 auto* Fresh=W->SpawnActor<ADasProbeActor>(BP->GeneratedClass);if(!Fresh || !Check(Int(Fresh,TEXT("Auto"))==20 && Int(Fresh,TEXT("Manual"))==77 && Int(Ref(Fresh,TEXT("OtherWorker")),TEXT("Bonus"))==52,TEXT("fresh_spawn")))return 1;R.ReleaseObject(Fresh);Fresh->Destroy();
 {UObject* Override=nullptr;for(auto* Node:BP->SimpleConstructionScript->GetAllNodes())if(Node->GetVariableName()==TEXT("Pivot"))Override=Child->GetInheritableComponentHandler(false)->GetOverridenComponentTemplate(FComponentKey(Node));if(!Check(Override && Vector(Override,TEXT("RelativeLocation"))==FVector(1,66,20),TEXT("inherited_component_template_override")))return 1;}
 if(Phase==TEXT("Create") && (!Check(IConsoleManager::Get().ProcessUserConsoleInput(*(TEXT("Das.DSL.Reimport ")+Root+TEXT("/BP_Merge defaults=preserve_overrides")),*GLog,W),TEXT("console_explicit_repeat_reimport")) || !Check(Int(CDO,TEXT("Manual"))==77 && Int(M,TEXT("Auto"))==111,TEXT("repeat_preserves_overrides"))))return 1;
 if(Phase==TEXT("Recompile")){
  R.ReleaseObject(A);R.ReleaseObject(M);FKismetEditorUtilities::CompileBlueprint(BP);FKismetEditorUtilities::CompileBlueprint(Child);
  auto* CheckActor=W->SpawnActor<ADasProbeActor>(BP->GeneratedClass);if(!Check(CheckActor && Int(BP->GeneratedClass->GetDefaultObject(),TEXT("Manual"))==77 && Int(BP->GeneratedClass->GetDefaultObject(),TEXT("Auto"))==20 && Int(Child->GeneratedClass->GetDefaultObject(),TEXT("Manual"))==55 && Call(CheckActor)==97 && CheckActor->LastDasError.IsEmpty(),TEXT("Blueprint_recompile_keeps_defaults_and_provenance")))return 1;R.ReleaseObject(CheckActor);CheckActor->Destroy();
 }
 if(Phase==TEXT("Create")){if(!Save(BP) || !Save(Child) || !Save(W))return 1;auto* Worker=LoadObject<UBlueprint>(nullptr,*(Root+TEXT("/BP_MergeWorker.BP_MergeWorker")));if(!Worker || !Save(Worker))return 1;}
 if(Phase!=TEXT("Recompile")){R.ReleaseObject(A);R.ReleaseObject(M);}W->DestroyWorld(false);UE_LOG(LogTemp,Display,TEXT("DAS_DEFAULTS_MERGE_RESULT=PASS phase=%s"),*Phase);return 0;
}
