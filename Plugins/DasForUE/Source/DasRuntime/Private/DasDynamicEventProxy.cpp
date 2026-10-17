#include "DasIntDelegateProxy.h"
#include "DasCallbackTypes.h"
#include "DasRuntime.h"
#include "DasCallbackFrames.h"
#include "UObject/StructOnScope.h"
#include "DasPropertyValues.h"
#include "Modules/ModuleManager.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "UObject/Script.h"

namespace {
TArray<TStrongObjectPtr<UFunction>> DynamicReceivers;
// Duplication clears CPF_ComputedFlags. Linking container properties does not
// restore every native UHT flag (notably array ZeroConstructor), so retain the
// source's computed traits on this otherwise identical signature clone.
void RestoreReceiverTraits(const FProperty* Source,FProperty* Target){
    if(!Source || !Target || !Source->SameType(Target))return;
    Target->ClearPropertyFlags(CPF_ComputedFlags);
    Target->SetPropertyFlags(Source->PropertyFlags&CPF_ComputedFlags);
    if(auto* A=CastField<FArrayProperty>(Source))RestoreReceiverTraits(A->Inner,CastFieldChecked<FArrayProperty>(Target)->Inner);
    else if(auto* S=CastField<FSetProperty>(Source))RestoreReceiverTraits(S->ElementProp,CastFieldChecked<FSetProperty>(Target)->ElementProp);
    else if(auto* M=CastField<FMapProperty>(Source)){auto* T=CastFieldChecked<FMapProperty>(Target);RestoreReceiverTraits(M->KeyProp,T->KeyProp);RestoreReceiverTraits(M->ValueProp,T->ValueProp);}
}

FProperty* ReceiverParameter(UFunction* F){if(F)for(TFieldIterator<FProperty> It(F);It;++It)if(It->HasAnyPropertyFlags(CPF_Parm) && !It->HasAnyPropertyFlags(CPF_ReturnParm))return *It;return nullptr;}
}

FName UDasIntDelegateProxy::ConfigureDynamic(UFunction* Signature,const FString& Path,EDasMethodSignature Kind,FString& Error)
{
    check(IsInGameThread());Error.Reset();
    auto* Frame=FindDasCallbackFrame(Path);
    if(Frame){if(Kind!=EDasMethodSignature::Struct || !ValidateDasCallbackFrameSource(Signature,Frame,Error))return NAME_None;}
    else {
    const auto* Type=DasCallbackTypeForProperty(ReceiverParameter(Signature));
    if(!Signature || Signature->NumParms!=1 || Signature->GetReturnProperty() || !Type || !Type->bDynamic || Type->Signature!=Kind || !ValidateDasCallbackTypePath(Kind==EDasMethodSignature::Struct?TEXT("struct"):Type->Name,Path,Error))
    {if(Error.IsEmpty())Error=TEXT("Unsupported dynamic event receiver signature");return NAME_None;}
    auto* Parameter=ReceiverParameter(Signature);
    if(Parameter->HasAnyPropertyFlags(CPF_OutParm|CPF_ReferenceParm) && !Parameter->HasAnyPropertyFlags(CPF_ConstParm)){Error=TEXT("Queued event receivers reject mutable ref/out parameters");return NAME_None;}
    const FString Actual=Kind==EDasMethodSignature::Object?DasReferencePropertyClass(Parameter)->GetPathName():Kind==EDasMethodSignature::Enum?DasEnumPropertyType(Parameter)->GetPathName():DasCallbackSnapshotForProperty(Parameter)->GetPathName();
    if(Actual!=Path){Error=TEXT("Dynamic event type identity mismatch");return NAME_None;}
    }
    auto* Parameter=ReceiverParameter(Signature);
    for(const auto& Existing:DynamicReceivers)if(Existing->IsSignatureCompatibleWith(Signature,0)){
        DynamicKind=int32(Kind);DynamicPath=Path;return Existing->GetFName();
    }
    if(DynamicReceivers.Num()>=1024){Error=TEXT("Dynamic event receiver signature limit exceeded");return NAME_None;}
    auto* Class=StaticClass();const FName Name=MakeUniqueObjectName(Class,UFunction::StaticClass(),TEXT("__DasValueEvent"));
    auto* Function=DuplicateObject<UFunction>(Signature,Class,Name);
    Function->SetFlags(RF_Transient);
    Function->Script.Empty();
#if UE_BLUEPRINT_EVENTGRAPH_FASTCALLS
    Function->EventGraphFunction=nullptr;Function->EventGraphCallOffset=0;
#endif
    Function->FunctionFlags=FUNC_Public|FUNC_Native|(Signature->FunctionFlags&FUNC_HasOutParms);
    Function->StaticLink(true);Function->InitializeDerivedMembers();
    {TFieldIterator<FProperty> A(Signature),B(Function);for(;A && B;++A,++B)RestoreReceiverTraits(*A,*B);}
    Function->SetNativeFunc(&UDasIntDelegateProxy::execReceiveDynamic);
    if(Function->GetPropertiesSize()<Function->ParmsSize || Function->ParmsSize!=Signature->ParmsSize || !Function->IsSignatureCompatibleWith(Signature,0)){auto* Receiver=ReceiverParameter(Function);Error=FString::Printf(TEXT("Dynamic event receiver layout mismatch for %s: property-size=%d frame=%d/%d flags=%llx/%llx compatible=%d"),*Signature->GetPathName(),Function->GetPropertiesSize(),Function->ParmsSize,Signature->ParmsSize,Receiver?uint64(Receiver->PropertyFlags):0,uint64(Parameter->PropertyFlags),Function->IsSignatureCompatibleWith(Signature,0));return NAME_None;}
    // The native proxy class may be in the cooked DisregardForGC pool; its
    // dynamic function-map targets must be explicitly rooted, not merely strong.
    Function->AddToRoot();
    Class->AddFunctionToFunctionMap(Function,Name);DynamicReceivers.Emplace(Function);
    DynamicKind=int32(Kind);DynamicPath=Path;return Name;
}
void UDasIntDelegateProxy::ResetDynamicReceivers()
{
    check(IsInGameThread());for(const auto& Function:DynamicReceivers){StaticClass()->RemoveFunctionFromFunctionMap(Function.Get());Function->RemoveFromRoot();}DynamicReceivers.Empty();
}
DEFINE_FUNCTION(UDasIntDelegateProxy::execReceiveDynamic)
{
    // Native delegate calls already provide a linked parameter frame. Unlike
    // bytecode Step(), StepExplicitProperty does not set MostRecentProperty and
    // requires destination storage, so do not use StepCompiledIn(nullptr) here.
    if(auto* Frame=FindDasCallbackFrame(P_THIS->DynamicPath)){
        FStructOnScope Evaluated(Stack.Node);const void* NativeMemory=Stack.Locals;
        if(Stack.Code){for(TFieldIterator<FProperty> I(Stack.Node);I;++I)if(I->HasAnyPropertyFlags(CPF_Parm))Stack.StepExplicitProperty(I->ContainerPtrToValuePtr<void>(Evaluated.GetStructMemory()),*I);NativeMemory=Evaluated.GetStructMemory();}
        else Stack.PropertyChainForCompiledIn=nullptr;
        P_FINISH;
        FDasScalarValue Value;FString Error;
        if(IsInGameThread() && CaptureDasCallbackFrame(Stack.Node,NativeMemory,Frame,Value,Error))if(auto* Runtime=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))Runtime->DispatchScalarEvent(P_THIS->SubscriptionId,MoveTemp(Value));
        return;
    }
    FProperty* Property=nullptr;const void* Memory=nullptr;
    if(!Stack.Code){Property=CastField<FProperty>(Stack.PropertyChainForCompiledIn);if(Property && Stack.Locals){Memory=Property->ContainerPtrToValuePtr<void>(Stack.Locals);Stack.PropertyChainForCompiledIn=Property->Next;}}
    else{Stack.MostRecentProperty=nullptr;Stack.StepCompiledIn<FProperty>(nullptr);Property=Stack.MostRecentProperty;Memory=Stack.MostRecentPropertyAddress;}
    P_FINISH;
    P_NATIVE_BEGIN;
    UE_LOG(LogTemp,VeryVerbose,TEXT("DYNAMIC_EVENT id=%llu kind=%d property=%s memory=%d code=%d"),P_THIS->SubscriptionId,P_THIS->DynamicKind,Property?*Property->GetName():TEXT("null"),Memory!=nullptr,Stack.Code!=nullptr);
    if(IsInGameThread() && Property && Memory){
        FDasScalarValue Value;Value.Kind=P_THIS->DynamicKind;Value.ObjectClass=P_THIS->DynamicPath;
        if(Value.Kind==int32(EDasMethodSignature::Object)){Value.ObjectValue=DasReferenceGet(Property,Memory);if(auto* Runtime=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))Runtime->DispatchScalarEvent(P_THIS->SubscriptionId,MoveTemp(Value));}
        else if(Value.Kind==int32(EDasMethodSignature::Enum)){
            auto* Enum=DasEnumPropertyType(Property);
            if(Enum && Enum->GetPathName()==Value.ObjectClass){const int64 Number=DasEnumPropertyNumber(Property,Memory);if(Number>=0 && Number<=254){Value.IntValue=int32(Number);if(auto* Runtime=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))Runtime->DispatchScalarEvent(P_THIS->SubscriptionId,MoveTemp(Value));}}
        }else if(Value.Kind==int32(EDasMethodSignature::Struct)){
            FString Error;FDasScalarValue Snapshot;
            if(ReadDasPropertyValue(Property,Memory,Snapshot,Error) && Snapshot.Kind==Value.Kind && Snapshot.ObjectClass==Value.ObjectClass)
                if(auto* Runtime=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))Runtime->DispatchScalarEvent(P_THIS->SubscriptionId,MoveTemp(Snapshot));
        }
    }
    P_NATIVE_END;
}
