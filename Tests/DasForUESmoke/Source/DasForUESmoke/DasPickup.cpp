#include "CoreMinimal.h"
#include "DasRuntime.h"
#include "Containers/Ticker.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "Modules/ModuleManager.h"
#include "UObject/UnrealType.h"
#include "UObject/StrongObjectPtr.h"

// Only the saved pickup example. No Foundation, broad PIE or cooked suite.
void StartDasPickupValidation(){
 struct FState {int32 Stage=0;double Started=FPlatformTime::Seconds();float Deadline=0;TWeakObjectPtr<AActor> Pickup;TWeakObjectPtr<ACharacter> Player;FSoftObjectPath MeshPath;TWeakObjectPtr<UClass> PickupClass;TWeakObjectPtr<AActor> Empty,Missing,Retry;TStrongObjectPtr<AActor> Destroyed;};
 auto State=MakeShared<FState>();
 FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([State](float){
  auto Fail=[](const FString& Why){UE_LOG(LogTemp,Error,TEXT("DAS_PICKUP_RESULT=FAIL %s"),*Why);FPlatformMisc::RequestExitWithStatus(false,1);return false;};
  if(FPlatformTime::Seconds()-State->Started>30)return Fail(TEXT("timeout"));
  UWorld* World=nullptr;for(const auto& Context:GEngine->GetWorldContexts())if(Context.World() && Context.World()->IsGameWorld() && Context.World()->HasBegunPlay()){World=Context.World();break;}
  if(!World || World->GetTimeSeconds()<.5f)return true;
  auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
  auto Int=[](UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FIntProperty>(O->GetClass(),N):nullptr;return P?P->GetPropertyValue_InContainer(O):INDEX_NONE;};
  auto Flag=[](UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FBoolProperty>(O->GetClass(),N):nullptr;return P && P->GetPropertyValue_InContainer(O);};
  auto Available=[](AActor* A){auto* P=A?FindFProperty<FBoolProperty>(A->GetClass(),TEXT("Available")):nullptr;return P && P->GetPropertyValue_InContainer(A);};
  auto Error=[](AActor* A){auto* P=A?FindFProperty<FStrProperty>(A->GetClass(),TEXT("LastDasError")):nullptr;return P?P->GetPropertyValue_InContainer(A):FString(TEXT("missing script host"));};
  if(State->Stage==0){
   auto* PC=World->GetFirstPlayerController();auto* Player=PC?Cast<ACharacter>(PC->GetPawn()):nullptr;AActor* Pickup=nullptr;
   for(const auto& A:World->PersistentLevel->Actors)if(A && A->GetFName()==TEXT("DAS_Pickup")){Pickup=A;break;}
   if(!Player || !Pickup)return Fail(TEXT("saved map lacks player or pickup"));
   if(!Error(Player).IsEmpty() || !Error(Pickup).IsEmpty())return Fail(Error(Player)+Error(Pickup));
   auto* Mesh=Pickup->FindComponentByClass<UStaticMeshComponent>();auto* Sphere=Pickup->FindComponentByClass<USphereComponent>();
   if(Flag(Pickup,TEXT("MeshLoading"))){if(!Available(Pickup) && Mesh && !Mesh->IsVisible() && Sphere && Sphere->GetCollisionEnabled()==ECollisionEnabled::NoCollision)return true;return Fail(TEXT("pickup active while loading"));}
   if(Flag(Pickup,TEXT("MeshWasLoaded")) || !Flag(Pickup,TEXT("MeshReady")) || Int(Pickup,TEXT("MeshCallbacks"))!=1 || Int(Pickup,TEXT("LoadFailures"))!=0)return Fail(TEXT("mesh did not complete a cold asynchronous load: ")+Error(Pickup));
   auto* Soft=FindFProperty<FSoftObjectProperty>(Pickup->GetClass(),TEXT("MeshAsset"));
   if(!Soft || Soft->PropertyClass!=UStaticMesh::StaticClass())return Fail(TEXT("typed soft mesh property missing"));
   State->MeshPath=Soft->GetPropertyValue_InContainer(Pickup).ToSoftObjectPath();State->PickupClass=Pickup->GetClass();
   if(!Mesh || Mesh->GetStaticMesh()!=State->MeshPath.ResolveObject() || !Mesh->IsVisible() || !Sphere || Sphere!=Pickup->GetRootComponent() || Sphere->GetCollisionEnabled()!=ECollisionEnabled::QueryOnly || !Sphere->GetGenerateOverlapEvents() || !FMath::IsNearlyEqual(Sphere->GetUnscaledSphereRadius(),75.f) || !Available(Pickup) || Int(Player,TEXT("Score"))!=0)return Fail(TEXT("mesh/collision/defaults were not restored from the saved map"));
   UE_LOG(LogTemp,Display,TEXT("PASS pickup_async_cold_mesh_loaded"));
   auto* Delay=FindFProperty<FFloatProperty>(Pickup->GetClass(),TEXT("RespawnSeconds"));if(!Delay)return Fail(TEXT("respawn property missing"));Delay->SetPropertyValue_InContainer(Pickup,.15f);
   Player->GetCharacterMovement()->DisableMovement();
   // A real overlapping Pawn that is not the assigned collector must be ignored.
   FActorSpawnParameters Params;Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
   auto* Other=World->SpawnActor<ACharacter>(ACharacter::StaticClass(),FTransform(FVector(-700,0,100)),Params);
   if(!Other)return Fail(TEXT("cannot create non-recipient"));Other->GetCapsuleComponent()->SetGenerateOverlapEvents(true);
   Other->SetActorLocation(Pickup->GetActorLocation());Other->GetCapsuleComponent()->UpdateOverlaps();
   if(!Other->IsOverlappingActor(Pickup) || Int(Pickup,TEXT("OverlapEvents"))<1 || !Available(Pickup) || Int(Player,TEXT("Score"))!=0){Other->Destroy();return Fail(TEXT("non-recipient physical overlap/filter"));}
   Other->Destroy();UE_LOG(LogTemp,Display,TEXT("PASS pickup_real_non_recipient_overlap_ignored"));
   Player->SetActorLocation(Pickup->GetActorLocation()+FVector(0,0,40));Player->GetCapsuleComponent()->UpdateOverlaps();
   if(Int(Player,TEXT("Score"))!=10 || Int(Pickup,TEXT("Pickups"))!=1 || Available(Pickup) || Mesh->IsVisible() || Sphere->GetCollisionEnabled()!=ECollisionEnabled::NoCollision || Runtime.ScriptTimerCount(Pickup)!=1)return Fail(TEXT("real overlap did not award/hide/disable/schedule once: ")+Error(Pickup));
   auto* Label=Player->FindComponentByClass<UTextRenderComponent>();if(!Label || Label->Text.ToString()!=TEXT("Score: 10"))return Fail(TEXT("score display"));
   UE_LOG(LogTemp,Display,TEXT("PASS pickup_real_overlap_score_mesh_collision_timer"));
   Pickup->OnActorBeginOverlap.Broadcast(Pickup,Player);
   if(Int(Player,TEXT("Score"))!=10 || Int(Pickup,TEXT("Pickups"))!=1 || Runtime.ScriptTimerCount(Pickup)!=1)return Fail(TEXT("duplicate callback awarded twice"));
   UE_LOG(LogTemp,Display,TEXT("PASS pickup_duplicate_callback_guard"));
   Player->SetActorLocation(FVector(-500,0,100));State->Pickup=Pickup;State->Player=Player;State->Deadline=World->GetTimeSeconds()+3.f;State->Stage=1;return true;
  }
  auto* Player=State->Player.Get();if(!Player)return Fail(TEXT("player expired"));
  if(State->Stage==1){
   auto* Pickup=State->Pickup.Get();if(!Pickup)return Fail(TEXT("pickup expired before timer"));
   if(!Available(Pickup)){if(World->GetTimeSeconds()>State->Deadline)return Fail(TEXT("respawn timer did not restore"));return true;}
   auto* Mesh=Pickup->FindComponentByClass<UStaticMeshComponent>();auto* Sphere=Pickup->FindComponentByClass<USphereComponent>();
   if(Int(Pickup,TEXT("Restores"))!=1 || Int(Player,TEXT("Score"))!=10 || !Mesh->IsVisible() || Sphere->GetCollisionEnabled()!=ECollisionEnabled::QueryOnly || Runtime.ScriptTimerCount(Pickup)!=0)return Fail(TEXT("restoration state/timer cleanup"));
   UE_LOG(LogTemp,Display,TEXT("PASS pickup_timer_restores_mesh_collision_availability"));
   Player->SetActorLocation(Pickup->GetActorLocation()+FVector(0,0,40));Player->GetCapsuleComponent()->UpdateOverlaps();
   if(Int(Player,TEXT("Score"))!=20 || Int(Pickup,TEXT("Pickups"))!=2 || Runtime.ScriptTimerCount(Pickup)!=1 || !Error(Pickup).IsEmpty())return Fail(TEXT("second pickup: ")+Error(Pickup));
   UE_LOG(LogTemp,Display,TEXT("PASS pickup_reenter_awards_again"));
   State->Stage=2;State->Deadline=World->GetTimeSeconds()+3.f;return true;
  }
  if(State->Stage==2){
   auto* Pickup=State->Pickup.Get();if(!Pickup)return Fail(TEXT("pickup expired during occupied respawn"));
   if(Int(Pickup,TEXT("Pickups"))<3){if(World->GetTimeSeconds()>State->Deadline)return Fail(TEXT("occupied respawn did not issue a new overlap"));return true;}
   if(Int(Player,TEXT("Score"))!=30 || Int(Pickup,TEXT("Pickups"))!=3 || Int(Pickup,TEXT("Restores"))!=2 || Available(Pickup) || Runtime.ScriptTimerCount(Pickup)!=1 || !Error(Pickup).IsEmpty())return Fail(TEXT("occupied respawn reentrancy: ")+Error(Pickup));
   UE_LOG(LogTemp,Display,TEXT("PASS pickup_occupied_respawn_reentrant_overlap"));
   Pickup->Destroy();
   if(Runtime.ScriptTimerCount(Pickup)!=0 || Runtime.SubscriptionCount(Pickup)!=0)return Fail(TEXT("destroy retained timer/subscription"));
   State->Stage=3;State->Deadline=World->GetTimeSeconds()+.3f;return true;
  }
  if(State->Stage==3){
   if(World->GetTimeSeconds()<State->Deadline)return true;
   if(Int(Player,TEXT("Score"))!=30 || !Error(Player).IsEmpty())return Fail(TEXT("destroyed pickup callback changed player"));
   UE_LOG(LogTemp,Display,TEXT("PASS pickup_destroy_revokes_timer_and_subscription"));
   auto* Class=State->PickupClass.Get();
   if(!Class)return Fail(TEXT("pickup class missing for async cases"));
   auto Spawn=[&](const FSoftObjectPath& Path,int32 Index){
    const FTransform Transform(FVector(-1500,Index*300,100));
    auto* A=World->SpawnActorDeferred<AActor>(Class,Transform,nullptr,nullptr,ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
    if(!A)return static_cast<AActor*>(nullptr);
    auto* Soft=FindFProperty<FSoftObjectProperty>(Class,TEXT("MeshAsset"));
    auto* Collector=FindFProperty<FObjectPropertyBase>(Class,TEXT("Collector"));
    if(!Soft || !Collector){A->Destroy();return static_cast<AActor*>(nullptr);}
    Soft->SetPropertyValue_InContainer(A,FSoftObjectPtr(Path));Collector->SetObjectPropertyValue_InContainer(A,Player);
    A->FinishSpawning(Transform);return A;
   };
   State->Empty=Spawn(FSoftObjectPath(),0);
   State->Missing=Spawn(FSoftObjectPath(TEXT("/Game/DasExamples/AbsentPickupMesh.AbsentPickupMesh")),1);
   State->Retry=Spawn(State->MeshPath,2);
   auto* Destroyed=Spawn(State->MeshPath,3);State->Destroyed.Reset(Destroyed);
   if(!State->Empty.IsValid() || !State->Missing.IsValid() || !State->Retry.IsValid() || !Destroyed)return Fail(TEXT("async case spawn failed"));
   auto* Retry=State->Retry.Get();
   for(auto* A:{Retry,Destroyed}){
    auto* M=A->FindComponentByClass<UStaticMeshComponent>();auto* S=A->FindComponentByClass<USphereComponent>();
    if(!Flag(A,TEXT("MeshLoading")) || Flag(A,TEXT("MeshReady")) || Available(A) || Int(A,TEXT("MeshCallbacks"))!=0 || !M || M->GetStaticMesh() || M->IsVisible() || !S || S->GetCollisionEnabled()!=ECollisionEnabled::NoCollision || Runtime.SubscriptionCount(A)!=2)return Fail(FString::Printf(TEXT("pending async load was not gated: %s loading=%d ready=%d available=%d callbacks=%d mesh=%d visible=%d collision=%d bindings=%d error=%s"),*A->GetName(),Flag(A,TEXT("MeshLoading")),Flag(A,TEXT("MeshReady")),Available(A),Int(A,TEXT("MeshCallbacks")),M && M->GetStaticMesh()!=nullptr,M && M->IsVisible(),S?int32(S->GetCollisionEnabled()):-1,Runtime.SubscriptionCount(A),*Error(A)));
    auto* Restore=A->FindFunction(TEXT("Restore"));if(!Restore)return Fail(TEXT("Restore method missing"));A->ProcessEvent(Restore,nullptr);
    if(Available(A) || Int(A,TEXT("Restores"))!=0)return Fail(TEXT("manual restore bypassed pending mesh"));
   }
   auto* Reload=Retry->FindFunction(TEXT("LoadMesh"));if(!Reload)return Fail(TEXT("LoadMesh method missing"));Retry->ProcessEvent(Reload,nullptr);
   if(Int(Retry,TEXT("MeshCallbacks"))!=0 || Runtime.SubscriptionCount(Retry)!=2)return Fail(TEXT("replacement retained old request"));
   UE_LOG(LogTemp,Display,TEXT("PASS pickup_async_pending_gate_and_replacement"));
   Destroyed->Destroy();if(Runtime.SubscriptionCount(Destroyed)!=0 || Runtime.ScriptTimerCount(Destroyed)!=0)return Fail(TEXT("destroy retained asset request"));
   State->Deadline=World->GetTimeSeconds()+.5f;State->Stage=4;return true;
  }
  auto* Empty=State->Empty.Get();auto* Missing=State->Missing.Get();auto* Retry=State->Retry.Get();auto* Destroyed=State->Destroyed.Get();
  if(!Empty || !Missing || !Retry || !Destroyed)return Fail(TEXT("async cases expired"));
  if(Flag(Empty,TEXT("MeshLoading")) || Flag(Missing,TEXT("MeshLoading")) || Flag(Retry,TEXT("MeshLoading")) || World->GetTimeSeconds()<State->Deadline)return true;
  for(auto* A:{Empty,Missing}){
   auto* M=A->FindComponentByClass<UStaticMeshComponent>();auto* S=A->FindComponentByClass<USphereComponent>();
   if(Available(A) || Flag(A,TEXT("MeshReady")) || Int(A,TEXT("LoadFailures"))!=1 || Int(A,TEXT("MeshCallbacks"))!=1 || !M || M->GetStaticMesh() || M->IsVisible() || !S || S->GetCollisionEnabled()!=ECollisionEnabled::NoCollision || Runtime.SubscriptionCount(A)!=1 || !Error(A).IsEmpty())return Fail(TEXT("empty/missing mesh did not fail safely: ")+Error(A));
  }
  UE_LOG(LogTemp,Display,TEXT("PASS pickup_async_empty_mesh_unavailable"));
  UE_LOG(LogTemp,Display,TEXT("PASS pickup_async_missing_mesh_unavailable"));
  if(!Available(Retry) || !Flag(Retry,TEXT("MeshReady")) || !Flag(Retry,TEXT("MeshWasLoaded")) || Int(Retry,TEXT("MeshCallbacks"))!=1 || Int(Retry,TEXT("LoadFailures"))!=0 || Runtime.SubscriptionCount(Retry)!=1 || !Error(Retry).IsEmpty())return Fail(TEXT("replacement callback did not complete once: ")+Error(Retry));
  if(Int(Destroyed,TEXT("MeshCallbacks"))!=0 || Runtime.SubscriptionCount(Destroyed)!=0 || !Error(Destroyed).IsEmpty() || Int(Player,TEXT("Score"))!=30)return Fail(TEXT("late completion reached destroyed pickup"));
  UE_LOG(LogTemp,Display,TEXT("PASS pickup_async_destroy_suppresses_late_completion"));
  Empty->Destroy();Missing->Destroy();Retry->Destroy();State->Destroyed.Reset();
  UE_LOG(LogTemp,Display,TEXT("DAS_PICKUP_RESULT=PASS"));FPlatformMisc::RequestExitWithStatus(false,0);return false;
 }));
}
