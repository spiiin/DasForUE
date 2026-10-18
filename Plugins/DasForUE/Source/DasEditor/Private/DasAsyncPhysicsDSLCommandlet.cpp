#include "DasAsyncPhysicsDSLCommandlet.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "DasProbeActor.h"
#include "DasScriptComponent.h"
#include "DasAsyncPhysicsBridge.h"
#include "Components/BoxComponent.h"
#include "Engine/Blueprint.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#include "Misc/ScopeExit.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
UDasAsyncPhysicsDSLCommandlet::UDasAsyncPhysicsDSLCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasAsyncPhysicsDSLCommandlet::Main(const FString& Params){
 FString Script,Physics,Package;if(!FParse::Value(*Params,TEXT("Script="),Script) || !FParse::Value(*Params,TEXT("Physics="),Physics) || !FParse::Value(*Params,TEXT("Package="),Package))return 1;
 auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));FString Error;
 int32 Failures=0,Checks=0;auto Check=[&](bool Good,const TCHAR* Name){++Checks;UE_LOG(LogTemp,Display,TEXT("DAS_ASYNC_DSL_%s %s"),Good?TEXT("PASS"):TEXT("FAIL"),Name);if(!Good)++Failures;};
 const FString Path=Runtime.ScriptPath(Script),PhysPath=Runtime.ScriptPath(Physics);FString Source,PhysSource;FFileHelper::LoadFileToString(Source,*Path);FFileHelper::LoadFileToString(PhysSource,*PhysPath);ON_SCOPE_EXIT{FFileHelper::SaveStringToFile(Source,*Path);FFileHelper::SaveStringToFile(PhysSource,*PhysPath);};
 FDasAuthoringModule Model,RoundTrip;FString Json;Check(Runtime.ReadAuthoringScript(Path,Model,Error),TEXT("annotation_compiles_into_typed_model"));if(Model.Classes.IsEmpty()){UE_LOG(LogTemp,Error,TEXT("%s"),*Error);return 1;}
 FDasDeclaredClass Effective;Check(Model.ResolveClass(TEXT("asyncphysicsdsl.Thruster"),Effective,Error) && Effective.AsyncPhysics && Effective.AsyncPhysics->Bodies.FindRef(0)==TEXT("Body"),TEXT("effective_body_slot_and_script"));
 Check(Model.ToJson(Json,Error) && FDasAuthoringModule::FromJson(Json,RoundTrip,Error) && RoundTrip.Classes.Last().AsyncPhysics==Model.Classes.Last().AsyncPhysics,TEXT("typed_IR_JSON_roundtrip"));
 if(!Effective.AsyncPhysics){Model.ToJson(Json,Error);UE_LOG(LogTemp,Error,TEXT("missing physics recipe: %s"),*Json);return 1;}
 auto Changed=Model.Classes.Last();Changed.AsyncPhysics->Bodies.Add(1,TEXT("Body2"));Check(FDasAuthoringModule::Compare(Model.Classes.Last(),Changed).bStructural,TEXT("body_recipe_change_requires_reimport"));
 UBlueprint* BP=nullptr;Check(FDasDSLService::Apply(Script,Package,TEXT("asyncphysicsdsl.Thruster"),BP,Error),TEXT("import_without_native_registration"));if(!BP){UE_LOG(LogTemp,Error,TEXT("%s"),*Error);return 1;}
 auto* Settings=GetMutableDefault<UPhysicsSettings>();const bool OldAsync=Settings->bTickPhysicsAsync;const float OldDt=Settings->AsyncFixedTimeStepSize;Settings->bTickPhysicsAsync=FParse::Param(*Params,TEXT("AsyncFixed"));Settings->AsyncFixedTimeStepSize=1.f/60.f;ON_SCOPE_EXIT{Settings->bTickPhysicsAsync=OldAsync;Settings->AsyncFixedTimeStepSize=OldDt;};
 const auto IVS=UWorld::InitializationValues().CreatePhysicsScene(true).ShouldSimulatePhysics(true).AllowAudioPlayback(false).CreateNavigation(false).CreateAISystem(false);
 TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&IVS));GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());World->InitializeActorsForPlay(FURL());
 ON_SCOPE_EXIT{UDasAsyncPhysicsBridge::RevokeAll();if(World){GEngine->DestroyWorldContext(World.Get());World->DestroyWorld(false);}};
 auto* Actor=World->SpawnActor<ADasProbeActor>(BP->GeneratedClass);TStrongObjectPtr<ADasProbeActor> Keep(Actor);auto* BodyProp=FindFProperty<FObjectPropertyBase>(Actor->GetClass(),TEXT("Body"));auto* Body=BodyProp?Cast<UBoxComponent>(BodyProp->GetObjectPropertyValue_InContainer(Actor)):nullptr;
 auto Bridges=[&](){TArray<UDasAsyncPhysicsBridge*> Found;Actor->GetComponents(Found);return Found.Num();};
 auto I=[&](const TCHAR* Name){auto* F=FindFProperty<FIntProperty>(Actor->GetClass(),Name);return F?F->GetPropertyValue_InContainer(Actor):-1;};auto D=[&](const TCHAR* Name){auto* F=FindFProperty<FDoubleProperty>(Actor->GetClass(),Name);return F?F->GetPropertyValue_InContainer(Actor):-1.;};
 Check(Body && I(TEXT("Initializes"))==0 && Actor->LastDasError.IsEmpty(),TEXT("component_created_before_gameplay_lifecycle"));
 Check(Bridges()==0,TEXT("no_bridge_before_BeginPlay"));World->BeginPlay();World->GetWorldSettings()->NotifyBeginPlay();
 auto Step=[&](int32 N){for(int32 J=0;J<N;++J){++GFrameCounter;World->Tick(LEVELTICK_All,1.f/60.f);}};Step(12);
 Check(I(TEXT("Begins"))==1 && I(TEXT("Initializes"))==1 && Body->IsSimulatingPhysics() && Actor->LastDasError.IsEmpty() && Bridges()==1,TEXT("single_automatic_binding_after_BeginPlay"));
 Check(Body && Body->GetPhysicsLinearVelocity().X>0 && D(TEXT("PhysicsSeconds"))>0,TEXT("DAS_only_force_and_gameplay_output_exchange"));
 const int32 Begins=I(TEXT("Begins")),Initializes=I(TEXT("Initializes"));
 const auto BadSource=PhysSource+TEXT("\ninvalid physics source\n");FFileHelper::SaveStringToFile(BadSource,*PhysPath);UBlueprint* Refused=nullptr;Check(!FDasDSLService::Apply(Script,Package+TEXT("_Refused"),TEXT("asyncphysicsdsl.Thruster"),Refused,Error) && !Refused,TEXT("physics_preflight_rejects_before_Blueprint_creation"));const auto Rejected=Runtime.RunLifecycle(Path,EDasCallback::Tick,Actor,1.f/60.f);const double Before=D(TEXT("PhysicsSeconds"));Step(4);
 Check(Rejected.bSucceeded && !Rejected.ReloadError.IsEmpty() && D(TEXT("PhysicsSeconds"))>Before && Bridges()==1,TEXT("broken_physics_reload_preserves_live_VM"));
 FFileHelper::SaveStringToFile(PhysSource+TEXT("\n// repaired physics revision\n"),*PhysPath);const auto Accepted=Runtime.RunLifecycle(Path,EDasCallback::Tick,Actor,1.f/60.f);Step(4);
 Check(Accepted.bSucceeded && Accepted.ReloadError.IsEmpty() && Accepted.Revision>Rejected.Revision && Bridges()==1 && I(TEXT("Begins"))==Begins && I(TEXT("Initializes"))==Initializes,TEXT("physics_reload_swaps_once_without_replaying_lifecycle"));
 FFileHelper::SaveStringToFile(Source+TEXT("\n// gameplay implementation revision\n"),*Path);const auto GameReload=Runtime.RunLifecycle(Path,EDasCallback::Tick,Actor,1.f/60.f);Step(3);Check(GameReload.bSucceeded && GameReload.ReloadError.IsEmpty() && Bridges()==1 && I(TEXT("Begins"))==Begins,TEXT("gameplay_reload_rebinds_physics_without_duplicate_bridge"));
 const FString Invalid=Source.Replace(TEXT("body_0=\"Body\""),TEXT("body_8=\"Body\""));FFileHelper::SaveStringToFile(Invalid,*Path);FDasAuthoringModule Bad;Check(!Runtime.ReadAuthoringScript(Path,Bad,Error),TEXT("invalid_annotation_slot_rejected"));
 FFileHelper::SaveStringToFile(Source.Replace(TEXT("body_0=\"Body\""),TEXT("body_0=\"MissingBody\"")),*Path);Check(!Runtime.ReadAuthoringScript(Path,Bad,Error),TEXT("missing_component_rejected_in_authoring"));FFileHelper::SaveStringToFile(Source,*Path);
 FFileHelper::SaveStringToFile(PhysSource,*PhysPath);
 TWeakObjectPtr<UObject> WeakActor=Actor,WeakBody=Body;Actor->Destroy();Keep.Reset();Actor=nullptr;Body=nullptr;Step(3);CollectGarbage(RF_NoFlags);Check(!WeakActor.IsValid() && !WeakBody.IsValid() && Runtime.PersistentContextCount(World.Get())==0,TEXT("automatic_Destroy_GC_cleanup"));
 // Failed gameplay BeginPlay must never enable physics on a later Tick.
 FFileHelper::SaveStringToFile(Source.Replace(TEXT("self.Begins+=1"),TEXT("self.Begins+=1; panic(\"intentional BeginPlay failure\")")),*Path);
 UBlueprint* FailedBP=nullptr;Check(FDasDSLService::Apply(Script,Package+TEXT("_FailedBegin"),TEXT("asyncphysicsdsl.Thruster"),FailedBP,Error),TEXT("failed_BeginPlay_fixture_import"));
 if(FailedBP){auto* Failed=World->SpawnActor<ADasProbeActor>(FailedBP->GeneratedClass);if(!Failed->HasActorBegunPlay())Failed->DispatchBeginPlay();Step(3);TArray<UDasAsyncPhysicsBridge*> Unexpected;Failed->GetComponents(Unexpected);Check(Unexpected.IsEmpty() && Failed->LastDasError.Contains(TEXT("BeginPlay")),TEXT("failed_BeginPlay_blocks_deferred_registration"));
  const FString ProbePath=Path+TEXT(".mailbox_probe.das");if(IFileManager::Get().FileExists(*ProbePath)){UE_LOG(LogTemp,Error,TEXT("preexisting mailbox probe path"));return 1;}ON_SCOPE_EXIT{IFileManager::Get().Delete(*ProbePath);};FFileHelper::SaveStringToFile(TEXT("options gen2\nrequire ue_foundation\n[export]\ndef main(input:int):int { return !ue_async_physics_running() && !ue_async_physics_has_output() && length(ue_async_physics_error())>0 ? 1 : 0 }\n"),*ProbePath);const auto Observed=Runtime.RunFile(ProbePath,0,false,Failed);Check(Observed.bSucceeded && Observed.Value==1,TEXT("gameplay_error_and_readiness_mailbox_API"));Failed->Destroy();}
 FFileHelper::SaveStringToFile(Source,*Path);Step(2);
 // Import an actual child DSL class in a separate source. Its inherited
 // Initialize/Tick execute in parent gameplay contexts but share one leaf VM.
 const FString DerivedName=FPaths::GetBaseFilename(Script)+TEXT("_derived.das"),DerivedPath=FPaths::GetPath(Path)/DerivedName;
 const FString DerivedSource=FString::Printf(TEXT("options gen2\nrequire ue_dsl\nrequire %s public\n[ue_class(module_id=\"asyncphysicschild\",Blueprintable)]\nclass Child:Thruster {}\n"),*FPaths::GetBaseFilename(Script));
 if(IFileManager::Get().FileExists(*DerivedPath)){UE_LOG(LogTemp,Error,TEXT("unexpected preexisting derived test script"));return 1;}FFileHelper::SaveStringToFile(DerivedSource,*DerivedPath);ON_SCOPE_EXIT{IFileManager::Get().Delete(*DerivedPath);};
 UBlueprint* ChildBP=nullptr;Check(FDasDSLService::Apply(DerivedName,Package+TEXT("_Child"),TEXT("asyncphysicschild.Child"),ChildBP,Error),TEXT("inherited_recipe_import"));
 if(!ChildBP)UE_LOG(LogTemp,Display,TEXT("Child import failure: %s"),*Error);
 if(ChildBP){auto* Child=World->SpawnActor<ADasProbeActor>(ChildBP->GeneratedClass);auto* Count=FindFProperty<FIntProperty>(Child->GetClass(),TEXT("Initializes"));Step(5);TArray<UDasAsyncPhysicsBridge*> ChildBridges;Child->GetComponents(ChildBridges);Check(ChildBridges.Num()==1 && Count && Count->GetPropertyValue_InContainer(Child)==1 && Child->LastDasError.IsEmpty(),TEXT("inherited_callbacks_share_leaf_mailbox_and_single_VM"));
  FDasScalarValue Force;Force.Kind=6;Force.DoubleValue=50;const auto ParentCall=Runtime.RunNamedFramePersistent(Path,TEXT("ue_Thruster_SetForce"),{Force},EDasMethodSignature::Void,Child);
  const auto ChildTick=Runtime.RunLifecycle(Runtime.ScriptPath(DerivedName),EDasCallback::Tick,Child,1.f/60.f);Step(2);Check(ParentCall.bSucceeded && ChildTick.bSucceeded && Child->LastDasError.IsEmpty(),TEXT("parent_session_cannot_replace_leaf_mailbox"));Child->Destroy();}
 const FString ComponentName=FPaths::GetBaseFilename(Script)+TEXT("_component.das"),ComponentPath=FPaths::GetPath(Path)/ComponentName;
 if(IFileManager::Get().FileExists(*ComponentPath)){UE_LOG(LogTemp,Error,TEXT("unexpected existing test component script"));return 1;}ON_SCOPE_EXIT{IFileManager::Get().Delete(*ComponentPath);};
 const FString ComponentSource=FString::Printf(TEXT("options gen2\nrequire ue_dsl\n[ue_async_physics(script=\"%s\",body_0=\"owner.RootComponent\"),ue_class(module_id=\"asyncdriver\",Blueprintable)]\nclass Driver:ActorComponent {\n [ue_lifecycle]\n def BeginPlay():void { ue_async_physics_set_input(0,1000.0lf) }\n [ue_function(BlueprintCallable)]\n def Read():double { if(ue_async_physics_has_output()) { return ue_async_physics_output(1) };return -1.0lf }\n}\n"),*Physics);FFileHelper::SaveStringToFile(ComponentSource,*ComponentPath);
 UBlueprint* DriverBP=nullptr;Check(FDasDSLService::Apply(ComponentName,Package+TEXT("_Driver"),TEXT("asyncdriver.Driver"),DriverBP,Error),TEXT("reversed_annotation_order_and_component_import"));
 if(!DriverBP)UE_LOG(LogTemp,Display,TEXT("Driver import failure: %s"),*Error);
 if(DriverBP){auto* Host=World->SpawnActorDeferred<AActor>(AActor::StaticClass(),FTransform::Identity);auto* B=NewObject<UBoxComponent>(Host);B->SetBoxExtent(FVector(20));Host->AddInstanceComponent(B);Host->SetRootComponent(B);B->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);B->SetCollisionResponseToAllChannels(ECR_Ignore);B->RegisterComponent();B->SetEnableGravity(false);B->SetLinearDamping(0);B->SetSimulatePhysics(true);B->SetMassOverrideInKg(NAME_None,10,true);
  auto* Driver=NewObject<UDasScriptComponent>(Host,DriverBP->GeneratedClass);Host->AddInstanceComponent(Driver);Driver->RegisterComponent();Host->FinishSpawning(FTransform::Identity);if(!Host->HasActorBegunPlay())Host->DispatchBeginPlay();Step(12);TArray<UDasAsyncPhysicsBridge*> Registered;Host->GetComponents(Registered);UE_LOG(LogTemp,Display,TEXT("Driver state: actorBegun=%d driverBegun=%d bridges=%d velocity=%g error=%s"),Host->HasActorBegunPlay(),Driver->HasBegunPlay(),Registered.Num(),B->GetPhysicsLinearVelocity().X,*Driver->LastDasError);auto* CurrentBody=Cast<UPrimitiveComponent>(Host->GetRootComponent());const auto Observed=Runtime.RunNamedFramePersistent(ComponentPath,TEXT("ue_Driver_Read"),{},EDasMethodSignature::Double,Driver);UE_LOG(LogTemp,Display,TEXT("Driver physics: observedSuccess=%d output=%g obsError=%s root=%s body=%s same=%d sim=%d mass=%g"),Observed.bSucceeded,Observed.DoubleValue,*Observed.Error,*GetNameSafe(CurrentBody),*GetNameSafe(B),CurrentBody==B,B->IsSimulatingPhysics(),B->GetMass());for(auto* Bridge:Registered)UE_LOG(LogTemp,Display,TEXT("Driver bridge live=%d error=%s"),Bridge->IsLiveBinding(),*Bridge->GetLastError());Check(Registered.Num()==1 && Driver->LastDasError.IsEmpty() && B->GetPhysicsLinearVelocity().X>0,TEXT("component_owner_starts_only_after_actor_BeginPlay_completes"));UE_LOG(LogTemp,Display,TEXT("Driver host destroy=%d"),Host->Destroy());}
 // A class with only the physics declaration has no synthetic fake UFunction.
 FFileHelper::SaveStringToFile(FString::Printf(TEXT("options gen2\nrequire ue_dsl\n[ue_class(module_id=\"onlyphysics\",Blueprintable),ue_async_physics(script=\"%s\")]\nclass OnlyPhysics {}\n"),*Physics),*ComponentPath);FDasAuthoringModule Only;Check(Runtime.ReadAuthoringScript(ComponentPath,Only,Error) && Only.Classes.Last().AsyncPhysics && Only.Classes.Last().Methods.IsEmpty(),TEXT("async_only_class_without_lifecycle_or_methods"));

 UE_LOG(LogTemp,Display,TEXT("DAS_ASYNC_DSL_RESULT=%s checks=%d"),Failures?TEXT("FAIL"):TEXT("PASS"),Checks);return Failures?1:0;
}
