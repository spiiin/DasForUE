#include "DasCharacter.h"
#include "DasTypeRegistry.h"
#include "DasActorHostRuntime.h"
ADasCharacter::ADasCharacter(){PrimaryActorTick.bCanEverTick=true;}
void ADasCharacter::OnConstruction(const FTransform& Transform){Super::OnConstruction(Transform);FDasActorHostRuntime::OnConstruction(*this,Transform);}
void ADasCharacter::ApplyDasSceneRoot(){FDasActorHostRuntime::ApplyDasSceneRoot(*this);}
void ADasCharacter::BeginPlay(){Super::BeginPlay();FDasActorHostRuntime::BeginPlay(*this);}
void ADasCharacter::Tick(float DeltaSeconds){Super::Tick(DeltaSeconds);FDasActorHostRuntime::Tick(*this,DeltaSeconds);}
void ADasCharacter::EndPlay(const EEndPlayReason::Type Reason){FDasActorHostRuntime::EndPlay(*this,Reason);Super::EndPlay(Reason);}
void ADasCharacter::RecordLifecycleResult(const FDasExecutionResult& Result){FDasActorHostRuntime::RecordLifecycleResult(*this,Result);}
FDasScalarValue ADasCharacter::ExecuteDasFrame(const FString& File,const FString& Entry,const TArray<FDasScalarValue>& Args,int32 Kind,const FString& ResultClass){return FDasActorHostRuntime::ExecuteDasFrame(*this,File,Entry,Args,Kind,ResultClass);}
FDasScalarValue ADasCharacter::ExecuteDasFrameOutputs(const FString& File,const FString& Entry,const TArray<FDasScalarValue>& Args,int32 Kind,const FString& ResultClass,TArray<FDasScalarValue>& Outputs){return FDasActorHostRuntime::ExecuteDasFrameOutputs(*this,File,Entry,Args,Kind,ResultClass,Outputs);}
void ADasCharacter::PostInitializeComponents(){Super::PostInitializeComponents();ApplyDasSceneRoot();}
void ADasCharacter::Destroyed(){Super::Destroyed();if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))R->ReleaseObject(this);}
FDasScalarValue ADasCharacter::ExecuteDasConstFrame(const FString& File,const FString& Entry,const TArray<FDasScalarValue>& Args,int32 Kind,const FString& ResultClass) const {return const_cast<ADasCharacter*>(this)->ExecuteDasFrame(File,Entry,Args,Kind,ResultClass);}
FDasScalarValue ADasCharacter::ExecuteDasConstFrameOutputs(const FString& File,const FString& Entry,const TArray<FDasScalarValue>& Args,int32 Kind,const FString& ResultClass,TArray<FDasScalarValue>& Outputs) const {return const_cast<ADasCharacter*>(this)->ExecuteDasFrameOutputs(File,Entry,Args,Kind,ResultClass,Outputs);}
