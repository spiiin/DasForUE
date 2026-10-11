#include "DasTypeRegistry.h"
#include "DasProbeActor.h"
#include "DasRuntime.h"
#include "Components/SceneComponent.h"
#include "Modules/ModuleManager.h"
#include "Misc/Paths.h"
#include "Blueprint/BlueprintExceptionInfo.h"
#include "UObject/Stack.h"
#include "UObject/Script.h"
#include "UObject/UnrealType.h"

ADasProbeActor::ADasProbeActor()
{
    PrimaryActorTick.bCanEverTick = true;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProbeRoot"));
}

void ADasProbeActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    ApplyDasSceneRoot();
}
void ADasProbeActor::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    // Loaded/cooked level actors do not necessarily run OnConstruction. Promote
    // the declared SCS root after components have been restored and initialized.
    ApplyDasSceneRoot();
}
void ADasProbeActor::ApplyDasSceneRoot()
{
    if(DasSceneRootProperty.IsNone())return;
    auto* Property=FindFProperty<FObjectProperty>(GetClass(),DasSceneRootProperty);
    auto* Scene=Property?Cast<USceneComponent>(Property->GetObjectPropertyValue_InContainer(this)):nullptr;
    if(!Scene || Scene->GetOwner()!=this || Scene==GetRootComponent())return;
    auto* Previous=GetRootComponent();
    Scene->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    SetRootComponent(Scene);
    if(Previous)Previous->AttachToComponent(Scene,FAttachmentTransformRules::KeepWorldTransform);
}

int32 ADasProbeActor::ExecuteDas(const FString& ScriptFile, int32 Input)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const auto Result = Runtime.RunPersistent(Runtime.ScriptPath(ScriptFile), Input, TWeakObjectPtr<UObject>(this));
    if (!Result.bSucceeded)
    {
        UE_LOG(LogTemp, Error, TEXT("daScript probe: %s"), *Result.Error);
        return MIN_int32;
    }
    return Result.Value;
}

int32 ADasProbeActor::ExecuteDasNamed(const FString& ScriptFile, const FString& EntryPoint, int32 Input)
{
    auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const auto Result=Runtime.RunNamedPersistent(Runtime.ScriptPath(ScriptFile),EntryPoint,Input,this);
    RecordLifecycleResult(Result);
    if (!Result.bSucceeded) { if(!Runtime.IsAuthoredMethodDispatchActive())UE_LOG(LogTemp,Error,TEXT("daScript method: %s"),*Result.Error);return MIN_int32; }
    return Result.Value;
}

float ADasProbeActor::ExecuteDasFloat(const FString& File,const FString& Entry,float Input)
{auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));const auto V=R.RunNamedFloatPersistent(R.ScriptPath(File),Entry,Input,this);RecordLifecycleResult(V);return V.FloatValue;}
bool ADasProbeActor::ExecuteDasBool(const FString& File,const FString& Entry,bool Input)
{auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));const auto V=R.RunNamedBoolPersistent(R.ScriptPath(File),Entry,Input,this);RecordLifecycleResult(V);return V.BoolValue;}
void ADasProbeActor::ExecuteDasVoid(const FString& File,const FString& Entry)
{auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));RecordLifecycleResult(R.RunNamedVoidPersistent(R.ScriptPath(File),Entry,this));}

FDasScalarValue ADasProbeActor::ExecuteDasFrame(const FString& File,const FString& Entry,const TArray<FDasScalarValue>& Args,int32 Kind,const FString& ResultClass)
{
    if(Kind<0 || (Kind>13 && Kind!=16) || Kind==4){FDasExecutionResult Error;Error.Error=TEXT("Invalid scalar result kind");RecordLifecycleResult(Error);FDasScalarValue Failed;Failed.Kind=-1;return Failed;}
    auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));const auto V=R.RunNamedFramePersistent(R.ScriptPath(File),Entry,Args,static_cast<EDasMethodSignature>(Kind),this,ResultClass);RecordLifecycleResult(V);
    FDasScalarValue Result;Result.Kind=V.bSucceeded?Kind:-1;Result.IntValue=V.Value;Result.FloatValue=V.FloatValue;Result.BoolValue=V.BoolValue;Result.StringValue=V.StringValue;Result.DoubleValue=V.DoubleValue;Result.NameValue=V.NameValue;Result.ObjectValue=V.ObjectValue.Get();Result.VectorValue=V.VectorValue;Result.RotatorValue=V.RotatorValue;Result.TransformValue=V.TransformValue;Result.StructValue=V.StructValue;Result.DelegateValue=V.DelegateValue;Result.ObjectClass=(Kind==8 || Kind==13 || Kind==16)?ResultClass:FString();return Result;
}
FDasScalarValue ADasProbeActor::ExecuteDasFrameOutputs(const FString& File,const FString& Entry,const TArray<FDasScalarValue>& Args,int32 Kind,const FString& ResultClass,TArray<FDasScalarValue>& Outputs){
    Outputs=Args;if(Kind<0 || (Kind>13 && Kind!=16) || Kind==4){FDasScalarValue Failed;Failed.Kind=-1;return Failed;}
    auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));auto V=R.RunNamedFramePersistent(R.ScriptPath(File),Entry,Args,static_cast<EDasMethodSignature>(Kind),this,ResultClass);RecordLifecycleResult(V);if(V.bSucceeded)Outputs=V.OutputValues;else if(!R.IsAuthoredMethodDispatchActive()) if(auto* Stack=FFrame::GetThreadLocalTopStackFrame()){FBlueprintExceptionInfo Info(EBlueprintExceptionType::AbortExecution,FText::FromString(V.Error));FBlueprintCoreDelegates::ThrowScriptException(this,*Stack,Info);}
    FDasScalarValue Result;Result.Kind=V.bSucceeded?Kind:-1;Result.IntValue=V.Value;Result.FloatValue=V.FloatValue;Result.BoolValue=V.BoolValue;Result.StringValue=V.StringValue;Result.DoubleValue=V.DoubleValue;Result.NameValue=V.NameValue;Result.StructValue=V.StructValue;Result.DelegateValue=V.DelegateValue;Result.ObjectValue=V.ObjectValue.Get();Result.VectorValue=V.VectorValue;Result.RotatorValue=V.RotatorValue;Result.TransformValue=V.TransformValue;Result.ObjectClass=(Kind==8 || Kind==13 || Kind==16)?ResultClass:FString();return Result;
}
void ADasProbeActor::RecordLifecycleResult(const FDasExecutionResult& Result)
{
    const FString Error = Result.bSucceeded ? Result.ReloadError : Result.Error;
    if (Error != LastDasError && !Error.IsEmpty())
        UE_LOG(LogTemp, Display, TEXT("daScript lifecycle %s: %s"), *GetName(), *Error);
    LastDasError = Error;
}

void ADasProbeActor::BeginPlay()
{
    Super::BeginPlay();
    if (IsActorBeingDestroyed() || LifecycleScript.IsEmpty())
    {
        SetActorTickEnabled(false);
        return;
    }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    ActiveLifecyclePath = FPaths::IsRelative(LifecycleScript) ? Runtime.ScriptPath(LifecycleScript) : LifecycleScript;
    bLifecycleActive = true;
    SetActorTickEnabled(true);
    RecordLifecycleResult(Runtime.RunLifecycle(ActiveLifecyclePath, EDasCallback::BeginPlay, this));
}

void ADasProbeActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (bLifecycleActive && !IsActorBeingDestroyed())
    {
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        RecordLifecycleResult(Runtime.RunLifecycle(ActiveLifecyclePath, EDasCallback::Tick, this, DeltaSeconds));
    }
}

void ADasProbeActor::EndPlay(const EEndPlayReason::Type Reason)
{
    SetActorTickEnabled(false);
    if (auto* Runtime = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
    {
        if (bLifecycleActive)
        {
            bLifecycleActive = false;
            RecordLifecycleResult(Runtime->RunLifecycle(ActiveLifecyclePath, EDasCallback::EndPlay, this, 0.f, static_cast<int32>(Reason)));
        }
        Runtime->ReleaseObject(this);
    }
    ActiveLifecyclePath.Empty();
    Super::EndPlay(Reason);
}

void ADasProbeActor::Destroyed()
{
    // Super routes EndPlay; its callback must run before the context is released.
    Super::Destroyed();
    if (auto* Runtime = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))) Runtime->ReleaseObject(this);
}
