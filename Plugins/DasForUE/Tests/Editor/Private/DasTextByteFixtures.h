#pragma once
#include "DasTextByteContainerFixtures.h"
#include "DasTypeService.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "Modules/ModuleManager.h"
#include "DasDSLService.h"
#include "DasStaticLibraryProbe.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"
inline bool PrepareDasTextByteFixtures(const FString& Root, ADasStaticLibraryProbe* Carrier,
                                       TArray<UBlueprint*>& Assets, FString& Error, bool SaveAssets)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    for (const auto* File :
         {TEXT("dsl_text_byte_bad_byte_range.das"), TEXT("dsl_text_byte_bad_byte_negative.das"),
                                                         TEXT("dsl_text_byte_bad_text_default.das"),
                                                              TEXT("dsl_text_byte_bad_text_identity.das")})
    {
        FDasAuthoringModule Model;
        FString Issue;
        if (Runtime.ReadAuthoringScript(Runtime.ScriptPath(File), Model, Issue) || Issue.IsEmpty())
        {
            Error = TEXT("Accepted invalid authored byte/text source: ")+ FString(File);
            return false;
        }
    }
    UE_LOG(LogTemp, Display, TEXT("PASS authored_byte_text_reject_out_of_range_and_flattened_defaults"));
    TArray<UObject*> Types;
    if (!FDasTypeService::Apply(TEXT("ue_text_byte_types.das"), TEXT("/Game/DasTextByte/Types"), Types, Error))
        return false;
    UBlueprint* BP = nullptr;
    if (!FDasDSLService::Apply(TEXT("dsl_text_byte.das"), Root + TEXT("/BP_TextByte"),
                                                                      TEXT("dsl_text_byte.TextByteActor"), BP, Error) ||
                                                                      !FDasDSLService::Reimport(BP, Error))
        return false;
    Carrier->TextByteClass = BP->GeneratedClass;
    Assets.Add(BP);
    if (SaveAssets)
    {
        Types.Add(BP);
        for (auto* Asset : Types)
        {
            FSavePackageArgs Save;
            Save.TopLevelFlags = RF_Public | RF_Standalone;
            const FString File = FPackageName::LongPackageNameToFilename(Asset->GetOutermost()->GetName(),
                                                                         FPackageName::GetAssetPackageExtension());
            IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
            if (!UPackage::SavePackage(Asset->GetOutermost(), Asset, *File, Save))
            {
                Error = TEXT("Cannot save authored text/byte fixture");
                return false;
            }
        }
    }
    return PrepareDasTextByteContainerFixtures(Root, Carrier, Assets, Error, SaveAssets);
}
