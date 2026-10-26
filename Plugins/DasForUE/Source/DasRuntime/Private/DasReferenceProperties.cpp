#include "DasInterfaceContracts.h"
#include "DasSchema.h"
#include "DasTypeAdapters.h"
#include "UObject/UnrealType.h"
#include "UObject/ScriptInterface.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
bool DasHasDirectWeakContainer(const FProperty* P)
{
    if (const auto* Adapter = FDasTypeAdapters::Get().FindProperty(P))
        for (const auto& Slot : Adapter->GetContainerSlots())
            if (CastField<FWeakObjectProperty>(Slot.ReadProperty(P)))
                return true;
    return false;
}
UClass* DasReferencePropertyClass(const FProperty* P)
{
    if (auto* I = CastField<FInterfaceProperty>(P))
        return I->InterfaceClass.Get();
    if (auto* W = CastField<FWeakObjectProperty>(P))
        return W->PropertyClass.Get();
    if (auto* O = CastField<FObjectProperty>(P); O && !CastField<FClassProperty>(P))
        return O->PropertyClass.Get();
    return nullptr;
}
bool DasReferenceMatches(const UObject* O, const UClass* C)
{
    return C && (!O || (C->HasAnyClassFlags(CLASS_Interface) ? DasImplementsInterface(O->GetClass(), C) : O->IsA(C)));
}
UObject* DasReferenceGet(const FProperty* P, const void* M)
{
    if (!P || !M)
        return nullptr;
    if (auto* I = CastField<FInterfaceProperty>(P))
        return I->GetPropertyValue(M).GetObject();
    if (auto* W = CastField<FWeakObjectProperty>(P))
    {
        auto* O = W->GetObjectPropertyValue(M);
        if (!IsValid(O))
            return nullptr;
        if (auto* A = Cast<AActor>(O); A && A->IsActorBeingDestroyed())
            return nullptr;
        if (auto* C = Cast<UActorComponent>(O); C && C->IsBeingDestroyed())
            return nullptr;
        return O;
    }
    if (auto* O = CastField<FObjectProperty>(P); O && !CastField<FClassProperty>(P))
        return O->GetObjectPropertyValue(M);
    return nullptr;
}
bool DasReferenceSet(const FProperty* P, void* M, UObject* O)
{
    auto* C = DasReferencePropertyClass(P);
    if (!M || !DasReferenceMatches(O, C))
        return false;
    if (auto* I = CastField<FInterfaceProperty>(P))
    {
        FScriptInterface Value;
        Value.SetObject(O);
        Value.SetInterface(O ? O->GetInterfaceAddress(C) : nullptr);
        I->SetPropertyValue(M, Value);
    }
    else if (auto* W = CastField<FWeakObjectProperty>(P))
        W->SetObjectPropertyValue(M, O);
    else
        CastFieldChecked<FObjectProperty>(P)->SetObjectPropertyValue(M, O);
    return true;
}
