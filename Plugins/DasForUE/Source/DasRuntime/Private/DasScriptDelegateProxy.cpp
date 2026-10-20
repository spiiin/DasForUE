#include "DasScriptDelegateProxy.h"
#include "DasNativeDelegateReceiver.h"
#include "DasPropertyValues.h"
#include "DasCallbackTypes.h"
#include "DasRuntime.h"
#include "DasFrameLibrary.h"
#include "UObject/Script.h"
#include "UObject/StructOnScope.h"
#include "UObject/StrongObjectPtr.h"
#include "Modules/ModuleManager.h"
#include "Blueprint/BlueprintExceptionInfo.h"
FName UDasScriptDelegateProxy::Configure(UFunction* Signature, FString& Error)
{
    auto* F = MakeDasNativeDelegateReceiver(StaticClass(), Signature, &UDasScriptDelegateProxy::execInvoke, Error);
    return F ? F->GetFName() : NAME_None;
}
bool UDasScriptDelegateProxy::IsActive() const
{
    if (!IsInGameThread())
        return false;
    auto Generation = Lease.Pin();
    return Generation.IsValid() && Generation->Active && DasCallbackObjectAlive(Owner.Get());
}
void UDasScriptDelegateProxy::ResetReceivers()
{
    ResetDasNativeDelegateReceivers(StaticClass());
}
FDasOwnedScriptDelegate::~FDasOwnedScriptDelegate()
{
    if (auto* S = Source.Get())
        if (auto* P = FindFProperty<FMulticastDelegateProperty>(S->GetClass(), Event))
            P->RemoveDelegate(Delegate, S, P->ContainerPtrToValuePtr<void>(S));
}
bool UDasScriptDelegateProxy::Invoke(UFunction* Signature, const void* Input, void* Staged, FString& Error)
{
    if (!IsActive())
    {
        Error = TEXT("Script delegate owner or VM generation was revoked");
        return false;
    }
    auto* R = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"));
    if (!R)
    {
        Error = TEXT("Runtime is unavailable");
        return false;
    }
    FDasPropertyValueRoots Roots;
    TArray<FDasScalarValue> Args;
    int32 Index = 0;
    for (TFieldIterator<FProperty> It(Signature); It; ++It)
        if (It->HasAnyPropertyFlags(CPF_Parm) && !It->HasAnyPropertyFlags(CPF_ReturnParm))
        {
            FDasScalarValue V;
            if (Schema.Parameters[Index].Direction == TEXT("out"))
                V = UDasFrameLibrary::DefaultValue(int32(Schema.Parameters[Index].Kind),
                                                   Schema.Parameters[Index].ClassPath, 1);
            else if (!ReadDasPropertyValue(*It, It->ContainerPtrToValuePtr<void>(Input), V, Error))
                return false;
            V.Direction = Schema.Parameters[Index].Direction ==
                          TEXT("out")? 1 : Schema.Parameters[Index].Direction == TEXT("inout")? 2 : 0;
            Args.Add(V);
            Roots.Keep(MoveTemp(V));
            ++Index;
        }
    const auto Result =
        R->RunScriptDelegateFrame(Lease.Pin(), Script, Entry, Args, Schema.ReturnKind, Owner, Schema.ReturnClass);
    if (!Result.bSucceeded)
    {
        Error = Result.Error;
        return false;
    }
    if (!IsActive())
    {
        Error = TEXT("Script delegate generation was revoked during callback");
        return false;
    }
    FDasScalarValue Ret;
    Ret.Kind = int32(Schema.ReturnKind);
    Ret.ObjectClass = Schema.ReturnClass;
    Ret.IntValue = Result.Value;
    Ret.FloatValue = Result.FloatValue;
    Ret.DoubleValue = Result.DoubleValue;
    Ret.BoolValue = Result.BoolValue;
    Ret.StringValue = Result.StringValue;
    Ret.NameValue = Result.NameValue;
    Ret.ByteValue = Result.ByteValue;
    Ret.TextValue = Result.TextValue;
    Ret.ObjectValue = Result.ObjectValue.Get();
    Ret.ClassValue = Cast<UClass>(Result.ObjectValue.Get());
    Ret.StructValue = Result.StructValue;
    Ret.DelegateValue = Result.DelegateValue;
    Ret.VectorValue = Result.VectorValue;
    Ret.RotatorValue = Result.RotatorValue;
    Ret.TransformValue = Result.TransformValue;
    Roots.Keep(Ret);
    for (const auto& Out : Result.OutputValues)
        Roots.Keep(Out);
    Index = 0;
    for (TFieldIterator<FProperty> It(Signature); It; ++It)
        if (It->HasAnyPropertyFlags(CPF_Parm))
        {
            if (It->HasAnyPropertyFlags(CPF_ReturnParm))
            {
                if (!WriteDasPropertyValue(*It, It->ContainerPtrToValuePtr<void>(Staged), Ret, Error))
                    return false;
            }
            else
            {
                if (Schema.Parameters[Index].Direction != TEXT("in"))
                {
                    if (!Result.OutputValues.IsValidIndex(Index) ||
                        !WriteDasPropertyValue(*It, It->ContainerPtrToValuePtr<void>(Staged),
                                               Result.OutputValues[Index], Error))
                        return false;
                }
                ++Index;
            }
        }
    return true;
}
DEFINE_FUNCTION(UDasScriptDelegateProxy::execInvoke)
{
    if (!IsInGameThread())
    {
        Stack.PropertyChainForCompiledIn = nullptr;
        return;
    }
    TStrongObjectPtr<UDasScriptDelegateProxy> Keep(P_THIS);
    FStructOnScope Evaluated(Stack.Node), Staged(Stack.Node);
    TMap<FProperty*, void*> References;
    const bool Bytecode = Stack.Code != nullptr;
    const void* Input = Stack.Locals;
    if (Bytecode)
    {
        for (TFieldIterator<FProperty> I(Stack.Node); I; ++I)
            if (I->HasAnyPropertyFlags(CPF_Parm) && !I->HasAnyPropertyFlags(CPF_ReturnParm))
            {
                Stack.MostRecentPropertyAddress = nullptr;
                Stack.StepExplicitProperty(I->ContainerPtrToValuePtr<void>(Evaluated.GetStructMemory()), *I);
                if (I->HasAnyPropertyFlags(CPF_OutParm | CPF_ReferenceParm))
                    References.Add(*I, Stack.MostRecentPropertyAddress);
            }
        Input = Evaluated.GetStructMemory();
    }
    else
        Stack.PropertyChainForCompiledIn = nullptr;
    P_FINISH;
    FString Error;
    if (P_THIS->Invoke(Stack.Node, Input, Staged.GetStructMemory(), Error))
    {
        for (TFieldIterator<FProperty> I(Stack.Node); I; ++I)
            if (I->HasAnyPropertyFlags(CPF_Parm))
            {
                void* Target = nullptr;
                if (I->HasAnyPropertyFlags(CPF_ReturnParm))
                    Target = RESULT_PARAM;
                else if (I->HasAnyPropertyFlags(CPF_OutParm | CPF_ReferenceParm) &&
                         !I->HasAnyPropertyFlags(CPF_ConstParm))
                {
                    Target = Bytecode ? References.FindRef(*I) : I->ContainerPtrToValuePtr<void>(Stack.Locals);
                    // ProcessEvent copies inputs into Locals; out parameters point back to the caller.
                    if (!Bytecode)
                        for (auto* Out = Stack.OutParms; Out; Out = Out->NextOutParm)
                            if (Out->Property == *I)
                            {
                                Target = Out->PropAddr;
                                break;
                            }
                }
                if (Target)
                    I->CopyCompleteValue(Target, I->ContainerPtrToValuePtr<void>(Staged.GetStructMemory()));
            }
    }
    P_THIS->LastError = Error;
    if (!Error.IsEmpty())
    {
        FBlueprintExceptionInfo Info(EBlueprintExceptionType::AbortExecution, FText::FromString(Error));
        FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, Info);
    }
}
