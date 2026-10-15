#include "DasScriptComponent.h"
#include "DasRuntime.h"
#include "Blueprint/BlueprintExceptionInfo.h"
#include "UObject/Stack.h"
#include "UObject/Script.h"
#include "GameFramework/Actor.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
UDasScriptComponent::UDasScriptComponent(){PrimaryComponentTick.bCanEverTick=true;PrimaryComponentTick.bStartWithTickEnabled=false;}
void UDasScriptComponent::Record(const FDasExecutionResult& R){const FString Error=R.bSucceeded?R.ReloadError:R.Error;if(!Error.IsEmpty() && Error!=LastDasError)UE_LOG(LogTemp,Display,TEXT("daScript component %s: %s"),*GetName(),*Error);LastDasError=Error;}
void UDasScriptComponent::BeginPlay(){Super::BeginPlay();if(LifecycleScript.IsEmpty() || !IsValid(GetOwner()) || GetOwner()->IsActorBeingDestroyed()){SetComponentTickEnabled(false);return;}auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));ActivePath=FPaths::IsRelative(LifecycleScript)?R.ScriptPath(LifecycleScript):LifecycleScript;bLifecycleActive=true;SetComponentTickEnabled(true);Record(R.RunLifecycle(ActivePath,EDasCallback::BeginPlay,this));}
void UDasScriptComponent::TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Tick){Super::TickComponent(Delta,Type,Tick);if(bLifecycleActive && IsValid(GetOwner()) && !GetOwner()->IsActorBeingDestroyed()){auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));Record(R.RunLifecycle(ActivePath,EDasCallback::Tick,this,Delta));}}
void UDasScriptComponent::FinishScript(EEndPlayReason::Type Reason){SetComponentTickEnabled(false);if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){if(bLifecycleActive){bLifecycleActive=false;Record(R->RunLifecycle(ActivePath,EDasCallback::EndPlay,this,0.f,int32(Reason)));OnScriptEnded.Broadcast(this,Reason);}R->ReleaseObject(this);}ActivePath.Empty();}
void UDasScriptComponent::EndPlay(const EEndPlayReason::Type Reason){FinishScript(Reason);Super::EndPlay(Reason);}
void UDasScriptComponent::OnComponentDestroyed(bool Hierarchy){FinishScript(EEndPlayReason::Destroyed);Super::OnComponentDestroyed(Hierarchy);}

FDasScalarValue UDasScriptComponent::ExecuteDasFrame(const FString& File,const FString& Entry,const TArray<FDasScalarValue>& Args,int32 Kind,const FString& ResultClass)
{
    if(Kind<0 || (Kind>13 && Kind!=15 && Kind!=16) || Kind==4){FDasExecutionResult Error;Error.Error=TEXT("Invalid scalar result kind");Record(Error);FDasScalarValue Failed;Failed.Kind=-1;return Failed;}
    auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));const auto V=R.RunNamedFramePersistent(R.ScriptPath(File),Entry,Args,static_cast<EDasMethodSignature>(Kind),this,ResultClass);Record(V);
    FDasScalarValue Result;Result.Kind=V.bSucceeded?Kind:-1;Result.IntValue=V.Value;Result.FloatValue=V.FloatValue;Result.BoolValue=V.BoolValue;Result.StringValue=V.StringValue;Result.DoubleValue=V.DoubleValue;Result.NameValue=V.NameValue;Result.ObjectValue=V.ObjectValue.Get();Result.ClassValue=Cast<UClass>(V.ObjectValue.Get());Result.VectorValue=V.VectorValue;Result.RotatorValue=V.RotatorValue;Result.TransformValue=V.TransformValue;Result.StructValue=V.StructValue;Result.DelegateValue=V.DelegateValue;Result.ObjectClass=(Kind==8 || Kind==13 || Kind==15 || Kind==16)?ResultClass:FString();return Result;
}
FDasScalarValue UDasScriptComponent::ExecuteDasFrameOutputs(const FString& File,const FString& Entry,const TArray<FDasScalarValue>& Args,int32 Kind,const FString& ResultClass,TArray<FDasScalarValue>& Outputs){
    Outputs=Args;if(Kind<0 || (Kind>13 && Kind!=15 && Kind!=16) || Kind==4){FDasScalarValue Failed;Failed.Kind=-1;return Failed;}
    auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));auto V=R.RunNamedFramePersistent(R.ScriptPath(File),Entry,Args,static_cast<EDasMethodSignature>(Kind),this,ResultClass);Record(V);if(V.bSucceeded)Outputs=V.OutputValues;else if(!R.IsAuthoredMethodDispatchActive()) if(auto* Stack=FFrame::GetThreadLocalTopStackFrame()){FBlueprintExceptionInfo Info(EBlueprintExceptionType::AbortExecution,FText::FromString(V.Error));FBlueprintCoreDelegates::ThrowScriptException(this,*Stack,Info);}
    FDasScalarValue Result;Result.Kind=V.bSucceeded?Kind:-1;Result.IntValue=V.Value;Result.FloatValue=V.FloatValue;Result.BoolValue=V.BoolValue;Result.StringValue=V.StringValue;Result.DoubleValue=V.DoubleValue;Result.NameValue=V.NameValue;Result.StructValue=V.StructValue;Result.DelegateValue=V.DelegateValue;Result.ObjectValue=V.ObjectValue.Get();Result.ClassValue=Cast<UClass>(V.ObjectValue.Get());Result.VectorValue=V.VectorValue;Result.RotatorValue=V.RotatorValue;Result.TransformValue=V.TransformValue;Result.ObjectClass=(Kind==8 || Kind==13 || Kind==15 || Kind==16)?ResultClass:FString();return Result;
}
