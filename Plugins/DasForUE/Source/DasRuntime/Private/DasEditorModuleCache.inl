// Editor-only acceleration. UE-dependent modules reparse through the global lint policy below.
// ModuleFileCache owns deserialized source locations and must outlive the AST/ModuleGroup.
struct FDasEditorModuleCache
{
    das::ModuleFileCache Cache;
};
TArray<TSharedPtr<FDasEditorModuleCache>>* ActiveEditorModuleCaches = nullptr;
FString ActiveEditorModuleCacheEnvironment;
// Restored stdlib modules may be promoted into the process-wide builtin list.
// Their line infos survive ordinary program-cache eviction and are released only
// after Module::Shutdown has destroyed those shared modules.
das::vector<das::FileInfoPtr> EditorSharedModuleFileInfos;
uint64 DiskModuleHits = 0, DiskModuleReparses = 0, DiskModuleCacheWrites = 0;
double CompilerPassSeconds = 0;
TAutoConsoleVariable<int32> CVarEditorModuleCache(TEXT("das.EditorModuleCache"), 1,
    TEXT("Validate and reuse standard daScript library modules from Saved/Das/ModuleCache in the editor. UE modules always reparse."));
TAutoConsoleVariable<int32> CVarCompilerTimings(TEXT("das.CompilerTimings"), 0,
    TEXT("Log per-pass daScript compile/cache timings. Startup always reports totals and the five slowest roots."));

das::ProgramPtr CompileEditorCachedPass(const char* File, const das::FileAccessPtr& Access, das::TextWriter& Output,
                                        das::ModuleGroup& Group, const das::CodeOfPolicies& Policies, const TCHAR* Pass)
{
    const double Started = FPlatformTime::Seconds();
    auto* Environment = das::daScriptEnvironment::getBound();
    // Nested authored-reference compilation has its own stream. Never let it consume
    // the parent's reader or append unrelated records to the parent's writer.
    TGuardValue<das::AstSerializer*> Read(Environment->serializer_read, nullptr);
    TGuardValue<das::AstSerializer*> Write(Environment->serializer_write, nullptr);
    const bool Enabled = GIsEditor && ActiveEditorModuleCaches && CVarEditorModuleCache.GetValueOnGameThread() != 0;
    TSharedPtr<FDasEditorModuleCache> Owner;
    FString CachePath;
    if (Enabled)
    {
        Owner = MakeShared<FDasEditorModuleCache>();
        ActiveEditorModuleCaches->Add(Owner);
        TArray<FString> SharedLibraries;
        das::Module::foreach (
            [&](das::Module* Module)
            {
                if (Module->promoted)
                    SharedLibraries.Add(UTF8_TO_TCHAR(Module->name.c_str()));
                return true;
            });
        SharedLibraries.Sort();
        // Shared modules already loaded in this process are omitted from a compile's
        // stream. A save must not overwrite the full-prefix cold-start stream with
        // that shortened graph; the next process still needs the library records.
        const FString SharedKey = FString::Join(SharedLibraries, TEXT(";"));
        const FString Identity = FString(UTF8_TO_TCHAR(File)) +
                                 TEXT("|") + Pass +
                                      TEXT("|") + ActiveEditorModuleCacheEnvironment +
                                           TEXT("|") + TEXT(DAS_PREPARED_ABI) + TEXT("|editor-modules-v2|") + SharedKey;
        CachePath = FPaths::ConvertRelativePathToFull(
            FPaths::ProjectSavedDir() / TEXT("Das/ModuleCache") / (FMD5::HashAnsiString(*Identity) + TEXT(".dasm")));
        const int64 Size = IFileManager::Get().FileSize(*CachePath);
        // A corrupt oversized stream is a cache miss, never an unbounded allocation.
        const FString ReadPath = Size >= 0 && Size <= 256ll * 1024 * 1024 ? CachePath : FString();
        Owner->Cache.install(TCHAR_TO_UTF8(*ReadPath), TCHAR_TO_UTF8(*CachePath), true);
    }
    das::ProgramPtr Program;
    bool Successful = false;
    ON_SCOPE_EXIT
    {
        das::ModuleFileCache::Result Result;
        if (Owner)
        {
            // A failed candidate may not replace the last good disk stream.
            if (!Successful)
            {
                Owner->Cache.writeStorage.buffer.clear();
                Owner->Cache.writeStorage.writePos = 0;
                Owner->Cache.writeStorage.wrote = false;
            }
            Result = Owner->Cache.finish();
            for (auto& Info : Owner->Cache.orphanedFileInfos)
                EditorSharedModuleFileInfos.push_back(std::move(Info));
            Owner->Cache.orphanedFileInfos.clear();
            // Readers/writers are finished; retain locations, not entire disk streams.
            das::vector<uint8_t>().swap(Owner->Cache.readStorage.buffer);
            das::vector<uint8_t>().swap(Owner->Cache.writeStorage.buffer);
            DiskModuleHits += Result.served;
            DiskModuleReparses += Result.resumed;
            DiskModuleCacheWrites += Result.wrote ? 1 : 0;
            if (Result.saveFailed)
                UE_LOG(LogDasRuntime, Display,
                       TEXT("DAS_MODULE_CACHE_WRITE_FAILED path=%s; compilation remains valid"), *CachePath);
        }
        const double Elapsed = FPlatformTime::Seconds() - Started;
        CompilerPassSeconds += Elapsed;
        if (CVarCompilerTimings.GetValueOnGameThread() != 0)
            UE_LOG(LogDasRuntime, Display,
                   TEXT("DAS_COMPILE_PASS file=%s pass=%s ms=%.3f disk_modules=%llu reparsed=%llu wrote=%d cutoff=%s"),
                        UTF8_TO_TCHAR(File), Pass, Elapsed * 1000, Result.served, Result.resumed, Result.wrote,
                        UTF8_TO_TCHAR(Result.cutoffReason.c_str()));
    };
    Program = das::compileDaScript(File, Access, Output, Group, Policies);
    Successful = Program && !Program->failed();
    return Program;
}
