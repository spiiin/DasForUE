#include "DasAssetLibrary.h"
#include "Misc/PackageName.h"
#include "UObject/SoftObjectPath.h"

UObject* UDasAssetLibrary::LoadOptionalAsset(const FString& AssetPath)
{
    if (!IsInGameThread() || !AssetPath.StartsWith(TEXT("/Game/")))
        return nullptr;
    const FSoftObjectPath Path(AssetPath);
    const FString Package = Path.GetLongPackageName();
    if (!Path.IsValid() || !Path.GetSubPathString().IsEmpty() ||
        !Package.StartsWith(TEXT("/Game/")) || !FPackageName::IsValidLongPackageName(Package))
        return nullptr;
    if (auto* Existing = Path.ResolveObject())
        return Existing;
    if (!FPackageName::DoesPackageExist(Package))
        return nullptr;
    return StaticLoadObject(UObject::StaticClass(), nullptr, *AssetPath, nullptr, LOAD_NoWarn | LOAD_Quiet);
}
