#include "DasNativeDelegateReceiver.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
namespace
{
TArray<TStrongObjectPtr<UFunction>> NativeReceivers;
// Duplication clears CPF_ComputedFlags. Linking container properties does not
// restore every native UHT flag (notably array ZeroConstructor), so retain the
// source's computed traits on this otherwise identical signature clone.
void RestoreReceiverTraits(const FProperty* Source, FProperty* Target)
{
    if (!Source || !Target || !Source->SameType(Target))
        return;
    Target->ClearPropertyFlags(CPF_ComputedFlags);
    Target->SetPropertyFlags(Source->PropertyFlags & CPF_ComputedFlags);
    if (auto* A = CastField<FArrayProperty>(Source))
        RestoreReceiverTraits(A->Inner, CastFieldChecked<FArrayProperty>(Target)->Inner);
    else if (auto* S = CastField<FSetProperty>(Source))
        RestoreReceiverTraits(S->ElementProp, CastFieldChecked<FSetProperty>(Target)->ElementProp);
    else if (auto* M = CastField<FMapProperty>(Source))
    {
        auto* T = CastFieldChecked<FMapProperty>(Target);
        RestoreReceiverTraits(M->KeyProp, T->KeyProp);
        RestoreReceiverTraits(M->ValueProp, T->ValueProp);
    }
}

}
UFunction* MakeDasNativeDelegateReceiver(UClass* Class, UFunction* Signature, FNativeFuncPtr Thunk, FString& Error)
{
    check(IsInGameThread());
    for (const auto& Existing : NativeReceivers)
        if (Existing->GetOuter() == Class && Existing->GetNativeFunc() == Thunk &&
            Existing->IsSignatureCompatibleWith(Signature, 0))
        {
            return Existing.Get();
        }
    if (NativeReceivers.Num() >= 1024)
    {
        Error = TEXT("Dynamic event receiver signature limit exceeded");
        return nullptr;
    }
    const FName Name = MakeUniqueObjectName(Class, UFunction::StaticClass(), TEXT("__DasValueEvent"));
    auto* Function = DuplicateObject<UFunction>(Signature, Class, Name);
    Function->SetFlags(RF_Transient);
    Function->Script.Empty();
#if UE_BLUEPRINT_EVENTGRAPH_FASTCALLS
    Function->EventGraphFunction = nullptr;
    Function->EventGraphCallOffset = 0;
#endif
    Function->FunctionFlags = FUNC_Public | FUNC_Native;
    // Authored delegate signatures carry property directions; UHT's derived function flag may be absent.
    for (TFieldIterator<FProperty> It(Function); It; ++It)
        if (It->HasAllPropertyFlags(CPF_Parm | CPF_OutParm))
            Function->FunctionFlags |= FUNC_HasOutParms;
    Function->StaticLink(true);
    Function->InitializeDerivedMembers();
    {
        TFieldIterator<FProperty> A(Signature), B(Function);
        for (; A && B; ++A, ++B)
            RestoreReceiverTraits(*A, *B);
    }
    Function->SetNativeFunc(Thunk);
    if (Function->GetPropertiesSize() < Function->ParmsSize || Function->ParmsSize != Signature->ParmsSize ||
        !Function->IsSignatureCompatibleWith(Signature, 0))
    {
        Error = TEXT("Native delegate receiver signature/layout mismatch");
        return nullptr;
    }
    // The native proxy class may be in the cooked DisregardForGC pool; its
    // dynamic function-map targets must be explicitly rooted, not merely strong.
    Function->AddToRoot();
    Class->AddFunctionToFunctionMap(Function, Name);
    NativeReceivers.Emplace(Function);
    return Function;
}

void ResetDasNativeDelegateReceivers(UClass* Class)
{
    check(IsInGameThread());
    for (int32 I = NativeReceivers.Num() - 1; I >= 0; --I)
        if (NativeReceivers[I]->GetOuter() == Class)
        {
            Class->RemoveFunctionFromFunctionMap(NativeReceivers[I].Get());
            NativeReceivers[I]->RemoveFromRoot();
            NativeReceivers.RemoveAt(I);
        }
}
