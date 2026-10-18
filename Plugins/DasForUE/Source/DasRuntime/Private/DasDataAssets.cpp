#include "DasDataAssets.h"
#include "DasActorHostRuntime.h"
bool ParseDasAssetBundleNames(const FString& Text,TArray<FName>& Out,FString& Error){
 Out.Reset();TArray<FString> Parts;Text.ParseIntoArray(Parts,TEXT(","),false);
 if(Parts.IsEmpty() || Parts.Num()>32){Error=TEXT("AssetBundles requires 1..32 bundle names");return false;}
 for(auto Part:Parts){Part.TrimStartAndEndInline();if(Part.IsEmpty() || Part.Len()>128 || Part.Equals(TEXT("None"),ESearchCase::IgnoreCase)){Error=TEXT("Invalid AssetBundles name");return false;}
  for(TCHAR C:Part)if(!FChar::IsAlnum(C) && C!='_' && C!='-'){Error=TEXT("AssetBundles names allow letters, digits, underscore and hyphen");return false;}
  FName Name(*Part);if(Out.Contains(Name)){Error=TEXT("Duplicate AssetBundles name");return false;}Out.Add(Name);
 }return true;
}
FPrimaryAssetId UDasPrimaryDataAsset::GetPrimaryAssetId() const {
 if(IsTemplate() || DasPrimaryAssetType.IsNone())return {};
 return FPrimaryAssetId(FPrimaryAssetType(DasPrimaryAssetType),GetFName());
}
void UDasDataAsset::RecordLifecycleResult(const FDasExecutionResult& R){FDasActorHostRuntime::RecordLifecycleResult(*this,R);}
FDasScalarValue UDasDataAsset::ExecuteDasFrame(const FString& F,const FString& E,const TArray<FDasScalarValue>& A,int32 K,const FString& R){return FDasActorHostRuntime::ExecuteDasFrame(*this,F,E,A,K,R);}
FDasScalarValue UDasDataAsset::ExecuteDasFrameOutputs(const FString& F,const FString& E,const TArray<FDasScalarValue>& A,int32 K,const FString& R,TArray<FDasScalarValue>& O){return FDasActorHostRuntime::ExecuteDasFrameOutputs(*this,F,E,A,K,R,O);}
FDasScalarValue UDasDataAsset::ExecuteDasConstFrame(const FString& F,const FString& E,const TArray<FDasScalarValue>& A,int32 K,const FString& R) const{return const_cast<UDasDataAsset*>(this)->ExecuteDasFrame(F,E,A,K,R);}
FDasScalarValue UDasDataAsset::ExecuteDasConstFrameOutputs(const FString& F,const FString& E,const TArray<FDasScalarValue>& A,int32 K,const FString& R,TArray<FDasScalarValue>& O) const{return const_cast<UDasDataAsset*>(this)->ExecuteDasFrameOutputs(F,E,A,K,R,O);}
void UDasDataAsset::BeginDestroy(){if(IsInGameThread())if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))R->ReleaseObject(this);Super::BeginDestroy();}

void UDasPrimaryDataAsset::RecordLifecycleResult(const FDasExecutionResult& R){FDasActorHostRuntime::RecordLifecycleResult(*this,R);}
FDasScalarValue UDasPrimaryDataAsset::ExecuteDasFrame(const FString& F,const FString& E,const TArray<FDasScalarValue>& A,int32 K,const FString& R){return FDasActorHostRuntime::ExecuteDasFrame(*this,F,E,A,K,R);}
FDasScalarValue UDasPrimaryDataAsset::ExecuteDasFrameOutputs(const FString& F,const FString& E,const TArray<FDasScalarValue>& A,int32 K,const FString& R,TArray<FDasScalarValue>& O){return FDasActorHostRuntime::ExecuteDasFrameOutputs(*this,F,E,A,K,R,O);}
FDasScalarValue UDasPrimaryDataAsset::ExecuteDasConstFrame(const FString& F,const FString& E,const TArray<FDasScalarValue>& A,int32 K,const FString& R) const{return const_cast<UDasPrimaryDataAsset*>(this)->ExecuteDasFrame(F,E,A,K,R);}
FDasScalarValue UDasPrimaryDataAsset::ExecuteDasConstFrameOutputs(const FString& F,const FString& E,const TArray<FDasScalarValue>& A,int32 K,const FString& R,TArray<FDasScalarValue>& O) const{return const_cast<UDasPrimaryDataAsset*>(this)->ExecuteDasFrameOutputs(F,E,A,K,R,O);}
void UDasPrimaryDataAsset::BeginDestroy(){if(IsInGameThread())if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))R->ReleaseObject(this);Super::BeginDestroy();}
