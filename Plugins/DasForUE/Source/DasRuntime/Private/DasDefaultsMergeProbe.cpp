#include "DasDefaultsMergeProbe.h"
#include "DasProbeActor.h"
#include "DasRuntime.h"
#include "Modules/ModuleManager.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/UnrealType.h"
#include "UObject/StructOnScope.h"
bool RunDasDefaultsMergeProbe(UWorld* World,FString& Report){
 if(!World || !World->PersistentLevel)return false;ADasProbeActor* A=nullptr;ADasProbeActor* M=nullptr;
 for(const auto& Actor:World->PersistentLevel->Actors)if(auto* P=Cast<ADasProbeActor>(Actor)){if(P->Tags.Contains(TEXT("MergeInherited")))A=P;if(P->Tags.Contains(TEXT("MergeManual")))M=P;}
 if(!A || !M)return false;bool Good=true;auto Check=[&](bool OK,const TCHAR* Name){Good &= OK;Report+=FString::Printf(TEXT("DAS_DEFAULTS_COOKED %s %s\n"),OK?TEXT("PASS"):TEXT("FAIL"),Name);};
 auto Int=[](UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FIntProperty>(O->GetClass(),N):nullptr;return P?P->GetPropertyValue_InContainer(O):MIN_int32;};
 auto Ref=[](UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FObjectPropertyBase>(O->GetClass(),N):nullptr;return P?P->GetObjectPropertyValue_InContainer(O):nullptr;};
 auto Vec=[](UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FStructProperty>(O->GetClass(),N):nullptr;return P?*P->ContainerPtrToValuePtr<FVector>(O):FVector::ZeroVector;};
 Check(Int(A,TEXT("Auto"))==20 && Int(A,TEXT("Manual"))==77 && Int(M,TEXT("Auto"))==111,TEXT("placed_actor_overrides"));
 Check(Vec(A,TEXT("Position"))==FVector(99,5,6) && Vec(M,TEXT("Position"))==FVector(99,222,6),TEXT("partial_struct_overrides"));
 Check(Vec(Ref(A,TEXT("Pivot")),TEXT("RelativeLocation"))==FVector(99,5,20) && Vec(Ref(M,TEXT("Pivot")),TEXT("RelativeLocation"))==FVector(99,5,777),TEXT("scene_component_overrides"));
 Check(Int(Ref(A,TEXT("Worker")),TEXT("Bonus"))==88 && Int(Ref(A,TEXT("OtherWorker")),TEXT("Bonus"))==52 && Int(Ref(M,TEXT("OtherWorker")),TEXT("Bonus"))==123,TEXT("component_sibling_overrides"));
 auto* MA=Cast<UStaticMeshComponent>(Ref(A,TEXT("Mesh")));auto* MM=Cast<UStaticMeshComponent>(Ref(M,TEXT("Mesh")));
 Check(MA && MM && MA->GetStaticMesh() && MM->GetStaticMesh() && MA->GetStaticMesh()->GetPathName()==TEXT("/Engine/BasicShapes/Sphere.Sphere") && MM->GetStaticMesh()->GetPathName()==TEXT("/Engine/BasicShapes/Cylinder.Cylinder"),TEXT("native_component_asset_overrides"));
 auto* Fresh=World->SpawnActor<ADasProbeActor>(A->GetClass());Check(Fresh && Int(Fresh,TEXT("Auto"))==20 && Int(Fresh,TEXT("Manual"))==77 && Int(Ref(Fresh,TEXT("OtherWorker")),TEXT("Bonus"))==52,TEXT("fresh_spawn_merged_CDO"));
 auto* F=A->FindFunction(TEXT("Read"));if(F){FStructOnScope Args(F);auto* Input=FindFProperty<FIntProperty>(F,TEXT("input"));Input->SetPropertyValue_InContainer(Args.GetStructMemory(),0);A->ProcessEvent(F,Args.GetStructMemory());Check(CastFieldChecked<FIntProperty>(F->GetReturnProperty())->GetPropertyValue_InContainer(Args.GetStructMemory())==97 && A->LastDasError.IsEmpty(),TEXT("cooked_source_contract_validation"));}else Check(false,TEXT("reflected_function"));
 if(Fresh){FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime")).ReleaseObject(Fresh);Fresh->Destroy();}return Good;
}
