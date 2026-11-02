#include "DasInterfaceMigration.h"
#include "DasBlueprintRollback.h"
#include "DasDSLService.h"
#include "DasTypePins.h"
#include "DasRuntime.h"
#include "Engine/Blueprint.h"
#include "Editor.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Event.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "EdGraphSchema_K2.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Modules/ModuleManager.h"
#include "UObject/MetaData.h"

namespace
{
const FDasDeclaredMethod* NextMethod(const FDasInterfaceChange& Change, const FDasDeclaredMethod& Before)
{
    return Change.After.Methods.FindByPredicate([&](const auto& M) { return M.Id == Before.Id; });
}
bool SameInterfaceSignature(const FDasDeclaredMethod& A, const FDasDeclaredMethod& B)
{
    if (A.Name != B.Name || A.bPure != B.bPure || A.bConst != B.bConst ||
        A.ReturnType.Identity() != B.ReturnType.Identity() || A.Parameters.Num() != B.Parameters.Num())
        return false;
    for (int32 I = 0; I < A.Parameters.Num(); ++I)
        if (A.Parameters[I].Name != B.Parameters[I].Name || A.Parameters[I].Direction != B.Parameters[I].Direction ||
            A.Parameters[I].Type.Identity() != B.Parameters[I].Type.Identity())
            return false;
    return true;
}
bool Implements(const UBlueprint* BP, const FDasInterfaceChange& Change)
{
    return BP->GeneratedClass && BP->GeneratedClass->ImplementsInterface(Change.Blueprint->GeneratedClass);
}
const FDasDeclaredMethod* ReferencedMethod(UK2Node* Node, UBlueprint* BP, const FDasInterfaceChange& Change)
{
    UFunction* Function = nullptr;
    FName Name;
    if (auto* Call = Cast<UK2Node_CallFunction>(Node))
    {
        Function = Call->FunctionReference.ResolveMember<UFunction>(BP->SkeletonGeneratedClass);
        Name = Call->FunctionReference.GetMemberName();
    }
    else if (auto* Event = Cast<UK2Node_Event>(Node))
    {
        Function = Event->EventReference.ResolveMember<UFunction>(BP->SkeletonGeneratedClass);
        Name = Event->EventReference.GetMemberName();
    }
    else if ((Cast<UK2Node_FunctionEntry>(Node) || Cast<UK2Node_FunctionResult>(Node)) && Implements(BP, Change))
        Name = Node->GetGraph()->GetFName();
    else
        return nullptr;
    if (Function)
    {
        const auto* Owner = Function->GetOuterUClass();
        const auto* OwnerBlueprint = Cast<UBlueprint>(Owner->ClassGeneratedBy);
        if (Owner != Change.Blueprint->GeneratedClass && OwnerBlueprint != Change.Blueprint &&
            !(OwnerBlueprint && Implements(OwnerBlueprint, Change)) &&
            !Owner->ImplementsInterface(Change.Blueprint->GeneratedClass))
            return nullptr;
    }
    else if (!Implements(BP, Change))
        return nullptr;
    return Change.Before.Methods.FindByPredicate([&](const auto& M) { return FName(*M.Name) == Name; });
}
bool CheckConsumerPins(UK2Node* Node, const FDasDeclaredMethod& Before, const FDasDeclaredMethod* After, FString& Error)
{
    if (!After)
    {
        Error = TEXT("Cannot remove used interface method: ") + Before.Name + TEXT(" at ") + Node->GetPathName();
        return false;
    }
    if (SameInterfaceSignature(Before, *After))
        return true;
    if (Before.bPure != After->bPure &&
        Node->Pins.ContainsByPredicate([](const auto* P) { return P && !P->LinkedTo.IsEmpty(); }))
    {
        Error = TEXT("Interface execution/pure shape changed on a connected node: ") + Node->GetPathName();
        return false;
    }
    const bool Entry = Cast<UK2Node_FunctionEntry>(Node) || Cast<UK2Node_Event>(Node);
    const bool Result = Cast<UK2Node_FunctionResult>(Node) != nullptr;
    for (auto* Pin : Node->Pins)
    {
        if (!Pin || Pin->PinName == UEdGraphSchema_K2::PN_Self || Pin->bHidden)
            continue;
        TOptional<FEdGraphPinType> Next;
        if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec)
        {
            if (!After->bPure)
                Next = Pin->PinType;
        }
        else if (Pin->PinName == UEdGraphSchema_K2::PN_ReturnValue)
        {
            if (After->ReturnType.Kind != TEXT("void"))
                Next = DasTypePinChecked(After->ReturnType);
        }
        else
            for (const auto& Parameter : After->Parameters)
            {
                if (FName(*Parameter.Name) != Pin->PinName)
                    continue;
                const bool Input = Parameter.Direction != TEXT("out");
                const auto Direction = Entry ? EGPD_Output : Result ? EGPD_Input : Input ? EGPD_Input : EGPD_Output;
                if (Pin->Direction != Direction || (Entry && !Input) || (Result && Parameter.Direction == TEXT("in")))
                    continue;
                auto Type = DasTypePinChecked(Parameter.Type);
                // Direction/ref flags are node-specific; compare their actual payload type below.
                Type.bIsReference = Pin->PinType.bIsReference;
                Type.bIsConst = Pin->PinType.bIsConst;
                Next = Type;
            }
        if (Next.IsSet() && *Next == Pin->PinType)
            continue;
        const bool CustomDefault = Pin->DefaultObject || !Pin->DefaultTextValue.IsEmpty() ||
                                   Pin->DefaultValue != Pin->AutogeneratedDefaultValue;
        if (!Pin->LinkedTo.IsEmpty() || CustomDefault)
        {
            Error = FString::Printf(TEXT("Interface migration would discard a connected pin or custom default: %s.%s"),
                                         *Node->GetPathName(), *Pin->PinName.ToString());
            return false;
        }
    }
    return true;
}
}
bool PrepareDasInterfaceMigration(const TArray<FDasInterfaceChange>& Changes, TArray<UBlueprint*>& Scope,
                                  TArray<TStrongObjectPtr<UObject>>& Assets, FString& Error)
{
    if (GEditor && GEditor->PlayWorld)
    {
        Error = TEXT("Stop PIE before structural interface migration");
        return false;
    }
    for (const auto& Change : Changes)
        Scope.AddUnique(Change.Blueprint);
    for (const auto& Change : Changes)
    {
        TArray<UBlueprint*> Local;
        TArray<TStrongObjectPtr<UObject>> LocalAssets;
        if (!FDasBlueprintRollback::GatherScope(Change.Blueprint, Local, LocalAssets, Error, false, true))
            return false;
        for (auto* BP : Local)
            Scope.AddUnique(BP);
        for (auto& Asset : LocalAssets)
            if (!Assets.ContainsByPredicate([&](const auto& A) { return A.Get() == Asset.Get(); }))
                Assets.Emplace(Asset.Get());
    }
    for (auto* BP : Scope)
    {
        if (Changes.ContainsByPredicate([&](const auto& C) { return C.Blueprint == BP; }))
            continue;
        for (const auto& Change : Changes)
        {
            if (Implements(BP, Change))
                for (const auto& Before : Change.Before.Methods)
                    if (const auto* After = NextMethod(Change, Before); After && Before.Name != After->Name)
                    {
                        TArray<UEdGraph*> Graphs;
                        BP->GetAllGraphs(Graphs);
                        if (Graphs.ContainsByPredicate([&](const auto* G)
                                                       { return G->GetFName() == FName(*After->Name); }))
                        {
                            Error = TEXT("Interface method rename conflicts with an existing graph: ") +
                                         BP->GetPathName() + TEXT(".") + After->Name;
                            return false;
                        }
                    }
            const FString Source = BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLSource"));
            TArray<UK2Node*> Nodes;
            FBlueprintEditorUtils::GetAllNodesOfClass(BP, Nodes);
            for (auto* Node : Nodes)
                if (const auto* Before = ReferencedMethod(Node, BP, Change))
                {
                    // Generated DAS bridges are regenerated from the validated source below;
                    // preserve the strict link checks for hand-authored graphs.
                    const auto* Graph = Node->GetGraph();
                    const bool GeneratedImplementation =
                        !Source.IsEmpty() && Implements(BP, Change) &&
                        ((Graph->GetFName() == FName(*Before->Name) &&
                              !BP->GetOutermost()->GetMetaData().GetValue(Graph, TEXT("DasMemberId")).IsEmpty()) ||
                          (Cast<UK2Node_Event>(Node) &&
                           !BP->GetOutermost()->GetMetaData().GetValue(Node, TEXT("DasMemberId")).IsEmpty()));
                    if (!GeneratedImplementation &&
                        !CheckConsumerPins(Node, *Before, NextMethod(Change, *Before), Error))
                        return false;
                }
        }
    }
    return true;
}
bool RefreshDasInterfaceConsumers(const TArray<FDasInterfaceChange>& Changes, const TArray<UBlueprint*>& Scope,
                                  FString& Error)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    // Validate every updated DSL source before reimporting any of its implementation bridges.
    TArray<UBlueprint*> Implementations;
    for (auto* BP : Scope)
    {
        auto& Meta = BP->GetOutermost()->GetMetaData();
        const FString Source = Meta.GetValue(BP, TEXT("DasDSLSource"));
        if (Source.IsEmpty())
            continue;
        FDasAuthoringModule Candidate, Previous;
        if (!Runtime.ReadAuthoringScript(Runtime.ScriptPath(Source), Candidate, Error))
        {
            Error = TEXT("Incompatible implementing DSL source or interface caller: ") + Source + TEXT(": ") + Error;
            return false;
        }
        const FString Id = Meta.GetValue(BP, TEXT("DasDSLClassId"));
        if (!FDasAuthoringModule::FromJson(Meta.GetValue(BP, TEXT("DasDSLModel")), Previous, Error))
            return false;
        FDasDeclaredClass Before, After;
        if (!Previous.ResolveClass(Id, Before, Error) || !Candidate.ResolveClass(Id, After, Error))
            return false;
        bool ImplementsChanged = false;
        for (const auto& Change : Changes)
            if (Implements(BP, Change))
            {
                ImplementsChanged = true;
                for (const auto& Required : Change.After.Methods)
                {
                    const auto* Found =
                        After.Methods.FindByPredicate([&](const auto& M) { return M.Name == Required.Name; });
                    if (!Found || !SameInterfaceSignature(*Found, Required))
                    {
                        Error =
                            TEXT("Incompatible implementing DSL source: ") + Source + TEXT(" method ") + Required.Name;
                        return false;
                    }
                }
                auto ChangedName = [&](const auto& Method)
                {
                    return Change.Before.Methods.ContainsByPredicate([&](const auto& M)
                                                                     { return M.Name == Method.Name; }) ||
                           Change.After.Methods.ContainsByPredicate([&](const auto& M)
                                                                    { return M.Name == Method.Name; });
                };
                Before.Methods.RemoveAll(ChangedName);
                After.Methods.RemoveAll(ChangedName);
            }
        const auto Delta = FDasAuthoringModule::Compare(Before, After);
        if (Delta.bStructural || Delta.bDefaults || Delta.bMetadata || Delta.bImplementation)
        {
            Error = TEXT("Apply unrelated DSL declaration/default changes separately before interface migration: ") +
                         Source;
            return false;
        }
        if (ImplementsChanged)
            Implementations.Add(BP);
    }
    auto Depth = [](const UBlueprint* BP)
    {
        int32 Result = 0;
        for (const UClass* Class = BP->GeneratedClass; Class; Class = Class->GetSuperClass())
            ++Result;
        return Result;
    };
    Implementations.Sort([&](const UBlueprint& A, const UBlueprint& B) { return Depth(&A) < Depth(&B); });
    FDasBlueprintMigrationBatch Batch(Scope);
    for (auto* BP : Implementations)
        if (!FDasDSLService::Reimport(BP, Error))
            return false;
    for (auto* BP : Scope)
    {
        if (Changes.ContainsByPredicate([&](const auto& C) { return C.Blueprint == BP; }))
            continue;
        for (const auto& Change : Changes)
            for (const auto& Before : Change.Before.Methods)
                if (const auto* After = NextMethod(Change, Before); After && Before.Name != After->Name)
                {
                    TArray<UK2Node*> Nodes;
                    FBlueprintEditorUtils::GetAllNodesOfClass(BP, Nodes);
                    for (auto* Node : Nodes)
                    {
                        if (auto* Call = Cast<UK2Node_CallFunction>(Node);
                            Call && Call->FunctionReference.GetMemberName() == FName(*Before.Name))
                        {
                            auto* Parent = Call->FunctionReference.GetMemberParentClass();
                            const auto* ParentBlueprint = Parent ? Cast<UBlueprint>(Parent->ClassGeneratedBy) : nullptr;
                            if (Parent == Change.Blueprint->GeneratedClass || ParentBlueprint == Change.Blueprint)
                                Call->FunctionReference.SetExternalMember(*After->Name,
                                                                          Change.Blueprint->GeneratedClass);
                            else if (Call->FunctionReference.IsSelfContext() && Implements(BP, Change))
                                Call->FunctionReference.SetSelfMember(*After->Name);
                            else if (ParentBlueprint && Implements(ParentBlueprint, Change))
                                Call->FunctionReference.SetExternalMember(*After->Name, Parent);
                        }
                        else if (auto* Event = Cast<UK2Node_Event>(Node);
                                 Event && Event->EventReference.GetMemberName() == FName(*Before.Name) &&
                                 Implements(BP, Change))
                            Event->EventReference.SetExternalMember(*After->Name, Change.Blueprint->GeneratedClass);
                    }
                    if (Implements(BP, Change))
                        for (UEdGraph* Graph : BP->FunctionGraphs)
                            if (Graph->GetFName() == FName(*Before.Name))
                                FBlueprintEditorUtils::RenameGraph(Graph, After->Name);
                }
        FBlueprintEditorUtils::ConformImplementedInterfaces(BP);
        FBlueprintEditorUtils::RefreshAllNodes(BP);
    }
    return true;
}
