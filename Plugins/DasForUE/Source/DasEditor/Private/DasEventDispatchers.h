#pragma once
#include "DasDelegateSignatures.h"
#include "UObject/StrongObjectPtr.h"
namespace
{
UFunction* DasEventSignature(const FString& Wire, UObject* Outer, FString& Error)
{
    FDasAuthoringModule M;
    if (!FDasAuthoringModule::FromJson(Wire, M, Error) || M.Classes.Num() != 1 || M.Classes[0].Methods.Num() != 1)
        return nullptr;
    return MakeDasDelegateSignature(M.Classes[0].Methods[0], true, Outer, NAME_None, Error);
}
bool DasValidateEventField(UBlueprint* BP, const FDasFieldSchema& F, FString& Error)
{
    auto* P = CastField<FMulticastDelegateProperty>(BP->GeneratedClass->FindPropertyByName(F.Name));
    TStrongObjectPtr<UFunction> S(DasEventSignature(F.DelegateWire, GetTransientPackage(), Error));
    if (!P || !S || !S->IsSignatureCompatibleWith(P->SignatureFunction) ||
        S->ParmsSize != P->SignatureFunction->ParmsSize)
    {
        Error = TEXT("Changing an event signature requires a later dependency migration: ")+ F.Name.ToString();
        return false;
    }
    auto* G = BP->DelegateSignatureGraphs.FindByPredicate([&](const auto& Graph)
                                                          { return Graph && Graph->GetFName() == F.Name; });
    if (!G || FString(BP->GetOutermost()->GetMetaData().GetValue(*G, TEXT("DasEventId"))) != F.StableId)
    {
        Error = TEXT("Existing dispatcher is not owned by this declaration");
        return false;
    }
    return true;
}
bool DasCreateEventGraph(UBlueprint* BP, const FDasFieldSchema& F, FString& Error)
{
    if (BP->DelegateSignatureGraphs.ContainsByPredicate([&](const auto& G) { return G && G->GetFName() == F.Name; }))
        return true;
    TStrongObjectPtr<UFunction> S(DasEventSignature(F.DelegateWire, GetTransientPackage(), Error));
    if (!S)
        return false;
    auto* G =
        FBlueprintEditorUtils::CreateNewGraph(BP, F.Name, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
    G->bEditable = false;
    const auto* Schema = GetDefault<UEdGraphSchema_K2>();
    Schema->CreateDefaultNodesForGraph(*G);
    Schema->CreateFunctionGraphTerminators(*G, (UClass*)nullptr);
    Schema->AddExtraFunctionFlags(G, FUNC_BlueprintCallable | FUNC_BlueprintEvent | FUNC_Public);
    Schema->MarkFunctionEntryAsEditable(G, true);
    TArray<UK2Node_FunctionEntry*> Entries;
    G->GetNodesOfClass(Entries);
    if (Entries.Num() != 1)
    {
        Error = TEXT("Unable to create dispatcher entry");
        return false;
    }
    for (TFieldIterator<FProperty> It(S.Get()); It; ++It)
        if (It->HasAnyPropertyFlags(CPF_Parm))
        {
            FEdGraphPinType Pin;
            if (!Schema->ConvertPropertyToPinType(*It, Pin) ||
                !Entries[0]->CreateUserDefinedPin(It->GetFName(), Pin, EGPD_Output))
            {
                Error = TEXT("Unable to create event signature pin");
                return false;
            }
        }
    BP->DelegateSignatureGraphs.Add(G);
    BP->GetOutermost()->GetMetaData().SetValue(G, TEXT("DasEventId"), *F.StableId);
    return true;
}
}
