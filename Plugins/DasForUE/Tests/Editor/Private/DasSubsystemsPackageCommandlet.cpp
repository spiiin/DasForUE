#include "DasSubsystemsPackageCommandlet.h"
#include "DasDSLService.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "HAL/FileManager.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"
UDasSubsystemsPackageCommandlet::UDasSubsystemsPackageCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasSubsystemsPackageCommandlet::Main(const FString&)
{
    auto Save = [](UObject* Object)
    {
        const FString File = FPackageName::LongPackageNameToFilename(
            Object->GetOutermost()->GetName(),
            Object->IsA<UWorld>() ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension());
        IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
        FSavePackageArgs Args;
        Args.TopLevelFlags = RF_Public | RF_Standalone;
        return UPackage::SavePackage(Object->GetOutermost(), Object, *File, Args);
    };
    const FString Root = TEXT("/Game/DasSubsystems/Packaged");
    FString Error;
    UBlueprint *WorldBP = nullptr, *SessionBP = nullptr;
    if (!FDasDSLService::Apply(TEXT("dsl_world_subsystem.das"),
            Root + TEXT("/BP_Missions"), TEXT("subsystemdemo.MissionService"), WorldBP, Error) ||
                        !FDasDSLService::Apply(TEXT("dsl_gameinstance_subsystem.das"),
                                                    Root + TEXT("/BP_Session"), TEXT("subsystemdemo.SessionService"),
                                                                                     SessionBP, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("DAS_SUBSYSTEMS_EXPORT=FAIL %s"), *Error);
        return 1;
    }
    TStrongObjectPtr<UBlueprint> KeepWorld(WorldBP), KeepSession(SessionBP);
    if (!Save(WorldBP) || !Save(SessionBP))
        return 1;
    // No actors or hard references to either service: config is the only runtime class source.
    const FString MapPath = Root + TEXT("/Map");
    auto* Map = FPackageName::DoesPackageExist(MapPath) ? LoadObject<UWorld>(nullptr, *MapPath) : nullptr;
    if (!Map)
        Map = UWorld::CreateWorld(EWorldType::Editor, false, TEXT("Map"), CreatePackage(*MapPath));
    if (!Map)
        return 1;
    Map->GetOutermost()->FullyLoad();
    Map->SetFlags(RF_Public | RF_Standalone);
    Map->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
    const bool Saved = Save(Map);
    Map->DestroyWorld(false);
    if (!Saved)
        return 1;
    UE_LOG(LogTemp, Display, TEXT("DAS_SUBSYSTEMS_EXPORT=PASS map=%s/Map"), *Root);
    return 0;
}
