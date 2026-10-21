#include "DasReloadCommandlet.h"
#include "DasTypeService.h"
#include "DasProbeActor.h"
#include "DasRuntime.h"
#include "DasBlueprintService.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/Level.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Curves/CurveFloat.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "BlueprintCompilationManager.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#include "Modules/ModuleManager.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"

namespace
{
bool Compile(UBlueprint* BP)
{
    FKismetEditorUtilities::CompileBlueprint(BP);
    FBlueprintCompilationManager::FlushCompilationQueueAndReinstance();
    return BP->Status != BS_Error && BP->GeneratedClass;
}

bool ApplySchema(UBlueprint* BP, const FString& Script, FString& Error)
{
    FDasClassSchema Schema;
    if (!FDasBlueprintService::ReadSchema(Script, Schema, Error))
        return false;
    // Test fixture values; they are no longer hard-coded in the editor backend.
    for (auto& Field : Schema.Fields)
    {
        if (Field.Name == TEXT("speed"))
            Field.FloatDefault = 10.f;
        if (Field.Name == TEXT("health"))
            Field.IntDefault = 100;
    }
    return FDasBlueprintService::Apply(BP, Schema, Error);
}
bool SetFloat(UObject* Object, FName Name, float Value)
{
    auto* Property = FindFProperty<FFloatProperty>(Object->GetClass(), Name);
    if (!Property)
        return false;
    Property->SetPropertyValue_InContainer(Object, Value);
    return true;
}
float Float(UObject* Object, FName Name)
{
    auto* Property = Object ? FindFProperty<FFloatProperty>(Object->GetClass(), Name) : nullptr;
    return Property ? Property->GetPropertyValue_InContainer(Object) : -9999.f;
}
int32 Int(UObject* Object, FName Name)
{
    auto* Property = Object ? FindFProperty<FIntProperty>(Object->GetClass(), Name) : nullptr;
    return Property ? Property->GetPropertyValue_InContainer(Object) : MIN_int32;
}
int32 Evaluate(UObject* Object)
{
    UFunction* Function = Object ? Object->FindFunction(TEXT("Evaluate")) : nullptr;
    if (!Function)
        return MIN_int32;
    FStructOnScope Frame(Function);
    auto* Input = FindFProperty<FIntProperty>(Function, TEXT("Input"));
    auto* Return = CastField<FIntProperty>(Function->GetReturnProperty());
    if (!Input || !Return)
        return MIN_int32;
    Input->SetPropertyValue_InContainer(Frame.GetStructMemory(), 40);
    // Editor-world ProcessEvent intentionally requires this explicit guard.
    FEditorScriptExecutionGuard ScriptGuard;
    Object->ProcessEvent(Function, Frame.GetStructMemory());
    return Return->GetPropertyValue_InContainer(Frame.GetStructMemory());
}
UBlueprint* Blueprint(const FString& PackagePath, UClass* Parent)
{
    UPackage* Package = CreatePackage(*PackagePath);
    return FKismetEditorUtilities::CreateBlueprint(Parent, Package, *FPackageName::GetShortName(PackagePath),
                                                   BPTYPE_Normal, UBlueprint::StaticClass(),
                                                   UBlueprintGeneratedClass::StaticClass(), TEXT("DasReloadProbe"));
}
bool Save(UObject* Object, bool bMap = false)
{
    UPackage* Package = Object->GetOutermost();
    Object->SetFlags(RF_Public | RF_Standalone);
    const FString Filename = FPackageName::LongPackageNameToFilename(
        Package->GetName(), bMap ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension());
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(Filename), true);
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public | RF_Standalone;
    Args.SaveFlags = SAVE_NoError;
    return UPackage::SavePackage(Package, Object, *Filename, Args);
}
ADasProbeActor* Find(UWorld* World, FName Name)
{
    for (TActorIterator<ADasProbeActor> It(World); It; ++It)
        if (It->GetFName() == Name)
            return *It;
    return nullptr;
}
}

UDasReloadCommandlet::UDasReloadCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}

int32 UDasReloadCommandlet::Main(const FString& Params)
{
    int32 Failures = 0;
    auto Expect = [&](bool Condition, const TCHAR* Name)
    {
        UE_LOG(LogTemp, Display, TEXT("DAS_RELOAD %s %s"), Condition ? TEXT("PASS") : TEXT("FAIL"), Name);
        if (!Condition)
            ++Failures;
        return Condition;
    };
    FString Root;
    const bool bReopen = FParse::Value(*Params, TEXT("Reopen="), Root);
    if (!bReopen)
        Root = TEXT("/Game/DasReloadProbe/Run_") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
    if (!Root.StartsWith(TEXT("/Game/DasReloadProbe/Run_")) || Root.Contains(TEXT("..")))
        return 1;
    UE_LOG(LogTemp, Display, TEXT("DAS_RELOAD_ROOT=%s"), *Root);
    TStrongObjectPtr<UBlueprint> Parent(
        bReopen                          ? LoadObject<UBlueprint>(nullptr, *(Root + TEXT("/BP_DasGenerated.BP_DasGenerated")))
                                         : Blueprint(Root + TEXT("/BP_DasGenerated"), ADasProbeActor::StaticClass()));
    if (!Expect(Parent.IsValid(), TEXT("parent_asset")))
        return 1;
    FString Error;
    if (!bReopen && !Expect(ApplySchema(Parent.Get(), TEXT("schema_v1.das"), Error), TEXT("schema_v1_to_uclass")))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        return 1;
    }
    TStrongObjectPtr<UBlueprint> Child(
        bReopen                          ? LoadObject<UBlueprint>(nullptr, *(Root + TEXT("/BP_Child.BP_Child")))
                                         : Blueprint(Root + TEXT("/BP_Child"), Parent->GeneratedClass));
    if (!Expect(Child.IsValid(), TEXT("blueprint_child")))
        return 1;
    if (!bReopen)
    {
        if (!Expect(Compile(Child.Get()), TEXT("child_compiles")))
            return 1;
        SetFloat(Child->GeneratedClass->GetDefaultObject(), TEXT("speed"), 20.f);
        Child->Modify();
    }
    UWorld* World = bReopen
        ? LoadObject<UWorld>(nullptr, *(Root + TEXT("/ProbeMap.ProbeMap")))
                             : UWorld::CreateWorld(EWorldType::Editor, false,
                                                   TEXT("ProbeMap"), CreatePackage(*(Root + TEXT("/ProbeMap"))));
    if (!Expect(World != nullptr, TEXT("editor_world")))
        return 1;
    TStrongObjectPtr<UWorld> KeepWorld(World);
    FWorldContext& Context = GEditor->GetEditorWorldContext();
    UWorld* PreviousWorld = Context.World();
    Context.SetCurrentWorld(World);
    if (bReopen)
    {
        World->WorldType = EWorldType::Editor;
        if (!World->IsInitialized())
            World->InitWorld();
        // Loading an asset does not register its components or update their world transforms.
        World->UpdateWorldComponents(true, false);
    }
    ADasProbeActor* Actor = nullptr;
    ADasProbeActor* Holder = nullptr;
    if (!bReopen)
    {
        FActorSpawnParameters Spawn;
        Spawn.Name = TEXT("ProbeInstance");
        Actor = World->SpawnActor<ADasProbeActor>(Child->GeneratedClass, FTransform(FVector(11, 22, 33)), Spawn);
        Spawn.Name = TEXT("ReferenceHolder");
        Holder = World->SpawnActor<ADasProbeActor>(ADasProbeActor::StaticClass(), FTransform::Identity, Spawn);
        if (!Actor || !Holder)
        {
            Context.SetCurrentWorld(PreviousWorld);
            return 1;
        }
        SetFloat(Actor, TEXT("speed"), 37.f);
        Holder->Peer = Actor;
        Actor->Peer = Holder;
        Expect(Evaluate(Actor) == 41, TEXT("v1_reflected_script_method"));
        Expect(Float(Parent->GeneratedClass->GetDefaultObject(), TEXT("speed")) == 10.f, TEXT("parent_default_v1"));
        Expect(Float(Child->GeneratedClass->GetDefaultObject(), TEXT("speed")) == 20.f,
                     TEXT("child_default_override_v1"));
        UObject* OldActor = Actor;
        bool bReplaced = false;
        const FDelegateHandle ReplacementHandle = FCoreUObjectDelegates::OnObjectsReplaced.AddLambda(
            [&](const FCoreUObjectDelegates::FReplacementObjectMap& Map) { bReplaced |= Map.Contains(OldActor); });
        const bool bApplied = ApplySchema(Parent.Get(), TEXT("reload_v2.das"), Error);
        FCoreUObjectDelegates::OnObjectsReplaced.Remove(ReplacementHandle);
        Expect(bApplied, TEXT("schema_v2_same_process"));
        if (!bApplied)
            UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        Actor = Find(World, TEXT("ProbeInstance"));
        Holder = Find(World, TEXT("ReferenceHolder"));
        Expect(bReplaced, TEXT("instance_reinstanced"));
        Expect(Child->ParentClass == Parent->GeneratedClass && Child->Status != BS_Error,
               TEXT("dependent_blueprint_updated"));
        Expect(!ApplySchema(Parent.Get(), TEXT("schema_invalid.das"), Error), TEXT("bad_schema_rejected"));
        Expect(Actor && Evaluate(Actor) == 42, TEXT("last_good_version_survives_error"));
    }
    else
    {
        Actor = Find(World, TEXT("ProbeInstance"));
        Holder = Find(World, TEXT("ReferenceHolder"));
    }
    Expect(Actor && Actor->GetClass() == Child->GeneratedClass, TEXT("instance_current_class"));
    Expect(Actor && Float(Actor, TEXT("speed")) == 37.f, TEXT("instance_override_preserved"));
    Expect(Actor && Int(Actor, TEXT("health")) == 100, TEXT("new_property_default"));
    Expect(Float(Parent->GeneratedClass->GetDefaultObject(), TEXT("speed")) == 10.f, TEXT("parent_default_preserved"));
    Expect(Float(Child->GeneratedClass->GetDefaultObject(), TEXT("speed")) == 20.f, TEXT("child_default_preserved"));
    Expect(Actor && Holder && Holder->Peer == Actor && Actor->Peer == Holder, TEXT("bidirectional_references"));
    Expect(Actor && Actor->GetActorLocation().Equals(FVector(11, 22, 33)), TEXT("transform_preserved"));
    Expect(Actor && Evaluate(Actor) == 42, TEXT("v2_reflected_script_method"));
    if (Actor)
    {
        const float Before = Float(Actor, TEXT("speed"));
        Expect(Actor->ExecuteDas(TEXT("self_properties.das"), 5) == 105 &&
                                 Int(Actor, TEXT("health")) == 105 &&
                                     Float(Actor, TEXT("speed")) == Before + 1.f,
                                           TEXT("script_self_reads_and_writes_reloaded_properties"));
        SetFloat(Actor, TEXT("speed"), Before);
        if (auto* Health = FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("health")))
            Health->SetPropertyValue_InContainer(Actor, 100);
    }
    CollectGarbage(RF_NoFlags);
    Actor = Find(World, TEXT("ProbeInstance"));
    Expect(Actor && Int(Actor, TEXT("health")) == 100 && Evaluate(Actor) == 42, TEXT("survives_gc"));
    if (!bReopen && Failures == 0)
    {
        Expect(Save(Parent.Get()), TEXT("save_parent"));
        Expect(Save(Child.Get()), TEXT("save_child"));
        Expect(Save(World, true), TEXT("save_map"));
        FString Manifest;
        if (FParse::Value(*Params, TEXT("Manifest="), Manifest))
            Expect(FFileHelper::SaveStringToFile(Root, *Manifest), TEXT("save_manifest"));
    }
    Context.SetCurrentWorld(PreviousWorld);
    World->DestroyWorld(false);
    UE_LOG(LogTemp, Display,
           TEXT("DAS_RELOAD_RESULT=%s failures=%d reopen=%d"),
                Failures ? TEXT("FAIL") : TEXT("PASS"), Failures, bReopen);
    return Failures ? 1 : 0;
}

#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
#include "DasLifecycleTestObserver.h"

namespace
{
struct FDasPIEState
{
    TStrongObjectPtr<UBlueprint> Parent;
    TStrongObjectPtr<UBlueprint> Child;
    bool bPassed = true;
};

class FDasPIEReloadCommand final : public IAutomationLatentCommand
{
    TSharedRef<FDasPIEState> State;
    FAutomationTestBase* Test;
    double Started = 0;
    bool bEnding = false;

public:
    FDasPIEReloadCommand(TSharedRef<FDasPIEState> InState, FAutomationTestBase* InTest) : State(InState), Test(InTest)
    {
    }

    virtual bool Update() override
    {
        if (Started == 0)
            Started = FPlatformTime::Seconds();
        auto Check = [&](bool Value, const TCHAR* Name)
        {
            State->bPassed &= Test->TestTrue(Name, Value);
            UE_LOG(LogTemp, Display, TEXT("DAS_PIE %s %s"), Value ? TEXT("PASS") : TEXT("FAIL"), Name);
        };
        if (bEnding)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Started < 30)
                return false;
            Check(GEditor->PlayWorld == nullptr, TEXT("PIE_ended"));
            auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
            Check(Runtime.PersistentContextCount() ==
                      Runtime.PersistentContextCount(GEditor->GetEditorWorldContext().World()),
                  TEXT("persistent_no_play_contexts_after_EndPIE"));
            auto* EditorActor = Find(GEditor->GetEditorWorldContext().World(), TEXT("ProbeInstance"));
            Check(EditorActor && Float(EditorActor, TEXT("speed")) == 37.f && Evaluate(EditorActor) == 42,
                                       TEXT("editor_state_after_EndPIE"));
            UE_LOG(LogTemp, Display, TEXT("DAS_PIE_RESULT=%s"), State->bPassed ? TEXT("PASS") : TEXT("FAIL"));
            return true;
        }
        UWorld* PlayWorld = GEditor->PlayWorld;
        if (!PlayWorld || !PlayWorld->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Started < 60)
                return false;
            Check(false, TEXT("PIE_started_before_timeout"));
            GEditor->RequestEndPlayMap();
            Started = FPlatformTime::Seconds();
            bEnding = true;
            return false;
        }
        Check(PlayWorld->WorldType == EWorldType::PIE, TEXT("real_PIE_world"));
        auto* Actor = Find(PlayWorld, TEXT("ProbeInstance"));
        auto* Holder = Find(PlayWorld, TEXT("ReferenceHolder"));
        Check(Actor && Actor->HasActorBegunPlay(), TEXT("actor_BeginPlay"));
        Check(Actor && Float(Actor, TEXT("speed")) == 37.f && Evaluate(Actor) == 41, TEXT("PIE_v1"));
        if (Actor)
            SetFloat(Actor, TEXT("speed"), 73.f);
        UObject* OldActor = Actor;
        bool bReplaced = false;
        const auto Handle = FCoreUObjectDelegates::OnObjectsReplaced.AddLambda(
            [&](const FCoreUObjectDelegates::FReplacementObjectMap& Map) { bReplaced |= Map.Contains(OldActor); });
        FString Error;
        Check(ApplySchema(State->Parent.Get(), TEXT("reload_v2.das"), Error), TEXT("reload_during_PIE"));
        FCoreUObjectDelegates::OnObjectsReplaced.Remove(Handle);
        if (!Error.IsEmpty())
            Test->AddInfo(Error);
        Actor = Find(PlayWorld, TEXT("ProbeInstance"));
        Holder = Find(PlayWorld, TEXT("ReferenceHolder"));
        auto* EditorActor = Find(GEditor->GetEditorWorldContext().World(), TEXT("ProbeInstance"));
        Check(bReplaced, TEXT("PIE_instance_reinstanced"));
        Check(Actor && Actor->HasActorBegunPlay(), TEXT("BeginPlay_state_preserved"));
        Check(Actor && Actor->GetClass() == State->Child->GeneratedClass, TEXT("PIE_current_class"));
        Check(Actor && Float(Actor, TEXT("speed")) == 73.f, TEXT("PIE_override_preserved"));
        Check(EditorActor && Float(EditorActor, TEXT("speed")) == 37.f, TEXT("editor_override_isolated"));
        Check(Actor && Int(Actor, TEXT("health")) == 100 && Evaluate(Actor) == 42, TEXT("PIE_new_field_and_method"));
        Check(Actor && Holder && EditorActor && Holder->Peer == Actor && Actor->Peer == Holder &&
                  Actor->GetWorld() != EditorActor->GetWorld(),
              TEXT("PIE_references_stay_in_world"));
        Check(Actor && Actor->GetActorLocation().Equals(FVector(11, 22, 33)), TEXT("PIE_transform"));
        CollectGarbage(RF_NoFlags);
        Actor = Find(PlayWorld, TEXT("ProbeInstance"));
        Check(Actor && Float(Actor, TEXT("speed")) == 73.f && Evaluate(Actor) == 42, TEXT("PIE_survives_GC"));
        Check(Actor && Actor->ExecuteDas(TEXT("self_properties.das"), 5) == 105 &&
                                         Float(Actor, TEXT("speed")) == 74.f &&
                                               Int(Actor, TEXT("health")) == 105,
                                                   TEXT("script_self_in_PIE_after_reload"));
        Check(EditorActor && Float(EditorActor, TEXT("speed")) == 37.f &&
                                   Int(EditorActor, TEXT("health")) == 100, TEXT("script_writes_do_not_cross_worlds"));
        auto& PersistentRuntime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        Check(Actor && Holder && EditorActor &&
              Actor->ExecuteDas(TEXT("persistent_counter.das"), 1) == 1 &&
                                Actor->ExecuteDas(TEXT("persistent_counter.das"), 1) == 2 &&
                                                  Holder->ExecuteDas(TEXT("persistent_counter.das"), 1) == 1 &&
                                                                     EditorActor->ExecuteDas(
                                                                         TEXT("persistent_counter.das"), 1) == 1,
                                                                         TEXT("persistent_actor_and_world_isolation"));
        const FString LogicPath = FPaths::ProjectSavedDir() / TEXT("DasPersistentPIE_") +
                                                                   FGuid::NewGuid().ToString(EGuidFormats::Digits) +
                                                                   TEXT(".das");
        FString Logic;
        Check(FFileHelper::LoadFileToString(Logic, *PersistentRuntime.ScriptPath(TEXT("persistent_counter.das"))),
                                            TEXT("load_logic_fixture"));
        auto WriteLogic = [&](const FString& Text)
        {
            return FFileHelper::SaveStringToFile(Text, *LogicPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
        };
        Check(WriteLogic(Logic), TEXT("write_PIE_logic_v1"));
        auto CallLogic = [&](UObject* Self)
        {
            return PersistentRuntime.RunPersistent(LogicPath, 1, Self);
        };
        Check(CallLogic(Actor).Value == 1 && CallLogic(Actor).Value == 2 && CallLogic(Holder).Value == 1,
              TEXT("PIE_two_counters_before_reload"));
        Check(WriteLogic(Logic.Replace(TEXT("return count //"), TEXT("return count + 100 //"))),
                         TEXT("write_PIE_logic_v2"));
        Check(CallLogic(Actor).Value == 103 && CallLogic(Holder).Value == 102,
              TEXT("PIE_logic_reload_preserves_two_counters"));
        Check(WriteLogic(TEXT("invalid script")), TEXT("write_PIE_broken_logic"));
        const auto Fallback = CallLogic(Actor);
        Check(Fallback.bSucceeded && Fallback.Value == 104 && !Fallback.ReloadError.IsEmpty(),
              TEXT("PIE_broken_logic_falls_back"));
        IFileManager::Get().Delete(*LogicPath);
        FActorSpawnParameters DestroySpawn;
        auto* Doomed =
            PlayWorld->SpawnActor<ADasProbeActor>(State->Child->GeneratedClass, FTransform::Identity, DestroySpawn);
        TWeakObjectPtr<UObject> DestroyedSelf(Doomed);
        Check(Doomed && Doomed->ExecuteDas(TEXT("persistent_counter.das"), 1) == 1, TEXT("doomed_actor_has_context"));
        const int32 BeforeDestroy = PersistentRuntime.PersistentContextCount(PlayWorld);
        Check(Doomed && Doomed->Destroy(), TEXT("actor_destroyed_for_handle_test"));
        Check(PersistentRuntime.PersistentContextCount(PlayWorld) == BeforeDestroy - 1,
              TEXT("destroy_releases_context_immediately"));
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        const auto DeadCall = Runtime.RunFile(Runtime.ScriptPath(TEXT("self_properties.das")), 1, false, DestroyedSelf);
        Check(!DeadCall.bSucceeded && DeadCall.Error.Contains(TEXT("self is missing or expired")),
                                                              TEXT("destroyed_actor_self_rejected"));
        GEditor->RequestEndPlayMap();
        Started = FPlatformTime::Seconds();
        bEnding = true;
        return false;
    }
};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasPIEReloadTest, "DasForUE.Reload.PIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDasPIEReloadTest::RunTest(const FString& Parameters)
{
    auto State = MakeShared<FDasPIEState>();
    UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
    if (!TestNotNull(TEXT("Editor world"), World))
        return false;
    const FString Root = TEXT("/Game/DasReloadProbe/PIE_") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
    State->Parent.Reset(Blueprint(Root + TEXT("/BP_DasGenerated"), ADasProbeActor::StaticClass()));
    FString Error;
    if (!TestTrue(TEXT("Parent schema"),
                       State->Parent.IsValid() && ApplySchema(State->Parent.Get(), TEXT("schema_v1.das"), Error)))
        return false;
    State->Child.Reset(Blueprint(Root + TEXT("/BP_Child"), State->Parent->GeneratedClass));
    if (!TestTrue(TEXT("Child class"), State->Child.IsValid() && Compile(State->Child.Get())))
        return false;
    SetFloat(State->Child->GeneratedClass->GetDefaultObject(), TEXT("speed"), 20.f);
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("ProbeInstance");
    auto* Actor =
        World->SpawnActor<ADasProbeActor>(State->Child->GeneratedClass, FTransform(FVector(11, 22, 33)), Spawn);
    Spawn.Name = TEXT("ReferenceHolder");
    auto* Holder = World->SpawnActor<ADasProbeActor>(ADasProbeActor::StaticClass(), FTransform::Identity, Spawn);
    if (!TestTrue(TEXT("Editor actors"), Actor && Holder))
        return false;
    SetFloat(Actor, TEXT("speed"), 37.f);
    Holder->Peer = Actor;
    Actor->Peer = Holder;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasPIEReloadCommand>(State, this));
    return true;
}

namespace
{
struct FDasLifecyclePIEState
{
    TStrongObjectPtr<UBlueprint> Blueprint;
    TStrongObjectPtr<UDasLifecycleTestObserver> DestroyObserver{NewObject<UDasLifecycleTestObserver>()};
    TStrongObjectPtr<UDasLifecycleTestObserver> StopObserver{NewObject<UDasLifecycleTestObserver>()};
    FString Path;
    FString Source;
    bool bPassed = true;
};

class FDasLifecyclePIECommand final : public IAutomationLatentCommand
{
    TSharedRef<FDasLifecyclePIEState> State;
    FAutomationTestBase* Test;
    int32 Phase = 0;
    int32 TicksBefore = 0;
    double Started = FPlatformTime::Seconds();

public:
    FDasLifecyclePIECommand(TSharedRef<FDasLifecyclePIEState> InState, FAutomationTestBase* InTest)
        : State(InState), Test(InTest)
    {
    }
    virtual bool Update() override
    {
        auto Check = [&](bool Condition, const TCHAR* Name)
        {
            State->bPassed &= Test->TestTrue(Name, Condition);
            UE_LOG(LogTemp, Display, TEXT("DAS_LIFECYCLE %s %s"), Condition ? TEXT("PASS") : TEXT("FAIL"), Name);
        };
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        if (Phase == 4)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Started < 30)
                return false;
            Check(!GEditor->PlayWorld, TEXT("PIE_stopped"));
            Check(State->StopObserver->Calls == 1 && State->StopObserver->Reason == EEndPlayReason::EndPlayInEditor &&
                      State->StopObserver->Health == 201 + static_cast<int32>(EEndPlayReason::EndPlayInEditor),
                  TEXT("EndPIE_callback_once_with_self_and_reason"));
            Check(State->DestroyObserver->Calls == 1, TEXT("destroyed_actor_not_ended_twice"));
            Check(Runtime.PersistentContextCount() == 0, TEXT("all_lifecycle_contexts_released"));
            IFileManager::Get().Delete(*State->Path);
            UE_LOG(LogTemp, Display, TEXT("DAS_LIFECYCLE_RESULT=%s"), State->bPassed ? TEXT("PASS") : TEXT("FAIL"));
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        auto* First = World ? Find(World, TEXT("LifecycleFirst")) : nullptr;
        auto* Second = World ? Find(World, TEXT("LifecycleSecond")) : nullptr;
        if (!World || !World->HasBegunPlay() || !First || !Second)
        {
            if (FPlatformTime::Seconds() - Started < 60)
                return false;
            Check(false, TEXT("lifecycle_actors_started_before_timeout"));
            GEditor->RequestEndPlayMap();
            Phase = 4;
            Started = FPlatformTime::Seconds();
            return false;
        }
        auto Call = [&](ADasProbeActor* Actor, int32 Input)
        {
            return Runtime.RunPersistent(State->Path, Input, Actor);
        };
        const auto Ticks = Call(First, 1);
        if (Ticks.Value <= TicksBefore && FPlatformTime::Seconds() - Started < 30)
            return false;
        Check(Ticks.bSucceeded && Ticks.Value > TicksBefore, TEXT("UE_delivered_real_ticks"));
        if (Phase == 0)
        {
            Check(Call(First, 0).Value == 1 && Call(Second, 0).Value == 1 &&
                  Int(First, TEXT("health")) == 101 && Int(Second, TEXT("health")) == 101,
                                                           TEXT("UE_delivered_BeginPlay_once_per_actor"));
            Check(Float(First, TEXT("speed")) > 10.f, TEXT("tick_delta_reaches_script"));
            auto* EditorActor = Find(GEditor->GetEditorWorldContext().World(), TEXT("LifecycleFirst"));
            Check(EditorActor &&
                  Int(EditorActor, TEXT("health")) == 100 &&
                      Float(EditorActor, TEXT("speed")) == 10.f &&
                            Runtime.PersistentContextCount(GEditor->GetEditorWorldContext().World()) == 0,
                            TEXT("editor_world_does_not_receive_play_callbacks"));
            First->OnEndPlay.AddDynamic(State->DestroyObserver.Get(), &UDasLifecycleTestObserver::OnEnded);
            Second->OnEndPlay.AddDynamic(State->StopObserver.Get(), &UDasLifecycleTestObserver::OnEnded);
            const FString V2 =
                State->Source.Replace(TEXT("if (input == 2) { return 1 }"), TEXT("if (input == 2) { return 2 }"));
            Check(FFileHelper::SaveStringToFile(V2, *State->Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                  TEXT("write_callback_reload"));
            TicksBefore = Ticks.Value;
            Phase = 1;
            Started = FPlatformTime::Seconds();
            return false;
        }
        if (Phase == 1)
        {
            Check(Call(First, 2).Value == 2 && Call(Second, 2).Value == 2 && Call(First, 0).Value == 1 &&
                  Call(Second, 0).Value == 1 &&
                  Int(First, TEXT("health")) == 101, TEXT("hot_reload_keeps_state_without_repeating_begin"));
            Check(FFileHelper::SaveStringToFile(TEXT("invalid script"), *State->Path),
                                                TEXT("write_invalid_callback_source"));
            TicksBefore = Ticks.Value;
            Phase = 2;
            Started = FPlatformTime::Seconds();
            return false;
        }
        if (Phase == 2)
        {
            Check(!First->LastDasError.IsEmpty() && Call(First, 2).Value == 2 && Call(First, 0).Value == 1,
                  TEXT("real_ticks_continue_on_last_good_code"));
            const int32 Before = Runtime.PersistentContextCount(World);
            Check(First->Destroy(), TEXT("destroy_lifecycle_actor"));
            Check(State->DestroyObserver->Calls == 1 && State->DestroyObserver->Reason == EEndPlayReason::Destroyed &&
                      State->DestroyObserver->Health == 201,
                  TEXT("Destroy_calls_end_before_context_release"));
            Check(Runtime.PersistentContextCount(World) == Before - 1, TEXT("Destroy_releases_lifecycle_context"));
            GEditor->RequestEndPlayMap();
            Phase = 4;
            Started = FPlatformTime::Seconds();
            return false;
        }
        return false;
    }
};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasLifecyclePIETest, "DasForUE.Lifecycle.PIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasLifecyclePIETest::RunTest(const FString& Parameters)
{
    auto State = MakeShared<FDasLifecyclePIEState>();
    UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
    if (!TestNotNull(TEXT("Lifecycle editor world"), World))
        return false;
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString Root = TEXT("/Game/DasReloadProbe/Lifecycle_") + Id;
    State->Blueprint.Reset(Blueprint(Root + TEXT("/BP_Lifecycle"), ADasProbeActor::StaticClass()));
    FString Error;
    if (!TestTrue(TEXT("Lifecycle schema"),
                       State->Blueprint.IsValid() && ApplySchema(State->Blueprint.Get(), TEXT("schema_v2.das"), Error)))
        return false;
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    State->Path =
        FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / (TEXT("Lifecycle_") + Id + TEXT(".das")));
    if (!TestTrue(TEXT("Read lifecycle fixture"),
                       FFileHelper::LoadFileToString(State->Source, *Runtime.ScriptPath(TEXT("lifecycle_actor.das")))))
        return false;
    State->Source =
        State->Source.Replace(TEXT("if (input == 0)"), TEXT("if (input == 2) { return 1 }\n    if (input == 0)"));
    if (!TestTrue(TEXT("Write lifecycle fixture"),
                       FFileHelper::SaveStringToFile(State->Source, *State->Path,
                                                     FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM)))
        return false;
    for (const TCHAR* Name : {TEXT("LifecycleFirst"), TEXT("LifecycleSecond")})
    {
        FActorSpawnParameters Spawn;
        Spawn.Name = Name;
        auto* Actor = World->SpawnActor<ADasProbeActor>(State->Blueprint->GeneratedClass, FTransform::Identity, Spawn);
        if (!TestNotNull(TEXT("Lifecycle actor"), Actor))
            return false;
        Actor->LifecycleScript = State->Path;
    }
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasLifecyclePIECommand>(State, this));
    return true;
}


namespace
{
struct FDasDelegatePIEState
{
    FString Path;
    FString Source;
    bool bPassed = true;
};
class FDasDelegatePIECommand final : public IAutomationLatentCommand
{
    TSharedRef<FDasDelegatePIEState> State;
    FAutomationTestBase* Test;
    int32 Phase = 0;
    double Started = FPlatformTime::Seconds();

public:
    FDasDelegatePIECommand(TSharedRef<FDasDelegatePIEState> InState, FAutomationTestBase* InTest)
        : State(InState), Test(InTest)
    {
    }
    virtual bool Update() override
    {
        auto Check = [&](bool Condition, const TCHAR* Name)
        {
            State->bPassed &= Test->TestTrue(Name, Condition);
            UE_LOG(LogTemp, Display, TEXT("DAS_DELEGATES %s %s"), Condition ? TEXT("PASS") : TEXT("FAIL"), Name);
        };
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        if (Phase == 1)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Started < 30)
                return false;
            Check(!GEditor->PlayWorld && Runtime.SubscriptionCount() == 0 && Runtime.PersistentContextCount() == 0,
                  TEXT("EndPIE_revokes_all_subscriptions_and_contexts"));
            IFileManager::Get().Delete(*State->Path);
            UE_LOG(LogTemp, Display, TEXT("DAS_DELEGATES_RESULT=%s"), State->bPassed ? TEXT("PASS") : TEXT("FAIL"));
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        auto* Source = World ? Find(World, TEXT("SignalSource")) : nullptr;
        auto* First = World ? Find(World, TEXT("SignalFirst")) : nullptr;
        auto* Second = World ? Find(World, TEXT("SignalSecond")) : nullptr;
        if (!World || !World->HasBegunPlay() || !Source || !First || !Second)
        {
            if (FPlatformTime::Seconds() - Started < 60)
                return false;
            Check(false, TEXT("delegate_actors_started"));
            GEditor->RequestEndPlayMap();
            Phase = 1;
            Started = FPlatformTime::Seconds();
            return false;
        }
        auto Call = [&](ADasProbeActor* Actor)
        {
            return Runtime.RunPersistent(State->Path, 0, Actor);
        };
        Check(Runtime.SubscriptionCount(First) == 1 && Runtime.SubscriptionCount(Second) == 1,
              TEXT("BeginPlay_installs_two_peer_bindings"));
        Source->EmitDasSignal(3);
        Check(Call(First).Value == 3 && Call(Second).Value == 3, TEXT("one_source_delivers_to_two_independent_actors"));
        auto* EditorSource = Find(GEditor->GetEditorWorldContext().World(), TEXT("SignalSource"));
        Check(EditorSource && !EditorSource->OnDasSignal.IsBound() && First->Peer == Source && Second->Peer == Source,
              TEXT("PIE_sources_are_remapped_and_isolated_from_editor"));
        const FString V2 = State->Source.Replace(TEXT("+ value)"), TEXT("+ value * 10)"));
        Check(FFileHelper::SaveStringToFile(V2, *State->Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("write_delegate_reload"));
        Call(First);
        Call(Second);
        Source->EmitDasSignal(2);
        Check(Call(First).Value == 23 && Call(Second).Value == 23 && Source->OnDasSignal.GetAllObjects().Num() == 2,
              TEXT("reload_replaces_handlers_without_duplicate_delivery"));
        Check(FFileHelper::SaveStringToFile(TEXT("invalid script"), *State->Path),
                                            TEXT("write_invalid_delegate_candidate"));
        const auto Fallback = Call(First);
        Source->EmitDasSignal(1);
        Check(!Fallback.ReloadError.IsEmpty() && Call(First).Value == 33 && Call(Second).Value == 33,
              TEXT("failed_reload_preserves_both_bindings"));
        Check(First->Destroy() && Source->OnDasSignal.GetAllObjects().Num() == 1 && Runtime.SubscriptionCount() == 1,
              TEXT("Destroy_removes_only_destroyed_owner_subscription"));
        Source->EmitDasSignal(1);
        Check(Call(Second).Value == 43, TEXT("remaining_listener_still_receives"));
        // A self subscription recursively emits once. The bridge queues this instead of re-entering the VM.
        const FString Nested = TEXT("options gen2\nrequire ue_foundation\n[export]\ndef bind_events() { ue_subscribe_int(\"self\", \"OnDasSignal\", \"on_signal\") }\n[export]\ndef on_signal(value:int) { ue_state_set_int(\"total\", ue_state_get_int(\"total\", 0) + value); if (value == 1) { ue_emit_signal(2) } }\n[export]\ndef main(input:int):int { return ue_state_get_int(\"total\", 0) }\n");
        Check(FFileHelper::SaveStringToFile(Nested, *State->Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("write_nested_event_fixture"));
        Call(Second);
        Check(!Source->OnDasSignal.IsBound() && Second->OnDasSignal.GetAllObjects().Num() == 1,
              TEXT("reload_can_change_subscription_source"));
        Second->EmitDasSignal(1);
        Check(Call(Second).Value == 46 && Second->LastDasError.IsEmpty(),
              TEXT("nested_events_are_queued_without_VM_reentry"));
        const FString DestroyDuringCall =
            Nested.Replace(TEXT("def main(input:int):int { return ue_state_get_int(\"total\", 0) }"),
                                TEXT("def main(input:int):int { ue_emit_signal(99); return 42 }"));
        Check(FFileHelper::SaveStringToFile(DestroyDuringCall, *State->Path,
                                            FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("write_destroy_during_VM_fixture"));
        Check(Call(Source).Value == 42, TEXT("source_context_ready_for_reentrant_destroy"));
        TStrongObjectPtr<UDasDelegateDestroyer> Destroyer(NewObject<UDasDelegateDestroyer>());
        Destroyer->Target = Source;
        Source->OnDasSignal.AddDynamic(Destroyer.Get(), &UDasDelegateDestroyer::OnSignal);
        const auto LastCall = Call(Source);
        Check(LastCall.bSucceeded && LastCall.Value == 42 && Destroyer->Calls == 1 && Source->IsActorBeingDestroyed() &&
                  Runtime.SubscriptionCount() == 1 && Runtime.PersistentContextCount(World) == 1,
              TEXT("destroy_inside_VM_defers_context_free_and_revokes_callbacks"));

        GEditor->RequestEndPlayMap();
        Phase = 1;
        Started = FPlatformTime::Seconds();
        return false;
    }
};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasDelegatePIETest, "DasForUE.Delegates.PIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasDelegatePIETest::RunTest(const FString& Parameters)
{
    auto State = MakeShared<FDasDelegatePIEState>();
    UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
    if (!TestNotNull(TEXT("Delegate editor world"), World))
        return false;
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    State->Path = FPaths::ConvertRelativePathToFull(
        FPaths::ProjectSavedDir() /
        (TEXT("Delegates_") + FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT(".das")));
    if (!TestTrue(TEXT("Read delegate fixture"),
                       FFileHelper::LoadFileToString(State->Source, *Runtime.ScriptPath(TEXT("delegate_actor.das")))))
        return false;
    if (!TestTrue(TEXT("Write delegate fixture"),
                       FFileHelper::SaveStringToFile(State->Source, *State->Path,
                                                     FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM)))
        return false;
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("SignalSource");
    auto* Source = World->SpawnActor<ADasProbeActor>(ADasProbeActor::StaticClass(), FTransform::Identity, Spawn);
    if (!TestNotNull(TEXT("Event source"), Source))
        return false;
    for (const TCHAR* Name : {TEXT("SignalFirst"), TEXT("SignalSecond")})
    {
        Spawn.Name = Name;
        auto* Actor = World->SpawnActor<ADasProbeActor>(ADasProbeActor::StaticClass(), FTransform::Identity, Spawn);
        if (!TestNotNull(TEXT("Event listener"), Actor))
            return false;
        Actor->Peer = Source;
        Actor->LifecycleScript = State->Path;
    }
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasDelegatePIECommand>(State, this));
    return true;
}


namespace
{
struct FDasObjectsPIEState
{
    TStrongObjectPtr<UBlueprint> Blueprint;
    FString Path;
    bool bPassed = true;
};
class FDasObjectsPIECommand final : public IAutomationLatentCommand
{
    TSharedRef<FDasObjectsPIEState> State;
    FAutomationTestBase* Test;
    bool bEnding = false;
    double Started = FPlatformTime::Seconds();

public:
    FDasObjectsPIECommand(TSharedRef<FDasObjectsPIEState> InState, FAutomationTestBase* InTest)
        : State(InState), Test(InTest)
    {
    }
    virtual bool Update() override
    {
        auto Check = [&](bool Condition, const TCHAR* Name)
        {
            State->bPassed &= Test->TestTrue(Name, Condition);
            UE_LOG(LogTemp, Display, TEXT("DAS_OBJECTS %s %s"), Condition ? TEXT("PASS") : TEXT("FAIL"), Name);
        };
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        if (bEnding)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Started < 30)
                return false;
            Check(!GEditor->PlayWorld && Runtime.PersistentContextCount() == 0 && Runtime.SubscriptionCount() == 0,
                  TEXT("object_contexts_released_after_EndPIE"));
            IFileManager::Get().Delete(*State->Path);
            UE_LOG(LogTemp, Display, TEXT("DAS_OBJECTS_RESULT=%s"), State->bPassed ? TEXT("PASS") : TEXT("FAIL"));
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        auto* Source = World ? Find(World, TEXT("ObjectSource")) : nullptr;
        auto* Listener = World ? Find(World, TEXT("ObjectListener")) : nullptr;
        if (!World || !World->HasBegunPlay() || !Source || !Listener)
        {
            if (FPlatformTime::Seconds() - Started < 60)
                return false;
            Check(false, TEXT("object_actors_started"));
            GEditor->RequestEndPlayMap();
            bEnding = true;
            Started = FPlatformTime::Seconds();
            return false;
        }
        auto Call = [&](int32 Input)
        {
            return Runtime.RunPersistent(State->Path, Input, Listener);
        };
        const auto Echo = Call(0);
        Check(Echo.bSucceeded && Echo.Value == 40 &&
              Source->FindFunction(TEXT("Echo")) &&
                                   !Source->FindFunction(TEXT("Echo"))->HasAnyFunctionFlags(FUNC_Native),
                                                         TEXT("ProcessEvent_executes_Blueprint_function"));
        const auto GenericEcho = Call(7);
        Check(GenericEcho.bSucceeded && GenericEcho.Value == 42, TEXT("generic_frame_executes_Blueprint_function"));
        Source->SetActorLocation(FVector(123, 456, 789));
        const auto Location = Call(8);
        Check(Location.bSucceeded && Location.Value == 123, TEXT("generic_frame_reads_engine_FVector_return"));
        const auto Emit = Call(1);
        Check(Emit.bSucceeded && Call(2).Value == 3 && Runtime.SubscriptionCount(Listener) == 1,
              TEXT("reflected_native_call_queues_object_subscription"));
        Check(Call(5).bSucceeded && Listener->GetOwner() == Source, TEXT("typed_Actor_argument_calls_engine_SetOwner"));
        Check(Call(6).bSucceeded && Call(6).Value == 1, TEXT("engine_GetOwner_returns_typed_handle"));
        Listener->SetOwner(nullptr);
        Check(Call(9).bSucceeded && Listener->GetOwner() == Source, TEXT("generic_object_input_calls_engine_SetOwner"));
        const auto GenericOwner = Call(10);
        Check(GenericOwner.bSucceeded && GenericOwner.Value == 1, TEXT("generic_object_result_calls_engine_GetOwner"));
        const auto BlueprintOut = Call(11);
        Check(BlueprintOut.bSucceeded && BlueprintOut.Value == 77, TEXT("generic_out_reads_Blueprint_output_pin"));
        const auto TickGroup = Call(12);
        Check(TickGroup.bSucceeded && Source->PrimaryActorTick.TickGroup == TG_PostPhysics,
              TEXT("generic_enum_calls_engine_SetTickGroup"));
        Source->Tags.Add(TEXT("DasEnumTag_17"));
        const auto Tag = Call(13);
        Check(Tag.bSucceeded && Tag.Value == 1, TEXT("generic_FName_calls_engine_ActorHasTag"));
        const auto NameOutput = Call(14);
        Check(NameOutput.bSucceeded && NameOutput.Value == 1, TEXT("generic_FName_roundtrips_Blueprint_output"));
        Check(Listener->AttachToActor(Source, FAttachmentTransformRules::KeepWorldTransform),
              TEXT("attach_actor_for_generated_object_array"));
        const auto GeneratedActor = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_actor.das")), 0, false, Source);
        Check(GeneratedActor.bSucceeded && GeneratedActor.Value == 123 &&
                  Source->PrimaryActorTick.TickGroup == TG_PrePhysics,
              TEXT("generated_Actor_wrappers_execute_in_PIE"));
        TWeakObjectPtr<UObject> WeakSource(Source);
        Check(Source->Destroy(), TEXT("destroy_cached_handle_target"));
        const auto Invalid = Call(3);
        const auto Expired = Call(4);
        Check(Invalid.bSucceeded && Invalid.Value == 0 && !Expired.bSucceeded &&
              Expired.Error.Contains(TEXT("expired")), TEXT("cached_Actor_handle_rejected_immediately_after_Destroy"));
        CollectGarbage(RF_NoFlags);
        Check(!WeakSource.IsValid() && Runtime.SubscriptionCount(Listener) == 0 && Call(3).Value == 0,
              TEXT("dead_source_not_pinned_and_subscription_pruned"));
        GEditor->RequestEndPlayMap();
        bEnding = true;
        Started = FPlatformTime::Seconds();
        return false;
    }
};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasObjectsPIETest, "DasForUE.Objects.PIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasObjectsPIETest::RunTest(const FString& Parameters)
{
    auto State = MakeShared<FDasObjectsPIEState>();
    UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
    if (!TestNotNull(TEXT("Objects editor world"), World))
        return false;
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    State->Blueprint.Reset(
        Blueprint(TEXT("/Game/DasReloadProbe/Objects_") + Id + TEXT("/BP_Echo"), ADasProbeActor::StaticClass()));
    if (!TestNotNull(TEXT("Echo Blueprint"), State->Blueprint.Get()))
        return false;
    auto* BP = State->Blueprint.Get();
    auto* Graph = FBlueprintEditorUtils::CreateNewGraph(BP, TEXT("Echo"), UEdGraph::StaticClass(),
                                                                 UEdGraphSchema_K2::StaticClass());
    FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP, Graph, true, nullptr);
    TArray<UK2Node_FunctionEntry*> Entries;
    Graph->GetNodesOfClass(Entries);
    if (!TestTrue(TEXT("Echo entry"), Entries.Num() == 1))
        return false;
    FEdGraphPinType IntType;
    IntType.PinCategory = UEdGraphSchema_K2::PC_Int;
    auto* InputPin = Entries[0]->CreateUserDefinedPin(TEXT("Input"), IntType, EGPD_Output);
    FGraphNodeCreator<UK2Node_FunctionResult> Creator(*Graph);
    auto* Result = Creator.CreateNode();
    Creator.Finalize();
    auto* ReturnPin = Result->CreateUserDefinedPin(UEdGraphSchema_K2::PN_ReturnValue, IntType, EGPD_Input);
    const auto* Schema = Graph->GetSchema();
    if (!TestTrue(TEXT("Echo links"),
                       Schema->TryCreateConnection(InputPin, ReturnPin) &&
                           Schema->TryCreateConnection(Entries[0]->FindPinChecked(UEdGraphSchema_K2::PN_Then),
                                                       Result->FindPinChecked(UEdGraphSchema_K2::PN_Execute))))
        return false;
    auto* OutGraph = FBlueprintEditorUtils::CreateNewGraph(BP, TEXT("CopyOut"), UEdGraph::StaticClass(),
                                                                    UEdGraphSchema_K2::StaticClass());
    FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP, OutGraph, true, nullptr);
    TArray<UK2Node_FunctionEntry*> OutEntries;
    OutGraph->GetNodesOfClass(OutEntries);
    if (!TestTrue(TEXT("Out entry"), OutEntries.Num() == 1))
        return false;
    auto* OutInput = OutEntries[0]->CreateUserDefinedPin(TEXT("Input"), IntType, EGPD_Output);
    FGraphNodeCreator<UK2Node_FunctionResult> OutCreator(*OutGraph);
    auto* OutResult = OutCreator.CreateNode();
    OutCreator.Finalize();
    auto* OutPin = OutResult->CreateUserDefinedPin(TEXT("Copied"), IntType, EGPD_Input);
    if (!TestTrue(TEXT("Out links"),
                       Schema->TryCreateConnection(OutInput, OutPin) &&
                           Schema->TryCreateConnection(OutEntries[0]->FindPinChecked(UEdGraphSchema_K2::PN_Then),
                                                       OutResult->FindPinChecked(UEdGraphSchema_K2::PN_Execute))))
        return false;
    auto* NameGraph = FBlueprintEditorUtils::CreateNewGraph(BP, TEXT("CopyName"), UEdGraph::StaticClass(),
                                                                     UEdGraphSchema_K2::StaticClass());
    FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP, NameGraph, true, nullptr);
    TArray<UK2Node_FunctionEntry*> NameEntries;
    NameGraph->GetNodesOfClass(NameEntries);
    if (!TestTrue(TEXT("Name entry"), NameEntries.Num() == 1))
        return false;
    FEdGraphPinType NameType;
    NameType.PinCategory = UEdGraphSchema_K2::PC_Name;
    auto* NameInput = NameEntries[0]->CreateUserDefinedPin(TEXT("NameIn"), NameType, EGPD_Output);
    FGraphNodeCreator<UK2Node_FunctionResult> NameCreator(*NameGraph);
    auto* NameResult = NameCreator.CreateNode();
    NameCreator.Finalize();
    auto* NameOutput = NameResult->CreateUserDefinedPin(TEXT("NameOut"), NameType, EGPD_Input);
    if (!TestTrue(TEXT("Name links"),
                       Schema->TryCreateConnection(NameInput, NameOutput) &&
                           Schema->TryCreateConnection(NameEntries[0]->FindPinChecked(UEdGraphSchema_K2::PN_Then),
                                                       NameResult->FindPinChecked(UEdGraphSchema_K2::PN_Execute))))
        return false;
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
    if (!TestTrue(TEXT("Echo compile"), Compile(BP)))
        return false;
    State->Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / (TEXT("Objects_") + Id + TEXT(".das")));
    const FString Script = TEXT("options gen2\nrequire ue_foundation\nvar cached = ue_null_object()\n[export]\ndef bind_events() { ue_subscribe_object_int(ue_get_object(ue_self_object(), \"Peer\"), \"OnDasSignal\", \"on_value\") }\n[export]\ndef on_value(value:int) { ue_state_set_int(\"sum\", ue_state_get_int(\"sum\", 0) + value) }\n[export]\ndef main(input:int):int { if (input == 0) { cached = ue_cast_object(ue_get_object(ue_self_object(), \"Peer\"), \"/Script/Engine.Actor\"); return ue_call_int(cached, \"Echo\", 40) }; if (input == 1) { ue_call_void_int(cached, \"EmitDasSignal\", 3); return 0 }; if (input == 2) { return ue_state_get_int(\"sum\", 0) }; if (input == 3) { if (ue_object_valid(cached)) { return 1 }; return 0 }; if (input == 7) { let c = ue_begin_call(cached, \"Echo\"); ue_arg_int(c, \"Input\", 42); ue_invoke_call(c); let v = ue_result_int(c, \"ReturnValue\"); ue_release_call(c); return v }; if (input == 8) { let c = ue_begin_call(cached, \"K2_GetActorLocation\"); ue_invoke_call(c); let x = ue_result_double(c, \"ReturnValue.X\"); ue_release_call(c); return int(x) }; if (input == 9) { let c = ue_begin_call(ue_self_object(), \"SetOwner\"); ue_arg_object(c, \"NewOwner\", cached); ue_invoke_call(c); return 0 }; if (input == 10) { let c = ue_begin_call(ue_self_object(), \"GetOwner\"); ue_invoke_call(c); let obj = ue_result_object(c, \"ReturnValue\"); if (ue_object_is_a(obj, \"/Script/Engine.Actor\")) { return 1 }; return 0 }; if (input == 11) { let c = ue_begin_call(cached, \"CopyOut\"); ue_arg_int(c, \"Input\", 77); ue_invoke_call(c); return ue_result_int(c, \"Copied\") }; if (input == 12) { let c = ue_begin_call(cached, \"SetTickGroup\"); ue_arg_enum(c, \"NewTickGroup\", \"/Script/Engine.ETickingGroup\", \"TG_PostPhysics\"); ue_invoke_call(c); return 1 }; if (input == 13) { let c = ue_begin_call(cached, \"ActorHasTag\"); ue_arg_name(c, \"Tag\", \"dasenumtag_17\"); ue_invoke_call(c); if (ue_result_bool(c, \"ReturnValue\")) { return 1 }; return 0 }; if (input == 14) { let c = ue_begin_call(cached, \"CopyName\"); ue_arg_name(c, \"NameIn\", \"BlueprintName_23\"); ue_invoke_call(c); if (ue_result_name(c, \"NameOut\") == \"BlueprintName_23\") { return 1 }; return 0 }; if (input == 5) { ue_call_void_object(ue_self_object(), \"SetOwner\", cached); return 0 }; if (input == 6) { let owner = ue_call_object(ue_self_object(), \"GetOwner\"); if (ue_object_is_a(owner, \"/Script/Engine.Actor\")) { return 1 }; return 0 }; return ue_call_int(cached, \"Echo\", 40) }\n");
    if (!TestTrue(TEXT("Write objects fixture"),
                       FFileHelper::SaveStringToFile(Script, *State->Path,
                                                     FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM)))
        return false;
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("ObjectSource");
    auto* Source = World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
    Spawn.Name = TEXT("ObjectListener");
    auto* Listener = World->SpawnActor<ADasProbeActor>(ADasProbeActor::StaticClass(), FTransform::Identity, Spawn);
    if (!TestTrue(TEXT("Objects actors"), Source && Listener))
        return false;
    Listener->Peer = Source;
    Listener->LifecycleScript = State->Path;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasObjectsPIECommand>(State, this));
    return true;
}

#endif


#include "DasDSLService.h"
#include "HAL/IConsoleManager.h"
namespace
{
struct FDasDSLPIEState
{
    TStrongObjectPtr<UBlueprint> Blueprint;
    FString ScriptName, ScriptPath, Source1, Source2;
    bool Passed = true;
};
class FDasDSLPIECommand final : public IAutomationLatentCommand
{
    TSharedRef<FDasDSLPIEState> S;
    FAutomationTestBase* Test;
    double Start = 0;
    bool Ending = false;

public:
    FDasDSLPIECommand(TSharedRef<FDasDSLPIEState> State, FAutomationTestBase* T) : S(State), Test(T) {}
    virtual bool Update() override
    {
        if (Start == 0)
            Start = FPlatformTime::Seconds();
        auto Check = [&](bool Value, const TCHAR* Name)
        {
            S->Passed &= Test->TestTrue(Name, Value);
            UE_LOG(LogTemp, Display, TEXT("DAS_DSL_PIE %s %s"), Value ? TEXT("PASS"): TEXT("FAIL"), Name);
        };
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        if (Ending)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Start < 30)
                return false;
            Check(!GEditor->PlayWorld, TEXT("EndPIE"));
            Check(Runtime.PersistentContextCount() ==
                      Runtime.PersistentContextCount(GEditor->GetEditorWorldContext().World()),
                  TEXT("EndPIE_releases_play_contexts"));
            IFileManager::Get().Delete(*S->ScriptPath);
            UE_LOG(LogTemp, Display, TEXT("DAS_DSL_PIE_RESULT=%s"), S->Passed ? TEXT("PASS"): TEXT("FAIL"));
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        if (!World || !World->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Start < 60)
                return false;
            Check(false, TEXT("PIE_started"));
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* Actor = Find(World, TEXT("DSLActor"));
        auto* Holder = Find(World, TEXT("DSLHolder"));
        Check(Actor && Actor->HasActorBegunPlay() && Evaluate(Actor) == 73, TEXT("native_DSL_method_in_real_PIE"));
        if (!Actor)
        {
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("Score"))->SetPropertyValue_InContainer(Actor, 77);
        auto Write = [&](const FString& Text)
        {
            return FFileHelper::SaveStringToFile(Text, *S->ScriptPath,
                                                 FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
        };
        auto Call = [&]()
        {
            return Runtime.RunNamedPersistent(S->ScriptPath, TEXT("ue_Counter_Evaluate"), 5, Actor);
        };
        Check(Call().Value == 82, TEXT("PIE_override_before_reload"));
        const FString Body = S->Source1.Replace(TEXT("self.Score + input"), TEXT("self.Score + input + 100"));
        Check(Write(Body), TEXT("write_body_change"));
        const auto Reload = Call();
        Check(Reload.bSucceeded && Reload.Value == 182 && Reload.ReloadError.IsEmpty(),
              TEXT("body_auto_reload_without_reinstance"));
        UClass* BeforeClass = Actor->GetClass();
        UObject* BeforeActor = Actor;
        FString Error;
        Check(FDasDSLService::Reimport(S->Blueprint.Get(), Error) &&
              Find(World, TEXT("DSLActor")) == BeforeActor && Actor->GetClass() == BeforeClass,
                   TEXT("body_only_reimport_preserves_actor_identity"));
        Check(Write(TEXT("broken DSL")), TEXT("write_invalid_DSL"));
        Check(!FDasDSLService::Reimport(S->Blueprint.Get(), Error) && Actor->GetClass() == BeforeClass,
              TEXT("invalid_reimport_no_Blueprint_mutation"));
        auto Fallback = Call();
        Check(Fallback.bSucceeded && Fallback.Value == 182 && !Fallback.ReloadError.IsEmpty(),
              TEXT("broken_DSL_keeps_last_good_body"));
        Check(Write(Body) && Call().ReloadError.IsEmpty(), TEXT("body_reload_recovers"));
        Check(Write(S->Source2), TEXT("write_structural_DSL_change"));
        Fallback = Call();
        Check(Fallback.bSucceeded && Fallback.Value == 182 &&
              Fallback.ReloadError.Contains(TEXT("apply/reimport")),
                                            TEXT("structural_candidate_waits_for_Unreal_apply"));
        bool Replaced = false;
        auto Handle = FCoreUObjectDelegates::OnObjectsReplaced.AddLambda(
            [&](const FCoreUObjectDelegates::FReplacementObjectMap& Map) { Replaced |= Map.Contains(BeforeActor); });
        Check(FDasDSLService::Reimport(S->Blueprint.Get(), Error), TEXT("structural_reimport_during_PIE"));
        FCoreUObjectDelegates::OnObjectsReplaced.Remove(Handle);
        if (!Error.IsEmpty())
            Test->AddInfo(Error);
        Actor = Find(World, TEXT("DSLActor"));
        Holder = Find(World, TEXT("DSLHolder"));
        auto* EditorActor = Find(GEditor->GetEditorWorldContext().World(), TEXT("DSLActor"));
        Check(Replaced && Actor && Actor->HasActorBegunPlay() && Actor->GetClass() == S->Blueprint->GeneratedClass,
              TEXT("PIE_actor_reinstanced_with_BeginPlay_state"));
        Check(Actor && Int(Actor, TEXT("Score")) == 77 && Int(Actor, TEXT("Bonus")) == 5 && Evaluate(Actor) == 222,
                                                              TEXT("new_field_and_body_preserve_override"));
        Check(EditorActor && Int(EditorActor, TEXT("Score")) == 33 && Int(EditorActor, TEXT("Bonus")) == 5,
                                                                          TEXT("editor_world_override_isolated"));
        Check(Actor && Holder && Holder->Peer == Actor && Actor->Peer == Holder, TEXT("references_preserved"));
        Check(Actor && Call().Value == 187, TEXT("new_declaration_runtime_accepted"));
        // A new default during PIE updates the class and the editor world; the running actor keeps its value.
        Check(Write(S->Source2.Replace(TEXT("Score : int = 10"), TEXT("Score : int = 999"))),
                    TEXT("write_default_change"));
        Check(FDasDSLService::Reimport(S->Blueprint.Get(), Error) &&
              Int(S->Blueprint->GeneratedClass->GetDefaultObject(), TEXT("Score")) == 999,
                  TEXT("default_change_applied_during_PIE"));
        Actor = Find(World, TEXT("DSLActor"));
        Fallback = Call();
        Check(Actor && Int(Actor, TEXT("Score")) == 77 && Fallback.bSucceeded && Fallback.Value == 187 &&
                           Fallback.ReloadError.IsEmpty() &&
                           Int(Find(GEditor->GetEditorWorldContext().World(), TEXT("DSLActor")), TEXT("Score")) == 33,
                               TEXT("default_change_keeps_game_state_and_overrides"));
        Check(Write(S->Source2) && FDasDSLService::Reimport(S->Blueprint.Get(), Error) && Call().ReloadError.IsEmpty(),
              TEXT("restore_last_good_source"));
        CollectGarbage(RF_NoFlags);
        Actor = Find(World, TEXT("DSLActor"));
        Check(Actor && Int(Actor, TEXT("Score")) == 77 && Evaluate(Actor) == 222, TEXT("reimport_survives_GC"));
        GEditor->RequestEndPlayMap();
        Ending = true;
        Start = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasDSLPIETest, "DasForUE.DSL.PIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasDSLPIETest::RunTest(const FString& Parameters)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
    {
        AddError(TEXT("DSL probe requires isolated smoke project"));
        return false;
    }
    auto S = MakeShared<FDasDSLPIEState>();
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Guid = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    S->ScriptName = TEXT("dsl_pie_")+ Guid + TEXT(".das");
    S->ScriptPath = Runtime.ScriptPath(S->ScriptName);
    if (!FFileHelper::LoadFileToString(S->Source1, *Runtime.ScriptPath(TEXT("dsl_counter_v1.das"))) ||
                                       !FFileHelper::LoadFileToString(S->Source2,
                                                                      *Runtime.ScriptPath(TEXT("dsl_counter_v2.das"))))
        return false;
    if (!FFileHelper::SaveStringToFile(S->Source1, *S->ScriptPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return false;
    UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
    const FString Package = TEXT("/Game/DasDSLProbe/Run_")+ Guid + TEXT("/BP_Counter");
    IConsoleManager::Get().ProcessUserConsoleInput(
        *(TEXT("Das.DSL.Apply ")+ S->ScriptName + TEXT(" ")+ Package + TEXT(" example.counter")), *GLog, World);
    UBlueprint* BP = LoadObject<UBlueprint>(nullptr, *(Package + TEXT(".BP_Counter")));
    if (!TestNotNull(TEXT("console command creates Blueprint"), BP))
    {
        IFileManager::Get().Delete(*S->ScriptPath);
        return false;
    }
    S->Blueprint.Reset(BP);
    FString Error;
    UBlueprint* Ignored = nullptr;
    TestTrue(TEXT("invalid package/source refused"),
                  !FDasDSLService::Apply(TEXT("../bad.das"), Package, TEXT("example.counter"), Ignored, Error));
    IConsoleManager::Get().ProcessUserConsoleInput(*(TEXT("Das.DSL.Reimport ")+ Package), *GLog, World);
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("DSLActor");
    auto* Actor = World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
    Spawn.Name = TEXT("DSLHolder");
    auto* Holder = World->SpawnActor<ADasProbeActor>(ADasProbeActor::StaticClass(), FTransform::Identity, Spawn);
    if (!Actor || !Holder)
    {
        IFileManager::Get().Delete(*S->ScriptPath);
        return false;
    }
    FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("Score"))->SetPropertyValue_InContainer(Actor, 33);
    Holder->Peer = Actor;
    Actor->Peer = Holder;
    // PIE duplicates the editor override; later writes exercise world isolation.
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasDSLPIECommand>(S, this));
    return true;
}


#include "DasAuthoring.h"
namespace
{
class FDasScalarPIECommand final : public IAutomationLatentCommand
{
    TStrongObjectPtr<UBlueprint> BP;
    FAutomationTestBase* Test;
    double Start = 0;
    bool Ending = false, Passed = true;

public:
    FDasScalarPIECommand(UBlueprint* B, FAutomationTestBase* T) : BP(B), Test(T) {}
    virtual bool Update() override
    {
        if (Start == 0)
            Start = FPlatformTime::Seconds();
        auto Check = [&](bool V, const TCHAR* N)
        {
            Passed &= Test->TestTrue(N, V);
            UE_LOG(LogTemp, Display, TEXT("DAS_SCALAR_PIE %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N);
        };
        if (Ending)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Start < 30)
                return false;
            Check(!GEditor->PlayWorld, TEXT("EndPIE"));
            UE_LOG(LogTemp, Display, TEXT("DAS_SCALAR_PIE_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        if (!World || !World->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Start < 60)
                return false;
            Check(false, TEXT("PIE_started"));
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* Actor = Find(World, TEXT("ScalarActor"));
        Check(Actor && Int(Actor, TEXT("health")) == 10 && Float(Actor, TEXT("speed")) == 1.5f,
                                                                 TEXT("scalar_defaults"));
        auto* InitialBool = Actor ? FindFProperty<FBoolProperty>(Actor->GetClass(), TEXT("bScalar")) : nullptr;
        Check(InitialBool && InitialBool->GetPropertyValue_InContainer(Actor), TEXT("bool_true_Blueprint_default"));
        if (!Actor)
        {
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto Invoke = [&](const TCHAR* Name, auto SetInput, auto GetResult)
        {
            UFunction* Method = Actor->FindFunction(Name);
            if (!Method)
                return false;
            FStructOnScope Frame(Method);
            if (!SetInput(Method, Frame.GetStructMemory()))
                return false;
            FEditorScriptExecutionGuard Guard;
            Actor->ProcessEvent(Method, Frame.GetStructMemory());
            return GetResult(Method, Frame.GetStructMemory());
        };
        Check(Invoke(
            TEXT("Scale"),
                 [](UFunction* F, uint8* Data)
                 {
                     auto* P = FindFProperty<FFloatProperty>(F, TEXT("Input"));
                     if (!P)
                         return false;
                     P->SetPropertyValue_InContainer(Data, 2.25f);
                     return true;
                 },
                 [](UFunction* F, uint8* Data)
                 {
                     auto* P = CastField<FFloatProperty>(F->GetReturnProperty());
                     return P && P->GetPropertyValue_InContainer(Data) == 3.75f;
                 }),
            TEXT("float_ProcessEvent_input_return"));
        Check(Invoke(
            TEXT("Toggle"),
                 [](UFunction* F, uint8* Data)
                 {
                     auto* P = FindFProperty<FBoolProperty>(F, TEXT("Input"));
                     if (!P)
                         return false;
                     P->SetPropertyValue_InContainer(Data, true);
                     return true;
                 },
                 [](UFunction* F, uint8* Data)
                 {
                     auto* P = CastField<FBoolProperty>(F->GetReturnProperty());
                     return P && P->GetPropertyValue_InContainer(Data);
                 }),
            TEXT("bool_ProcessEvent_input_return"));
        auto* Flag = FindFProperty<FBoolProperty>(Actor->GetClass(), TEXT("bScalar"));
        Check(Flag && Flag->GetPropertyValue_InContainer(Actor), TEXT("bool_reflected_field_write"));
        Check(Invoke(
            TEXT("Check"),
                 [](UFunction* F, uint8* Data)
                 {
                     auto* P = FindFProperty<FIntProperty>(F, TEXT("Input"));
                     if (!P)
                         return false;
                     P->SetPropertyValue_InContainer(Data, 5);
                     return true;
                 },
                 [](UFunction* F, uint8* Data)
                 {
                     auto* P = CastField<FIntProperty>(F->GetReturnProperty());
                     return P && P->GetPropertyValue_InContainer(Data) == 15;
                 }),
            TEXT("int_method_shares_context"));
        Check(Invoke(
            TEXT("Reset"), [](UFunction* F, uint8*) { return F->NumParms == 0; },
                 [](UFunction* F, uint8*) { return !F->GetReturnProperty(); }) &&
            Int(Actor, TEXT("health")) == 0 && Flag && !Flag->GetPropertyValue_InContainer(Actor) &&
                Float(Actor, TEXT("speed")) == 3.75f, TEXT("void_action_and_bool_reset"));
        auto* EditorActor = Find(GEditor->GetEditorWorldContext().World(), TEXT("ScalarActor"));
        Check(EditorActor && Int(EditorActor, TEXT("health")) == 10 && Float(EditorActor, TEXT("speed")) == 1.5f,
                                                                             TEXT("scalar_world_isolation"));
        FString Error;
        UClass* Before = Actor->GetClass();
        Check(FDasDSLService::Reimport(BP.Get(), Error) && Actor->GetClass() == Before,
              TEXT("typed_reimport_without_reinstance"));
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        FDasAuthoringModule Model;
        if (Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_scalar_methods.das")), Model, Error))
        {
            auto* M = Model.Classes[0].Methods.FindByPredicate([](const auto& X) { return X.Name == TEXT("Scale"); });
            if (M)
            {
                M->ReturnType.Kind = TEXT("int");
                M->Parameters[0].Type.Kind = TEXT("int");
            }
            Check(M && !FDasBlueprintService::ApplyAuthoring(BP.Get(), Model, TEXT("typed.fixture"), Error) &&
                                                             BP->GeneratedClass == Before,
                                                             TEXT("typed_signature_mismatch_no_mutation"));
            if (M)
            {
                M->ReturnType.Kind = TEXT("float");
                M->Parameters[0].Type.Kind = TEXT("float");
            }
            auto* P =
                Model.Classes[0].Properties.FindByPredicate([](const auto& X) { return X.Name == TEXT("bScalar"); });
            if (P)
                P->Default.Boolean = false;
            Check(P && !FDasBlueprintService::ApplyAuthoring(BP.Get(), Model, TEXT("typed.fixture"), Error) &&
                                                             BP->GeneratedClass == Before,
                                                             TEXT("bool_default_migration_no_mutation"));
        }
        else
            Check(false, TEXT("typed_model_read"));
        CollectGarbage(RF_NoFlags);
        Actor = Find(World, TEXT("ScalarActor"));
        Check(Actor && Float(Actor, TEXT("speed")) == 3.75f && Int(Actor, TEXT("health")) == 0,
                                                                   TEXT("typed_state_survives_GC"));
        GEditor->RequestEndPlayMap();
        Ending = true;
        Start = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasScalarPIETest, "DasForUE.DSL.ScalarsPIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasScalarPIETest::RunTest(const FString& Parameters)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
    {
        AddError(TEXT("Isolated smoke project required"));
        return false;
    }
    UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
    UBlueprint* BP = nullptr;
    FString Error;
    const FString Package =
        TEXT("/Game/DasScalarProbe/Run_")+ FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT("/BP_Scalar");
    if (!TestTrue(TEXT("Create typed DSL Blueprint"), FDasDSLService::Apply(TEXT("dsl_scalar_methods.das"), Package,
                                                                                 TEXT("typed.fixture"), BP, Error)))
    {
        AddInfo(Error);
        return false;
    }
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("ScalarActor");
    auto* Actor = World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
    if (!TestNotNull(TEXT("Spawn scalar actor"), Actor))
        return false;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasScalarPIECommand>(BP, this));
    return true;
}


namespace
{
class FDasFramePIECommand final : public IAutomationLatentCommand
{
    TStrongObjectPtr<UBlueprint> BP;
    FAutomationTestBase* Test;
    double Start = 0;
    bool Ending = false, Passed = true;

public:
    FDasFramePIECommand(UBlueprint* B, FAutomationTestBase* T) : BP(B), Test(T) {}
    virtual bool Update() override
    {
        if (Start == 0)
            Start = FPlatformTime::Seconds();
        auto Check = [&](bool V, const TCHAR* N)
        {
            Passed &= Test->TestTrue(N, V);
            UE_LOG(LogTemp, Display, TEXT("DAS_FRAME_PIE %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N);
        };
        if (Ending)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Start < 30)
                return false;
            Check(!GEditor->PlayWorld, TEXT("EndPIE"));
            UE_LOG(LogTemp, Display, TEXT("DAS_FRAME_PIE_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        if (!World || !World->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Start < 60)
                return false;
            Check(false, TEXT("PIE_started"));
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* Actor = Find(World, TEXT("FrameActor"));
        Check(Actor != nullptr, TEXT("real_PIE_actor"));
        if (!Actor)
        {
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto Invoke = [&](const TCHAR* Name, auto Input, auto Output)
        {
            auto* F = Actor->FindFunction(Name);
            if (!F)
                return false;
            FStructOnScope Frame(F);
            if (!Input(F, Frame.GetStructMemory()))
                return false;
            FEditorScriptExecutionGuard Guard;
            Actor->ProcessEvent(F, Frame.GetStructMemory());
            return Output(F, Frame.GetStructMemory());
        };
        auto SetInt = [](UFunction* F, uint8* Data, const TCHAR* Name, int32 V)
        {
            auto* P = FindFProperty<FIntProperty>(F, Name);
            if (!P)
                return false;
            P->SetPropertyValue_InContainer(Data, V);
            return true;
        };
        auto SetBool = [](UFunction* F, uint8* Data, const TCHAR* Name, bool V)
        {
            auto* P = FindFProperty<FBoolProperty>(F, Name);
            if (!P)
                return false;
            P->SetPropertyValue_InContainer(Data, V);
            return true;
        };
        Check(Invoke(
            TEXT("Compute"),
                 [&](UFunction* F, uint8* D)
                 {
                     auto* P = FindFProperty<FFloatProperty>(F, TEXT("factor"));
                     if (!P)
                         return false;
                     P->SetPropertyValue_InContainer(D, 0.5f);
                     return SetInt(F, D, TEXT("count"), 2) && SetBool(F, D, TEXT("apply"), true);
                 },
                 [](UFunction* F, uint8* D)
                 {
                     auto* P = CastField<FFloatProperty>(F->GetReturnProperty());
                     return P && P->GetPropertyValue_InContainer(D) == 24.f;
                 }),
            TEXT("mixed_three_named_inputs_float_result"));
        Check(Invoke(
            TEXT("IsReady"), [&](UFunction* F, uint8* D)
                 { return SetInt(F, D, TEXT("minimum"), 11) && SetBool(F, D, TEXT("enabled"), true); },
                 [](UFunction* F, uint8* D)
                 {
                     auto* P = CastField<FBoolProperty>(F->GetReturnProperty());
                     return P && P->GetPropertyValue_InContainer(D);
                 }),
            TEXT("mixed_bool_result"));
        Check(Invoke(
            TEXT("ReadScore"), [](UFunction* F, uint8*) { return F->NumParms == 1; },
                 [](UFunction* F, uint8* D)
                 {
                     auto* P = CastField<FIntProperty>(F->GetReturnProperty());
                     return P && P->GetPropertyValue_InContainer(D) == 12;
                 }),
            TEXT("zero_input_scalar_result"));
        Check(Invoke(
            TEXT("BoolAsInt"), [&](UFunction* F, uint8* D) { return SetBool(F, D, TEXT("value"), true); },
                 [](UFunction* F, uint8* D)
                 {
                     auto* P = CastField<FIntProperty>(F->GetReturnProperty());
                     return P && P->GetPropertyValue_InContainer(D) == 12;
                 }),
            TEXT("single_input_different_result_type"));
        Check(Invoke(
            TEXT("ResetWith"), [&](UFunction* F, uint8* D)
                 { return SetInt(F, D, TEXT("amount"), 7) && SetBool(F, D, TEXT("resetGain"), true); },
                 [](UFunction* F, uint8*) { return F->NumParms == 2 && !F->GetReturnProperty(); }) &&
            Int(Actor, TEXT("health")) == 7 && Float(Actor, TEXT("speed")) == 1.5f, TEXT("mixed_void_action"));
        auto* EditorActor = Find(GEditor->GetEditorWorldContext().World(), TEXT("FrameActor"));
        Check(EditorActor && Int(EditorActor, TEXT("health")) == 10, TEXT("frame_world_isolation"));
        FString Error;
        UClass* Before = BP->GeneratedClass;
        Check(FDasDSLService::Reimport(BP.Get(), Error) && BP->GeneratedClass == Before,
              TEXT("frame_reimport_no_reinstance"));
        auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        FDasAuthoringModule Model;
        if (R.ReadAuthoringScript(R.ScriptPath(TEXT("dsl_mixed_methods.das")), Model, Error))
        {
            auto* M = Model.Classes[0].Methods.FindByPredicate([](const auto& X) { return X.Name == TEXT("Compute"); });
            if (M)
                M->Parameters.Swap(0, 1);
            Check(M && !FDasBlueprintService::ApplyAuthoring(BP.Get(), Model, TEXT("mixed.MixedCounter"), Error) &&
                                                             BP->GeneratedClass == Before,
                                                             TEXT("frame_parameter_reorder_no_mutation"));
            if (M)
            {
                M->Parameters.Swap(0, 1);
                M->Parameters[0].Id = TEXT("changed-parameter-id");
            }
            Check(M && !FDasBlueprintService::ApplyAuthoring(BP.Get(), Model, TEXT("mixed.MixedCounter"), Error) &&
                                                             BP->GeneratedClass == Before,
                                                             TEXT("frame_parameter_id_change_no_mutation"));
        }
        else
            Check(false, TEXT("frame_model_read"));
        CollectGarbage(RF_NoFlags);
        Actor = Find(World, TEXT("FrameActor"));
        Check(Actor && Int(Actor, TEXT("health")) == 7, TEXT("frame_state_survives_GC"));
        GEditor->RequestEndPlayMap();
        Ending = true;
        Start = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasFramePIETest, "DasForUE.DSL.FramePIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasFramePIETest::RunTest(const FString& Parameters)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
    {
        AddError(TEXT("Isolated project required"));
        return false;
    }
    auto* World = FAutomationEditorCommonUtils::CreateNewMap();
    UBlueprint* BP = nullptr;
    FString Error;
    const FString Package =
        TEXT("/Game/DasFrameProbe/Run_")+ FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT("/BP_Frame");
    if (!TestTrue(
            TEXT("Create mixed DSL Blueprint"),
                 FDasDSLService::Apply(TEXT("dsl_mixed_methods.das"), Package, TEXT("mixed.MixedCounter"), BP, Error)))
    {
        AddInfo(Error);
        return false;
    }
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("FrameActor");
    if (!TestNotNull(TEXT("Spawn frame actor"),
                          World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn)))
        return false;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasFramePIECommand>(BP, this));
    return true;
}


namespace
{
class FDasTextPIECommand final : public IAutomationLatentCommand
{
    TStrongObjectPtr<UBlueprint> BP;
    FAutomationTestBase* Test;
    double Start = 0;
    bool Ending = false, Passed = true;

public:
    FDasTextPIECommand(UBlueprint* B, FAutomationTestBase* T) : BP(B), Test(T) {}
    virtual bool Update() override
    {
        if (Start == 0)
            Start = FPlatformTime::Seconds();
        auto Check = [&](bool V, const TCHAR* N)
        {
            Passed &= Test->TestTrue(N, V);
            UE_LOG(LogTemp, Display, TEXT("DAS_TEXT_PIE %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N);
        };
        if (Ending)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Start < 30)
                return false;
            Check(!GEditor->PlayWorld, TEXT("EndPIE"));
            UE_LOG(LogTemp, Display, TEXT("DAS_TEXT_PIE_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        if (!World || !World->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Start < 60)
                return false;
            Check(false, TEXT("PIE_started"));
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* Actor = Find(World, TEXT("TextActor"));
        Check(Actor != nullptr, TEXT("real_PIE_actor"));
        if (!Actor)
        {
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* Text = FindFProperty<FStrProperty>(Actor->GetClass(), TEXT("DSLText"));
        auto* Number = FindFProperty<FDoubleProperty>(Actor->GetClass(), TEXT("DSLPrecise"));
        auto* Key = FindFProperty<FNameProperty>(Actor->GetClass(), TEXT("DSLKey"));
        Check(Text && Text->GetPropertyValue_InContainer(Actor) ==
                          TEXT("Привет") && Number && Number->GetPropertyValue_InContainer(Actor) == 1.23456789012345 &&
                               Key &&
                               Key->GetPropertyValue_InContainer(Actor) ==
                                   FName(TEXT("Initial_7")), TEXT("string_double_name_CDO_defaults"));
        auto Invoke = [&](const TCHAR* Name, auto Input, auto Output)
        {
            auto* F = Actor->FindFunction(Name);
            if (!F)
                return false;
            FStructOnScope Frame(F);
            if (!Input(F, Frame.GetStructMemory()))
                return false;
            FEditorScriptExecutionGuard Guard;
            Actor->ProcessEvent(F, Frame.GetStructMemory());
            return Output(F, Frame.GetStructMemory());
        };
        Check(Invoke(
            TEXT("Echo"),
                 [](UFunction* F, uint8* D)
                 {
                     auto* A = FindFProperty<FStrProperty>(F, TEXT("value"));
                     auto* B = FindFProperty<FStrProperty>(F, TEXT("suffix"));
                     if (!A || !B)
                         return false;
                     A->SetPropertyValue_InContainer(D, TEXT("Строка 世界"));
                     B->SetPropertyValue_InContainer(D, TEXT(" !"));
                     return true;
                 },
                 [](UFunction* F, uint8* D)
                 {
                     auto* P = CastField<FStrProperty>(F->GetReturnProperty());
                     return P && P->GetPropertyValue_InContainer(D) == TEXT("Строка 世界 !");
                 }),
            TEXT("unicode_string_ProcessEvent"));
        Check(Invoke(
            TEXT("Recall"), [](UFunction*, uint8*) { return true; },
                 [](UFunction* F, uint8* D)
                 {
                     auto* P = CastField<FStrProperty>(F->GetReturnProperty());
                     return P && P->GetPropertyValue_InContainer(D) == TEXT("Строка 世界");
                 }),
            TEXT("VM_owned_input_persists"));
        Check(Invoke(
            TEXT("Precise"),
                 [](UFunction* F, uint8* D)
                 {
                     auto* A = FindFProperty<FDoubleProperty>(F, TEXT("value"));
                     auto* B = FindFProperty<FDoubleProperty>(F, TEXT("delta"));
                     if (!A || !B)
                         return false;
                     A->SetPropertyValue_InContainer(D, 9007199254740990.0);
                     B->SetPropertyValue_InContainer(D, 1.0);
                     return true;
                 },
                 [](UFunction* F, uint8* D)
                 {
                     auto* P = CastField<FDoubleProperty>(F->GetReturnProperty());
                     return P && P->GetPropertyValue_InContainer(D) == 9007199254740991.0;
                 }),
            TEXT("double_ProcessEvent_64bit"));
        Check(Invoke(
            TEXT("Rename"),
                 [](UFunction* F, uint8* D)
                 {
                     auto* P = FindFProperty<FNameProperty>(F, TEXT("value"));
                     if (!P)
                         return false;
                     P->SetPropertyValue_InContainer(D, FName(TEXT("Item_42")));
                     return true;
                 },
                 [](UFunction* F, uint8* D)
                 {
                     auto* P = CastField<FNameProperty>(F->GetReturnProperty());
                     return P && P->GetPropertyValue_InContainer(D) == FName(TEXT("item_42"));
                 }),
            TEXT("nominal_name_ProcessEvent"));
        auto* EditorActor = Find(GEditor->GetEditorWorldContext().World(), TEXT("TextActor"));
        auto* EditorText =
            EditorActor ? FindFProperty<FStrProperty>(EditorActor->GetClass(), TEXT("DSLText")) : nullptr;
        Check(EditorText && EditorText->GetPropertyValue_InContainer(EditorActor) == TEXT("Привет"),
                                                                                          TEXT("text_world_isolation"));
        FString Error;
        UClass* Before = BP->GeneratedClass;
        Check(FDasDSLService::Reimport(BP.Get(), Error) && BP->GeneratedClass == Before,
              TEXT("text_reimport_no_reinstance"));
        CollectGarbage(RF_NoFlags);
        Actor = Find(World, TEXT("TextActor"));
        Check(Actor && Text &&
              Text->GetPropertyValue_InContainer(Actor) == TEXT("Строка 世界 !"), TEXT("text_survives_UE_GC"));
        GEditor->RequestEndPlayMap();
        Ending = true;
        Start = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasTextPIETest, "DasForUE.DSL.TextPIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasTextPIETest::RunTest(const FString& Parameters)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
    {
        AddError(TEXT("Isolated project required"));
        return false;
    }
    auto* World = FAutomationEditorCommonUtils::CreateNewMap();
    UBlueprint* BP = nullptr;
    FString Error;
    const FString Package =
        TEXT("/Game/DasTextProbe/Run_")+ FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT("/BP_Text");
    if (!TestTrue(TEXT("Create text DSL Blueprint"), FDasDSLService::Apply(TEXT("dsl_text_methods.das"), Package,
                                                                                TEXT("text.TextCounter"), BP, Error)))
    {
        AddInfo(Error);
        return false;
    }
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("TextActor");
    if (!TestNotNull(TEXT("Spawn text actor"),
                          World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn)))
        return false;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasTextPIECommand>(BP, this));
    return true;
}


namespace
{
class FDasObjectFramePIECommand final : public IAutomationLatentCommand
{
    TStrongObjectPtr<UBlueprint> BP;
    FAutomationTestBase* Test;
    FString Path, Original;
    double Start = 0;
    bool Ending = false, Passed = true;
    TStrongObjectPtr<UObject> Observer;

public:
    FDasObjectFramePIECommand(UBlueprint* B, FAutomationTestBase* T, FString P, FString Source)
        : BP(B), Test(T), Path(P), Original(Source)
    {
    }
    virtual bool Update() override
    {
        if (Start == 0)
            Start = FPlatformTime::Seconds();
        auto Check = [&](bool V, const TCHAR* N)
        {
            Passed &= Test->TestTrue(N, V);
            UE_LOG(LogTemp, Display, TEXT("DAS_OBJECT_FRAME_PIE %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N);
        };
        auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        if (Ending)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Start < 30)
                return false;
            Check(!GEditor->PlayWorld, TEXT("EndPIE"));
            if (Observer.IsValid())
                R.ReleaseObject(Observer.Get());
            IFileManager::Get().Delete(*Path);
            UE_LOG(LogTemp, Display, TEXT("DAS_OBJECT_FRAME_PIE_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        if (!World || !World->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Start < 60)
                return false;
            Check(false, TEXT("PIE_started"));
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* Receiver = Find(World, TEXT("ObjectReceiver"));
        auto* Target = Find(World, TEXT("ObjectTarget"));
        Check(Receiver && Target, TEXT("real_PIE_object_actors"));
        if (!Receiver || !Target)
        {
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* F = Receiver->FindFunction(TEXT("ActorIdentity"));
        auto InvokeActor = [&](UObject* Input)
        {
            if (!F)
                return false;
            FStructOnScope Memory(F);
            auto* A = FindFProperty<FObjectProperty>(F, TEXT("actor"));
            auto* B = CastField<FObjectProperty>(F->GetReturnProperty());
            if (!A || !B || A->PropertyClass != AActor::StaticClass() || B->PropertyClass != AActor::StaticClass())
                return false;
            A->SetObjectPropertyValue_InContainer(Memory.GetStructMemory(), Input);
            FEditorScriptExecutionGuard Guard;
            Receiver->ProcessEvent(F, Memory.GetStructMemory());
            return B->GetObjectPropertyValue_InContainer(Memory.GetStructMemory()) == Input;
        };
        Check(InvokeActor(Target), TEXT("typed_Actor_input_result_ProcessEvent"));
        Check(InvokeActor(nullptr), TEXT("typed_Actor_null_ProcessEvent"));
        Observer.Reset(NewObject<UDasLifecycleTestObserver>());
        FDasScalarValue O;
        O.Kind = 8;
        O.ObjectValue = Target;
        Check(R.RunNamedFramePersistent(
            Path, TEXT("ue_ObjectBridge_Remember"), {O}, EDasMethodSignature::Void, Observer.Get()).bSucceeded,
            TEXT("observer_stores_weak_target"));
        auto Before = R.RunNamedFramePersistent(Path, TEXT("ue_ObjectBridge_Recall"), {}, EDasMethodSignature::Object,
                                                           Observer.Get());
        Check(Before.bSucceeded && Before.ObjectValue.Get() == Target,
              TEXT("observer_object_return_before_reinstance"));
        O.ObjectValue = nullptr;
        const FString Changed = Original.Replace(TEXT("class ObjectBridge {"),
            TEXT("class ObjectBridge {\n    @ue_property=(EditAnywhere,BlueprintReadWrite)\n    Revision : int = 42\n"));
        Check(FFileHelper::SaveStringToFile(Changed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("write_object_structural_change"));
        FString Error;
        Check(FDasDSLService::Reimport(BP.Get(), Error), TEXT("object_class_reinstance_in_PIE"));
        if (!Error.IsEmpty())
            Test->AddInfo(Error);
        Receiver = Find(World, TEXT("ObjectReceiver"));
        Target = Find(World, TEXT("ObjectTarget"));
        Check(Receiver && Target && Int(Target, TEXT("Revision")) == 42, TEXT("object_instances_updated"));
        auto After = R.RunNamedFramePersistent(Path, TEXT("ue_ObjectBridge_Recall"), {}, EDasMethodSignature::Object,
                                                          Observer.Get());
        Check(After.bSucceeded && After.ObjectValue.Get() == Target, TEXT("weak_handle_follows_Unreal_replacement"));
        if (Receiver)
        {
            F = Receiver->FindFunction(TEXT("ActorIdentity"));
            Check(InvokeActor(Target), TEXT("Actor_result_after_reinstance"));
        }
        Check(Target && Target->Destroy(), TEXT("destroy_target"));
        CollectGarbage(RF_NoFlags);
        const auto Valid = R.RunNamedFramePersistent(Path, TEXT("ue_ObjectBridge_IsRememberedValid"), {},
                                                                EDasMethodSignature::Bool, Observer.Get());
        Check(Valid.bSucceeded && !Valid.BoolValue, TEXT("destroyed_target_handle_invalid"));
        const auto ExpiredResult = R.RunNamedFramePersistent(Path, TEXT("ue_ObjectBridge_Recall"), {},
                                                                        EDasMethodSignature::Object, Observer.Get());
        Check(!ExpiredResult.bSucceeded && ExpiredResult.Error.Contains(TEXT("expired")),
                                                                        TEXT("destroyed_object_return_rejected"));
        GEditor->RequestEndPlayMap();
        Ending = true;
        Start = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasObjectFramePIETest, "DasForUE.DSL.ObjectFramePIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasObjectFramePIETest::RunTest(const FString& Parameters)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
    {
        AddError(TEXT("Isolated project required"));
        return false;
    }
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FString Source;
    FFileHelper::LoadFileToString(Source, *R.ScriptPath(TEXT("dsl_object_methods.das")));
    const FString Guid = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString Script = TEXT("dsl_object_pie_")+ Guid + TEXT(".das"), Path = R.ScriptPath(Script);
    if (!FFileHelper::SaveStringToFile(Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return false;
    auto* World = FAutomationEditorCommonUtils::CreateNewMap();
    UBlueprint* BP = nullptr;
    FString Error;
    const FString Package = TEXT("/Game/DasObjectFrameProbe/Run_")+ Guid + TEXT("/BP_Objects");
    if (!TestTrue(TEXT("Create object DSL Blueprint"),
                       FDasDSLService::Apply(Script, Package, TEXT("objects.ObjectBridge"), BP, Error)))
    {
        AddInfo(Error);
        IFileManager::Get().Delete(*Path);
        return false;
    }
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("ObjectReceiver");
    auto* Receiver = World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
    Spawn.Name = TEXT("ObjectTarget");
    auto* Target = World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
    if (!TestTrue(TEXT("Spawn object actors"), Receiver && Target))
    {
        IFileManager::Get().Delete(*Path);
        return false;
    }
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasObjectFramePIECommand>(BP, this, Path, Source));
    return true;
}


#include "UObject/NoExportTypes.h"
namespace
{
class FDasMathPIECommand final : public IAutomationLatentCommand
{
    TStrongObjectPtr<UBlueprint> BP;
    FAutomationTestBase* Test;
    double Start = 0;
    bool Ending = false, Passed = true;

public:
    FDasMathPIECommand(UBlueprint* B, FAutomationTestBase* T) : BP(B), Test(T) {}
    virtual bool Update() override
    {
        if (Start == 0)
            Start = FPlatformTime::Seconds();
        auto Check = [&](bool V, const TCHAR* N)
        {
            Passed &= Test->TestTrue(N, V);
            UE_LOG(LogTemp, Display, TEXT("DAS_MATH_PIE %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N);
        };
        if (Ending)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Start < 30)
                return false;
            Check(!GEditor->PlayWorld, TEXT("EndPIE"));
            UE_LOG(LogTemp, Display, TEXT("DAS_MATH_PIE_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        if (!World || !World->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Start < 60)
                return false;
            Check(false, TEXT("PIE_started"));
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* Actor = Find(World, TEXT("MathActor"));
        Check(Actor != nullptr, TEXT("real_PIE_math_actor"));
        if (!Actor)
        {
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto Invoke = [&](const TCHAR* Name, auto Input, auto Output)
        {
            auto* F = Actor->FindFunction(Name);
            if (!F)
                return false;
            FStructOnScope Memory(F);
            if (!Input(F, Memory.GetStructMemory()))
                return false;
            FEditorScriptExecutionGuard Guard;
            Actor->ProcessEvent(F, Memory.GetStructMemory());
            return Output(F, Memory.GetStructMemory());
        };
        Check(Invoke(
            TEXT("Shift"),
                 [](UFunction* F, uint8* D)
                 {
                     auto* V = FindFProperty<FStructProperty>(F, TEXT("value"));
                     auto* A = FindFProperty<FDoubleProperty>(F, TEXT("delta"));
                     if (!V || V->Struct != TBaseStructure<FVector>::Get() || !A)
                         return false;
                     *V->ContainerPtrToValuePtr<FVector>(D) = FVector(1, 2.25, 3.5);
                     A->SetPropertyValue_InContainer(D, 4.0);
                     return true;
                 },
                 [](UFunction* F, uint8* D)
                 {
                     auto* V = CastField<FStructProperty>(F->GetReturnProperty());
                     return V && V->Struct == TBaseStructure<FVector>::Get() &&
                            V->ContainerPtrToValuePtr<FVector>(D)->Equals(FVector(5, 2.25, 3.5), 1e-12);
                 }),
            TEXT("native_FVector_ProcessEvent"));
        Check(Invoke(
            TEXT("Turn"),
                 [](UFunction* F, uint8* D)
                 {
                     auto* V = FindFProperty<FStructProperty>(F, TEXT("value"));
                     auto* A = FindFProperty<FDoubleProperty>(F, TEXT("yaw"));
                     if (!V || V->Struct != TBaseStructure<FRotator>::Get() || !A)
                         return false;
                     *V->ContainerPtrToValuePtr<FRotator>(D) = FRotator(12.5, 20.25, 3.75);
                     A->SetPropertyValue_InContainer(D, 5.0);
                     return true;
                 },
                 [](UFunction* F, uint8* D)
                 {
                     auto* V = CastField<FStructProperty>(F->GetReturnProperty());
                     return V && V->Struct == TBaseStructure<FRotator>::Get() &&
                            V->ContainerPtrToValuePtr<FRotator>(D)->Equals(FRotator(12.5, 25.25, 3.75), 1e-12);
                 }),
            TEXT("native_FRotator_ProcessEvent"));
        const FTransform T(FRotator(0, 90, 0), FVector(10, 20, 30), FVector(2, 3, 4));
        Check(Invoke(
            TEXT("PassTransform"),
                 [&](UFunction* F, uint8* D)
                 {
                     auto* V = FindFProperty<FStructProperty>(F, TEXT("value"));
                     if (!V || V->Struct != TBaseStructure<FTransform>::Get())
                         return false;
                     *V->ContainerPtrToValuePtr<FTransform>(D) = T;
                     return true;
                 },
                 [&](UFunction* F, uint8* D)
                 {
                     auto* V = CastField<FStructProperty>(F->GetReturnProperty());
                     return V && V->Struct == TBaseStructure<FTransform>::Get() &&
                            V->ContainerPtrToValuePtr<FTransform>(D)->Equals(T, 1e-12);
                 }),
            TEXT("native_FTransform_ProcessEvent"));
        Check(Invoke(
            TEXT("Apply"),
                 [&](UFunction* F, uint8* D)
                 {
                     auto* A = FindFProperty<FStructProperty>(F, TEXT("value"));
                     auto* B = FindFProperty<FStructProperty>(F, TEXT("position"));
                     if (!A || !B)
                         return false;
                     *A->ContainerPtrToValuePtr<FTransform>(D) = T;
                     *B->ContainerPtrToValuePtr<FVector>(D) = FVector(1, 2, 3);
                     return true;
                 },
                 [&](UFunction* F, uint8* D)
                 {
                     auto* V = CastField<FStructProperty>(F->GetReturnProperty());
                     return V &&
                            V->ContainerPtrToValuePtr<FVector>(D)->Equals(T.TransformPosition(FVector(1, 2, 3)), 1e-12);
                 }),
            TEXT("mixed_math_struct_semantics"));
        FString Error;
        UClass* Before = BP->GeneratedClass;
        Check(FDasDSLService::Reimport(BP.Get(), Error) && BP->GeneratedClass == Before,
              TEXT("math_reimport_no_reinstance"));
        CollectGarbage(RF_NoFlags);
        Check(Find(World, TEXT("MathActor")) != nullptr, TEXT("math_actor_survives_GC"));
        GEditor->RequestEndPlayMap();
        Ending = true;
        Start = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasMathPIETest, "DasForUE.DSL.MathPIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasMathPIETest::RunTest(const FString& Parameters)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
    {
        AddError(TEXT("Isolated project required"));
        return false;
    }
    auto* World = FAutomationEditorCommonUtils::CreateNewMap();
    UBlueprint* BP = nullptr;
    FString Error;
    const FString Package =
        TEXT("/Game/DasMathProbe/Run_")+ FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT("/BP_Math");
    if (!TestTrue(TEXT("Create math DSL Blueprint"), FDasDSLService::Apply(TEXT("dsl_math_methods.das"), Package,
                                                                                TEXT("math.MathBridge"), BP, Error)))
    {
        AddInfo(Error);
        return false;
    }
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("MathActor");
    if (!TestNotNull(TEXT("Spawn math actor"),
                          World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn)))
        return false;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasMathPIECommand>(BP, this));
    return true;
}


namespace
{
class FDasReflectedPIECommand final : public IAutomationLatentCommand
{
    TStrongObjectPtr<UBlueprint> BP;
    FAutomationTestBase* Test;
    double Start = 0;
    bool Ending = false, Passed = true;

public:
    FDasReflectedPIECommand(UBlueprint* B, FAutomationTestBase* T) : BP(B), Test(T) {}
    virtual bool Update() override
    {
        if (Start == 0)
            Start = FPlatformTime::Seconds();
        auto Check = [&](bool V, const TCHAR* N)
        {
            Passed &= Test->TestTrue(N, V);
            UE_LOG(LogTemp, Display, TEXT("DAS_REFLECTED_PIE %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N);
        };
        if (Ending)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Start < 30)
                return false;
            Check(!GEditor->PlayWorld, TEXT("EndPIE"));
            UE_LOG(LogTemp, Display, TEXT("DAS_REFLECTED_PIE_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        if (!World || !World->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Start < 60)
                return false;
            Check(false, TEXT("PIE_started"));
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* Actor = Find(World, TEXT("StructActor"));
        Check(Actor != nullptr, TEXT("real_PIE_struct_actor"));
        if (!Actor)
        {
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* F = Actor->FindFunction(TEXT("Edit"));
        bool OK = false;
        if (F)
        {
            FStructOnScope Frame(F);
            auto* A = FindFProperty<FStructProperty>(F, TEXT("value"));
            auto* Amount = FindFProperty<FIntProperty>(F, TEXT("amount"));
            auto* Result = CastField<FStructProperty>(F->GetReturnProperty());
            if (A && Amount && Result && A->Struct == Result->Struct &&
                A->Struct->GetPathName() == TEXT("/Script/DasRuntime.DasCallPayload"))
            {
                uint8* Input = A->ContainerPtrToValuePtr<uint8>(Frame.GetStructMemory());
                auto* Count = FindFProperty<FIntProperty>(A->Struct, TEXT("Count"));
                auto* Label = FindFProperty<FStrProperty>(A->Struct, TEXT("Label"));
                auto* Metrics = FindFProperty<FStructProperty>(A->Struct, TEXT("Metrics"));
                auto* Ratio = Metrics ? FindFProperty<FFloatProperty>(Metrics->Struct, TEXT("Ratio")) : nullptr;
                if (Count && Label && Metrics && Ratio)
                {
                    Count->SetPropertyValue_InContainer(Input, 7);
                    Label->SetPropertyValue_InContainer(Input, TEXT("Привет 世界"));
                    Ratio->SetPropertyValue_InContainer(Metrics->ContainerPtrToValuePtr<uint8>(Input), 1.25f);
                    Amount->SetPropertyValue_InContainer(Frame.GetStructMemory(), 3);
                    FEditorScriptExecutionGuard Guard;
                    Actor->ProcessEvent(F, Frame.GetStructMemory());
                    uint8* Output = Result->ContainerPtrToValuePtr<uint8>(Frame.GetStructMemory());
                    OK = Count->GetPropertyValue_InContainer(Output) == 10 &&
                         Label->GetPropertyValue_InContainer(Output) ==
                             TEXT("Привет 世界 !") &&
                                  Ratio->GetPropertyValue_InContainer(Metrics->ContainerPtrToValuePtr<uint8>(Output)) ==
                                      1.75f &&
                                  Count->GetPropertyValue_InContainer(Input) == 7;
                }
            }
        }
        Check(OK, TEXT("wildcard_reflected_struct_ProcessEvent_nested_fields"));
        FString Error;
        UClass* Before = BP->GeneratedClass;
        Check(FDasDSLService::Reimport(BP.Get(), Error) && BP->GeneratedClass == Before,
              TEXT("reflected_struct_reimport_no_reinstance"));
        CollectGarbage(RF_NoFlags);
        Check(Find(World, TEXT("StructActor")) != nullptr, TEXT("reflected_actor_survives_GC"));
        GEditor->RequestEndPlayMap();
        Ending = true;
        Start = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasReflectedPIETest, "DasForUE.DSL.ReflectedPIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasReflectedPIETest::RunTest(const FString& Parameters)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
    {
        AddError(TEXT("Isolated project required"));
        return false;
    }
    auto* World = FAutomationEditorCommonUtils::CreateNewMap();
    UBlueprint* BP = nullptr;
    FString Error;
    const FString Package =
        TEXT("/Game/DasReflectedProbe/Run_")+ FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT("/BP_Struct");
    if (!TestTrue(TEXT("Create reflected DSL Blueprint"),
                       FDasDSLService::Apply(TEXT("dsl_reflected_methods.das"), Package,
                                                  TEXT("reflected.StructBridge"), BP, Error)))
    {
        AddInfo(Error);
        return false;
    }
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("StructActor");
    if (!TestNotNull(TEXT("Spawn reflected actor"),
                          World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn)))
        return false;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasReflectedPIECommand>(BP, this));
    return true;
}


namespace
{
class FDasStructEnumPIECommand final : public IAutomationLatentCommand
{
    TStrongObjectPtr<UBlueprint> BP;
    FAutomationTestBase* Test;
    double Start = 0;
    bool Ending = false, Passed = true;

public:
    FDasStructEnumPIECommand(UBlueprint* B, FAutomationTestBase* T) : BP(B), Test(T) {}
    virtual bool Update() override
    {
        if (Start == 0)
            Start = FPlatformTime::Seconds();
        auto Check = [&](bool V, const TCHAR* N)
        {
            Passed &= Test->TestTrue(N, V);
            UE_LOG(LogTemp, Display, TEXT("DAS_STRUCT_ENUM_PIE %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N);
        };
        if (Ending)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Start < 30)
                return false;
            Check(!GEditor->PlayWorld, TEXT("EndPIE"));
            UE_LOG(LogTemp, Display, TEXT("DAS_STRUCT_ENUM_PIE_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        if (!World || !World->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Start < 60)
                return false;
            Check(false, TEXT("PIE_started"));
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* Actor = Find(World, TEXT("EnumStructActor"));
        Check(Actor != nullptr, TEXT("real_PIE_enum_struct_actor"));
        if (!Actor)
        {
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* F = Actor->FindFunction(TEXT("SetMode"));
        bool OK = false;
        if (F)
        {
            FStructOnScope Memory(F);
            auto* Input = FindFProperty<FStructProperty>(F, TEXT("value"));
            auto* Symbol = FindFProperty<FStrProperty>(F, TEXT("symbol"));
            auto* Result = CastField<FStructProperty>(F->GetReturnProperty());
            if (Input && Symbol && Result && Input->Struct == Result->Struct)
            {
                auto* Mode = FindFProperty<FEnumProperty>(Input->Struct, TEXT("Mode"));
                auto* Key = FindFProperty<FNameProperty>(Input->Struct, TEXT("Key"));
                if (Mode && Key)
                {
                    uint8* Data = Input->ContainerPtrToValuePtr<uint8>(Memory.GetStructMemory());
                    Mode->GetUnderlyingProperty()->SetIntPropertyValue(Mode->ContainerPtrToValuePtr<uint8>(Data),
                                                                       int64(0));
                    Key->SetPropertyValue_InContainer(Data, FName(TEXT("Item_42")));
                    Symbol->SetPropertyValue_InContainer(Memory.GetStructMemory(), TEXT("Paused"));
                    FEditorScriptExecutionGuard Guard;
                    Actor->ProcessEvent(F, Memory.GetStructMemory());
                    uint8* Output = Result->ContainerPtrToValuePtr<uint8>(Memory.GetStructMemory());
                    OK = Mode->GetUnderlyingProperty()->GetSignedIntPropertyValue(
                             Mode->ContainerPtrToValuePtr<uint8>(Output)) == 42 &&
                         Key->GetPropertyValue_InContainer(Output) ==
                             FName(TEXT("Item_42")) && Mode->GetUnderlyingProperty()->GetSignedIntPropertyValue(
                                                           Mode->ContainerPtrToValuePtr<uint8>(Data)) == 0;
                }
            }
        }
        Check(OK, TEXT("enum_struct_wildcard_ProcessEvent_immutable_update"));
        FString Error;
        UClass* Before = BP->GeneratedClass;
        Check(FDasDSLService::Reimport(BP.Get(), Error) && BP->GeneratedClass == Before,
              TEXT("enum_struct_reimport_no_reinstance"));
        CollectGarbage(RF_NoFlags);
        Check(Find(World, TEXT("EnumStructActor")) != nullptr, TEXT("enum_struct_actor_survives_GC"));
        GEditor->RequestEndPlayMap();
        Ending = true;
        Start = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasStructEnumPIETest, "DasForUE.DSL.StructEnumPIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasStructEnumPIETest::RunTest(const FString& Parameters)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
    {
        AddError(TEXT("Isolated project required"));
        return false;
    }
    auto* World = FAutomationEditorCommonUtils::CreateNewMap();
    UBlueprint* BP = nullptr;
    FString Error;
    const FString Package =
        TEXT("/Game/DasStructEnumProbe/Run_")+ FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT("/BP_EnumStruct");
    if (!TestTrue(TEXT("Create enum struct DSL Blueprint"),
                       FDasDSLService::Apply(TEXT("dsl_struct_enum_methods.das"), Package,
                                                  TEXT("enumvalues.EnumStructBridge"), BP, Error)))
    {
        AddInfo(Error);
        return false;
    }
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("EnumStructActor");
    if (!TestNotNull(TEXT("Spawn enum struct actor"),
                          World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn)))
        return false;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasStructEnumPIECommand>(BP, this));
    return true;
}

namespace
{
class FDasStructFieldsPIECommand final : public IAutomationLatentCommand
{
    TStrongObjectPtr<UBlueprint> BP;
    FAutomationTestBase* Test;
    double Start = 0;
    bool Ending = false, Passed = true;

public:
    FDasStructFieldsPIECommand(UBlueprint* B, FAutomationTestBase* T) : BP(B), Test(T) {}
    virtual bool Update() override
    {
        if (Start == 0)
            Start = FPlatformTime::Seconds();
        auto Check = [&](bool V, const TCHAR* N)
        {
            Passed &= Test->TestTrue(N, V);
            UE_LOG(LogTemp, Display, TEXT("DAS_STRUCT_FIELDS_PIE %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N);
        };
        if (Ending)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Start < 30)
                return false;
            Check(!GEditor->PlayWorld, TEXT("EndPIE"));
            UE_LOG(LogTemp, Display, TEXT("DAS_STRUCT_FIELDS_PIE_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        if (!World || !World->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Start < 60)
                return false;
            Check(false, TEXT("PIE_started"));
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* Actor = Find(World, TEXT("StructFieldsActor"));
        Check(Actor != nullptr, TEXT("real_PIE_struct_fields_actor"));
        if (Actor)
        {
            auto* P = FindFProperty<FStructProperty>(Actor->GetClass(), TEXT("Payload"));
            auto* Count = P ? FindFProperty<FIntProperty>(P->Struct, TEXT("Count")) : nullptr;
            Check(Count && Count->GetPropertyValue_InContainer(P->ContainerPtrToValuePtr<void>(Actor)) == 10,
                  TEXT("PIE_struct_CDO_default"));
            if (Count)
            {
                Count->SetPropertyValue_InContainer(P->ContainerPtrToValuePtr<void>(Actor), 77);
                auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
                FDasScalarValue Amount;
                Amount.Kind = 0;
                Amount.IntValue = 3;
                const auto Result = Runtime.RunNamedFramePersistent(
                    Runtime.ScriptPath(TEXT("dsl_struct_fields.das")), TEXT("ue_StructFields_Add"), {Amount},
                                                                            EDasMethodSignature::Int, Actor);
                Check(Result.bSucceeded && Result.Value == 80 &&
                          Count->GetPropertyValue_InContainer(P->ContainerPtrToValuePtr<void>(Actor)) == 80,
                      TEXT("PIE_struct_self_read_write"));
                if (!Result.bSucceeded)
                    Test->AddInfo(Result.Error);
                FString Error;
                UClass* Before = BP->GeneratedClass;
                Check(FDasDSLService::Reimport(BP.Get(), Error) && BP->GeneratedClass == Before,
                      TEXT("struct_fields_body_reimport_keeps_class"));
                FDasAuthoringModule Model;
                FDasClassSchema Schema;
                bool Prepared =
                    Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_struct_fields.das")), Model, Error) &&
                                                Model.ToActorSchema(TEXT("structfields.StructFields"), Schema, Error);
                Check(Prepared, TEXT("struct_fields_reinstance_schema"));
                if (Prepared)
                {
                    auto Changed = Schema;
                    Changed.Fields.FindByPredicate([](const auto& F) { return F.Name == TEXT("Payload"); })
                        ->StructDefault = TEXT("(Count=11)");
                    Check(!FDasBlueprintService::Apply(BP.Get(), Changed, Error) && BP->GeneratedClass == Before,
                          TEXT("PIE_default_change_refused_without_mutation"));
                    FDasFieldSchema Extra;
                    Extra.Name = TEXT("AddedProbe");
                    Extra.StableId = TEXT("structfields.StructFields.AddedProbe");
                    Extra.IntDefault = 5;
                    Schema.Fields.Add(Extra);
                    Check(FDasBlueprintService::Apply(BP.Get(), Schema, Error), TEXT("PIE_struct_reinstance"));
                    Actor = Find(World, TEXT("StructFieldsActor"));
                    P = FindFProperty<FStructProperty>(BP->GeneratedClass, TEXT("Payload"));
                    Count = P ? FindFProperty<FIntProperty>(P->Struct, TEXT("Count")) : nullptr;
                    Check(Actor && Count &&
                              Count->GetPropertyValue_InContainer(P->ContainerPtrToValuePtr<void>(Actor)) == 80 &&
                              Count->GetPropertyValue_InContainer(
                                  P->ContainerPtrToValuePtr<void>(BP->GeneratedClass->GetDefaultObject())) == 10,
                          TEXT("PIE_struct_override_and_CDO_preserved"));
                    CollectGarbage(RF_NoFlags);
                    Check(Find(World, TEXT("StructFieldsActor")) != nullptr, TEXT("PIE_struct_actor_survives_GC"));
                }
            }
        }
        GEditor->RequestEndPlayMap();
        Ending = true;
        Start = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasStructFieldsPIETest, "DasForUE.DSL.StructFieldsPIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasStructFieldsPIETest::RunTest(const FString& Parameters)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
    {
        AddError(TEXT("Isolated project required"));
        return false;
    }
    auto* World = FAutomationEditorCommonUtils::CreateNewMap();
    UBlueprint* BP = nullptr;
    FString Error;
    const FString Package =
        TEXT("/Game/DasFrameAssetProbe/Run_")+ FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT("/BP_Fields");
    if (!TestTrue(TEXT("Create struct fields DSL Blueprint"),
                       FDasDSLService::Apply(TEXT("dsl_struct_fields.das"), Package,
                                                  TEXT("structfields.StructFields"), BP, Error)))
    {
        AddInfo(Error);
        return false;
    }
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("StructFieldsActor");
    if (!TestNotNull(TEXT("Spawn struct fields actor"),
                          World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn)))
        return false;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasStructFieldsPIECommand>(BP, this));
    return true;
}

namespace
{
class FDasDefaultMigrationPIECommand final : public IAutomationLatentCommand
{
    FString Path, Source;
    TStrongObjectPtr<UBlueprint> BP;
    FAutomationTestBase* Test;
    double Start = 0;
    bool Ending = false, Passed = true;

public:
    FDasDefaultMigrationPIECommand(UBlueprint* B, FAutomationTestBase* T, FString P, FString S)
        : Path(P), Source(S), BP(B), Test(T)
    {
    }
    virtual bool Update() override
    {
        if (Start == 0)
            Start = FPlatformTime::Seconds();
        auto Check = [&](bool V, const TCHAR* N)
        {
            Passed &= Test->TestTrue(N, V);
            UE_LOG(LogTemp, Display, TEXT("DAS_DEFAULT_MIGRATION_PIE %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N);
        };
        if (Ending)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Start < 30)
                return false;
            Check(!GEditor->PlayWorld, TEXT("EndPIE"));
            IFileManager::Get().Delete(*Path);
            UE_LOG(LogTemp, Display, TEXT("DAS_DEFAULT_MIGRATION_PIE_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        if (!World || !World->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Start < 60)
                return false;
            Check(false, TEXT("PIE_started"));
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* Actor = Find(World, TEXT("StructFieldsActor"));
        Check(Actor != nullptr, TEXT("real_PIE_struct_fields_actor"));
        if (Actor)
        {
            auto* P = FindFProperty<FStructProperty>(Actor->GetClass(), TEXT("Payload"));
            auto* Count = P ? FindFProperty<FIntProperty>(P->Struct, TEXT("Count")) : nullptr;
            Check(Count && Count->GetPropertyValue_InContainer(P->ContainerPtrToValuePtr<void>(Actor)) == 10,
                  TEXT("PIE_struct_CDO_default"));
            if (Count)
            {
                Count->SetPropertyValue_InContainer(P->ContainerPtrToValuePtr<void>(Actor), 77);
                auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
                FDasScalarValue Amount;
                Amount.Kind = 0;
                Amount.IntValue = 3;
                const auto Result = Runtime.RunNamedFramePersistent(Path, TEXT("ue_StructFields_Add"), {Amount},
                                                                               EDasMethodSignature::Int, Actor);
                Check(Result.bSucceeded && Result.Value == 80 &&
                          Count->GetPropertyValue_InContainer(P->ContainerPtrToValuePtr<void>(Actor)) == 80,
                      TEXT("PIE_struct_self_read_write"));
                if (!Result.bSucceeded)
                    Test->AddInfo(Result.Error);
                FString Error;
                UClass* Before = BP->GeneratedClass;
                Check(FDasDSLService::Reimport(BP.Get(), Error) && BP->GeneratedClass == Before,
                      TEXT("struct_fields_body_reimport_keeps_class"));
                auto* Untouched = World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity);
                const FName UntouchedName = Untouched->GetFName();
                const FString MigratedSource = Source.Replace(TEXT("Count=10"), TEXT("Count=11"));
                Check(FFileHelper::SaveStringToFile(MigratedSource, *Path,
                                                    FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                      TEXT("PIE_write_new_default"));
                // The automatic merge is allowed during PIE as well (DasForUE.ActorComponent.DeclarationReimportPIE);
                // here the explicit policy rewrites the CDO without reinstancing.
                Check(FDasDSLService::Reimport(BP.Get(), Error, EDasDefaultMigrationPolicy::PreserveInstances) &&
                          BP->GeneratedClass == Before,
                      TEXT("PIE_explicit_default_migration_without_reinstance"));
                if (!Error.IsEmpty())
                    Test->AddInfo(Error);
                Check(Count->GetPropertyValue_InContainer(P->ContainerPtrToValuePtr<void>(Actor)) == 80 &&
                          Count->GetPropertyValue_InContainer(P->ContainerPtrToValuePtr<void>(Untouched)) == 10 &&
                          Count->GetPropertyValue_InContainer(
                              P->ContainerPtrToValuePtr<void>(BP->GeneratedClass->GetDefaultObject())) == 11,
                      TEXT("PIE_existing_values_preserved_new_CDO"));
                auto* Fresh = World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity);
                Check(Fresh && Count->GetPropertyValue_InContainer(P->ContainerPtrToValuePtr<void>(Fresh)) == 11,
                      TEXT("PIE_new_actor_uses_migrated_default"));
                const auto AfterMigration = Runtime.RunNamedFramePersistent(Path, TEXT("ue_StructFields_Add"), {Amount},
                                                                                       EDasMethodSignature::Int, Actor);
                Check(AfterMigration.bSucceeded && AfterMigration.Value == 83,
                      TEXT("PIE_script_promotes_after_default_migration"));
                FDasAuthoringModule Model;
                FDasClassSchema Schema;
                bool Prepared = Runtime.ReadAuthoringScript(Path, Model, Error) &&
                                Model.ToActorSchema(TEXT("structfields.StructFields"), Schema, Error);
                Check(Prepared, TEXT("struct_fields_reinstance_schema"));
                if (Prepared)
                {
                    auto Changed = Schema;
                    Changed.Fields.FindByPredicate([](const auto& F) { return F.Name == TEXT("Payload"); })
                        ->StructDefault = TEXT("(Count=12)");
                    Check(!FDasBlueprintService::Apply(BP.Get(), Changed, Error) && BP->GeneratedClass == Before,
                          TEXT("PIE_default_change_refused_without_mutation"));
                    FDasFieldSchema Extra;
                    Extra.Name = TEXT("AddedProbe");
                    Extra.StableId = TEXT("structfields.StructFields.AddedProbe");
                    Extra.IntDefault = 5;
                    Schema.Fields.Add(Extra);
                    Check(FDasBlueprintService::Apply(BP.Get(), Schema, Error), TEXT("PIE_struct_reinstance"));
                    Actor = Find(World, TEXT("StructFieldsActor"));
                    P = FindFProperty<FStructProperty>(BP->GeneratedClass, TEXT("Payload"));
                    Count = P ? FindFProperty<FIntProperty>(P->Struct, TEXT("Count")) : nullptr;
                    Check(Actor && Count &&
                              Count->GetPropertyValue_InContainer(P->ContainerPtrToValuePtr<void>(Actor)) == 83 &&
                              Count->GetPropertyValue_InContainer(
                                  P->ContainerPtrToValuePtr<void>(BP->GeneratedClass->GetDefaultObject())) == 11,
                          TEXT("PIE_struct_override_and_CDO_preserved"));
                    Untouched = nullptr;
                    for (TActorIterator<ADasProbeActor> It(World); It; ++It)
                        if (It->GetFName() == UntouchedName)
                            Untouched = *It;
                    Check(Untouched && Count &&
                              Count->GetPropertyValue_InContainer(P->ContainerPtrToValuePtr<void>(Untouched)) == 10,
                          TEXT("PIE_unmodified_old_actor_retains_value_after_later_reinstance"));
                    CollectGarbage(RF_NoFlags);
                    Check(Find(World, TEXT("StructFieldsActor")) != nullptr, TEXT("PIE_struct_actor_survives_GC"));
                }
            }
        }
        GEditor->RequestEndPlayMap();
        Ending = true;
        Start = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasDefaultMigrationPIETest, "DasForUE.DSL.DefaultMigrationPIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasDefaultMigrationPIETest::RunTest(const FString& Parameters)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
    {
        AddError(TEXT("Isolated project required"));
        return false;
    }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FString Source;
    FFileHelper::LoadFileToString(Source, *Runtime.ScriptPath(TEXT("dsl_struct_fields.das")));
    const FString Script = TEXT("dsl_migration_pie_")+ FGuid::NewGuid().ToString(EGuidFormats::Digits) +
                                TEXT(".das"), Path = Runtime.ScriptPath(Script);
    if (!FFileHelper::SaveStringToFile(Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return false;
    auto* World = FAutomationEditorCommonUtils::CreateNewMap();
    UBlueprint* BP = nullptr;
    FString Error;
    const FString Package =
        TEXT("/Game/DasFrameAssetProbe/Run_")+ FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT("/BP_Fields");
    if (!TestTrue(TEXT("Create struct fields DSL Blueprint"),
                       FDasDSLService::Apply(Script, Package, TEXT("structfields.StructFields"), BP, Error)))
    {
        AddInfo(Error);
        return false;
    }
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("StructFieldsActor");
    if (!TestNotNull(TEXT("Spawn struct fields actor"),
                          World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn)))
        return false;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(
        MakeShared<FDasDefaultMigrationPIECommand>(BP, this, Path, Source));
    return true;
}

namespace
{
class FDasFieldRenamePIECommand final : public IAutomationLatentCommand
{
    FString Path, Source;
    TStrongObjectPtr<UBlueprint> BP;
    FAutomationTestBase* Test;
    double Start = 0;
    bool Ending = false, Passed = true;

public:
    FDasFieldRenamePIECommand(UBlueprint* B, FAutomationTestBase* T, FString P, FString S)
        : Path(P), Source(S), BP(B), Test(T)
    {
    }
    virtual bool Update() override
    {
        if (Start == 0)
            Start = FPlatformTime::Seconds();
        auto Check = [&](bool V, const TCHAR* N)
        {
            Passed &= Test->TestTrue(N, V);
            UE_LOG(LogTemp, Display, TEXT("DAS_FIELD_RENAME_PIE %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N);
        };
        if (Ending)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Start < 30)
                return false;
            Check(!GEditor->PlayWorld, TEXT("EndPIE"));
            IFileManager::Get().Delete(*Path);
            UE_LOG(LogTemp, Display, TEXT("DAS_FIELD_RENAME_PIE_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        if (!World || !World->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Start < 60)
                return false;
            Check(false, TEXT("PIE_started"));
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* Actor = Find(World, TEXT("RenameActor"));
        auto* Untouched = Find(World, TEXT("RenameUntouched"));
        Check(Actor && Untouched, TEXT("real_PIE_rename_actors"));
        if (Actor && Untouched)
        {
            auto* Score = FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("Score"));
            auto* Payload = FindFProperty<FStructProperty>(Actor->GetClass(), TEXT("Payload"));
            if (Score && Payload)
            {
                Score->SetPropertyValue_InContainer(Actor, 77);
                auto* Count = FindFProperty<FIntProperty>(Payload->Struct, TEXT("Count"));
                Count->SetPropertyValue_InContainer(Payload->ContainerPtrToValuePtr<void>(Actor), 88);
                auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
                const auto BeforeCall = Runtime.RunNamedPersistent(Path, TEXT("ue_RenameFields_Add"), 0, Actor);
                Check(BeforeCall.bSucceeded && BeforeCall.Value == 165, TEXT("PIE_persistent_context_before_rename"));
                const FString Changed = Source.Replace(
                    TEXT("Score :"),
                         TEXT("Total :"))
                             .Replace(TEXT("self.Score"),
                                 TEXT("self.Total"))
                                     .Replace(TEXT("Payload :"),
                                                   TEXT("Data :")).Replace(TEXT("self.Payload"), TEXT("self.Data"));
                FString Error;
                UClass* Before = BP->GeneratedClass;
                Check(FFileHelper::SaveStringToFile(Changed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                      TEXT("PIE_write_rename"));
                Check(!FDasDSLService::Reimport(BP.Get(), Error) && BP->GeneratedClass == Before,
                      TEXT("PIE_no_policy_rename_refused"));
                Check(FDasDSLService::ReimportRenamedFields(BP.Get(), Error), TEXT("PIE_rename_by_ID"));
                if (!Error.IsEmpty())
                    Test->AddInfo(Error);
                Actor = Find(World, TEXT("RenameActor"));
                Untouched = Find(World, TEXT("RenameUntouched"));
                Score = FindFProperty<FIntProperty>(BP->GeneratedClass, TEXT("Total"));
                Payload = FindFProperty<FStructProperty>(BP->GeneratedClass, TEXT("Data"));
                Count = Payload ? FindFProperty<FIntProperty>(Payload->Struct, TEXT("Count")) : nullptr;
                Check(Actor && Untouched && Score && Count && Score->GetPropertyValue_InContainer(Actor) == 77 &&
                          Count->GetPropertyValue_InContainer(Payload->ContainerPtrToValuePtr<void>(Actor)) == 88 &&
                          Score->GetPropertyValue_InContainer(Untouched) == 5 &&
                          Count->GetPropertyValue_InContainer(Payload->ContainerPtrToValuePtr<void>(Untouched)) == 10,
                      TEXT("PIE_rename_preserves_overrides_and_defaults"));
                const auto AfterCall = Runtime.RunNamedPersistent(Path, TEXT("ue_RenameFields_Add"), 3, Actor);
                Check(AfterCall.bSucceeded && AfterCall.Value == 171, TEXT("PIE_updated_self_method_after_reinstance"));
                if (!AfterCall.bSucceeded)
                    Test->AddInfo(AfterCall.Error);
                auto* Fresh = World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity);
                Check(Fresh && Score && Score->GetPropertyValue_InContainer(Fresh) == 5 && Count &&
                          Count->GetPropertyValue_InContainer(Payload->ContainerPtrToValuePtr<void>(Fresh)) == 10,
                      TEXT("PIE_new_actor_defaults_after_rename"));
                Check(FDasDSLService::ReimportRenamedFields(BP.Get(), Error), TEXT("PIE_rename_idempotent"));
                CollectGarbage(RF_NoFlags);
                Check(Find(World, TEXT("RenameActor")) != nullptr, TEXT("PIE_renamed_actor_survives_GC"));
            }
            else
                Check(false, TEXT("PIE_fixture_fields"));
        }
        GEditor->RequestEndPlayMap();
        Ending = true;
        Start = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasFieldRenamePIETest, "DasForUE.DSL.FieldRenamePIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasFieldRenamePIETest::RunTest(const FString& Parameters)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
    {
        AddError(TEXT("Isolated project required"));
        return false;
    }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FString Source;
    FFileHelper::LoadFileToString(Source, *Runtime.ScriptPath(TEXT("dsl_rename_fields.das")));
    const FString Guid = FGuid::NewGuid().ToString(EGuidFormats::Digits),
                  Script = TEXT("dsl_renamefields_pie_")+ Guid + TEXT(".das"), Path = Runtime.ScriptPath(Script);
    if (!FFileHelper::SaveStringToFile(Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return false;
    auto* World = FAutomationEditorCommonUtils::CreateNewMap();
    UBlueprint* BP = nullptr;
    FString Error;
    const FString Package = TEXT("/Game/DasFrameAssetProbe/Run_")+ Guid + TEXT("/BP_Rename");
    if (!TestTrue(TEXT("Create rename DSL Blueprint"),
                       FDasDSLService::Apply(Script, Package, TEXT("rename.RenameFields"), BP, Error)))
    {
        AddInfo(Error);
        IFileManager::Get().Delete(*Path);
        return false;
    }
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("RenameActor");
    auto* Actor = World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
    Spawn.Name = TEXT("RenameUntouched");
    auto* Untouched = World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
    if (!TestTrue(TEXT("Spawn rename actors"), Actor && Untouched))
    {
        IFileManager::Get().Delete(*Path);
        return false;
    }
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasFieldRenamePIECommand>(BP, this, Path, Source));
    return true;
}

namespace
{
class FDasRenameScopePIECommand final : public IAutomationLatentCommand
{
    FString Path, Source;
    TStrongObjectPtr<UBlueprint> BP;
    FAutomationTestBase* Test;
    double Start = 0;
    bool Ending = false, Passed = true;

public:
    FDasRenameScopePIECommand(UBlueprint* B, FAutomationTestBase* T, FString P, FString S)
        : Path(P), Source(S), BP(B), Test(T)
    {
    }
    virtual bool Update() override
    {
        if (Start == 0)
            Start = FPlatformTime::Seconds();
        auto Check = [&](bool V, const TCHAR* N)
        {
            Passed &= Test->TestTrue(N, V);
            UE_LOG(LogTemp, Display, TEXT("DAS_RENAME_SCOPE_PIE %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N);
        };
        if (Ending)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Start < 30)
                return false;
            Check(!GEditor->PlayWorld, TEXT("EndPIE"));
            IFileManager::Get().Delete(*Path);
            UE_LOG(LogTemp, Display, TEXT("DAS_RENAME_SCOPE_PIE_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        if (!World || !World->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Start < 60)
                return false;
            Check(false, TEXT("PIE_started"));
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* Actor = Find(World, TEXT("RenameActor"));
        auto* Untouched = Find(World, TEXT("RenameUntouched"));
        Check(Actor && Untouched, TEXT("real_PIE_rename_actors"));
        if (Actor && Untouched)
        {
            auto* Score = FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("Score"));
            auto* Payload = FindFProperty<FStructProperty>(Actor->GetClass(), TEXT("Payload"));
            if (Score && Payload)
            {
                Score->SetPropertyValue_InContainer(Actor, 77);
                auto* Count = FindFProperty<FIntProperty>(Payload->Struct, TEXT("Count"));
                Count->SetPropertyValue_InContainer(Payload->ContainerPtrToValuePtr<void>(Actor), 88);
                auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
                const auto BeforeCall = Runtime.RunNamedPersistent(Path, TEXT("ue_RenameFields_Add"), 0, Actor);
                Check(BeforeCall.bSucceeded && BeforeCall.Value == 165, TEXT("PIE_persistent_context_before_rename"));
                const FString Changed = Source.Replace(
                    TEXT("Score :"),
                         TEXT("Total :"))
                             .Replace(TEXT("self.Score"),
                                 TEXT("self.Total"))
                                     .Replace(TEXT("Payload :"),
                                                   TEXT("Data :")).Replace(TEXT("self.Payload"), TEXT("self.Data"));
                FString Error;
                UClass* Before = BP->GeneratedClass;
                Check(FFileHelper::SaveStringToFile(Changed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                      TEXT("PIE_write_rename"));
                Check(!FDasDSLService::Reimport(BP.Get(), Error) && BP->GeneratedClass == Before,
                      TEXT("PIE_no_policy_rename_refused"));
                Check(FDasDSLService::ReimportRenamedFields(BP.Get(), Error, true), TEXT("PIE_rename_by_ID"));
                if (!Error.IsEmpty())
                    Test->AddInfo(Error);
                Actor = Find(World, TEXT("RenameActor"));
                Untouched = Find(World, TEXT("RenameUntouched"));
                Score = FindFProperty<FIntProperty>(BP->GeneratedClass, TEXT("Total"));
                Payload = FindFProperty<FStructProperty>(BP->GeneratedClass, TEXT("Data"));
                Count = Payload ? FindFProperty<FIntProperty>(Payload->Struct, TEXT("Count")) : nullptr;
                Check(Actor && Untouched && Score && Count && Score->GetPropertyValue_InContainer(Actor) == 77 &&
                          Count->GetPropertyValue_InContainer(Payload->ContainerPtrToValuePtr<void>(Actor)) == 88 &&
                          Score->GetPropertyValue_InContainer(Untouched) == 45 &&
                          Count->GetPropertyValue_InContainer(Payload->ContainerPtrToValuePtr<void>(Untouched)) == 56,
                      TEXT("PIE_rename_preserves_overrides_and_defaults"));
                const auto AfterCall = Runtime.RunNamedPersistent(Path, TEXT("ue_RenameFields_Add"), 3, Actor);
                Check(AfterCall.bSucceeded && AfterCall.Value == 171, TEXT("PIE_updated_self_method_after_reinstance"));
                if (!AfterCall.bSucceeded)
                    Test->AddInfo(AfterCall.Error);
                auto* Fresh = World->SpawnActor<ADasProbeActor>(Actor->GetClass(), FTransform::Identity);
                Check(Fresh && Score && Score->GetPropertyValue_InContainer(Fresh) == 45 && Count &&
                          Count->GetPropertyValue_InContainer(Payload->ContainerPtrToValuePtr<void>(Fresh)) == 56,
                      TEXT("PIE_new_actor_defaults_after_rename"));
                Check(FFileHelper::SaveStringToFile(Changed.Replace(TEXT("Count=10"), TEXT("Count=11")), *Path,
                                                                    FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                                                    TEXT("PIE_write_unapplied_parent_default"));
                const auto InvalidParentDefault =
                    Runtime.RunNamedPersistent(Path, TEXT("ue_RenameFields_Add"), 0, Fresh);
                Check(!InvalidParentDefault.bSucceeded &&
                      InvalidParentDefault.Error.Contains(
                          TEXT("default")), TEXT("PIE_child_override_does_not_bypass_declaring_default_guard"));
                Check(FFileHelper::SaveStringToFile(Changed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                      TEXT("PIE_restore_parent_declaration"));
                Check(FDasDSLService::ReimportRenamedFields(BP.Get(), Error, true), TEXT("PIE_rename_idempotent"));
                CollectGarbage(RF_NoFlags);
                Check(Find(World, TEXT("RenameActor")) != nullptr, TEXT("PIE_renamed_actor_survives_GC"));
            }
            else
                Check(false, TEXT("PIE_fixture_fields"));
        }
        GEditor->RequestEndPlayMap();
        Ending = true;
        Start = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasRenameScopePIETest, "DasForUE.DSL.RenameScopePIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasRenameScopePIETest::RunTest(const FString& Parameters)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
    {
        AddError(TEXT("Isolated project required"));
        return false;
    }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FString Source;
    FFileHelper::LoadFileToString(Source, *Runtime.ScriptPath(TEXT("dsl_rename_fields.das")));
    const FString Guid = FGuid::NewGuid().ToString(EGuidFormats::Digits),
                  Script = TEXT("dsl_renamescope_pie_")+ Guid + TEXT(".das"), Path = Runtime.ScriptPath(Script);
    if (!FFileHelper::SaveStringToFile(Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return false;
    auto* World = FAutomationEditorCommonUtils::CreateNewMap();
    UBlueprint* BP = nullptr;
    FString Error;
    const FString Package = TEXT("/Game/DasFrameAssetProbe/Run_")+ Guid + TEXT("/BP_Rename");
    if (!TestTrue(TEXT("Create rename DSL Blueprint"),
                       FDasDSLService::Apply(Script, Package, TEXT("rename.RenameFields"), BP, Error)))
    {
        AddInfo(Error);
        IFileManager::Get().Delete(*Path);
        return false;
    }
    auto* Child = FKismetEditorUtilities::CreateBlueprint(
        BP->GeneratedClass, CreatePackage(*(Package + TEXT("_Child"))),
                                          TEXT("BP_Child"), BPTYPE_Normal, UBlueprint::StaticClass(),
                                               UBlueprintGeneratedClass::StaticClass(), TEXT("DasRenameScopePIE"));
    auto SetDefaults = [](UBlueprint* B, int32 ScoreValue, int32 CountValue)
    {
        UObject* CDO = B->GeneratedClass->GetDefaultObject();
        FindFProperty<FIntProperty>(B->GeneratedClass, TEXT("Score"))->SetPropertyValue_InContainer(CDO, ScoreValue);
        auto* P = FindFProperty<FStructProperty>(B->GeneratedClass, TEXT("Payload"));
        FindFProperty<FIntProperty>(
            P->Struct, TEXT("Count"))->SetPropertyValue_InContainer(P->ContainerPtrToValuePtr<void>(CDO), CountValue);
    };
    SetDefaults(Child, 23, 34);
    auto* Grand = FKismetEditorUtilities::CreateBlueprint(
        Child->GeneratedClass, CreatePackage(*(Package + TEXT("_Grand"))),
                                             TEXT("BP_Grand"), BPTYPE_Normal, UBlueprint::StaticClass(),
                                                  UBlueprintGeneratedClass::StaticClass(), TEXT("DasRenameScopePIE"));
    SetDefaults(Grand, 45, 56);
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("RenameActor");
    auto* Actor = World->SpawnActor<ADasProbeActor>(Grand->GeneratedClass, FTransform::Identity, Spawn);
    Spawn.Name = TEXT("RenameUntouched");
    auto* Untouched = World->SpawnActor<ADasProbeActor>(Grand->GeneratedClass, FTransform::Identity, Spawn);
    if (!TestTrue(TEXT("Spawn rename actors"), Actor && Untouched))
    {
        IFileManager::Get().Delete(*Path);
        return false;
    }
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasRenameScopePIECommand>(BP, this, Path, Source));
    return true;
}

#if WITH_DEV_AUTOMATION_TESTS
void DasFailWidenAfterCompileForTesting(bool Enabled);
#endif
namespace
{
class FDasNumericWidenPIECommand final : public IAutomationLatentCommand
{
    FString Path, Source;
    TStrongObjectPtr<UBlueprint> BP;
    FAutomationTestBase* Test;
    double Start = 0;
    bool Ending = false, Passed = true;

public:
    FDasNumericWidenPIECommand(UBlueprint* B, FAutomationTestBase* T, FString P, FString S)
        : Path(P), Source(S), BP(B), Test(T)
    {
    }
    virtual bool Update() override
    {
        if (Start == 0)
            Start = FPlatformTime::Seconds();
        auto Check = [&](bool V, const TCHAR* N)
        {
            Passed &= Test->TestTrue(N, V);
            UE_LOG(LogTemp, Display, TEXT("DAS_NUMERIC_WIDEN_PIE %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N);
        };
        if (Ending)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Start < 30)
                return false;
            Check(!GEditor->PlayWorld, TEXT("EndPIE"));
            IFileManager::Get().Delete(*Path);
            UE_LOG(LogTemp, Display, TEXT("DAS_NUMERIC_WIDEN_PIE_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        if (!World || !World->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Start < 60)
                return false;
            Check(false, TEXT("PIE_started"));
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* Actor = Find(World, TEXT("WidenActor"));
        auto* Untouched = Find(World, TEXT("WidenUntouched"));
        Check(Actor && Untouched, TEXT("real_PIE_widen_actors"));
        if (Actor && Untouched)
        {
            auto* Score = FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("Score"));
            auto* Rate = FindFProperty<FFloatProperty>(Actor->GetClass(), TEXT("Rate"));
            Score->SetPropertyValue_InContainer(Actor, MAX_int32);
            Rate->SetPropertyValue_InContainer(Actor, 0.1f);
            auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
            const auto BeforeCall = Runtime.RunNamedFramePersistent(Path, TEXT("ue_WidenFields_ReadScore"), {},
                                                                               EDasMethodSignature::Double, Actor);
            Check(BeforeCall.bSucceeded && BeforeCall.DoubleValue == MAX_int32, TEXT("PIE_old_numeric_callback"));
            const FString Changed = Source.Replace(
                TEXT("Score : int = 5"),
                     TEXT("Score : double = 5.0lf")).Replace(TEXT("Rate : float = 1.5"), TEXT("Rate : double = 1.5lf"));
            FString Error;
            Check(FFileHelper::SaveStringToFile(Changed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                  TEXT("PIE_write_double_fields"));
#if WITH_DEV_AUTOMATION_TESTS
            DasFailWidenAfterCompileForTesting(true);
            const bool RolledBack = !FDasDSLService::ReimportWidenedNumericFields(BP.Get(), Error, true);
            DasFailWidenAfterCompileForTesting(false);
            Actor = Find(World, TEXT("WidenActor"));
            Check(RolledBack &&
                  Error.StartsWith(
                      TEXT("Numeric migration rolled back:")) && Actor &&
                      FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("Score")) &&
                                                  FindFProperty<FIntProperty>(
                                                      Actor->GetClass(),
                                                      TEXT("Score"))->GetPropertyValue_InContainer(Actor) == MAX_int32,
                                                      TEXT("PIE_type_and_override_rollback"));
            const auto LastGood = Runtime.RunNamedFramePersistent(Path, TEXT("ue_WidenFields_ReadScore"), {},
                                                                             EDasMethodSignature::Double, Actor);
            Check(LastGood.bSucceeded && LastGood.DoubleValue == MAX_int32,
                  TEXT("PIE_last_good_callback_after_rollback"));
            Test->AddInfo(FString::Printf(TEXT("Rollback callback: success=%d value=%.17g error=%s"),
                                               LastGood.bSucceeded, LastGood.DoubleValue, *LastGood.Error));
#endif
            Check(FDasDSLService::ReimportWidenedNumericFields(BP.Get(), Error, true), TEXT("PIE_exact_numeric_widen"));
            if (!Error.IsEmpty())
                Test->AddInfo(Error);
            Actor = Find(World, TEXT("WidenActor"));
            Untouched = Find(World, TEXT("WidenUntouched"));
            auto* D = Actor ? FindFProperty<FDoubleProperty>(Actor->GetClass(), TEXT("Score")) : nullptr;
            auto* R = Actor ? FindFProperty<FDoubleProperty>(Actor->GetClass(), TEXT("Rate")) : nullptr;
            Check(Actor && Untouched && D && R && D->GetPropertyValue_InContainer(Actor) == MAX_int32 &&
                      R->GetPropertyValue_InContainer(Actor) == static_cast<double>(0.1f) &&
                      D->GetPropertyValue_InContainer(Untouched) == 23 &&
                      R->GetPropertyValue_InContainer(Untouched) == 2.25,
                  TEXT("PIE_exact_override_and_child_default_values"));
            const auto AfterCall = Runtime.RunNamedFramePersistent(Path, TEXT("ue_WidenFields_ReadRate"), {},
                                                                              EDasMethodSignature::Double, Actor);
            Check(AfterCall.bSucceeded && AfterCall.DoubleValue == static_cast<double>(0.1f),
                  TEXT("PIE_callback_uses_new_double_field"));
            auto* Fresh = World->SpawnActor<ADasProbeActor>(Actor->GetClass(), FTransform::Identity);
            Check(Fresh && D && R && D->GetPropertyValue_InContainer(Fresh) == 23 &&
                      R->GetPropertyValue_InContainer(Fresh) == 2.25,
                  TEXT("PIE_fresh_child_uses_promoted_defaults"));
            Check(FFileHelper::SaveStringToFile(
                TEXT("options gen2\nrequire ue_foundation\n[export]\ndef ue_WidenFields_ReadScore():double { return 12.0lf }\n[export]\ndef ue_WidenFields_ReadRate():double { return 13.0lf }\n"),
                    *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                TEXT("PIE_write_removed_DSL_declaration"));
            const auto RemovedDSL = Runtime.RunNamedFramePersistent(Path, TEXT("ue_WidenFields_ReadScore"), {},
                                                                               EDasMethodSignature::Double, Actor);
            Check(RemovedDSL.bSucceeded && RemovedDSL.DoubleValue == MAX_int32 &&
                  RemovedDSL.ReloadError.Contains(TEXT("DSL declaration changed")),
                                                  TEXT("PIE_missing_declaration_does_not_bypass_approval_guard"));
            Check(FFileHelper::SaveStringToFile(Changed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                  TEXT("PIE_restore_promoted_source"));
            Check(FDasDSLService::ReimportWidenedNumericFields(BP.Get(), Error, true), TEXT("PIE_widen_idempotent"));
            CollectGarbage(RF_NoFlags);
            Check(Find(World, TEXT("WidenActor")) != nullptr, TEXT("PIE_promoted_actor_survives_GC"));
        }
        GEditor->RequestEndPlayMap();
        Ending = true;
        Start = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasNumericWidenPIETest, "DasForUE.DSL.NumericWidenPIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasNumericWidenPIETest::RunTest(const FString& Parameters)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
    {
        AddError(TEXT("Isolated project required"));
        return false;
    }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FString Source;
    FFileHelper::LoadFileToString(Source, *Runtime.ScriptPath(TEXT("dsl_widen_numeric.das")));
    const FString Guid = FGuid::NewGuid().ToString(EGuidFormats::Digits),
                  Script = TEXT("dsl_widen_pie_")+ Guid + TEXT(".das"), Path = Runtime.ScriptPath(Script);
    if (!FFileHelper::SaveStringToFile(Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return false;
    auto* World = FAutomationEditorCommonUtils::CreateNewMap();
    UBlueprint* BP = nullptr;
    FString Error;
    const FString Package = TEXT("/Game/DasFrameAssetProbe/WidenPIE_")+ Guid + TEXT("/BP_Parent");
    if (!TestTrue(TEXT("Create widening Blueprint"),
                       FDasDSLService::Apply(Script, Package, TEXT("widen.WidenFields"), BP, Error)))
    {
        AddInfo(Error);
        IFileManager::Get().Delete(*Path);
        return false;
    }
    auto* Child = FKismetEditorUtilities::CreateBlueprint(
        BP->GeneratedClass, CreatePackage(*(Package + TEXT("_Child"))),
                                          TEXT("BP_Child"), BPTYPE_Normal, UBlueprint::StaticClass(),
                                               UBlueprintGeneratedClass::StaticClass(), TEXT("DasWidenPIE"));
    UObject* CDO = Child->GeneratedClass->GetDefaultObject();
    FindFProperty<FIntProperty>(Child->GeneratedClass, TEXT("Score"))->SetPropertyValue_InContainer(CDO, 23);
    FindFProperty<FFloatProperty>(Child->GeneratedClass, TEXT("Rate"))->SetPropertyValue_InContainer(CDO, 2.25f);
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("WidenActor");
    auto* Actor = World->SpawnActor<ADasProbeActor>(Child->GeneratedClass, FTransform::Identity, Spawn);
    Spawn.Name = TEXT("WidenUntouched");
    auto* Untouched = World->SpawnActor<ADasProbeActor>(Child->GeneratedClass, FTransform::Identity, Spawn);
    if (!TestTrue(TEXT("Spawn widening actors"), Actor && Untouched))
    {
        IFileManager::Get().Delete(*Path);
        return false;
    }
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(
        MakeShared<FDasNumericWidenPIECommand>(BP, this, Path, Source));
    return true;
}

#include "Blueprint/BlueprintExceptionInfo.h"
#include "UObject/Script.h"
void DasCreateNumericGraphFixture(UBlueprint* B);
int32 DasReadNumericGraphFixture(ADasProbeActor* A);
int32 DasNumericGraphAdapterCount(UBlueprint* B);
namespace
{
class FDasNumericConvertPIECommand final : public IAutomationLatentCommand
{
    FString Path, Source;
    TStrongObjectPtr<UBlueprint> BP;
    FAutomationTestBase* Test;
    double Start = 0;
    bool Ending = false, Passed = true;

public:
    FDasNumericConvertPIECommand(UBlueprint* B, FAutomationTestBase* T, FString P, FString S)
        : Path(P), Source(S), BP(B), Test(T)
    {
    }
    virtual bool Update() override
    {
        if (Start == 0)
            Start = FPlatformTime::Seconds();
        auto Check = [&](bool V, const TCHAR* N)
        {
            Passed &= Test->TestTrue(N, V);
            UE_LOG(LogTemp, Display, TEXT("DAS_NUMERIC_CONVERT_PIE %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N);
        };
        if (Ending)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Start < 30)
                return false;
            Check(!GEditor->PlayWorld, TEXT("EndPIE"));
            IFileManager::Get().Delete(*Path);
            UE_LOG(LogTemp, Display, TEXT("DAS_NUMERIC_CONVERT_PIE_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        if (!World || !World->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Start < 60)
                return false;
            Check(false, TEXT("PIE_started"));
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* Actor = Find(World, TEXT("WidenActor"));
        auto* Untouched = Find(World, TEXT("WidenUntouched"));
        Check(Actor && Untouched, TEXT("real_PIE_widen_actors"));
        if (Actor && Untouched)
        {
            auto* Score = FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("Score"));
            auto* Rate = FindFProperty<FFloatProperty>(Actor->GetClass(), TEXT("Rate"));
            Score->SetPropertyValue_InContainer(Actor, MAX_int32);
            Rate->SetPropertyValue_InContainer(Actor, 0.1f);
            auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
            const auto BeforeCall = Runtime.RunNamedFramePersistent(Path, TEXT("ue_WidenFields_ReadScore"), {},
                                                                               EDasMethodSignature::Double, Actor);
            Check(BeforeCall.bSucceeded && BeforeCall.DoubleValue == MAX_int32, TEXT("PIE_old_numeric_callback"));
            Test->AddExpectedError(TEXT("Potential data loss during conversion of integer property Rate"),
                                        EAutomationExpectedErrorFlags::Contains, 0);
            const FString Changed = Source.Replace(
                TEXT("Score : int = 5"),
                     TEXT("Score : double = 5.0lf")).Replace(TEXT("Rate : float = 1.5"), TEXT("Rate : int = 2"));
            FString Error;
            FDasNumericMigrationOptions Options;
            Options.IncludeDependencies = true;
            Options.Integers = EDasIntegerRule::NearestAway;
            Check(FFileHelper::SaveStringToFile(Changed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                  TEXT("PIE_write_double_fields"));
#if WITH_DEV_AUTOMATION_TESTS
            DasFailWidenAfterCompileForTesting(true);
            const bool RolledBack = !FDasDSLService::ReimportConvertedNumericFields(BP.Get(), Options, Error);
            DasFailWidenAfterCompileForTesting(false);
            Actor = Find(World, TEXT("WidenActor"));
            Check(RolledBack &&
                  Error.StartsWith(
                      TEXT("Numeric migration rolled back:")) && Actor &&
                      FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("Score")) &&
                                                  FindFProperty<FIntProperty>(
                                                      Actor->GetClass(),
                                                      TEXT("Score"))->GetPropertyValue_InContainer(Actor) == MAX_int32,
                                                      TEXT("PIE_type_and_override_rollback"));
            Check(DasNumericGraphAdapterCount(BP.Get()) == 0 && Actor && DasReadNumericGraphFixture(Actor) == MAX_int32,
                  TEXT("PIE_rollback_restores_original_graph_execution"));
            const auto LastGood = Runtime.RunNamedFramePersistent(Path, TEXT("ue_WidenFields_ReadScore"), {},
                                                                             EDasMethodSignature::Double, Actor);
            Check(LastGood.bSucceeded && LastGood.DoubleValue == MAX_int32,
                  TEXT("PIE_last_good_callback_after_rollback"));
            Test->AddInfo(FString::Printf(TEXT("Rollback callback: success=%d value=%.17g error=%s"),
                                               LastGood.bSucceeded, LastGood.DoubleValue, *LastGood.Error));
#endif
            Check(FDasDSLService::ReimportConvertedNumericFields(BP.Get(), Options, Error),
                  TEXT("PIE_exact_numeric_widen"));
            if (!Error.IsEmpty())
                Test->AddInfo(Error);
            Actor = Find(World, TEXT("WidenActor"));
            Untouched = Find(World, TEXT("WidenUntouched"));
            auto* D = Actor ? FindFProperty<FDoubleProperty>(Actor->GetClass(), TEXT("Score")) : nullptr;
            auto* R = Actor ? FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("Rate")) : nullptr;
            Check(Actor && Untouched && D && R && D->GetPropertyValue_InContainer(Actor) == MAX_int32 &&
                      R->GetPropertyValue_InContainer(Actor) == 0 && D->GetPropertyValue_InContainer(Untouched) == 23 &&
                      R->GetPropertyValue_InContainer(Untouched) == 2,
                  TEXT("PIE_exact_override_and_child_default_values"));
            Check(DasNumericGraphAdapterCount(BP.Get()) >= 2 && Actor && DasReadNumericGraphFixture(Actor) == MAX_int32,
                  TEXT("PIE_checked_graph_adapters_execute"));
            D->SetPropertyValue_InContainer(Actor, -2.5);
            Check(DasReadNumericGraphFixture(Actor) == -3, TEXT("PIE_graph_rounding_half_away"));
            D->SetPropertyValue_InContainer(Actor, 1e20);
            int32 Aborts = 0;
            const auto ExceptionHandle = FBlueprintCoreDelegates::OnScriptException.AddLambda(
                [&](const UObject*, const FFrame&, const FBlueprintExceptionInfo& Info)
                {
                    if (Info.GetType() == EBlueprintExceptionType::AbortExecution &&
                        Info.GetDescription().ToString().Contains(TEXT("int32 range")))
                        ++Aborts;
                });
            Test->AddExpectedError(TEXT("Numeric value exceeds int32 range"), EAutomationExpectedErrorFlags::Contains,
                                        1);
            DasReadNumericGraphFixture(Actor);
            FBlueprintCoreDelegates::OnScriptException.Remove(ExceptionHandle);
            Check(Aborts == 1 && D->GetPropertyValue_InContainer(Actor) == 1e20,
                  TEXT("PIE_runtime_overflow_aborts_without_field_write"));
            D->SetPropertyValue_InContainer(Actor, double(MAX_int32));
            R->SetPropertyValue_InContainer(Actor, 123);
            int32 SetterAborts = 0;
            const auto SetterHandle = FBlueprintCoreDelegates::OnScriptException.AddLambda(
                [&](const UObject*, const FFrame&, const FBlueprintExceptionInfo& Info)
                {
                    if (Info.GetType() == EBlueprintExceptionType::AbortExecution &&
                        Info.GetDescription().ToString().Contains(TEXT("int32 range")))
                        ++SetterAborts;
                });
            auto* Setter = Actor->FindFunction(TEXT("WriteRateByGraph"));
            FStructOnScope SetterFrame(Setter);
            FindFProperty<FFloatProperty>(Setter, TEXT("Input"))
                                                      ->SetPropertyValue_InContainer(SetterFrame.GetStructMemory(),
                                                                                     TNumericLimits<float>::Max());
            Test->AddExpectedError(TEXT("Numeric value exceeds int32 range"), EAutomationExpectedErrorFlags::Contains,
                                        1);
            {
                FEditorScriptExecutionGuard Guard;
                Actor->ProcessEvent(Setter, SetterFrame.GetStructMemory());
            }
            FBlueprintCoreDelegates::OnScriptException.Remove(SetterHandle);
            Check(SetterAborts == 1 && R->GetPropertyValue_InContainer(Actor) == 123,
                  TEXT("PIE_failed_setter_adapter_leaves_destination_unchanged"));
            R->SetPropertyValue_InContainer(Actor, 0);
            const auto AfterCall = Runtime.RunNamedFramePersistent(Path, TEXT("ue_WidenFields_ReadRate"), {},
                                                                              EDasMethodSignature::Double, Actor);
            Check(AfterCall.bSucceeded && AfterCall.DoubleValue == 0, TEXT("PIE_callback_uses_new_double_field"));
            auto* Fresh = World->SpawnActor<ADasProbeActor>(Actor->GetClass(), FTransform::Identity);
            Check(Fresh && D && R && D->GetPropertyValue_InContainer(Fresh) == 23 &&
                      R->GetPropertyValue_InContainer(Fresh) == 2,
                  TEXT("PIE_fresh_child_uses_promoted_defaults"));
            Check(FFileHelper::SaveStringToFile(
                TEXT("options gen2\nrequire ue_foundation\n[export]\ndef ue_WidenFields_ReadScore():double { return 12.0lf }\n[export]\ndef ue_WidenFields_ReadRate():double { return 13.0lf }\n"),
                    *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                TEXT("PIE_write_removed_DSL_declaration"));
            const auto RemovedDSL = Runtime.RunNamedFramePersistent(Path, TEXT("ue_WidenFields_ReadScore"), {},
                                                                               EDasMethodSignature::Double, Actor);
            Check(RemovedDSL.bSucceeded && RemovedDSL.DoubleValue == MAX_int32 &&
                  RemovedDSL.ReloadError.Contains(TEXT("DSL declaration changed")),
                                                  TEXT("PIE_missing_declaration_does_not_bypass_approval_guard"));
            Check(FFileHelper::SaveStringToFile(Changed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                  TEXT("PIE_restore_promoted_source"));
            Check(FDasDSLService::ReimportConvertedNumericFields(BP.Get(), Options, Error),
                  TEXT("PIE_widen_idempotent"));
            CollectGarbage(RF_NoFlags);
            Check(Find(World, TEXT("WidenActor")) != nullptr, TEXT("PIE_promoted_actor_survives_GC"));
        }
        GEditor->RequestEndPlayMap();
        Ending = true;
        Start = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasNumericConvertPIETest, "DasForUE.DSL.NumericConvertPIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasNumericConvertPIETest::RunTest(const FString& Parameters)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
    {
        AddError(TEXT("Isolated project required"));
        return false;
    }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FString Source;
    FFileHelper::LoadFileToString(Source, *Runtime.ScriptPath(TEXT("dsl_widen_numeric.das")));
    const FString Guid = FGuid::NewGuid().ToString(EGuidFormats::Digits),
                  Script = TEXT("dsl_convert_pie_")+ Guid + TEXT(".das"), Path = Runtime.ScriptPath(Script);
    if (!FFileHelper::SaveStringToFile(Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return false;
    auto* World = FAutomationEditorCommonUtils::CreateNewMap();
    UBlueprint* BP = nullptr;
    FString Error;
    const FString Package = TEXT("/Game/DasFrameAssetProbe/WidenPIE_")+ Guid + TEXT("/BP_Parent");
    if (!TestTrue(TEXT("Create widening Blueprint"),
                       FDasDSLService::Apply(Script, Package, TEXT("widen.WidenFields"), BP, Error)))
    {
        AddInfo(Error);
        IFileManager::Get().Delete(*Path);
        return false;
    }
    DasCreateNumericGraphFixture(BP);
    auto* Child = FKismetEditorUtilities::CreateBlueprint(
        BP->GeneratedClass, CreatePackage(*(Package + TEXT("_Child"))),
                                          TEXT("BP_Child"), BPTYPE_Normal, UBlueprint::StaticClass(),
                                               UBlueprintGeneratedClass::StaticClass(), TEXT("DasWidenPIE"));
    UObject* CDO = Child->GeneratedClass->GetDefaultObject();
    FindFProperty<FIntProperty>(Child->GeneratedClass, TEXT("Score"))->SetPropertyValue_InContainer(CDO, 23);
    FindFProperty<FFloatProperty>(Child->GeneratedClass, TEXT("Rate"))->SetPropertyValue_InContainer(CDO, 2.25f);
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("WidenActor");
    auto* Actor = World->SpawnActor<ADasProbeActor>(Child->GeneratedClass, FTransform::Identity, Spawn);
    Spawn.Name = TEXT("WidenUntouched");
    auto* Untouched = World->SpawnActor<ADasProbeActor>(Child->GeneratedClass, FTransform::Identity, Spawn);
    if (!TestTrue(TEXT("Spawn widening actors"), Actor && Untouched))
    {
        IFileManager::Get().Delete(*Path);
        return false;
    }
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(
        MakeShared<FDasNumericConvertPIECommand>(BP, this, Path, Source));
    return true;
}


void DasFailArchiveAfterCompileForTesting(bool Enabled);
void DasFailStructuralForTesting(int32 Stage);
void DasFailRenameForTesting(int32 Stage);
namespace
{
class FDasArchivePIECommand final : public IAutomationLatentCommand
{
    FString Path, Source;
    TStrongObjectPtr<UBlueprint> BP;
    FAutomationTestBase* Test;
    double Start = 0;
    bool Ending = false, Passed = true;

public:
    FDasArchivePIECommand(UBlueprint* B, FAutomationTestBase* T, FString P, FString S)
        : Path(P), Source(S), BP(B), Test(T)
    {
    }
    virtual bool Update() override
    {
        if (Start == 0)
            Start = FPlatformTime::Seconds();
        auto Check = [&](bool V, const TCHAR* N)
        {
            Passed &= Test->TestTrue(N, V);
            UE_LOG(LogTemp, Display, TEXT("DAS_ARCHIVE_PIE %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N);
        };
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        if (Ending)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Start < 30)
                return false;
            Check(!GEditor->PlayWorld, TEXT("EndPIE"));
            Check(Runtime.PersistentContextCount() ==
                      Runtime.PersistentContextCount(GEditor->GetEditorWorldContext().World()),
                  TEXT("play_contexts_released"));
            IFileManager::Get().Delete(*Path);
            UE_LOG(LogTemp, Display, TEXT("DAS_ARCHIVE_PIE_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        if (!World || !World->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Start < 60)
                return false;
            Check(false, TEXT("PIE_started"));
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* Actor = Find(World, TEXT("ArchiveActor"));
        Check(Actor != nullptr, TEXT("real_PIE_actor"));
        if (Actor)
        {
            const FString Key = TEXT("retire.RetireFields::retire.RetireFields.Score");
            auto ReadScore = [](ADasProbeActor* O)
            {
                auto* P = O ? FindFProperty<FIntProperty>(O->GetClass(), TEXT("Score")) : nullptr;
                return P ? P->GetPropertyValue_InContainer(O) : -1;
            };
            auto ArchivedScore = [&](ADasProbeActor* O)
            {
                const auto* V = O ? O->RetiredDasValues.Find(Key) : nullptr;
                return V ? V->IntValue : -1;
            };
            int32 ExpectedCalls = 0;
            auto Call = [&]()
            {
                const auto Result = Runtime.RunNamedFramePersistent(Path, TEXT("ue_RetireFields_Read"), {},
                                                                               EDasMethodSignature::Int, Actor);
                return Result.bSucceeded && Result.Value == ++ExpectedCalls;
            };
            FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("Score"))->SetPropertyValue_InContainer(Actor, 77);
            Check(Call(), TEXT("persistent_method_before_retire"));
            const FGuid OriginalGuid =
                BP->NewVariables
                    .FindByPredicate([](const FBPVariableDescription& V) { return V.VarName == TEXT("Score"); })
                    ->VarGuid;
            FString Removed = Source;
            const int32 A = Removed.Find(TEXT("    //RetireBegin")), B = Removed.Find(TEXT("    //RetireEnd"));
            Removed.RemoveAt(A, B - A + FString(TEXT("    //RetireEnd")).Len());
            FString Error;
            Check(FFileHelper::SaveStringToFile(Removed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                  TEXT("write_retired_declaration"));
            DasFailArchiveAfterCompileForTesting(true);
            bool Failed = !FDasDSLService::ReimportArchivedFields(BP.Get(), false, true, Error);
            DasFailArchiveAfterCompileForTesting(false);
            Actor = Find(World, TEXT("ArchiveActor"));
            Check(Failed && Error.StartsWith(TEXT("Archive migration rolled back:")) && ReadScore(Actor) == 77 &&
                                             Actor->RetiredDasValues.IsEmpty(),
                                             TEXT("retire_failure_restores_layout_and_value"));
            Check(Call(), TEXT("last_good_method_after_retire_failure"));
            bool Retired = FDasDSLService::ReimportArchivedFields(BP.Get(), false, true, Error);
            Check(Retired, TEXT("retire_during_PIE"));
            if (!Retired)
                Test->AddInfo(Error);
            Actor = Find(World, TEXT("ArchiveActor"));
            Check(Actor && !Actor->GetClass()->FindPropertyByName(TEXT("Score")) && ArchivedScore(Actor) == 77,
                                                                  TEXT("retired_live_override"));
            Check(Call(), TEXT("persistent_method_after_retire"));
            if (Retired && Actor)
            {
                FActorSpawnParameters Spawn;
                Spawn.Name = TEXT("ArchiveFresh");
                auto* Fresh = World->SpawnActor<ADasProbeActor>(Actor->GetClass(), FTransform::Identity, Spawn);
                Check(ArchivedScore(Fresh) == 23, TEXT("spawn_retired_child_inherits_archive"));
                Check(FFileHelper::SaveStringToFile(Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                      TEXT("write_restore_declaration"));
                auto Saved = Actor->RetiredDasValues.FindChecked(Key);
                Actor->RetiredDasValues.Remove(Key);
                UClass* Before = BP->GeneratedClass;
                Check(!FDasDSLService::ReimportArchivedFields(BP.Get(), true, true, Error) &&
                          BP->GeneratedClass == Before,
                      TEXT("missing_instance_archive_refused_before_mutation"));
                Actor->RetiredDasValues.Add(Key, Saved);
                auto Incompatible = Saved;
                Incompatible.Kind = int32(EDasScalarKind::Bool);
                Actor->RetiredDasValues.Add(Key, Incompatible);
                Check(!FDasDSLService::ReimportArchivedFields(BP.Get(), true, true, Error) &&
                          BP->GeneratedClass == Before,
                      TEXT("incompatible_instance_archive_refused_before_mutation"));
                Actor->RetiredDasValues.Add(Key, Saved);
                DasFailArchiveAfterCompileForTesting(true);
                Failed = !FDasDSLService::ReimportArchivedFields(BP.Get(), true, true, Error);
                DasFailArchiveAfterCompileForTesting(false);
                Actor = Find(World, TEXT("ArchiveActor"));
                Check(Failed && Error.StartsWith(
                                    TEXT("Archive migration rolled back:")) && ArchivedScore(Actor) == 77 &&
                                    !Actor->GetClass()->FindPropertyByName(TEXT("Score")),
                                                                           TEXT("restore_failure_preserves_archive"));
                Check(Call(), TEXT("last_good_method_after_restore_failure"));
                const bool Restored = FDasDSLService::ReimportArchivedFields(BP.Get(), true, true, Error);
                Check(Restored, TEXT("restore_during_PIE"));
                if (!Restored)
                    Test->AddInfo(Error);
                Actor = Find(World, TEXT("ArchiveActor"));
                Fresh = Find(World, TEXT("ArchiveFresh"));
                Check(ReadScore(Actor) == 77 && ReadScore(Fresh) == 23, TEXT("restore_live_and_new_instance_values"));
                Check(Actor && Actor->RetiredDasValues.IsEmpty() && Fresh && Fresh->RetiredDasValues.IsEmpty(),
                      TEXT("archive_consumed_after_success"));
                auto* Variable = BP->NewVariables.FindByPredicate([](const FBPVariableDescription& V)
                                                                  { return V.VarName == TEXT("Score"); });
                Check(Variable && Variable->VarGuid == OriginalGuid, TEXT("original_variable_GUID_restored"));
                Check(Call(), TEXT("persistent_method_after_restore"));
                CollectGarbage(RF_NoFlags);
                Check(ReadScore(Find(World, TEXT("ArchiveActor"))) == 77, TEXT("restored_actor_survives_GC"));
            }
        }
        GEditor->RequestEndPlayMap();
        Ending = true;
        Start = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasArchivePIETest, "DasForUE.DSL.ArchivePIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasArchivePIETest::RunTest(const FString& Parameters)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
    {
        AddError(TEXT("Isolated project required"));
        return false;
    }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FString Source;
    FFileHelper::LoadFileToString(Source, *Runtime.ScriptPath(TEXT("dsl_retired_fields.das")));
    Source = Source.Replace(TEXT("return self.Marker"),
        TEXT("let count = ue_state_get_int(\"archive.calls\", 0) + self.Marker; ue_state_set_int(\"archive.calls\", count); return count"));
    const FString Guid = FGuid::NewGuid().ToString(EGuidFormats::Digits),
                  Script = TEXT("dsl_archive_pie_")+ Guid + TEXT(".das"), Path = Runtime.ScriptPath(Script);
    if (!FFileHelper::SaveStringToFile(Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return false;
    auto* World = FAutomationEditorCommonUtils::CreateNewMap();
    UBlueprint* BP = nullptr;
    FString Error;
    const FString Package = TEXT("/Game/DasFrameAssetProbe/ArchivePIE_")+ Guid + TEXT("/BP_Parent");
    if (!TestTrue(TEXT("Create archive Blueprint"),
                       FDasDSLService::Apply(Script, Package, TEXT("retire.RetireFields"), BP, Error)))
    {
        AddInfo(Error);
        IFileManager::Get().Delete(*Path);
        return false;
    }
    auto* Child = FKismetEditorUtilities::CreateBlueprint(
        BP->GeneratedClass, CreatePackage(*(Package + TEXT("_Child"))),
                                          TEXT("BP_Child"), BPTYPE_Normal, UBlueprint::StaticClass(),
                                               UBlueprintGeneratedClass::StaticClass(), TEXT("DasArchivePIE"));
    FindFProperty<FIntProperty>(
        Child->GeneratedClass,
        TEXT("Score"))->SetPropertyValue_InContainer(Child->GeneratedClass->GetDefaultObject(), 23);
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("ArchiveActor");
    if (!TestTrue(TEXT("Spawn archive actor"), World->SpawnActor<ADasProbeActor>(
                                                   Child->GeneratedClass, FTransform::Identity, Spawn) != nullptr))
    {
        IFileManager::Get().Delete(*Path);
        return false;
    }
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasArchivePIECommand>(BP, this, Path, Source));
    return true;
}


namespace
{
class FDasLifecycleMigrationCommand final : public IAutomationLatentCommand
{
    FString Path, Source, LifePath;
    TStrongObjectPtr<UBlueprint> BP;
    FAutomationTestBase* Test;
    double Start = 0;
    int32 Phase = 0, TicksBefore = 0;
    bool Passed = true;
    TStrongObjectPtr<UDasLifecycleTestObserver> DestroyObserver{NewObject<UDasLifecycleTestObserver>()},
        StopObserver{NewObject<UDasLifecycleTestObserver>()};

public:
    FDasLifecycleMigrationCommand(UBlueprint* B, FAutomationTestBase* T, FString P, FString S, FString L)
        : Path(P), Source(S), LifePath(L), BP(B), Test(T)
    {
    }
    virtual bool Update() override
    {
        if (Start == 0)
            Start = FPlatformTime::Seconds();
        auto Check = [&](bool V, const TCHAR* N)
        {
            Passed &= Test->TestTrue(N, V);
            UE_LOG(LogTemp, Display, TEXT("DAS_LIFECYCLE_MIGRATION %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N);
        };
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        if (Phase == 2)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Start < 30)
                return false;
            Check(!GEditor->PlayWorld && Runtime.SubscriptionCount() == 0 && Runtime.PersistentContextCount() == 0,
                  TEXT("EndPIE_releases_all_contexts_and_proxies"));
            Check(StopObserver->Calls == 1 && StopObserver->Reason == EEndPlayReason::EndPlayInEditor &&
                      DestroyObserver->Calls == 1,
                  TEXT("native_end_notifications_once"));
            IFileManager::Get().Delete(*Path);
            IFileManager::Get().Delete(*LifePath);
            UE_LOG(LogTemp, Display, TEXT("DAS_LIFECYCLE_MIGRATION_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        auto* Actor = World ? Find(World, TEXT("MigrationActor")) : nullptr;
        auto* External = World ? Find(World, TEXT("MigrationExternal")) : nullptr;
        auto* Signal = World ? Find(World, TEXT("MigrationSignal")) : nullptr;
        if (!World || !World->HasBegunPlay() || !Actor || !External || !Signal)
        {
            if (FPlatformTime::Seconds() - Start < 60)
                return false;
            Check(false, TEXT("PIE_started"));
            GEditor->RequestEndPlayMap();
            Phase = 2;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto Call = [&](ADasProbeActor* O, int32 Selector)
        {
            const auto V = Runtime.RunPersistent(LifePath, Selector, O);
            Check(V.bSucceeded, TEXT("lifecycle_diagnostic_succeeds"));
            return V.Value;
        };
        auto Bindings = [&]()
        {
            return Runtime.SubscriptionCount(Actor) == 2 && Runtime.SubscriptionCount(External) == 2 &&
                   Signal->OnDasSignal.GetAllObjects().Num() == 1 && Actor->OnDasSignal.GetAllObjects().Num() == 2 &&
                   External->OnDasSignal.GetAllObjects().Num() == 1;
        };
        if (Phase == 1)
        {
            const int32 Ticks = Call(Actor, 1);
            if (Ticks <= TicksBefore && FPlatformTime::Seconds() - Start < 30)
                return false;
            Check(Ticks > TicksBefore, TEXT("native_actor_resumes_actual_ticks_after_reinstance"));
            Check(Call(Actor, 0) == 1 && Call(Actor, 2) == 0 && Call(External, 0) == 1 && Call(External, 2) == 0,
                  TEXT("lifecycle_flags_preserved_without_replayed_begin_or_end"));
            CollectGarbage(RF_NoFlags);
            Check(Bindings(), TEXT("proxies_and_remapped_sources_survive_GC"));
            Actor->OnEndPlay.AddDynamic(DestroyObserver.Get(), &UDasLifecycleTestObserver::OnEnded);
            External->OnEndPlay.AddDynamic(StopObserver.Get(), &UDasLifecycleTestObserver::OnEnded);
            const int32 Before = Call(External, 3);
            Check(Actor->Destroy(), TEXT("destroy_reinstanced_actor"));
            Check(Call(External, 3) == Before + 1000 && Runtime.SubscriptionCount(Actor) == 0 &&
                      Runtime.SubscriptionCount(External) == 1 && !Signal->OnDasSignal.IsBound(),
                  TEXT("real_destroy_delivers_script_end_once_then_revokes_owner_bindings"));
            GEditor->RequestEndPlayMap();
            Phase = 2;
            Start = FPlatformTime::Seconds();
            return false;
        }
        Check(Call(Actor, 0) == 1 && Call(Actor, 2) == 0 && Call(Actor, 1) > 0 && Bindings(),
              TEXT("initial_lifecycle_and_self_peer_subscriptions"));
        const bool Suspended = Runtime.BeginPreserveContextsForReinstance(BP->GeneratedClass);
        Check(Suspended, TEXT("explicit_idle_preservation_scope"));
        if (Suspended)
        {
            Check(Runtime.SubscriptionCount() == 0 && !Signal->OnDasSignal.IsBound() && !Actor->OnDasSignal.IsBound(),
                  TEXT("proxies_detached_during_scope"));
            const auto OneShot = Runtime.RunFile(LifePath, 0, false, Actor);
            const auto Persistent = Runtime.RunPersistent(LifePath, 0, Actor);
            Check(!OneShot.bSucceeded &&
                  OneShot.Error.Contains(
                      TEXT("suspended")) && !Persistent.bSucceeded &&
                      Persistent.Error.Contains(TEXT("suspended")),
                                                TEXT("one_shot_and_persistent_VM_calls_blocked_during_scope"));
            Check(Runtime.RunLifecycle(LifePath, EDasCallback::EndPlay, Actor).bSucceeded,
                  TEXT("compile_teardown_suppressed"));
            Check(!Runtime.BeginPreserveContextsForReinstance(BP->GeneratedClass), TEXT("nested_scope_refused"));
            Runtime.EndPreserveContextsForReinstance();
            Check(Bindings() && Call(Actor, 0) == 1 && Call(Actor, 2) == 0,
                  TEXT("scope_resumes_without_script_end_or_duplicate_proxies"));
        }
        Signal->EmitDasSignal(2);
        Actor->EmitDasSignal(3);
        Check(Call(Actor, 3) == 5 && Call(External, 3) == 3, TEXT("independent_listener_state_before_migration"));
        FString Removed = Source;
        const int32 A = Removed.Find(TEXT("    //RetireBegin")), B = Removed.Find(TEXT("    //RetireEnd"));
        Removed.RemoveAt(A, B - A + FString(TEXT("    //RetireEnd")).Len());
        FString Error;
        Check(FFileHelper::SaveStringToFile(Removed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("write_migration_source"));
        auto Cached = Runtime.RunNamedPersistent(LifePath, TEXT("cache_frame"), 1, External);
        Check(Cached.bSucceeded, TEXT("create_call_frame_on_external_owner"));
        const auto Expired = Runtime.RunNamedPersistent(LifePath, TEXT("cache_frame"), 0, External);
        Check(!Expired.bSucceeded && Expired.Error.Contains(TEXT("call handle is released")) && Bindings(),
                                                            TEXT("call_frames_remain_scoped_to_one_VM_call"));
        auto Migrate = [&](bool Restore, bool Inject)
        {
            const bool Written = FFileHelper::SaveStringToFile(Restore ? Source : Removed, *Path,
                                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
            Check(Written, TEXT("write_operation_source"));
            DasFailArchiveAfterCompileForTesting(Inject);
            const bool Result = FDasDSLService::ReimportArchivedFields(BP.Get(), Restore, true, Error);
            DasFailArchiveAfterCompileForTesting(false);
            Check(Inject ? !Result && Error.StartsWith(TEXT("Archive migration rolled back:")) : Result,
                                                       TEXT("migration_or_injected_rollback"));
            if ((Inject && Result) || (!Inject && !Result))
                Test->AddInfo(Error);
            Actor = Find(World, TEXT("MigrationActor"));
            Check(Actor && Actor->HasActorBegunPlay() && Bindings(),
                  TEXT("self_peer_and_external_owner_bindings_reinstalled_once"));
            if (Actor)
            {
                Check(External->Peer == Actor, TEXT("external_native_peer_points_at_replacement"));
                Check(Call(Actor, 0) == 1 && Call(Actor, 2) == 0 && Call(External, 0) == 1 && Call(External, 2) == 0,
                      TEXT("no_gameplay_lifecycle_event_during_compile"));
            }
            return Actor != nullptr;
        };
        if (Migrate(false, true) && Migrate(false, false) && Migrate(true, true) && Migrate(true, false))
        {
            const FString Renamed = Source.Replace(
                TEXT("Score : int = 5"), TEXT("@ue_id=\"retire.RetireFields.Score\"\n    RenamedScore : int = 5"));
            Check(FFileHelper::SaveStringToFile(Renamed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                  TEXT("write_field_rename_source"));
            for (int32 Stage = 1; Stage <= 3; ++Stage)
            {
                const FString OldModel = BP->GetOutermost()->GetMetaData().GetValue(BP.Get(), TEXT("DasDSLModel"));
                DasFailRenameForTesting(Stage);
                const bool Failed = !FDasDSLService::ReimportRenamedFields(BP.Get(), Error, true);
                DasFailRenameForTesting(0);
                Actor = Find(World, TEXT("MigrationActor"));
                Check(Failed &&
                      Error.StartsWith(
                          TEXT("Rename migration rolled back:")) &&
                          BP->GeneratedClass->FindPropertyByName(
                              TEXT("Score")) &&
                              !BP->GeneratedClass->FindPropertyByName(
                                  TEXT("RenamedScore")) &&
                                  OldModel == BP->GetOutermost()->GetMetaData().GetValue(
                                                  BP.Get(), TEXT("DasDSLModel")) && Bindings() && Call(Actor, 0) == 1 &&
                                                  Call(Actor, 2) == 0,
                                                  TEXT("rename_fault_restores_layout_metadata_lifecycle_and_bindings"));
                if (!Failed || !Error.StartsWith(TEXT("Rename migration rolled back:")))
                    Test->AddInfo(Error);
            }
            Check(FDasDSLService::ReimportRenamedFields(BP.Get(), Error, true),
                  TEXT("rename_keeps_lifecycle_contexts"));
            Actor = Find(World, TEXT("MigrationActor"));
            Check(Actor && Bindings() && Call(Actor, 0) == 1 && Call(Actor, 2) == 0,
                  TEXT("lifecycle_and_bindings_after_rename"));
            FString Added = Renamed.Replace(
                TEXT("Marker : int = 1"),
                     TEXT("Marker : int = 1\n    @ue_property=(EditAnywhere,BlueprintReadWrite)\n    Added : int = 9"))
                         .Replace(TEXT("[ue_function(BlueprintCallable)]"),
                                       TEXT("[ue_function(BlueprintCallable,Category=\"Changed\")]"));
            const int32 EndClass = Added.Find(TEXT("}"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
            Added.InsertAt(
                EndClass,
                TEXT("\n    [ue_function(BlueprintCallable)]\n    def NewQuery():int { return self.Added }\n"));
            Check(FFileHelper::SaveStringToFile(Added, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                  TEXT("write_structural_reimport_source"));
            for (int32 Stage = 1; Stage <= 2; ++Stage)
            {
                const FString OldModel = BP->GetOutermost()->GetMetaData().GetValue(BP.Get(), TEXT("DasDSLModel"));
                DasFailStructuralForTesting(Stage);
                const bool Failed = !FDasDSLService::Reimport(BP.Get(), Error);
                DasFailStructuralForTesting(0);
                Actor = Find(World, TEXT("MigrationActor"));
                Check(Failed &&
                      Error.StartsWith(
                          TEXT("Structural migration rolled back:")) &&
                          !BP->GeneratedClass->FindPropertyByName(
                              TEXT("Added")) &&
                              !BP->GeneratedClass->FindFunctionByName(
                                  TEXT("NewQuery")) &&
                                  !BP->FunctionGraphs.ContainsByPredicate(
                                      [](const auto& G) { return G->GetFName() == TEXT("NewQuery"); }) &&
                                  OldModel ==
                                      BP->GetOutermost()->GetMetaData().GetValue(
                                          BP.Get(), TEXT("DasDSLModel")) && Bindings() && Call(Actor, 0) == 1 &&
                                          Call(Actor, 2) == 0,
                                          TEXT("structural_fault_restores_layout_metadata_lifecycle_and_bindings"));
                if (!Failed || !Error.StartsWith(TEXT("Structural migration rolled back:")))
                    Test->AddInfo(Error);
            }
            Check(FDasDSLService::Reimport(BP.Get(), Error), TEXT("structural_reimport_keeps_lifecycle_contexts"));
            Actor = Find(World, TEXT("MigrationActor"));
            Check(Actor && Bindings() && Call(Actor, 0) == 1 && Call(Actor, 2) == 0,
                  TEXT("lifecycle_and_bindings_after_structural_reimport"));
            Check(BP->GeneratedClass->FindFunctionByName(
                TEXT("NewQuery")) &&
                BP->FunctionGraphs.ContainsByPredicate([](const auto& G) { return G->GetFName() == TEXT("NewQuery"); }),
                TEXT("retry_after_rollback_creates_added_method_with_exact_name"));
            Signal->EmitDasSignal(7);
            Actor->EmitDasSignal(11);
            Check(Call(Actor, 3) == 23 && Call(External, 3) == 14,
                  TEXT("retained_handler_state_and_no_duplicate_delivery_after_success_and_rollback"));
            TicksBefore = Call(Actor, 1);
            Phase = 1;
            Start = FPlatformTime::Seconds();
            return false;
        }
        GEditor->RequestEndPlayMap();
        Phase = 2;
        Start = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasLifecycleMigrationTest, "DasForUE.DSL.LifecycleMigrationPIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasLifecycleMigrationTest::RunTest(const FString& Parameters)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
    {
        AddError(TEXT("Isolated project required"));
        return false;
    }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FString Source;
    FFileHelper::LoadFileToString(Source, *Runtime.ScriptPath(TEXT("dsl_retired_fields.das")));
    const FString Guid = FGuid::NewGuid().ToString(EGuidFormats::Digits),
                  Script =
                      TEXT("dsl_lifecycle_migration_")+ Guid +
                           TEXT(".das"),
                                Path = Runtime.ScriptPath(Script),
                                LifePath = FPaths::ConvertRelativePathToFull(
                                    FPaths::ProjectSavedDir() / (TEXT("LifecycleMigration_")+ Guid + TEXT(".das")));
    const FString Life = TEXT("options gen2\nrequire ue_foundation\nvar saved:UCallHandle\n[export]\ndef cache_frame(input:int):int { if(input==1){ saved=ue_begin_call(ue_self_object(), \"K2_GetActorLocation\") } else { ue_release_call(saved) }; return 1 }\n[export]\ndef begin_play(){ ue_state_set_int(\"begins\",ue_state_get_int(\"begins\",0)+1) }\n[export]\ndef tick(dt:float){ ue_state_set_int(\"ticks\",ue_state_get_int(\"ticks\",0)+1) }\n[export]\ndef end_play(reason:int){ ue_state_set_int(\"ends\",ue_state_get_int(\"ends\",0)+1); ue_emit_signal(1000+reason) }\n[export]\ndef bind_events(){ ue_subscribe_int(\"self\",\"OnDasSignal\",\"on_signal\"); ue_subscribe_int(\"peer\",\"OnDasSignal\",\"on_signal\") }\n[export]\ndef on_signal(value:int){ ue_state_set_int(\"total\",ue_state_get_int(\"total\",0)+value) }\n[export]\ndef main(input:int):int { if(input==0){ return ue_state_get_int(\"begins\",0) }; if(input==1){ return ue_state_get_int(\"ticks\",0) }; if(input==2){ return ue_state_get_int(\"ends\",0) }; return ue_state_get_int(\"total\",0) }\n");
    if (!FFileHelper::SaveStringToFile(Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
        !FFileHelper::SaveStringToFile(Life, *LifePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return false;
    auto* World = FAutomationEditorCommonUtils::CreateNewMap();
    UBlueprint* BP = nullptr;
    FString Error;
    const FString Package = TEXT("/Game/DasFrameAssetProbe/LifecycleMigration_")+ Guid + TEXT("/BP_Parent");
    if (!TestTrue(TEXT("Create migration Blueprint"),
                       FDasDSLService::Apply(Script, Package, TEXT("retire.RetireFields"), BP, Error)))
    {
        AddInfo(Error);
        IFileManager::Get().Delete(*Path);
        IFileManager::Get().Delete(*LifePath);
        return false;
    }
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("MigrationSignal");
    auto* Signal = World->SpawnActor<ADasProbeActor>(ADasProbeActor::StaticClass(), FTransform::Identity, Spawn);
    Spawn.Name = TEXT("MigrationActor");
    auto* Actor = World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
    Spawn.Name = TEXT("MigrationExternal");
    auto* External = World->SpawnActor<ADasProbeActor>(ADasProbeActor::StaticClass(), FTransform::Identity, Spawn);
    if (!TestTrue(TEXT("Spawn migration actors"), Signal && Actor && External))
        return false;
    Actor->Peer = Signal;
    External->Peer = Actor;
    Actor->LifecycleScript = LifePath;
    External->LifecycleScript = LifePath;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(
        MakeShared<FDasLifecycleMigrationCommand>(BP, this, Path, Source, LifePath));
    return true;
}


namespace
{
class FDasObjectFieldsPIECommand final : public IAutomationLatentCommand
{
    FString Path, Source;
    TStrongObjectPtr<UBlueprint> BP;
    FAutomationTestBase* Test;
    double Start = 0;
    bool Ending = false, Passed = true;

public:
    FDasObjectFieldsPIECommand(UBlueprint* B, FAutomationTestBase* T, FString P, FString S)
        : Path(P), Source(S), BP(B), Test(T)
    {
    }
    virtual bool Update() override
    {
        if (Start == 0)
            Start = FPlatformTime::Seconds();
        auto Check = [&](bool V, const TCHAR* N)
        {
            Passed &= Test->TestTrue(N, V);
            UE_LOG(LogTemp, Display, TEXT("DAS_OBJECT_FIELDS_PIE %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N);
        };
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        if (Ending)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Start < 30)
                return false;
            Check(!GEditor->PlayWorld && Runtime.PersistentContextCount() == 0,
                  TEXT("EndPIE_releases_object_field_contexts"));
            IFileManager::Get().Delete(*Path);
            UE_LOG(LogTemp, Display, TEXT("DAS_OBJECT_FIELDS_PIE_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        if (!World || !World->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Start < 60)
                return false;
            Check(false, TEXT("PIE_started"));
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* A = Find(World, TEXT("ObjectFieldA"));
        auto* B = Find(World, TEXT("ObjectFieldB"));
        if (!A || !B)
        {
            Check(false, TEXT("PIE_actors"));
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto Invoke = [&](ADasProbeActor* Owner, const TCHAR* Entry, UObject* Value)
        {
            FDasScalarValue Arg;
            Arg.Kind = 8;
            Arg.ObjectValue = Value;
            Arg.ObjectClass = TEXT("/Script/CoreUObject.Object");
            return Runtime.RunNamedFramePersistent(Path, FString(TEXT("ue_ObjectFields_")) + Entry, {Arg},
                                                                 EDasMethodSignature::Void, Owner);
        };
        auto Read = [&](ADasProbeActor* Owner, const TCHAR* Entry, const TCHAR* Class)
        {
            return Runtime.RunNamedFramePersistent(Path, FString(TEXT("ue_ObjectFields_")) + Entry, {},
                                                                 EDasMethodSignature::Object, Owner, Class);
        };
        auto Field = [](ADasProbeActor* O, FName Name)
        {
            auto* P = O ? FindFProperty<FObjectProperty>(O->GetClass(), Name) : nullptr;
            return P ? P->GetObjectPropertyValue_InContainer(O) : nullptr;
        };
        Check(Invoke(A, TEXT("SetTarget"), B).bSucceeded && Invoke(B, TEXT("SetTarget"), A).bSucceeded,
                                                                   TEXT("typed_actor_field_assignment_and_cycle"));
        Check(Invoke(A, TEXT("SetComponent"), B->GetRootComponent()).bSucceeded,
                     TEXT("typed_component_field_assignment"));
        auto Wrong = Invoke(A, TEXT("SetTarget"), B->GetRootComponent());
        Check(!Wrong.bSucceeded && Wrong.Error.Contains(TEXT("field class")) &&
                                                        Field(A, TEXT("Target")) == B,
                                                              TEXT("wrong_class_refused_without_write"));
        auto* F = A->FindFunction(TEXT("GetTarget"));
        FStructOnScope Frame(F);
        {
            FEditorScriptExecutionGuard Guard;
            A->ProcessEvent(F, Frame.GetStructMemory());
        }
        Check(CastField<FObjectProperty>(F->GetReturnProperty())->PropertyClass == AActor::StaticClass() &&
                  CastField<FObjectProperty>(F->GetReturnProperty())
                          ->GetObjectPropertyValue_InContainer(Frame.GetStructMemory()) == B,
              TEXT("native_typed_getter_ProcessEvent"));
        UObject* Held = NewObject<UCurveFloat>(GetTransientPackage());
        TWeakObjectPtr<UObject> WeakHeld(Held);
        Check(Invoke(A, TEXT("Hold"), Held).bSucceeded, TEXT("assign_unrooted_object_to_strong_UPROPERTY"));
        Held = nullptr;
        CollectGarbage(RF_NoFlags);
        Check(WeakHeld.IsValid() &&
              Read(A, TEXT("Recall"), TEXT("/Script/CoreUObject.Object")).ObjectValue.Get() == WeakHeld.Get(),
                   TEXT("field_keeps_object_alive_across_GC"));
        Check(Invoke(A, TEXT("Hold"), nullptr).bSucceeded, TEXT("clear_nullable_object_field"));
        CollectGarbage(RF_NoFlags);
        Check(!WeakHeld.IsValid(), TEXT("cleared_field_does_not_leave_VM_GC_root"));
        FString Error;
        FString Added = Source;
        const int32 End = Added.Find(TEXT("}"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
        Added.InsertAt(End, TEXT("\n    @ue_property=(EditAnywhere,BlueprintReadWrite)\n    Extra : int = 9\n"));
        Check(FFileHelper::SaveStringToFile(Added, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("write_object_structural_change"));
        DasFailStructuralForTesting(1);
        const bool Failed = !FDasDSLService::Reimport(BP.Get(), Error);
        DasFailStructuralForTesting(0);
        A = Find(World, TEXT("ObjectFieldA"));
        B = Find(World, TEXT("ObjectFieldB"));
        Check(Failed && Error.StartsWith(TEXT("Structural migration rolled back:")) &&
                                         Field(A, TEXT("Target")) == B &&
                                               Field(B, TEXT("Target")) == A &&
                                                     Field(A, TEXT("Component")) == B->GetRootComponent(),
                                                           TEXT("rollback_remaps_cycle_and_component_references"));
        const FString Renamed =
            Source.Replace(TEXT("Target : UObjectHandle"),
                                TEXT("@ue_id=\"objectfields.ObjectFields.Target\"\n    PeerActor : UObjectHandle"))
                                    .Replace(TEXT("self.Target"), TEXT("self.PeerActor"));
        Check(FFileHelper::SaveStringToFile(Renamed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("write_object_rename"));
        Check(FDasDSLService::ReimportRenamedFields(BP.Get(), Error, true), TEXT("rename_object_field_by_id"));
        if (!Error.IsEmpty())
            Test->AddInfo(Error);
        A = Find(World, TEXT("ObjectFieldA"));
        B = Find(World, TEXT("ObjectFieldB"));
        Check(Field(A, TEXT("PeerActor")) == B && Field(B, TEXT("PeerActor")) == A &&
                                                        Field(A, TEXT("Component")) == B->GetRootComponent(),
                                                              TEXT("rename_preserves_remapped_reference_identity"));
        FString Removed = Renamed;
        const FString Block = TEXT("    @ue_property=(EditAnywhere,BlueprintReadWrite)\n    @ue_id=\"objectfields.ObjectFields.Target\"\n    PeerActor : UObjectHandle = ue_object_value(\"/Script/Engine.Actor\")\n");
        Removed = Removed.Replace(
            *Block,
            TEXT(""))
                .Replace(TEXT("self.PeerActor = value"),
                    TEXT(""))
                        .Replace(TEXT("return self.PeerActor"),
                            TEXT("return ue_null_object()"))
                                .Replace(
                                    TEXT("ue_object_valid(self.PeerActor)"),
                                         TEXT("false")).Replace(TEXT("self.PeerActor = ue_null_object(); "), TEXT(""));
        Check(FFileHelper::SaveStringToFile(Removed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("write_retired_object_field"));
        Check(FDasDSLService::ReimportArchivedFields(BP.Get(), false, true, Error), TEXT("retire_object_field"));
        if (!Error.IsEmpty())
            Test->AddInfo(Error);
        A = Find(World, TEXT("ObjectFieldA"));
        B = Find(World, TEXT("ObjectFieldB"));
        const FString Key = TEXT("objectfields.ObjectFields::objectfields.ObjectFields.Target");
        const auto* Archived = A->RetiredDasValues.Find(Key);
        Check(Archived && Archived->ObjectValue.Get() == B, TEXT("retired_object_reference_is_remapped"));
        CollectGarbage(RF_NoFlags);
        Check(FFileHelper::SaveStringToFile(Renamed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("write_object_restore"));
        Check(FDasDSLService::ReimportArchivedFields(BP.Get(), true, true, Error), TEXT("restore_object_field"));
        if (!Error.IsEmpty())
            Test->AddInfo(Error);
        A = Find(World, TEXT("ObjectFieldA"));
        B = Find(World, TEXT("ObjectFieldB"));
        Check(Field(A, TEXT("PeerActor")) == B && Field(B, TEXT("PeerActor")) == A && A->RetiredDasValues.IsEmpty(),
                                                        TEXT("restored_reference_cycle_and_consumed_archive"));
        Check(Read(A, TEXT("GetComponent"), TEXT("/Script/Engine.ActorComponent")).ObjectValue.Get() ==
                           B->GetRootComponent(),
                   TEXT("component_getter_after_reinstance"));
        Check(B->Destroy(), TEXT("destroy_referenced_actor"));
        const auto Dead = Read(A, TEXT("GetTarget"), TEXT("/Script/Engine.Actor"));
        Check(Dead.bSucceeded && !Dead.ObjectValue.IsValid(), TEXT("destroyed_actor_field_reads_as_null"));
        const auto Valid =
            Runtime.RunNamedFramePersistent(Path, TEXT("ue_ObjectFields_HasTarget"), {}, EDasMethodSignature::Bool, A);
        Check(Valid.bSucceeded && !Valid.BoolValue, TEXT("destroyed_reference_not_resurrected"));
        GEditor->RequestEndPlayMap();
        Ending = true;
        Start = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasObjectFieldsPIETest, "DasForUE.DSL.ObjectFieldsPIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasObjectFieldsPIETest::RunTest(const FString& Parameters)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
    {
        AddError(TEXT("Isolated smoke project required"));
        return false;
    }
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FString Source;
    FFileHelper::LoadFileToString(Source, *R.ScriptPath(TEXT("dsl_object_fields.das")));
    Source = Source.Replace(TEXT("\r\n"), TEXT("\n"));
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits),
                  Script = TEXT("dsl_object_fields_pie_")+ Id + TEXT(".das"), Path = R.ScriptPath(Script);
    if (!FFileHelper::SaveStringToFile(Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return false;
    auto* W = FAutomationEditorCommonUtils::CreateNewMap();
    UBlueprint* BP = nullptr;
    FString Error;
    if (!TestTrue(
            TEXT("Create typed object fields"),
                 FDasDSLService::Apply(Script, TEXT("/Game/DasFrameAssetProbe/ObjectFields_")+ Id +
                                                    TEXT("/BP_Fields"), TEXT("objectfields.ObjectFields"), BP, Error)))
    {
        AddInfo(Error);
        IFileManager::Get().Delete(*Path);
        return false;
    }
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("ObjectFieldA");
    auto* A = W->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
    Spawn.Name = TEXT("ObjectFieldB");
    auto* B = W->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
    if (!TestTrue(TEXT("Spawn object field actors"), A && B))
        return false;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(
        MakeShared<FDasObjectFieldsPIECommand>(BP, this, Path, Source));
    return true;
}


namespace
{
class FDasAuthoredValuesPIECommand final : public IAutomationLatentCommand
{
    TStrongObjectPtr<UBlueprint> BP;
    FAutomationTestBase* Test;
    FString Path, Source, EnumPath, StructPath;
    double Start = 0;
    bool Ending = false, Passed = true;

public:
    FDasAuthoredValuesPIECommand(UBlueprint* B, FAutomationTestBase* T, FString P, FString S, FString E, FString V)
        : BP(B), Test(T), Path(P), Source(S), EnumPath(E), StructPath(V)
    {
    }
    virtual bool Update() override
    {
        if (Start == 0)
            Start = FPlatformTime::Seconds();
        auto Check = [&](bool V, const TCHAR* N)
        {
            Passed &= Test->TestTrue(N, V);
            UE_LOG(LogTemp, Display, TEXT("DAS_AUTHORED_VALUES_PIE %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N);
        };
        auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        if (Ending)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Start < 30)
                return false;
            Check(!GEditor->PlayWorld && R.PersistentContextCount() == 0, TEXT("EndPIE_releases_typed_contexts"));
            IFileManager::Get().Delete(*Path);
            UE_LOG(LogTemp, Display, TEXT("DAS_AUTHORED_VALUES_PIE_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
            return true;
        }
        UWorld* W = GEditor->PlayWorld;
        if (!W || !W->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Start < 60)
                return false;
            Check(false, TEXT("PIE_started"));
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* A = Find(W, TEXT("AuthoredActor"));
        if (!A)
        {
            Check(false, TEXT("typed_PIE_actor"));
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto* Struct = FindObject<UScriptStruct>(nullptr, *StructPath);
        auto* Count = CastFieldChecked<FIntProperty>(FindDasStructField(Struct, TEXT("Count")));
        FDasScalarValue Mode;
        Mode.Kind = 13;
        Mode.IntValue = 42;
        Mode.ObjectClass = EnumPath;
        FDasScalarValue Data;
        Data.Kind = 12;
        Data.ObjectClass = StructPath;
        Data.StructValue.InitializeAs(Struct);
        Count->SetPropertyValue_InContainer(Data.StructValue.GetMutableMemory(), 77);
        Check(R.RunNamedFramePersistent(
            Path, TEXT("ue_TypedActor_SetMode"), {Mode}, EDasMethodSignature::Void, A).bSucceeded &&
            R.RunNamedFramePersistent(Path,
                                      TEXT("ue_TypedActor_SetConfig"), {Data}, EDasMethodSignature::Void, A).bSucceeded,
                                      TEXT("PIE_typed_field_setters"));
        auto Mixed = R.RunNamedFramePersistent(Path, TEXT("ue_TypedActor_Evaluate"), {Mode, Data},
                                                          EDasMethodSignature::Struct, A, StructPath);
        Check(Mixed.bSucceeded && Count->GetPropertyValue_InContainer(Mixed.StructValue.GetMemory()) == 119,
              TEXT("PIE_mixed_typed_enum_struct_frame"));
        FDasScalarValue Other = Mode;
        Other.ObjectClass = TEXT("/Script/DasRuntime.EDasCallMode");
        Check(!R.RunNamedFramePersistent(
            Path, TEXT("ue_TypedActor_SetMode"), {Other}, EDasMethodSignature::Void, A).bSucceeded &&
            FindFProperty<FByteProperty>(A->GetClass(), TEXT("Mode"))->GetPropertyValue_InContainer(A) == 42,
                                         TEXT("different_enum_identity_refused_without_write"));
        FString Changed = Source;
        const int32 End = Changed.Find(TEXT("}"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
        Changed.InsertAt(End, TEXT("\n    @ue_property=(EditAnywhere,BlueprintReadWrite)\n    Extra : int = 9\n"));
        Check(FFileHelper::SaveStringToFile(Changed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("write_typed_structural_change"));
        FString Error;
        DasFailStructuralForTesting(1);
        const bool Failed = !FDasDSLService::Reimport(BP.Get(), Error);
        DasFailStructuralForTesting(0);
        A = Find(W, TEXT("AuthoredActor"));
        auto* Config = FindFProperty<FStructProperty>(A->GetClass(), TEXT("Config"));
        Check(Failed &&
              Error.StartsWith(TEXT("Structural migration rolled back:")) &&
                               FindFProperty<FByteProperty>(
                                   A->GetClass(), TEXT("Mode"))->GetPropertyValue_InContainer(A) == 42 &&
                                   Count->GetPropertyValue_InContainer(Config->ContainerPtrToValuePtr<void>(A)) == 77,
                                   TEXT("typed_enum_struct_override_rollback"));
        Mixed = R.RunNamedFramePersistent(Path, TEXT("ue_TypedActor_Evaluate"), {Mode, Data},
                                                     EDasMethodSignature::Struct, A, StructPath);
        Check(Mixed.bSucceeded && Count->GetPropertyValue_InContainer(Mixed.StructValue.GetMemory()) == 119,
              TEXT("last_good_typed_callback_after_rollback"));
        Check(FFileHelper::SaveStringToFile(Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("restore_disposable_source"));
        CollectGarbage(RF_NoFlags);
        Check(Find(W, TEXT("AuthoredActor")) != nullptr, TEXT("typed_PIE_actor_survives_GC"));
        GEditor->RequestEndPlayMap();
        Ending = true;
        Start = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasAuthoredValuesPIETest, "DasForUE.DSL.AuthoredValuesPIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasAuthoredValuesPIETest::RunTest(const FString& Parameters)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
    {
        AddError(TEXT("Isolated project required"));
        return false;
    }
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits),
                  Base = TEXT("/Game/DasFrameAssetProbe/AuthoredPIE_")+ Id,
                              Script = TEXT("dsl_authored_pie_")+ Id + TEXT(".das"), Path = R.ScriptPath(Script);
    TArray<UObject*> Types;
    FString Error;
    if (!TestTrue(TEXT("Create PIE types"),
                       FDasTypeService::Apply(TEXT("dsl_type_declarations.das"), Base + TEXT("/Types"), Types, Error)))
    {
        AddInfo(Error);
        return false;
    }
    FString Source;
    FFileHelper::LoadFileToString(Source, *R.ScriptPath(TEXT("dsl_authored_values.das")));
    Source = Source.Replace(TEXT("/Game/DasAuthoredTypes"), *(Base + TEXT("/Types")));
    if (!FFileHelper::SaveStringToFile(Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return false;
    auto* W = FAutomationEditorCommonUtils::CreateNewMap();
    UBlueprint* BP = nullptr;
    if (!TestTrue(
            TEXT("Create typed PIE class"),
                 FDasDSLService::Apply(Script, Base + TEXT("/BP_Actor"), TEXT("authoredvalues.TypedActor"), BP, Error)))
    {
        AddInfo(Error);
        IFileManager::Get().Delete(*Path);
        return false;
    }
    FActorSpawnParameters P;
    P.Name = TEXT("AuthoredActor");
    if (!TestTrue(TEXT("Spawn typed PIE actor"),
                       W->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, P) != nullptr))
        return false;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasAuthoredValuesPIECommand>(
        BP, this, Path, Source, Types[0]->GetPathName(), Types[1]->GetPathName()));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasDirectTypesPIETest, "DasForUE.DSL.DirectTypesPIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasDirectTypesPIETest::RunTest(const FString& Parameters)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
    {
        AddError(TEXT("Isolated project required"));
        return false;
    }
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits),
                  Base = TEXT("/Game/DasFrameAssetProbe/AuthoredPIE_")+ Id,
                              Script = TEXT("dsl_authored_pie_")+ Id + TEXT(".das"), Path = R.ScriptPath(Script);
    TArray<UObject*> Types;
    FString Error, Source;
    FFileHelper::LoadFileToString(Source, *R.ScriptPath(TEXT("dsl_direct_types.das")));
    Source = Source.Replace(TEXT("/Game/DasAuthoredTypes"), *(Base + TEXT("/Types")));
    if (!FFileHelper::SaveStringToFile(Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return false;
    if (!TestTrue(TEXT("Create direct PIE types"), FDasTypeService::Apply(Script, Base + TEXT("/Types"), Types, Error)))
    {
        AddInfo(Error);
        IFileManager::Get().Delete(*Path);
        return false;
    }
    auto* W = FAutomationEditorCommonUtils::CreateNewMap();
    UBlueprint* BP = nullptr;
    if (!TestTrue(
            TEXT("Create typed PIE class"),
                 FDasDSLService::Apply(Script, Base + TEXT("/BP_Actor"), TEXT("authoredvalues.TypedActor"), BP, Error)))
    {
        AddInfo(Error);
        IFileManager::Get().Delete(*Path);
        return false;
    }
    FActorSpawnParameters P;
    P.Name = TEXT("AuthoredActor");
    if (!TestTrue(TEXT("Spawn typed PIE actor"),
                       W->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, P) != nullptr))
        return false;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasAuthoredValuesPIECommand>(
        BP, this, Path, Source, Types[0]->GetPathName(), Types[1]->GetPathName()));
    return true;
}
