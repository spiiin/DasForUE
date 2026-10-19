#pragma once
#include "CoreMinimal.h"
class UDataAsset;
struct FDasAssetIssue
{
    FString Severity, Code, Resource, Message;
};
struct DASEDITOR_API FDasAssetPreflightReport
{
    FString Asset, PrimaryId;
    TArray<FString> Packages, Scripts;
    TArray<FDasAssetIssue> Issues;
    bool HasErrors() const;
    bool HasIssue(const FString& Code) const;
    FString ToJson() const;
};
class DASEDITOR_API FDasAssetPreflight
{
public:
    // Diagnostic only: no save, rescan, configuration changes or load-state mutation.
    static FDasAssetPreflightReport Inspect(UDataAsset* Asset);
};
