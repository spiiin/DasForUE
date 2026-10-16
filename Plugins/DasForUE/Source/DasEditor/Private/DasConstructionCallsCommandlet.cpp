#include "DasConstructionCallsCommandlet.h"
#include "DasDSLService.h"
#include "DasProbeActor.h"
#include "DasScriptComponent.h"
#include "DasRuntime.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "UObject/StructOnScope.h"
#include "Misc/ScopeExit.h"
#include "Misc/PackageName.h"
#include "UObject/SavePackage.h"
#include "HAL/FileManager.h"

UDasConstructionCallsCommandlet::UDasConstructionCallsCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasConstructionCallsCommandlet::Main(const FString& Params){
    FString Id,Phase;FParse::Value(*Params,TEXT("ProbeId="),Id);FParse::Value(*Params,TEXT("Phase="),Phase);
    FGuid Guid;if(!FGuid::ParseExact(Id,EGuidFormats::Digits,Guid)){UE_LOG(LogTemp,Error,TEXT("Construction calls assertion failed at line %d"),__LINE__);return 1;}
    const FString Package=TEXT("/Game/DasConstructionCalls/")+Id+TEXT("/BP_ConstructionCalls");
    UBlueprint* BP=nullptr;FString Error;
    const FString PeerPackage=FPackageName::GetLongPackagePath(Package)+TEXT("/BP_ConstructionCallPeer");UBlueprint* PeerBP=nullptr;
    if(Phase==TEXT("Create")){
        if(!FDasDSLService::Apply(TEXT("ue_inheritance_base.das"),PeerPackage,TEXT("inheritance.BaseCounter"),PeerBP,Error)){UE_LOG(LogTemp,Error,TEXT("Construction peer import: %s"),*Error);return 1;}
        const FString File=FPackageName::LongPackageNameToFilename(PeerPackage,FPackageName::GetAssetPackageExtension());IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);
        FSavePackageArgs Save;Save.TopLevelFlags=RF_Public|RF_Standalone;if(!UPackage::SavePackage(PeerBP->GetOutermost(),PeerBP,*File,Save))return 1;
    }else PeerBP=LoadObject<UBlueprint>(nullptr,*(PeerPackage+TEXT(".BP_ConstructionCallPeer")));
    if(!PeerBP || !PeerBP->GeneratedClass)return 1;
    if(Phase==TEXT("Create")){
        if(!FDasDSLService::Apply(TEXT("dsl_construction_calls.das"),Package,TEXT("constructioncalls.ConstructionCalls"),BP,Error)){UE_LOG(LogTemp,Error,TEXT("Construction calls import: %s"),*Error);{UE_LOG(LogTemp,Error,TEXT("Construction calls assertion failed at line %d"),__LINE__);return 1;}}
        auto* WorkerField=FindFProperty<FObjectProperty>(BP->GeneratedClass,TEXT("Worker"));
        auto* WorkerBP=WorkerField?Cast<UBlueprint>(WorkerField->PropertyClass->ClassGeneratedBy):nullptr;
        if(!WorkerBP){UE_LOG(LogTemp,Error,TEXT("Missing generated worker Blueprint"));return 1;}
        const FString WorkerFile=FPackageName::LongPackageNameToFilename(WorkerBP->GetOutermost()->GetName(),FPackageName::GetAssetPackageExtension());
        FSavePackageArgs WorkerSave;WorkerSave.TopLevelFlags=RF_Public|RF_Standalone;
        if(!UPackage::SavePackage(WorkerBP->GetOutermost(),WorkerBP,*WorkerFile,WorkerSave))return 1;
        const FString File=FPackageName::LongPackageNameToFilename(Package,FPackageName::GetAssetPackageExtension());IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);
        FSavePackageArgs Save;Save.TopLevelFlags=RF_Public|RF_Standalone;if(!UPackage::SavePackage(BP->GetOutermost(),BP,*File,Save)){UE_LOG(LogTemp,Error,TEXT("Construction calls assertion failed at line %d"),__LINE__);return 1;}
    }else BP=LoadObject<UBlueprint>(nullptr,*(Package+TEXT(".BP_ConstructionCalls")));
    if(!BP || !BP->GeneratedClass){UE_LOG(LogTemp,Error,TEXT("Construction calls assertion failed at line %d"),__LINE__);return 1;}
    // No global FEditorScriptExecutionGuard: the bridge must authorize editor-world calls itself.
    auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    auto Int=[](UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FIntProperty>(O->GetClass(),N):nullptr;return P?P->GetPropertyValue_InContainer(O):MIN_int32;};
    for(auto Type:{EWorldType::Editor,EWorldType::Game}){
        TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(Type,false));if(!World){UE_LOG(LogTemp,Error,TEXT("Construction calls assertion failed at line %d"),__LINE__);return 1;}
        ON_SCOPE_EXIT{World->DestroyWorld(false);};
        auto* Actor=World->SpawnActor<ADasProbeActor>(BP->GeneratedClass);
        if(!Actor || !Actor->LastDasError.IsEmpty() || Int(Actor,TEXT("Result"))!=27){UE_LOG(LogTemp,Error,TEXT("Construction component dispatch: %s"),Actor?*Actor->LastDasError:TEXT("no actor"));{UE_LOG(LogTemp,Error,TEXT("Construction calls assertion failed at line %d"),__LINE__);return 1;}}
        auto* WorkerProperty=FindFProperty<FObjectProperty>(Actor->GetClass(),TEXT("Worker"));auto* Worker=WorkerProperty?Cast<UDasScriptComponent>(WorkerProperty->GetObjectPropertyValue_InContainer(Actor)):nullptr;
        auto* PeerProperty=FindFProperty<FObjectProperty>(Actor->GetClass(),TEXT("CounterTarget"));if(!Worker || !PeerProperty){UE_LOG(LogTemp,Error,TEXT("Construction calls assertion failed at line %d"),__LINE__);return 1;}
        auto* Peer=World->SpawnActor<ADasProbeActor>(PeerBP->GeneratedClass);if(!Peer){UE_LOG(LogTemp,Error,TEXT("Construction calls assertion failed at line %d"),__LINE__);return 1;}PeerProperty->SetObjectPropertyValue_InContainer(Actor,Peer);
        auto Evaluate=[&](){auto* F=Worker->FindFunction(TEXT("Evaluate"));if(!F)return MIN_int32;FStructOnScope Frame(F);auto* Input=FindFProperty<FIntProperty>(F,TEXT("input"));auto* Output=CastField<FIntProperty>(F->GetReturnProperty());if(!Input || !Output)return MIN_int32;Input->SetPropertyValue_InContainer(Frame.GetStructMemory(),0);Worker->ProcessEvent(F,Frame.GetStructMemory());return Output->GetPropertyValue_InContainer(Frame.GetStructMemory());};
        // Seed a real persistent VM; construction must neither reuse nor discard it.
        if(Evaluate()!=6){UE_LOG(LogTemp,Error,TEXT("Construction calls assertion failed at line %d"),__LINE__);return 1;}const int32 Contexts=Runtime.PersistentContextCount();const int32 Subscriptions=Runtime.SubscriptionCount(Worker);if(Subscriptions!=1){UE_LOG(LogTemp,Error,TEXT("Expected one persistent worker subscription, got %d"),Subscriptions);return 1;}
        for(int32 Attempt=0;Attempt<2;++Attempt){
            Actor->OnConstruction(FTransform::Identity);
            if(!Actor->LastDasError.IsEmpty() || Int(Actor,TEXT("Result"))!=37 || Int(Actor,TEXT("InitCount"))!=0 || Int(Worker,TEXT("InitCount"))!=0 || Runtime.PersistentContextCount()!=Contexts || Runtime.SubscriptionCount(Worker)!=Subscriptions || Runtime.SubscriptionCount(Actor)!=0){UE_LOG(LogTemp,Error,TEXT("Construction calls assertion failed at line %d"),__LINE__);return 1;}
        }
        if(Evaluate()!=7){UE_LOG(LogTemp,Error,TEXT("Construction calls assertion failed at line %d"),__LINE__);return 1;}
        auto* Mode=FindFProperty<FIntProperty>(Actor->GetClass(),TEXT("FailureMode"));if(!Mode){UE_LOG(LogTemp,Error,TEXT("Construction calls assertion failed at line %d"),__LINE__);return 1;}
        for(int32 Failure=1;Failure<=2;++Failure){
            Mode->SetPropertyValue_InContainer(Actor,Failure);Actor->OnConstruction(FTransform::Identity);
            const FString Expected=Failure==1?TEXT("managed callbacks are forbidden during ConstructionScript"):TEXT("construction nested method failure");
            if(!Actor->LastDasError.Contains(Expected) || Runtime.PersistentContextCount()!=Contexts || Runtime.SubscriptionCount(Worker)!=Subscriptions){UE_LOG(LogTemp,Error,TEXT("Nested failure propagation: %s"),*Actor->LastDasError);{UE_LOG(LogTemp,Error,TEXT("Construction calls assertion failed at line %d"),__LINE__);return 1;}}
            Mode->SetPropertyValue_InContainer(Actor,0);Actor->OnConstruction(FTransform::Identity);
            if(!Actor->LastDasError.IsEmpty() || Int(Actor,TEXT("Result"))!=37){UE_LOG(LogTemp,Error,TEXT("Construction calls assertion failed at line %d"),__LINE__);return 1;}
        }
        if(Evaluate()!=8){UE_LOG(LogTemp,Error,TEXT("Construction calls assertion failed at line %d"),__LINE__);return 1;}
        Actor->RerunConstructionScripts();if(!Actor->LastDasError.IsEmpty() || Int(Actor,TEXT("Result"))!=37){UE_LOG(LogTemp,Error,TEXT("Construction calls assertion failed at line %d"),__LINE__);return 1;}
        Runtime.ReleaseObject(Worker);Runtime.ReleaseObject(Actor);Runtime.ReleaseObject(Peer);Actor->Destroy();Peer->Destroy();
        UE_LOG(LogTemp,Display,TEXT("PASS construction_reflected_actor_component_calls_isolated_VMs_recovery_%s_%s"),Type==EWorldType::Editor?TEXT("Editor"):TEXT("Game"),*Phase);
    }
    UE_LOG(LogTemp,Display,TEXT("DAS_CONSTRUCTION_CALLS_RESULT=PASS"));return 0;
}
