#include "DasScriptClassData.h"
#include "DasRuntime.h"
#include "DasScriptHosts.h"
#include "Modules/ModuleManager.h"
#include "Blueprint/BlueprintExceptionInfo.h"
#include "UObject/Script.h"
#include "UObject/Package.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"

UDasScriptClassData* UDasScriptClassData::Find(const UClass* Class)
{
    if (!IsInGameThread())
        return nullptr;
    for (auto* C = Class; C && !C->HasAnyClassFlags(CLASS_Native); C = C->GetSuperClass())
    {
        // Package exports are loaded through the generated graph's hard reference. Do not load arbitrary assets here.
        auto* Data = FindObject<UDasScriptClassData>(C->GetOutermost(), TEXT("DasScriptClassData"));
        if (Data && Data->OwnerClass.Get() == C && !Data->AuthoredTypeId.IsEmpty())
            return Data;
    }
    return nullptr;
}
namespace
{
bool ValidOwner(UObject* Owner, const UDasScriptClassData* Data, bool Teardown = false)
{
    return IsInGameThread() && IsValid(Owner) && !Owner->IsTemplate() && IsValid(Data) && Data->OwnerClass &&
           Owner->IsA(Data->OwnerClass) && UDasScriptClassData::Find(Owner->GetClass()) &&
           !Data->AuthoredTypeId.IsEmpty() &&
           (Teardown || ((!Owner->IsA<AActor>() || !CastChecked<AActor>(Owner)->IsActorBeingDestroyed()) &&
                         (!Owner->IsA<UActorComponent>() || !CastChecked<UActorComponent>(Owner)->IsBeingDestroyed())));
}
void Report(UObject* Owner, const FString& Error)
{
    if (auto* Stack = FFrame::GetThreadLocalTopStackFrame())
    {
        FBlueprintExceptionInfo Info(EBlueprintExceptionType::AbortExecution, FText::FromString(Error));
        FBlueprintCoreDelegates::ThrowScriptException(Owner, *Stack, Info);
    }
    else
        UE_LOG(LogTemp, Warning, TEXT("DAS object call rejected: %s"), *Error);
}
FDasScalarValue Execute(UObject* Owner, UDasScriptClassData* Data, const FString& File, const FString& Entry,
                        const TArray<FDasScalarValue>& Args, int32 Kind, const FString& ResultClass,
                        TArray<FDasScalarValue>& Outputs)
{
    Outputs = Args;
    FDasScalarValue Result;
    Result.Kind = -1;
    if (!ValidOwner(Owner, Data) || Data->MethodScripts.FindRef(Entry) != File || Kind < 0 || Kind == 4 ||
        (Kind > 13 && Kind != 15 && Kind != 16 && Kind != 17 && Kind != 18))
    {
        Report(Owner, TEXT("Invalid authored object owner, entry contract or result kind"));
        return Result;
    }
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const auto V = R.RunNamedFramePersistent(R.ScriptPath(File), Entry, Args, static_cast<EDasMethodSignature>(Kind),
                                             Owner, ResultClass);
    if (!V.bSucceeded)
    {
        if (!R.IsAuthoredMethodDispatchActive())
            Report(Owner, V.Error);
        return Result;
    }
    Outputs = V.OutputValues;
    Result.Kind = Kind;
    Result.IntValue = V.Value;
    Result.FloatValue = V.FloatValue;
    Result.DoubleValue = V.DoubleValue;
    Result.BoolValue = V.BoolValue;
    Result.StringValue = V.StringValue;
    Result.NameValue = V.NameValue;
    Result.ByteValue = V.ByteValue;
    Result.TextValue = V.TextValue;
    Result.ObjectValue = V.ObjectValue.Get();
    Result.ClassValue = Cast<UClass>(V.ObjectValue.Get());
    Result.StructValue = V.StructValue;
    Result.DelegateValue = V.DelegateValue;
    Result.VectorValue = V.VectorValue;
    Result.RotatorValue = V.RotatorValue;
    Result.TransformValue = V.TransformValue;
    Result.ObjectClass = (Kind == 8 || Kind == 13 || Kind == 15 || Kind == 16) ? ResultClass : FString();
    return Result;
}
}
FDasScalarValue UDasObjectScriptLibrary::ExecuteObjectFrame(UObject* O, UDasScriptClassData* D, const FString& F,
                                                            const FString& E, const TArray<FDasScalarValue>& A, int32 K,
                                                            const FString& R)
{
    TArray<FDasScalarValue> Outputs;
    return Execute(O, D, F, E, A, K, R, Outputs);
}
FDasScalarValue UDasObjectScriptLibrary::ExecuteObjectFrameOutputs(UObject* O, UDasScriptClassData* D, const FString& F,
                                                                   const FString& E, const TArray<FDasScalarValue>& A,
                                                                   int32 K, const FString& R,
                                                                   TArray<FDasScalarValue>& Outputs)
{
    return Execute(O, D, F, E, A, K, R, Outputs);
}
void UDasObjectScriptLibrary::ExecuteObjectLifecycle(UObject* Owner, UDasScriptClassData* Data, int32 Kind,
                                                     float DeltaSeconds, TEnumAsByte<EEndPlayReason::Type> Reason)
{
    if (!ValidOwner(Owner, Data, Kind == 2) || Kind < 0 || Kind > 2 || !FMath::IsFinite(DeltaSeconds) ||
        DeltaSeconds < 0)
    {
        Report(Owner, TEXT("Invalid authored lifecycle owner or parameters"));
        return;
    }
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    if (Kind == 2 && !(Data->LifecycleMask & 4))
    {
        R.ReleaseObject(Owner);
        return;
    }
    const auto Result = R.RunLifecycle(R.ScriptPath(Data->LifecycleScript),
                                       Kind == 0   ? EDasCallback::BeginPlay
                                       : Kind == 1 ? EDasCallback::Tick
                                                   : EDasCallback::EndPlay,
                                       Owner, DeltaSeconds, int32(Reason.GetValue()));
    if (Kind == 2)
        R.ReleaseObject(Owner);
    if (!Result.bSucceeded)
        Report(Owner, Result.Error);
}
