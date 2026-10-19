#include "DasScriptHosts.h"
#include "DasNativeDefaults.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "DasAuthoring.h"
#include "DasProbeActor.h"
#include "DasScriptComponent.h"
#include "Components/SceneComponent.h"
#include "UObject/UnrealType.h"
#include "Misc/SecureHash.h"
UClass* DasNativeDefaultBase(const FString& Parent)
{
    return Parent == TEXT("/Script/Engine.Actor")
                          ? AActor::StaticClass()
                          : Parent == TEXT("/Script/Engine.Pawn")
                                           ? APawn::StaticClass()
                                           : Parent == TEXT("/Script/Engine.Character")
                                                            ? ACharacter::StaticClass()
                                                            : Parent == TEXT("/Script/Engine.ActorComponent")
                                                                             ? UActorComponent::StaticClass()
                                                                             : nullptr;
}
UObject* DasNativeDefaultTarget(const FString& Parent, UObject* Host, const FString& Component, FString& Error)
{
    auto* Base = DasNativeDefaultBase(Parent);
    if (!Base || !Host || !Host->IsA(Base))
    {
        Error = TEXT("Unsupported native defaults host");
        return nullptr;
    }
    if (Component.IsEmpty())
        return Host;
    auto* Ref = CastField<FObjectPropertyBase>(Base->FindPropertyByName(*Component));
    UObject* Profile = DasNativeScriptHost(Parent)->GetDefaultObject();
    auto* Native = Ref ? Cast<UActorComponent>(Ref->GetObjectPropertyValue_InContainer(Profile)) : nullptr;
    if (!Native || !Native->HasAnyFlags(RF_DefaultSubObject) || Native->GetOuter() != Profile)
    {
        Error = TEXT("Native defaults require a built-in component reference: ")+ Component;
        return nullptr;
    }
    auto* Target = Host->GetDefaultSubobjectByName(Native->GetFName());
    if (!Target || Target->GetClass() != Native->GetClass() || Target->GetOuter() != Host ||
        !Target->HasAnyFlags(RF_DefaultSubObject))
    {
        Error = TEXT("Native default subobject identity differs: ")+ Component;
        return nullptr;
    }
    return Target;
}
bool DasValidateNativeDefaults(const FDasDeclaredClass& C, FString& Error)
{
    if (C.NativeDefaults.IsEmpty())
        return true;
    auto* Base = DasNativeDefaultBase(C.NativeParent);
    if (!Base)
    {
        Error = TEXT("Native defaults require Actor/ActorComponent host");
        return false;
    }
    UObject* Profile = DasNativeScriptHost(C.NativeParent)->GetDefaultObject();
    for (const auto& G : C.NativeDefaults)
    {
        // Native root references must keep their meaning; SCS roots have a separate contract.
        if (!G.Component.IsEmpty() && !C.Components.IsEmpty())
        {
            Error = TEXT("Native subobject defaults with SCS composition require a later identity policy");
            return false;
        }
        if (!G.Component.IsEmpty())
            if (auto* Reference = Base->FindPropertyByName(*G.Component))
                if (!Reference->GetName().Equals(G.Component, ESearchCase::CaseSensitive))
                {
                    Error = TEXT("Native defaults require canonical component reference name: ")+ Reference->GetName();
                    return false;
                }
        auto* Target = DasNativeDefaultTarget(C.NativeParent, Profile, G.Component, Error);
        if (!Target)
            return false;
        for (const auto& V : G.Defaults)
        {
            auto* P = (G.Component.IsEmpty() ? Base : Target->GetClass())->FindPropertyByName(*V.Key);
            if (P && !P->GetName().Equals(V.Key, ESearchCase::CaseSensitive))
            {
                Error = TEXT("Native defaults require canonical property name: ")+ P->GetName();
                return false;
            }
            if (!P || !P->GetOwnerClass() || !P->GetOwnerClass()->HasAnyClassFlags(CLASS_Native) ||
                (G.Component.IsEmpty() &&
                 C.Properties.ContainsByPredicate([&](const auto& X)
                                                  { return X.Name.Equals(V.Key, ESearchCase::IgnoreCase); })) ||
                !DasParseEditableDefault(P, V.Value, nullptr, Error))
            {
                if (Error.IsEmpty())
                    Error = TEXT("Unknown, shadowed or inaccessible native default: ")+ V.Key;
                return false;
            }
        }
    }
    return true;
}
FString DasNativeDefaultKey(const FString& Id, const FString& Component, const FString& Property)
{
    return TEXT("native:")+ Id + TEXT(":")+ Component + TEXT(".")+ Property;
}
FString DasNativeDefaultContract(const FDasDeclaredLiteral& V)
{
    FDasDeclaredProperty P;
    P.Id = TEXT("native-default-v1");
    P.Default = V;
    return DasDefaultSourceContract(P);
}
bool DasCheckNativeDefaultOwner(const FDasAuthoringModule& M, const FString& Id, UObject* Self, FString& Error)
{
    const auto* C = M.Classes.FindByPredicate([&](const auto& X) { return X.Id == Id; });
    FDasDeclaredClass Effective;
    if (!C || !M.ResolveClass(Id, Effective, Error))
        return false;
    auto Identity = [](UObject* O)
    {
        return DasHostIdentity(O);
    };
    for (const auto* Source = C; Source;
         Source = Source->ParentId.IsEmpty()
                      ? nullptr
                      : M.Classes.FindByPredicate([&](const auto& X) { return X.Id == Source->ParentId; }))
    {
        UObject* CDO = nullptr;
        for (auto* Class = Self->GetClass(); Class; Class = Class->GetSuperClass())
            if (Identity(Class->GetDefaultObject()) == Source->Id)
                CDO = Class->GetDefaultObject();
        if (!CDO)
        {
            if (Source->NativeDefaults.IsEmpty())
                continue;
            Error = TEXT("Missing native default owner");
            return false;
        }
        const auto* Contracts = DasHostContracts(CDO);
        TSet<FString> Expected;
        for (const auto& G : Source->NativeDefaults)
        {
            auto* Target = DasNativeDefaultTarget(Effective.NativeParent, CDO, G.Component, Error);
            if (!Target)
                return false;
            for (const auto& V : G.Defaults)
            {
                const auto Key = DasNativeDefaultKey(Source->Id, G.Component, V.Key);
                Expected.Add(Key);
                if (!DasParseEditableDefault(Target->GetClass()->FindPropertyByName(*V.Key), V.Value, nullptr, Error) ||
                    Contracts->FindRef(Key) != DasNativeDefaultContract(V.Value))
                {
                    Error = TEXT("Native defaults differ from imported source; reimport Blueprint: ")+ Key;
                    return false;
                }
            }
        }
        for (const auto& V : *Contracts)
            if (V.Key.StartsWith(TEXT("native:")+ Source->Id + TEXT(":")) && !Expected.Contains(V.Key))
            {
                Error = TEXT("Removed native defaults require reimport: ")+ V.Key;
                return false;
            }
    }
    return true;
}
