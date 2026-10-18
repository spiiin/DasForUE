#include "DasExportCommandlet.h"
#include "DasRuntime.h"
#include "Modules/ModuleManager.h"
#include "Misc/Parse.h"
#include "DasAuthoredInterfaceClass.h"
#include "Engine/Blueprint.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonSerializer.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "BlueprintCompilationManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Misc/ScopeExit.h"
UDasExportCommandlet::UDasExportCommandlet() { IsClient = false; IsServer = false; IsEditor = true; LogToConsole = true; }
int32 UDasExportCommandlet::Main(const FString& Params)
{
    FString Modules;
    if (FParse::Value(*Params, TEXT("Modules="), Modules))
    {
        TArray<FString> Names; Modules.ParseIntoArray(Names, TEXT(","), true);
        for (const FString& Name : Names)
        {
            if (!FModuleManager::Get().LoadModule(*Name))
            { UE_LOG(LogTemp, Error, TEXT("DAS_EXPORT_FAILED: Cannot load module %s"), *Name); return 1; }
        }
    }
    FString Probe;
    if (FParse::Value(*Params, TEXT("Probe="), Probe))
    {
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        FString ProbeError;
        if (!Runtime.ValidateIntMethod(Probe, ProbeError))
        { UE_LOG(LogTemp, Error, TEXT("DAS_PACKAGE_PREFLIGHT_FAILED: %s"), *ProbeError); return 1; }
        TStrongObjectPtr<UWorld> ProbeWorld;
        TStrongObjectPtr<AActor> ProbeOwner;
        ON_SCOPE_EXIT
        {
            if (ProbeOwner.IsValid()) Runtime.ReleaseObject(ProbeOwner.Get());
            if (ProbeWorld.IsValid())
            {
                GEngine->DestroyWorldContext(ProbeWorld.Get());
                ProbeWorld->DestroyWorld(false);
            }
        };
        if (FParse::Param(*Params, TEXT("GameWorld")))
        {
            ProbeWorld.Reset(UWorld::CreateWorld(EWorldType::Game, false));
            if (!ProbeWorld.IsValid()) return 1;
            GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(ProbeWorld.Get());
            ProbeWorld->InitializeActorsForPlay(FURL());
            ProbeOwner.Reset(ProbeWorld->SpawnActor<AActor>());
            if (!ProbeOwner.IsValid()) return 1;
            auto* Box = NewObject<UBoxComponent>(ProbeOwner.Get(), TEXT("PackageRoot"));
            ProbeOwner->AddInstanceComponent(Box);
            ProbeOwner->SetRootComponent(Box);
            Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            Box->RegisterComponent();
            auto* Child = NewObject<USceneComponent>(ProbeOwner.Get(), TEXT("PackageChild"));
            ProbeOwner->AddInstanceComponent(Child);
            Child->SetupAttachment(Box);
            Child->RegisterComponent();
            ProbeOwner->Tags.Add(TEXT("PackageWorldProbe"));
            if(FParse::Param(*Params,TEXT("CollisionWorld"))){
                for(int32 Index=0;Index<2;++Index){
                    auto* Target=ProbeWorld->SpawnActor<AActor>();
                    auto* Shape=NewObject<UBoxComponent>(Target);
                    Target->AddInstanceComponent(Shape);Target->SetRootComponent(Shape);
                    Shape->SetBoxExtent(FVector(40));Shape->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
                    Shape->SetCollisionObjectType(ECC_WorldStatic);Shape->SetCollisionResponseToAllChannels(ECR_Block);
                    if(Index==0)Shape->SetCollisionResponseToChannel(ECC_Visibility,ECR_Overlap);
                    Shape->RegisterComponent();Target->SetActorLocation(FVector(300+200*Index,0,0));
                }
            }
        }
        if (FParse::Param(*Params, TEXT("WorldOwner")) && !ProbeWorld.IsValid()) return 1;
        UObject* ProbeContext = FParse::Param(*Params, TEXT("WorldOwner")) ? static_cast<UObject*>(ProbeWorld.Get()) : ProbeOwner.Get();
        int32 ProbeInput = 0;
        if (FParse::Param(*Params, TEXT("ProbeContextRoundtrip")))
        {
            // RunFile owns a fresh VM context per invocation. Exercise stale tokens
            // against a second context while keeping the same game world alive.
            const auto First = Runtime.RunFile(Probe, 0, false, ProbeContext);
            if (!First.bSucceeded || First.Value <= 0)
            { UE_LOG(LogTemp, Error, TEXT("DAS_CONTEXT_ROUNDTRIP_FAILED: %s"), *First.Error); return 1; }
            ProbeInput = First.Value;
        }
        const auto Result = Runtime.RunFile(Probe, ProbeInput, false, ProbeContext);
        if (!Result.bSucceeded || Result.Value != 42)
        { UE_LOG(LogTemp, Error, TEXT("DAS_PACKAGE_PREFLIGHT_FAILED: value=%d error=%s"), Result.Value, *Result.Error); return 1; }
        UE_LOG(LogTemp, Display, TEXT("DAS_PACKAGE_PREFLIGHT=PASS value=42")); return 0;
    }
    FString Selection, Output, Error;
    if (!FParse::Value(*Params, TEXT("Selection="), Selection) || !FParse::Value(*Params, TEXT("Output="), Output)) return 1;
    FString Json;TSharedPtr<FJsonObject> SelectionObject;
    if(FFileHelper::LoadFileToString(Json,*Selection) && FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json),SelectionObject) && SelectionObject){
        TSet<FString> Prepared;for(const auto* Key:{TEXT("functions"),TEXT("class_rules")}){const TArray<TSharedPtr<FJsonValue>>* Items=nullptr;if(!SelectionObject->TryGetArrayField(Key,Items))continue;for(const auto& Item:*Items){const TSharedPtr<FJsonObject>* Entry=nullptr;FString ClassPath;if(!Item->TryGetObject(Entry) || !(*Entry)->TryGetStringField(TEXT("class"),ClassPath) || !ClassPath.StartsWith(TEXT("/Game/")) || !ClassPath.EndsWith(TEXT("_C")) || Prepared.Contains(ClassPath))continue;Prepared.Add(ClassPath);
            auto* BP=LoadObject<UDasAuthoredInterfaceBlueprint>(nullptr,*ClassPath.LeftChop(2),nullptr,LOAD_NoWarn);if(!BP || BP->StableId.IsEmpty()){UE_LOG(LogTemp,Error,TEXT("DAS_EXPORT_FAILED: Expected authored interface Blueprint"));return 1;}
            // BPI signature UFunctions are reconstructed by the editor compiler on load.
            FKismetEditorUtilities::CompileBlueprint(BP);FBlueprintCompilationManager::FlushCompilationQueueAndReinstance();if(BP->Status==BS_Error){UE_LOG(LogTemp,Error,TEXT("DAS_EXPORT_FAILED: Authored interface compilation failed"));return 1;}
        }}
    }
    if (!FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime")).ExportBindings(Selection, Output, Error))
    { UE_LOG(LogTemp, Error, TEXT("DAS_EXPORT_FAILED: %s"), *Error); return 1; }
    UE_LOG(LogTemp, Display, TEXT("DAS_EXPORT_RESULT=PASS")); return 0;
}
