#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "DasRuntime.h"
#include "DasClassMethodsProbe.h"
#include "DasInitializerProbe.h"
#include "DasProbeActor.h"
#include "DasScriptComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
namespace {
int32 CallInt(UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FIntProperty>(O->GetClass(),N):nullptr;return P?P->GetPropertyValue_InContainer(O):MIN_int32;}
struct FCookedCalls {
 TWeakObjectPtr<UWorld> World;TWeakObjectPtr<ADasProbeActor> Driver,Library,Inherited,Skipped,Failure;TStrongObjectPtr<ADasProbeActor> Spawned;TStrongObjectPtr<UDasScriptComponent> Component;
 double Started=FPlatformTime::Seconds();int Phase=0;bool Good=true;
 void Check(bool OK,const TCHAR* Name){Good &= OK;UE_LOG(LogTemp,Display,TEXT("DAS_COOKED_CALLS %s %s"),OK?TEXT("PASS"):TEXT("FAIL"),Name);}
 FDasExecutionResult Run(ADasProbeActor* Self,const TCHAR* Script,const TCHAR* Entry){auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));auto V=R.RunNamedBoolPersistent(R.ScriptPath(Script),Entry,false,Self);if(!V.bSucceeded)UE_LOG(LogTemp,Display,TEXT("COOKED_CALL_ERROR %s: %s"),Entry,*V.Error);return V;}
 void Pass(ADasProbeActor* Self,const TCHAR* Script,const TCHAR* Entry,const TCHAR* Name){auto V=Run(Self,Script,Entry);Check(V.bSucceeded && V.BoolValue,Name);}
 bool Finish(){auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));Spawned.Reset();Component.Reset();for(auto* A:{Driver.Get(),Library.Get(),Inherited.Get(),Skipped.Get(),Failure.Get()})if(A){R.ReleaseObject(A);A->Destroy();}Check(R.PersistentContextCount(World.Get())==0,TEXT("sessions_released"));UE_LOG(LogTemp,Display,TEXT("DAS_COOKED_CALLS_RESULT=%s"),Good?TEXT("PASS"):TEXT("FAIL"));FPlatformMisc::RequestExitWithStatus(false,Good?0:1);return false;}
 bool Tick(float){
  auto* W=World.Get();auto* D=Driver.Get();auto* L=Library.Get();
  if(FPlatformTime::Seconds()-Started>60){Check(false,TEXT("timeout"));return Finish();}
  if(!W || !W->HasBegunPlay() || !D || !L || !Inherited.IsValid() || !Skipped.IsValid() || !Failure.IsValid())return true;
  auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
  if(Phase==0){
#if WITH_EDITOR
   Check(false,TEXT("game_target_without_editor"));
#else
   Check(true,TEXT("game_target_without_editor"));
#endif
   auto* Field=FindFProperty<FClassProperty>(D->GetClass(),TEXT("SpawnClass"));auto* Target=Field?Cast<UClass>(Field->GetObjectPropertyValue_InContainer(D)):nullptr;
   Check(Target && Target->IsChildOf(ADasProbeActor::StaticClass()) && Target->GetName()==TEXT("BP_Child_C"),TEXT("serialized_class_reference_and_cook_dependency"));
   Pass(D,TEXT("dsl_spawn_driver.das"),TEXT("ue_SpawnDriver_QueryEmpty"),TEXT("empty_authored_queries"));
   Pass(D,TEXT("dsl_spawn_driver.das"),TEXT("ue_SpawnDriver_Native"),TEXT("native_spawn_destroy"));
   Pass(D,TEXT("dsl_spawn_driver.das"),TEXT("ue_SpawnDriver_Spawn"),TEXT("authored_spawn_BeginPlay"));
   auto* Object=FindFProperty<FObjectProperty>(D->GetClass(),TEXT("Spawned"));Spawned.Reset(Object?Cast<ADasProbeActor>(Object->GetObjectPropertyValue_InContainer(D)):nullptr);
   if(!Spawned.IsValid()){Check(false,TEXT("spawned_actor_missing"));return Finish();}
   Check(Spawned->GetClass()==Target && Spawned->GetOwner()==D && Spawned->GetActorLocation().Equals(FVector(125,250,375)),TEXT("spawn_class_owner_transform"));
   auto* Life=FindFProperty<FObjectProperty>(Spawned->GetClass(),TEXT("Life"));Component.Reset(Life?Cast<UDasScriptComponent>(Life->GetObjectPropertyValue_InContainer(Spawned.Get())):nullptr);
   FString InitReport;Check(RunDasInitializerProbe(Spawned.Get(),Component.Get(),InitReport),TEXT("spawned_initializer_hierarchy"));Check(RunDasInitializerLifetimeProbe(Spawned->GetClass(),Inherited->GetClass(),Skipped->GetClass(),Failure->GetClass(),InitReport),TEXT("initializer_lifetime_guards"));UE_LOG(LogTemp,Display,TEXT("%s"),*InitReport);
   Check(CallInt(Spawned.Get(),TEXT("SeenEditable"))==41,TEXT("spawned_initializer_preserves_Blueprint_default"));Check(CallInt(Inherited.Get(),TEXT("InitCount"))==1 && CallInt(Inherited.Get(),TEXT("BeginSawTrace"))==1 && CallInt(Inherited.Get(),TEXT("SeenEditable"))==49,TEXT("placed_actor_initializer_preserves_instance_edit"));Check(CallInt(Failure.Get(),TEXT("Attempts"))==1 && CallInt(Failure.Get(),TEXT("Begins"))==0,TEXT("placed_failed_initializer_blocks_BeginPlay"));
   FString ClassReport;Check(RunDasClassMethodsProbe(Spawned.Get(),ClassReport),TEXT("authored_actor_class_methods"));Check(RunDasClassMethodsProbe(Component.Get(),ClassReport),TEXT("authored_component_class_methods"));UE_LOG(LogTemp,Display,TEXT("%s"),*ClassReport);
   Pass(D,TEXT("dsl_spawn_driver.das"),TEXT("ue_SpawnDriver_ClassMethods"),TEXT("cross_context_class_method_accessors"));auto BadClassMethod=Run(D,TEXT("dsl_spawn_driver.das"),TEXT("ue_SpawnDriver_WrongClassMethod"));Check(!BadClassMethod.bSucceeded,TEXT("cross_context_class_base_rejected"));auto ForgedClassMethod=Run(D,TEXT("dsl_spawn_driver.das"),TEXT("ue_SpawnDriver_ForgedClassMethod"));Check(!ForgedClassMethod.bSucceeded,TEXT("forged_typed_class_base_rejected"));
   Pass(D,TEXT("dsl_spawn_driver.das"),TEXT("ue_SpawnDriver_SpawnExtra"),TEXT("second_actor"));
   Pass(D,TEXT("dsl_spawn_driver.das"),TEXT("ue_SpawnDriver_QueryMatches"),TEXT("typed_actor_component_queries"));
   Pass(L,TEXT("dsl_function_libraries.das"),TEXT("ue_LibraryProbe_Check"),TEXT("static_engine_user_libraries_worldcontext"));
   Pass(L,TEXT("dsl_function_libraries.das"),TEXT("ue_LibraryProbe_Classes"),TEXT("class_input_return_out_arrays_static_instance_calls"));
   Pass(L,TEXT("dsl_function_libraries.das"),TEXT("ue_LibraryProbe_AbstractClass"),TEXT("abstract_class_value"));
   auto WorldId=R.RunNamedPersistent(R.ScriptPath(TEXT("dsl_function_libraries.das")),TEXT("ue_LibraryProbe_WorldId"),0,L);Check(WorldId.bSucceeded && WorldId.Value==int32(W->GetUniqueID()),TEXT("worldcontext_without_editor_metadata"));
   for(const auto* Entry:{TEXT("ue_LibraryProbe_OverrideWorld"),TEXT("ue_LibraryProbe_NotLibrary"),TEXT("ue_LibraryProbe_WrongMetaClass")}){auto Rejected=Run(L,TEXT("dsl_function_libraries.das"),Entry);Check(!Rejected.bSucceeded && !Rejected.Error.IsEmpty(),Entry);}
   Phase=1;return true;
  }
  if((CallInt(Spawned.Get(),TEXT("Ticks"))<2 || CallInt(Component.Get(),TEXT("Ticks"))<2))return true;
  Check(CallInt(Spawned.Get(),TEXT("Begins"))==1 && CallInt(Component.Get(),TEXT("Begins"))==1,TEXT("actor_component_BeginPlay_Tick_once"));
  Pass(D,TEXT("dsl_spawn_driver.das"),TEXT("ue_SpawnDriver_Remove"),TEXT("destroy_and_stale_query_handles"));
  Check(CallInt(Spawned.Get(),TEXT("Ends"))==1 && CallInt(Component.Get(),TEXT("Ends"))==1,TEXT("actor_component_EndPlay_once"));
  Check(Spawned->LastDasError.IsEmpty() && Component.IsValid() && Component->LastDasError.IsEmpty(),TEXT("no_lifecycle_errors"));return Finish();
 }
};
}
void StartDasCookedCallsValidation(){auto S=MakeShared<FCookedCalls>();for(const auto& C:GEngine->GetWorldContexts())if(auto* W=C.World();W && W->IsGameWorld()){S->World=W;for(TActorIterator<ADasProbeActor> It(W);It;++It){if(It->ActorHasTag(TEXT("CookedCallDriver")))S->Driver=*It;if(It->ActorHasTag(TEXT("CookedLibrary")))S->Library=*It;if(It->ActorHasTag(TEXT("InitializerInherited")))S->Inherited=*It;if(It->ActorHasTag(TEXT("InitializerSkipped")))S->Skipped=*It;if(It->ActorHasTag(TEXT("InitializerFailure")))S->Failure=*It;}}FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([S](float D){return S->Tick(D);}));}
