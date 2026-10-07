#include "DasDSLService.h"
#include "DasBlueprintService.h"
#include "DasAuthoring.h"
#include "DasRuntime.h"
#include "DasProbeActor.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "K2Node_VariableGet.h"
#include "UObject/MetaData.h"
#include "UObject/SavePackage.h"
#include "UObject/StrProperty.h"
#include "Modules/ModuleManager.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Parse.h"
#include "HAL/FileManager.h"
int32 RunDasRenameScopeProbe(const FString& Params);
void DasFailStructuralForTesting(int32 Stage);
namespace {
UBlueprint* StructuralBP(const FString& Base,const TCHAR* N){return LoadObject<UBlueprint>(nullptr,*(Base+N+TEXT(".")+N));}
int32 StructuralScore(UObject* O){auto* P=O?FindFProperty<FIntProperty>(O->GetClass(),TEXT("Score")):nullptr;return P?P->GetPropertyValue_InContainer(O):-1;}
ADasProbeActor* StructuralActor(UWorld* W,FName N){if(W && W->PersistentLevel)for(auto& E:W->PersistentLevel->Actors)if(auto* A=Cast<ADasProbeActor>(E.Get()))if(A->GetFName()==N)return A;return nullptr;}
bool StructuralSave(UObject* O){const FString File=FPackageName::LongPackageNameToFilename(O->GetOutermost()->GetName(),O->IsA<UWorld>()?FPackageName::GetMapPackageExtension():FPackageName::GetAssetPackageExtension());FSavePackageArgs A;A.TopLevelFlags=RF_Public|RF_Standalone;A.SaveFlags=SAVE_NoError;return UPackage::SavePackage(O->GetOutermost(),O,*File,A);}
}
int32 RunDasStructuralRollbackProbe(const FString& Params){
 if(FApp::GetProjectName()!=FString(TEXT("DasForUESmoke")))return 1;FString Id,Phase,Error;if(!FParse::Value(*Params,TEXT("ProbeId="),Id) || Id.Len()!=32 || !FParse::Value(*Params,TEXT("Phase="),Phase))return 1;for(TCHAR C:Id)if(!FChar::IsHexDigit(C))return 1;
 if(Phase==TEXT("Create")){const int32 Result=RunDasRenameScopeProbe(Params);UE_LOG(LogTemp,Display,TEXT("DAS_STRUCTURAL_ASSET_RESULT=%s"),Result?TEXT("FAIL"):TEXT("PASS"));return Result;}
 const FString Base=TEXT("/Game/DasFrameAssetProbe/Scope_")+Id+TEXT("/"),Script=TEXT("dsl_renamescope_")+Id+TEXT(".das");auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));auto* Parent=StructuralBP(Base,TEXT("BP_Parent"));if(!Parent)return 1;bool Passed=true;auto Check=[&](bool V,const TCHAR* N){Passed &= V;UE_LOG(LogTemp,Display,TEXT("DAS_STRUCTURAL_ASSET %s %s"),V?TEXT("PASS"):TEXT("FAIL"),N);};
 FString Original;FFileHelper::LoadFileToString(Original,*R.ScriptPath(Script));const FString OldModel=Parent->GetOutermost()->GetMetaData().GetValue(Parent,TEXT("DasDSLModel"));FDasAuthoringModule Old;FDasClassSchema Previous;if(!FDasAuthoringModule::FromJson(OldModel,Old,Error) || !Old.ToActorSchema(Parent->GetOutermost()->GetMetaData().GetValue(Parent,TEXT("DasDSLClassId")),Previous,Error))return 1;
 if(Phase==TEXT("Rollback")){
  Check(!FindObject<UWorld>(nullptr,*(Base+TEXT("Map.Map"))),TEXT("saved_map_initially_unloaded"));FString Changed=Original;const int32 End=Changed.Find(TEXT("}"),ESearchCase::CaseSensitive,ESearchDir::FromEnd);Changed.InsertAt(End,TEXT("\n    @ue_property=(EditAnywhere,BlueprintReadWrite)\n    Added : int = 9\n    [ue_function(BlueprintCallable)]\n    def NewQuery():int { return self.Added }\n"));Check(FFileHelper::SaveStringToFile(Changed,*R.ScriptPath(Script),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),TEXT("write_added_field_and_method"));
  for(int32 Stage=1;Stage<=2;++Stage){const bool Dirty=Parent->GetOutermost()->IsDirty();DasFailStructuralForTesting(Stage);const bool Failed=!FDasDSLService::Reimport(Parent,Error);DasFailStructuralForTesting(0);Check(Failed && Error.StartsWith(TEXT("Structural migration rolled back:")) && Parent->GetOutermost()->IsDirty()==Dirty && Parent->GetOutermost()->GetMetaData().GetValue(Parent,TEXT("DasDSLModel"))==OldModel,TEXT("structural_fault_preserves_model_and_package_dirty"));if(!Error.StartsWith(TEXT("Structural migration rolled back:")))UE_LOG(LogTemp,Error,TEXT("%s"),*Error);Check(FDasBlueprintService::Apply(Parent,Previous,Error,true),TEXT("restored_blueprint_matches_original_schema"));}
 }
 auto* Child=StructuralBP(Base,TEXT("BP_Child"));auto* Grand=StructuralBP(Base,TEXT("BP_Grand"));auto* External=StructuralBP(Base,TEXT("BP_External"));auto* World=LoadObject<UWorld>(nullptr,*(Base+TEXT("Map.Map")));if(!Child || !Grand || !External || !World)return 1;
 Check(!Parent->GeneratedClass->FindPropertyByName(TEXT("Added")) && !Parent->GeneratedClass->FindFunctionByName(TEXT("NewQuery")) && !Parent->FunctionGraphs.ContainsByPredicate([](const auto& G){return G->GetFName()==TEXT("NewQuery");}),TEXT("rolled_back_field_and_method_absent"));
 Check(StructuralScore(Parent->GeneratedClass->GetDefaultObject())==5 && StructuralScore(Child->GeneratedClass->GetDefaultObject())==23 && StructuralScore(Grand->GeneratedClass->GetDefaultObject())==45 && StructuralScore(StructuralActor(World,TEXT("ParentActor")))==77 && StructuralScore(StructuralActor(World,TEXT("ChildActor")))==99 && StructuralScore(StructuralActor(World,TEXT("GrandActor")))==111,TEXT("restored_parent_child_grandchild_and_map_values"));TArray<UK2Node_VariableGet*> Nodes;FBlueprintEditorUtils::GetAllNodesOfClass(External,Nodes);Check(Nodes.ContainsByPredicate([](const auto* N){return N->VariableReference.GetMemberName()==TEXT("Score");}),TEXT("external_graph_reference_preserved"));Check(FDasBlueprintService::Apply(Parent,Previous,Error,true),TEXT("reopened_original_schema_valid"));
 if(Phase==TEXT("Rollback")){Check(StructuralSave(Parent) && StructuralSave(Child) && StructuralSave(Grand) && StructuralSave(External) && StructuralSave(World),TEXT("save_rolled_back_assets"));Check(FFileHelper::SaveStringToFile(Original,*R.ScriptPath(Script),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),TEXT("restore_disposable_source"));}else if(Phase!=TEXT("Reopen"))return 1;
 if(World->IsInitialized())World->DestroyWorld(false);UE_LOG(LogTemp,Display,TEXT("DAS_STRUCTURAL_ASSET_RESULT=%s"),Passed?TEXT("PASS"):TEXT("FAIL"));return Passed?0:1;
}
