#include "Modules/ModuleManager.h"
#include "DasActor.h"
#include "Engine/Blueprint.h"
#include "Misc/OutputDevice.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformMisc.h"
#include "Misc/CommandLine.h"
#include "Editor.h"
#include "Misc/Parse.h"
#include "UObject/Class.h"
#if !defined(DAS_WITH_TEST_PROBES) || DAS_WITH_TEST_PROBES
#error Clean editor must not include runtime fixtures.
#endif
class FDasEditorIsolationModule : public IModuleInterface
{
    FDelegateHandle Init;

public:
    virtual void StartupModule() override
    {
        if (FParse::Param(FCommandLine::Get(), TEXT("DasEditorIsolation")))
            Init = FEditorDelegates::OnEditorInitialized.AddStatic(&FDasEditorIsolationModule::Verify);
    }
    virtual void ShutdownModule() override { FEditorDelegates::OnEditorInitialized.Remove(Init); }
    static void Verify(double Duration)
    {
        if (!FParse::Param(FCommandLine::Get(), TEXT("DasEditorIsolation")))
            return;
        FModuleManager::LoadModuleChecked<IModuleInterface>(TEXT("DasEditor"));
        bool Good = ADasActor::StaticClass() && IConsoleManager::Get().FindConsoleObject(TEXT("Das.DSL.Apply"));
        for (const TCHAR* Name :
             {TEXT("DasExportCommandlet"), TEXT("DasSourceCatalogCommandlet"),
                                                TEXT("DasAssetCheckCommandlet"), TEXT("DasNativeGameplayCommandlet") })
            Good &= FindObject<UClass>(nullptr, *(FString(TEXT("/Script/DasEditor.")) + Name)) != nullptr;
        for (const TCHAR* Name :
             {TEXT("DasSmokeCommandlet"), TEXT("DasAuthoringCommandlet"), TEXT("DasReloadCommandlet"),
                                                                               TEXT("DasMethodValuesCommandlet") })
            Good &= FindObject<UClass>(nullptr, *(FString(TEXT("/Script/DasEditor.")) + Name)) == nullptr;
        Good &= FindObject<UClass>(nullptr, TEXT("/Script/DasRuntime.DasProbeActor")) == nullptr;
        Good &= IConsoleManager::Get().ProcessUserConsoleInput(TEXT("Das.DSL.Apply clean_editor.das /Game/EditorIsolation/BP_Clean cleaneditor.CleanActor"), *GLog, nullptr);
        auto* BP = FindObject<UBlueprint>(nullptr, TEXT("/Game/EditorIsolation/BP_Clean.BP_Clean"));
        Good &= BP && BP->ParentClass == ADasActor::StaticClass() && BP->GeneratedClass &&
                BP->GeneratedClass->FindFunctionByName(TEXT("Add")) != nullptr;
        UE_LOG(LogTemp, Display, TEXT("DAS_EDITOR_ISOLATION=%s"), Good ? TEXT("PASS") : TEXT("FAIL"));
        FPlatformMisc::RequestExitWithStatus(false, Good ? 0 : 1);
    }
};
IMPLEMENT_MODULE(FDasEditorIsolationModule, DasEditorIsolation)
