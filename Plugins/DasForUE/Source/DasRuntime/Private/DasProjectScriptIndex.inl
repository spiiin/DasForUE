// Indexed at startup and on reported hierarchy changes, never by ScriptPath lookup.
TMap<FString, FString> ProjectScriptPaths;
TSet<FString> AmbiguousProjectScripts;
void IndexProjectScripts()
{
    ProjectScriptPaths.Reset();
    AmbiguousProjectScripts.Reset();
    TArray<FString> Files;
    IFileManager::Get().FindFilesRecursive(Files, *(FPaths::ProjectContentDir() / TEXT("Das/Scripts")),
                                                    TEXT("*.das"), true, false);
    for (const auto& File : Files)
    {
        const FString Name = FPaths::GetCleanFilename(File);
        const FString Path = FPaths::ConvertRelativePathToFull(File);
        if (const auto* Prior = ProjectScriptPaths.Find(Name); Prior && *Prior != Path)
            AmbiguousProjectScripts.Add(Name);
        else
            ProjectScriptPaths.Add(Name, Path);
    }
    for (const auto& Name : AmbiguousProjectScripts)
        UE_LOG(LogDasRuntime, Error, TEXT("Ambiguous project script filename; use unique module filenames: %s"), *Name);
}
