#include "DasDSLService.h"
#include "DasRuntime.h"
#include "Components/SceneComponent.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "HAL/FileManager.h"
#include "EngineUtils.h"
#include "Misc/FileHelper.h"
#include "UObject/StrongObjectPtr.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
namespace
{
// An actor with native components; v2 adds a field and changes a body, v3 changes the composition.
const TCHAR* ComposedV1 = TEXT(R"(options gen2
require ue_scene_components
[ue_class(module_id="composedreimport",id="composedreimport.host",parent="/Script/Engine.Actor",Blueprintable)]
class ComposedHost {
    @ue_component
    @RootComponent
    Pivot:SceneComponentRef
    @ue_component
    @Attach="Pivot"
    Child:SceneComponentRef
    defaults=ue_defaults() {
        self.Child.RelativeLocation=ue_vector(0.0lf,0.0lf,50.0lf)
    }
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    @ue_id="composedreimport.host.score"
    Score : int = 10
    [ue_function(id="composedreimport.host.evaluate",BlueprintCallable)]
    def Evaluate(input:int):int { return self.Score + input }
}
)");
FString ComposedV2()
{
    return FString(ComposedV1)
        .Replace(TEXT("    [ue_function"),
                      TEXT("    @ue_property=(EditAnywhere,BlueprintReadWrite)\n"
                                              "    @ue_id=\"composedreimport.host.bonus\"\n"
                                              "    Bonus : int = 5\n"
                                              "    [ue_function"))
                          .Replace(TEXT("self.Score + input"), TEXT("self.Score + input + self.Bonus"));
}
FString ComposedV3()
{
    return ComposedV2().Replace(TEXT("    defaults="), TEXT("    @ue_component\n    @Attach=\"Pivot\"\n    Extra:SceneComponentRef\n    defaults="));
}

AActor* FindActor(UWorld* World, FName Name)
{
    if (World)
        for (TActorIterator<AActor> It(World); It; ++It)
            if (It->GetFName() == Name)
                return *It;
    return nullptr;
}
int32 Int(UObject* Object, const TCHAR* Name)
{
    auto* P = Object ? FindFProperty<FIntProperty>(Object->GetClass(), Name) : nullptr;
    return P ? P->GetPropertyValue_InContainer(Object) : INT32_MIN;
}
void SetInt(UObject* Object, const TCHAR* Name, int32 Value)
{
    if (auto* P = Object ? FindFProperty<FIntProperty>(Object->GetClass(), Name) : nullptr)
        P->SetPropertyValue_InContainer(Object, Value);
}
int32 Evaluate(AActor* Actor, int32 Input)
{
    struct FArgs
    {
        int32 Input, Result;
    };
    FArgs Args{Input, INT32_MIN};
    if (auto* Fn = Actor ? Actor->FindFunction(TEXT("Evaluate")) : nullptr)
        Actor->ProcessEvent(Fn, &Args);
    return Args.Result;
}
USceneComponent* Component(AActor* Actor, FName Name)
{
    if (Actor)
        for (auto* C : Actor->GetComponents())
            if (C && C->GetFName() == Name)
                return Cast<USceneComponent>(C);
    return nullptr;
}
// Both SCS nodes keep their identity and the authored template default.
bool CompositionKept(UBlueprint* BP)
{
    int32 Matched = 0;
    for (auto* Node : BP->SimpleConstructionScript->GetAllNodes())
    {
        const FName Name = Node->GetVariableName();
        if ((Name == TEXT("Pivot") || Name == TEXT("Child")) &&
                          Node->FindMetaDataEntryIndexForKey(TEXT("DasComponentId")) != INDEX_NONE)
            ++Matched;
        if (Name == TEXT("Child") && Cast<USceneComponent>(Node->ComponentTemplate)->GetRelativeLocation().Z != 50.0)
            return false;
    }
    return Matched == 2 && BP->SimpleConstructionScript->GetAllNodes().Num() == 2;
}
struct FComposedScript
{
    FString Name, Path;
    explicit FComposedScript(const TCHAR* Prefix)
    {
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        Name = Prefix + FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT(".das");
        Path = Runtime.ScriptPath(Name);
    }
    bool Write(const FString& Text) const
    {
        return FFileHelper::SaveStringToFile(Text, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    }
    void Delete() const { IFileManager::Get().Delete(*Path); }
};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasComposedReimportTest, "DasForUE.ActorComponent.DeclarationReimport",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasComposedReimportTest::RunTest(const FString&)
{
    const FComposedScript Script(TEXT("dsl_composed_reimport_"));
    ON_SCOPE_EXIT
    {
        Script.Delete();
    };
    UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
    const FString Package =
        TEXT("/Game/DasActorComponent/Reimport_")+ FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT("/BP_Host");
    UBlueprint* BP = nullptr;
    FString Error;
    if (!TestTrue(TEXT("write_v1"), Script.Write(ComposedV1)) ||
                  !TestTrue(TEXT("apply_v1"),
                                 FDasDSLService::Apply(Script.Name, Package, TEXT("composedreimport.host"), BP, Error)))
    {
        AddError(Error);
        return false;
    }
    TStrongObjectPtr<UBlueprint> Keep(BP);
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("ComposedPlaced");
    AActor* Placed = World->SpawnActor<AActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
    if (!TestNotNull(TEXT("placed_actor"), Placed))
        return false;
    SetInt(Placed, TEXT("Score"), 77);
    auto* Child = Component(Placed, TEXT("Child"));
    if (!TestNotNull(TEXT("placed_child_component"), Child))
        return false;
    Child->Modify();
    Child->SetRelativeLocation(FVector(0, 0, 80));

    // A Blueprint created in this editor session migrates without an extra compile.
    TestTrue(TEXT("created_Blueprint_is_up_to_date"),
                  BP->Status == BS_UpToDate || BP->Status == BS_UpToDateWithWarnings);
    TestTrue(TEXT("write_v2_new_field"), Script.Write(ComposedV2()));
    TestTrue(TEXT("composed_actor_declaration_reimport"), FDasDSLService::Reimport(BP, Error));
    if (!Error.IsEmpty())
        AddInfo(Error);
    Placed = FindActor(World, TEXT("ComposedPlaced"));
    Child = Component(Placed, TEXT("Child"));
    TestTrue(TEXT("placed_actor_reinstanced_with_new_field"),
                  Placed && Placed->GetClass() == BP->GeneratedClass && Int(Placed, TEXT("Bonus")) == 5);
    TestEqual(TEXT("placed_override_preserved"), Int(Placed, TEXT("Score")), 77);
    // Methods run only in game worlds; the PIE test checks the new body.
    TestTrue(TEXT("method_kept"), Placed && Placed->FindFunction(TEXT("Evaluate")) != nullptr);
    TestTrue(TEXT("composition_and_template_defaults_kept"), CompositionKept(BP));
    TestTrue(TEXT("placed_component_instance_change_kept"),
                  Child && Component(Placed, TEXT("Pivot")) && Child->GetRelativeLocation().Z == 80.0);

    UClass* Before = BP->GeneratedClass;
    TestTrue(TEXT("write_v3_composition_change"), Script.Write(ComposedV3()));
    TestTrue(TEXT("composition_change_refused"),
                  !FDasDSLService::Reimport(BP, Error) && Error.Contains(TEXT("composition migration is deferred")) &&
                                                                         BP->GeneratedClass == Before &&
                                                                         CompositionKept(BP));
    TestTrue(TEXT("restore_v2"), Script.Write(ComposedV2()));
    TestTrue(TEXT("explicit_instance_default_migration_refused"),
                  !FDasDSLService::Reimport(BP, Error, EDasDefaultMigrationPolicy::PreserveInstances) &&
                      Error.Contains(TEXT("actors with components")));
    TestTrue(TEXT("unchanged_reimport_after_refusals"),
                  FDasDSLService::Reimport(BP, Error) && BP->GeneratedClass == Before);
    return true;
}

namespace
{
struct FComposedPIEState
{
    TStrongObjectPtr<UBlueprint> Blueprint;
    TUniquePtr<FComposedScript> Script;
    bool Passed = true;
};
class FComposedPIECommand final : public IAutomationLatentCommand
{
    TSharedRef<FComposedPIEState> S;
    FAutomationTestBase* Test;
    double Start = FPlatformTime::Seconds();
    bool Ending = false;

public:
    FComposedPIECommand(TSharedRef<FComposedPIEState> State, FAutomationTestBase* T) : S(State), Test(T) {}
    bool Update() override
    {
        auto Check = [&](bool Value, const TCHAR* Name)
        {
            S->Passed &= Test->TestTrue(Name, Value);
            UE_LOG(LogTemp, Display, TEXT("DAS_COMPOSED_PIE %s %s"), Value ? TEXT("PASS"): TEXT("FAIL"), Name);
        };
        auto Finish = [&]()
        {
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        };
        if (Ending)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Start < 30)
                return false;
            Check(!GEditor->PlayWorld, TEXT("EndPIE"));
            S->Script->Delete();
            UE_LOG(LogTemp, Display, TEXT("DAS_COMPOSED_PIE_RESULT=%s"), S->Passed ? TEXT("PASS"): TEXT("FAIL"));
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        if (!World || !World->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Start < 60)
                return false;
            Check(false, TEXT("PIE_started"));
            return Finish();
        }
        AActor* Actor = FindActor(World, TEXT("ComposedPlaced"));
        Check(Actor && Actor->HasActorBegunPlay() && Evaluate(Actor, 1) == 11, TEXT("composed_actor_runs_in_PIE"));
        if (!Actor)
            return Finish();
        SetInt(Actor, TEXT("Score"), 77);
        AActor* BeforeActor = Actor;
        bool Replaced = false;
        auto Handle = FCoreUObjectDelegates::OnObjectsReplaced.AddLambda(
            [&](const FCoreUObjectDelegates::FReplacementObjectMap& Map) { Replaced |= Map.Contains(BeforeActor); });
        FString Error;
        Check(S->Script->Write(ComposedV2()), TEXT("write_v2_during_PIE"));
        Check(FDasDSLService::Reimport(S->Blueprint.Get(), Error), TEXT("composed_reimport_during_PIE"));
        FCoreUObjectDelegates::OnObjectsReplaced.Remove(Handle);
        if (!Error.IsEmpty())
            Test->AddInfo(Error);
        Actor = FindActor(World, TEXT("ComposedPlaced"));
        Check(Replaced && Actor && Actor->HasActorBegunPlay() && Actor->GetClass() == S->Blueprint->GeneratedClass,
              TEXT("PIE_actor_reinstanced_with_BeginPlay_state"));
        Check(Int(Actor, TEXT("Score")) == 77 && Int(Actor, TEXT("Bonus")) == 5 && Evaluate(Actor, 1) == 83,
                                                     TEXT("PIE_new_field_and_body_preserve_runtime_value"));
        auto* Child = Component(Actor, TEXT("Child"));
        Check(Child && Component(Actor, TEXT("Pivot")) &&
                                 Child->GetAttachParent() == Component(Actor, TEXT("Pivot")) &&
                                                                       Child->GetRelativeLocation().Z == 50.0,
                                                                       TEXT("PIE_components_kept_after_reinstance"));
        Check(Int(FindActor(GEditor->GetEditorWorldContext().World(), TEXT("ComposedPlaced")), TEXT("Score")) == 10,
                  TEXT("editor_world_actor_isolated"));
        return Finish();
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasComposedReimportPIETest, "DasForUE.ActorComponent.DeclarationReimportPIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasComposedReimportPIETest::RunTest(const FString&)
{
    auto S = MakeShared<FComposedPIEState>();
    S->Script = MakeUnique<FComposedScript>(TEXT("dsl_composed_pie_"));
    UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
    const FString Package = TEXT("/Game/DasActorComponent/ReimportPIE_")+
                                 FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT("/BP_Host");
    UBlueprint* BP = nullptr;
    FString Error;
    if (!TestTrue(TEXT("write_v1"), S->Script->Write(ComposedV1)) ||
                  !TestTrue(TEXT("apply_v1"), FDasDSLService::Apply(S->Script->Name, Package,
                                                                    TEXT("composedreimport.host"), BP, Error)))
    {
        AddError(Error);
        S->Script->Delete();
        return false;
    }
    S->Blueprint.Reset(BP);
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("ComposedPlaced");
    World->SpawnActor<AActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FComposedPIECommand>(S, this));
    return true;
}
#endif
