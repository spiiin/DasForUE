#include "Modules/ModuleManager.h"
#include "Editor.h"
#include "DasActor.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "HAL/FileManager.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/OutputDevice.h"
#include "UObject/SavePackage.h"
class FCleanGameplayEditorModule : public IModuleInterface
{
    FDelegateHandle Ready;

public:
    void StartupModule() override
    {
        if (FParse::Param(FCommandLine::Get(), TEXT("PrepareCleanGameplay")))
            Ready = FEditorDelegates::OnEditorInitialized.AddStatic(&Prepare);
    }
    void ShutdownModule() override { FEditorDelegates::OnEditorInitialized.Remove(Ready); }
    static bool Save(UObject* Object, bool Map = false)
    {
        const FString File = FPackageName::LongPackageNameToFilename(Object->GetOutermost()->GetName(),
                                                                     Map ? FPackageName::GetMapPackageExtension()
                                                                         : FPackageName::GetAssetPackageExtension());
        IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
        FSavePackageArgs Args;
        Args.TopLevelFlags = RF_Public | RF_Standalone;
        return UPackage::SavePackage(Object->GetOutermost(), Object, *File, Args);
    }
    static void Prepare(double Duration)
    {
        bool Good = !FindObject<UClass>(nullptr, TEXT("/Script/DasEditor.DasSmokeCommandlet")) &&
                                        !FindObject<UClass>(nullptr, TEXT("/Script/DasRuntime.DasProbeActor"));
        Good &= IConsoleManager::Get().ProcessUserConsoleInput(
            TEXT("Das.DSL.Apply collectible.das /Game/CleanGameplay/BP_Collectible cleangameplay.Collectible"), *GLog,
                 nullptr);
        auto* BP = FindObject<UBlueprint>(nullptr, TEXT("/Game/CleanGameplay/BP_Collectible.BP_Collectible"));
        Good &= BP && BP->ParentClass == ADasActor::StaticClass() && Save(BP);
        const TCHAR* MapPackage = TEXT("/Game/CleanGameplay/Arena");
        UWorld* World = nullptr;
        if (FPackageName::DoesPackageExist(MapPackage))
            World = LoadObject<UWorld>(nullptr, TEXT("/Game/CleanGameplay/Arena.Arena"));
        else
            World = UWorld::CreateWorld(EWorldType::Editor, false, TEXT("Arena"), CreatePackage(MapPackage));
        if (World)
        {
            World->SetFlags(RF_Public | RF_Standalone);
            World->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
        }
        Good &= World && Save(World, true);
        UE_LOG(LogTemp, Display, TEXT("DAS_CLEAN_GAMEPLAY_PREPARE=%s"), Good ? TEXT("PASS"): TEXT("FAIL"));
        FPlatformMisc::RequestExitWithStatus(false, Good ? 0 : 1);
    }
};
IMPLEMENT_MODULE(FCleanGameplayEditorModule, CleanGameplayEditor)
