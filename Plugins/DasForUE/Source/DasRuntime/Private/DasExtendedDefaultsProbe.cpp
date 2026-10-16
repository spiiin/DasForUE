#include "DasExtendedDefaultsProbe.h"
#include "DasProbeActor.h"
#include "DasRuntime.h"
#include "Modules/ModuleManager.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "UObject/UnrealType.h"
#include "UObject/StructOnScope.h"

bool RunDasExtendedDefaultsProbe(UClass* Class,UWorld* World,FString& Report){
 bool Good=true;auto Check=[&](bool OK,const TCHAR* Name){Good &= OK;Report+=FString::Printf(TEXT("DAS_EXTENDED_DEFAULTS %s %s\n"),OK?TEXT("PASS"):TEXT("FAIL"),Name);};
 if(!Class || !World)return false;
 auto Int=[](UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FIntProperty>(O->GetClass(),N):nullptr;return P?P->GetPropertyValue_InContainer(O):MIN_int32;};
 auto Ref=[](UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FObjectPropertyBase>(O->GetClass(),N):nullptr;return P?P->GetObjectPropertyValue_InContainer(O):nullptr;};
 auto Path=[&](UObject* O,const TCHAR* N){auto* V=Ref(O,N);return V?V->GetPathName():FString();};
 auto Pose=[](UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FStructProperty>(O->GetClass(),N):nullptr;return P && P->Struct==TBaseStructure<FTransform>::Get()?*P->ContainerPtrToValuePtr<FTransform>(O):FTransform::Identity;};
 auto* CDO=Class->GetDefaultObject();auto* A=World->SpawnActor<ADasProbeActor>(Class);if(!A)return false;
 Check(Int(CDO,TEXT("CDOPoints"))==31 && Int(A,TEXT("CDOPoints"))==31,TEXT("own_scalar_default_replaces_inline"));
 Check(Pose(A,TEXT("CDOTransform")).Equals(FTransform(FRotator(0,45,0),FVector(10,20,30),FVector(2))),TEXT("nonidentity_transform_default"));
 Check(Path(A,TEXT("CDOAsset"))==TEXT("/Engine/BasicShapes/Cube.Cube") && Path(CDO,TEXT("CDOAsset"))==Path(A,TEXT("CDOAsset")),TEXT("asset_default_is_hard_reference"));
 Check(Path(A,TEXT("CDOActorClass"))==TEXT("/Script/Engine.StaticMeshActor"),TEXT("typed_class_default"));
 auto* Worker=Ref(A,TEXT("Worker"));auto* Sibling=Ref(A,TEXT("Sibling"));auto* Mesh=Cast<UStaticMeshComponent>(Ref(A,TEXT("Mesh")));auto* Pivot=Cast<USceneComponent>(Ref(A,TEXT("Pivot")));
 Check(Worker && Sibling && Int(Worker,TEXT("Bonus"))==42 && Int(Sibling,TEXT("Bonus"))==7 && Int(Worker->GetClass()->GetDefaultObject(),TEXT("Bonus"))==7,TEXT("component_defaults_preserve_sibling_and_CDO"));
 Check(Pose(Worker,TEXT("Pose")).Equals(FTransform(FRotator(0,90,0),FVector(1,2,3))) && Path(Worker,TEXT("Visual"))==TEXT("/Engine/BasicShapes/Cube.Cube") && Path(Worker,TEXT("SpawnType"))==TEXT("/Script/Engine.StaticMeshActor"),TEXT("authored_component_math_asset_class_defaults"));
 Check(Mesh && Mesh->LightingChannels.bChannel1 && !Mesh->LightingChannels.bChannel0,TEXT("native_component_reflected_struct_default"));
 Check(Mesh && Mesh->GetStaticMesh() && Mesh->GetStaticMesh()->GetPathName()==TEXT("/Engine/BasicShapes/Cube.Cube") && Pivot && Pivot->GetRelativeLocation().Equals(FVector(0,0,50)) && Pivot->GetRelativeRotation().Equals(FRotator(0,30,0)),TEXT("native_template_asset_and_math_defaults"));
 auto* Verify=A->FindFunction(TEXT("VerifyDefaults"));bool Verified=false;if(Verify){FStructOnScope Frame(Verify);A->ProcessEvent(Verify,Frame.GetStructMemory());auto* P=CastField<FBoolProperty>(Verify->GetReturnProperty());Verified=P && P->GetPropertyValue_InContainer(Frame.GetStructMemory());}
 Check(Verified && A->LastDasError.IsEmpty(),TEXT("DSL_runtime_owner_validation_and_struct_value"));
 auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));R.ReleaseObject(A);A->Destroy();return Good;
}
