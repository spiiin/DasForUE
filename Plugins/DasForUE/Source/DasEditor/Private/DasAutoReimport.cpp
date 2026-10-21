#include "DasAutoReimport.h"
#include "DasAuthoring.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DirectoryWatcherModule.h"
#include "IDirectoryWatcher.h"
#include "Engine/Blueprint.h"
#include "Framework/Notifications/NotificationManager.h"
#include "HAL/FileManager.h"
#include "HAL/IConsoleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Logging/MessageLog.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "UObject/MetaData.h"
#include "UObject/UObjectIterator.h"
#include "Widgets/Notifications/SNotificationList.h"

namespace
{
TAutoConsoleVariable<int32> CVarAutoReimport(TEXT("Das.DSL.AutoReimport"), 1,
    TEXT("Reimport DSL Blueprints when their .das source is saved (1, default) or only with Das.DSL.Reimport (0)."));

// Editors often write a file in several steps; changes are collected and handled once after this delay.
constexpr float SettleSeconds = 0.5f;

void Notify(const FString& Text, bool bSuccess)
{
    FNotificationInfo Info(FText::FromString(Text));
    Info.ExpireDuration = bSuccess ? 4.f : 8.f;
    Info.bUseSuccessFailIcons = true;
    if (auto Item = FSlateNotificationManager::Get().AddNotification(Info))
        Item->SetCompletionState(bSuccess ? SNotificationItem::CS_Success : SNotificationItem::CS_Fail);
}
FString FullPath(const FString& Path)
{
    FString Full = FPaths::ConvertRelativePathToFull(Path);
    FPaths::NormalizeFilename(Full);
    return Full;
}
}

void FDasAutoReimport::Start()
{
    // The same folders the runtime resolves script names in (FDasRuntimeModule::ScriptPath).
    TArray<FString> Folders{FPaths::ProjectContentDir() / TEXT("Das/Scripts")};
    if (auto Plugin = IPluginManager::Get().FindPlugin(TEXT("DasForUE")))
        Folders.Add(Plugin->GetContentDir() / TEXT("Das/Scripts"));
    auto* Watcher = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>(TEXT("DirectoryWatcher")).Get();
    for (const FString& Folder : Folders)
    {
        const FString Full = FullPath(Folder);
        FDelegateHandle Handle;
        if (IFileManager::Get().DirectoryExists(*Full) &&
            Watcher->RegisterDirectoryChangedCallback_Handle(
                Full, IDirectoryWatcher::FDirectoryChanged::CreateRaw(this, &FDasAutoReimport::OnScriptsChanged),
                Handle))
            Watches.Emplace(Full, Handle);
    }
}
void FDasAutoReimport::Stop()
{
    if (auto* Module = FModuleManager::GetModulePtr<FDirectoryWatcherModule>(TEXT("DirectoryWatcher")))
        if (auto* Watcher = Module->Get())
            for (const auto& Watch : Watches)
                Watcher->UnregisterDirectoryChangedCallback_Handle(Watch.Key, Watch.Value);
    Watches.Reset();
    if (Ticker.IsValid())
        FTSTicker::GetCoreTicker().RemoveTicker(Ticker);
    Ticker.Reset();
    Pending.Reset();
}
void FDasAutoReimport::OnScriptsChanged(const TArray<FFileChangeData>& Changes)
{
    for (const auto& Change : Changes)
        if (Change.Action != FFileChangeData::FCA_Removed && FPaths::GetExtension(Change.Filename) == TEXT("das"))
            Pending.Add(FullPath(Change.Filename));
    if (!Pending.IsEmpty() && !Ticker.IsValid())
        Ticker = FTSTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateRaw(this, &FDasAutoReimport::ProcessPending), SettleSeconds);
}
bool FDasAutoReimport::ProcessPending(float)
{
    Ticker.Reset();
    const TSet<FString> Saved = MoveTemp(Pending);
    Pending.Reset();
    // Automation tests rewrite sources and apply them themselves.
    if (CVarAutoReimport.GetValueOnGameThread() == 0 || GIsAutomationTesting)
        return false;
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    // Only loaded Blueprints: those of the open level and editors. Others still run their last applied
    // declaration and report the mismatch when they play.
    TArray<UBlueprint*> Bound;
    for (TObjectIterator<UBlueprint> It; It; ++It)
    {
        UBlueprint* Blueprint = *It;
        if (!Blueprint->GeneratedClass || Blueprint->HasAnyFlags(RF_ClassDefaultObject | RF_Transient))
            continue;
        const FString Source = Blueprint->GetOutermost()->GetMetaData().GetValue(Blueprint, TEXT("DasDSLSource"));
        if (!Source.IsEmpty() &&
            Saved.Contains(FullPath(FPaths::IsRelative(Source) ? Runtime.ScriptPath(Source) : Source)))
            Bound.Add(Blueprint);
    }
    for (UBlueprint* Blueprint : Bound)
    {
        const FString Name = Blueprint->GetName();
        const FString Before = Blueprint->GetOutermost()->GetMetaData().GetValue(Blueprint, TEXT("DasDSLModel"));
        FString Error;
        // Body-only edits are already live through hot reload. Reimport would still run the defaults
        // preflight, which is refused during PIE and for actors with components.
        const FString Source = Blueprint->GetOutermost()->GetMetaData().GetValue(Blueprint, TEXT("DasDSLSource"));
        FDasAuthoringModule Model;
        FString Json;
        if (Runtime.ReadAuthoringScript(Runtime.ScriptPath(Source), Model, Error) && Model.ToJson(Json, Error) &&
            Json == Before)
            continue;
        if (!FDasDSLService::Reimport(Blueprint, Error))
        {
            const FString Text = FString::Printf(TEXT("daScript: %s was not updated: %s"), *Name, *Error);
            UE_LOG(LogTemp, Warning, TEXT("DAS_DSL_AUTO_REIMPORT rejected %s: %s"), *Name, *Error);
            FMessageLog(TEXT("BlueprintLog")).Error(FText::FromString(Text));
            Notify(Text, false);
            continue;
        }
        if (Blueprint->GetOutermost()->GetMetaData().GetValue(Blueprint, TEXT("DasDSLModel")) != Before)
        {
            UE_LOG(LogTemp, Display, TEXT("DAS_DSL_AUTO_REIMPORT applied %s"), *Name);
            Notify(FString::Printf(TEXT("daScript: %s updated from its script"), *Name), true);
        }
    }
    return false;
}
