#pragma once
#include "DasTextByteTimerFixtures.h"
#include "DasInterfaceService.h"
#include "DasDSLService.h"
#include "DasStaticLibraryProbe.h"
#include "Engine/Blueprint.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"
inline bool PrepareDasInterfaceReentryFixtures(const FString& Root, ADasStaticLibraryProbe* Carrier,
                                               TArray<UBlueprint*>& Assets, FString& Error, bool SaveAssets)
{
    TArray<UBlueprint*> Types;
    if (!FDasInterfaceService::Apply(TEXT("ue_interface_reentry_contract.das"), TEXT("/Game/DasInterfaceReentry/Types"),
                                                                                     Types, Error))
        return false;
    UBlueprint *Actor = nullptr, *Component = nullptr;
    if (!FDasDSLService::Apply(
            TEXT("dsl_interface_reentry_actor.das"),
                 Root + TEXT("/BP_InterfaceReentryActor"),
                             TEXT("dsl_interface_reentry_actor.InterfaceReentryActor"), Actor, Error) ||
                             !FDasDSLService::Apply(
                                 TEXT("dsl_interface_reentry_component.das"),
                                      Root + TEXT("/BP_InterfaceReentryComponent"),
                                                  TEXT("dsl_interface_reentry_component.InterfaceReentryComponent"),
                                                       Component, Error))
        return false;
    Types.Add(Actor);
    Types.Add(Component);
    Assets.Append(Types);
    Carrier->InterfaceReentryActorClass = Actor->GeneratedClass;
    Carrier->InterfaceReentryComponentClass = Component->GeneratedClass;
    if (SaveAssets)
        for (auto* Asset : Types)
        {
            FSavePackageArgs Save;
            Save.TopLevelFlags = RF_Public | RF_Standalone;
            const FString File = FPackageName::LongPackageNameToFilename(Asset->GetOutermost()->GetName(),
                                                                         FPackageName::GetAssetPackageExtension());
            IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
            if (!UPackage::SavePackage(Asset->GetOutermost(), Asset, *File, Save))
            {
                Error = TEXT("Cannot save interface reentry fixture");
                return false;
            }
        }
    return PrepareDasTextByteTimerFixtures(Root, Carrier, Assets, Error, SaveAssets);
}
