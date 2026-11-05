void ResetSharedCompilerModules();

// Preparation is a synchronous game-thread pass. Share immutable byte snapshots
// across roots for this pass only; each compiled program still owns its dependencies.
TAutoConsoleVariable<int32> CVarEditorSourceBatchCache(
    TEXT("das.EditorSourceBatchCache"), 1,
         TEXT("Read/hash a source file once per editor preparation pass. Saves invalidate the active pass cache."));
struct FEditorSourceReadBatch
{
    FSourceSet Snapshots;
};
TUniquePtr<FEditorSourceReadBatch> EditorSourceReadBatch;
int32 EditorSourceReadDepth = 0;
uint64 SourcePhysicalReads = 0, SourceBytesRead = 0, SourceSnapshotHits = 0, SourceContentChecks = 0,
       SourceBatchInvalidations = 0;
bool BeginEditorSourceReadBatch()
{
    check(IsInGameThread());
    if (!GIsEditor || CVarEditorSourceBatchCache.GetValueOnGameThread() == 0)
        return false;
    if (EditorSourceReadDepth++ == 0)
        EditorSourceReadBatch = MakeUnique<FEditorSourceReadBatch>();
    return true;
}
void EndEditorSourceReadBatch()
{
    check(IsInGameThread());
    check(EditorSourceReadDepth > 0);
    if (--EditorSourceReadDepth == 0)
    {
        ResetSharedCompilerModules();
        EditorSourceReadBatch.Reset();
    }
}
void InvalidateEditorSourceReadBatch()
{
    if (EditorSourceReadBatch)
    {
        ResetSharedCompilerModules();
        EditorSourceReadBatch->Snapshots.Reset();
        ++SourceBatchInvalidations;
    }
}
FSourceSnapshot ReadSourceSnapshotData(const FString& Path)
{
    ++SourcePhysicalReads;
    FSourceSnapshot Result;
    Result.bExists = IFileManager::Get().FileExists(*Path) && FFileHelper::LoadFileToArray(Result.Bytes, *Path);
    if (Result.bExists)
    {
        const FFileStatData Stat = IFileManager::Get().GetStatData(*Path);
        Result.Size = Stat.FileSize;
        Result.Stamp = Stat.ModificationTime;
        Result.ReadAt = FDateTime::UtcNow();
    }
    SourceBytesRead += uint64(Result.Bytes.Num());
    FSHA1::HashBuffer(Result.Bytes.GetData(), Result.Bytes.Num(), Result.ContentHash.Hash);
    return Result;
}
const FSourceSnapshot& BatchSourceSnapshot(const FString& Path)
{
    FString Key = FPaths::ConvertRelativePathToFull(Path);
    FPaths::NormalizeFilename(Key);
    FPaths::CollapseRelativeDirectories(Key);
#if PLATFORM_WINDOWS
    Key.ToLowerInline();
#endif
    if (const auto* Found = EditorSourceReadBatch->Snapshots.Find(Key))
    {
        ++SourceSnapshotHits;
        return *Found;
    }
    return EditorSourceReadBatch->Snapshots.Add(Key, ReadSourceSnapshotData(Path));
}
FSourceSnapshot FSourceSnapshot::Read(const FString& Path)
{
    return EditorSourceReadBatch ? BatchSourceSnapshot(Path) : ReadSourceSnapshotData(Path);
}
bool FSourceSnapshot::Matches(const FString& Path) const
{
    ++SourceContentChecks;
    if (EditorSourceReadBatch)
    {
        // Compare this program's own baseline, not a single cached yes/no for the
        // path: different roots may hold different previous source revisions.
        const auto& Current = BatchSourceSnapshot(Path);
        return bExists == Current.bExists && ContentHash == Current.ContentHash;
    }
    const auto Current = ReadSourceSnapshotData(Path);
    return bExists == Current.bExists && Bytes == Current.Bytes;
}
