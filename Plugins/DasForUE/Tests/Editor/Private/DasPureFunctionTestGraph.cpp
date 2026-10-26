#include "DasPureFunctionTestGraph.h"
#include "DasActor.h"
#include "UObject/UnrealType.h"
#include "Engine/Blueprint.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "K2Node_CallFunction.h"
#include "EdGraphSchema_K2.h"

UBlueprint* CreateDasPureConsumer(UClass* Parent, UPackage* Package, FName Name, FString& Error, FName Method,
                                  bool UseDefaults)
{
    auto Fail = [&](const TCHAR* E) -> UBlueprint*
    {
        Error = E;
        return nullptr;
    };
    auto* Function = Parent
        ? Parent->FindFunctionByName(!Method.IsNone() ? Method
                                                      : Parent->IsChildOf(UBlueprintFunctionLibrary::StaticClass())
                                         ? (Parent->FindFunctionByName(TEXT("Add")) ? TEXT("Add"): TEXT("Time"))
                                            : TEXT("Query"))
                                         : nullptr;
    if (!Function || !Function->HasAllFunctionFlags(FUNC_BlueprintPure | FUNC_BlueprintCallable))
        return Fail(TEXT("Query must be a callable pure function"));
    auto* BP = FKismetEditorUtilities::CreateBlueprint(
        Function->HasAnyFunctionFlags(FUNC_Static) ? ADasActor::StaticClass() : Parent, Package, Name, BPTYPE_Normal,
        UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), TEXT("PureFunctionValidation"));
    if (!BP)
        return Fail(TEXT("Cannot create pure consumer"));
    if (!AddDasPureConsumerGraph(BP, Function, Error, UseDefaults))
        return nullptr;
    return BP;
}

bool AddDasPureConsumerGraph(UBlueprint* BP, UFunction* Function, FString& Error, bool UseDefaults)
{
    auto Fail = [&](const TCHAR* E)
    {
        Error = E;
        return false;
    };
    if (!BP || !Function || !Function->HasAllFunctionFlags(FUNC_BlueprintPure | FUNC_BlueprintCallable))
        return Fail(TEXT("Pure consumer requires a callable pure function"));
    auto* G = FBlueprintEditorUtils::CreateNewGraph(BP, TEXT("EvaluatePure"), UEdGraph::StaticClass(),
                                                             UEdGraphSchema_K2::StaticClass());
    FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP, G, true, nullptr);
    TArray<UK2Node_FunctionEntry*> Entries;
    G->GetNodesOfClass(Entries);
    if (Entries.Num() != 1)
        return Fail(TEXT("Missing function entry"));
    auto* Entry = Entries[0];
    Entry->AddExtraFlags(FUNC_BlueprintCallable | FUNC_Public);
    TArray<UK2Node_FunctionResult*> Results;
    G->GetNodesOfClass(Results);
    UK2Node_FunctionResult* Result = Results.Num() ? Results[0] : nullptr;
    if (!Result)
    {
        FGraphNodeCreator<UK2Node_FunctionResult> Creator(*G);
        Result = Creator.CreateNode();
        Result->FunctionReference = Entry->FunctionReference;
        Creator.Finalize();
    }
    FEdGraphPinType Type;
    Type.PinCategory = CastField<FDoubleProperty>(Function->GetReturnProperty()) ? UEdGraphSchema_K2::PC_Real
                                                                                 : UEdGraphSchema_K2::PC_Int;
    if (Type.PinCategory == UEdGraphSchema_K2::PC_Real)
        Type.PinSubCategory = UEdGraphSchema_K2::PC_Double;
    auto* Output = Result->CreateUserDefinedPin(UEdGraphSchema_K2::PN_ReturnValue, Type, EGPD_Input);
    auto* Call = NewObject<UK2Node_CallFunction>(G);
    Call->SetFromFunction(Function);
    G->AddNode(Call, false, false);
    Call->CreateNewGuid();
    Call->PostPlacedNewNode();
    Call->AllocateDefaultPins();
    if (Call->FindPin(UEdGraphSchema_K2::PN_Execute) || Call->FindPin(UEdGraphSchema_K2::PN_Then))
        return Fail(TEXT("Pure call unexpectedly has exec pins"));
    const auto* Schema = GetDefault<UEdGraphSchema_K2>();
    if (UseDefaults)
    {
        if (Call->FindPinChecked(TEXT("left"))->DefaultValue !=
                                 TEXT("7") || Call->FindPinChecked(TEXT("right"))->DefaultValue != TEXT("-2"))
            return Fail(TEXT("Declared Blueprint parameter defaults differ"));
    }
    else if (Function->HasAnyFunctionFlags(FUNC_Static) && Function->GetFName() == TEXT("Time"))
    {
        if (Function->GetMetaData(TEXT("WorldContext")) != TEXT("context") ||
                                                                !Call->FindPinChecked(TEXT("context"))->bHidden)
            return Fail(TEXT("WorldContext input was not hidden/autofilled"));
    }
    else if (Function->HasAnyFunctionFlags(FUNC_Static))
    {
        Schema->TrySetDefaultValue(*Call->FindPinChecked(TEXT("left")), TEXT("7"));
        Schema->TrySetDefaultValue(*Call->FindPinChecked(TEXT("right")), TEXT("5"));
        if (auto* World = Call->FindPin(TEXT("__WorldContext")))
        {
            if (!World->bHidden || !Function->HasMetaData(TEXT("CallableWithoutWorldContext")))
                return Fail(TEXT("Static call requires a world"));
        }
    }
    else
        Schema->TrySetDefaultValue(*Call->FindPinChecked(TEXT("Input")), TEXT("2"));
    if (!Output ||
        !Schema->TryCreateConnection(Entry->FindPinChecked(UEdGraphSchema_K2::PN_Then),
                                     Result->FindPinChecked(UEdGraphSchema_K2::PN_Execute)) ||
        !Schema->TryCreateConnection(Call->GetReturnValuePin(), Output))
        return Fail(TEXT("Cannot wire pure data consumer"));
    // Output-pin creation may produce an automatic result. Keep the explicit, wired return.
    TArray<UK2Node_FunctionResult*> FinalResults;
    G->GetNodesOfClass(FinalResults);
    for (auto* Other : FinalResults)
        if (Other != Result && Other->FindPinChecked(UEdGraphSchema_K2::PN_Execute)->LinkedTo.IsEmpty())
        {
            Other->BreakAllNodeLinks();
            G->RemoveNode(Other);
        }
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
    FKismetEditorUtilities::CompileBlueprint(BP);
    return BP->Status != BS_Error || Fail(TEXT("Pure consumer failed compilation"));
}
