#include "DasEditorModuleCacheCommandlet.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "DasScriptClassData.h"
#include "HAL/FileManager.h"
#include "HAL/IConsoleManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "UObject/StrongObjectPtr.h"

UDasEditorModuleCacheCommandlet::UDasEditorModuleCacheCommandlet()
{
    IsEditor = true;
    IsClient = IsServer = false;
    LogToConsole = true;
}
int32 UDasEditorModuleCacheCommandlet::Main(const FString& Params)
{
    FString Id;
    FGuid Guid;
    if (!FParse::Value(*Params, TEXT("ProbeId="), Id) || !FGuid::Parse(Id, Guid))
        return 1;
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    auto* Strict = IConsoleManager::Get().FindConsoleVariable(TEXT("das.RequirePreparedPrograms"));
    const int32 OldStrict = Strict->GetInt();
    IConsoleManager::Get().FindConsoleVariable(TEXT("das.CompilerTimings"))->Set(1, ECVF_SetByCode);
    Strict->Set(0, ECVF_SetByCode);
    ON_SCOPE_EXIT
    {
        Strict->Set(OldStrict, ECVF_SetByCode);
        R.ClearProgramCache();
    };
    const FString Dir = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() / TEXT("Das/Scripts"));
    const FString Name = TEXT("module_cache_") + Id, Root = Dir / (Name + TEXT(".das"));
    const FString Dependency = Name + TEXT("_dep"), Dep = Dir / (Dependency + TEXT(".das"));
    const FString Unused = Dir / (Name + TEXT("_unused.das"));
    const FString CacheDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / TEXT("Das/ModuleCache"));
    const FString ProbeDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / TEXT("Das/CacheProbe") / Id);
    const bool Cold = FParse::Param(*Params, TEXT("Cold"));
    const bool Reopen = FParse::Param(*Params, TEXT("Reopen"));
    IFileManager::Get().MakeDirectory(*Dir, true);
    IFileManager::Get().MakeDirectory(*ProbeDir, true);
    FString Error;
    auto Save = [&](const FString& Path, const FString& Text)
    {
        return FFileHelper::SaveStringToFile(Text, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    };
    int32 Checks = 0;
    auto Check = [&](bool OK, const TCHAR* What)
    {
        ++Checks;
        if (!OK)
            UE_LOG(LogTemp, Error, TEXT("DAS_EDITOR_MODULE_CACHE FAIL %s: %s"), What, *Error);
        return OK;
    };
    const FString Source =
        TEXT("options gen2\nrequire ue_native_gameplay\nrequire daslib/algorithm\nrequire daslib/option\nrequire daslib/match\nrequire ue_dsl\nrequire ") + Dependency + TEXT(R"DAS(
struct DiskSnapshot {
    Value : int = 0
}
def disk_optional(value:int):Option<DiskSnapshot> {
    return some(DiskSnapshot(Value=value))
}
[ue_class(Blueprintable)]
class DiskCachedActor : Actor {
    @ue_property=(BlueprintReadWrite)
    Score:int=2+3
    [ue_function(BlueprintCallable)]
    def Add(value:int):int { self.Score+=value; return self.Score }
}
[export]
def main(input:int):int { return input+(disk_optional(dependency_value()) ?? DiskSnapshot()).Value }
)DAS");
    auto DepSource = [&](int32 Value)
    {
        return TEXT("options gen2\nmodule ") + Dependency +
                    FString::Printf(TEXT(" public\ndef dependency_value():int { return %d }\n"), Value);
    };
    TArray<FString> BeforeFiles;
    IFileManager::Get().FindFiles(BeforeFiles, *(CacheDir / TEXT("*.dasm")), true, false);
    if (Cold && (!Save(Dep, DepSource(10)) || !Save(Root, Source)))
        return 1;
    const auto Before = R.ProgramCacheStats();
    const double Started = FPlatformTime::Seconds();
    FDasAuthoringModule Model;
    if (!Check(R.ReadAuthoringScript(Root, Model, Error), TEXT("compile authored model with computed defaults")))
        return 1;
    FString Json;
    if (!Check(Model.ToJson(Json, Error), TEXT("model serialization")))
        return 1;
    const double Seconds = FPlatformTime::Seconds() - Started;
    const auto After = R.ProgramCacheStats();
    if (!Check(After.DiskModuleRejections == Before.DiskModuleRejections,
               TEXT("no hidden payload rejection during initial preparation")))
        return 1;
    TStrongObjectPtr<UObject> Owner(NewObject<UDasScriptClassData>());
    ON_SCOPE_EXIT
    {
        R.ReleaseObject(Owner.Get());
    };
    auto Run = [&](int32 Expected)
    {
        const auto Result = R.RunPersistent(Root, 1, Owner.Get());
        Error = Result.Error;
        return Check(Result.bSucceeded && Result.Value == Expected, TEXT("prepared runtime result"));
    };
    if (!Run(Reopen ? 31 : 11))
        return 1;
    if (Cold)
    {
        TArray<FString> Files, NewFiles;
        IFileManager::Get().FindFiles(Files, *(CacheDir / TEXT("*.dasm")), true, false);
        for (const auto& File : Files)
            if (!BeforeFiles.Contains(File))
                NewFiles.Add(File);
        if (!Check(!NewFiles.IsEmpty() && After.DiskModuleCacheWrites > Before.DiskModuleCacheWrites,
                   TEXT("cold disk writes")))
            return 1;
        if (!Save(ProbeDir / TEXT("model.json"), Json) ||
                  !Save(ProbeDir / TEXT("files.txt"), FString::Join(NewFiles, TEXT("\n"))))
            return 1;
    }
    else if (Reopen)
    {
        FString Expected;
        FFileHelper::LoadFileToString(Expected, *(ProbeDir / TEXT("model.json")));
        if (!Check(Expected == Json, TEXT("reopened DSL model unchanged")) ||
                   !Check(After.DiskModuleHits > Before.DiskModuleHits,
                          TEXT("stdlib prefix survives saved dependency and process restart")))
            return 1;
    }
    else
    {
        FString Expected;
        FFileHelper::LoadFileToString(Expected, *(ProbeDir / TEXT("model.json")));
        if (!Check(Expected == Json, TEXT("fresh process DSL metadata matches cold compile")) ||
                   !Check(After.DiskModuleHits > Before.DiskModuleHits,
                          TEXT("fresh process served standard modules from disk")))
            return 1;
        if (!Check(After.DiskModuleCacheWrites == Before.DiskModuleCacheWrites,
                   TEXT("unchanged warm stream used without rewriting or repair")))
            return 1;
        const uint64 Compiles = R.ProgramCacheStats().Compilations;
        if (!Check(R.PrepareEditorPrograms({Root}, Error), TEXT("repeat full preparation")) ||
                   !Check(R.ProgramCacheStats().Compilations == Compiles, TEXT("repeat checks reuse memory program")))
            return 1;
        if (!Save(Dep, DepSource(30)))
            return 1;
        R.NotifyScriptsChanged({Dep});
        if (!Check(R.PrepareEditorChanges({Dep}, false, Error), TEXT("dependency save refreshes root")) || !Run(31))
            return 1;
        Save(Unused, TEXT("options gen2\nthis is invalid syntax\n"));
        R.NotifyScriptsChanged({Unused}, true);
        if (!Check(!R.PrepareEditorPrograms({Unused, Root}, Error) &&
                   Error.Contains(Name + TEXT("_unused")), TEXT("unreferenced invalid script is checked")))
            return 1;
        Save(Root, TEXT("options gen2\nthis is invalid syntax\n"));
        R.NotifyScriptsChanged({Root});
        if (!Check(!R.PrepareEditorChanges({Root}, false, Error), TEXT("invalid saved root rejected")) || !Run(31))
            return 1;
        Save(Root, Source);
        R.NotifyScriptsChanged({Root});
        if (!Check(R.PrepareEditorChanges({Root}, false, Error), TEXT("repaired root prepared")) || !Run(31))
            return 1;
        R.ReleaseObject(Owner.Get());
        R.ClearProgramCache();
        TArray<FString> Files, AllFiles, NewFiles;
        FFileHelper::LoadFileToStringArray(Files, *(ProbeDir / TEXT("files.txt")));
        IFileManager::Get().FindFiles(AllFiles, *(CacheDir / TEXT("*.dasm")), true, false);
        for (const auto& File : AllFiles)
            if (!BeforeFiles.Contains(File))
            {
                NewFiles.Add(File);
                Files.AddUnique(File);
            }
        if (!Check(!NewFiles.IsEmpty(), TEXT("save uses separate in-process graph stream")))
            return 1;
        if (!Save(ProbeDir / TEXT("files.txt"), FString::Join(Files, TEXT("\n"))))
            return 1;
        // Corrupt only the in-process variant. The full-prefix startup stream must
        // stay intact, and the Reopen phase will prove it still serves libraries.
        for (const auto& File : NewFiles)
        {
            if (!Check(FPaths::GetCleanFilename(File) == File && File.EndsWith(TEXT(".dasm")),
                                                                               TEXT("fixture cache path")))
                return 1;
            Save(CacheDir / File, TEXT("damaged cache header"));
        }
        if (!Check(R.PrepareEditorPrograms({Root}, Error), TEXT("damaged stream falls back to source")) || !Run(31))
            return 1;
        IFileManager::Get().Delete(*Unused);
    }
    UE_LOG(
        LogTemp, Display,
        TEXT("DAS_EDITOR_MODULE_CACHE PASS phase=%s checks=%d seconds=%.6f disk_modules=%llu reparsed=%llu rejected=%llu writes=%llu"),
            Cold ? TEXT("cold")
                 : (Reopen ? TEXT("reopen") : TEXT("warm")), Checks, Seconds,
                    After.DiskModuleHits - Before.DiskModuleHits, After.DiskModuleReparses - Before.DiskModuleReparses,
                After.DiskModuleRejections - Before.DiskModuleRejections,
                    After.DiskModuleCacheWrites - Before.DiskModuleCacheWrites);
    return 0;
}
