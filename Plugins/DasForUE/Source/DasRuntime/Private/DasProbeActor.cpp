#include "DasProbeActor.h"
#include "DasRuntime.h"
#include "Components/SceneComponent.h"
#include "Modules/ModuleManager.h"
#include "Misc/Paths.h"

ADasProbeActor::ADasProbeActor()
{
    PrimaryActorTick.bCanEverTick = true;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProbeRoot"));
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
    if (!Result.bSucceeded) { UE_LOG(LogTemp,Error,TEXT("daScript method: %s"),*Result.Error);return MIN_int32; }
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
    if(Kind<0 || Kind>12 || Kind==4){FDasExecutionResult Error;Error.Error=TEXT("Invalid scalar result kind");RecordLifecycleResult(Error);FDasScalarValue Failed;Failed.Kind=-1;return Failed;}
    auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));const auto V=R.RunNamedFramePersistent(R.ScriptPath(File),Entry,Args,static_cast<EDasMethodSignature>(Kind),this,ResultClass);RecordLifecycleResult(V);
    FDasScalarValue Result;Result.Kind=V.bSucceeded?Kind:-1;Result.IntValue=V.Value;Result.FloatValue=V.FloatValue;Result.BoolValue=V.BoolValue;Result.StringValue=V.StringValue;Result.DoubleValue=V.DoubleValue;Result.NameValue=V.NameValue;Result.ObjectValue=V.ObjectValue.Get();Result.VectorValue=V.VectorValue;Result.RotatorValue=V.RotatorValue;Result.TransformValue=V.TransformValue;Result.StructValue=V.StructValue;return Result;
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
