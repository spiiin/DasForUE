#include "DasSaveGame.h"
#include "DasActorHostRuntime.h"
void UDasSaveGame::RecordLifecycleResult(const FDasExecutionResult& R){FDasActorHostRuntime::RecordLifecycleResult(*this,R);}
FDasScalarValue UDasSaveGame::ExecuteDasFrame(const FString& F,const FString& E,const TArray<FDasScalarValue>& A,int32 K,const FString& R){return FDasActorHostRuntime::ExecuteDasFrame(*this,F,E,A,K,R);}
FDasScalarValue UDasSaveGame::ExecuteDasFrameOutputs(const FString& F,const FString& E,const TArray<FDasScalarValue>& A,int32 K,const FString& R,TArray<FDasScalarValue>& O){return FDasActorHostRuntime::ExecuteDasFrameOutputs(*this,F,E,A,K,R,O);}
FDasScalarValue UDasSaveGame::ExecuteDasConstFrame(const FString& F,const FString& E,const TArray<FDasScalarValue>& A,int32 K,const FString& R) const{return const_cast<UDasSaveGame*>(this)->ExecuteDasFrame(F,E,A,K,R);}
FDasScalarValue UDasSaveGame::ExecuteDasConstFrameOutputs(const FString& F,const FString& E,const TArray<FDasScalarValue>& A,int32 K,const FString& R,TArray<FDasScalarValue>& O) const{return const_cast<UDasSaveGame*>(this)->ExecuteDasFrameOutputs(F,E,A,K,R,O);}
void UDasSaveGame::BeginDestroy(){if(IsInGameThread())if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))R->ReleaseObject(this);Super::BeginDestroy();}
