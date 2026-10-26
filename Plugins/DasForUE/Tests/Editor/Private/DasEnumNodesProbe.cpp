#include "Engine/Blueprint.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "K2Node_EnumLiteral.h"
#include "K2Node_GetNumEnumEntries.h"
#include "K2Node_CastByteToEnum.h"
#include "K2Node_Select.h"
#include "K2Node_ForEachElementInEnum.h"
#include "K2Node_CallFunction.h"
#include "K2Node_VariableGet.h"
#include "K2Node_VariableSet.h"
#include "Kismet/KismetMathLibrary.h"
#include "DasActor.h"
#include "UObject/StructOnScope.h"
#include "Editor.h"
namespace
{
struct FProbeGraph
{
    UEdGraph* Graph;
    UK2Node_FunctionEntry* Entry;
    UK2Node_FunctionResult* Result;
};
FProbeGraph ProbeGraph(UBlueprint* BP, FName Name, FEdGraphPinType Return)
{
    auto* G =
        FBlueprintEditorUtils::CreateNewGraph(BP, Name, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
    FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP, G, true, nullptr);
    TArray<UK2Node_FunctionEntry*> Entries;
    G->GetNodesOfClass(Entries);
    auto* R = NewObject<UK2Node_FunctionResult>(G);
    G->AddNode(R);
    R->CreateNewGuid();
    R->FunctionReference = Entries[0]->FunctionReference;
    R->AllocateDefaultPins();
    R->CreateUserDefinedPin(UEdGraphSchema_K2::PN_ReturnValue, Return, EGPD_Input);
    return {G, Entries[0], R};
}
void Wire(UEdGraph* G, UEdGraphPin* A, UEdGraphPin* B)
{
    check(G->GetSchema()->TryCreateConnection(A, B));
}
template <class T> T* AddNode(UEdGraph* G)
{
    auto* N = NewObject<T>(G);
    G->AddNode(N);
    N->CreateNewGuid();
    return N;
}
UK2Node_CallFunction* Call(UEdGraph* G, FName Name)
{
    auto* N = AddNode<UK2Node_CallFunction>(G);
    N->SetFromFunction(UKismetMathLibrary::StaticClass()->FindFunctionByName(Name));
    N->AllocateDefaultPins();
    return N;
}
}
void BuildDasEnumNodeProbes(UBlueprint* BP, UEnum* Enum)
{
    FEdGraphPinType Int;
    Int.PinCategory = UEdGraphSchema_K2::PC_Int;
    FEdGraphPinType Byte;
    Byte.PinCategory = UEdGraphSchema_K2::PC_Byte;
    FEdGraphPinType EnumType = Byte;
    EnumType.PinSubCategoryObject = Enum;
    {
        auto P = ProbeGraph(BP, TEXT("EnumCount"), Int);
        auto* N = AddNode<UK2Node_GetNumEnumEntries>(P.Graph);
        N->Enum = Enum;
        N->AllocateDefaultPins();
        Wire(P.Graph, P.Entry->FindPinChecked(UEdGraphSchema_K2::PN_Then),
             P.Result->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
        Wire(P.Graph, N->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue),
             P.Result->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));
    }
    {
        auto P = ProbeGraph(BP, TEXT("EnumLiteral"), EnumType);
        auto* N = AddNode<UK2Node_EnumLiteral>(P.Graph);
        N->Enum = Enum;
        N->AllocateDefaultPins();
        P.Graph->GetSchema()->TrySetDefaultValue(*N->FindPinChecked(UK2Node_EnumLiteral::GetEnumInputPinName()),
                                                 TEXT("Active"));
        Wire(P.Graph, P.Entry->FindPinChecked(UEdGraphSchema_K2::PN_Then),
             P.Result->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
        Wire(P.Graph, N->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue),
             P.Result->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));
    }
    {
        auto P = ProbeGraph(BP, TEXT("EnumCast"), EnumType);
        P.Entry->CreateUserDefinedPin(TEXT("Input"), Byte, EGPD_Output);
        auto* N = AddNode<UK2Node_CastByteToEnum>(P.Graph);
        N->Enum = Enum;
        N->bSafe = true;
        N->AllocateDefaultPins();
        Wire(P.Graph, P.Entry->FindPinChecked(UEdGraphSchema_K2::PN_Then),
             P.Result->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
        Wire(P.Graph, P.Entry->FindPinChecked(TEXT("Input")), N->FindPinChecked(TEXT("Byte")));
        Wire(P.Graph, N->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue),
             P.Result->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));
    }
    {
        auto P = ProbeGraph(BP, TEXT("EnumSelect"), Int);
        P.Entry->CreateUserDefinedPin(TEXT("Input"), EnumType, EGPD_Output);
        auto* N = AddNode<UK2Node_Select>(P.Graph);
        static_cast<INodeDependingOnEnumInterface*>(N)->ReloadEnum(Enum);
        N->AllocateDefaultPins();
        Wire(P.Graph, P.Entry->FindPinChecked(UEdGraphSchema_K2::PN_Then),
             P.Result->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
        Wire(P.Graph, P.Entry->FindPinChecked(TEXT("Input")), N->GetIndexPin());
        Wire(P.Graph, N->GetReturnValuePin(), P.Result->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));
        N->FindPinChecked(TEXT("Idle"))->DefaultValue = TEXT("70");
        N->FindPinChecked(TEXT("Active"))->DefaultValue = TEXT("420");
    }

    {
        auto P = ProbeGraph(BP, TEXT("EnumSum"), Int);
        FBPVariableDescription Local;
        Local.VarName = TEXT("EnumAccumulator");
        Local.VarGuid = FGuid::NewGuid();
        Local.VarType = Int;
        Local.PropertyFlags = CPF_BlueprintVisible;
        Local.DefaultValue = TEXT("0");
        P.Entry->LocalVariables.Add(Local);
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
        auto* Start = AddNode<UK2Node_VariableSet>(P.Graph);
        Start->VariableReference.SetLocalMember(TEXT("EnumAccumulator"), P.Graph->GetName(), Local.VarGuid);
        Start->AllocateDefaultPins();
        Start->FindPinChecked(TEXT("EnumAccumulator"))->DefaultValue = TEXT("0");
        auto* Loop = AddNode<UK2Node_ForEachElementInEnum>(P.Graph);
        Loop->Enum = Enum;
        Loop->AllocateDefaultPins();
        auto* Get = AddNode<UK2Node_VariableGet>(P.Graph);
        Get->VariableReference.SetLocalMember(TEXT("EnumAccumulator"), P.Graph->GetName(), Local.VarGuid);
        Get->AllocateDefaultPins();
        auto* Conv = Call(P.Graph, TEXT("Conv_ByteToInt"));
        auto* Add = Call(P.Graph, TEXT("Add_IntInt"));
        auto* Set = AddNode<UK2Node_VariableSet>(P.Graph);
        Set->VariableReference.SetLocalMember(TEXT("EnumAccumulator"), P.Graph->GetName(), Local.VarGuid);
        Set->AllocateDefaultPins();
        Wire(P.Graph, P.Entry->FindPinChecked(UEdGraphSchema_K2::PN_Then),
             Start->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
        Wire(P.Graph, Start->FindPinChecked(UEdGraphSchema_K2::PN_Then),
             Loop->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
        Wire(P.Graph, Loop->FindPinChecked(TEXT("LoopBody")), Set->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
        Wire(P.Graph, Loop->FindPinChecked(TEXT("EnumValue")), Conv->FindPinChecked(TEXT("InByte")));
        Wire(P.Graph, Conv->GetReturnValuePin(), Add->FindPinChecked(TEXT("B")));
        Wire(P.Graph, Get->GetValuePin(), Add->FindPinChecked(TEXT("A")));
        Wire(P.Graph, Add->GetReturnValuePin(), Set->FindPinChecked(TEXT("EnumAccumulator")));
        Wire(P.Graph, Loop->FindPinChecked(UEdGraphSchema_K2::PN_Then),
             P.Result->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
        Wire(P.Graph, Get->GetValuePin(), P.Result->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));
    }
    FKismetEditorUtilities::CompileBlueprint(BP);
}
bool RunDasEnumNodeProbes(ADasActor* Actor, bool Added)
{
    auto Run = [&](FName Name, int32 Input = 0)
    {
        auto* F = Actor->FindFunction(Name);
        if (!F)
            return -1;
        FStructOnScope Args(F);
        if (auto* P = FindFProperty<FByteProperty>(F, TEXT("Input")))
            P->SetPropertyValue_InContainer(Args.GetStructMemory(), uint8(Input));
        {
            FEditorScriptExecutionGuard G;
            Actor->ProcessEvent(F, Args.GetStructMemory());
        }
        auto* R = CastField<FNumericProperty>(F->GetReturnProperty());
        return R ? int32(R->GetSignedIntPropertyValue(R->ContainerPtrToValuePtr<void>(Args.GetStructMemory()))) : -1;
    };
    return Run(TEXT("EnumCount")) == (Added ? 3 : 2) &&
               Run(TEXT("EnumLiteral")) == 42 && Run(TEXT("EnumCast"), 42) == 42 &&
                                                     Run(TEXT("EnumCast"), 200) == (Added ? 200 : 43) &&
                                                         Run(TEXT("EnumCast"), 255) == (Added ? 201 : 43) &&
                                                             Run(TEXT("EnumSelect"), 7) == 70 &&
                                                                 Run(TEXT("EnumSelect"), 42) == 420 &&
                                                                     Run(TEXT("EnumSum")) == (Added ? 249 : 49);
}
