#include "DasInteropPerformanceCommandlet.h"
#include "DasDSLService.h"
#include "DasInteropProbe.h"
#include "DasCallPhases.h"
#include "DasNativeBinds.h"
#include "DasProbeActor.h"
#include "DasPropertyValues.h"
#include "DasRuntime.h"
#include "Editor.h"
#include "Components/SceneComponent.h"
#include "EdGraphSchema_K2.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "K2Node_CallFunction.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "K2Node_MacroInstance.h"
#include "K2Node_VariableGet.h"
#include "K2Node_VariableSet.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Engine/World.h"
#include "Misc/FileHelper.h"
#include "Misc/Guid.h"
#include "Misc/Parse.h"
#include "Misc/ScopeExit.h"
#include "UObject/Script.h"
#include "UObject/Stack.h"
#include "UObject/StructOnScope.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

// The Blueprint VM's loop limit (ScriptCore.cpp; no public declaration).
extern COREUOBJECT_API int32 GMaximumScriptLoopIterations;

namespace
{
constexpr const TCHAR* InteropScript = TEXT("interop_probe_actor.das");
int64 Triangle(int32 N)
{
    return int64(N) * (N + 1) / 2;
}
FDasScalarValue IntArgument(int32 Value)
{
    FDasScalarValue Out;
    Out.Kind = 0;
    Out.IntValue = Value;
    return Out;
}
// Sets a generated Blueprint function parameter from its own type (a DSL float may be generated as double).
bool SetParam(FProperty* P, void* Frame, int32 I, float F, const FString& S, const FVector& V)
{
    if (auto* Int = CastField<FIntProperty>(P))
        Int->SetPropertyValue_InContainer(Frame, I);
    else if (auto* Float = CastField<FFloatProperty>(P))
        Float->SetPropertyValue_InContainer(Frame, F);
    else if (auto* Double = CastField<FDoubleProperty>(P))
        Double->SetPropertyValue_InContainer(Frame, F);
    else if (auto* Str = CastField<FStrProperty>(P))
        Str->SetPropertyValue_InContainer(Frame, S);
    else if (auto* Struct = CastField<FStructProperty>(P); Struct && Struct->Struct == TBaseStructure<FVector>::Get())
        *Struct->ContainerPtrToValuePtr<FVector>(Frame) = V;
    else
        return false;
    return true;
}
// Cases on an imported DSL actor: the generated Blueprint function (what a Blueprint or a C++ ProcessEvent caller
// pays), its ExecuteDasFrame bridge alone, method calls inside the script and property reads.
void RunEditorCases(FDasInteropBench& B)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Package =
        TEXT("/Game/DasInteropProbe/Run_") + FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT("/BP_InteropProbe");
    UBlueprint* BP = nullptr;
    FString Error;
    if (!FDasDSLService::Apply(InteropScript, Package, TEXT("interopprobe.InteropProbe"), BP, Error) || !BP ||
                               !BP->GeneratedClass)
    {
        B.Fail(TEXT("Import of interop_probe_actor.das failed: ") + Error);
        return;
    }
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Editor, false));
    FActorSpawnParameters Spawn;
    Spawn.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    auto* Actor = World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
    ON_SCOPE_EXIT
    {
        if (Actor)
            Runtime.ReleaseObject(Actor);
        World->DestroyWorld(false);
    };
    if (!Actor)
    {
        B.Fail(TEXT("Cannot spawn the InteropProbe actor"));
        return;
    }
    FEditorScriptExecutionGuard Guard;
    constexpr int32 Heavy = 256, Light = 1 << 14, Direct = 1 << 20;
    const FString Abc = TEXT("abc");
    const FVector Unit(1, 0, 0);
    // Echo(i) == i + 1; Mixed(i, 2.0, "abc", (1,0,0)) == i + 6.
    auto EchoSum = [](int32 N)
    {
        return Triangle(N);
    };
    auto MixedSum = [](int32 N)
    {
        return Triangle(N) + 5ll * N;
    };
    auto MixedArgs = [&](int32 I)
    {
        FDasScalarValue F, S, V;
        F.Kind = 1;
        F.FloatValue = 2.f;
        S.Kind = 5;
        S.StringValue = Abc;
        V.Kind = 9;
        V.VectorValue = Unit;
        return TArray<FDasScalarValue>{IntArgument(I), F, S, V};
    };
    // A4 and A5 run with the source check on every call (_check_always) and with a file watcher (_check_watch).
    struct FMode
    {
        const TCHAR* Suffix;
        int32 Mode, Count;
    };
    for (const FMode& M : {FMode{TEXT("_check_always"), 0, Heavy}, FMode{TEXT("_check_watch"), 1, Light}})
    {
        FDasScriptChangeCheckScope ModeScope(M.Mode);
        // A4. The bridge the generated graph calls: a frame of FDasScalarValue in, one out.
        auto Frame = [&](const TCHAR* Name, const TCHAR* Entry, TFunctionRef<TArray<FDasScalarValue>(int32)> Args,
                         TFunctionRef<int64(int32)> Sum)
        {
            const FString Case = FString(Name) + M.Suffix;
            auto Calls = [&](int32 N)
            {
                int64 S = 0;
                for (int32 I = 0; I < N; ++I)
                {
                    const TArray<FDasScalarValue> Arguments = Args(I);
                    DasCallPhaseBegin();
                    const FDasScalarValue R = Actor->ExecuteDasFrame(InteropScript, Entry, Arguments, 0, FString());
                    if (R.Kind != 0)
                    {
                        B.Fail(Case + TEXT(": ") + Actor->LastDasError);
                        return int64(-1);
                    }
                    S += R.IntValue;
                }
                return S;
            };
            B.Measure(Case, TEXT("A4 C++ -> script, frame bridge"), M.Count, Calls, Sum(M.Count));
            if (M.Mode == 1)
                B.Profile(Case, M.Count, Calls);
        };
        Frame(
            TEXT("frame_bridge_int"), TEXT("ue_InteropProbe_Echo"),
                                           [](int32 I) { return TArray<FDasScalarValue>{IntArgument(I)}; }, EchoSum);
        Frame(TEXT("frame_bridge_mixed"), TEXT("ue_InteropProbe_Mixed"), MixedArgs, MixedSum);

        // A5. ProcessEvent on the generated Blueprint function: the whole path of a Blueprint or C++ caller.
        auto Event = [&](const TCHAR* Name, const TCHAR* Method, TFunctionRef<int64(int32)> Sum)
        {
            const FString Case = FString(Name) + M.Suffix;
            UFunction* Function = Actor->FindFunction(Method);
            auto* Return = Function ? CastField<FIntProperty>(Function->GetReturnProperty()) : nullptr;
            if (!Return)
            {
                B.Fail(Case + TEXT(": generated function or its int result is missing"));
                return;
            }
            FStructOnScope Params(Function);
            auto Calls = [&](int32 N)
            {
                int64 S = 0;
                void* Memory = Params.GetStructMemory();
                for (int32 I = 0; I < N; ++I)
                {
                    for (TFieldIterator<FProperty> It(Function); It && (It->PropertyFlags & CPF_Parm); ++It)
                        if (!(It->PropertyFlags & CPF_ReturnParm) && !SetParam(*It, Memory, I, 2.f, Abc, Unit))
                        {
                            B.Fail(Case + TEXT(": unsupported parameter ") + It->GetName());
                            return int64(-1);
                        }
                    DasCallPhaseBegin();
                    Actor->ProcessEvent(Function, Memory);
                    DasCallPhaseMark(EDasCallPhase::BlueprintReturn);
                    S += Return->GetPropertyValue_InContainer(Memory);
                }
                return S;
            };
            B.Measure(Case, TEXT("A5 C++/Blueprint -> script, ProcessEvent"), M.Count, Calls, Sum(M.Count));
            if (M.Mode == 1)
                B.Profile(Case, M.Count, Calls);
        };
        Event(TEXT("processevent_generated_int"), TEXT("Echo"), EchoSum);
        Event(TEXT("processevent_generated_mixed"), TEXT("Mixed"), MixedSum);
    }
    // A4 without the bridge: the same frame straight into RunNamedFramePersistent.
    {
        FDasScriptChangeCheckScope Watch(1);
        const FString Path = Runtime.ScriptPath(InteropScript);
        const TCHAR* Case = TEXT("frame_direct_check_watch");
        auto Calls = [&](int32 N)
        {
            int64 S = 0;
            for (int32 I = 0; I < N; ++I)
            {
                const TArray<FDasScalarValue> Arguments{IntArgument(I)};
                DasCallPhaseBegin();
                const auto R = Runtime.RunNamedFramePersistent(Path, TEXT("ue_InteropProbe_Echo"), Arguments,
                                                                          EDasMethodSignature::Int, Actor, FString());
                DasCallPhaseMark(EDasCallPhase::Exit);
                if (!R.bSucceeded)
                {
                    B.Fail(FString(Case) + TEXT(": ") + R.Error);
                    return int64(-1);
                }
                S += R.Value;
            }
            return S;
        };
        B.Measure(Case, TEXT("A4 C++ -> script, frame bridge"), Light, Calls, Triangle(Light));
        B.Profile(Case, Light, Calls);
        // The same function through TDasFunction (DasScript.h): typed, the session remembered between calls.
        const TDasFunction<int32(int32)> Echo(InteropScript, TEXT("ue_InteropProbe_Echo"));
        auto Typed = [&](int32 N)
        {
            int64 S = 0;
            for (int32 I = 0; I < N; ++I)
            {
                const auto R = Echo.Call(Actor, I);
                if (R.HasError())
                {
                    B.Fail(TEXT("cpp_typed_function_call: ") + R.GetError());
                    return int64(-1);
                }
                S += R.GetValue();
            }
            return S;
        };
        B.Measure(TEXT("cpp_typed_function_call"), TEXT("A4 C++ -> script, frame bridge"), Light, Typed,
                  Triangle(Light));
    }

    // B, C, D. Loops inside the script, one dispatch per sample; with a file watcher, so that dispatch and the
    // script-to-script calls through typed references do not re-read sources.
    auto Loop = [&](const TCHAR* Case, const TCHAR* Entry, const TCHAR* Group, int64 Expected, bool bProfile = false)
    {
        FDasScriptChangeCheckScope Watch(1);
        auto Run = [&](int32 N)
        {
            DasCallPhaseBegin();
            const FDasScalarValue R = Actor->ExecuteDasFrame(InteropScript, Entry, {IntArgument(N)}, 0, FString());
            if (R.Kind != 0)
            {
                B.Fail(FString(Case) + TEXT(": ") + Actor->LastDasError);
                return int64(-1);
            }
            return int64(R.IntValue);
        };
        B.Measure(Case, Group, Light, Run, Expected);
        // The loop runs inside the script: each of the N calls is profiled, the one outer dispatch adds 1/N.
        if (bProfile)
            B.Profile(Case, Light, Run);
    };
    Loop(TEXT("script_self_method_call"), TEXT("ue_InteropProbe_DirectCalls"), TEXT("C script -> script"),
                                                                                    Triangle(Light));
    Loop(TEXT("script_authored_ref_method_call"), TEXT("ue_InteropProbe_AuthoredCalls"), TEXT("C script -> script"),
                                                                                              Triangle(Light), true);
    Loop(TEXT("script_authored_field_read"), TEXT("ue_InteropProbe_FieldReads"), TEXT("D properties"), 7ll * Light);
    Loop(TEXT("script_native_self_field_read"), TEXT("ue_InteropProbe_NativeFieldReads"), TEXT("D properties"),
                                                                                               7ll * Light);

    // E. Script -> C++ through native binds (DasNativeBinds.h), each a loop inside the script. The probe scripts
    // and the module "smoke_native" exist only in the smoke project; elsewhere the cases are skipped (a registered
    // module is built by the first compilation). Compare cases of one run only: the machine's speed varies more
    // between runs than these costs differ.
    auto NativeLoop = [&](const TCHAR* Case, const TCHAR* Script, const TCHAR* Entry, int64 Expected)
    {
        if (DasNativeModuleStatus(TEXT("smoke_native")).EndsWith(TEXT("is not registered")))
            return;
        FDasScriptChangeCheckScope Watch(1);
        const FString Path = Runtime.ScriptPath(Script);
        auto Run = [&](int32 N)
        {
            const auto R = Runtime.RunNamedPersistent(Path, Entry, N, Actor);
            if (!R.bSucceeded)
            {
                B.Fail(FString(Case) + TEXT(": ") + R.Error);
                return int64(-1);
            }
            return int64(R.Value);
        };
        B.Measure(Case, TEXT("E script -> C++"), Light, Run, Expected);
    };
    NativeLoop(TEXT("script_native_bind_call"), TEXT("native_binds_probe.das"), TEXT("bench"), Triangle(Light));
    NativeLoop(TEXT("script_native_method_call"), TEXT("native_binds_objects.das"), TEXT("bench_method"), Light);
    // The same engine function, direct and through the reflected generated binding.
    const TCHAR* ActorLocation = TEXT("bench_actor_location");
    NativeLoop(TEXT("script_native_actor_location"), TEXT("native_engine_probe.das"), ActorLocation, Light);
    NativeLoop(TEXT("script_reflected_actor_location"), TEXT("native_reflected_bench.das"), ActorLocation, Light);
    // A cheap engine function against the same formula in daScript: the cost of crossing into C++.
    NativeLoop(TEXT("script_native_interp"), TEXT("native_math_probe.das"), TEXT("bench_interp_native"), Light);
    NativeLoop(TEXT("script_daslang_interp"), TEXT("native_math_probe.das"), TEXT("bench_interp_script"), Light);

    // D3. C++ reading the authored property.
    auto* Value = FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("Value"));
    if (!Value)
    {
        B.Fail(TEXT("Authored property Value is missing"));
        return;
    }
    B.Measure(
        TEXT("cpp_authored_property_read_cached"), TEXT("D properties"), Direct,
                                                        [&](int32 N)
                                                        {
                                                            int64 S = 0;
                                                            for (int32 I = 0; I < N; ++I)
                                                                S += Value->GetPropertyValue_InContainer(Actor);
                                                            return S;
                                                        },
                                                        7ll * Direct);
    B.Measure(
        TEXT("cpp_authored_property_read_codec"),
             TEXT("D properties"), Light,
                  [&](int32 N)
                  {
                      int64 S = 0;
                      FDasScalarValue Out;
                      FString ReadError;
                      for (int32 I = 0; I < N; ++I)
                          S += ReadDasPropertyValue(Value, Value->ContainerPtrToValuePtr<void>(Actor), Out, ReadError)
                                   ? Out.IntValue
                                   : -1;
                      return S;
                  },
                  7ll * Light);
}

// F. The workloads of RunDasBlueprintComparison as Blueprint functions of an actor Blueprint built here: one
// ForLoop over 0 .. N - 1 accumulating into a local variable, the result returned. Fails with the reason when a
// node, a pin or a connection is missing (the graph API names pins by strings).
class FBlueprintWorkloads
{
public:
    FString Error;
    UBlueprint* Build()
    {
        const FString Path = TEXT("/Game/DasInteropProbe/Run_") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
        UPackage* Package = CreatePackage(*(Path + TEXT("/BP_BlueprintWorkloads")));
        BP = FKismetEditorUtilities::CreateBlueprint(AActor::StaticClass(), Package, TEXT("BP_BlueprintWorkloads"),
                                                     BPTYPE_Normal, UBlueprint::StaticClass(),
                                                     UBlueprintGeneratedClass::StaticClass());
        auto* Macros =
            LoadObject<UBlueprint>(nullptr, TEXT("/Engine/EditorBlueprintResources/StandardMacros.StandardMacros"));
        if (!BP || !Macros)
            return Fail(TEXT("cannot create the Blueprint or load StandardMacros"));
        for (UEdGraph* G : Macros->MacroGraphs)
            if (G->GetFName() == TEXT("ForLoop"))
                ForLoop = G;
        if (!ForLoop)
            return Fail(TEXT("no ForLoop macro in StandardMacros"));
        for (int32 W = 0; W < 3; ++W)
            if (!Function(W))
                return nullptr;
        FKismetEditorUtilities::CompileBlueprint(BP);
        if (BP->Status == BS_Error || !BP->GeneratedClass)
            return Fail(TEXT("the Blueprint does not compile"));
        return BP;
    }
    static FName Name(int32 W)
    {
        return W == 0 ? TEXT("BpIntLoop") : W == 1 ? TEXT("BpEngineCall") : TEXT("BpVectorMath");
    }

private:
    UBlueprint* BP = nullptr;
    UEdGraph* ForLoop = nullptr;
    UEdGraph* Graph = nullptr;
    UBlueprint* Fail(const FString& Why)
    {
        if (Error.IsEmpty())
            Error = Why;
        return nullptr;
    }
    static FString PinNames(UEdGraphNode* Node)
    {
        FString Names;
        for (UEdGraphPin* P : Node->Pins)
            Names += P->PinName.ToString() + TEXT(" ");
        return Names;
    }
    UEdGraphPin* Pin(UEdGraphNode* Node, const FName PinName, EEdGraphPinDirection Direction)
    {
        UEdGraphPin* P = Node ? Node->FindPin(PinName, Direction) : nullptr;
        if (!P)
            Fail(FString::Printf(TEXT("%s: no pin %s (pins: %s)"), Node ? *Node->GetName() : TEXT("?"),
                                 *PinName.ToString(), Node ? *PinNames(Node) : TEXT("")));
        return P;
    }
    bool Link(UEdGraphPin* From, UEdGraphPin* To)
    {
        if (!From || !To)
            return false;
        if (!GetDefault<UEdGraphSchema_K2>()->TryCreateConnection(From, To))
        {
            Fail(FString::Printf(TEXT("cannot connect %s.%s to %s.%s"), *From->GetOwningNode()->GetName(),
                                 *From->PinName.ToString(), *To->GetOwningNode()->GetName(), *To->PinName.ToString()));
            return false;
        }
        return true;
    }
    UK2Node_CallFunction* Call(UClass* Class, const TCHAR* FunctionName)
    {
        UFunction* F = Class->FindFunctionByName(FunctionName);
        if (!F)
        {
            Fail(FString::Printf(TEXT("no function %s"), FunctionName));
            return nullptr;
        }
        FGraphNodeCreator<UK2Node_CallFunction> Creator(*Graph);
        UK2Node_CallFunction* Node = Creator.CreateNode();
        Node->SetFromFunction(F);
        Creator.Finalize();
        return Node;
    }
    UEdGraphPin* Out(UK2Node_CallFunction* Node, const FName PinName = UEdGraphSchema_K2::PN_ReturnValue)
    {
        return Pin(Node, PinName, EGPD_Output);
    }
    UEdGraphPin* In(UEdGraphNode* Node, const FName PinName)
    {
        return Pin(Node, PinName, EGPD_Input);
    }
    bool Default(UEdGraphNode* Node, const FName PinName, const TCHAR* Value)
    {
        UEdGraphPin* P = In(Node, PinName);
        if (P)
            P->DefaultValue = Value;
        return P != nullptr;
    }
    // FunctionName(N : int) : int { Acc = 0; for (Index in 0 .. N - 1) Acc = Acc + Step(Index); return Acc }
    bool Function(int32 W)
    {
        Graph = FBlueprintEditorUtils::CreateNewGraph(BP, Name(W), UEdGraph::StaticClass(),
                                                      UEdGraphSchema_K2::StaticClass());
        FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP, Graph, true, nullptr);
        TArray<UK2Node_FunctionEntry*> Entries;
        Graph->GetNodesOfClass(Entries);
        if (Entries.Num() != 1)
            return Fail(TEXT("no function entry")) != nullptr;
        UK2Node_FunctionEntry* Entry = Entries[0];
        FEdGraphPinType IntType;
        IntType.PinCategory = UEdGraphSchema_K2::PC_Int;
        Entry->CreateUserDefinedPin(TEXT("N"), IntType, EGPD_Output);
        UK2Node_FunctionResult* Result = FBlueprintEditorUtils::FindOrCreateFunctionResultNode(Entry);
        if (!Result)
            return Fail(TEXT("no function result")) != nullptr;
        Result->CreateUserDefinedPin(TEXT("Sum"), IntType, EGPD_Input);
        if (!FBlueprintEditorUtils::AddLocalVariable(BP, Graph, TEXT("Acc"), IntType, TEXT("0")))
            return Fail(TEXT("cannot add the local variable Acc")) != nullptr;
        const FGuid AccGuid = FBlueprintEditorUtils::FindLocalVariableGuidByName(BP, Graph, TEXT("Acc"));

        FGraphNodeCreator<UK2Node_MacroInstance> LoopCreator(*Graph);
        UK2Node_MacroInstance* Loop = LoopCreator.CreateNode();
        Loop->SetMacroGraph(ForLoop);
        LoopCreator.Finalize();
        auto Variable = [&](auto* Node)
        {
            Node->VariableReference.SetLocalMember(TEXT("Acc"), Graph->GetName(), AccGuid);
        };
        FGraphNodeCreator<UK2Node_VariableGet> GetCreator(*Graph);
        UK2Node_VariableGet* Get = GetCreator.CreateNode();
        Variable(Get);
        GetCreator.Finalize();
        FGraphNodeCreator<UK2Node_VariableGet> FinalGetCreator(*Graph);
        UK2Node_VariableGet* FinalGet = FinalGetCreator.CreateNode();
        Variable(FinalGet);
        FinalGetCreator.Finalize();
        FGraphNodeCreator<UK2Node_VariableSet> SetCreator(*Graph);
        UK2Node_VariableSet* Set = SetCreator.CreateNode();
        Variable(Set);
        SetCreator.Finalize();

        // The loop: 0 .. N - 1.
        UK2Node_CallFunction* Last = Call(UKismetMathLibrary::StaticClass(), TEXT("Subtract_IntInt"));
        bool bOk = Last && Default(Last, TEXT("B"), TEXT("1")) &&
                   Link(Pin(Entry, TEXT("N"), EGPD_Output), In(Last, TEXT("A"))) &&
                   Default(Loop, TEXT("FirstIndex"), TEXT("0")) &&
                   Link(Out(Last), In(Loop, TEXT("LastIndex"))) &&
                   Link(Pin(Entry, UEdGraphSchema_K2::PN_Then, EGPD_Output), In(Loop, UEdGraphSchema_K2::PN_Execute)) &&
                   Link(Pin(Loop, TEXT("LoopBody"), EGPD_Output), In(Set, UEdGraphSchema_K2::PN_Execute)) &&
                   Link(Pin(Loop, TEXT("Completed"), EGPD_Output), In(Result, UEdGraphSchema_K2::PN_Execute)) &&
                   Link(Pin(FinalGet, TEXT("Acc"), EGPD_Output), In(Result, TEXT("Sum")));
        if (!bOk)
            return false;
        // The step of this workload, an int.
        UEdGraphPin* Index = Pin(Loop, TEXT("Index"), EGPD_Output);
        UEdGraphPin* Step = nullptr;
        UK2Node_CallFunction* Mod = nullptr;
        if (W != 1)
        {
            Mod = Call(UKismetMathLibrary::StaticClass(), TEXT("Percent_IntInt"));
            if (!Mod || !Default(Mod, TEXT("B"), TEXT("7")) || !Link(Index, In(Mod, TEXT("A"))))
                return false;
        }
        if (W == 0)
            Step = Out(Mod);
        else if (W == 1)
        {
            // Trunc(GetActorLocation().X) of self.
            UK2Node_CallFunction* Location = Call(AActor::StaticClass(), TEXT("K2_GetActorLocation"));
            UK2Node_CallFunction* Break = Call(UKismetMathLibrary::StaticClass(), TEXT("BreakVector"));
            UK2Node_CallFunction* Trunc = Call(UKismetMathLibrary::StaticClass(), TEXT("FTrunc"));
            if (!Location || !Break || !Trunc || !Link(Out(Location), In(Break, TEXT("InVec"))) ||
                !Link(Out(Break, TEXT("X")), In(Trunc, TEXT("A"))))
                return false;
            // An impure call (when the getter is not pure) runs before the set in the loop body.
            if (UEdGraphPin* Exec = Location->FindPin(UEdGraphSchema_K2::PN_Execute, EGPD_Input))
            {
                Pin(Loop, TEXT("LoopBody"), EGPD_Output)->BreakAllPinLinks();
                if (!Link(Pin(Loop, TEXT("LoopBody"), EGPD_Output), Exec) ||
                    !Link(Out(Location, UEdGraphSchema_K2::PN_Then), In(Set, UEdGraphSchema_K2::PN_Execute)))
                    return false;
            }
            Step = Out(Trunc);
        }
        else
        {
            // Trunc(VSize(MakeVector(Index % 7, 0, 0) + MakeVector(2, 0, 0))).
            UK2Node_CallFunction* ToDouble = Call(UKismetMathLibrary::StaticClass(), TEXT("Conv_IntToDouble"));
            UK2Node_CallFunction* Make = Call(UKismetMathLibrary::StaticClass(), TEXT("MakeVector"));
            UK2Node_CallFunction* Two = Call(UKismetMathLibrary::StaticClass(), TEXT("MakeVector"));
            UK2Node_CallFunction* Add = Call(UKismetMathLibrary::StaticClass(), TEXT("Add_VectorVector"));
            UK2Node_CallFunction* Size = Call(UKismetMathLibrary::StaticClass(), TEXT("VSize"));
            UK2Node_CallFunction* Trunc = Call(UKismetMathLibrary::StaticClass(), TEXT("FTrunc"));
            if (!ToDouble || !Make || !Two || !Add || !Size || !Trunc || !Link(Out(Mod), In(ToDouble, TEXT("InInt"))) ||
                !Link(Out(ToDouble), In(Make, TEXT("X"))) || !Default(Make, TEXT("Y"), TEXT("0")) ||
                !Default(Make, TEXT("Z"), TEXT("0")) || !Default(Two, TEXT("X"), TEXT("2")) ||
                !Default(Two, TEXT("Y"), TEXT("0")) || !Default(Two, TEXT("Z"), TEXT("0")) ||
                !Link(Out(Make), In(Add, TEXT("A"))) || !Link(Out(Two), In(Add, TEXT("B"))) ||
                !Link(Out(Add), In(Size, TEXT("A"))) || !Link(Out(Size), In(Trunc, TEXT("A"))))
                return false;
            Step = Out(Trunc);
        }
        // Acc = Acc + Step.
        UK2Node_CallFunction* Sum = Call(UKismetMathLibrary::StaticClass(), TEXT("Add_IntInt"));
        return Sum && Link(Pin(Get, TEXT("Acc"), EGPD_Output), In(Sum, TEXT("A"))) && Link(Step, In(Sum, TEXT("B"))) &&
               Link(Out(Sum), In(Set, TEXT("Acc")));
    }
};
void RunBlueprintComparison(FDasInteropBench& B)
{
    FBlueprintWorkloads Workloads;
    UBlueprint* BP = Workloads.Build();
    if (!BP)
    {
        B.Fail(TEXT("Blueprint workloads: ") + Workloads.Error);
        return;
    }
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Editor, false));
    FActorSpawnParameters Spawn;
    Spawn.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    AActor* Actor = World->SpawnActor<AActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
    ON_SCOPE_EXIT
    {
        World->DestroyWorld(false);
    };
    if (!Actor)
    {
        B.Fail(TEXT("Cannot spawn the Blueprint workloads actor"));
        return;
    }
    // Workload 1 reads the actor's location: give it a root at (3, 0, 0).
    if (!Actor->GetRootComponent())
    {
        auto* Root = NewObject<USceneComponent>(Actor, TEXT("Root"));
        Actor->SetRootComponent(Root);
        Root->RegisterComponent();
    }
    Actor->SetActorLocation(FVector(3, 0, 0));
    FEditorScriptExecutionGuard Guard;
    UFunction* Functions[3];
    for (int32 W = 0; W < 3; ++W)
        if (!(Functions[W] = Actor->FindFunction(FBlueprintWorkloads::Name(W))))
        {
            B.Fail(TEXT("Blueprint workloads: no compiled function ") + FBlueprintWorkloads::Name(W).ToString());
            return;
        }
    // The Blueprint VM stops a loop after GMaximumScriptLoopIterations jumps, counted until GInitRunaway (once a
    // frame; a commandlet has none) and several per ForLoop iteration: no limit while measuring, a reset per call.
    const int32 LoopLimit = GMaximumScriptLoopIterations;
    FBlueprintCoreDelegates::SetScriptMaximumLoopIterations(MAX_int32);
    ON_SCOPE_EXIT
    {
        FBlueprintCoreDelegates::SetScriptMaximumLoopIterations(LoopLimit);
    };
    auto RunBlueprint = [&](int32 W, int32 N) -> int64
    {
        GInitRunaway();
        FStructOnScope Params(Functions[W]);
        auto* Input = CastField<FIntProperty>(Functions[W]->FindPropertyByName(TEXT("N")));
        auto* Output = CastField<FIntProperty>(Functions[W]->FindPropertyByName(TEXT("Sum")));
        if (!Input || !Output)
        {
            B.Fail(TEXT("Blueprint workloads: unexpected parameters of ") + Functions[W]->GetName());
            return -1;
        }
        Input->SetPropertyValue_InContainer(Params.GetStructMemory(), N);
        Actor->ProcessEvent(Functions[W], Params.GetStructMemory());
        return Output->GetPropertyValue_InContainer(Params.GetStructMemory());
    };
    RunDasBlueprintComparison(B, Actor, RunBlueprint);
}
}

UDasInteropPerformanceCommandlet::UDasInteropPerformanceCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasInteropPerformanceCommandlet::Main(const FString& Params)
{
    FDasInteropBench Bench;
    FString Output, Report, Json;
    FParse::Value(*Params, TEXT("PerfSamples="), Bench.Samples);
    FParse::Value(*Params, TEXT("PerfOutput="), Output);
    Bench.Samples = FMath::Max(Bench.Samples, 1);
    RunDasInteropProbe(Bench);
    RunEditorCases(Bench);
    RunBlueprintComparison(Bench);
    bool Passed = Bench.Finish(Report, Json, TEXT("editor"));
    if (!Output.IsEmpty())
        Passed &= FFileHelper::SaveStringToFile(Json, *Output, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    UE_LOG(LogTemp, Display, TEXT("%s"), *Report);
    UE_LOG(LogTemp, Display, TEXT("DAS_INTEROP_PERFORMANCE_RESULT=%s"), Passed ? TEXT("PASS") : TEXT("FAIL"));
    return Passed ? 0 : 1;
}
