#include "DasAssetCheckCommandlet.h"
#include "DasAssetPreflight.h"
#include "Engine/DataAsset.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
UDasAssetCheckCommandlet::UDasAssetCheckCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasAssetCheckCommandlet::Main(const FString& Params)
{
    FString Path, File;
    if (!FParse::Value(*Params, TEXT("Asset="), Path))
        return 2;
    if (!Path.Contains(TEXT(".")) && FPackageName::IsValidLongPackageName(Path))
        Path += TEXT(".")+ FPackageName::GetLongPackageAssetName(Path);
    auto Report = FDasAssetPreflight::Inspect(
        FPackageName::IsValidObjectPath(Path) ? LoadObject<UDataAsset>(nullptr, *Path, nullptr, LOAD_NoWarn) : nullptr);
    if (Report.Asset.IsEmpty())
        Report.Asset = Path;
    if (FParse::Value(*Params, TEXT("Report="), File) &&
                      !FFileHelper::SaveStringToFile(Report.ToJson(), *File,
                                                     FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return 2;
    for (const auto& I : Report.Issues)
        UE_LOG(LogTemp, Display, TEXT("DAS_ASSET_CHECK %s %s %s: %s"), *I.Severity, *I.Code, *I.Resource, *I.Message);
    UE_LOG(LogTemp, Display,
           TEXT("DAS_ASSET_CHECK_RESULT=%s asset=%s packaged_verified=false"),
                Report.HasErrors() ? TEXT("FAIL"): TEXT("PASS"), *Report.Asset);
    return Report.HasErrors() ? 1 : 0;
}
