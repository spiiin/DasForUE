#pragma once
#include "DasDSLService.h"
#include "DasAuthoring.h"
#include "DasRuntime.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
inline bool CreateDasMontageFixture(const FString& Root, FString& Error)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FString Rejected;
    if (Runtime.ValidateIntMethod(Runtime.ScriptPath(TEXT("montage_bad_callback.das")), Rejected) ||
                                  !Rejected.Contains(TEXT("Montage callback requires")))
    {
        Error = TEXT("Invalid montage callback was not rejected at authoring: ")+ Rejected;
        return false;
    }
    UE_LOG(LogTemp, Display, TEXT("PASS montage_bad_callback_rejected_at_authoring"));
    UBlueprint* BP = nullptr;
    if (!FDasDSLService::Apply(TEXT("dsl_montage_actor.das"),
                                    Root + TEXT("/BP_MontageActor"), TEXT("montageexample.MontageActor"), BP, Error))
        return false;
    auto Save = [](UObject* O, bool Map)
    {
        const FString File = FPackageName::LongPackageNameToFilename(O->GetOutermost()->GetName(),
                                                                     Map ? FPackageName::GetMapPackageExtension()
                                                                         : FPackageName::GetAssetPackageExtension());
        IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
        FSavePackageArgs Args;
        Args.TopLevelFlags = RF_Public | RF_Standalone;
        return UPackage::SavePackage(O->GetOutermost(), O, *File, Args);
    };
    if (!Save(BP, false))
    {
        Error = TEXT("Cannot save Montage actor");
        return false;
    }
    auto* W = UWorld::CreateWorld(EWorldType::Editor, false,
                                  TEXT("MontageMap"), CreatePackage(*(Root + TEXT("/MontageMap"))));
    W->SetFlags(RF_Public | RF_Standalone);
    W->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("DAS_Montage");
    auto* A = W->SpawnActor<AActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
    const bool Good = A && Save(W, true);
    W->DestroyWorld(false);
    if (!Good)
        Error = TEXT("Cannot save Montage map");
    return Good;
}
