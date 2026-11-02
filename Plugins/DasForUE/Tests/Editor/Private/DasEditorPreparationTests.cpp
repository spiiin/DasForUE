#include "DasAutoReimport.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "DasProbeActor.h"
#include "DasScriptClassData.h"
#include "Engine/Blueprint.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "HAL/FileManager.h"
#include "HAL/IConsoleManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "UObject/StrongObjectPtr.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasEditorPreparationTest, "DasForUE.Runtime.EditorPreparation",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasEditorPreparationTest::RunTest(const FString&)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    auto* Strict = IConsoleManager::Get().FindConsoleVariable(TEXT("das.RequirePreparedPrograms"));
    const int32 PreviousStrict = Strict->GetInt();
    Strict->Set(0, ECVF_SetByCode);
    R.ClearProgramCache();
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString Directory = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() / TEXT("Das/Scripts"));
    IFileManager::Get().MakeDirectory(*Directory, true);
    const FString File = TEXT("editor_preparation_") + Id + TEXT(".das"), Path = Directory / File;
    const FString Dependency = TEXT("editor_dependency_") + Id, DepPath = Directory / (Dependency + TEXT(".das"));
    const FString OtherFile = TEXT("editor_unknown_") + Id + TEXT(".das"), OtherPath = Directory / OtherFile;
    const FString LatePath = Directory / (TEXT("editor_late_") + Id + TEXT(".das"));
    const FString Source = TEXT("options gen2\nrequire ue_dsl\nrequire ") + Dependency + TEXT(R"DAS(
[ue_class(module_id="editorprep",Blueprintable)]
class PreparedEditorActor : Actor {
    @ue_property=(BlueprintReadWrite)
    Score:int=2
    [ue_function(BlueprintCallable)]
    def Add(value:int):int { self.Score+=value; return self.Score }
}
var counter:int=0
[export]
def main(input:int):int { counter+=input; return counter+dependency_value() }
)DAS");
    TStrongObjectPtr<UWorld> World;
    TArray<ADasProbeActor*> Actors;
    TStrongObjectPtr<UObject> Worldless(NewObject<UDasScriptClassData>());
    ON_SCOPE_EXIT
    {
        R.SetEditorGameplayActive(false);
        R.ReleaseObject(Worldless.Get());
        for (auto* Actor : Actors)
            if (IsValid(Actor))
            {
                R.ReleaseObject(Actor);
                Actor->Destroy();
            }
        if (World.IsValid())
        {
            GEngine->DestroyWorldContext(World.Get());
            World->DestroyWorld(false);
        }
        for (const auto& P : {Path, DepPath, OtherPath, LatePath})
            IFileManager::Get().Delete(*P, false, true);
        R.ClearProgramCache();
        Strict->Set(PreviousStrict, ECVF_SetByCode);
    };
    auto Write = [&](const FString& P, const FString& Text)
    {
        const bool OK = FFileHelper::SaveStringToFile(Text, *P, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
        TestTrue(TEXT("Write editor fixture"), OK);
        R.NotifyScriptsChanged({P});
    };
    auto WriteDependency = [&](int32 Value)
    {
        Write(DepPath, TEXT("options gen2\nmodule ") + Dependency +
                            FString::Printf(TEXT(" public\ndef dependency_value():int {return %d}\n"), Value));
    };
    WriteDependency(10);
    Write(Path, Source);
    Write(OtherPath, TEXT("options gen2\n[export]\ndef main(input:int):int {return input+1}\n"));
    const auto BeforeAuthoring = R.ProgramCacheStats();
    FString Error;
    UBlueprint* RawBlueprint = nullptr;
    if (!FDasDSLService::Apply(File, TEXT("/Game/Tests/EditorPreparation/") + Id +
                                          TEXT("/BP_Actor"), TEXT("editorprep.PreparedEditorActor"), RawBlueprint,
                                                                  Error))
    {
        AddError(Error);
        return false;
    }
    TStrongObjectPtr<UBlueprint> Blueprint(RawBlueprint);
    FDasAuthoringModule Model;
    TestTrue(TEXT("Repeated metadata read"), R.ReadAuthoringScript(Path, Model, Error));
    TestTrue(TEXT("Method preflight"), R.ValidateIntMethod(Path, Error, TEXT("ue_PreparedEditorActor_Add")));
    TestEqual(TEXT("Authoring and preflight share one compiled program"),
                   R.ProgramCacheStats().Compilations - BeforeAuthoring.Compilations, uint64(1));
    TestEqual(TEXT("Authoring never simulates owner globals"), R.ProgramCacheStats().Simulations,
                   BeforeAuthoring.Simulations);
    R.ClearProgramCache();
    FDasAutoReimport Editor;
    if (!Editor.PrepareForPlay(Error))
    {
        AddError(Error);
        return false;
    }
    auto Permission = Editor.RequestPIEPermissionInternal(false);
    TestTrue(TEXT("Prepared PIE is authorized"), Permission.HasValue() && Permission.GetValue());
    World.Reset(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    R.SetEditorGameplayActive(true);
    auto Spawn = [&]()
    {
        auto* Actor = World->SpawnActor<ADasProbeActor>();
        Actors.Add(Actor);
        return Actor;
    };
    auto* A = Spawn();
    auto* B = Spawn();
    auto* C = Spawn();
    if (!A || !B || !C)
    {
        AddError(TEXT("Cannot spawn owners"));
        return false;
    }
    const auto Ready = R.ProgramCacheStats();
    auto Unbound = R.RunPersistent(OtherPath, 6, Worldless.Get());
    TestTrue(TEXT("Full preparation includes scripts with no Blueprint"), Unbound.bSucceeded && Unbound.Value == 7);
    TestEqual(TEXT("Unbound script was already compiled"), R.ProgramCacheStats().Compilations, Ready.Compilations);
    auto Result = R.RunPersistent(Path, 5, A);
    TestTrue(TEXT("Prepared gameplay executes"), Result.bSucceeded && Result.Value == 15);
    Result = R.RunPersistent(Path, 0, B);
    TestTrue(TEXT("Independent owner globals"), Result.bSucceeded && Result.Value == 10);
    TestEqual(TEXT("Gameplay never compiles"), R.ProgramCacheStats().Compilations, Ready.Compilations);
    const FString HotSource =
        Source.Replace(TEXT("return counter+dependency_value()"), TEXT("return counter+dependency_value()+100"));
    Write(Path, HotSource);
    Result = R.RunPersistent(Path, 0, A);
    TestTrue(TEXT("Before queued preparation, keep working code"),
                  Result.bSucceeded && Result.Value == 15 && Result.ReloadError.IsEmpty());
    Result = R.RunPersistent(Path, 0, C);
    TestTrue(TEXT("A new owner refuses unprepared code"),
                  !Result.bSucceeded && Result.Error.Contains(TEXT("not prepared")));
    TestEqual(TEXT("A stale call does not compile"), R.ProgramCacheStats().Compilations, Ready.Compilations);
    if (!Editor.PrepareSavedPrograms({Path}, false, Error))
    {
        AddError(Error);
        return false;
    }
    TestEqual(TEXT("Save compiles the root once"), R.ProgramCacheStats().Compilations - Ready.Compilations, uint64(1));
    const auto Saved = R.ProgramCacheStats();
    Result = R.RunPersistent(Path, 0, A);
    TestTrue(TEXT("Old owner adopts prepared code"), Result.bSucceeded && Result.Value == 110);
    Result = R.RunPersistent(Path, 0, C);
    TestTrue(TEXT("A previously refused owner recovers without another file edit"),
                  Result.bSucceeded && Result.Value == 110);
    TestEqual(TEXT("Adoption does not compile"), R.ProgramCacheStats().Compilations, Saved.Compilations);
    WriteDependency(20);
    if (!Editor.PrepareSavedPrograms({DepPath}, false, Error))
    {
        AddError(Error);
        return false;
    }
    TestEqual(TEXT("A dependency save updates the module and its dependent root"),
                   R.ProgramCacheStats().Compilations - Saved.Compilations, uint64(2));
    Result = R.RunPersistent(Path, 0, A);
    TestTrue(TEXT("Dependency update executes from cache"), Result.bSucceeded && Result.Value == 120);
    Write(Path, TEXT("options gen2\ndef broken(\n"));
    TestFalse(TEXT("Broken saved source is rejected"), Editor.PrepareSavedPrograms({Path}, false, Error));
    const auto Broken = R.ProgramCacheStats();
    Result = R.RunPersistent(Path, 0, A);
    TestTrue(TEXT("Compiler errors keep the last working VM"),
                  Result.bSucceeded && Result.Value == 120 && !Result.ReloadError.IsEmpty());
    TestFalse(TEXT("Broken source denies PIE preparation"), Editor.PrepareForPlay(Error));
    Permission = Editor.RequestPIEPermissionInternal(false);
    TestTrue(TEXT("The PIE authorizer rejects a compile error"), Permission.HasError());
    TestEqual(TEXT("Repeated broken attempts do not compile again"), R.ProgramCacheStats().Compilations,
                   Broken.Compilations);
    const FString Structural = HotSource.Replace(
        TEXT("Score:int=2"), TEXT("Score:int=2\n    @ue_property=(BlueprintReadWrite)\n    Extra:int=1"));
    Write(Path, Structural);
    TestFalse(TEXT("Unapplied structural change blocks PIE"), Editor.PrepareForPlay(Error));
    TestTrue(TEXT("Actionable reimport diagnostic"), Error.Contains(TEXT("Apply/reimport")));
    Write(Path, HotSource);
    if (!Editor.PrepareForPlay(Error))
    {
        AddError(Error);
        return false;
    }
    Permission = Editor.RequestPIEPermissionInternal(false);
    TestTrue(TEXT("A repair re-enables PIE"), Permission.HasValue() && Permission.GetValue());
    Write(OtherPath, TEXT("options gen2\ndef unused_broken(\n"));
    TestFalse(TEXT("An unused script error is reported before Play"), Editor.PrepareAllPrograms(Error));
    TestTrue(TEXT("Unused script diagnostic identifies its source"), Error.Contains(OtherFile));
    Write(OtherPath, TEXT("options gen2\n[export]\ndef main(input:int):int {return input+1}\n"));
    if (!Editor.PrepareAllPrograms(Error))
    {
        AddError(Error);
        return false;
    }
    Write(LatePath, TEXT("options gen2\n[export]\ndef main(input:int):int {return input+1}\n"));
    const auto Unknown = R.ProgramCacheStats();
    Result = R.RunPersistent(LatePath, 6, Worldless.Get());
    TestTrue(TEXT("A file created after preparation does not compile from a callback"),
                  !Result.bSucceeded && Result.Error.Contains(TEXT("not prepared")));
    TestEqual(TEXT("No lazy source compilation"), R.ProgramCacheStats().Compilations, Unknown.Compilations);
    if (!Editor.PrepareSavedPrograms({LatePath}, false, Error))
    {
        AddError(Error);
        return false;
    }
    const auto LateReady = R.ProgramCacheStats();
    Result = R.RunPersistent(LatePath, 6, Worldless.Get());
    TestTrue(TEXT("A newly saved file becomes callable after the queue prepares it"),
                  Result.bSucceeded && Result.Value == 7);
    TestEqual(TEXT("A previously refused callback never compiles"), R.ProgramCacheStats().Compilations,
                   LateReady.Compilations);
    return !HasAnyErrors();
}
