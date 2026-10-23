#include "DasScriptWriteTracker.h"
#include "Async/AsyncFileHandle.h"
#include "Async/MappedFileHandle.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/PathViews.h"
#include "Misc/ScopeLock.h"
#include <atomic>

namespace
{
FCriticalSection QueueLock;
std::atomic<bool> bQueued{false};
TSet<FString> QueuedFiles, QueuedDirectories;
bool bQueuedAddedOrRemoved = false;

bool IsScript(const TCHAR* Path)
{
    const int32 Length = Path ? FCString::Strlen(Path) : 0;
    return Length > 4 && FCString::Stricmp(Path + Length - 4, TEXT(".das")) == 0;
}
// A path without an extension may be a directory that holds scripts.
bool MayHoldScripts(const TCHAR* Path)
{
    return Path && *Path && FPathViews::GetExtension(Path).IsEmpty();
}
// File operations run on any thread (async IO, DDC, shader workers); the queue is drained on the game thread.
void Queue(const TCHAR* Path, bool bDirectory, bool bAddedOrRemoved)
{
    FScopeLock Lock(&QueueLock);
    (bDirectory ? QueuedDirectories : QueuedFiles).Add(Path);
    bQueuedAddedOrRemoved |= bAddedOrRemoved;
    bQueued.store(true, std::memory_order_release);
}
void QueuePath(const TCHAR* Path, bool bAddedOrRemoved)
{
    if (IsScript(Path))
        Queue(Path, false, bAddedOrRemoved);
    else if (MayHoldScripts(Path))
        Queue(Path, true, true);
}

// Write handle of a script: reported again when it closes, so a reload after the write sees the whole file.
class FScriptWriteHandle final : public IFileHandle
{
public:
    FScriptWriteHandle(IFileHandle* InInner, const TCHAR* InPath) : Inner(InInner), Path(InPath) {}
    virtual ~FScriptWriteHandle() override
    {
        Inner.Reset();
        Queue(*Path, false, false);
    }
    virtual int64 Tell() override { return Inner->Tell(); }
    virtual bool Seek(int64 NewPosition) override { return Inner->Seek(NewPosition); }
    virtual bool SeekFromEnd(int64 NewPositionRelativeToEnd) override
    {
        return Inner->SeekFromEnd(NewPositionRelativeToEnd);
    }
    virtual bool Read(uint8* Destination, int64 BytesToRead) override { return Inner->Read(Destination, BytesToRead); }
    virtual bool ReadAt(uint8* Destination, int64 BytesToRead, int64 Offset) override
    {
        return Inner->ReadAt(Destination, BytesToRead, Offset);
    }
    virtual bool Write(const uint8* Source, int64 BytesToWrite) override { return Inner->Write(Source, BytesToWrite); }
    virtual bool Flush(const bool bFullFlush) override { return Inner->Flush(bFullFlush); }
    virtual bool Truncate(int64 NewSize) override { return Inner->Truncate(NewSize); }
    virtual void ShrinkBuffers() override { Inner->ShrinkBuffers(); }
    virtual int64 Size() override { return Inner->Size(); }

private:
    TUniquePtr<IFileHandle> Inner;
    FString Path;
};

// Pass-through layer: every operation goes to the lower level unchanged; script writes are also queued.
class FDasScriptWriteTracker final : public IPlatformFile
{
public:
    static constexpr const TCHAR* Name = TEXT("DasScriptWrites");
    IPlatformFile* Lower = nullptr;

    virtual bool ShouldBeUsed(IPlatformFile* Inner, const TCHAR* CmdLine) const override { return true; }
    virtual bool Initialize(IPlatformFile* Inner, const TCHAR* CmdLine) override
    {
        Lower = Inner;
        return Lower != nullptr;
    }
    virtual IPlatformFile* GetLowerLevel() override { return Lower; }
    virtual void SetLowerLevel(IPlatformFile* NewLowerLevel) override { Lower = NewLowerLevel; }
    virtual const TCHAR* GetName() const override { return Name; }

    // Writes, deletes, moves and copies: forwarded, then queued.
    virtual bool DeleteFile(const TCHAR* Filename) override
    {
        const bool bResult = Lower->DeleteFile(Filename);
        QueuePath(Filename, true);
        return bResult;
    }
    virtual bool DeleteFiles(const TArrayView<const TCHAR*>& Filenames) override
    {
        const bool bResult = Lower->DeleteFiles(Filenames);
        for (const TCHAR* Filename : Filenames)
            QueuePath(Filename, true);
        return bResult;
    }
    virtual bool MoveFile(const TCHAR* To, const TCHAR* From) override
    {
        const bool bResult = Lower->MoveFile(To, From);
        QueuePath(To, true);
        QueuePath(From, true);
        return bResult;
    }
    virtual bool CopyFile(const TCHAR* To, const TCHAR* From, EPlatformFileRead ReadFlags,
                          EPlatformFileWrite WriteFlags) override
    {
        const bool bExisted = IsScript(To) && Lower->FileExists(To);
        const bool bResult = Lower->CopyFile(To, From, ReadFlags, WriteFlags);
        QueuePath(To, !bExisted);
        return bResult;
    }
    virtual IFileHandle* OpenWrite(const TCHAR* Filename, bool bAppend, bool bAllowRead) override
    {
        if (!IsScript(Filename))
            return Lower->OpenWrite(Filename, bAppend, bAllowRead);
        const bool bExisted = Lower->FileExists(Filename);
        IFileHandle* Handle = Lower->OpenWrite(Filename, bAppend, bAllowRead);
        Queue(Filename, false, !bExisted);
        return Handle ? new FScriptWriteHandle(Handle, Filename) : nullptr;
    }
    virtual FFileOpenResult OpenWrite(const TCHAR* Filename, EOpenWriteFlags Flags) override
    {
        if (!IsScript(Filename))
            return Lower->OpenWrite(Filename, Flags);
        const bool bExisted = Lower->FileExists(Filename);
        FFileOpenResult Result = Lower->OpenWrite(Filename, Flags);
        Queue(Filename, false, !bExisted);
        if (Result.HasValue())
            return MakeValue(MakeUnique<FScriptWriteHandle>(Result.StealValue().Release(), Filename));
        return Result;
    }
    virtual bool DeleteDirectory(const TCHAR* Directory) override
    {
        const bool bResult = Lower->DeleteDirectory(Directory);
        Queue(Directory, true, true);
        return bResult;
    }
    virtual bool DeleteDirectoryRecursively(const TCHAR* Directory) override
    {
        const bool bResult = Lower->DeleteDirectoryRecursively(Directory);
        Queue(Directory, true, true);
        return bResult;
    }
    virtual bool CopyDirectoryTree(const TCHAR* DestinationDirectory, const TCHAR* Source,
                                   bool bOverwriteAllExisting) override
    {
        const bool bResult = Lower->CopyDirectoryTree(DestinationDirectory, Source, bOverwriteAllExisting);
        Queue(DestinationDirectory, true, true);
        return bResult;
    }

    // Everything else: forwarded unchanged.
    virtual void SetSandboxEnabled(bool bInEnabled) override { Lower->SetSandboxEnabled(bInEnabled); }
    virtual bool IsSandboxEnabled() const override { return Lower->IsSandboxEnabled(); }
    virtual void MakeUniquePakFilesForTheseFiles(const TArray<TArray<FString>>& InFiles) override
    {
        Lower->MakeUniquePakFilesForTheseFiles(InFiles);
    }
    virtual void InitializeNewAsyncIO() override { Lower->InitializeNewAsyncIO(); }
    virtual void AddLocalDirectories(TArray<FString>& LocalDirectories) override
    {
        Lower->AddLocalDirectories(LocalDirectories);
    }
    virtual void BypassSecurity(bool bInBypass) override { Lower->BypassSecurity(bInBypass); }
    virtual void Tick() override { Lower->Tick(); }
    virtual bool FileExists(const TCHAR* Filename) override { return Lower->FileExists(Filename); }
    virtual int64 FileSize(const TCHAR* Filename) override { return Lower->FileSize(Filename); }
    virtual bool IsReadOnly(const TCHAR* Filename) override { return Lower->IsReadOnly(Filename); }
    virtual bool SetReadOnly(const TCHAR* Filename, bool bNewReadOnlyValue) override
    {
        return Lower->SetReadOnly(Filename, bNewReadOnlyValue);
    }
    virtual FDateTime GetTimeStamp(const TCHAR* Filename) override { return Lower->GetTimeStamp(Filename); }
    virtual void SetTimeStamp(const TCHAR* Filename, FDateTime DateTime) override
    {
        Lower->SetTimeStamp(Filename, DateTime);
    }
    virtual FDateTime GetAccessTimeStamp(const TCHAR* Filename) override { return Lower->GetAccessTimeStamp(Filename); }
    virtual FString GetFilenameOnDisk(const TCHAR* Filename) override { return Lower->GetFilenameOnDisk(Filename); }
    virtual ESymlinkResult IsSymlink(const TCHAR* Filename) override { return Lower->IsSymlink(Filename); }
    virtual bool HasMarkOfTheWeb(FStringView Filename, FString* OutSourceURL) override
    {
        return Lower->HasMarkOfTheWeb(Filename, OutSourceURL);
    }
    virtual bool SetMarkOfTheWeb(FStringView Filename, bool bNewStatus, const FString* InSourceURL) override
    {
        return Lower->SetMarkOfTheWeb(Filename, bNewStatus, InSourceURL);
    }
    virtual FFileOpenResult OpenRead(const TCHAR* Filename, EOpenReadFlags Flags) override
    {
        return Lower->OpenRead(Filename, Flags);
    }
    virtual IFileHandle* OpenRead(const TCHAR* Filename, bool bAllowWrite) override
    {
        return Lower->OpenRead(Filename, bAllowWrite);
    }
    virtual FFileOpenResult OpenReadNoBuffering(const TCHAR* Filename, EOpenReadFlags Flags) override
    {
        return Lower->OpenReadNoBuffering(Filename, Flags);
    }
    virtual IFileHandle* OpenReadNoBuffering(const TCHAR* Filename, bool bAllowWrite) override
    {
        return Lower->OpenReadNoBuffering(Filename, bAllowWrite);
    }
    virtual bool DirectoryExists(const TCHAR* Directory) override { return Lower->DirectoryExists(Directory); }
    virtual bool CreateDirectory(const TCHAR* Directory) override { return Lower->CreateDirectory(Directory); }
    virtual FFileStatData GetStatData(const TCHAR* FilenameOrDirectory) override
    {
        return Lower->GetStatData(FilenameOrDirectory);
    }
    virtual bool IterateDirectory(const TCHAR* Directory, FDirectoryVisitor& Visitor) override
    {
        return Lower->IterateDirectory(Directory, Visitor);
    }
    virtual bool IterateDirectoryStat(const TCHAR* Directory, FDirectoryStatVisitor& Visitor) override
    {
        return Lower->IterateDirectoryStat(Directory, Visitor);
    }
    virtual bool FileJournalIsAvailable(const TCHAR* VolumeOrPath, ELogVerbosity::Type* OutErrorLevel,
                                        FString* OutError) override
    {
        return Lower->FileJournalIsAvailable(VolumeOrPath, OutErrorLevel, OutError);
    }
    virtual uint64 FileJournalGetMaximumSize(const TCHAR* VolumeOrPath, ELogVerbosity::Type* OutErrorLevel,
                                             FString* OutError) const override
    {
        return Lower->FileJournalGetMaximumSize(VolumeOrPath, OutErrorLevel, OutError);
    }
    virtual EFileJournalResult FileJournalGetLatestEntry(const TCHAR* VolumeName, FFileJournalId& OutJournalId,
                                                         FFileJournalEntryHandle& OutEntryHandle,
                                                         FString* OutError) override
    {
        return Lower->FileJournalGetLatestEntry(VolumeName, OutJournalId, OutEntryHandle, OutError);
    }
    virtual bool FileJournalIterateDirectory(const TCHAR* Directory, FDirectoryJournalVisitorFunc Visitor,
                                             FString* OutError) override
    {
        return Lower->FileJournalIterateDirectory(Directory, Visitor, OutError);
    }
    virtual FFileJournalData FileJournalGetFileData(const TCHAR* FilenameOrDirectory, FString* OutError) override
    {
        return Lower->FileJournalGetFileData(FilenameOrDirectory, OutError);
    }
    virtual EFileJournalResult FileJournalReadModified(
        const TCHAR* VolumeName, const FFileJournalId& JournalIdOfStartingEntry,
        const FFileJournalEntryHandle& StartingJournalEntry, TMap<FFileJournalFileHandle, FString>& KnownDirectories,
        TSet<FString>& OutModifiedDirectories, FFileJournalEntryHandle& OutNextJournalEntry, FString* OutError) override
    {
        return Lower->FileJournalReadModified(VolumeName, JournalIdOfStartingEntry, StartingJournalEntry,
                                              KnownDirectories, OutModifiedDirectories, OutNextJournalEntry, OutError);
    }
    virtual FString FileJournalGetVolumeName(FStringView InPath) override
    {
        return Lower->FileJournalGetVolumeName(InPath);
    }
    virtual FFileOpenAsyncResult OpenAsyncRead(const TCHAR* Filename, EOpenReadFlags Flags) override
    {
        return Lower->OpenAsyncRead(Filename, Flags);
    }
    virtual IAsyncReadFileHandle* OpenAsyncRead(const TCHAR* Filename, bool bAllowWrite) override
    {
        return Lower->OpenAsyncRead(Filename, bAllowWrite);
    }
    virtual void SetAsyncMinimumPriority(EAsyncIOPriorityAndFlags MinPriority) override
    {
        Lower->SetAsyncMinimumPriority(MinPriority);
    }
    virtual FOpenMappedResult OpenMappedEx(const TCHAR* Filename, EOpenReadFlags OpenOptions,
                                           int64 MaximumSize) override
    {
        return Lower->OpenMappedEx(Filename, OpenOptions, MaximumSize);
    }
    virtual FOpenMappedResult OpenMappedEx2(const TCHAR* Filename, EOpenMappedFlags OpenOptions,
                                            int64 MaximumSize) override
    {
        return Lower->OpenMappedEx2(Filename, OpenOptions, MaximumSize);
    }
    virtual void GetTimeStampPair(const TCHAR* PathA, const TCHAR* PathB, FDateTime& OutTimeStampA,
                                  FDateTime& OutTimeStampB) override
    {
        Lower->GetTimeStampPair(PathA, PathB, OutTimeStampA, OutTimeStampB);
    }
    virtual FDateTime GetTimeStampLocal(const TCHAR* Filename) override { return Lower->GetTimeStampLocal(Filename); }
    virtual bool IterateDirectory(const TCHAR* Directory, FDirectoryVisitorFunc Visitor) override
    {
        return Lower->IterateDirectory(Directory, Visitor);
    }
    virtual bool IterateDirectoryStat(const TCHAR* Directory, FDirectoryStatVisitorFunc Visitor) override
    {
        return Lower->IterateDirectoryStat(Directory, Visitor);
    }
    virtual bool IterateDirectoryRecursively(const TCHAR* Directory, FDirectoryVisitor& Visitor) override
    {
        return Lower->IterateDirectoryRecursively(Directory, Visitor);
    }
    virtual bool IterateDirectoryStatRecursively(const TCHAR* Directory, FDirectoryStatVisitor& Visitor) override
    {
        return Lower->IterateDirectoryStatRecursively(Directory, Visitor);
    }
    virtual bool IterateDirectoryRecursively(const TCHAR* Directory, FDirectoryVisitorFunc Visitor) override
    {
        return Lower->IterateDirectoryRecursively(Directory, Visitor);
    }
    virtual bool IterateDirectoryStatRecursively(const TCHAR* Directory, FDirectoryStatVisitorFunc Visitor) override
    {
        return Lower->IterateDirectoryStatRecursively(Directory, Visitor);
    }
    virtual void FindFiles(TArray<FString>& FoundFiles, const TCHAR* Directory, const TCHAR* FileExtension) override
    {
        Lower->FindFiles(FoundFiles, Directory, FileExtension);
    }
    virtual void FindFilesRecursively(TArray<FString>& FoundFiles, const TCHAR* Directory,
                                      const TCHAR* FileExtension) override
    {
        Lower->FindFilesRecursively(FoundFiles, Directory, FileExtension);
    }
    virtual bool CreateDirectoryTree(const TCHAR* Directory) override { return Lower->CreateDirectoryTree(Directory); }
    virtual FString ConvertToAbsolutePathForExternalAppForRead(const TCHAR* Filename) override
    {
        return Lower->ConvertToAbsolutePathForExternalAppForRead(Filename);
    }
    virtual FString ConvertToAbsolutePathForExternalAppForWrite(const TCHAR* Filename) override
    {
        return Lower->ConvertToAbsolutePathForExternalAppForWrite(Filename);
    }
    virtual bool SendMessageToServer(const TCHAR* Message, IFileServerMessageHandler* Handler) override
    {
        return Lower->SendMessageToServer(Message, Handler);
    }
    virtual bool DoesCreatePublicFiles() override { return Lower->DoesCreatePublicFiles(); }
    virtual void SetCreatePublicFiles(bool bCreatePublicFiles) override
    {
        Lower->SetCreatePublicFiles(bCreatePublicFiles);
    }
    virtual int64 GetAllowedBytesToWriteThrottledStorage(const TCHAR* DestinationPath) override
    {
        return Lower->GetAllowedBytesToWriteThrottledStorage(DestinationPath);
    }
};
// Static storage: IO threads may still be inside the layer when the module removes it.
FDasScriptWriteTracker Tracker;
}

void DasInstallScriptWriteTracker()
{
    auto& Manager = FPlatformFileManager::Get();
    if (Manager.FindPlatformFile(FDasScriptWriteTracker::Name))
        return;
    if (Tracker.Initialize(&Manager.GetPlatformFile(), TEXT("")))
        Manager.SetPlatformFile(Tracker);
}
void DasRemoveScriptWriteTracker()
{
    auto& Manager = FPlatformFileManager::Get();
    if (Manager.FindPlatformFile(FDasScriptWriteTracker::Name) == &Tracker)
        Manager.RemovePlatformFile(&Tracker);
}
bool DasTakeScriptWrites(TArray<FString>& Files, TArray<FString>& Directories, bool& bAddedOrRemoved)
{
    if (!bQueued.load(std::memory_order_acquire))
        return false;
    FScopeLock Lock(&QueueLock);
    Files = QueuedFiles.Array();
    Directories = QueuedDirectories.Array();
    bAddedOrRemoved = bQueuedAddedOrRemoved;
    QueuedFiles.Reset();
    QueuedDirectories.Reset();
    bQueuedAddedOrRemoved = false;
    bQueued.store(false, std::memory_order_relaxed);
    return true;
}
