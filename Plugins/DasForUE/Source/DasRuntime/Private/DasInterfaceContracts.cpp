#include "DasAuthoredInterfaceClass.h"
#include "DasTypeAdapters.h"
#include "DasInterfaceContracts.h"
#include "DasSchema.h"
#include "DasContainerTypes.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"
#include "UObject/NoExportTypes.h"
FString DasInterfaceStableId(const UClass* Interface)
{
    const auto* Authored = Cast<UDasAuthoredInterfaceClass>(Interface);
    return Authored ? Authored->StableId : FString();
}
bool DasInterfaceIdentityMatches(const UClass* Interface, const FString* ExpectedStableId)
{
    if (!Interface || !Interface->HasAnyClassFlags(CLASS_Interface))
        return false;
    const auto* Authored = Cast<UDasAuthoredInterfaceClass>(Interface);
    if (Authored)
        return !Authored->StableId.IsEmpty() && (!ExpectedStableId || *ExpectedStableId == Authored->StableId);
    return !ExpectedStableId || ExpectedStableId->IsEmpty();
}
bool DasImplementsInterface(const UClass* Class, const UClass* Interface)
{
    return Class && DasInterfaceIdentityMatches(Interface) && Class->ImplementsInterface(Interface);
}
UFunction* DasResolveInterfaceFunction(UObject* Object, UClass* Interface, FName Name, FString& Error)
{
    if (!Object || !DasImplementsInterface(Object->GetClass(), Interface))
    {
        Error = TEXT("object does not implement requested interface");
        return nullptr;
    }
    auto* Declared = Interface->FindFunctionByName(Name);
    auto* Actual = Object->FindFunction(Name);
    if (!Declared || !Declared->HasAnyFunctionFlags(FUNC_BlueprintEvent) || !Actual)
    {
        Error = TEXT("reflected interface implementation is unavailable");
        return nullptr;
    }
    if (!Actual->IsSignatureCompatibleWith(Declared))
    {
        Error = TEXT("interface dispatch signature mismatch");
        return nullptr;
    }
    return Actual;
}
bool ValidateDasInterfaceContracts(const FDasClassSchema& Schema, FString& Error)
{
    TSet<FString> Seen;
    TSet<FName> MethodNames;
    const auto Methods = Schema.EffectiveMethods();
    for (const auto& Path : Schema.Interfaces)
    {
        auto* I = FindObject<UClass>(nullptr, *Path);
        if ((!Path.StartsWith(TEXT("/Script/")) && !Path.StartsWith(TEXT("/Game/"))) || !I ||
              !DasInterfaceIdentityMatches(I) || Seen.Contains(Path))
        {
            Error = TEXT("Expected unique loaded native UInterface: ")+ Path;
            return false;
        }
        Seen.Add(Path);
#if WITH_EDITOR
        if (I->HasMetaData(TEXT("CannotImplementInterfaceInBlueprint")))
        {
            Error = TEXT("Interface cannot be implemented in Blueprint");
            return false;
        }
#endif
        int32 Count = 0;
        for (TFieldIterator<UFunction> It(I, EFieldIteratorFlags::IncludeSuper); It; ++It)
        {
            auto* F = *It;
            if (!F->GetOwnerClass()->HasAnyClassFlags(CLASS_Interface))
                continue;
            ++Count;
            if (MethodNames.Contains(F->GetFName()))
            {
                Error = TEXT("Overlapping interface method names require a later backend: ")+ F->GetName();
                return false;
            }
            MethodNames.Add(F->GetFName());
            const auto* M =
                Methods.FindByPredicate([&](const auto& Candidate) { return Candidate.Name == F->GetFName(); });
            if (!M || M->bPure != F->HasAnyFunctionFlags(FUNC_BlueprintPure) ||
                M->bConst != F->HasAnyFunctionFlags(FUNC_Const) || !F->HasAnyFunctionFlags(FUNC_BlueprintEvent) ||
                F->HasAnyFunctionFlags(FUNC_Static | FUNC_Net | FUNC_Delegate | FUNC_MulticastDelegate))
            {
                Error = TEXT("Missing/unsupported interface method or purity/const mismatch: ")+ F->GetName();
                return false;
            }
#if WITH_EDITOR
            if (F->HasMetaData(TEXT("Latent")))
            {
                Error = TEXT("Latent interface method requires a later backend");
                return false;
            }
#endif
            const auto Params = M->EffectiveParameters();
            if (F->NumParms != Params.Num() + (M->Contract().ReturnKind == EDasMethodSignature::Void ? 0 : 1))
            {
                Error = TEXT("Interface parameter count mismatch: ")+ F->GetName();
                return false;
            }
            for (const auto& P : Params)
            {
                auto* Actual = F->FindPropertyByName(P.Name);
                if (!DasPropertyMatchesContract(Actual, P.Kind, P.ClassPath))
                {
                    Error =
                        TEXT("Interface parameter type/name mismatch: ")+ F->GetName() + TEXT(".")+ P.Name.ToString();
                    return false;
                }
                const FString Direction =
                    Actual->HasAnyPropertyFlags(CPF_OutParm) && !Actual->HasAnyPropertyFlags(CPF_ConstParm)
                    ? (Actual->HasAnyPropertyFlags(CPF_ReferenceParm) ? TEXT("inout"): TEXT("out"))
                    : TEXT("in");
                if (P.Direction != Direction)
                {
                    Error = TEXT("Interface parameter direction mismatch: ")+ F->GetName();
                    return false;
                }
            }
            if (M->Contract().ReturnKind == EDasMethodSignature::Void
                    ? F->GetReturnProperty() != nullptr
                    : !DasPropertyMatchesContract(F->GetReturnProperty(), M->Contract().ReturnKind, M->ReturnClass))
            {
                Error = TEXT("Interface return type mismatch: ")+ F->GetName();
                return false;
            }
        }
        if (Count == 0)
        {
            Error = TEXT("Interface has no supported methods");
            return false;
        }
    }
    return true;
}
