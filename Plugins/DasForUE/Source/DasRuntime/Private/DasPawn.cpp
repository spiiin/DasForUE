#include "DasPawn.h"
#include "DasTypeRegistry.h"
#include "DasActorHostRuntime.h"
ADasPawn::ADasPawn(){PrimaryActorTick.bCanEverTick=true;RootComponent=CreateDefaultSubobject<USceneComponent>(TEXT("ProbeRoot"));}
void ADasPawn::OnConstruction(const FTransform& Transform){Super::OnConstruction(Transform);FDasActorHostRuntime::OnConstruction(*this,Transform);}
void ADasPawn::ApplyDasSceneRoot(){FDasActorHostRuntime::ApplyDasSceneRoot(*this);}
void ADasPawn::BeginPlay(){Super::BeginPlay();FDasActorHostRuntime::BeginPlay(*this);}
void ADasPawn::Tick(float DeltaSeconds){Super::Tick(DeltaSeconds);FDasActorHostRuntime::Tick(*this,DeltaSeconds);}
void ADasPawn::EndPlay(const EEndPlayReason::Type Reason){FDasActorHostRuntime::EndPlay(*this,Reason);Super::EndPlay(Reason);}
void ADasPawn::RecordLifecycleResult(const FDasExecutionResult& Result){FDasActorHostRuntime::RecordLifecycleResult(*this,Result);}
FDasScalarValue ADasPawn::ExecuteDasFrame(const FString& File,const FString& Entry,const TArray<FDasScalarValue>& Args,int32 Kind,const FString& ResultClass){return FDasActorHostRuntime::ExecuteDasFrame(*this,File,Entry,Args,Kind,ResultClass);}
FDasScalarValue ADasPawn::ExecuteDasFrameOutputs(const FString& File,const FString& Entry,const TArray<FDasScalarValue>& Args,int32 Kind,const FString& ResultClass,TArray<FDasScalarValue>& Outputs){return FDasActorHostRuntime::ExecuteDasFrameOutputs(*this,File,Entry,Args,Kind,ResultClass,Outputs);}
void ADasPawn::PostInitializeComponents(){Super::PostInitializeComponents();ApplyDasSceneRoot();}
void ADasPawn::Destroyed(){Super::Destroyed();if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))R->ReleaseObject(this);}
FDasScalarValue ADasPawn::ExecuteDasConstFrame(const FString& File,const FString& Entry,const TArray<FDasScalarValue>& Args,int32 Kind,const FString& ResultClass) const {return const_cast<ADasPawn*>(this)->ExecuteDasFrame(File,Entry,Args,Kind,ResultClass);}
FDasScalarValue ADasPawn::ExecuteDasConstFrameOutputs(const FString& File,const FString& Entry,const TArray<FDasScalarValue>& Args,int32 Kind,const FString& ResultClass,TArray<FDasScalarValue>& Outputs) const {return const_cast<ADasPawn*>(this)->ExecuteDasFrameOutputs(File,Entry,Args,Kind,ResultClass,Outputs);}
