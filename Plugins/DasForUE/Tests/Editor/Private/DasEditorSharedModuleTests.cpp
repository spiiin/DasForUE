#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "DasScriptClassData.h"
#include "Engine/World.h"
#include "HAL/FileManager.h"
#include "HAL/IConsoleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "UObject/GarbageCollection.h"
#include "UObject/StrongObjectPtr.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasSharedEditorModulesTest, "DasForUE.Runtime.ProgramCacheSharedModules",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasSharedEditorModulesTest::RunTest(const FString&)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    auto* Shared = IConsoleManager::Get().FindConsoleVariable(TEXT("das.EditorSharedModules"));
    auto* Batch = IConsoleManager::Get().FindConsoleVariable(TEXT("das.EditorSourceBatchCache"));
    auto* Strict = IConsoleManager::Get().FindConsoleVariable(TEXT("das.RequirePreparedPrograms"));
    const int32 OldStrict = Strict->GetInt();
    Strict->Set(0, ECVF_SetByCode);
    const int32 OldShared = Shared->GetInt(), OldBatch = Batch->GetInt();
    Shared->Set(1, ECVF_SetByCode);
    Batch->Set(1, ECVF_SetByCode);
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString HelperName = TEXT("ue_cache_fixture_") + Id;
    const FString Helper = IPluginManager::Get().FindPlugin(TEXT("DasForUE"))->GetContentDir() /
        TEXT("Das/Scripts") / (HelperName + TEXT(".das"));
    const FString A = R.ScriptPath(TEXT("shared_root_a_") + Id + TEXT(".das"));
    const FString B = R.ScriptPath(TEXT("shared_root_b_") + Id + TEXT(".das"));
    TStrongObjectPtr<UObject> First(NewObject<UDasScriptClassData>()), Second(NewObject<UDasScriptClassData>());
    const FString Prepared = FPaths::ProjectContentDir() / TEXT("Das/Prepared");
    TArray<FString> BeforeArtifacts;
    IFileManager::Get().FindFiles(BeforeArtifacts, *(Prepared / TEXT("*.dasp")), true, false);
    ON_SCOPE_EXIT
    {
        R.ReleaseObject(First.Get());
        R.ReleaseObject(Second.Get());
        R.ClearProgramCache();
        for (const auto& File : {A, B, Helper})
            IFileManager::Get().Delete(*File, false, true);
        R.NotifyScriptsChanged({A, B, Helper}, true);
        TArray<FString> Artifacts;
        IFileManager::Get().FindFiles(Artifacts, *(Prepared / TEXT("*.dasp")), true, false);
        for (const auto& File : Artifacts)
            if (!BeforeArtifacts.Contains(File))
                IFileManager::Get().Delete(*(Prepared / File), false, true);
        Strict->Set(OldStrict, ECVF_SetByCode);
        Shared->Set(OldShared, ECVF_SetByCode);
        Batch->Set(OldBatch, ECVF_SetByCode);
    };
    auto Write = [&](const FString& File, const FString& Source)
    {
        TestTrue(TEXT("Write shared-module fixture"), FFileHelper::SaveStringToFile(Source, *File,
            FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM));
        R.NotifyScriptsChanged({File}, true);
    };
    auto HelperSource = [&](int Value)
    {
        return TEXT("options gen2\nmodule ") + HelperName + TEXT(" public\nrequire ue_api_world public\nrequire ue_dsl public\ndef shared_value():int { return ") +
            FString::FromInt(Value) + TEXT(" }\ndef alternate_value():int { return shared_value()+7 }\n");
    };
    auto RootSource = [&](const FString& Name, bool Alternate)
    {
        return (TEXT("options gen2\nmodule ") + Name + TEXT(" public\nrequire ") + HelperName + TEXT(R"DAS(
var counter:int=0
[ue_class(Blueprintable)]
class SharedActor : Actor {
    @ue_property=(BlueprintReadWrite)
    Bonus:int=shared_value()

    [ue_function(BlueprintCallable)]
    def Add(value:int):int { return self.Bonus+value }
}
[export]
def main(input:int):int { counter+=input; return counter+VALUE_FUNCTION() }
)DAS")).Replace(TEXT("VALUE_FUNCTION"), Alternate ? TEXT("alternate_value") : TEXT("shared_value"));
    };
    Write(Helper, HelperSource(10));
    Write(A, RootSource(TEXT("cache_a_") + Id, false));
    Write(B, RootSource(TEXT("cache_b_") + Id, true));
    R.ClearProgramCache();
    FString Error;
    FDasAuthoringModule Model;
    {
        FDasEditorSourceReadScope Scope;
        if (!TestTrue(TEXT("Compile first authored root: ") + Error, R.PrepareEditorPrograms({A}, Error)))
            return false;
        const uint64 Hits = R.ProgramCacheStats().SharedModuleHits;
        if (!TestTrue(TEXT("Compile second authored root: ") + Error, R.PrepareEditorPrograms({B}, Error)))
            return false;
        TestTrue(TEXT("Second root reuses native helper AST"), R.ProgramCacheStats().SharedModuleHits > Hits);
        if (!TestTrue(TEXT("Read authored defaults: ") + Error, R.ReadAuthoringScript(B, Model, Error)))
            return false;
        if (TestEqual(TEXT("Root retains its authored model"), Model.Classes.Num(), 1) &&
            TestEqual(TEXT("Root retains its property"), Model.Classes[0].Properties.Num(), 1))
            TestEqual(TEXT("Resolved default was not replaced by collect placeholder"),
                Model.Classes[0].Properties[0].Default.Number, 10.0);
    }
    // Registry is gone. AST owners must still outlive independent VM contexts and cache eviction.
    auto Read = [&](const FString& File, UObject* Owner, int Input, int Expected)
    {
        const auto Result = R.RunPersistent(File, Input, Owner);
        TestTrue(TEXT("Run shared helper after batch: ") + Result.Error, Result.bSucceeded);
        TestEqual(TEXT("Correct isolated root state"), Result.Value, Expected);
    };
    Read(A, First.Get(), 2, 12);
    Read(B, Second.Get(), 4, 21);
    R.ReleaseObject(First.Get());
    R.ClearProgramCache();
    CollectGarbage(RF_NoFlags);
    Read(B, Second.Get(), 1, 22);
    if (!TestTrue(TEXT("Export a root with borrowed helpers: ") + Error, R.ExportPreparedProgram(B, Error)))
        return false;
    R.ReleaseObject(Second.Get());
    R.ClearProgramCache();
    Strict->Set(1, ECVF_SetByCode);
    Read(B, Second.Get(), 4, 21);
    Strict->Set(0, ECVF_SetByCode);
    R.ReleaseObject(Second.Get());
    FString SharedModel;
    {
        FDasEditorSourceReadScope Scope;
        if (!TestTrue(TEXT("Seed registry before an in-batch write: ") + Error, R.PrepareEditorPrograms({A, B}, Error)))
            return false;
        Write(Helper, HelperSource(30));
        if (!TestTrue(TEXT("Edited dependency recompiles both roots: ") + Error,
            R.PrepareEditorPrograms({A, B}, Error)))
            return false;
        if (!TestTrue(TEXT("Read changed default: ") + Error, R.ReadAuthoringScript(B, Model, Error)))
            return false;
        if (Model.Classes.Num() == 1 && Model.Classes[0].Properties.Num() == 1)
            TestEqual(TEXT("Borrowed source participates in invalidation"), Model.Classes[0].Properties[0].Default.Number, 30.0);
        TestTrue(TEXT("Serialize shared authored model"), Model.ToJson(SharedModel, Error));
        Read(A, First.Get(), 2, 32);
        Read(B, Second.Get(), 4, 41);
    }
    R.ReleaseObject(First.Get());
    R.ReleaseObject(Second.Get());
    R.ClearProgramCache();
    Shared->Set(0, ECVF_SetByCode);
    const uint64 Hits = R.ProgramCacheStats().SharedModuleHits;
    if (!TestTrue(TEXT("Fallback without module sharing: ") + Error, R.PrepareEditorPrograms({A, B}, Error)))
        return false;
    TestEqual(TEXT("Disabled sharing adds no hits"), R.ProgramCacheStats().SharedModuleHits, Hits);
    Read(B, Second.Get(), 4, 41);
    FDasAuthoringModule Fresh;
    FString FreshJson;
    if (TestTrue(TEXT("Read fallback authored model"), R.ReadAuthoringScript(B, Fresh, Error)) &&
        TestTrue(TEXT("Serialize fallback model"), Fresh.ToJson(FreshJson, Error)))
        TestEqual(TEXT("Sharing preserves the complete authored model"), FreshJson, SharedModel);
    R.ReleaseObject(First.Get());
    R.ReleaseObject(Second.Get());
    Shared->Set(1, ECVF_SetByCode);
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    R.ClearProgramCache();
    const uint64 WorldHits = R.ProgramCacheStats().SharedModuleHits;
    const bool WorldReady = R.PreparePrograms(World.Get(), {A, B}, Error);
    TestTrue(TEXT("World preparation creates its own shared compilation scope: ") + Error, WorldReady);
    TestTrue(TEXT("Configured world roots reuse helpers without a caller scope"),
        R.ProgramCacheStats().SharedModuleHits > WorldHits);
    World->DestroyWorld(false);
    return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasEditorExclusionsTest, "DasForUE.Runtime.ProgramCacheEditorExclusions",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasEditorExclusionsTest::RunTest(const FString&)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Section = TEXT("DasForUE.EditorPreparation"), Programs = TEXT("DasForUE.Programs");
    TArray<FString> OldDirectories, OldPrograms;
    GConfig->GetArray(*Section, TEXT("ExcludedDirectories"), OldDirectories, GGameIni);
    GConfig->GetArray(*Programs, TEXT("Scripts"), OldPrograms, GGameIni);
    const FString Directory = TEXT("excluded_fixture_") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString Relative = Directory / TEXT("unused.das");
    const FString FullDirectory = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() / TEXT("Das/Scripts") / Directory);
    const FString Path = FullDirectory / TEXT("unused.das");
    const FString Root = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() / TEXT("Das/Scripts") / (Directory + TEXT("_root.das")));
    IFileManager::Get().MakeDirectory(*FullDirectory, true);
    ON_SCOPE_EXIT
    {
        GConfig->SetArray(*Section, TEXT("ExcludedDirectories"), OldDirectories, GGameIni);
        GConfig->SetArray(*Programs, TEXT("Scripts"), OldPrograms, GGameIni);
        R.ClearProgramCache();
        IFileManager::Get().Delete(*Root, false, true);
        IFileManager::Get().Delete(*Path, false, true);
        IFileManager::Get().DeleteDirectory(*FullDirectory);
        R.NotifyScriptsChanged({Path, Root}, true);
    };
    TestTrue(TEXT("Write invalid unused root"), FFileHelper::SaveStringToFile(TEXT("this is intentionally invalid dascript"), *Path));
    GConfig->SetArray(*Section, TEXT("ExcludedDirectories"), {Directory}, GGameIni);
    R.NotifyScriptsChanged({Path}, true);
    FString Error;
    TestTrue(TEXT("Exclusion resolves the test directory"), R.ExcludedEditorScripts(nullptr).Contains(Path));
    TestTrue(TEXT("Automatic preparation skips excluded root"), R.PrepareEditorChanges({Path}, false, Error));
    TestFalse(TEXT("Explicit preparation still validates excluded root"), R.PrepareEditorPrograms({Path}, Error));
    TestTrue(TEXT("Write dependent root"), FFileHelper::SaveStringToFile(TEXT("options gen2\nrequire ") + Directory + TEXT("/unused\n"), *Root));
    R.NotifyScriptsChanged({Root}, true);
    TestFalse(TEXT("Exclusion does not hide errors in required dependencies"), R.PrepareEditorPrograms({Root}, Error));
    GConfig->SetArray(*Programs, TEXT("Scripts"), {Relative}, GGameIni);
    TestFalse(TEXT("Explicit configured program takes precedence"), R.ExcludedEditorScripts(nullptr).Contains(Path));
    TestFalse(TEXT("Configured test root is validated automatically"), R.PrepareEditorChanges({Path}, false, Error));
    return !HasAnyErrors();
}
