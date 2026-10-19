#include "DasDelegateSignatures.h"
#include "UObject/TextProperty.h"
#include "DasAuthoring.h"
#include "DasPropertyTypes.h"
#include "DasSchema.h"
#include "UObject/Package.h"
#include "UObject/UnrealType.h"
#include "DasContainerTypes.h"

UFunction* MakeDasDelegateSignature(const FDasDeclaredMethod& M, bool Multicast, UObject* Outer, FName Name,
                                    FString& Error)
{
    Error.Reset();
    if (M.Parameters.Num() > 16 || (Multicast && M.ReturnType.Kind != TEXT("void")))
    {
        Error = TEXT("Delegates support up to 16 inputs; multicast events cannot return a value");
        return nullptr;
    }
    for (const auto& P : M.Parameters)
        if ((P.Direction != TEXT("in") && P.Direction != TEXT("inout") && P.Direction != TEXT("out")) ||
                                              (Multicast && P.Direction == TEXT("out")))
        {
            Error = TEXT("Invalid delegate direction; multicast supports in/ref, not out-only parameters");
            return nullptr;
        }
    auto* F = NewObject<UFunction>(Outer, Name, RF_Transient);
    F->FunctionFlags = FUNC_Public | FUNC_Delegate | (Multicast ? FUNC_MulticastDelegate : FUNC_None);
    TArray<TPair<FName, FDasDeclaredType>> Params;
    for (const auto& P : M.Parameters)
        Params.Emplace(*P.Name, P.Type);
    if (M.ReturnType.Kind != TEXT("void"))
        Params.Emplace(TEXT("ReturnValue"), M.ReturnType);
    // Kismet compiles multicast array inputs as refs; mark input-only arrays
    // const so queued listeners cannot mutate the broadcaster's data.
    for (int32 I = Params.Num() - 1; I >= 0; --I)
    {
        auto* P = CreateDasProperty(F, Params[I].Key, Params[I].Value, EDasPropertyUsage::DelegateParameter, Error);
        if (!P)
            return nullptr;
        P->SetPropertyFlags(CPF_Parm);
        if (Params[I].Key == TEXT("ReturnValue"))
            P->SetPropertyFlags(CPF_ReturnParm | CPF_OutParm);
        else if (M.Parameters[I].Direction == TEXT("out"))
            P->SetPropertyFlags(CPF_OutParm);
        else if (M.Parameters[I].Direction == TEXT("inout"))
            P->SetPropertyFlags(CPF_OutParm | CPF_ReferenceParm);
        else if (Multicast && Params[I].Value.GetKind() == EDasTypeKind::Array)
            P->SetPropertyFlags(CPF_ConstParm | CPF_OutParm | CPF_ReferenceParm);
        F->AddCppProperty(P);
    }
    F->Bind();
    F->StaticLink(true);
    F->InitializeDerivedMembers();
    return F;
}

UFunction* MakeDasSingleDelegateSignature(const FString& Wire, FString& Error)
{
    FDasAuthoringModule Model;
    if (Wire.Len() > 131072 || !FDasAuthoringModule::FromJson(Wire, Model, Error) || Model.Classes.Num() != 1 ||
        Model.Classes[0].Methods.Num() != 1)
    {
        if (Error.IsEmpty())
            Error = TEXT("Invalid single delegate signature contract");
        return nullptr;
    }
    return MakeDasDelegateSignature(Model.Classes[0].Methods[0], false, GetTransientPackage(), NAME_None, Error);
}
