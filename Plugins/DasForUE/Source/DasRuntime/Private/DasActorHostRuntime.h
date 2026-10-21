#pragma once
#include <type_traits>
#include "DasRuntime.h"
#include "DasScriptComponent.h"
#include "Components/SceneComponent.h"
#include "Modules/ModuleManager.h"
#include "Misc/Paths.h"
#include "Blueprint/BlueprintExceptionInfo.h"
#include "UObject/Stack.h"
#include "UObject/Script.h"
#include "UObject/UnrealType.h"
class ADasProbeActor;
// Shows a script error of a running game world in the PIE Message Log and on screen (DasScriptErrors.cpp).
void DasReportScriptError(const UObject& Owner, const FString& Error);
// Shared implementation; native hosts retain their real Unreal inheritance.
struct FDasActorHostRuntime
{
    template <class T> static void OnConstruction(T& A, const FTransform& Transform)
    {
        A.ApplyDasSceneRoot();
        if (A.IsTemplate() || !A.GetWorld() || A.IsActorBeingDestroyed() || A.HasActorBegunPlay())
            return;
        FDasExecutionResult Result;
        Result.bSucceeded = true;
        TArray<UDasScriptComponent*> Components;
        A.GetComponents(Components);
        for (auto* Component : Components)
            if (IsValid(Component) && !Component->IsBeingDestroyed())
            {
                const auto ComponentResult = Component->RunDasConstruction();
                if (!ComponentResult.bSucceeded && Result.bSucceeded)
                    Result = ComponentResult;
            }
        // Actor construction can depend on configured component values. Do not run it on failure.
        if (Result.bSucceeded && A.bDasConstructionScript && !A.LifecycleScript.IsEmpty())
        {
            auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
            const FString Path =
                FPaths::IsRelative(A.LifecycleScript) ? Runtime.ScriptPath(A.LifecycleScript) : A.LifecycleScript;
            Result = Runtime.RunConstruction(Path, &A);
        }
        A.RecordLifecycleResult(Result);
    }
    template <class T> static void ApplyDasSceneRoot(T& A)
    {
        if (A.DasSceneRootProperty.IsNone())
            return;
        auto* Property = FindFProperty<FObjectProperty>(A.GetClass(), A.DasSceneRootProperty);
        auto* Scene = Property ? Cast<USceneComponent>(Property->GetObjectPropertyValue_InContainer(&A)) : nullptr;
        if (!Scene || Scene->GetOwner() != &A || Scene == A.GetRootComponent())
            return;
        auto* Previous = A.GetRootComponent();
        Scene->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        A.SetRootComponent(Scene);
        // PIE duplication restores the CDO native root before restoring the SCS root.
        // Keeping the fallback below the SCS root would then create a reverse edge/cycle.
        if (Previous && Previous->CreationMethod == EComponentCreationMethod::Native &&
            Previous->GetFName() == TEXT("ProbeRoot"))
            Previous->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        else if (Previous)
            Previous->AttachToComponent(Scene, FAttachmentTransformRules::KeepWorldTransform);
    }
    template <class T> static void BeginPlay(T& A)
    {
        if (A.IsActorBeingDestroyed() || A.LifecycleScript.IsEmpty())
        {
            // Pawn/Character still need their native tick without any DAS callbacks.
            if constexpr (std::is_same_v<T, ADasProbeActor>)
                A.SetActorTickEnabled(false);
            return;
        }
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        A.ActiveLifecyclePath =
            FPaths::IsRelative(A.LifecycleScript) ? Runtime.ScriptPath(A.LifecycleScript) : A.LifecycleScript;
        A.bLifecycleActive = true;
        A.SetActorTickEnabled(true);
        A.RecordLifecycleResult(Runtime.RunLifecycle(A.ActiveLifecyclePath, EDasCallback::BeginPlay, &A));
    }
    template <class T> static void Tick(T& A, float DeltaSeconds)
    {
        if (A.bLifecycleActive && !A.IsActorBeingDestroyed())
        {
            auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
            A.RecordLifecycleResult(Runtime.RunLifecycle(A.ActiveLifecyclePath, EDasCallback::Tick, &A, DeltaSeconds));
        }
    }
    template <class T> static void EndPlay(T& A, const EEndPlayReason::Type Reason)
    {
        A.SetActorTickEnabled(false);
        if (auto* Runtime = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
        {
            if (A.bLifecycleActive)
            {
                A.bLifecycleActive = false;
                A.RecordLifecycleResult(Runtime->RunLifecycle(A.ActiveLifecyclePath, EDasCallback::EndPlay, &A, 0.f,
                                                              static_cast<int32>(Reason)));
            }
            Runtime->ReleaseObject(&A);
        }
        A.ActiveLifecyclePath.Empty();
    }
    template <class T> static void RecordLifecycleResult(T& A, const FDasExecutionResult& Result)
    {
        const FString Error = Result.bSucceeded ? Result.ReloadError : Result.Error;
        if (Error != A.LastDasError && !Error.IsEmpty())
        {
            UE_LOG(LogTemp, Display, TEXT("daScript lifecycle %s: %s"), *A.GetName(), *Error);
            DasReportScriptError(A, Error);
        }
        A.LastDasError = Error;
    }
    template <class T>
    static FDasScalarValue ExecuteDasFrame(T& A, const FString& File, const FString& Entry,
                                           const TArray<FDasScalarValue>& Args, int32 Kind, const FString& ResultClass)
    {
        if (Kind < 0 || (Kind > 13 && Kind != 15 && Kind != 16 && Kind != 17 && Kind != 18) || Kind == 4)
        {
            FDasExecutionResult Error;
            Error.Error = TEXT("Invalid scalar result kind");
            A.RecordLifecycleResult(Error);
            FDasScalarValue Failed;
            Failed.Kind = -1;
            return Failed;
        }
        auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        const auto V = R.RunNamedFramePersistent(R.ScriptPath(File), Entry, Args,
                                                 static_cast<EDasMethodSignature>(Kind), &A, ResultClass);
        A.RecordLifecycleResult(V);
        FDasScalarValue Result;
        Result.Kind = V.bSucceeded ? Kind : -1;
        Result.IntValue = V.Value;
        Result.FloatValue = V.FloatValue;
        Result.BoolValue = V.BoolValue;
        Result.StringValue = V.StringValue;
        Result.DoubleValue = V.DoubleValue;
        Result.NameValue = V.NameValue;
        Result.ByteValue = V.ByteValue;
        Result.TextValue = V.TextValue;
        Result.ObjectValue = V.ObjectValue.Get();
        Result.ClassValue = Cast<UClass>(V.ObjectValue.Get());
        Result.VectorValue = V.VectorValue;
        Result.RotatorValue = V.RotatorValue;
        Result.TransformValue = V.TransformValue;
        Result.StructValue = V.StructValue;
        Result.DelegateValue = V.DelegateValue;
        Result.ObjectClass = (Kind == 8 || Kind == 13 || Kind == 15 || Kind == 16) ? ResultClass : FString();
        return Result;
    }
    template <class T>
    static FDasScalarValue ExecuteDasFrameOutputs(T& A, const FString& File, const FString& Entry,
                                                  const TArray<FDasScalarValue>& Args, int32 Kind,
                                                  const FString& ResultClass, TArray<FDasScalarValue>& Outputs)
    {
        Outputs = Args;
        if (Kind < 0 || (Kind > 13 && Kind != 15 && Kind != 16 && Kind != 17 && Kind != 18) || Kind == 4)
        {
            FDasScalarValue Failed;
            Failed.Kind = -1;
            return Failed;
        }
        auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        auto V = R.RunNamedFramePersistent(R.ScriptPath(File), Entry, Args, static_cast<EDasMethodSignature>(Kind), &A,
                                           ResultClass);
        A.RecordLifecycleResult(V);
        if (V.bSucceeded)
            Outputs = V.OutputValues;
        else if (!R.IsAuthoredMethodDispatchActive())
            if (auto* Stack = FFrame::GetThreadLocalTopStackFrame())
            {
                FBlueprintExceptionInfo Info(EBlueprintExceptionType::AbortExecution, FText::FromString(V.Error));
                FBlueprintCoreDelegates::ThrowScriptException(&A, *Stack, Info);
            }
        FDasScalarValue Result;
        Result.Kind = V.bSucceeded ? Kind : -1;
        Result.IntValue = V.Value;
        Result.FloatValue = V.FloatValue;
        Result.BoolValue = V.BoolValue;
        Result.StringValue = V.StringValue;
        Result.DoubleValue = V.DoubleValue;
        Result.NameValue = V.NameValue;
        Result.ByteValue = V.ByteValue;
        Result.TextValue = V.TextValue;
        Result.StructValue = V.StructValue;
        Result.DelegateValue = V.DelegateValue;
        Result.ObjectValue = V.ObjectValue.Get();
        Result.ClassValue = Cast<UClass>(V.ObjectValue.Get());
        Result.VectorValue = V.VectorValue;
        Result.RotatorValue = V.RotatorValue;
        Result.TransformValue = V.TransformValue;
        Result.ObjectClass = (Kind == 8 || Kind == 13 || Kind == 15 || Kind == 16) ? ResultClass : FString();
        return Result;
    }
};
