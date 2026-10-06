#include "DasReloadCommandlet.h"
#include "DasProbeActor.h"
#include "DasRuntime.h"
#include "DasBlueprintService.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/Level.h"
#include "Engine/World.h"
#include "EngineUtils.h"
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
        if (!FDasBlueprintService::ReadSchema(Script, Schema, Error)) return false;
        // Test fixture values; they are no longer hard-coded in the editor backend.
        for (auto& Field : Schema.Fields)
        {
            if (Field.Name == TEXT("speed")) Field.FloatDefault = 10.f;
            if (Field.Name == TEXT("health")) Field.IntDefault = 100;
        }
        return FDasBlueprintService::Apply(BP, Schema, Error);
    }
    bool SetFloat(UObject* Object, FName Name, float Value)
    {
        auto* Property = FindFProperty<FFloatProperty>(Object->GetClass(), Name);
        if (!Property) return false;
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
        if (!Function) return MIN_int32;
        FStructOnScope Frame(Function);
        auto* Input = FindFProperty<FIntProperty>(Function, TEXT("Input"));
        auto* Return = CastField<FIntProperty>(Function->GetReturnProperty());
        if (!Input || !Return) return MIN_int32;
        Input->SetPropertyValue_InContainer(Frame.GetStructMemory(), 40);
        // Editor-world ProcessEvent intentionally requires this explicit guard.
        FEditorScriptExecutionGuard ScriptGuard;
        Object->ProcessEvent(Function, Frame.GetStructMemory());
        return Return->GetPropertyValue_InContainer(Frame.GetStructMemory());
    }
    UBlueprint* Blueprint(const FString& PackagePath, UClass* Parent)
    {
        UPackage* Package = CreatePackage(*PackagePath);
        return FKismetEditorUtilities::CreateBlueprint(Parent, Package, *FPackageName::GetShortName(PackagePath), BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), TEXT("DasReloadProbe"));
    }
    bool Save(UObject* Object, bool bMap = false)
    {
        UPackage* Package = Object->GetOutermost();
        Object->SetFlags(RF_Public | RF_Standalone);
        const FString Filename = FPackageName::LongPackageNameToFilename(Package->GetName(), bMap ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension());
        IFileManager::Get().MakeDirectory(*FPaths::GetPath(Filename), true);
        FSavePackageArgs Args;
        Args.TopLevelFlags = RF_Public | RF_Standalone;
        Args.SaveFlags = SAVE_NoError;
        return UPackage::SavePackage(Package, Object, *Filename, Args);
    }
    ADasProbeActor* Find(UWorld* World, FName Name)
    {
        for (TActorIterator<ADasProbeActor> It(World); It; ++It)
            if (It->GetFName() == Name) return *It;
        return nullptr;
    }
}

UDasReloadCommandlet::UDasReloadCommandlet()
{
    IsClient = false; IsServer = false; IsEditor = true; LogToConsole = true;
}

int32 UDasReloadCommandlet::Main(const FString& Params)
{
    int32 Failures = 0;
    auto Expect = [&](bool Condition, const TCHAR* Name)
    {
        UE_LOG(LogTemp, Display, TEXT("DAS_RELOAD %s %s"), Condition ? TEXT("PASS") : TEXT("FAIL"), Name);
        if (!Condition) ++Failures;
        return Condition;
    };
    FString Root;
    const bool bReopen = FParse::Value(*Params, TEXT("Reopen="), Root);
    if (!bReopen) Root = TEXT("/Game/DasReloadProbe/Run_") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
    if (!Root.StartsWith(TEXT("/Game/DasReloadProbe/Run_")) || Root.Contains(TEXT(".."))) return 1;
    UE_LOG(LogTemp, Display, TEXT("DAS_RELOAD_ROOT=%s"), *Root);
    TStrongObjectPtr<UBlueprint> Parent(bReopen ? LoadObject<UBlueprint>(nullptr, *(Root + TEXT("/BP_DasGenerated.BP_DasGenerated"))) : Blueprint(Root + TEXT("/BP_DasGenerated"), ADasProbeActor::StaticClass()));
    if (!Expect(Parent.IsValid(), TEXT("parent_asset"))) return 1;
    FString Error;
    if (!bReopen && !Expect(ApplySchema(Parent.Get(), TEXT("schema_v1.das"), Error), TEXT("schema_v1_to_uclass")))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Error); return 1;
    }
    TStrongObjectPtr<UBlueprint> Child(bReopen ? LoadObject<UBlueprint>(nullptr, *(Root + TEXT("/BP_Child.BP_Child"))) : Blueprint(Root + TEXT("/BP_Child"), Parent->GeneratedClass));
    if (!Expect(Child.IsValid(), TEXT("blueprint_child"))) return 1;
    if (!bReopen)
    {
        if (!Expect(Compile(Child.Get()), TEXT("child_compiles"))) return 1;
        SetFloat(Child->GeneratedClass->GetDefaultObject(), TEXT("speed"), 20.f);
        Child->Modify();
    }
    UWorld* World = bReopen ? LoadObject<UWorld>(nullptr, *(Root + TEXT("/ProbeMap.ProbeMap"))) :
        UWorld::CreateWorld(EWorldType::Editor, false, TEXT("ProbeMap"), CreatePackage(*(Root + TEXT("/ProbeMap"))));
    if (!Expect(World != nullptr, TEXT("editor_world"))) return 1;
    TStrongObjectPtr<UWorld> KeepWorld(World);
    FWorldContext& Context = GEditor->GetEditorWorldContext();
    UWorld* PreviousWorld = Context.World();
    Context.SetCurrentWorld(World);
    if (bReopen)
    {
        World->WorldType = EWorldType::Editor;
        if (!World->IsInitialized()) World->InitWorld();
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
        if (!Actor || !Holder) { Context.SetCurrentWorld(PreviousWorld); return 1; }
        SetFloat(Actor, TEXT("speed"), 37.f);
        Holder->Peer = Actor;
        Actor->Peer = Holder;
        Expect(Evaluate(Actor) == 41, TEXT("v1_reflected_script_method"));
        Expect(Float(Parent->GeneratedClass->GetDefaultObject(), TEXT("speed")) == 10.f, TEXT("parent_default_v1"));
        Expect(Float(Child->GeneratedClass->GetDefaultObject(), TEXT("speed")) == 20.f, TEXT("child_default_override_v1"));
        UObject* OldActor = Actor;
        bool bReplaced = false;
        const FDelegateHandle ReplacementHandle = FCoreUObjectDelegates::OnObjectsReplaced.AddLambda([&](const FCoreUObjectDelegates::FReplacementObjectMap& Map)
        {
            bReplaced |= Map.Contains(OldActor);
        });
        const bool bApplied = ApplySchema(Parent.Get(), TEXT("reload_v2.das"), Error);
        FCoreUObjectDelegates::OnObjectsReplaced.Remove(ReplacementHandle);
        Expect(bApplied, TEXT("schema_v2_same_process"));
        if (!bApplied) UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        Actor = Find(World, TEXT("ProbeInstance"));
        Holder = Find(World, TEXT("ReferenceHolder"));
        Expect(bReplaced, TEXT("instance_reinstanced"));
        Expect(Child->ParentClass == Parent->GeneratedClass && Child->Status != BS_Error, TEXT("dependent_blueprint_updated"));
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
        Expect(Actor->ExecuteDas(TEXT("self_properties.das"), 5) == 105 && Int(Actor, TEXT("health")) == 105
            && Float(Actor, TEXT("speed")) == Before + 1.f, TEXT("script_self_reads_and_writes_reloaded_properties"));
        SetFloat(Actor, TEXT("speed"), Before);
        if (auto* Health = FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("health"))) Health->SetPropertyValue_InContainer(Actor, 100);
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
        if (FParse::Value(*Params, TEXT("Manifest="), Manifest)) Expect(FFileHelper::SaveStringToFile(Root, *Manifest), TEXT("save_manifest"));
    }
    Context.SetCurrentWorld(PreviousWorld);
    World->DestroyWorld(false);
    UE_LOG(LogTemp, Display, TEXT("DAS_RELOAD_RESULT=%s failures=%d reopen=%d"), Failures ? TEXT("FAIL") : TEXT("PASS"), Failures, bReopen);
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
        FDasPIEReloadCommand(TSharedRef<FDasPIEState> InState, FAutomationTestBase* InTest)
            : State(InState), Test(InTest) {}

        virtual bool Update() override
        {
            if (Started == 0) Started = FPlatformTime::Seconds();
            auto Check = [&](bool Value, const TCHAR* Name)
            {
                State->bPassed &= Test->TestTrue(Name, Value);
                UE_LOG(LogTemp, Display, TEXT("DAS_PIE %s %s"), Value ? TEXT("PASS") : TEXT("FAIL"), Name);
            };
            if (bEnding)
            {
                if (GEditor->PlayWorld && FPlatformTime::Seconds() - Started < 30) return false;
                Check(GEditor->PlayWorld == nullptr, TEXT("PIE_ended"));
                auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
                Check(Runtime.PersistentContextCount() == Runtime.PersistentContextCount(GEditor->GetEditorWorldContext().World()),
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
                if (FPlatformTime::Seconds() - Started < 60) return false;
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
            if (Actor) SetFloat(Actor, TEXT("speed"), 73.f);
            UObject* OldActor = Actor;
            bool bReplaced = false;
            const auto Handle = FCoreUObjectDelegates::OnObjectsReplaced.AddLambda([&](const FCoreUObjectDelegates::FReplacementObjectMap& Map)
            {
                bReplaced |= Map.Contains(OldActor);
            });
            FString Error;
            Check(ApplySchema(State->Parent.Get(), TEXT("reload_v2.das"), Error), TEXT("reload_during_PIE"));
            FCoreUObjectDelegates::OnObjectsReplaced.Remove(Handle);
            if (!Error.IsEmpty()) Test->AddInfo(Error);
            Actor = Find(PlayWorld, TEXT("ProbeInstance"));
            Holder = Find(PlayWorld, TEXT("ReferenceHolder"));
            auto* EditorActor = Find(GEditor->GetEditorWorldContext().World(), TEXT("ProbeInstance"));
            Check(bReplaced, TEXT("PIE_instance_reinstanced"));
            Check(Actor && Actor->HasActorBegunPlay(), TEXT("BeginPlay_state_preserved"));
            Check(Actor && Actor->GetClass() == State->Child->GeneratedClass, TEXT("PIE_current_class"));
            Check(Actor && Float(Actor, TEXT("speed")) == 73.f, TEXT("PIE_override_preserved"));
            Check(EditorActor && Float(EditorActor, TEXT("speed")) == 37.f, TEXT("editor_override_isolated"));
            Check(Actor && Int(Actor, TEXT("health")) == 100 && Evaluate(Actor) == 42, TEXT("PIE_new_field_and_method"));
            Check(Actor && Holder && EditorActor && Holder->Peer == Actor && Actor->Peer == Holder && Actor->GetWorld() != EditorActor->GetWorld(),
                TEXT("PIE_references_stay_in_world"));
            Check(Actor && Actor->GetActorLocation().Equals(FVector(11,22,33)), TEXT("PIE_transform"));
            CollectGarbage(RF_NoFlags);
            Actor = Find(PlayWorld, TEXT("ProbeInstance"));
            Check(Actor && Float(Actor, TEXT("speed")) == 73.f && Evaluate(Actor) == 42, TEXT("PIE_survives_GC"));
            Check(Actor && Actor->ExecuteDas(TEXT("self_properties.das"), 5) == 105 &&
                Float(Actor, TEXT("speed")) == 74.f && Int(Actor, TEXT("health")) == 105,
                TEXT("script_self_in_PIE_after_reload"));
            Check(EditorActor && Float(EditorActor, TEXT("speed")) == 37.f && Int(EditorActor, TEXT("health")) == 100,
                TEXT("script_writes_do_not_cross_worlds"));
            auto& PersistentRuntime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
            Check(Actor && Holder && EditorActor && Actor->ExecuteDas(TEXT("persistent_counter.das"), 1) == 1 &&
                Actor->ExecuteDas(TEXT("persistent_counter.das"), 1) == 2 && Holder->ExecuteDas(TEXT("persistent_counter.das"), 1) == 1 &&
                EditorActor->ExecuteDas(TEXT("persistent_counter.das"), 1) == 1, TEXT("persistent_actor_and_world_isolation"));
            const FString LogicPath = FPaths::ProjectSavedDir() / TEXT("DasPersistentPIE_") + FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT(".das");
            FString Logic;
            Check(FFileHelper::LoadFileToString(Logic, *PersistentRuntime.ScriptPath(TEXT("persistent_counter.das"))), TEXT("load_logic_fixture"));
            auto WriteLogic = [&](const FString& Text) { return FFileHelper::SaveStringToFile(Text, *LogicPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM); };
            Check(WriteLogic(Logic), TEXT("write_PIE_logic_v1"));
            auto CallLogic = [&](UObject* Self) { return PersistentRuntime.RunPersistent(LogicPath, 1, Self); };
            Check(CallLogic(Actor).Value == 1 && CallLogic(Actor).Value == 2 && CallLogic(Holder).Value == 1, TEXT("PIE_two_counters_before_reload"));
            Check(WriteLogic(Logic.Replace(TEXT("return count //"), TEXT("return count + 100 //"))), TEXT("write_PIE_logic_v2"));
            Check(CallLogic(Actor).Value == 103 && CallLogic(Holder).Value == 102, TEXT("PIE_logic_reload_preserves_two_counters"));
            Check(WriteLogic(TEXT("invalid script")), TEXT("write_PIE_broken_logic"));
            const auto Fallback = CallLogic(Actor);
            Check(Fallback.bSucceeded && Fallback.Value == 104 && !Fallback.ReloadError.IsEmpty(), TEXT("PIE_broken_logic_falls_back"));
            IFileManager::Get().Delete(*LogicPath);
            FActorSpawnParameters DestroySpawn;
            auto* Doomed = PlayWorld->SpawnActor<ADasProbeActor>(State->Child->GeneratedClass, FTransform::Identity, DestroySpawn);
            TWeakObjectPtr<UObject> DestroyedSelf(Doomed);
            Check(Doomed && Doomed->ExecuteDas(TEXT("persistent_counter.das"), 1) == 1, TEXT("doomed_actor_has_context"));
            const int32 BeforeDestroy = PersistentRuntime.PersistentContextCount(PlayWorld);
            Check(Doomed && Doomed->Destroy(), TEXT("actor_destroyed_for_handle_test"));
            Check(PersistentRuntime.PersistentContextCount(PlayWorld) == BeforeDestroy - 1, TEXT("destroy_releases_context_immediately"));
            auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
            const auto DeadCall = Runtime.RunFile(Runtime.ScriptPath(TEXT("self_properties.das")), 1, false, DestroyedSelf);
            Check(!DeadCall.bSucceeded && DeadCall.Error.Contains(TEXT("self is missing or expired")), TEXT("destroyed_actor_self_rejected"));
            GEditor->RequestEndPlayMap();
            Started = FPlatformTime::Seconds();
            bEnding = true;
            return false;
        }
    };
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasPIEReloadTest, "DasForUE.Reload.PIE", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDasPIEReloadTest::RunTest(const FString& Parameters)
{
    auto State = MakeShared<FDasPIEState>();
    UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
    if (!TestNotNull(TEXT("Editor world"), World)) return false;
    const FString Root = TEXT("/Game/DasReloadProbe/PIE_") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
    State->Parent.Reset(Blueprint(Root + TEXT("/BP_DasGenerated"), ADasProbeActor::StaticClass()));
    FString Error;
    if (!TestTrue(TEXT("Parent schema"), State->Parent.IsValid() && ApplySchema(State->Parent.Get(), TEXT("schema_v1.das"), Error))) return false;
    State->Child.Reset(Blueprint(Root + TEXT("/BP_Child"), State->Parent->GeneratedClass));
    if (!TestTrue(TEXT("Child class"), State->Child.IsValid() && Compile(State->Child.Get()))) return false;
    SetFloat(State->Child->GeneratedClass->GetDefaultObject(), TEXT("speed"), 20.f);
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("ProbeInstance");
    auto* Actor = World->SpawnActor<ADasProbeActor>(State->Child->GeneratedClass, FTransform(FVector(11,22,33)), Spawn);
    Spawn.Name = TEXT("ReferenceHolder");
    auto* Holder = World->SpawnActor<ADasProbeActor>(ADasProbeActor::StaticClass(), FTransform::Identity, Spawn);
    if (!TestTrue(TEXT("Editor actors"), Actor && Holder)) return false;
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
        FDasLifecyclePIECommand(TSharedRef<FDasLifecyclePIEState> InState, FAutomationTestBase* InTest) : State(InState), Test(InTest) {}
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
                if (GEditor->PlayWorld && FPlatformTime::Seconds() - Started < 30) return false;
                Check(!GEditor->PlayWorld, TEXT("PIE_stopped"));
                Check(State->StopObserver->Calls == 1 && State->StopObserver->Reason == EEndPlayReason::EndPlayInEditor &&
                    State->StopObserver->Health == 201 + static_cast<int32>(EEndPlayReason::EndPlayInEditor), TEXT("EndPIE_callback_once_with_self_and_reason"));
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
                if (FPlatformTime::Seconds() - Started < 60) return false;
                Check(false, TEXT("lifecycle_actors_started_before_timeout"));
                GEditor->RequestEndPlayMap(); Phase = 4; Started = FPlatformTime::Seconds(); return false;
            }
            auto Call = [&](ADasProbeActor* Actor, int32 Input) { return Runtime.RunPersistent(State->Path, Input, Actor); };
            const auto Ticks = Call(First, 1);
            if (Ticks.Value <= TicksBefore && FPlatformTime::Seconds() - Started < 30) return false;
            Check(Ticks.bSucceeded && Ticks.Value > TicksBefore, TEXT("UE_delivered_real_ticks"));
            if (Phase == 0)
            {
                Check(Call(First, 0).Value == 1 && Call(Second, 0).Value == 1 && Int(First, TEXT("health")) == 101 &&
                    Int(Second, TEXT("health")) == 101, TEXT("UE_delivered_BeginPlay_once_per_actor"));
                Check(Float(First, TEXT("speed")) > 10.f, TEXT("tick_delta_reaches_script"));
                auto* EditorActor = Find(GEditor->GetEditorWorldContext().World(), TEXT("LifecycleFirst"));
                Check(EditorActor && Int(EditorActor, TEXT("health")) == 100 && Float(EditorActor, TEXT("speed")) == 10.f &&
                    Runtime.PersistentContextCount(GEditor->GetEditorWorldContext().World()) == 0, TEXT("editor_world_does_not_receive_play_callbacks"));
                First->OnEndPlay.AddDynamic(State->DestroyObserver.Get(), &UDasLifecycleTestObserver::OnEnded);
                Second->OnEndPlay.AddDynamic(State->StopObserver.Get(), &UDasLifecycleTestObserver::OnEnded);
                const FString V2 = State->Source.Replace(TEXT("if (input == 2) { return 1 }"), TEXT("if (input == 2) { return 2 }"));
                Check(FFileHelper::SaveStringToFile(V2, *State->Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM), TEXT("write_callback_reload"));
                TicksBefore = Ticks.Value; Phase = 1; Started = FPlatformTime::Seconds(); return false;
            }
            if (Phase == 1)
            {
                Check(Call(First, 2).Value == 2 && Call(Second, 2).Value == 2 && Call(First, 0).Value == 1 &&
                    Call(Second, 0).Value == 1 && Int(First, TEXT("health")) == 101, TEXT("hot_reload_keeps_state_without_repeating_begin"));
                Check(FFileHelper::SaveStringToFile(TEXT("invalid script"), *State->Path), TEXT("write_invalid_callback_source"));
                TicksBefore = Ticks.Value; Phase = 2; Started = FPlatformTime::Seconds(); return false;
            }
            if (Phase == 2)
            {
                Check(!First->LastDasError.IsEmpty() && Call(First, 2).Value == 2 && Call(First, 0).Value == 1,
                    TEXT("real_ticks_continue_on_last_good_code"));
                const int32 Before = Runtime.PersistentContextCount(World);
                Check(First->Destroy(), TEXT("destroy_lifecycle_actor"));
                Check(State->DestroyObserver->Calls == 1 && State->DestroyObserver->Reason == EEndPlayReason::Destroyed &&
                    State->DestroyObserver->Health == 201, TEXT("Destroy_calls_end_before_context_release"));
                Check(Runtime.PersistentContextCount(World) == Before - 1, TEXT("Destroy_releases_lifecycle_context"));
                GEditor->RequestEndPlayMap(); Phase = 4; Started = FPlatformTime::Seconds(); return false;
            }
            return false;
        }
    };
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasLifecyclePIETest, "DasForUE.Lifecycle.PIE", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasLifecyclePIETest::RunTest(const FString& Parameters)
{
    auto State = MakeShared<FDasLifecyclePIEState>();
    UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
    if (!TestNotNull(TEXT("Lifecycle editor world"), World)) return false;
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString Root = TEXT("/Game/DasReloadProbe/Lifecycle_") + Id;
    State->Blueprint.Reset(Blueprint(Root + TEXT("/BP_Lifecycle"), ADasProbeActor::StaticClass()));
    FString Error;
    if (!TestTrue(TEXT("Lifecycle schema"), State->Blueprint.IsValid() && ApplySchema(State->Blueprint.Get(), TEXT("schema_v2.das"), Error))) return false;
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    State->Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / (TEXT("Lifecycle_") + Id + TEXT(".das")));
    if (!TestTrue(TEXT("Read lifecycle fixture"), FFileHelper::LoadFileToString(State->Source, *Runtime.ScriptPath(TEXT("lifecycle_actor.das"))))) return false;
    State->Source = State->Source.Replace(TEXT("if (input == 0)"), TEXT("if (input == 2) { return 1 }\n    if (input == 0)"));
    if (!TestTrue(TEXT("Write lifecycle fixture"), FFileHelper::SaveStringToFile(State->Source, *State->Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))) return false;
    for (const TCHAR* Name : {TEXT("LifecycleFirst"), TEXT("LifecycleSecond")})
    {
        FActorSpawnParameters Spawn; Spawn.Name = Name;
        auto* Actor = World->SpawnActor<ADasProbeActor>(State->Blueprint->GeneratedClass, FTransform::Identity, Spawn);
        if (!TestNotNull(TEXT("Lifecycle actor"), Actor)) return false;
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
        FDasDelegatePIECommand(TSharedRef<FDasDelegatePIEState> InState, FAutomationTestBase* InTest) : State(InState), Test(InTest) {}
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
                if (GEditor->PlayWorld && FPlatformTime::Seconds() - Started < 30) return false;
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
                if (FPlatformTime::Seconds() - Started < 60) return false;
                Check(false, TEXT("delegate_actors_started"));
                GEditor->RequestEndPlayMap(); Phase = 1; Started = FPlatformTime::Seconds(); return false;
            }
            auto Call = [&](ADasProbeActor* Actor) { return Runtime.RunPersistent(State->Path, 0, Actor); };
            Check(Runtime.SubscriptionCount(First) == 1 && Runtime.SubscriptionCount(Second) == 1, TEXT("BeginPlay_installs_two_peer_bindings"));
            Source->EmitDasSignal(3);
            Check(Call(First).Value == 3 && Call(Second).Value == 3, TEXT("one_source_delivers_to_two_independent_actors"));
            auto* EditorSource = Find(GEditor->GetEditorWorldContext().World(), TEXT("SignalSource"));
            Check(EditorSource && !EditorSource->OnDasSignal.IsBound() && First->Peer == Source && Second->Peer == Source,
                TEXT("PIE_sources_are_remapped_and_isolated_from_editor"));
            const FString V2 = State->Source.Replace(TEXT("+ value)"), TEXT("+ value * 10)"));
            Check(FFileHelper::SaveStringToFile(V2, *State->Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM), TEXT("write_delegate_reload"));
            Call(First); Call(Second);
            Source->EmitDasSignal(2);
            Check(Call(First).Value == 23 && Call(Second).Value == 23 && Source->OnDasSignal.GetAllObjects().Num() == 2,
                TEXT("reload_replaces_handlers_without_duplicate_delivery"));
            Check(FFileHelper::SaveStringToFile(TEXT("invalid script"), *State->Path), TEXT("write_invalid_delegate_candidate"));
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
            Check(FFileHelper::SaveStringToFile(Nested, *State->Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM), TEXT("write_nested_event_fixture"));
            Call(Second);
            Check(!Source->OnDasSignal.IsBound() && Second->OnDasSignal.GetAllObjects().Num() == 1, TEXT("reload_can_change_subscription_source"));
            Second->EmitDasSignal(1);
            Check(Call(Second).Value == 46 && Second->LastDasError.IsEmpty(), TEXT("nested_events_are_queued_without_VM_reentry"));
            const FString DestroyDuringCall = Nested.Replace(
                TEXT("def main(input:int):int { return ue_state_get_int(\"total\", 0) }"),
                TEXT("def main(input:int):int { ue_emit_signal(99); return 42 }"));
            Check(FFileHelper::SaveStringToFile(DestroyDuringCall, *State->Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM), TEXT("write_destroy_during_VM_fixture"));
            Check(Call(Source).Value == 42, TEXT("source_context_ready_for_reentrant_destroy"));
            TStrongObjectPtr<UDasDelegateDestroyer> Destroyer(NewObject<UDasDelegateDestroyer>());
            Destroyer->Target = Source;
            Source->OnDasSignal.AddDynamic(Destroyer.Get(), &UDasDelegateDestroyer::OnSignal);
            const auto LastCall = Call(Source);
            Check(LastCall.bSucceeded && LastCall.Value == 42 && Destroyer->Calls == 1 && Source->IsActorBeingDestroyed() &&
                Runtime.SubscriptionCount() == 1 && Runtime.PersistentContextCount(World) == 1,
                TEXT("destroy_inside_VM_defers_context_free_and_revokes_callbacks"));

            GEditor->RequestEndPlayMap(); Phase = 1; Started = FPlatformTime::Seconds(); return false;
        }
    };
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasDelegatePIETest, "DasForUE.Delegates.PIE", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasDelegatePIETest::RunTest(const FString& Parameters)
{
    auto State = MakeShared<FDasDelegatePIEState>();
    UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
    if (!TestNotNull(TEXT("Delegate editor world"), World)) return false;
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    State->Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / (TEXT("Delegates_") + FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT(".das")));
    if (!TestTrue(TEXT("Read delegate fixture"), FFileHelper::LoadFileToString(State->Source, *Runtime.ScriptPath(TEXT("delegate_actor.das"))))) return false;
    if (!TestTrue(TEXT("Write delegate fixture"), FFileHelper::SaveStringToFile(State->Source, *State->Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))) return false;
    FActorSpawnParameters Spawn; Spawn.Name = TEXT("SignalSource");
    auto* Source = World->SpawnActor<ADasProbeActor>(ADasProbeActor::StaticClass(), FTransform::Identity, Spawn);
    if (!TestNotNull(TEXT("Event source"), Source)) return false;
    for (const TCHAR* Name : {TEXT("SignalFirst"), TEXT("SignalSecond")})
    {
        Spawn.Name = Name;
        auto* Actor = World->SpawnActor<ADasProbeActor>(ADasProbeActor::StaticClass(), FTransform::Identity, Spawn);
        if (!TestNotNull(TEXT("Event listener"), Actor)) return false;
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
        FDasObjectsPIECommand(TSharedRef<FDasObjectsPIEState> InState, FAutomationTestBase* InTest) : State(InState), Test(InTest) {}
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
                if (GEditor->PlayWorld && FPlatformTime::Seconds() - Started < 30) return false;
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
                if (FPlatformTime::Seconds() - Started < 60) return false;
                Check(false, TEXT("object_actors_started"));
                GEditor->RequestEndPlayMap(); bEnding = true; Started = FPlatformTime::Seconds(); return false;
            }
            auto Call = [&](int32 Input) { return Runtime.RunPersistent(State->Path, Input, Listener); };
            const auto Echo = Call(0);
            Check(Echo.bSucceeded && Echo.Value == 40 && Source->FindFunction(TEXT("Echo")) &&
                !Source->FindFunction(TEXT("Echo"))->HasAnyFunctionFlags(FUNC_Native), TEXT("ProcessEvent_executes_Blueprint_function"));
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
            Check(TickGroup.bSucceeded && Source->PrimaryActorTick.TickGroup == TG_PostPhysics, TEXT("generic_enum_calls_engine_SetTickGroup"));
            Source->Tags.Add(TEXT("DasEnumTag_17"));
            const auto Tag = Call(13);
            Check(Tag.bSucceeded && Tag.Value == 1, TEXT("generic_FName_calls_engine_ActorHasTag"));
            const auto NameOutput = Call(14);
            Check(NameOutput.bSucceeded && NameOutput.Value == 1, TEXT("generic_FName_roundtrips_Blueprint_output"));
            Check(Listener->AttachToActor(Source, FAttachmentTransformRules::KeepWorldTransform), TEXT("attach_actor_for_generated_object_array"));
            const auto GeneratedActor = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_actor.das")), 0, false, Source);
            Check(GeneratedActor.bSucceeded && GeneratedActor.Value == 123 && Source->PrimaryActorTick.TickGroup == TG_PrePhysics,
                TEXT("generated_Actor_wrappers_execute_in_PIE"));
            TWeakObjectPtr<UObject> WeakSource(Source);
            Check(Source->Destroy(), TEXT("destroy_cached_handle_target"));
            const auto Invalid = Call(3);
            const auto Expired = Call(4);
            Check(Invalid.bSucceeded && Invalid.Value == 0 && !Expired.bSucceeded && Expired.Error.Contains(TEXT("expired")),
                TEXT("cached_Actor_handle_rejected_immediately_after_Destroy"));
            CollectGarbage(RF_NoFlags);
            Check(!WeakSource.IsValid() && Runtime.SubscriptionCount(Listener) == 0 && Call(3).Value == 0,
                TEXT("dead_source_not_pinned_and_subscription_pruned"));
            GEditor->RequestEndPlayMap(); bEnding = true; Started = FPlatformTime::Seconds(); return false;
        }
    };
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasObjectsPIETest, "DasForUE.Objects.PIE", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasObjectsPIETest::RunTest(const FString& Parameters)
{
    auto State = MakeShared<FDasObjectsPIEState>();
    UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
    if (!TestNotNull(TEXT("Objects editor world"), World)) return false;
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    State->Blueprint.Reset(Blueprint(TEXT("/Game/DasReloadProbe/Objects_") + Id + TEXT("/BP_Echo"), ADasProbeActor::StaticClass()));
    if (!TestNotNull(TEXT("Echo Blueprint"), State->Blueprint.Get())) return false;
    auto* BP = State->Blueprint.Get();
    auto* Graph = FBlueprintEditorUtils::CreateNewGraph(BP, TEXT("Echo"), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
    FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP, Graph, true, nullptr);
    TArray<UK2Node_FunctionEntry*> Entries; Graph->GetNodesOfClass(Entries);
    if (!TestTrue(TEXT("Echo entry"), Entries.Num() == 1)) return false;
    FEdGraphPinType IntType; IntType.PinCategory = UEdGraphSchema_K2::PC_Int;
    auto* InputPin = Entries[0]->CreateUserDefinedPin(TEXT("Input"), IntType, EGPD_Output);
    FGraphNodeCreator<UK2Node_FunctionResult> Creator(*Graph);
    auto* Result = Creator.CreateNode(); Creator.Finalize();
    auto* ReturnPin = Result->CreateUserDefinedPin(UEdGraphSchema_K2::PN_ReturnValue, IntType, EGPD_Input);
    const auto* Schema = Graph->GetSchema();
    if (!TestTrue(TEXT("Echo links"), Schema->TryCreateConnection(InputPin, ReturnPin) &&
        Schema->TryCreateConnection(Entries[0]->FindPinChecked(UEdGraphSchema_K2::PN_Then), Result->FindPinChecked(UEdGraphSchema_K2::PN_Execute)))) return false;
    auto* OutGraph = FBlueprintEditorUtils::CreateNewGraph(BP, TEXT("CopyOut"), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
    FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP, OutGraph, true, nullptr);
    TArray<UK2Node_FunctionEntry*> OutEntries; OutGraph->GetNodesOfClass(OutEntries);
    if (!TestTrue(TEXT("Out entry"), OutEntries.Num() == 1)) return false;
    auto* OutInput = OutEntries[0]->CreateUserDefinedPin(TEXT("Input"), IntType, EGPD_Output);
    FGraphNodeCreator<UK2Node_FunctionResult> OutCreator(*OutGraph);
    auto* OutResult = OutCreator.CreateNode(); OutCreator.Finalize();
    auto* OutPin = OutResult->CreateUserDefinedPin(TEXT("Copied"), IntType, EGPD_Input);
    if (!TestTrue(TEXT("Out links"), Schema->TryCreateConnection(OutInput, OutPin) &&
        Schema->TryCreateConnection(OutEntries[0]->FindPinChecked(UEdGraphSchema_K2::PN_Then), OutResult->FindPinChecked(UEdGraphSchema_K2::PN_Execute)))) return false;
    auto* NameGraph = FBlueprintEditorUtils::CreateNewGraph(BP, TEXT("CopyName"), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
    FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP, NameGraph, true, nullptr);
    TArray<UK2Node_FunctionEntry*> NameEntries; NameGraph->GetNodesOfClass(NameEntries);
    if (!TestTrue(TEXT("Name entry"), NameEntries.Num() == 1)) return false;
    FEdGraphPinType NameType; NameType.PinCategory = UEdGraphSchema_K2::PC_Name;
    auto* NameInput = NameEntries[0]->CreateUserDefinedPin(TEXT("NameIn"), NameType, EGPD_Output);
    FGraphNodeCreator<UK2Node_FunctionResult> NameCreator(*NameGraph);
    auto* NameResult = NameCreator.CreateNode(); NameCreator.Finalize();
    auto* NameOutput = NameResult->CreateUserDefinedPin(TEXT("NameOut"), NameType, EGPD_Input);
    if (!TestTrue(TEXT("Name links"), Schema->TryCreateConnection(NameInput, NameOutput) &&
        Schema->TryCreateConnection(NameEntries[0]->FindPinChecked(UEdGraphSchema_K2::PN_Then), NameResult->FindPinChecked(UEdGraphSchema_K2::PN_Execute)))) return false;
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
    if (!TestTrue(TEXT("Echo compile"), Compile(BP))) return false;
    State->Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / (TEXT("Objects_") + Id + TEXT(".das")));
    const FString Script = TEXT("options gen2\nrequire ue_foundation\nvar cached = ue_null_object()\n[export]\ndef bind_events() { ue_subscribe_object_int(ue_get_object(ue_self_object(), \"Peer\"), \"OnDasSignal\", \"on_value\") }\n[export]\ndef on_value(value:int) { ue_state_set_int(\"sum\", ue_state_get_int(\"sum\", 0) + value) }\n[export]\ndef main(input:int):int { if (input == 0) { cached = ue_cast_object(ue_get_object(ue_self_object(), \"Peer\"), \"/Script/Engine.Actor\"); return ue_call_int(cached, \"Echo\", 40) }; if (input == 1) { ue_call_void_int(cached, \"EmitDasSignal\", 3); return 0 }; if (input == 2) { return ue_state_get_int(\"sum\", 0) }; if (input == 3) { if (ue_object_valid(cached)) { return 1 }; return 0 }; if (input == 7) { let c = ue_begin_call(cached, \"Echo\"); ue_arg_int(c, \"Input\", 42); ue_invoke_call(c); let v = ue_result_int(c, \"ReturnValue\"); ue_release_call(c); return v }; if (input == 8) { let c = ue_begin_call(cached, \"K2_GetActorLocation\"); ue_invoke_call(c); let x = ue_result_double(c, \"ReturnValue.X\"); ue_release_call(c); return int(x) }; if (input == 9) { let c = ue_begin_call(ue_self_object(), \"SetOwner\"); ue_arg_object(c, \"NewOwner\", cached); ue_invoke_call(c); return 0 }; if (input == 10) { let c = ue_begin_call(ue_self_object(), \"GetOwner\"); ue_invoke_call(c); let obj = ue_result_object(c, \"ReturnValue\"); if (ue_object_is_a(obj, \"/Script/Engine.Actor\")) { return 1 }; return 0 }; if (input == 11) { let c = ue_begin_call(cached, \"CopyOut\"); ue_arg_int(c, \"Input\", 77); ue_invoke_call(c); return ue_result_int(c, \"Copied\") }; if (input == 12) { let c = ue_begin_call(cached, \"SetTickGroup\"); ue_arg_enum(c, \"NewTickGroup\", \"/Script/Engine.ETickingGroup\", \"TG_PostPhysics\"); ue_invoke_call(c); return 1 }; if (input == 13) { let c = ue_begin_call(cached, \"ActorHasTag\"); ue_arg_name(c, \"Tag\", \"dasenumtag_17\"); ue_invoke_call(c); if (ue_result_bool(c, \"ReturnValue\")) { return 1 }; return 0 }; if (input == 14) { let c = ue_begin_call(cached, \"CopyName\"); ue_arg_name(c, \"NameIn\", \"BlueprintName_23\"); ue_invoke_call(c); if (ue_result_name(c, \"NameOut\") == \"BlueprintName_23\") { return 1 }; return 0 }; if (input == 5) { ue_call_void_object(ue_self_object(), \"SetOwner\", cached); return 0 }; if (input == 6) { let owner = ue_call_object(ue_self_object(), \"GetOwner\"); if (ue_object_is_a(owner, \"/Script/Engine.Actor\")) { return 1 }; return 0 }; return ue_call_int(cached, \"Echo\", 40) }\n");
    if (!TestTrue(TEXT("Write objects fixture"), FFileHelper::SaveStringToFile(Script, *State->Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))) return false;
    FActorSpawnParameters Spawn; Spawn.Name = TEXT("ObjectSource");
    auto* Source = World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
    Spawn.Name = TEXT("ObjectListener");
    auto* Listener = World->SpawnActor<ADasProbeActor>(ADasProbeActor::StaticClass(), FTransform::Identity, Spawn);
    if (!TestTrue(TEXT("Objects actors"), Source && Listener)) return false;
    Listener->Peer = Source;
    Listener->LifecycleScript = State->Path;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasObjectsPIECommand>(State, this));
    return true;
}

#endif


#include "DasDSLService.h"
#include "HAL/IConsoleManager.h"
namespace {
struct FDasDSLPIEState {
    TStrongObjectPtr<UBlueprint> Blueprint;
    FString ScriptName,ScriptPath,Source1,Source2;
    bool Passed=true;
};
class FDasDSLPIECommand final : public IAutomationLatentCommand {
    TSharedRef<FDasDSLPIEState> S;FAutomationTestBase* Test;double Start=0;bool Ending=false;
public:
    FDasDSLPIECommand(TSharedRef<FDasDSLPIEState> State,FAutomationTestBase* T):S(State),Test(T){}
    virtual bool Update() override {
        if(Start==0) Start=FPlatformTime::Seconds();
        auto Check=[&](bool Value,const TCHAR* Name){S->Passed &= Test->TestTrue(Name,Value);UE_LOG(LogTemp,Display,TEXT("DAS_DSL_PIE %s %s"),Value?TEXT("PASS"):TEXT("FAIL"),Name);};
        auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        if(Ending){
            if(GEditor->PlayWorld && FPlatformTime::Seconds()-Start<30) return false;
            Check(!GEditor->PlayWorld,TEXT("EndPIE"));
            Check(Runtime.PersistentContextCount()==Runtime.PersistentContextCount(GEditor->GetEditorWorldContext().World()),TEXT("EndPIE_releases_play_contexts"));
            IFileManager::Get().Delete(*S->ScriptPath);
            UE_LOG(LogTemp,Display,TEXT("DAS_DSL_PIE_RESULT=%s"),S->Passed?TEXT("PASS"):TEXT("FAIL"));return true;
        }
        UWorld* World=GEditor->PlayWorld;
        if(!World || !World->HasBegunPlay()){
            if(FPlatformTime::Seconds()-Start<60) return false;
            Check(false,TEXT("PIE_started"));GEditor->RequestEndPlayMap();Ending=true;Start=FPlatformTime::Seconds();return false;
        }
        auto* Actor=Find(World,TEXT("DSLActor"));auto* Holder=Find(World,TEXT("DSLHolder"));
        Check(Actor && Actor->HasActorBegunPlay() && Evaluate(Actor)==73,TEXT("native_DSL_method_in_real_PIE"));
        if(!Actor){GEditor->RequestEndPlayMap();Ending=true;Start=FPlatformTime::Seconds();return false;}
        FindFProperty<FIntProperty>(Actor->GetClass(),TEXT("Score"))->SetPropertyValue_InContainer(Actor,77);
        auto Write=[&](const FString& Text){return FFileHelper::SaveStringToFile(Text,*S->ScriptPath,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);};
        auto Call=[&](){return Runtime.RunNamedPersistent(S->ScriptPath,TEXT("ue_Counter_Evaluate"),5,Actor);};
        Check(Call().Value==82,TEXT("PIE_override_before_reload"));
        const FString Body=S->Source1.Replace(TEXT("self.Score + input"),TEXT("self.Score + input + 100"));
        Check(Write(Body),TEXT("write_body_change"));
        const auto Reload=Call();Check(Reload.bSucceeded && Reload.Value==182 && Reload.ReloadError.IsEmpty(),TEXT("body_auto_reload_without_reinstance"));
        UClass* BeforeClass=Actor->GetClass();UObject* BeforeActor=Actor;FString Error;
        Check(FDasDSLService::Reimport(S->Blueprint.Get(),Error) && Find(World,TEXT("DSLActor"))==BeforeActor && Actor->GetClass()==BeforeClass,TEXT("body_only_reimport_preserves_actor_identity"));
        Check(Write(TEXT("broken DSL")),TEXT("write_invalid_DSL"));
        Check(!FDasDSLService::Reimport(S->Blueprint.Get(),Error) && Actor->GetClass()==BeforeClass,TEXT("invalid_reimport_no_Blueprint_mutation"));
        auto Fallback=Call();Check(Fallback.bSucceeded && Fallback.Value==182 && !Fallback.ReloadError.IsEmpty(),TEXT("broken_DSL_keeps_last_good_body"));
        Check(Write(Body) && Call().ReloadError.IsEmpty(),TEXT("body_reload_recovers"));
        Check(Write(S->Source2),TEXT("write_structural_DSL_change"));
        Fallback=Call();Check(Fallback.bSucceeded && Fallback.Value==182 && Fallback.ReloadError.Contains(TEXT("apply/reimport")),TEXT("structural_candidate_waits_for_Unreal_apply"));
        bool Replaced=false;
        auto Handle=FCoreUObjectDelegates::OnObjectsReplaced.AddLambda([&](const FCoreUObjectDelegates::FReplacementObjectMap& Map){Replaced |= Map.Contains(BeforeActor);});
        Check(FDasDSLService::Reimport(S->Blueprint.Get(),Error),TEXT("structural_reimport_during_PIE"));
        FCoreUObjectDelegates::OnObjectsReplaced.Remove(Handle);
        if(!Error.IsEmpty()) Test->AddInfo(Error);
        Actor=Find(World,TEXT("DSLActor"));Holder=Find(World,TEXT("DSLHolder"));
        auto* EditorActor=Find(GEditor->GetEditorWorldContext().World(),TEXT("DSLActor"));
        Check(Replaced && Actor && Actor->HasActorBegunPlay() && Actor->GetClass()==S->Blueprint->GeneratedClass,TEXT("PIE_actor_reinstanced_with_BeginPlay_state"));
        Check(Actor && Int(Actor,TEXT("Score"))==77 && Int(Actor,TEXT("Bonus"))==5 && Evaluate(Actor)==222,TEXT("new_field_and_body_preserve_override"));
        Check(EditorActor && Int(EditorActor,TEXT("Score"))==33 && Int(EditorActor,TEXT("Bonus"))==5,TEXT("editor_world_override_isolated"));
        Check(Actor && Holder && Holder->Peer==Actor && Actor->Peer==Holder,TEXT("references_preserved"));
        Check(Actor && Call().Value==187,TEXT("new_declaration_runtime_accepted"));
        BeforeClass=Actor?Actor->GetClass():nullptr;
        Check(Write(S->Source2.Replace(TEXT("Score : int = 10"),TEXT("Score : int = 999"))),TEXT("write_unsupported_default_change"));
        Check(!FDasDSLService::Reimport(S->Blueprint.Get(),Error) && Actor && Actor->GetClass()==BeforeClass,TEXT("unsupported_migration_rejected"));
        Fallback=Call();Check(Fallback.bSucceeded && Fallback.Value==187 && Fallback.ReloadError.Contains(TEXT("apply/reimport")),TEXT("rejected_default_change_keeps_last_good_body"));
        Check(Write(S->Source2) && Call().ReloadError.IsEmpty(),TEXT("restore_last_good_source"));
        CollectGarbage(RF_NoFlags);Actor=Find(World,TEXT("DSLActor"));
        Check(Actor && Int(Actor,TEXT("Score"))==77 && Evaluate(Actor)==222,TEXT("reimport_survives_GC"));
        GEditor->RequestEndPlayMap();Ending=true;Start=FPlatformTime::Seconds();return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasDSLPIETest,"DasForUE.DSL.PIE",EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasDSLPIETest::RunTest(const FString& Parameters){
    if(FApp::GetProjectName()!=FString(TEXT("DasForUESmoke"))) {AddError(TEXT("DSL probe requires isolated smoke project"));return false;}
    auto S=MakeShared<FDasDSLPIEState>();auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Guid=FGuid::NewGuid().ToString(EGuidFormats::Digits);
    S->ScriptName=TEXT("dsl_pie_")+Guid+TEXT(".das");S->ScriptPath=Runtime.ScriptPath(S->ScriptName);
    if(!FFileHelper::LoadFileToString(S->Source1,*Runtime.ScriptPath(TEXT("dsl_counter_v1.das"))) || !FFileHelper::LoadFileToString(S->Source2,*Runtime.ScriptPath(TEXT("dsl_counter_v2.das")))) return false;
    if(!FFileHelper::SaveStringToFile(S->Source1,*S->ScriptPath,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM)) return false;
    UWorld* World=FAutomationEditorCommonUtils::CreateNewMap();
    const FString Package=TEXT("/Game/DasDSLProbe/Run_")+Guid+TEXT("/BP_Counter");
    IConsoleManager::Get().ProcessUserConsoleInput(*(TEXT("Das.DSL.Apply ")+S->ScriptName+TEXT(" ")+Package+TEXT(" example.counter")),*GLog,World);
    UBlueprint* BP=LoadObject<UBlueprint>(nullptr,*(Package+TEXT(".BP_Counter")));
    if(!TestNotNull(TEXT("console command creates Blueprint"),BP)){IFileManager::Get().Delete(*S->ScriptPath);return false;}S->Blueprint.Reset(BP);
    FString Error;UBlueprint* Ignored=nullptr;
    TestTrue(TEXT("invalid package/source refused"),!FDasDSLService::Apply(TEXT("../bad.das"),Package,TEXT("example.counter"),Ignored,Error));
    IConsoleManager::Get().ProcessUserConsoleInput(*(TEXT("Das.DSL.Reimport ")+Package),*GLog,World);
    FActorSpawnParameters Spawn;Spawn.Name=TEXT("DSLActor");
    auto* Actor=World->SpawnActor<ADasProbeActor>(BP->GeneratedClass,FTransform::Identity,Spawn);
    Spawn.Name=TEXT("DSLHolder");auto* Holder=World->SpawnActor<ADasProbeActor>(ADasProbeActor::StaticClass(),FTransform::Identity,Spawn);
    if(!Actor || !Holder){IFileManager::Get().Delete(*S->ScriptPath);return false;}
    FindFProperty<FIntProperty>(Actor->GetClass(),TEXT("Score"))->SetPropertyValue_InContainer(Actor,33);Holder->Peer=Actor;Actor->Peer=Holder;
    // PIE duplicates the editor override; later writes exercise world isolation.
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasDSLPIECommand>(S,this));return true;
}


#include "DasAuthoring.h"
namespace {
class FDasScalarPIECommand final : public IAutomationLatentCommand {
    TStrongObjectPtr<UBlueprint> BP;FAutomationTestBase* Test;double Start=0;bool Ending=false,Passed=true;
public:
    FDasScalarPIECommand(UBlueprint* B,FAutomationTestBase* T):BP(B),Test(T){}
    virtual bool Update() override {
        if(Start==0) Start=FPlatformTime::Seconds();
        auto Check=[&](bool V,const TCHAR* N){Passed &= Test->TestTrue(N,V);UE_LOG(LogTemp,Display,TEXT("DAS_SCALAR_PIE %s %s"),V?TEXT("PASS"):TEXT("FAIL"),N);};
        if(Ending){if(GEditor->PlayWorld && FPlatformTime::Seconds()-Start<30) return false;Check(!GEditor->PlayWorld,TEXT("EndPIE"));UE_LOG(LogTemp,Display,TEXT("DAS_SCALAR_PIE_RESULT=%s"),Passed?TEXT("PASS"):TEXT("FAIL"));return true;}
        UWorld* World=GEditor->PlayWorld;
        if(!World || !World->HasBegunPlay()){if(FPlatformTime::Seconds()-Start<60) return false;Check(false,TEXT("PIE_started"));GEditor->RequestEndPlayMap();Ending=true;Start=FPlatformTime::Seconds();return false;}
        auto* Actor=Find(World,TEXT("ScalarActor"));
        Check(Actor && Int(Actor,TEXT("health"))==10 && Float(Actor,TEXT("speed"))==1.5f,TEXT("scalar_defaults"));
        auto* InitialBool=Actor?FindFProperty<FBoolProperty>(Actor->GetClass(),TEXT("bScalar")):nullptr;
        Check(InitialBool && InitialBool->GetPropertyValue_InContainer(Actor),TEXT("bool_true_Blueprint_default"));
        if(!Actor){GEditor->RequestEndPlayMap();Ending=true;Start=FPlatformTime::Seconds();return false;}
        auto Invoke=[&](const TCHAR* Name,auto SetInput,auto GetResult){
            UFunction* Method=Actor->FindFunction(Name);if(!Method) return false;
            FStructOnScope Frame(Method);if(!SetInput(Method,Frame.GetStructMemory())) return false;
            FEditorScriptExecutionGuard Guard;Actor->ProcessEvent(Method,Frame.GetStructMemory());return GetResult(Method,Frame.GetStructMemory());
        };
        Check(Invoke(TEXT("Scale"),[](UFunction* F,uint8* Data){auto* P=FindFProperty<FFloatProperty>(F,TEXT("Input"));if(!P)return false;P->SetPropertyValue_InContainer(Data,2.25f);return true;},[](UFunction* F,uint8* Data){auto* P=CastField<FFloatProperty>(F->GetReturnProperty());return P && P->GetPropertyValue_InContainer(Data)==3.75f;}),TEXT("float_ProcessEvent_input_return"));
        Check(Invoke(TEXT("Toggle"),[](UFunction* F,uint8* Data){auto* P=FindFProperty<FBoolProperty>(F,TEXT("Input"));if(!P)return false;P->SetPropertyValue_InContainer(Data,true);return true;},[](UFunction* F,uint8* Data){auto* P=CastField<FBoolProperty>(F->GetReturnProperty());return P && P->GetPropertyValue_InContainer(Data);}),TEXT("bool_ProcessEvent_input_return"));
        auto* Flag=FindFProperty<FBoolProperty>(Actor->GetClass(),TEXT("bScalar"));
        Check(Flag && Flag->GetPropertyValue_InContainer(Actor),TEXT("bool_reflected_field_write"));
        Check(Invoke(TEXT("Check"),[](UFunction* F,uint8* Data){auto* P=FindFProperty<FIntProperty>(F,TEXT("Input"));if(!P)return false;P->SetPropertyValue_InContainer(Data,5);return true;},[](UFunction* F,uint8* Data){auto* P=CastField<FIntProperty>(F->GetReturnProperty());return P && P->GetPropertyValue_InContainer(Data)==15;}),TEXT("int_method_shares_context"));
        Check(Invoke(TEXT("Reset"),[](UFunction* F,uint8*){return F->NumParms==0;},[](UFunction* F,uint8*){return !F->GetReturnProperty();}) && Int(Actor,TEXT("health"))==0 && Flag && !Flag->GetPropertyValue_InContainer(Actor) && Float(Actor,TEXT("speed"))==3.75f,TEXT("void_action_and_bool_reset"));
        auto* EditorActor=Find(GEditor->GetEditorWorldContext().World(),TEXT("ScalarActor"));
        Check(EditorActor && Int(EditorActor,TEXT("health"))==10 && Float(EditorActor,TEXT("speed"))==1.5f,TEXT("scalar_world_isolation"));
        FString Error;UClass* Before=Actor->GetClass();
        Check(FDasDSLService::Reimport(BP.Get(),Error) && Actor->GetClass()==Before,TEXT("typed_reimport_without_reinstance"));
        auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));FDasAuthoringModule Model;
        if(Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_scalar_methods.das")),Model,Error)){
            auto* M=Model.Classes[0].Methods.FindByPredicate([](const auto& X){return X.Name==TEXT("Scale");});
            if(M){M->ReturnType.Kind=TEXT("int");M->Parameters[0].Type.Kind=TEXT("int");}
            Check(M && !FDasBlueprintService::ApplyAuthoring(BP.Get(),Model,TEXT("typed.fixture"),Error) && BP->GeneratedClass==Before,TEXT("typed_signature_mismatch_no_mutation"));
            if(M){M->ReturnType.Kind=TEXT("float");M->Parameters[0].Type.Kind=TEXT("float");}
            auto* P=Model.Classes[0].Properties.FindByPredicate([](const auto& X){return X.Name==TEXT("bScalar");});if(P) P->Default.Boolean=false;
            Check(P && !FDasBlueprintService::ApplyAuthoring(BP.Get(),Model,TEXT("typed.fixture"),Error) && BP->GeneratedClass==Before,TEXT("bool_default_migration_no_mutation"));
        }else Check(false,TEXT("typed_model_read"));
        CollectGarbage(RF_NoFlags);Actor=Find(World,TEXT("ScalarActor"));
        Check(Actor && Float(Actor,TEXT("speed"))==3.75f && Int(Actor,TEXT("health"))==0,TEXT("typed_state_survives_GC"));
        GEditor->RequestEndPlayMap();Ending=true;Start=FPlatformTime::Seconds();return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasScalarPIETest,"DasForUE.DSL.ScalarsPIE",EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasScalarPIETest::RunTest(const FString& Parameters){
    if(FApp::GetProjectName()!=FString(TEXT("DasForUESmoke"))){AddError(TEXT("Isolated smoke project required"));return false;}
    UWorld* World=FAutomationEditorCommonUtils::CreateNewMap();UBlueprint* BP=nullptr;FString Error;
    const FString Package=TEXT("/Game/DasScalarProbe/Run_")+FGuid::NewGuid().ToString(EGuidFormats::Digits)+TEXT("/BP_Scalar");
    if(!TestTrue(TEXT("Create typed DSL Blueprint"),FDasDSLService::Apply(TEXT("dsl_scalar_methods.das"),Package,TEXT("typed.fixture"),BP,Error))){AddInfo(Error);return false;}
    FActorSpawnParameters Spawn;Spawn.Name=TEXT("ScalarActor");auto* Actor=World->SpawnActor<ADasProbeActor>(BP->GeneratedClass,FTransform::Identity,Spawn);
    if(!TestNotNull(TEXT("Spawn scalar actor"),Actor)) return false;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasScalarPIECommand>(BP,this));return true;
}


namespace {
class FDasFramePIECommand final : public IAutomationLatentCommand {
    TStrongObjectPtr<UBlueprint> BP;FAutomationTestBase* Test;double Start=0;bool Ending=false,Passed=true;
public:
    FDasFramePIECommand(UBlueprint* B,FAutomationTestBase* T):BP(B),Test(T){}
    virtual bool Update() override {
        if(Start==0) Start=FPlatformTime::Seconds();
        auto Check=[&](bool V,const TCHAR* N){Passed &= Test->TestTrue(N,V);UE_LOG(LogTemp,Display,TEXT("DAS_FRAME_PIE %s %s"),V?TEXT("PASS"):TEXT("FAIL"),N);};
        if(Ending){if(GEditor->PlayWorld && FPlatformTime::Seconds()-Start<30) return false;Check(!GEditor->PlayWorld,TEXT("EndPIE"));UE_LOG(LogTemp,Display,TEXT("DAS_FRAME_PIE_RESULT=%s"),Passed?TEXT("PASS"):TEXT("FAIL"));return true;}
        UWorld* World=GEditor->PlayWorld;if(!World || !World->HasBegunPlay()){if(FPlatformTime::Seconds()-Start<60) return false;Check(false,TEXT("PIE_started"));GEditor->RequestEndPlayMap();Ending=true;Start=FPlatformTime::Seconds();return false;}
        auto* Actor=Find(World,TEXT("FrameActor"));Check(Actor!=nullptr,TEXT("real_PIE_actor"));if(!Actor){GEditor->RequestEndPlayMap();Ending=true;Start=FPlatformTime::Seconds();return false;}
        auto Invoke=[&](const TCHAR* Name,auto Input,auto Output){auto* F=Actor->FindFunction(Name);if(!F)return false;FStructOnScope Frame(F);if(!Input(F,Frame.GetStructMemory()))return false;FEditorScriptExecutionGuard Guard;Actor->ProcessEvent(F,Frame.GetStructMemory());return Output(F,Frame.GetStructMemory());};
        auto SetInt=[](UFunction* F,uint8* Data,const TCHAR* Name,int32 V){auto* P=FindFProperty<FIntProperty>(F,Name);if(!P)return false;P->SetPropertyValue_InContainer(Data,V);return true;};
        auto SetBool=[](UFunction* F,uint8* Data,const TCHAR* Name,bool V){auto* P=FindFProperty<FBoolProperty>(F,Name);if(!P)return false;P->SetPropertyValue_InContainer(Data,V);return true;};
        Check(Invoke(TEXT("Compute"),[&](UFunction* F,uint8* D){auto* P=FindFProperty<FFloatProperty>(F,TEXT("factor"));if(!P)return false;P->SetPropertyValue_InContainer(D,0.5f);return SetInt(F,D,TEXT("count"),2) && SetBool(F,D,TEXT("apply"),true);},[](UFunction* F,uint8* D){auto* P=CastField<FFloatProperty>(F->GetReturnProperty());return P && P->GetPropertyValue_InContainer(D)==24.f;}),TEXT("mixed_three_named_inputs_float_result"));
        Check(Invoke(TEXT("IsReady"),[&](UFunction* F,uint8* D){return SetInt(F,D,TEXT("minimum"),11) && SetBool(F,D,TEXT("enabled"),true);},[](UFunction* F,uint8* D){auto* P=CastField<FBoolProperty>(F->GetReturnProperty());return P && P->GetPropertyValue_InContainer(D);}),TEXT("mixed_bool_result"));
        Check(Invoke(TEXT("ReadScore"),[](UFunction* F,uint8*){return F->NumParms==1;},[](UFunction* F,uint8* D){auto* P=CastField<FIntProperty>(F->GetReturnProperty());return P && P->GetPropertyValue_InContainer(D)==12;}),TEXT("zero_input_scalar_result"));
        Check(Invoke(TEXT("BoolAsInt"),[&](UFunction* F,uint8* D){return SetBool(F,D,TEXT("value"),true);},[](UFunction* F,uint8* D){auto* P=CastField<FIntProperty>(F->GetReturnProperty());return P && P->GetPropertyValue_InContainer(D)==12;}),TEXT("single_input_different_result_type"));
        Check(Invoke(TEXT("ResetWith"),[&](UFunction* F,uint8* D){return SetInt(F,D,TEXT("amount"),7) && SetBool(F,D,TEXT("resetGain"),true);},[](UFunction* F,uint8*){return F->NumParms==2 && !F->GetReturnProperty();}) && Int(Actor,TEXT("health"))==7 && Float(Actor,TEXT("speed"))==1.5f,TEXT("mixed_void_action"));
        auto* EditorActor=Find(GEditor->GetEditorWorldContext().World(),TEXT("FrameActor"));Check(EditorActor && Int(EditorActor,TEXT("health"))==10,TEXT("frame_world_isolation"));
        FString Error;UClass* Before=BP->GeneratedClass;Check(FDasDSLService::Reimport(BP.Get(),Error) && BP->GeneratedClass==Before,TEXT("frame_reimport_no_reinstance"));
        auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));FDasAuthoringModule Model;
        if(R.ReadAuthoringScript(R.ScriptPath(TEXT("dsl_mixed_methods.das")),Model,Error)){
            auto* M=Model.Classes[0].Methods.FindByPredicate([](const auto& X){return X.Name==TEXT("Compute");});if(M) M->Parameters.Swap(0,1);
            Check(M && !FDasBlueprintService::ApplyAuthoring(BP.Get(),Model,TEXT("mixed.MixedCounter"),Error) && BP->GeneratedClass==Before,TEXT("frame_parameter_reorder_no_mutation"));
            if(M){M->Parameters.Swap(0,1);M->Parameters[0].Id=TEXT("changed-parameter-id");}
            Check(M && !FDasBlueprintService::ApplyAuthoring(BP.Get(),Model,TEXT("mixed.MixedCounter"),Error) && BP->GeneratedClass==Before,TEXT("frame_parameter_id_change_no_mutation"));
        }else Check(false,TEXT("frame_model_read"));
        CollectGarbage(RF_NoFlags);Actor=Find(World,TEXT("FrameActor"));Check(Actor && Int(Actor,TEXT("health"))==7,TEXT("frame_state_survives_GC"));
        GEditor->RequestEndPlayMap();Ending=true;Start=FPlatformTime::Seconds();return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasFramePIETest,"DasForUE.DSL.FramePIE",EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasFramePIETest::RunTest(const FString& Parameters){
    if(FApp::GetProjectName()!=FString(TEXT("DasForUESmoke"))){AddError(TEXT("Isolated project required"));return false;}
    auto* World=FAutomationEditorCommonUtils::CreateNewMap();UBlueprint* BP=nullptr;FString Error;const FString Package=TEXT("/Game/DasFrameProbe/Run_")+FGuid::NewGuid().ToString(EGuidFormats::Digits)+TEXT("/BP_Frame");
    if(!TestTrue(TEXT("Create mixed DSL Blueprint"),FDasDSLService::Apply(TEXT("dsl_mixed_methods.das"),Package,TEXT("mixed.MixedCounter"),BP,Error))){AddInfo(Error);return false;}
    FActorSpawnParameters Spawn;Spawn.Name=TEXT("FrameActor");if(!TestNotNull(TEXT("Spawn frame actor"),World->SpawnActor<ADasProbeActor>(BP->GeneratedClass,FTransform::Identity,Spawn)))return false;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasFramePIECommand>(BP,this));return true;
}


namespace {
class FDasTextPIECommand final : public IAutomationLatentCommand {
    TStrongObjectPtr<UBlueprint> BP;FAutomationTestBase* Test;double Start=0;bool Ending=false,Passed=true;
public:
    FDasTextPIECommand(UBlueprint* B,FAutomationTestBase* T):BP(B),Test(T){}
    virtual bool Update() override {
        if(Start==0)Start=FPlatformTime::Seconds();auto Check=[&](bool V,const TCHAR* N){Passed &= Test->TestTrue(N,V);UE_LOG(LogTemp,Display,TEXT("DAS_TEXT_PIE %s %s"),V?TEXT("PASS"):TEXT("FAIL"),N);};
        if(Ending){if(GEditor->PlayWorld && FPlatformTime::Seconds()-Start<30)return false;Check(!GEditor->PlayWorld,TEXT("EndPIE"));UE_LOG(LogTemp,Display,TEXT("DAS_TEXT_PIE_RESULT=%s"),Passed?TEXT("PASS"):TEXT("FAIL"));return true;}
        UWorld* World=GEditor->PlayWorld;if(!World || !World->HasBegunPlay()){if(FPlatformTime::Seconds()-Start<60)return false;Check(false,TEXT("PIE_started"));GEditor->RequestEndPlayMap();Ending=true;Start=FPlatformTime::Seconds();return false;}
        auto* Actor=Find(World,TEXT("TextActor"));Check(Actor!=nullptr,TEXT("real_PIE_actor"));if(!Actor){GEditor->RequestEndPlayMap();Ending=true;Start=FPlatformTime::Seconds();return false;}
        auto* Text=FindFProperty<FStrProperty>(Actor->GetClass(),TEXT("DSLText"));auto* Number=FindFProperty<FDoubleProperty>(Actor->GetClass(),TEXT("DSLPrecise"));auto* Key=FindFProperty<FNameProperty>(Actor->GetClass(),TEXT("DSLKey"));
        Check(Text && Text->GetPropertyValue_InContainer(Actor)==TEXT("Привет") && Number && Number->GetPropertyValue_InContainer(Actor)==1.23456789012345 && Key && Key->GetPropertyValue_InContainer(Actor)==FName(TEXT("Initial_7")),TEXT("string_double_name_CDO_defaults"));
        auto Invoke=[&](const TCHAR* Name,auto Input,auto Output){auto* F=Actor->FindFunction(Name);if(!F)return false;FStructOnScope Frame(F);if(!Input(F,Frame.GetStructMemory()))return false;FEditorScriptExecutionGuard Guard;Actor->ProcessEvent(F,Frame.GetStructMemory());return Output(F,Frame.GetStructMemory());};
        Check(Invoke(TEXT("Echo"),[](UFunction* F,uint8* D){auto* A=FindFProperty<FStrProperty>(F,TEXT("value"));auto* B=FindFProperty<FStrProperty>(F,TEXT("suffix"));if(!A || !B)return false;A->SetPropertyValue_InContainer(D,TEXT("Строка 世界"));B->SetPropertyValue_InContainer(D,TEXT(" !"));return true;},[](UFunction* F,uint8* D){auto* P=CastField<FStrProperty>(F->GetReturnProperty());return P && P->GetPropertyValue_InContainer(D)==TEXT("Строка 世界 !");}),TEXT("unicode_string_ProcessEvent"));
        Check(Invoke(TEXT("Recall"),[](UFunction*,uint8*){return true;},[](UFunction* F,uint8* D){auto* P=CastField<FStrProperty>(F->GetReturnProperty());return P && P->GetPropertyValue_InContainer(D)==TEXT("Строка 世界");}),TEXT("VM_owned_input_persists"));
        Check(Invoke(TEXT("Precise"),[](UFunction* F,uint8* D){auto* A=FindFProperty<FDoubleProperty>(F,TEXT("value"));auto* B=FindFProperty<FDoubleProperty>(F,TEXT("delta"));if(!A || !B)return false;A->SetPropertyValue_InContainer(D,9007199254740990.0);B->SetPropertyValue_InContainer(D,1.0);return true;},[](UFunction* F,uint8* D){auto* P=CastField<FDoubleProperty>(F->GetReturnProperty());return P && P->GetPropertyValue_InContainer(D)==9007199254740991.0;}),TEXT("double_ProcessEvent_64bit"));
        Check(Invoke(TEXT("Rename"),[](UFunction* F,uint8* D){auto* P=FindFProperty<FNameProperty>(F,TEXT("value"));if(!P)return false;P->SetPropertyValue_InContainer(D,FName(TEXT("Item_42")));return true;},[](UFunction* F,uint8* D){auto* P=CastField<FNameProperty>(F->GetReturnProperty());return P && P->GetPropertyValue_InContainer(D)==FName(TEXT("item_42"));}),TEXT("nominal_name_ProcessEvent"));
        auto* EditorActor=Find(GEditor->GetEditorWorldContext().World(),TEXT("TextActor"));auto* EditorText=EditorActor?FindFProperty<FStrProperty>(EditorActor->GetClass(),TEXT("DSLText")):nullptr;
        Check(EditorText && EditorText->GetPropertyValue_InContainer(EditorActor)==TEXT("Привет"),TEXT("text_world_isolation"));
        FString Error;UClass* Before=BP->GeneratedClass;Check(FDasDSLService::Reimport(BP.Get(),Error) && BP->GeneratedClass==Before,TEXT("text_reimport_no_reinstance"));
        CollectGarbage(RF_NoFlags);Actor=Find(World,TEXT("TextActor"));Check(Actor && Text && Text->GetPropertyValue_InContainer(Actor)==TEXT("Строка 世界 !"),TEXT("text_survives_UE_GC"));
        GEditor->RequestEndPlayMap();Ending=true;Start=FPlatformTime::Seconds();return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasTextPIETest,"DasForUE.DSL.TextPIE",EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasTextPIETest::RunTest(const FString& Parameters){
    if(FApp::GetProjectName()!=FString(TEXT("DasForUESmoke"))){AddError(TEXT("Isolated project required"));return false;}
    auto* World=FAutomationEditorCommonUtils::CreateNewMap();UBlueprint* BP=nullptr;FString Error;const FString Package=TEXT("/Game/DasTextProbe/Run_")+FGuid::NewGuid().ToString(EGuidFormats::Digits)+TEXT("/BP_Text");
    if(!TestTrue(TEXT("Create text DSL Blueprint"),FDasDSLService::Apply(TEXT("dsl_text_methods.das"),Package,TEXT("text.TextCounter"),BP,Error))){AddInfo(Error);return false;}
    FActorSpawnParameters Spawn;Spawn.Name=TEXT("TextActor");if(!TestNotNull(TEXT("Spawn text actor"),World->SpawnActor<ADasProbeActor>(BP->GeneratedClass,FTransform::Identity,Spawn)))return false;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDasTextPIECommand>(BP,this));return true;
}
