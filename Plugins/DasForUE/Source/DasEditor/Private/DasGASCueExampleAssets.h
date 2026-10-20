#pragma once
#include "NiagaraSystem.h"
#include "UObject/SavePackage.h"
#include "UObject/Package.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

inline bool SaveDasCueExampleAsset(UObject* Object)
{
    const FString File = FPackageName::LongPackageNameToFilename(Object->GetOutermost()->GetName(),
                                                                 FPackageName::GetAssetPackageExtension());
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public | RF_Standalone;
    return UPackage::SavePackage(Object->GetOutermost(), Object, *File, Args);
}

// Copies are confined to the demo folder; existing project copies are reused.
inline UNiagaraSystem* PrepareDasCueExampleSystem(const FString& Folder, const TCHAR* TemplateName, const TCHAR* Name)
{
    const FString Path = Folder + TEXT("/") + Name;
    if (auto* Existing = LoadObject<UNiagaraSystem>(nullptr, *(Path + TEXT(".") + Name), nullptr, LOAD_NoWarn))
    {
        Existing->WaitForCompilationComplete(true, false);
        return Existing;
    }
    const FString Source =
        FString(TEXT("/Niagara/DefaultAssets/Templates/Systems/")) + TemplateName + TEXT(".") + TemplateName;
    auto* Template = LoadObject<UNiagaraSystem>(nullptr, *Source);
    if (!Template)
        return static_cast<UNiagaraSystem*>(nullptr);
    auto* System = DuplicateObject<UNiagaraSystem>(Template, CreatePackage(*Path), Name);
    System->SetFlags(RF_Public | RF_Standalone);
    System->WaitForCompilationComplete(true, false);
    return SaveDasCueExampleAsset(System) ? System : nullptr;
}
