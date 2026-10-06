#include "DasFrameCommandlet.h"
#include "DasDSLService.h"
#include "DasProbeActor.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/World.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"
#include "UObject/SavePackage.h"
#include "UObject/MetaData.h"
#include "UObject/StructOnScope.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
UDasFrameCommandlet::UDasFrameCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasFrameCommandlet::Main(const FString& Params){
    if(FApp::GetProjectName()!=FString(TEXT("DasForUESmoke")))return 1;
    FString Id,Phase,Error;if(!FParse::Value(*Params,TEXT("ProbeId="),Id) || Id.Len()!=32 || !FParse::Value(*Params,TEXT("Phase="),Phase))return 1;for(TCHAR Ch:Id)if(!FChar::IsHexDigit(Ch))return 1;
    const bool TextMode=FParse::Param(*Params,TEXT("Text"));
    const FString Script=TextMode?TEXT("dsl_text_methods.das"):TEXT("dsl_mixed_methods.das");const FString ClassId=TextMode?TEXT("text.TextCounter"):TEXT("mixed.MixedCounter");
    const FString Package=TEXT("/Game/DasFrameAssetProbe/Run_")+Id+TEXT("/BP_Mixed");UBlueprint* BP=nullptr;bool Passed=true;
    auto Check=[&](bool V,const TCHAR* N){Passed &= V;UE_LOG(LogTemp,Display,TEXT("DAS_FRAME_ASSET %s %s"),V?TEXT("PASS"):TEXT("FAIL"),N);};
    if(Phase==TEXT("Create"))Check(FDasDSLService::Apply(Script,Package,ClassId,BP,Error),TEXT("create_frame_graphs"));
    else if(Phase==TEXT("Reopen")){BP=LoadObject<UBlueprint>(nullptr,*(Package+TEXT(".BP_Mixed")));Check(BP && BP->GeneratedClass,TEXT("reopen_frame_graphs"));}
    else return 1;
    if(!Passed || !BP){UE_LOG(LogTemp,Error,TEXT("%s"),*Error);return 1;}
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Editor,false));FActorSpawnParameters Spawn;auto* Actor=World->SpawnActor<ADasProbeActor>(BP->GeneratedClass,FTransform::Identity,Spawn);Check(Actor!=nullptr,TEXT("spawn_saved_frame_class"));if(!Actor)return 1;
    if(TextMode){
        auto* F=Actor->FindFunction(TEXT("Echo"));Check(F && F->NumParms==3,TEXT("text_signature_survives_save"));
        if(F){FStructOnScope Memory(F);auto* A=FindFProperty<FStrProperty>(F,TEXT("value"));auto* B=FindFProperty<FStrProperty>(F,TEXT("suffix"));auto* Result=CastField<FStrProperty>(F->GetReturnProperty());if(A && B && Result){A->SetPropertyValue_InContainer(Memory.GetStructMemory(),TEXT("Привет 世界"));B->SetPropertyValue_InContainer(Memory.GetStructMemory(),TEXT(" !"));FEditorScriptExecutionGuard Guard;Actor->ProcessEvent(F,Memory.GetStructMemory());Check(Result->GetPropertyValue_InContainer(Memory.GetStructMemory())==TEXT("Привет 世界 !"),TEXT("saved_text_ProcessEvent"));}else Check(false,TEXT("text_parameters"));}
        auto* D=FindFProperty<FDoubleProperty>(Actor->GetClass(),TEXT("DSLPrecise"));auto* N=FindFProperty<FNameProperty>(Actor->GetClass(),TEXT("DSLKey"));Check(D && D->GetPropertyValue_InContainer(Actor)==1.23456789012345 && N && N->GetPropertyValue_InContainer(Actor)==FName(TEXT("Initial_7")),TEXT("saved_double_name_defaults"));
        Check(BP->GetOutermost()->GetMetaData().GetValue(BP,TEXT("DasParameterId.Echo.value"))==TEXT("text.TextCounter.Echo.value"),TEXT("text_parameter_identity_persisted"));
    }else {
    auto* F=Actor->FindFunction(TEXT("Compute"));Check(F && F->NumParms==4,TEXT("mixed_signature_survives_save"));
    if(F){FStructOnScope Memory(F);auto* A=FindFProperty<FIntProperty>(F,TEXT("count"));auto* B=FindFProperty<FFloatProperty>(F,TEXT("factor"));auto* C=FindFProperty<FBoolProperty>(F,TEXT("apply"));auto* R=CastField<FFloatProperty>(F->GetReturnProperty());
        if(A && B && C && R){A->SetPropertyValue_InContainer(Memory.GetStructMemory(),2);B->SetPropertyValue_InContainer(Memory.GetStructMemory(),0.5f);C->SetPropertyValue_InContainer(Memory.GetStructMemory(),true);FEditorScriptExecutionGuard Guard;Actor->ProcessEvent(F,Memory.GetStructMemory());Check(R->GetPropertyValue_InContainer(Memory.GetStructMemory())==24.f,TEXT("saved_frame_graph_ProcessEvent"));}else Check(false,TEXT("saved_parameter_types"));}
    Check(BP->GetOutermost()->GetMetaData().GetValue(BP,TEXT("DasParameterId.Compute.count"))==TEXT("mixed.MixedCounter.Compute.count"),TEXT("frame_parameter_identity_persisted"));
    }
    UClass* Before=BP->GeneratedClass;Check(FDasDSLService::Reimport(BP,Error) && BP->GeneratedClass==Before,TEXT("saved_frame_reimport_without_reinstance"));
    if(Phase==TEXT("Create")){BP->SetFlags(RF_Public|RF_Standalone);const FString File=FPackageName::LongPackageNameToFilename(Package,FPackageName::GetAssetPackageExtension());IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;Args.SaveFlags=SAVE_NoError;Check(UPackage::SavePackage(BP->GetOutermost(),BP,*File,Args),TEXT("save_frame_blueprint"));}
    World->DestroyWorld(false);UE_LOG(LogTemp,Display,TEXT("DAS_FRAME_ASSET_RESULT=%s"),Passed?TEXT("PASS"):TEXT("FAIL"));return Passed?0:1;
}
