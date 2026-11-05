struct FSharedEditorModule
{
    das::Module* Module = nullptr;
    TSharedPtr<FCompiledProgram> Owner;
    FSourceSet Sources;
};
TMap<FString, TMap<FString, FSharedEditorModule>> SharedEditorModules;

void ResetSharedCompilerModules()
{
    SharedEditorModules.Reset();
}

bool CanShareEditorModule(das::Module* Module, TMap<das::Module*, bool>& Checked)
{
    if (Module->builtIn)
        return true;
    if (const bool* Found = Checked.Find(Module))
        return *Found;
    // Reject cycles until the whole dependency closure has passed validation.
    Checked.Add(Module, false);
    FString File = FPaths::ConvertRelativePathToFull(UTF8_TO_TCHAR(Module->fileName.c_str()));
    FPaths::NormalizeFilename(File);
    const FString Library = FPaths::ConvertRelativePathToFull(
        IPluginManager::Get().FindPlugin(TEXT("DasForUE"))->GetContentDir() / TEXT("Das/Scripts"));
    if (!FPaths::IsUnderDirectory(File, Library) || Module->name.rfind("ue_", 0) != 0)
        return false;
    bool Stable = true;
    Module->structures.foreach([&](const das::StructurePtr& S)
    {
        for (const auto& A : S->annotations)
        {
            const auto& Name = A->annotation->name;
            if (Name.rfind("ue_", 0) == 0 && Name != "ue_native_object" && Name != "ue_native_interface" &&
                Name != "ue_native_struct" && Name != "ue_native_enum" && Name != "ue_native_base")
                Stable = false;
        }
    });
    // Plugin helper state must not make different root programs share runtime globals.
    if (!Module->globals.each().empty())
        Stable = false;
    for (const auto& Dependency : Module->requireModule)
        if (Dependency.first != Module && !CanShareEditorModule(Dependency.first, Checked))
            Stable = false;
    Checked[Module] = Stable;
    return Stable;
}

bool CollectSharedModuleSources(das::Module* Module, const FCompiledProgram& Owner,
                                TSet<das::Module*>& Seen, FSourceSet& Sources)
{
    if (Module->builtIn || Seen.Contains(Module))
        return true;
    Seen.Add(Module);
    FString Path = FPaths::ConvertRelativePathToFull(UTF8_TO_TCHAR(Module->fileName.c_str()));
    FPaths::NormalizeFilename(Path);
    const auto* Source = Owner.Sources.Find(Path);
    if (!Source)
        for (const auto& Pair : Owner.Sources)
            if (Pair.Key.Equals(Path, ESearchCase::IgnoreCase))
            {
                Path = Pair.Key;
                Source = &Pair.Value;
                break;
            }
    // Never publish an AST without the exact source snapshot it was compiled from.
    if (!Source)
        return false;
    Sources.Add(Path, *Source);
    for (const auto& Dependency : Module->requireModule)
        if (Dependency.first != Module && !CollectSharedModuleSources(Dependency.first, Owner, Seen, Sources))
            return false;
    return true;
}

void PublishSharedEditorModules(const TSharedPtr<FCompiledProgram>& Compiled, const FString& Environment)
{
    if (!GIsEditor || !EditorSourceReadBatch || CVarEditorSharedModules.GetValueOnGameThread() == 0 ||
        !Compiled->Program || !Compiled->CompileError.IsEmpty())
        return;
    auto& Cache = SharedEditorModules.FindOrAdd(Environment);
    TMap<das::Module*, bool> Checked;
    for (auto* Module : Compiled->Modules.getModules())
    {
        const FString Name = UTF8_TO_TCHAR(Module->name.c_str());
        if (Module->builtIn || Cache.Contains(Name) || !CanShareEditorModule(Module, Checked))
            continue;
        FSharedEditorModule Entry;
        Entry.Module = Module;
        Entry.Owner = Compiled;
        TSet<das::Module*> Seen;
        if (!CollectSharedModuleSources(Module, *Compiled, Seen, Entry.Sources))
            continue;
        Cache.Add(Name, MoveTemp(Entry));
    }
}

void SeedSharedEditorModules(FDasCompilerModuleGroup& Group)
{
    if (!GIsEditor || !EditorSourceReadBatch || !ActiveEditorModuleCaches ||
        CVarEditorSharedModules.GetValueOnGameThread() == 0)
        return;
    const auto* Cache = SharedEditorModules.Find(ActiveEditorModuleCacheEnvironment);
    if (!Cache)
        return;
    for (const auto& Pair : *Cache)
    {
        const auto& Entry = Pair.Value;
        if (Group.findModule(Entry.Module->name))
            continue;
        Group.Borrow(Entry.Module, Entry.Owner);
        if (AuthoredReferenceSources)
            for (const auto& Source : Entry.Sources)
                AuthoredReferenceSources->Add(Source.Key, Source.Value);
        ++SharedEditorModuleHits;
    }
}
