#include "DasDataAssetService.h"
#include "DasDataAssets.h"
#include "DasScriptHosts.h"
#include "Engine/Blueprint.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/PackageName.h"
#include "UObject/Package.h"
#include "Editor.h"
bool FDasDataAssetService::Create(UBlueprint* Blueprint, const FString& Package, UDataAsset*& Out, FString& Error)
{
    Out = nullptr;
    Error.Reset();
    if (!IsInGameThread() || (GEditor && GEditor->PlayWorld) || !IsValid(Blueprint) || Blueprint->Status == BS_Error ||
        !Blueprint->GeneratedClass ||
        Blueprint->GeneratedClass->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists) ||
        (!Blueprint->GeneratedClass->IsChildOf(UDasDataAsset::StaticClass()) &&
         !Blueprint->GeneratedClass->IsChildOf(UDasPrimaryDataAsset::StaticClass())) ||
        DasHostIdentity(Blueprint->GeneratedClass->GetDefaultObject()).IsEmpty())
    {
        Error = TEXT("Expected a compiled authored DataAsset/PrimaryDataAsset Blueprint outside PIE");
        return false;
    }
    if (!Package.StartsWith(TEXT("/Game/")) || !FPackageName::IsValidLongPackageName(Package))
    {
        Error = TEXT("Expected /Game/Folder/AssetName package");
        return false;
    }
    if (FPackageName::DoesPackageExist(Package) || FindPackage(nullptr, *Package))
    {
        Error = TEXT("Asset package already exists; refusing to overwrite");
        return false;
    }
    auto* P = CreatePackage(*Package);
    auto* Asset = NewObject<UDataAsset>(P, Blueprint->GeneratedClass, *FPackageName::GetLongPackageAssetName(Package),
                                        RF_Public | RF_Standalone | RF_Transactional);
    if (auto* Primary = Cast<UPrimaryDataAsset>(Asset))
        Primary->UpdateAssetBundleData();
    FAssetRegistryModule::AssetCreated(Asset);
    Asset->MarkPackageDirty();
    Out = Asset;
    return true;
}
