#include "DasClassCallbacksCommandlet.h"
#include "DasDSLService.h"
#include "DasInterfaceService.h"
#include "DasProbeActor.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"
UDasClassCallbacksCommandlet::UDasClassCallbacksCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasClassCallbacksCommandlet::Main(const FString& Params)
{
    FString Id;
    FParse::Value(*Params, TEXT("ProbeId="), Id);
    FGuid Guid;
    if (!FGuid::ParseExact(Id, EGuidFormats::Digits, Guid))
        return 1;
    auto Save = [](UObject* O)
    {
        const auto File = FPackageName::LongPackageNameToFilename(
            O->GetOutermost()->GetName(),
            O->IsA<UWorld>() ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension());
        IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
        FSavePackageArgs Args;
        Args.TopLevelFlags = RF_Public | RF_Standalone;
        return UPackage::SavePackage(O->GetOutermost(), O, *File, Args);
    };
    FString Error;
    TArray<UBlueprint*> Interfaces;
    if (!FDasInterfaceService::Apply(TEXT("dsl_class_callback_interface.das"), TEXT("/Game/DasClassCallbacks/Types"),
                                                                                    Interfaces, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("Class interface export: %s"), *Error);
        return 1;
    }
    TArray<TStrongObjectPtr<UBlueprint>> Pins;
    for (auto* Interface : Interfaces)
    {
        Pins.Emplace(Interface);
        if (!Save(Interface))
            return 1;
    }
    const FString Root = TEXT("/Game/DasClassCallbacks/")+ Id;
    UBlueprint* BP = nullptr;
    if (!FDasDSLService::Apply(TEXT("dsl_class_callbacks.das"),
                                    Root + TEXT("/BP_ClassCallbacks"),
                                                TEXT("dsl_class_callbacks.ClassCallbackActor"), BP, Error) || !Save(BP))
    {
        UE_LOG(LogTemp, Error, TEXT("Class actor export: %s"), *Error);
        return 1;
    }
    TStrongObjectPtr<UBlueprint> Owner(BP);
    auto* World = UWorld::CreateWorld(EWorldType::Editor, false, TEXT("Map"), CreatePackage(*(Root + TEXT("/Map"))));
    if (!World)
        return 1;
    World->SetFlags(RF_Public | RF_Standalone);
    World->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("ClassCallbacks");
    auto* Actor = World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
    const bool Good = Actor && Save(World);
    World->DestroyWorld(false);
    if (!Good)
        return 1;
    UE_LOG(LogTemp, Display, TEXT("DAS_CLASS_CALLBACKS_EXPORT=PASS map=%s/Map"), *Root);
    return 0;
}
