#include "DasBlueprintService.h"
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
    template<class T> T* Node(UEdGraph* Graph)
    {
        FGraphNodeCreator<T> Creator(*Graph);
        T* Result = Creator.CreateNode();
        Creator.Finalize();
        return Result;
    }

    // A fixed int -> int wrapper is enough to test reflected dispatch and body replacement.
    // Production function declarations/signatures will come from the normalized DSL IR.
    bool SetMethod(UBlueprint* BP, const FDasMethodSchema& Method)
    {
        UEdGraph* Graph = nullptr;
        for (UEdGraph* Existing : BP->FunctionGraphs)
            if (Existing->GetFName() == Method.Name) Graph = Existing;
        if (Graph)
        {
            for (UEdGraphNode* Item : Graph->Nodes)
                if (auto* Call = Cast<UK2Node_CallFunction>(Item))
                    if (Call->GetTargetFunction() == ADasProbeActor::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(ADasProbeActor, ExecuteDas)))
                    if (auto* Pin = Call->FindPin(TEXT("ScriptFile")))
                    {
                        Graph->GetSchema()->TrySetDefaultValue(*Pin, Method.ScriptFile);
                        FBlueprintEditorUtils::MarkBlueprintAsModified(BP);
                        return true;
                    }
            return false;
        }
        Graph = FBlueprintEditorUtils::CreateNewGraph(BP, Method.Name, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
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
        Schema->TrySetDefaultValue(*Call->FindPinChecked(TEXT("ScriptFile")), Method.ScriptFile);
        const bool Linked =
            Schema->TryCreateConnection(Entry->FindPinChecked(UEdGraphSchema_K2::PN_Then), Call->GetExecPin()) &&
            Schema->TryCreateConnection(Call->GetThenPin(), Result->FindPinChecked(UEdGraphSchema_K2::PN_Execute)) &&
            Schema->TryCreateConnection(Entry->FindPinChecked(TEXT("Input")), Call->FindPinChecked(TEXT("Input"))) &&
            Schema->TryCreateConnection(Call->GetReturnValuePin(), Result->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
        return Linked;
    }

}

bool FDasBlueprintService::ReadSchema(const FString& ScriptFile, FDasClassSchema& Out, FString& Error)
{
    Error.Reset();
    if (!IsInGameThread()) { Error = TEXT("Schema compilation requires the game thread"); return false; }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const auto Candidate = Runtime.RunFile(Runtime.ScriptPath(ScriptFile), 0);
    if (!Candidate.bSucceeded || Candidate.Types.Num() != 1)
    { Error = Candidate.Error.IsEmpty() ? TEXT("Expected one type declaration") : Candidate.Error; return false; }
    return FDasClassSchema::FromDeclaration(Candidate.Types[0], ScriptFile, Out, Error);
}

bool FDasBlueprintService::Apply(UBlueprint* BP, const FDasClassSchema& Schema, FString& Error)
{
    Error.Reset();
    if (!IsInGameThread() || !IsValid(BP) || !BP->GeneratedClass || !BP->ParentClass || !BP->ParentClass->IsChildOf(ADasProbeActor::StaticClass()))
    { Error = TEXT("Expected an Actor adapter Blueprint on the game thread"); return false; }
    if (!Schema.Validate(Error)) return false;
    if (BP->ParentClass->FindFunctionByName(Schema.Method.Name) || BP->ParentClass->FindPropertyByName(Schema.Method.Name))
    { Error = TEXT("Method conflicts with an inherited member"); return false; }
    TMap<FName, FEdGraphPinType> Pins;
    TMap<FName, FString> Defaults;
    for (const auto& Field : Schema.Fields)
    {
        if (BP->ParentClass->FindPropertyByName(Field.Name) || BP->GeneratedClass->FindFunctionByName(Field.Name))
        { Error = TEXT("Field conflicts with an inherited property or function"); return false; }
        FEdGraphPinType Pin;
        Pin.PinCategory = Field.Kind == EDasScalarKind::Int32 ? UEdGraphSchema_K2::PC_Int : UEdGraphSchema_K2::PC_Real;
        if (Field.Kind == EDasScalarKind::Float32) Pin.PinSubCategory = UEdGraphSchema_K2::PC_Float;
        Pins.Add(Field.Name, Pin);
        Defaults.Add(Field.Name, Field.Kind == EDasScalarKind::Int32 ? FString::FromInt(Field.IntDefault) : FString::Printf(TEXT("%.9g"), Field.FloatDefault));
    }
    for (const auto& Old : BP->NewVariables)
    {
        const auto* Field = Schema.Fields.FindByPredicate([&](const FDasFieldSchema& F) { return F.Name == Old.VarName; });
        if (!Field || Pins[Old.VarName] != Old.VarType ||
            Field->bInstanceEditable == !!(Old.PropertyFlags & CPF_DisableEditOnInstance))
        { Error = TEXT("Removal, type, default or editability changes require a migration policy"); return false; }
        UObject* DefaultsObject = BP->GeneratedClass->GetDefaultObject();
        const auto* IntProperty = FindFProperty<FIntProperty>(BP->GeneratedClass, Old.VarName);
        const auto* FloatProperty = FindFProperty<FFloatProperty>(BP->GeneratedClass, Old.VarName);
        const bool bSameDefault = Field->Kind == EDasScalarKind::Int32
            ? IntProperty && IntProperty->GetPropertyValue_InContainer(DefaultsObject) == Field->IntDefault
            : FloatProperty && FloatProperty->GetPropertyValue_InContainer(DefaultsObject) == Field->FloatDefault;
        if (!bSameDefault) { Error = TEXT("Changing a class default requires a migration policy"); return false; }
    }
    // Validate existing graph shape before adding any properties.
    for (UEdGraph* Graph : BP->FunctionGraphs)
        if (Graph->GetFName() == Schema.Method.Name)
        {
            UFunction* Function = BP->GeneratedClass->FindFunctionByName(Schema.Method.Name);
            bool bAdapter = false;
            for (UEdGraphNode* Item : Graph->Nodes)
                if (auto* Call = Cast<UK2Node_CallFunction>(Item))
                    bAdapter |= Call->GetTargetFunction() == ADasProbeActor::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(ADasProbeActor, ExecuteDas)) && Call->FindPin(TEXT("ScriptFile"));
            if (!bAdapter || !Function || Function->NumParms != 2 ||
                !FindFProperty<FIntProperty>(Function, TEXT("Input")) || !CastField<FIntProperty>(Function->GetReturnProperty()))
            { Error = TEXT("Existing method is not a supported daScript adapter graph"); return false; }
        }
    BP->Modify();
    for (const auto& Field : Schema.Fields)
        if (!BP->NewVariables.ContainsByPredicate([&](const FBPVariableDescription& V) { return V.VarName == Field.Name; }))
        {
            if (!FBlueprintEditorUtils::AddMemberVariable(BP, Field.Name, Pins[Field.Name], Defaults[Field.Name]))
            { Error = TEXT("Unable to create Blueprint field"); return false; }
            FBlueprintEditorUtils::SetBlueprintOnlyEditableFlag(BP, Field.Name, !Field.bInstanceEditable);
        }
    if (!SetMethod(BP, Schema.Method)) { Error = TEXT("Unable to create method graph"); return false; }
    FKismetEditorUtilities::CompileBlueprint(BP);
    FBlueprintCompilationManager::FlushCompilationQueueAndReinstance();
    if (BP->Status == BS_Error || !BP->GeneratedClass)
    { Error = TEXT("Blueprint compilation failed after mutation; inspect compiler diagnostics"); return false; }
    return true;
}
