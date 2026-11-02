#include "DasRuntime.h"
#include "DasScriptClassData.h"
#include "DasDSLService.h"
#include "DasProbeActor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "HAL/FileManager.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformTime.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/ScopeExit.h"
#include "UObject/GarbageCollection.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/Package.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasProgramCacheTest, "DasForUE.Runtime.ProgramCache",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasProgramCacheTest::RunTest(const FString&)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    auto* Limit = IConsoleManager::Get().FindConsoleVariable(TEXT("das.CompiledProgramCacheEntries"));
    const int32 OldLimit = Limit->GetInt();
    Limit->Set(32, ECVF_SetByCode);
    R.ClearProgramCache();
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString File = TEXT("program_cache_") + Id + TEXT(".das"), Path = R.ScriptPath(File);
    const FString DepName = TEXT("program_cache_dep_") + Id, Dep = R.ScriptPath(DepName + TEXT(".das"));
    const FString OtherPath = R.ScriptPath(TEXT("program_cache_other_") + Id + TEXT(".das"));
    const FString Source = TEXT("options gen2\nrequire ue_dsl\nrequire ") + DepName + TEXT(R"DAS(
var counter:int=0
var values:array<int>
var initial:int=dependency_value()
let shared immutable_data <- [4,5]
[export]
def begin_play():void { counter+=1; push(values,100) }
[export]
def bind_events():void { ue_subscribe_object_int(ue_self_object(),"OnDasSignal","on_signal") }
[export]
def on_signal(value:int):void { counter+=value }
[export]
def main(input:int):int { counter+=input; return counter*100+length(values)*10+initial+immutable_data[input & 1]-4-(input & 1) }
)DAS");
    auto Write = [&](const FString& P, const FString& S)
    {
        const bool OK = FFileHelper::SaveStringToFile(S, *P, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
        TestTrue(TEXT("Write source fixture"), OK);
        R.NotifyScriptsChanged({P});
        return OK;
    };
    Write(Dep, TEXT("options gen2\nmodule ") + DepName + TEXT(" public\ndef dependency_value():int {return 3}\n"));
    Write(Path, Source);
    Write(OtherPath, TEXT("options gen2\n[export]\ndef main(input:int):int {return input+1}\n"));
    auto* BP = FKismetEditorUtilities::CreateBlueprint(ADasProbeActor::StaticClass(),
                                                       CreatePackage(*(TEXT("/Game/Tests/ProgramCache/") + Id)),
                                                                     TEXT("BP_ProjectileCache"), BPTYPE_Normal,
                                                                          UBlueprint::StaticClass(),
                                                                          UBlueprintGeneratedClass::StaticClass());
    if (!TestNotNull(TEXT("Blueprint fixture"), BP))
        return false;
    auto* Default = CastChecked<ADasProbeActor>(BP->GeneratedClass->GetDefaultObject());
    Default->LifecycleScript = File;
    CastChecked<UBlueprintGeneratedClass>(BP->GeneratedClass)->UpdateCustomPropertyListForPostConstruction();
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    World->InitializeActorsForPlay(FURL());
    World->GetWorldSettings()->NotifyBeginPlay();
    TArray<ADasProbeActor*> Actors;
    ON_SCOPE_EXIT
    {
        World->EndPlay(EEndPlayReason::Quit);
        for (auto* A : Actors)
            if (IsValid(A))
            {
                R.ReleaseObject(A);
                A->Destroy();
            }
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
        for (const auto& P : {Path, Dep, OtherPath})
            IFileManager::Get().Delete(*P, false, true);
        R.ClearProgramCache();
        Limit->Set(OldLimit, ECVF_SetByCode);
    };
    auto Spawn = [&](bool Lifecycle = true)
    {
        auto* A =
            World->SpawnActor<ADasProbeActor>(Lifecycle ? BP->GeneratedClass.Get() : ADasProbeActor::StaticClass());
        Actors.Add(A);
        return A;
    };
    auto Read = [&](ADasProbeActor* A, int32 Input = 0)
    {
        auto V = R.RunNamedPersistent(Path, TEXT("main"), Input, A);
        if (!V.bSucceeded)
            AddError(V.Error);
        return V.Value;
    };
    const auto Start = R.ProgramCacheStats();
    auto Prepare = [&](const FString& P)
    {
        FString Error;
        const bool OK = R.PrepareEditorPrograms({P}, Error);
        if (!OK)
            AddError(Error);
        return OK;
    };
    const double PreparationStart = FPlatformTime::Seconds();
    if (!Prepare(Path))
        return false;
    const double PreparationMs = (FPlatformTime::Seconds() - PreparationStart) * 1000.;
    const double ColdStart = FPlatformTime::Seconds();
    auto* First = Spawn();
    const double ColdMs = (FPlatformTime::Seconds() - ColdStart) * 1000.;
    if (!TestNotNull(TEXT("Spawn first prepared actor"), First))
        return false;
    TestTrue(TEXT("Prepared lifecycle succeeds"), First->LastDasError.IsEmpty());
    TestEqual(TEXT("Preparation compiles once; spawning does not compile"),
                   R.ProgramCacheStats().Compilations - Start.Compilations, uint64(1));
    const double WarmStart = FPlatformTime::Seconds();
    for (int32 I = 0; I < 7; ++I)
        Spawn();
    const double WarmMs = (FPlatformTime::Seconds() - WarmStart) * 1000. / 7.;
    TestEqual(TEXT("Eight real Blueprint actor lifecycles share one compilation"),
                   R.ProgramCacheStats().Compilations - Start.Compilations, uint64(1));
    TestEqual(TEXT("Every actor still gets a simulated context"), R.ProgramCacheStats().Simulations - Start.Simulations,
                   uint64(8));
    for (auto* A : Actors)
    {
        TestTrue(TEXT("Warm lifecycle succeeds"), A && A->LastDasError.IsEmpty());
        TestEqual(TEXT("Global scalar and array initialized separately"), Read(A), 113);
        TestEqual(TEXT("One owner-specific subscription"), R.SubscriptionCount(A), 1);
    }
    TestEqual(TEXT("First owner mutation"), Read(First, 2), 313);
    TestEqual(TEXT("Second owner is isolated"), Read(Actors[1]), 113);
    First->EmitDasSignal(5);
    Read(First); // The managed queue drains at the end of the call.
    TestEqual(TEXT("Signal affects its own VM"), Read(First), 813);
    TestEqual(TEXT("Signal does not affect another VM"), Read(Actors[1]), 113);
    const auto BeforeDep = R.ProgramCacheStats();
    Write(Dep, TEXT("options gen2\nmodule ") + DepName + TEXT(" public\ndef dependency_value():int {return 9}\n"));
    if (!Prepare(Path))
        return false;
    TestEqual(TEXT("Dependency reload uses fresh initializer"), Read(First), 9);
    TestEqual(TEXT("Other live owner reuses updated compilation"), Read(Actors[1]), 9);
    auto* NewVersion = Spawn();
    TestEqual(TEXT("New owner uses updated program and independent lifecycle"), Read(NewVersion), 119);
    TestEqual(TEXT("Changed require dependency compiles once for all owners"),
                   R.ProgramCacheStats().Compilations - BeforeDep.Compilations, uint64(1));
    // A failure during simulate must not permanently set failToCompile on the shared AST.
    const auto BeforeInitFailure = R.ProgramCacheStats();
    Write(
        Path,
        Source.Replace(TEXT("return counter*100"), TEXT("return broken+counter*100")) +
            TEXT("\nvar broken:int=fail_init()\ndef fail_init():int {panic(\"cache initialization failure\"); return 0}\n"));
    if (!Prepare(Path))
        return false;
    auto Rejected = R.RunNamedPersistent(Path, TEXT("main"), 0, First);
    TestTrue(TEXT("Failed reload retains working context"), Rejected.bSucceeded && !Rejected.ReloadError.IsEmpty());
    auto* FailedOwner = Spawn(false);
    auto Failed = R.RunNamedPersistent(Path, TEXT("main"), 0, FailedOwner);
    TestFalse(TEXT("New owner's initializer failure is reported"), Failed.bSucceeded);
    TestEqual(TEXT("Initializer failure does not trigger another compile"),
                   R.ProgramCacheStats().Compilations - BeforeInitFailure.Compilations, uint64(1));
    TestEqual(TEXT("Initializer executes independently even after previous failure"),
                   R.ProgramCacheStats().Simulations - BeforeInitFailure.Simulations, uint64(2));
    TestEqual(TEXT("Working subscriptions survive rejected reload"), R.SubscriptionCount(First), 1);
    TestEqual(TEXT("Rejected owner has no installed subscription"), R.SubscriptionCount(FailedOwner), 0);
    Write(Path, Source);
    if (!Prepare(Path))
        return false;
    TestEqual(TEXT("Repair is accepted without losing live field storage"), Read(First), 9);
    Limit->Set(1, ECVF_SetByCode);

    if (!Prepare(OtherPath))
        return false;
    auto Other = R.RunPersistent(OtherPath, 5, First);
    TestTrue(TEXT("Second compiler key executes"), Other.bSucceeded && Other.Value == 6);
    const auto BeforeWeakHit = R.ProgramCacheStats();
    auto* AfterEviction = Spawn();
    TestEqual(TEXT("Evicted resident program is reused while live contexts own it"), R.ProgramCacheStats().Compilations,
                   BeforeWeakHit.Compilations);
    TestEqual(TEXT("Resident cache respects capacity"), R.ProgramCacheStats().ResidentPrograms, 1);
    TestEqual(TEXT("State of owner created after LRU eviction"), Read(AfterEviction), 119);
    const auto BeforeGC = R.ProgramCacheStats();
    CollectGarbage(RF_NoFlags);
    TestEqual(TEXT("GC drops idle compiler cache safely"), R.ProgramCacheStats().ResidentPrograms, 0);
    TestEqual(TEXT("Existing VM remains valid after cache flush and GC"), Read(First), 9);
    TestEqual(TEXT("Existing VM did not recompile after GC"), R.ProgramCacheStats().Compilations,
                   BeforeGC.Compilations);
    auto* AfterGC = Spawn();
    TestEqual(TEXT("New owner reuses live compiler after GC"),
                   R.ProgramCacheStats().Compilations - BeforeGC.Compilations, uint64(0));
    TestEqual(TEXT("Fresh context after GC"), Read(AfterGC), 119);
    Limit->Set(0, ECVF_SetByCode);
    const auto NoIdle = R.ProgramCacheStats();
    auto* WithoutIdleRetention = Spawn();
    TestEqual(TEXT("Zero idle retention still uses prepared/live programs"), R.ProgramCacheStats().Compilations,
                   NoIdle.Compilations);
    TestEqual(TEXT("Zero idle retention preserves independent state"), Read(WithoutIdleRetention), 119);
    R.ReleaseObject(First);
    TestEqual(TEXT("Release clears only this owner's subscriptions"), R.SubscriptionCount(First), 0);
    TestEqual(TEXT("Other owner remains subscribed"), R.SubscriptionCount(AfterGC), 1);
    AddInfo(FString::Printf(
        TEXT("DAS_PROGRAM_CACHE_TIMING preparation_ms=%.3f first_spawn_ms=%.3f warm_spawn_mean_ms=%.3f"), PreparationMs,
             ColdMs, WarmMs));
    return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasPreparedProgramsTest, "DasForUE.Runtime.PreparedPrograms",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasPreparedProgramsTest::RunTest(const FString&)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    auto* Strict = IConsoleManager::Get().FindConsoleVariable(TEXT("das.RequirePreparedPrograms"));
    const int32 Previous = Strict->GetInt();
    Strict->Set(0, ECVF_SetByCode);
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString Name = TEXT("prepared_") + Id + TEXT(".das"), Path = R.ScriptPath(Name);
    const FString Source = TEXT(R"DAS(options gen2
require ue_dsl
var counter:int=0
[ue_class(module_id="preparedfixture",Blueprintable)]
class PreparedActor : Actor {
    @ue_property=(BlueprintReadWrite)
    Score:int=3
    [ue_function(BlueprintCallable)]
    def Add(value:int):int { self.Score+=value; return self.Score }
}
[export]
def main(input:int):int {counter+=input; return counter}
)DAS");
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    const FString Directory = FPaths::ProjectContentDir() / TEXT("Das/Prepared");
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    TArray<FString> BeforeFiles;
    IFileManager::Get().FindFiles(BeforeFiles, *(Directory / TEXT("*.dasp")), true, false);
    TStrongObjectPtr<UObject> First(NewObject<UDasScriptClassData>()), Second(NewObject<UDasScriptClassData>());
    AActor* ScriptActor = nullptr;
    ON_SCOPE_EXIT
    {
        R.ReleaseObject(First.Get());
        R.ReleaseObject(Second.Get());
        if (IsValid(ScriptActor))
        {
            R.ReleaseObject(ScriptActor);
            ScriptActor->Destroy();
        }
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
        IFileManager::Get().Delete(*Path, false, true);
        TArray<FString> AfterFiles;
        IFileManager::Get().FindFiles(AfterFiles, *(Directory / TEXT("*.dasp")), true, false);
        for (const auto& File : AfterFiles)
            if (!BeforeFiles.Contains(File))
                IFileManager::Get().Delete(*(Directory / File), false, true);
        Strict->Set(Previous, ECVF_SetByCode);
        R.ClearProgramCache();
    };
    TestTrue(TEXT("Write authored source"), FFileHelper::SaveStringToFile(Source, *Path));
    R.NotifyScriptsChanged({Path});
    R.ClearProgramCache();
    FString Error;
    UBlueprint* RawBlueprint = nullptr;
    if (!FDasDSLService::Apply(Name, TEXT("/Game/Tests/PreparedPrograms/") + Id +
                                          TEXT("/BP_Prepared"), TEXT("preparedfixture.PreparedActor"), RawBlueprint,
                                                                     Error))
    {
        AddError(Error);
        return false;
    }
    TStrongObjectPtr<UBlueprint> Blueprint(RawBlueprint);
    R.ClearProgramCache();
    const auto Start = R.ProgramCacheStats();
    TArray<FString> SavedConfiguration;
    GConfig->GetArray(TEXT("DasForUE.Programs"), TEXT("Scripts"), SavedConfiguration, GGameIni);
    GConfig->SetArray(TEXT("DasForUE.Programs"), TEXT("Scripts"), {Name}, GGameIni);
    TStrongObjectPtr<UWorld> AutomaticWorld(UWorld::CreateWorld(EWorldType::Game, false));
    GConfig->SetArray(TEXT("DasForUE.Programs"), TEXT("Scripts"), SavedConfiguration, GGameIni);
    TestEqual(TEXT("World initialization automatically prepares the configured root"),
                   R.ProgramCacheStats().Compilations - Start.Compilations, uint64(1));
    AutomaticWorld->DestroyWorld(false);
    if (!TestTrue(TEXT("Prepare before BeginPlay: ") + Error, R.PreparePrograms(World.Get(), {Name}, Error)))
    {
        AddError(Error);
        return false;
    }
    TestEqual(TEXT("Preparation compiles once"), R.ProgramCacheStats().Compilations - Start.Compilations, uint64(1));
    TestEqual(TEXT("Preparation does not initialize any instance"), R.ProgramCacheStats().Simulations,
                   Start.Simulations);
    CollectGarbage(RF_NoFlags);
    auto A = R.RunPersistent(Path, 7, First.Get());
    auto B = R.RunPersistent(Path, 2, Second.Get());
    TestTrue(TEXT("Prepared contexts keep independent globals"),
                  A.bSucceeded && A.Value == 7 && B.bSucceeded && B.Value == 2);
    TestEqual(TEXT("No gameplay compilation, also after GC"), R.ProgramCacheStats().Compilations - Start.Compilations,
                   uint64(1));
    if (!R.ExportPreparedProgram(Name, Error))
    {
        AddError(TEXT("Export authored program: ") + Error);
        return false;
    }
    R.ReleaseObject(First.Get());
    R.ReleaseObject(Second.Get());
    R.ClearProgramCache();
    Strict->Set(1, ECVF_SetByCode);
    const auto Packaged = R.ProgramCacheStats();
    A = R.RunPersistent(Path, 11, First.Get());
    TestTrue(TEXT("Strict prepared AST executes: ") + A.Error, A.bSucceeded && A.Value == 11);
    TestEqual(TEXT("Strict runtime never requests source compiler"), R.ProgramCacheStats().Compilations,
                   Packaged.Compilations);
    TestEqual(TEXT("Loaded a prepared artifact"), R.ProgramCacheStats().PreparedLoads - Packaged.PreparedLoads,
                   uint64(1));
    ScriptActor = World->SpawnActor<AActor>(Blueprint->GeneratedClass);
    if (!TestNotNull(TEXT("Spawn authored Blueprint owner"), ScriptActor))
        return false;
    const auto Method = R.RunNamedPersistent(Path, TEXT("ue_PreparedActor_Add"), 4, ScriptActor);
    TestTrue(TEXT("Prepared authored method updates the real UE property: ") + Method.Error,
                  Method.bSucceeded && Method.Value == 7);
    TestEqual(TEXT("Authored method does not compile on first use"), R.ProgramCacheStats().Compilations,
                   Packaged.Compilations);
    R.ReleaseObject(First.Get());
    TestTrue(TEXT("Edit fixture"), FFileHelper::SaveStringToFile(Source + TEXT("\n// edited\n"), *Path));
    R.NotifyScriptsChanged({Path});
    R.ClearProgramCache();
    A = R.RunPersistent(Path, 1, First.Get());
    TestTrue(TEXT("Strict runtime refuses stale source without compiling"),
                  !A.bSucceeded && A.Error.Contains(TEXT("source dependency changed")));
    TestEqual(TEXT("Refusal never invokes compiler"), R.ProgramCacheStats().Compilations, Packaged.Compilations);
    return !HasAnyErrors();
}
