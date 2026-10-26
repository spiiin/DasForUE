#pragma once
#include "DasStaticLibraryProbe.h"
#include "DasDSLService.h"
#include "DasPureFunctionTestGraph.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"

// One fixture definition for both PIE and cook; no editor dependency in the probe.
inline bool PrepareDasInstanceDefaultGameFixtures(const FString& Root, ADasStaticLibraryProbe* Carrier,
                                                  TArray<UBlueprint*>& Assets, FString& Error, bool SaveAssets)
{
    UBlueprint *Actor = nullptr, *Component = nullptr, *Child = nullptr;
    if (!FDasDSLService::Apply(TEXT("dsl_defaults_actor.das"),
            Root + TEXT("/BP_DefaultActor"), TEXT("dsl_defaults_actor.DefaultActor"), Actor, Error) ||
                        !FDasDSLService::Apply(TEXT("dsl_defaults_component.das"),
                            Root + TEXT("/BP_DefaultComponent"),
                                        TEXT("dsl_defaults_component.DefaultComponent"), Component, Error) ||
                                        !FDasDSLService::Apply(TEXT("dsl_defaults_child.das"),
                                                                    Root + TEXT("/BP_DefaultChild"),
                                                                                TEXT("dsl_defaults_child.DefaultChild"),
                                                                                     Child, Error))
        return false;
    auto* ActorConsumer = CreateDasPureConsumer(
        Actor->GeneratedClass, CreatePackage(*(Root + TEXT("/BP_InstanceDefaultActorConsumer"))),
                                             TEXT("BP_InstanceDefaultActorConsumer"), Error, TEXT("DefaultAdd"), true);
    auto* ComponentConsumer = CreateDasPureConsumer(
        Component->GeneratedClass, CreatePackage(*(Root + TEXT("/BP_InstanceDefaultComponentConsumer"))),
                                                 TEXT("BP_InstanceDefaultComponentConsumer"), Error,
                                                      TEXT("DefaultAdd"), true);
    if (!ActorConsumer || !ComponentConsumer)
        return false;
    Assets = {Actor, Component, Child, ActorConsumer, ComponentConsumer};
    if (SaveAssets)
        for (auto* Asset : Assets)
        {
            const FString File = FPackageName::LongPackageNameToFilename(Asset->GetOutermost()->GetName(),
                                                                         FPackageName::GetAssetPackageExtension());
            IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
            FSavePackageArgs Args;
            Args.TopLevelFlags = RF_Public | RF_Standalone;
            if (!UPackage::SavePackage(Asset->GetOutermost(), Asset, *File, Args))
            {
                Error = TEXT("Cannot save instance-default dependency ")+ File;
                return false;
            }
        }
    Carrier->DefaultActorClass = ActorConsumer->GeneratedClass;
    Carrier->DefaultComponentClass = ComponentConsumer->GeneratedClass;
    Carrier->DefaultChildClass = Child->GeneratedClass;
    return true;
}
