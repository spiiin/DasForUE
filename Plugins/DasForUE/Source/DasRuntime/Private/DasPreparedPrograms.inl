// Trusted build artifacts, not an interchange format for downloaded scripts.
namespace
{
TAutoConsoleVariable<int32> CVarRequirePreparedPrograms(
    TEXT("das.RequirePreparedPrograms"),
#if UE_BUILD_SHIPPING
         1,
#else
         0,
#endif
         TEXT("Refuse source compilation on a persistent gameplay cache miss. Shipping defaults to 1."));
bool RequirePreparedPrograms()
{
    return CVarRequirePreparedPrograms.GetValueOnGameThread() != 0;
}

FString PreparedHash(const TArray<uint8>& Bytes)
{
    FSHAHash Hash;
    FSHA1::HashBuffer(Bytes.GetData(), Bytes.Num(), Hash.Hash);
    return Hash.ToString();
}
FString PreparedLogicalPath(FString Path)
{
    Path = FPaths::ConvertRelativePathToFull(Path);
    FPaths::NormalizeFilename(Path);
    FString Project = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());
    FString Plugin =
        FPaths::ConvertRelativePathToFull(IPluginManager::Get().FindPlugin(TEXT("DasForUE"))->GetContentDir());
    FPaths::NormalizeDirectoryName(Project);
    FPaths::NormalizeDirectoryName(Plugin);
    if (FPaths::IsUnderDirectory(Path, Project))
        return TEXT("project:") + Path.Mid(Project.Len() + 1);
    if (FPaths::IsUnderDirectory(Path, Plugin))
        return TEXT("plugin:") + Path.Mid(Plugin.Len() + 1);
    return {}; // Release artifacts may not depend on arbitrary files outside staged content.
}
FString PreparedPhysicalPath(const FString& Logical)
{
    FString Root, Relative;
    if (Logical.StartsWith(TEXT("project:")))
    {
        Root = FPaths::ProjectContentDir();
        Relative = Logical.Mid(8);
    }
    else if (Logical.StartsWith(TEXT("plugin:")))
    {
        Root = IPluginManager::Get().FindPlugin(TEXT("DasForUE"))->GetContentDir();
        Relative = Logical.Mid(7);
    }
    else
        return {};
    if (Relative.IsEmpty() || !FPaths::IsRelative(Relative))
        return {};
    Root = FPaths::ConvertRelativePathToFull(Root);
    FString Result = FPaths::ConvertRelativePathToFull(Root / Relative);
    FPaths::NormalizeFilename(Result);
    if (!FPaths::CollapseRelativeDirectories(Result) || !FPaths::IsUnderDirectory(Result, Root))
        return {};
    return Result;
}
FString PreparedArtifactPath(const FString& Source, const FString& Environment)
{
    const FString Identity = PreparedLogicalPath(Source) + Environment;
    const FTCHARToUTF8 Utf8(*Identity);
    TArray<uint8> Bytes;
    Bytes.Append(reinterpret_cast<const uint8*>(Utf8.Get()), Utf8.Length());
    return FPaths::ProjectContentDir() / TEXT("Das/Prepared") / (PreparedHash(Bytes) + TEXT(".dasp"));
}
struct FPreparedDependency
{
    FString Logical, Original, Hash;
    friend FArchive& operator<<(FArchive& Ar, FPreparedDependency& D)
    {
        return Ar << D.Logical << D.Original << D.Hash;
    }
};
bool LoadPreparedProgram(const FString& Path, const FString& Environment, FCompiledProgram& Out, FString& Error)
{
    Error = TEXT("missing artifact; run DasPreparePrograms before packaging");
    TArray<uint8> File;
    const FString Artifact = PreparedArtifactPath(Path, Environment);
    const int64 Size = IFileManager::Get().FileSize(*Artifact);
    if (Size < 0)
        return false;
    if (Size > 256 * 1024 * 1024 || !FFileHelper::LoadFileToArray(File, *Artifact))
    {
        Error = TEXT("unreadable or oversized artifact");
        return false;
    }
    struct FBoundedReader : FMemoryReader
    {
        explicit FBoundedReader(TArray<uint8>& Bytes) : FMemoryReader(Bytes, true)
        {
            ArMaxSerializeSize = 256 * 1024 * 1024;
        }
    } Reader(File);
    uint32 Magic = 0, Version = 0;
    FString ABI, SavedEnvironment, Root, PayloadHash;
    TArray<FPreparedDependency> Dependencies;
    TArray<uint8> Payload;
    Reader << Magic << Version;
    if (Reader.IsError() || Magic != 0x50534144 || Version != 1)
    {
        Error = TEXT("unsupported artifact header");
        return false;
    }
    Reader << ABI << SavedEnvironment << Root << Dependencies << PayloadHash << Payload;
    if (Reader.IsError() || Reader.Tell() != Reader.TotalSize() ||
        ABI != TEXT(DAS_PREPARED_ABI) || SavedEnvironment != Environment || Root != PreparedLogicalPath(Path) ||
                    PreparedHash(Payload) != PayloadHash)
    {
        Error = TEXT("stale ABI/environment or damaged artifact");
        return false;
    }
    FSourceSet Sources;
    TMap<FString, FString> Relocated;
    for (const auto& Dependency : Dependencies)
    {
        const FString Physical = PreparedPhysicalPath(Dependency.Logical);
        auto Snapshot = FSourceSnapshot::Read(Physical);
        if (Physical.IsEmpty() || !Snapshot.bExists || PreparedHash(Snapshot.Bytes) != Dependency.Hash)
        {
            Error = TEXT("source dependency changed: ") + Dependency.Logical;
            return false;
        }
        Relocated.Add(Dependency.Original, Physical);
        Sources.Add(Physical, MoveTemp(Snapshot));
    }
    if (!Sources.Contains(Path))
    {
        Error = TEXT("artifact does not include its root source");
        return false;
    }
    das::SerializationStorageVector Storage;
    Storage.buffer.assign(Payload.GetData(), Payload.GetData() + Payload.Num());
    das::gc_guard ReadNodes;
    Out.Sources = MoveTemp(Sources);
    // Bind restored source locations to validated, relocated UFS sources for diagnostics.
    class FPreparedFileSystem final : public das::AnyFileSystem
    {
    public:
        TMap<FString, FString> Paths;
        FSourceSet* Snapshots = nullptr;
        das::FileInfo* tryOpenFile(const das::string& Name) override
        {
            const auto* Path = Paths.Find(DSLText(Name));
            const auto* Snapshot = Path ? Snapshots->Find(*Path) : nullptr;
            if (!Snapshot || !Snapshot->bExists)
                return nullptr;
            auto File = MakeUnique<FUnrealTextFile>();
            File->Bytes = Snapshot->Bytes;
            File->Bytes.Add(0);
            File->name = Name;
            return File.Release();
        }
    };
    auto ReadAccess = das::make_smart<FUnrealFileAccess>();
    auto* FileSystem = new FPreparedFileSystem();
    FileSystem->Paths = Relocated;
    FileSystem->Snapshots = &Out.Sources;
    ReadAccess->addFileSystem(FileSystem, true, false);
    das::AstSerializer Serializer(&Storage, false);
    Serializer.thisModuleGroup = &Out.Modules;
    Serializer.fileAccess = ReadAccess.get();
    FDSLDiagnosticCompileScope Diagnostics(Out.Modules);
    FDSLDeclarationCompileScope Declarations;
    auto* EnvironmentState = das::daScriptEnvironment::getBound();
    const auto PreviousProgram = EnvironmentState->g_Program;
    ON_SCOPE_EXIT
    {
        EnvironmentState->g_Program = PreviousProgram;
    };
    auto Program = das::make_smart<das::Program>();
    Program->thisModuleGroup = &Out.Modules;
    const bool OK = Serializer.trySerialize([&](das::AstSerializer& S) { Program->serialize(S); });
    Serializer.moduleLibrary = nullptr; // Program owns the reconstructed modules, also on refusal.
    Serializer.collectFileInfo(Out.SerializedFileInfos);
    if (!OK || Serializer.failed || Program->failed() || Storage.bufferPos != Storage.buffer.size())
    {
        Error = TEXT("daScript rejected prepared program");
        return false;
    }
    for (auto* Info : Serializer.readFileInfos)
        if (const auto* Physical = Relocated.Find(DSLText(Info->name)))
            Info->name = DSLUtf8(*Physical);
    Program->library.foreach (
        [&](das::Module* Module)
        {
            if (Module != Program->thisModule.get())
                Out.Modules.addModule(Module);
            if (const auto* Physical = Relocated.Find(DSLText(Module->fileName)))
                Module->fileName = DSLUtf8(*Physical);
            return true;
        },
        "*");
    Out.Access = ReadAccess;
    Out.Program = MoveTemp(Program);
    DSLDiagnosticGenerated(Out.Program);
    Error.Reset();
    return true;
}
}

bool FDasRuntimeModule::ExportPreparedProgram(const FString& Script, FString& Error)
{
    check(IsInGameThread());
    const FString Path = NormalizeScriptPath(FPaths::IsRelative(Script) ? ScriptPath(Script) : Script);
    const FString Environment = PersistentCompilerKey(Path).RightChop(Path.Len());
    auto Compiled = AcquireCompiledProgram(Path, true, true);
    if (!Compiled->Program || !Compiled->CompileError.IsEmpty())
    {
        Error = Compiled->CompileError;
        return false;
    }
    TArray<FPreparedDependency> Dependencies;
    for (const auto& Pair : Compiled->Sources)
    {
        const FString Logical = PreparedLogicalPath(Pair.Key);
        if (Logical.IsEmpty() || !Pair.Value.bExists)
        {
            Error = TEXT("Unstaged dependency: ") + Pair.Key;
            return false;
        }
        Dependencies.Add({Logical, Pair.Key, PreparedHash(Pair.Value.Bytes)});
    }
    das::SerializationStorageVector Storage;
    das::AstSerializer Serializer(&Storage, true);
    const bool OK = Serializer.trySerialize([&](das::AstSerializer& S) { Compiled->Program->serialize(S); });
    Serializer.moduleLibrary = nullptr;
    if (!OK || Serializer.failed)
    {
        Error = TEXT("daScript cannot serialize this program");
        return false;
    }
    Storage.flush();
    TArray<uint8> Payload;
    Payload.Append(Storage.buffer.data(), int32(Storage.buffer.size()));
    FBufferArchive File;
    uint32 Magic = 0x50534144, Version = 1;
    FString ABI = TEXT(DAS_PREPARED_ABI), SavedEnvironment = Environment, RootIdentity = PreparedLogicalPath(Path);
    FString PayloadHash = PreparedHash(Payload);
    File << Magic << Version << ABI << SavedEnvironment << RootIdentity << Dependencies << PayloadHash << Payload;
    const FString Artifact = PreparedArtifactPath(Path, Environment);
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(Artifact), true);
    if (!FFileHelper::SaveArrayToFile(File, *Artifact))
    {
        Error = TEXT("Cannot write artifact: ") + Artifact;
        return false;
    }
    // Never publish an artifact that its current runtime cannot read back.
    FCompiledProgram RoundTrip;
    if (!LoadPreparedProgram(Path, Environment, RoundTrip, Error))
    {
        IFileManager::Get().Delete(*Artifact);
        return false;
    }
    return true;
}

bool FDasRuntimeModule::PreparePrograms(UWorld* World, const TArray<FString>& Scripts, FString& Error)
{
    check(IsInGameThread());
    Error.Reset();
    if (!IsValid(World) || World->HasBegunPlay())
    {
        Error = TEXT("Prepare programs before world BeginPlay");
        return false;
    }
    TArray<TSharedPtr<FCompiledProgram>> Prepared;
    for (const auto& Script : Scripts)
    {
        const FString Path = NormalizeScriptPath(FPaths::IsRelative(Script) ? ScriptPath(Script) : Script);
        auto Compiled = AcquireCompiledProgram(Path, true, true);
        for (const auto& Session : Sessions)
            if (Session->Path == Path && Session->bPreparationMissing)
                Session->AttemptedSources.Reset();
        if (!Compiled->Program || !Compiled->CompileError.IsEmpty())
        {
            Error = Path + TEXT(": ") + Compiled->CompileError;
            return false;
        }
        Prepared.AddUnique(Compiled);
    }
    WorldPreparedPrograms.FindOrAdd(World).Append(Prepared);
    UE_LOG(LogDasRuntime, Display, TEXT("DAS_PREPARED programs=%d world=%s"), Prepared.Num(), *World->GetName());
    return true;
}
bool FDasRuntimeModule::PrepareConfiguredPrograms(UWorld* World, FString& Error)
{
    return PreparePrograms(World, ConfiguredProgramScripts(World), Error);
}
TArray<FString> FDasRuntimeModule::ConfiguredProgramScripts(UWorld* World) const
{
    TArray<FString> Scripts, MapScripts;
    GConfig->GetArray(TEXT("DasForUE.Programs"), TEXT("Scripts"), Scripts, GGameIni);
    FString Map = World ? World->GetOutermost()->GetName() : FString();
    Map = UWorld::RemovePIEPrefix(Map);
    GConfig->GetArray(*(TEXT("DasForUE.Programs ") + Map), TEXT("Scripts"), MapScripts, GGameIni);
    for (const auto& Script : MapScripts)
        Scripts.AddUnique(Script);
    return Scripts;
}

namespace
{
void PinEditorProgram(const FString& Path, const TSharedPtr<FCompiledProgram>& Compiled)
{
    if (!GIsEditor || CollectTypesOnly || DSLDeclarationCompileDepth != 0 || !Compiled->Program ||
        !Compiled->CompileError.IsEmpty())
        return;
    EditorProgramAttempts.Add(Path, Compiled);
    // An earlier call may have refused this exact source before the queued preparation ran.
    // Publishing code must let that session retry even though no bytes changed since refusal.
    for (const auto& Session : Sessions)
        if (Session->Path == Path && (!Session->Live || Session->Live->Compiler != Compiled) &&
            (CVarScriptChangeCheck.GetValueOnGameThread() != 2 || !Session->Live))
            Session->AttemptedSources.Reset();
}
}
void FDasRuntimeModule::SetEditorGameplayActive(bool Active)
{
    check(IsInGameThread());
    EditorGameplayActive = Active;
}
bool FDasRuntimeModule::PrepareEditorPrograms(const TArray<FString>& Scripts, FString& Error)
{
    check(IsInGameThread());
    Error.Reset();
    for (const auto& Script : Scripts)
    {
        const FString Path = NormalizeScriptPath(FPaths::IsRelative(Script) ? ScriptPath(Script) : Script);
        auto Compiled = AcquireCompiledProgram(Path, true, true);
        if (GIsEditor)
            EditorProgramAttempts.Add(Path, Compiled);
        for (const auto& Session : Sessions)
            if (Session->Path == Path && Session->bPreparationMissing)
                Session->AttemptedSources.Reset();
        if (!Compiled->Program || !Compiled->CompileError.IsEmpty())
        {
            if (!Error.IsEmpty())
                Error += TEXT("\n");
            Error += Path + TEXT(": ") + Compiled->CompileError;
            continue;
        }
        PinEditorProgram(Path, Compiled);
    }
    return Error.IsEmpty();
}
bool FDasRuntimeModule::PrepareEditorChanges(const TArray<FString>& Files, bool bHierarchyChanged, FString& Error)
{
    check(IsInGameThread());
    TSet<FString> Changed, Roots, Covered;
    for (const auto& File : Files)
        Changed.Add(NormalizeScriptPath(File));
    auto Consider = [&](const FString& Path, const TSharedPtr<FCompiledProgram>& Program)
    {
        bool Affected = bHierarchyChanged || Changed.Contains(Path);
        for (const auto& File : Changed)
            if (Program->Sources.Contains(File))
            {
                Affected = true;
                Covered.Add(File);
            }
        if (Affected)
            Roots.Add(Path);
    };
    for (const auto& Pair : EditorProgramAttempts)
        Consider(Pair.Key, Pair.Value);
    for (const auto& Session : Sessions)
        if (Session->Live)
            Consider(Session->Path, Session->Live->Compiler);
    for (const auto& Pair : KnownCompiledPrograms)
        if (auto Program = Pair.Value.Pin(); Program && !Program->Sources.IsEmpty())
            for (const auto& Source : Program->Sources)
                if (Pair.Key.StartsWith(Source.Key + TEXT("|")))
                {
                    Consider(Source.Key, Program);
                    break;
                }
    for (const auto& File : Changed)
        if (!Covered.Contains(File) && FPaths::GetExtension(File).Equals(TEXT("das"), ESearchCase::IgnoreCase) &&
                                                                         IFileManager::Get().FileExists(*File))
            Roots.Add(File);
    return PrepareEditorPrograms(Roots.Array(), Error);
}
