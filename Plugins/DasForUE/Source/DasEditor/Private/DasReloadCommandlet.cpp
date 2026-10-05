#include "DasReloadCommandlet.h"
#include "DasProbeActor.h"
#include "DasRuntime.h"
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

    template<class T> T* Node(UEdGraph* Graph)
    {
        FGraphNodeCreator<T> Creator(*Graph);
        T* Result = Creator.CreateNode();
        Creator.Finalize();
        return Result;
    }

    // A fixed int -> int wrapper is enough to test reflected dispatch and body replacement.
    // Production function declarations/signatures will come from the normalized DSL IR.
    bool SetMethod(UBlueprint* BP, const FString& Script)
    {
        UEdGraph* Graph = nullptr;
        for (UEdGraph* Existing : BP->FunctionGraphs)
            if (Existing->GetFName() == TEXT("Evaluate")) Graph = Existing;
        if (Graph)
        {
            for (UEdGraphNode* Item : Graph->Nodes)
                if (auto* Call = Cast<UK2Node_CallFunction>(Item))
                    if (auto* Pin = Call->FindPin(TEXT("ScriptFile")))
                    {
                        Graph->GetSchema()->TrySetDefaultValue(*Pin, Script);
                        FBlueprintEditorUtils::MarkBlueprintAsModified(BP);
                        return true;
                    }
            return false;
        }
        Graph = FBlueprintEditorUtils::CreateNewGraph(BP, TEXT("Evaluate"), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
        FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP, Graph, true, nullptr);
        TArray<UK2Node_FunctionEntry*> Entries;
        Graph->GetNodesOfClass(Entries);
        if (Entries.Num() != 1) return false;
        auto* Entry = Entries[0];
        FEdGraphPinType IntType;
        IntType.PinCategory = UEdGraphSchema_K2::PC_Int;
        Entry->CreateUserDefinedPin(TEXT("Input"), IntType, EGPD_Output);
        TArray<UK2Node_FunctionResult*> Results;
        Graph->GetNodesOfClass(Results);
        auto* Result = Results.Num() ? Results[0] : Node<UK2Node_FunctionResult>(Graph);
        Result->CreateUserDefinedPin(UEdGraphSchema_K2::PN_ReturnValue, IntType, EGPD_Input);
        auto* Call = NewObject<UK2Node_CallFunction>(Graph);
        Call->SetFromFunction(ADasProbeActor::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(ADasProbeActor, ExecuteDas)));
        Graph->AddNode(Call, false, false);
        Call->CreateNewGuid();
        Call->PostPlacedNewNode();
        Call->AllocateDefaultPins();
        const auto* Schema = Graph->GetSchema();
        Schema->TrySetDefaultValue(*Call->FindPinChecked(TEXT("ScriptFile")), Script);
        const bool Linked =
            Schema->TryCreateConnection(Entry->FindPinChecked(UEdGraphSchema_K2::PN_Then), Call->GetExecPin()) &&
            Schema->TryCreateConnection(Call->GetThenPin(), Result->FindPinChecked(UEdGraphSchema_K2::PN_Execute)) &&
            Schema->TryCreateConnection(Entry->FindPinChecked(TEXT("Input")), Call->FindPinChecked(TEXT("Input"))) &&
            Schema->TryCreateConnection(Call->GetReturnValuePin(), Result->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
        return Linked;
    }

    bool ApplySchema(UBlueprint* BP, const FString& Script, FString& Error)
    {
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        const auto Candidate = Runtime.RunFile(Runtime.ScriptPath(Script), 0);
        if (!Candidate.bSucceeded || Candidate.Types.Num() != 1)
        {
            Error = Candidate.Error.IsEmpty() ? TEXT("Expected one type declaration") : Candidate.Error;
            return false;
        }
        const auto& Type = Candidate.Types[0];
        if (Type.Name != TEXT("ProbeActor") || Type.Parent != TEXT("Actor"))
        {
            Error = TEXT("This probe only accepts ProbeActor : Actor");
            return false;
        }
        TMap<FName, FEdGraphPinType> Fields;
        TArray<FString> Items;
        Type.Fields.ParseIntoArray(Items, TEXT(";"));
        for (const auto& Item : Items)
        {
            FString Name, Kind;
            if (!Item.Split(TEXT(":"), &Name, &Kind) || Name.IsEmpty() || FChar::IsDigit(Name[0])) return false;
            for (TCHAR Ch : Name) if (!FChar::IsAlnum(Ch) && Ch != TEXT('_')) return false;
            const FName Key(*Name);
            if (Fields.Contains(Key) || ADasProbeActor::StaticClass()->FindPropertyByName(Key)) return false;
            FEdGraphPinType Pin;
            if (Kind == TEXT("int")) Pin.PinCategory = UEdGraphSchema_K2::PC_Int;
            else if (Kind == TEXT("float"))
            {
                Pin.PinCategory = UEdGraphSchema_K2::PC_Real;
                Pin.PinSubCategory = UEdGraphSchema_K2::PC_Float;
            }
            else return false;
            Fields.Add(Key, Pin);
        }
        // Validate the entire candidate before touching the currently usable class.
        // Removal/type changes deliberately require a future migration policy.
        for (const auto& Old : BP->NewVariables)
            if (!Fields.Contains(Old.VarName) || Fields[Old.VarName] != Old.VarType)
            {
                Error = TEXT("Probe supports additive fields only");
                return false;
            }
        for (const auto& Field : Fields)
            if (!BP->NewVariables.ContainsByPredicate([&](const FBPVariableDescription& V) { return V.VarName == Field.Key; }))
            {
                // Fixture defaults, not the future public DSL default-value syntax.
                const FString Default = Field.Key == TEXT("speed") ? TEXT("10.0") : TEXT("100");
                if (!FBlueprintEditorUtils::AddMemberVariable(BP, Field.Key, Field.Value, Default)) return false;
                FBlueprintEditorUtils::SetBlueprintOnlyEditableFlag(BP, Field.Key, false);
            }
        return SetMethod(BP, Script) && Compile(BP);
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
#endif
