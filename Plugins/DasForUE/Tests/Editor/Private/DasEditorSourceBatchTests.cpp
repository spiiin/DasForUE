#include "DasRuntime.h"
#include "DasScriptClassData.h"
#include "HAL/FileManager.h"
#include "HAL/IConsoleManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "UObject/StrongObjectPtr.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasEditorSourceBatchTest, "DasForUE.Runtime.EditorSourceBatch",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasEditorSourceBatchTest::RunTest(const FString&)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    auto* Batch = IConsoleManager::Get().FindConsoleVariable(TEXT("das.EditorSourceBatchCache"));
    auto* Disk = IConsoleManager::Get().FindConsoleVariable(TEXT("das.EditorModuleCache"));
    auto* Strict = IConsoleManager::Get().FindConsoleVariable(TEXT("das.RequirePreparedPrograms"));
    const int32 OldBatch = Batch->GetInt(), OldDisk = Disk->GetInt(), OldStrict = Strict->GetInt();
    Batch->Set(1, ECVF_SetByCode);
    Disk->Set(0, ECVF_SetByCode);
    Strict->Set(0, ECVF_SetByCode);
    R.ClearProgramCache();
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString Dir = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() / TEXT("Das/Scripts"));
    const FString Module = TEXT("source_batch_") + Id, Dep = Dir / (Module + TEXT(".das"));
    const FString Missing = Dir / (Module + TEXT("_missing.das"));
    TArray<FString> Roots;
    for (int32 I = 0; I < 3; ++I)
        Roots.Add(Dir / (Module + FString::Printf(TEXT("_%d.das"), I)));
    TStrongObjectPtr<UObject> Owner(NewObject<UDasScriptClassData>());
    ON_SCOPE_EXIT
    {
        R.ReleaseObject(Owner.Get());
        R.ClearProgramCache();
        for (const auto& Path : Roots)
            IFileManager::Get().Delete(*Path, false, true);
        IFileManager::Get().Delete(*Dep, false, true);
        IFileManager::Get().Delete(*Missing, false, true);
        Batch->Set(OldBatch, ECVF_SetByCode);
        Disk->Set(OldDisk, ECVF_SetByCode);
        Strict->Set(OldStrict, ECVF_SetByCode);
    };
    IFileManager::Get().MakeDirectory(*Dir, true);
    auto Save = [&](const FString& Path, const FString& Source)
    {
        return TestTrue(TEXT("Write fixture"), FFileHelper::SaveStringToFile(
                                                   Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM));
    };
    auto Dependency = [&](int32 Value)
    {
        return TEXT("options gen2\nmodule ") + Module +
                    FString::Printf(TEXT(" public\ndef dependency_value():int {return %d}\n"), Value);
    };
    const FString Source = TEXT("options gen2\nrequire ") + Module +
                                TEXT("\n[export]\ndef main(input:int):int {return input+dependency_value()}\n");
    if (!Save(Dep, Dependency(10)))
        return false;
    for (const auto& Path : Roots)
        if (!Save(Path, Source))
            return false;
    R.NotifyScriptsChanged({}, true);
    FString Error;
    if (!TestTrue(TEXT("Cold batch prepares all roots: ") + Error, R.PrepareEditorPrograms(Roots, Error)))
        return false;
    TestEqual(TEXT("Outermost scope releases snapshots"), R.ProgramCacheStats().SourceSnapshotsInBatch, 0);
    Batch->Set(0, ECVF_SetByCode);
    auto Before = R.ProgramCacheStats();
    if (!TestTrue(TEXT("Source-only repeated validation"), R.PrepareEditorPrograms(Roots, Error)))
        return false;
    auto After = R.ProgramCacheStats();
    const uint64 BaselineReads = After.SourcePhysicalReads - Before.SourcePhysicalReads;
    const uint64 BaselineBytes = After.SourceBytesRead - Before.SourceBytesRead;
    const uint64 BaselineChecks = After.SourceContentChecks - Before.SourceContentChecks;
    TestEqual(TEXT("Baseline reuses compiled programs"), After.Compilations, Before.Compilations);
    Batch->Set(1, ECVF_SetByCode);
    Before = R.ProgramCacheStats();
    if (!TestTrue(TEXT("Batched repeated validation"), R.PrepareEditorPrograms(Roots, Error)))
        return false;
    After = R.ProgramCacheStats();
    const uint64 CachedReads = After.SourcePhysicalReads - Before.SourcePhysicalReads;
    const uint64 CachedBytes = After.SourceBytesRead - Before.SourceBytesRead;
    TestTrue(TEXT("Shared dependency reads eliminated"), CachedReads < BaselineReads);
    TestTrue(TEXT("Fewer bytes read"), CachedBytes < BaselineBytes);
    TestEqual(TEXT("Every root dependency is still checked"), After.SourceContentChecks - Before.SourceContentChecks,
                   BaselineChecks);
    TestEqual(TEXT("Cached validation never compiles unchanged roots"), After.Compilations, Before.Compilations);
    TestTrue(TEXT("Snapshot hits recorded"), After.SourceSnapshotHits > Before.SourceSnapshotHits);
    {
        FDasEditorSourceReadScope Outer;
        Before = R.ProgramCacheStats();
        if (!R.PrepareEditorPrograms({Roots[0]}, Error))
        {
            AddError(Error);
            return false;
        }
        const auto First = R.ProgramCacheStats();
        if (!R.PrepareEditorPrograms({Roots[1]}, Error))
        {
            AddError(Error);
            return false;
        }
        After = R.ProgramCacheStats();
        TestTrue(TEXT("Startup's individual root calls share the outer batch"),
                      After.SourcePhysicalReads - First.SourcePhysicalReads <
                          First.SourcePhysicalReads - Before.SourcePhysicalReads);
        TestTrue(TEXT("Snapshots live only within the outer pass"), After.SourceSnapshotsInBatch > 0);
    }
    TestEqual(TEXT("Nested scopes release all pass snapshots"), R.ProgramCacheStats().SourceSnapshotsInBatch, 0);
    const auto Stamp = IFileManager::Get().GetTimeStamp(*Dep);
    if (!Save(Dep, Dependency(11)))
        return false;
    IFileManager::Get().SetTimeStamp(*Dep,
                                     Stamp); // same-size bytes and same timestamp: a stat-only cache would miss this.
    Before = R.ProgramCacheStats();
    if (!TestTrue(TEXT("New pass checks content without a watcher notification"),
                       R.PrepareEditorPrograms(Roots, Error)))
        return false;
    TestEqual(TEXT("All three stale roots recompile"), R.ProgramCacheStats().Compilations - Before.Compilations,
                   uint64(3));
    auto Run = [&](const FString& Path, int32 Expected)
    {
        const auto Result = R.RunPersistent(Path, 1, Owner.Get());
        return TestTrue(TEXT("Prepared execution: ") + Result.Error, Result.bSucceeded && Result.Value == Expected);
    };
    if (!Run(Roots[0], 12))
        return false;
    {
        FDasEditorSourceReadScope Outer;
        if (!R.PrepareEditorPrograms({Roots[0]}, Error))
        {
            AddError(Error);
            return false;
        }
        Before = R.ProgramCacheStats();
        if (!Save(Dep, Dependency(22)))
            return false;
        R.NotifyScriptsChanged({Dep});
        TestTrue(TEXT("Write invalidates active snapshot map"),
                      R.ProgramCacheStats().SourceBatchInvalidations > Before.SourceBatchInvalidations);
        if (!TestTrue(TEXT("Save refreshes every root within active pass"), R.PrepareEditorPrograms(Roots, Error)) ||
                      !Run(Roots[0], 23))
            return false;
    }
    {
        FDasEditorSourceReadScope Outer;
        TestFalse(TEXT("Missing file is diagnosed"), R.PrepareEditorPrograms({Missing}, Error));
        if (!Save(Missing, TEXT("options gen2\n[export]\ndef main(input:int):int {return input+4}\n")))
            return false;
        R.NotifyScriptsChanged({Missing}, true);
        if (!TestTrue(TEXT("Creation invalidates negative snapshots"), R.PrepareEditorPrograms({Missing}, Error)) ||
                      !Run(Missing, 5))
            return false;
    }
    if (!Save(Roots[0], TEXT("options gen2\nthis is invalid syntax\n")))
        return false;
    R.NotifyScriptsChanged({Roots[0]});
    TestFalse(TEXT("Failed saved candidate rejected"), R.PrepareEditorChanges({Roots[0]}, false, Error));
    if (!Run(Roots[0], 23))
        return false;
    if (!Save(Roots[0], Source))
        return false;
    R.NotifyScriptsChanged({Roots[0]});
    if (!TestTrue(TEXT("Repaired source prepares"), R.PrepareEditorChanges({Roots[0]}, false, Error)) ||
                  !Run(Roots[0], 23))
        return false;
    TestEqual(TEXT("No snapshots retained after preparation"), R.ProgramCacheStats().SourceSnapshotsInBatch, 0);
    UE_LOG(
        LogTemp, Display,
        TEXT("DAS_EDITOR_SOURCE_BATCH PASS baseline_reads=%llu cached_reads=%llu baseline_bytes=%llu cached_bytes=%llu dependency_checks=%llu"),
            BaselineReads, CachedReads, BaselineBytes, CachedBytes, BaselineChecks);
    return true;
}
