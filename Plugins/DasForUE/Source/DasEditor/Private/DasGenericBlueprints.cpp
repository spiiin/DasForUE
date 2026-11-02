#include "DasGenericBlueprints.h"
#include "DasObject.h"
#include "DasScriptClassData.h"
#include "DasAuthoring.h"
#include "DasSchema.h"
#include "DasGenericParents.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "K2Node_CallParentFunction.h"
#include "K2Node_Self.h"
#include "K2Node_Event.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/MetaData.h"
#include "UObject/Package.h"
#include "UObject/UObjectIterator.h"

bool DasCheckBlueprintParent(UClass* Parent, FString& Error)
{
    if (!Parent || Parent->HasAnyClassFlags(CLASS_Interface | CLASS_Deprecated | CLASS_NewerVersionExists) ||
        !FKismetEditorUtilities::CanCreateBlueprintOfClass(Parent))
    {
        Error = TEXT("UE class is missing, obsolete, an interface or does not permit Blueprint inheritance");
        return false;
    }
    return true;
}
UDasScriptClassData* DasGenericClassData(UBlueprint* BP)
{
    return BP ? FindObject<UDasScriptClassData>(BP->GetOutermost(), TEXT("DasScriptClassData")) : nullptr;
}
bool DasCanReimportGenericBlueprint(UBlueprint* BP)
{
    // Plain script objects have no generated lifecycle graph to rebuild. Other
    // generic native parents keep their existing structural migration guard.
    return BP && BP->ParentClass && BP->ParentClass->IsChildOf<UDasObject>() && DasGenericClassData(BP);
}
bool DasWireGenericOwner(UK2Node_CallFunction* Call, UBlueprint* BP)
{
    auto* Data = DasGenericClassData(BP);
    if (!Data)
        return false;
    Call->FindPinChecked(TEXT("ClassData"))->DefaultObject = Data;
    FGraphNodeCreator<UK2Node_Self> Creator(*Call->GetGraph());
    auto* Self = Creator.CreateNode();
    Creator.Finalize();
    return Call->GetGraph()->GetSchema()->TryCreateConnection(Self->FindPinChecked(UEdGraphSchema_K2::PN_Self),
                                                              Call->FindPinChecked(TEXT("Owner")));
}
bool DasPrepareGenericBlueprint(UBlueprint* BP, const FDasDeclaredClass& Class, const FDasClassSchema& Schema,
                                const FString& Script, bool BodyOnly, FString& Error)
{
    if (!DasGenericScriptParent(Class.NativeParent))
        return true;
    auto* Data = DasGenericClassData(BP);
    if (BodyOnly)
    {
        if (!Data)
        {
            Error = TEXT("Generic Blueprint is missing its cooked DAS contract");
            return false;
        }
        return true;
    }
    if (Data)
    {
        if (!DasCanReimportGenericBlueprint(BP))
        {
            Error = TEXT("Structural reimport of this generic parent is not supported");
            return false;
        }
        Data->Modify();
        Data->AuthoredTypeId = Class.Id;
        Data->LifecycleScript = Script;
        Data->MethodScripts.Reset();
        for (const auto& Method : Schema.EffectiveMethods())
            Data->MethodScripts.Add(Method.EntryPoint, Method.ScriptFile);
        Data->LifecycleMask = 0;
        return true;
    }
    if (FindObject<UObject>(BP->GetOutermost(), TEXT("DasScriptClassData")))
    {
        Error = TEXT("DAS class-contract export name is already occupied");
        return false;
    }
    auto* Parent = BP->ParentClass.Get();
    if (auto* Actor = Cast<AActor>(Parent->GetDefaultObject()))
        if (Actor->GetRootComponent() && Class.Components.ContainsByPredicate([](const auto& C) { return C.bRoot; }))
        {
            Error = TEXT("Generic parents retain their inherited native root component");
            return false;
        }
    for (const auto& Hook : Class.Callbacks)
        if (Hook.Kind != TEXT("event") && (!Parent->IsChildOf<AActor>() && !Parent->IsChildOf<UActorComponent>()))
        {
            Error = TEXT("BeginPlay/Tick/EndPlay require an Actor or ActorComponent parent");
            return false;
        }
    Data = NewObject<UDasScriptClassData>(BP->GetOutermost(), TEXT("DasScriptClassData"), RF_Public | RF_Transactional);
    Data->OwnerClass = BP->GeneratedClass;
    Data->AuthoredTypeId = Class.Id;
    Data->LifecycleScript = Script;
    for (const auto& M : Schema.EffectiveMethods())
        Data->MethodScripts.Add(M.EntryPoint, M.ScriptFile);
    // Each generated call has a hard reference to Data, so its contract is serialized and cooked with the class.
    // Actor/component hooks use reflected UE events, without a companion tick component or global object scan.
    if (!Parent->IsChildOf<AActor>() && !Parent->IsChildOf<UActorComponent>())
        return true;
    auto* Graph = FBlueprintEditorUtils::CreateNewGraph(BP, TEXT("DasLifecycle"), UEdGraph::StaticClass(),
                                                                 UEdGraphSchema_K2::StaticClass());
    FBlueprintEditorUtils::AddUbergraphPage(BP, Graph);
    for (int32 Kind = 0; Kind != 3; ++Kind)
    {
        const FString Name = Kind == 0 ? TEXT("BeginPlay") : Kind == 1 ? TEXT("Tick") : TEXT("EndPlay");
        const bool Needed = Class.Callbacks.ContainsByPredicate([&](const auto& H) { return H.Kind == Name; });
        // Ensure managed event subscriptions are initialized and contexts are released at EndPlay too.
        const bool Events =
            Kind == 0 && Class.Callbacks.ContainsByPredicate([](const auto& H) { return H.Kind == TEXT("event"); });
        if (!Needed && !Events && Kind != 2)
            continue;
        if (Needed)
            Data->LifecycleMask |= uint8(1 << Kind);
        const FName EventName =
            Kind == 0 ? TEXT("ReceiveBeginPlay") : Kind == 1 ? TEXT("ReceiveTick") : TEXT("ReceiveEndPlay");
        auto* Function = Parent->FindFunctionByName(EventName);
        if (!Function || !Function->HasAnyFunctionFlags(FUNC_BlueprintEvent))
        {
            Error = TEXT("Parent does not expose the requested Blueprint lifecycle event: ") + EventName.ToString();
            return false;
        }
        FGraphNodeCreator<UK2Node_Event> EventCreator(*Graph);
        auto* Event = EventCreator.CreateNode();
        Event->EventReference.SetExternalMember(EventName, Function->GetOwnerClass());
        Event->bOverrideFunction = true;
        EventCreator.Finalize();
        FGraphNodeCreator<UK2Node_CallParentFunction> ParentCreator(*Graph);
        auto* ParentCall = ParentCreator.CreateNode();
        ParentCall->SetFromFunction(Function);
        ParentCreator.Finalize();
        FGraphNodeCreator<UK2Node_CallFunction> CallCreator(*Graph);
        auto* Call = CallCreator.CreateNode();
        Call->SetFromFunction(UDasObjectScriptLibrary::StaticClass()->FindFunctionByName(
            GET_FUNCTION_NAME_CHECKED(UDasObjectScriptLibrary, ExecuteObjectLifecycle)));
        CallCreator.Finalize();
        auto* K2 = Graph->GetSchema();
        K2->TrySetDefaultValue(*Call->FindPinChecked(TEXT("Kind")), FString::FromInt(Kind));
        K2->TrySetDefaultValue(*Call->FindPinChecked(TEXT("DeltaSeconds")), TEXT("0.0"));
        K2->TrySetDefaultValue(*Call->FindPinChecked(TEXT("Reason")), TEXT("Destroyed"));
        bool Linked = DasWireGenericOwner(Call, BP);
        if (UDasScriptClassData::Find(Parent))
        {
            Linked &= K2->TryCreateConnection(Event->FindPinChecked(UEdGraphSchema_K2::PN_Then), Call->GetExecPin());
            ParentCall->DestroyNode();
            ParentCall = nullptr;
        }
        else
        {
            Linked &=
                K2->TryCreateConnection(Event->FindPinChecked(UEdGraphSchema_K2::PN_Then), ParentCall->GetExecPin());
            Linked &= K2->TryCreateConnection(ParentCall->GetThenPin(), Call->GetExecPin());
        }
        for (auto* Pin : Event->Pins)
            if (Pin->Direction == EGPD_Output && Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec)
                if (auto* Input = ParentCall ? ParentCall->FindPin(Pin->PinName) : nullptr)
                    Linked &= K2->TryCreateConnection(Pin, Input);
        if (Kind == 1)
            Linked &= K2->TryCreateConnection(Event->FindPinChecked(TEXT("DeltaSeconds")),
                                                                    Call->FindPinChecked(TEXT("DeltaSeconds")));
        if (Kind == 2)
            Linked &= K2->TryCreateConnection(Event->FindPinChecked(TEXT("EndPlayReason")),
                                                                    Call->FindPinChecked(TEXT("Reason")));
        if (!Linked)
        {
            Error = TEXT("Cannot wire generic Blueprint lifecycle: ") + Name;
            return false;
        }
    }
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
    return true;
}
void DasFinishGenericBlueprint(UBlueprint* BP)
{
    // Recompilation can replace the generated class of a dependent Blueprint,
    // not just the explicitly imported one. Its cooked contract must follow
    // that replacement, or inherited default ownership loses the child's ID.
    for (TObjectIterator<UBlueprint> It; It; ++It)
    {
        auto* Candidate = *It;
        auto* Data = DasGenericClassData(Candidate);
        if (!Data || !Candidate->GeneratedClass)
            continue;
        if (Candidate != BP &&
            Candidate->GetOutermost()->GetMetaData().GetValue(Candidate, TEXT("DasDSLClassId")) != Data->AuthoredTypeId)
            continue;
        if (Data->OwnerClass != Candidate->GeneratedClass)
        {
            Data->OwnerClass = Candidate->GeneratedClass;
            Data->MarkPackageDirty();
        }
    }
}
