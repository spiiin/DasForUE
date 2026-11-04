#include "DasAutoReimport.h"
#include "DasAuthoring.h"
#include "DasSourceCatalog.h"
#include "Misc/ConfigCacheIni.h"
#include "DasDSLService.h"
#include "DasGASAssetService.h"
#include "DasTypeRegistry.h"
#include "DasTypeService.h"
#include "StructUtils/UserDefinedStruct.h"
#include "DasRuntime.h"
#include "Features/IModularFeatures.h"
#include "Misc/ScopeExit.h"
#include "Misc/ScopedSlowTask.h"
#include "DirectoryWatcherModule.h"
#include "Editor.h"
#include "IDirectoryWatcher.h"
#include "Components/ActorComponent.h"
#include "Engine/Blueprint.h"
#include "Framework/Notifications/NotificationManager.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "HAL/IConsoleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Logging/MessageLog.h"
#include "DasDiagnosticEditor.h"
#include "Logging/TokenizedMessage.h"
#include "Misc/MessageDialog.h"
#include "Misc/PackageName.h"
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

bool UpdateAutomaticCatalog(FString& Error)
{
    bool Enabled = false;
    GConfig->GetBool(TEXT("DasForUE.SourceCatalog"), TEXT("AutoGenerate"), Enabled, GEngineIni);
    return !Enabled ||
           FDasSourceCatalog::UpdateDeclarationInventory(FPaths::ProjectContentDir() / TEXT("Das/Scripts"), Error);
}
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
    if (bStarted)
        return;
    bStarted = true;
    IModularFeatures::Get().RegisterModularFeature(IPIEAuthorizer::GetModularFeatureName(), this);
    // The same folders the runtime resolves script names in (FDasRuntimeModule::ScriptPath).
    TArray<FString> Folders{FPaths::ProjectContentDir() / TEXT("Das/Scripts")};
    IFileManager::Get().MakeDirectory(*Folders[0], true);
    if (auto Plugin = IPluginManager::Get().FindPlugin(TEXT("DasForUE")))
    {
        Folders.Add(Plugin->GetContentDir() / TEXT("Das/Scripts"));
        Folders.Add(Plugin->GetContentDir() / TEXT("Das/Runtime"));
    }
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
    // Persistent calls stop re-reading their sources on every call; saves are reported from OnScriptsChanged.
    if (!Watches.IsEmpty())
        FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime")).SetScriptWatcherActive(true);
    EndPIEHandle = FEditorDelegates::EndPIE.AddRaw(this, &FDasAutoReimport::OnEndPIE);
    PreBeginPIEHandle = FEditorDelegates::PreBeginPIE.AddRaw(this, &FDasAutoReimport::OnPreBeginPIE);
    BeginPIEHandle = FEditorDelegates::BeginPIE.AddLambda(
        [](bool)
        { FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime")).SetEditorGameplayActive(true); });
    CancelPIEHandle = FEditorDelegates::CancelPIE.AddLambda(
        []
        { FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime")).SetEditorGameplayActive(false); });
    MapOpenedHandle = FEditorDelegates::OnMapOpened.AddRaw(this, &FDasAutoReimport::OnMapOpened);
    FString CatalogError;
    if (!UpdateAutomaticCatalog(CatalogError))
        DasDiagnosticEditor::Report(CatalogError);
    // The startup map may already be open.
    OnMapOpened(FString(), false);
}
void FDasAutoReimport::Stop()
{
    if (!bStarted)
        return;
    bStarted = false;
    IModularFeatures::Get().UnregisterModularFeature(IPIEAuthorizer::GetModularFeatureName(), this);
    if (auto* Runtime = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
        Runtime->SetEditorGameplayActive(false);
    if (auto* Module = FModuleManager::GetModulePtr<FDirectoryWatcherModule>(TEXT("DirectoryWatcher")))
        if (auto* Watcher = Module->Get())
            for (const auto& Watch : Watches)
                Watcher->UnregisterDirectoryChangedCallback_Handle(Watch.Key, Watch.Value);
    if (!Watches.IsEmpty())
        if (auto* Runtime = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
            Runtime->SetScriptWatcherActive(false);
    Watches.Reset();
    FEditorDelegates::EndPIE.Remove(EndPIEHandle);
    FEditorDelegates::PreBeginPIE.Remove(PreBeginPIEHandle);
    FEditorDelegates::BeginPIE.Remove(BeginPIEHandle);
    FEditorDelegates::CancelPIE.Remove(CancelPIEHandle);
    FEditorDelegates::OnMapOpened.Remove(MapOpenedHandle);
    for (auto* Handle : {&Ticker, &RetryTicker, &StaleTicker})
    {
        if (Handle->IsValid())
            FTSTicker::GetCoreTicker().RemoveTicker(*Handle);
        Handle->Reset();
    }
    TArray<FString> Open;
    Confirmations.GetKeys(Open);
    for (const FString& Path : Open)
        CloseConfirmation(Path);
    Confirmed.Reset();
    Pending.Reset();
    bPendingHierarchyChange = false;
    Deferred.Reset();
    DeferredGAS.Reset();
    DeferredTypes.Reset();
    Checked.Reset();
}
void FDasAutoReimport::OnScriptsChanged(const TArray<FFileChangeData>& Changes)
{
    // Invalidate immediately, but compile once in the settled editor queue, never in a gameplay call.
    TArray<FString> Changed;
    bool bAddedOrRemoved = false;
    for (const auto& Change : Changes)
    {
        const FString Extension = FPaths::GetExtension(Change.Filename).ToLower();
        if (!Extension.IsEmpty() && Extension != TEXT("das") && Extension != TEXT("json"))
            continue;
        Changed.Add(FullPath(Change.Filename));
        bAddedOrRemoved |= Change.Action != FFileChangeData::FCA_Modified;
    }
    if (!Changed.IsEmpty())
        FModuleManager::LoadModuleChecked<FDasRuntimeModule>(
            TEXT("DasRuntime")).NotifyScriptsChanged(Changed, bAddedOrRemoved);
    bPendingHierarchyChange |= bAddedOrRemoved;
    for (const auto& File : Changed)
        Pending.Add(File);
    if (!Pending.IsEmpty() && !Ticker.IsValid())
        Ticker = FTSTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateRaw(this, &FDasAutoReimport::ProcessPending), SettleSeconds);
}
bool FDasAutoReimport::ProcessPending(float)
{
    FDasEditorSourceReadScope SourcePass;
    Ticker.Reset();
    FString CatalogError;
    if (!UpdateAutomaticCatalog(CatalogError))
    {
        DasDiagnosticEditor::Report(CatalogError);
        return false;
    }
    const TSet<FString> Saved = MoveTemp(Pending);
    Pending.Reset();
    const bool Hierarchy = bPendingHierarchyChange;
    bPendingHierarchyChange = false;
    // Automation tests rewrite sources and apply them themselves.
    if (CVarAutoReimport.GetValueOnGameThread() != 0 && !GIsAutomationTesting)
        ReimportFiles(Saved);
    if (!GIsAutomationTesting)
    {
        FString Error;
        if (!PrepareSavedPrograms(Saved, Hierarchy, Error))
            DasDiagnosticEditor::Report(Error);
    }
    return false;
}
TArray<UBlueprint*> FDasAutoReimport::Bound(const TSet<FString>* Files) const
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    // Only loaded Blueprints: those of the open level and editors. Others still run their last applied
    // declaration and report the mismatch when they play.
    TArray<UBlueprint*> Result;
    for (TObjectIterator<UBlueprint> It; It; ++It)
    {
        UBlueprint* Blueprint = *It;
        if (!Blueprint->GeneratedClass || Blueprint->HasAnyFlags(RF_ClassDefaultObject | RF_Transient))
            continue;
        const FString Source = Blueprint->GetOutermost()->GetMetaData().GetValue(Blueprint, TEXT("DasDSLSource"));
        if (Source.IsEmpty())
            continue;
        bool Match =
            !Files || Files->Contains(FullPath(FPaths::IsRelative(Source) ? Runtime.ScriptPath(Source) : Source));
        // An authored child also depends on its parents' scripts (recorded in its declaration).
        if (!Match)
        {
            FDasAuthoringModule Model;
            FString Error;
            if (FDasAuthoringModule::FromJson(
                    Blueprint->GetOutermost()->GetMetaData().GetValue(Blueprint, TEXT("DasDSLModel")), Model, Error))
                for (const auto& C : Model.Classes)
                    Match |= !C.SourceScript.IsEmpty() && Files->Contains(FullPath(Runtime.ScriptPath(C.SourceScript)));
        }
        if (Match)
            Result.Add(Blueprint);
    }
    return Result;
}
void FDasAutoReimport::ReimportFiles(const TSet<FString>& Files)
{
    // Component classes first: the actors that contain them reimport against their new declaration.
    TArray<UBlueprint*> Blueprints = Bound(&Files);
    Blueprints.StableSort(
        [](const UBlueprint& A, const UBlueprint& B)
        {
            return A.GeneratedClass->IsChildOf(UActorComponent::StaticClass()) &&
                   !B.GeneratedClass->IsChildOf(UActorComponent::StaticClass());
        });
    for (UBlueprint* Blueprint : Blueprints)
        Reimport(Blueprint, false);
    for (const auto& Import : BoundGAS(&Files))
        ReimportGAS(Import, false);
    const auto Types = BoundTypes(Files);
    UE_LOG(LogTemp, Verbose, TEXT("DAS_TYPES_AUTO_REIMPORT %d struct scripts"), Types.Num());
    for (const auto& Import : Types)
        ReimportTypes(Import, false);
}
TSet<TPair<FString, FString>> FDasAutoReimport::BoundTypes(const TSet<FString>& Files) const
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    TSet<TPair<FString, FString>> Result;
    for (TObjectIterator<UUserDefinedStruct> It; It; ++It)
    {
        if (It->HasAnyFlags(RF_Transient | RF_ClassDefaultObject))
            continue;
        const FString Source = It->GetOutermost()->GetMetaData().GetValue(*It, TEXT("DasTypesSource"));
        if (Source.IsEmpty() || !Files.Contains(FullPath(Runtime.ScriptPath(Source))))
            continue;
        for (TObjectIterator<UDasTypeRegistry> Registry; Registry; ++Registry)
            if (Registry->Bindings.ContainsByPredicate([&](const auto& B) { return B.Type == *It; }))
            {
                Result.Add({Source, Registry->GetPathName()});
                break;
            }
    }
    return Result;
}
void FDasAutoReimport::ReimportTypes(const TPair<FString, FString>& Import, bool bRetry)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Key = TEXT("types:") + Import.Key + TEXT("|") + Import.Value;
    FDasReimportPlan Plan;
    FString Rules, Error;
    if (!FDasTypeService::PlanStructMigration(Import.Key, Import.Value, Plan, Rules, Error))
    {
        UE_LOG(LogTemp, Warning, TEXT("DAS_TYPES_AUTO_REIMPORT unreadable %s: %s"), *Import.Key, *Error);
        Notify(FString::Printf(TEXT("daScript: structs of %s were not updated: %s"), *Import.Key, *Error), false);
        return;
    }
    // Only struct changes; enums keep their explicit migrations.
    if (Plan.Summary().IsEmpty())
    {
        UE_LOG(LogTemp, Display, TEXT("DAS_TYPES_AUTO_REIMPORT unchanged %s"), *Import.Key);
        return;
    }
    if (GEditor && GEditor->PlayWorld)
    {
        DeferredTypes.Add(Import);
        Notify(FString::Printf(TEXT("daScript: structs of %s are updated when Play stops"), *Import.Key), false);
        return;
    }
    if (Plan.NeedsConfirmation && !Confirmed.Contains(Key))
    {
        AskConfirmation(Key, FString::Printf(TEXT("structs of %s"), *Import.Key), Plan);
        return;
    }
    // Removed fields are dropped and numeric retypes converted with exact rules (the user confirmed).
    FString RulesFile;
    if (!Rules.IsEmpty())
    {
        RulesFile = FPaths::GetBaseFilename(Import.Key) + TEXT("_auto_rules.json");
        FFileHelper::SaveStringToFile(Rules, *Runtime.ScriptPath(RulesFile));
    }
    const bool bApplied = FDasTypeService::MigrateStructFields(Import.Key, Import.Value, Error, RulesFile);
    if (!RulesFile.IsEmpty())
        IFileManager::Get().Delete(*Runtime.ScriptPath(RulesFile));
    Confirmed.Remove(Key);
    DeferredTypes.Remove(Import);
    if (!bApplied)
    {
        const FString Text = FString::Printf(TEXT("daScript: structs of %s were not updated: %s"), *Import.Key, *Error);
        UE_LOG(LogTemp, Warning, TEXT("DAS_TYPES_AUTO_REIMPORT rejected %s: %s"), *Import.Key, *Error);
        DasDiagnosticEditor::Report(Text);
        Notify(Text, false);
        return;
    }
    UE_LOG(LogTemp, Display, TEXT("DAS_TYPES_AUTO_REIMPORT applied %s (%s)"), *Import.Key, *Plan.Summary());
    Notify(FString::Printf(TEXT("daScript: structs of %s updated (%s)"), *Import.Key, *Plan.Summary()), true);
}
TSet<TPair<FString, FString>> FDasAutoReimport::BoundGAS(const TSet<FString>* Files) const
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    TSet<TPair<FString, FString>> Result;
    for (TObjectIterator<UBlueprint> It; It; ++It)
    {
        if (!It->GeneratedClass || It->HasAnyFlags(RF_ClassDefaultObject | RF_Transient))
            continue;
        auto& Meta = It->GetOutermost()->GetMetaData();
        const FString Source = Meta.GetValue(*It, TEXT("DasGASSource")),
                                             Folder = Meta.GetValue(*It, TEXT("DasGASFolder"));
        if (!Source.IsEmpty() && !Folder.IsEmpty() && (!Files || Files->Contains(FullPath(Runtime.ScriptPath(Source)))))
            Result.Add({Source, Folder});
    }
    return Result;
}
void FDasAutoReimport::ReimportGAS(const TPair<FString, FString>& Import, bool bRetry)
{
    TMap<FString, UBlueprint*> Assets;
    FString Error;
    int32 Updated = 0;
    // The assets stay dirty until the user saves them, like other reimports.
    if (!FDasGASAssetService::Apply(Import.Key, Import.Value, Assets, Error, false, &Updated))
    {
        const bool bPlaying = GEditor && GEditor->PlayWorld;
        const FString Text = bPlaying
            ? FString::Printf(TEXT("daScript: GAS assets of %s are not updated during Play; they will be updated "
                                   "when Play stops"),
                                   *Import.Key)
            : FString::Printf(TEXT("daScript: GAS assets of %s were not updated: %s"), *Import.Key, *Error);
        if (bPlaying)
            DeferredGAS.Add(Import);
        else
            DasDiagnosticEditor::Report(Text);
        UE_LOG(LogTemp, Display,
               TEXT("DAS_GAS_AUTO_REIMPORT %s %s: %s"),
                    bPlaying ? TEXT("deferred") : TEXT("rejected"), *Import.Key, *Error);
        Notify(Text, false);
        return;
    }
    DeferredGAS.Remove(Import);
    if (Updated > 0 || bRetry)
    {
        UE_LOG(LogTemp, Display, TEXT("DAS_GAS_AUTO_REIMPORT applied %s (%d assets)"), *Import.Key, Updated);
        Notify(FString::Printf(TEXT("daScript: %d GAS assets updated from %s"), Updated, *Import.Key), true);
    }
}
void FDasAutoReimport::ReimportStale()
{
    FString CatalogError;
    if (!UpdateAutomaticCatalog(CatalogError))
    {
        DasDiagnosticEditor::Report(CatalogError);
        return;
    }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    for (UBlueprint* Blueprint : Bound(nullptr))
    {
        auto& Metadata = Blueprint->GetOutermost()->GetMetaData();
        const FString Source = Metadata.GetValue(Blueprint, TEXT("DasDSLSource"));
        const FDateTime Stamp =
            IFileManager::Get().GetTimeStamp(*(FPaths::IsRelative(Source) ? Runtime.ScriptPath(Source) : Source));
        // Compiling a script to compare declarations is not free; skip what did not change since the last check.
        const FString Key = Blueprint->GetPathName();
        if (const auto* Seen = Checked.Find(Key);
            Seen && Seen->Key == Stamp && Seen->Value == Metadata.GetValue(Blueprint, TEXT("DasDSLModel")))
            continue;
        Reimport(Blueprint, false);
        Checked.Add(Key, {Stamp, Blueprint->GetOutermost()->GetMetaData().GetValue(Blueprint, TEXT("DasDSLModel"))});
    }
    for (const auto& Import : BoundGAS(nullptr))
    {
        const FDateTime Stamp = IFileManager::Get().GetTimeStamp(*Runtime.ScriptPath(Import.Key));
        const FString Key = TEXT("gas:") + Import.Key + TEXT("|") + Import.Value;
        if (const auto* Seen = Checked.Find(Key); Seen && Seen->Key == Stamp)
            continue;
        ReimportGAS(Import, false);
        Checked.Add(Key, {Stamp, FString()});
    }
}
bool FDasAutoReimport::PrepareSavedPrograms(const TSet<FString>& Files, bool bHierarchyChanged, FString& Error)
{
    return FModuleManager::LoadModuleChecked<FDasRuntimeModule>(
        TEXT("DasRuntime")).PrepareEditorChanges(Files.Array(), bHierarchyChanged, Error);
}
bool FDasAutoReimport::PrepareAllPrograms(FString& Error)
{
    const double BatchStarted = FPlatformTime::Seconds();
    FDasEditorSourceReadScope SourcePass;
    Error.Reset();
    if (!UpdateAutomaticCatalog(Error))
        return false;
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    TArray<FString> Scripts;
    IFileManager::Get().FindFilesRecursive(Scripts, *(FPaths::ProjectContentDir() / TEXT("Das/Scripts")),
                                                      TEXT("*.das"), true, false);
    for (auto& Script : Scripts)
        Script = FullPath(Script);
    const auto Plugin = IPluginManager::Get().FindPlugin(TEXT("DasForUE"));
    if (!Plugin.IsValid())
    {
        Error = TEXT("DasForUE plugin content is unavailable");
        return false;
    }
    const FString PluginScripts = FullPath(Plugin->GetContentDir() / TEXT("Das"));
    auto Manifest = [&](const FString& File, bool Required)
    {
        if (!Required && !IFileManager::Get().FileExists(*File))
            return true;
        TArray<FString> Lines;
        if (!FFileHelper::LoadFileToStringArray(Lines, *File))
        {
            Error = TEXT("Cannot read script manifest: ") + File;
            return false;
        }
        for (auto& Line : Lines)
        {
            Line.TrimStartAndEndInline();
            if (Line.IsEmpty() || Line.StartsWith(TEXT("#")))
                continue;
            FString Full = FullPath(PluginScripts / Line);
            if (!FPaths::IsRelative(Line) ||
                Line.Contains(TEXT("*")) || Line.Contains(TEXT("?")) || !FPaths::CollapseRelativeDirectories(Full) ||
                                                          !FPaths::IsUnderDirectory(Full, PluginScripts) ||
                                                          !IFileManager::Get().FileExists(*Full))
            {
                Error = TEXT("Missing or invalid script manifest entry: ") + File + TEXT(": ") + Line;
                return false;
            }
            if (FPaths::GetExtension(Full).Equals(TEXT("das"), ESearchCase::IgnoreCase))
                Scripts.AddUnique(Full);
        }
        return true;
    };
    // Runtime modules and explicitly connected plugin examples, not intentional negative test fixtures.
    if (!Manifest(PluginScripts / TEXT("RuntimeScripts.txt"), true) ||
                  !Manifest(FPaths::ProjectConfigDir() / TEXT("DasPluginScripts.txt"), false))
        return false;
    UWorld* EditorWorld = GEditor ? GEditor->GetEditorWorldContext(false).World() : nullptr;
    for (const auto& Script : Runtime.ConfiguredProgramScripts(EditorWorld))
        Scripts.AddUnique(FullPath(FPaths::IsRelative(Script) ? Runtime.ScriptPath(Script) : Script));
    Scripts.Sort();
    FScopedSlowTask Progress(float(Scripts.Num()),
                             NSLOCTEXT("DasForUE", "CompileAllScripts", "Compiling daScript scripts"));
    if (!IsRunningCommandlet() && !GIsAutomationTesting)
        Progress.MakeDialogDelayed(1.f);
    const auto Before = Runtime.ProgramCacheStats();
    struct FRootTiming
    {
        FString File;
        double Milliseconds;
    };
    TArray<FRootTiming> Timings;
    int32 Failed = 0;
    for (const auto& Script : Scripts)
    {
        Progress.EnterProgressFrame(1.f, FText::FromString(FPaths::GetCleanFilename(Script)));
        const double RootStarted = FPlatformTime::Seconds();
        FString Issue;
        const bool Prepared = Runtime.PrepareEditorPrograms({Script}, Issue);
        Timings.Add({Script, (FPlatformTime::Seconds() - RootStarted) * 1000});
        if (!Prepared)
        {
            if (!Error.IsEmpty())
                Error += TEXT("\n");
            Error += Issue;
            ++Failed;
        }
    }
    const auto After = Runtime.ProgramCacheStats();
    UE_LOG(
        LogTemp, Display,
        TEXT("DAS_EDITOR_COMPILE_ALL scripts=%d failed=%d source_compiles=%llu seconds=%.3f memory_hits=%llu disk_modules=%llu disk_reparsed=%llu disk_rejected=%llu disk_writes=%llu"),
            Scripts.Num(), Failed, After.Compilations - Before.Compilations, FPlatformTime::Seconds() - BatchStarted,
            After.Hits - Before.Hits, After.DiskModuleHits - Before.DiskModuleHits,
            After.DiskModuleReparses - Before.DiskModuleReparses,
            After.DiskModuleRejections - Before.DiskModuleRejections,
            After.DiskModuleCacheWrites - Before.DiskModuleCacheWrites);
    UE_LOG(
        LogTemp, Display,
        TEXT("DAS_EDITOR_SOURCE_READS reads=%llu bytes=%llu snapshot_hits=%llu dependency_checks=%llu invalidations=%llu"),
            After.SourcePhysicalReads - Before.SourcePhysicalReads, After.SourceBytesRead - Before.SourceBytesRead,
            After.SourceSnapshotHits - Before.SourceSnapshotHits,
            After.SourceContentChecks - Before.SourceContentChecks,
            After.SourceBatchInvalidations - Before.SourceBatchInvalidations);
    Timings.Sort([](const auto& A, const auto& B) { return A.Milliseconds > B.Milliseconds; });
    for (int32 I = 0; I < FMath::Min(5, Timings.Num()); ++I)
        UE_LOG(LogTemp, Display,
               TEXT("DAS_EDITOR_COMPILE_SLOW ms=%.3f file=%s"), Timings[I].Milliseconds, *Timings[I].File);
    return Failed == 0;
}
bool FDasAutoReimport::PrepareForPlay(FString& Error)
{
    FDasEditorSourceReadScope SourcePass;
    Error.Reset();
    ON_SCOPE_EXIT
    {
        PlayPreparationError = Error;
    };
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const auto Blueprints = Bound(nullptr);
    if (!PrepareAllPrograms(Error))
        return false;
    // A successful compile alone is insufficient when a structural reimport was refused/deferred.
    for (auto* Blueprint : Blueprints)
    {
        auto& Metadata = Blueprint->GetOutermost()->GetMetaData();
        const FString Source = Metadata.GetValue(Blueprint, TEXT("DasDSLSource"));
        FDasAuthoringModule Model;
        FString Json;
        const FString Path = FPaths::IsRelative(Source) ? Runtime.ScriptPath(Source) : Source;
        if (!Runtime.ReadAuthoringScript(Path, Model, Error) || !Model.ToJson(Json, Error))
            return false;
        if (Json != Metadata.GetValue(Blueprint, TEXT("DasDSLModel")))
        {
            Error = TEXT("Apply/reimport the changed DSL declaration before Play: ") + Blueprint->GetPathName();
            return false;
        }
    }
    return true;
}
TValueOrError<bool, FText> FDasAutoReimport::IsPIEAuthorizedInternal(bool) const
{
    // Silent toolbar queries cannot run the compiler or keep Play disabled after a source repair.
    return MakeValue(true);
}
TValueOrError<bool, FText> FDasAutoReimport::RequestPIEPermissionInternal(bool) const
{
    if (!PlayPreparationError.IsEmpty())
        return MakeError(FText::FromString(PlayPreparationError));
    return MakeValue(true);
}
void FDasAutoReimport::OnPreBeginPIE(bool)
{
    FDasEditorSourceReadScope SourcePass;
    if (CVarAutoReimport.GetValueOnGameThread() != 0 && !GIsAutomationTesting)
        ReimportStale();
    FString Error;
    if (!PrepareForPlay(Error))
        DasDiagnosticEditor::Report(Error);
}
void FDasAutoReimport::OnMapOpened(const FString&, bool)
{
    if (!StaleTicker.IsValid())
        StaleTicker = FTSTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateRaw(this, &FDasAutoReimport::CheckStale), SettleSeconds);
}
bool FDasAutoReimport::CheckStale(float)
{
    FDasEditorSourceReadScope SourcePass;
    StaleTicker.Reset();
    if (CVarAutoReimport.GetValueOnGameThread() != 0 && !GIsAutomationTesting)
        ReimportStale();
    if (!IsRunningCommandlet() && !GIsAutomationTesting)
    {
        FString Error;
        if (!PrepareAllPrograms(Error))
            DasDiagnosticEditor::Report(Error);
    }
    return false;
}
void FDasAutoReimport::Reimport(UBlueprint* Blueprint, bool bRetry)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Name = Blueprint->GetName();
    const FString Before = Blueprint->GetOutermost()->GetMetaData().GetValue(Blueprint, TEXT("DasDSLModel"));
    FString Error;
    // Body-only edits are already live through hot reload. Reimport would still run the defaults
    // preflight, which is refused during PIE.
    const FString Source = Blueprint->GetOutermost()->GetMetaData().GetValue(Blueprint, TEXT("DasDSLSource"));
    FDasAuthoringModule Model;
    FString Json;
    if (Runtime.ReadAuthoringScript(Runtime.ScriptPath(Source), Model, Error) && Model.ToJson(Json, Error) &&
        Json == Before)
        return;
    // A field migration that also changes saved maps or other Blueprints is applied only after Apply.
    const FString Path = Blueprint->GetPathName();
    const bool bConfirmed = Confirmed.Contains(Path);
    FDasReimportPlan Plan;
    FString PlanError;
    if (!bConfirmed && FDasDSLService::PlanReimport(Blueprint, Plan, PlanError) && Plan.NeedsConfirmation)
    {
        if (GEditor && GEditor->PlayWorld)
        {
            UE_LOG(LogTemp, Display, TEXT("DAS_DSL_AUTO_REIMPORT deferred %s: needs confirmation"), *Name);
            Deferred.AddUnique(Blueprint);
            Notify(FString::Printf(TEXT("daScript: %s changes other assets; you will be asked when Play stops"), *Name),
                                   false);
            return;
        }
        AskConfirmation(Path, Name, Plan);
        return;
    }
    if (!FDasDSLService::Reimport(Blueprint, Error, EDasDefaultMigrationPolicy::Automatic, bConfirmed))
    {
        // Some migrations (default values) never touch a running game world; apply them after PIE.
        const bool bPlaying = GEditor && GEditor->PlayWorld;
        const FString Text = bPlaying
            ? FString::Printf(TEXT("daScript: %s is not updated during Play (%s); it will be updated when Play stops"),
                                   *Name, *Error)
            : FString::Printf(TEXT("daScript: %s was not updated: %s"), *Name, *Error);
        if (bPlaying)
        {
            UE_LOG(LogTemp, Display, TEXT("DAS_DSL_AUTO_REIMPORT deferred %s: %s"), *Name, *Error);
            Deferred.AddUnique(Blueprint);
        }
        else
        {
            Confirmed.Remove(Path);
            UE_LOG(LogTemp, Warning, TEXT("DAS_DSL_AUTO_REIMPORT rejected %s: %s"), *Name, *Error);
            DasDiagnosticEditor::Report(Text);
        }
        Notify(Text, false);
        return;
    }
    Deferred.Remove(Blueprint);
    Confirmed.Remove(Path);
    if (bConfirmed || bRetry ||
        Blueprint->GetOutermost()->GetMetaData().GetValue(Blueprint, TEXT("DasDSLModel")) != Before)
    {
        UE_LOG(LogTemp, Display, TEXT("DAS_DSL_AUTO_REIMPORT applied %s"), *Name);
        Notify(FString::Printf(TEXT("daScript: %s updated from its script"), *Name), true);
    }
}
void FDasAutoReimport::OnEndPIE(bool)
{
    FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime")).SetEditorGameplayActive(false);
    if ((!Deferred.IsEmpty() || !DeferredGAS.IsEmpty() || !DeferredTypes.IsEmpty()) && !RetryTicker.IsValid())
        RetryTicker = FTSTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateRaw(this, &FDasAutoReimport::RetryAfterPIE), SettleSeconds);
}
bool FDasAutoReimport::RetryAfterPIE(float)
{
    // EndPIE is broadcast before the play world is torn down.
    if (GEditor && GEditor->PlayWorld)
        return true;
    RetryTicker.Reset();
    RetryDeferred();
    return false;
}
void FDasAutoReimport::RetryDeferred()
{
    const auto Blueprints = MoveTemp(Deferred);
    Deferred.Reset();
    for (const auto& Blueprint : Blueprints)
        if (Blueprint.IsValid())
            Reimport(Blueprint.Get(), true);
    const auto Imports = MoveTemp(DeferredGAS);
    DeferredGAS.Reset();
    for (const auto& Import : Imports)
        ReimportGAS(Import, true);
    const auto Types = MoveTemp(DeferredTypes);
    DeferredTypes.Reset();
    for (const auto& Import : Types)
        ReimportTypes(Import, true);
}
void FDasAutoReimport::AskConfirmation(const FString& Key, const FString& Name, const FDasReimportPlan& Plan)
{
    CloseConfirmation(Key);
    // The complete list goes to the log; the notification and the dialog stay short for large changes.
    UE_LOG(LogTemp, Display, TEXT("DAS_DSL_REIMPORT_PLAN %s\n%s"), *Name, *Plan.Details(MAX_int32));
    PlanDetails.Add(Key, Plan.Details());
    PlanNames.Add(Key, Name);
    const bool bAssets = !Plan.Maps.IsEmpty() || !Plan.Blueprints.IsEmpty() || !Plan.Tables.IsEmpty();
    FNotificationInfo Info(FText::FromString(FString::Printf(
        TEXT("daScript: %s - %s"), *Name,
             bAssets ? TEXT("this change also modifies other assets") : TEXT("this change drops or converts values"))));
    Info.SubText = FText::FromString(Plan.Short());
    Info.bFireAndForget = false;
    Info.bUseSuccessFailIcons = false;
    Info.ButtonDetails.Add(FNotificationButtonInfo(FText::FromString(
        TEXT("Apply")), FText::FromString(TEXT("Apply the change and update the listed assets")),
                                          FSimpleDelegate::CreateRaw(this, &FDasAutoReimport::Confirm, Key),
                                          SNotificationItem::CS_Pending));
    Info.ButtonDetails.Add(FNotificationButtonInfo(FText::FromString(
        TEXT("Details")), FText::FromString(TEXT("Show every changed field and affected asset")),
                                            FSimpleDelegate::CreateRaw(this, &FDasAutoReimport::ShowDetails, Key),
                                            SNotificationItem::CS_Pending));
    Info.ButtonDetails.Add(FNotificationButtonInfo(FText::FromString(
        TEXT("Cancel")), FText::FromString(TEXT("Keep everything as it is")),
                                           FSimpleDelegate::CreateRaw(this, &FDasAutoReimport::Decline, Key),
                                           SNotificationItem::CS_Pending));
    TSharedPtr<SNotificationItem> Item = FSlateNotificationManager::Get().AddNotification(Info);
    if (Item)
        Item->SetCompletionState(SNotificationItem::CS_Pending);
    Confirmations.Add(Key, Item);
}
void FDasAutoReimport::ShowDetails(FString Key)
{
    if (const FString* Text = PlanDetails.Find(Key))
        FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(*Text),
                             FText::FromString(TEXT("daScript: changes of ") + PlanNames.FindRef(Key)));
}
void FDasAutoReimport::CloseConfirmation(const FString& Key)
{
    TWeakPtr<SNotificationItem> Item;
    if (Confirmations.RemoveAndCopyValue(Key, Item))
        if (TSharedPtr<SNotificationItem> Pinned = Item.Pin())
        {
            Pinned->SetCompletionState(SNotificationItem::CS_None);
            Pinned->ExpireAndFadeout();
        }
    PlanDetails.Remove(Key);
}
void FDasAutoReimport::Confirm(FString Key)
{
    CloseConfirmation(Key);
    PlanNames.Remove(Key);
    Confirmed.Add(Key);
    FString Script, Registry;
    if (Key.StartsWith(TEXT("types:")) && Key.Mid(6).Split(TEXT("|"), &Script, &Registry))
        ReimportTypes({Script, Registry}, false);
    else if (UBlueprint* Blueprint = FindObject<UBlueprint>(nullptr, *Key))
        Reimport(Blueprint, false);
    else
        Confirmed.Remove(Key);
}
void FDasAutoReimport::Decline(FString Key)
{
    CloseConfirmation(Key);
    FString Name;
    if (!PlanNames.RemoveAndCopyValue(Key, Name))
        Name = FPackageName::ObjectPathToObjectName(Key);
    UE_LOG(LogTemp, Display, TEXT("DAS_DSL_AUTO_REIMPORT declined %s"), *Name);
    FMessageLog(
        TEXT("BlueprintLog"))
            .Info(FText::FromString(FString::Printf(
                TEXT("daScript: %s was not updated; its script changes fields used by other assets or drops values."),
                     *Name)))
                      ->AddToken(FActionToken::Create(FText::FromString(
                          TEXT("Apply")),
                          FText::FromString(TEXT("Apply the change and update the assets using it")),
                                            FOnActionTokenExecuted::CreateRaw(this, &FDasAutoReimport::Confirm, Key),
                                            true));
}
