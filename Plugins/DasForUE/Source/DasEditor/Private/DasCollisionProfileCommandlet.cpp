#include "DasCollisionProfileCommandlet.h"
#include "DasCollisionProfileProbe.h"
#include "DasRuntime.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UObject/StrongObjectPtr.h"
#include "Misc/Parse.h"
#include "Misc/ScopeExit.h"
UDasCollisionProfileCommandlet::UDasCollisionProfileCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasCollisionProfileCommandlet::Main(const FString& Params){
 FString Probe;if(!FParse::Value(*Params,TEXT("Probe="),Probe))return 1;
 auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
 int32 Failures=0;auto Check=[&](bool Good,const TCHAR* Name){UE_LOG(LogTemp,Display,TEXT("DAS_COLLISION_PROFILE_%s %s"),Good?TEXT("PASS"):TEXT("FAIL"),Name);if(!Good)++Failures;};
 TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game,false)),Foreign(UWorld::CreateWorld(EWorldType::Game,false)),Editor(UWorld::CreateWorld(EWorldType::Editor,false));
 for(auto* W:{World.Get(),Foreign.Get()}){GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(W);W->InitializeActorsForPlay(FURL());}
 ON_SCOPE_EXIT{for(auto* W:{World.Get(),Foreign.Get()}){GEngine->DestroyWorldContext(W);W->DestroyWorld(false);}Editor->DestroyWorld(false);};
 // The built-in adapter must remain guarded without editor-only thunk metadata.
 auto* Function=UKismetSystemLibrary::StaticClass()->FindFunctionByName(TEXT("SetCollisionProfileNameProperty"));
 const FString Thunk=Function->GetMetaData(TEXT("CustomThunk"));const bool HadThunk=Function->HasMetaData(TEXT("CustomThunk"));Function->RemoveMetaData(TEXT("CustomThunk"));
 ON_SCOPE_EXIT{if(HadThunk)Function->SetMetaData(TEXT("CustomThunk"),*Thunk);};
 const TCHAR* Names[]={TEXT("inherited_scalar"),TEXT("wrong_integer"),TEXT("wrong_struct"),TEXT("missing_property"),TEXT("readonly_property"),TEXT("hidden_property"),TEXT("fixed_array"),TEXT("dynamic_array"),TEXT("empty_name"),TEXT("null_target"),TEXT("template_target"),TEXT("foreign_world"),TEXT("destroy_after_frame_assignment"),TEXT("semantic_setter"),TEXT("unassigned_value"),TEXT("wrong_source_type"),TEXT("unrelated_custom_thunk")};
 for(int32 Mode=0;Mode<UE_ARRAY_COUNT(Names);++Mode){
  auto* Actor=World->SpawnActor<ADasCollisionProfileDerivedProbe>();auto* Other=Mode==10?GetMutableDefault<ADasCollisionProfileDerivedProbe>():Mode==11?Foreign->SpawnActor<ADasCollisionProfileDerivedProbe>():World->SpawnActor<ADasCollisionProfileDerivedProbe>();
  Actor->OtherActor=Other;const FName Before=Other->Profile.Name;
  const auto Result=Runtime.RunFile(Probe,Mode,false,Actor);

  const bool Positive=Mode==0 || Mode==13;
  const TCHAR* Expected[]={TEXT(""),TEXT("scalar FCollisionProfileName"),TEXT("scalar FCollisionProfileName"),TEXT("scalar FCollisionProfileName"),TEXT("hidden or read-only"),TEXT("hidden or read-only"),TEXT("scalar FCollisionProfileName"),TEXT("scalar FCollisionProfileName"),TEXT("name is empty"),TEXT("live non-template target"),TEXT("live non-template target"),TEXT("live game world"),TEXT("object argument expired"),TEXT(""),TEXT("missing input argument"),TEXT("type mismatch"),TEXT("special call adapter")};
  const bool RunOK=Positive?(Result.bSucceeded && Result.Value==42):(!Result.bSucceeded && Result.Error.Contains(Expected[Mode]));
  if(!RunOK)UE_LOG(LogTemp,Error,TEXT("mode=%d: %s"),Mode,*Result.Error);
  Check(RunOK && (Mode==0?Actor->Profile.Name==TEXT("BlockAll"):Actor->Profile.Name==TEXT("Before")) && (Mode==13?Actor->Semantic.Name==TEXT("BlockAll") && Actor->SetterCalls==1:Actor->Semantic.Name==TEXT("Before") && Actor->SetterCalls==0) && Actor->Readonly.Name==TEXT("Before") && Actor->Hidden.Name==TEXT("Before") && Actor->Counter==7 && Actor->Direction==FVector::ZeroVector && Other->Profile.Name==Before,Names[Mode]);
  Runtime.ReleaseObject(Actor);Actor->Destroy();if(!Other->IsTemplate())Other->Destroy();
 }
 auto* Actor=World->SpawnActor<ADasCollisionProfileDerivedProbe>();
 auto Construction=Runtime.RunConstruction(Probe,Actor);Check(!Construction.bSucceeded && Construction.Error.Contains(TEXT("unsafe during ConstructionScript")) && Actor->Profile.Name==TEXT("Before"),TEXT("construction_without_metadata"));Runtime.ReleaseObject(Actor);
 auto Begin=Runtime.RunLifecycle(Probe,EDasCallback::BeginPlay,Actor);auto End=Runtime.RunLifecycle(Probe,EDasCallback::EndPlay,Actor);
 Check(Begin.bSucceeded && !End.bSucceeded && End.Error.Contains(TEXT("requires gameplay")) && Actor->Profile.Name==TEXT("Before"),TEXT("EndPlay_without_metadata"));Runtime.ReleaseObject(Actor);Actor->Destroy();
 auto* EditorActor=Editor->SpawnActor<ADasCollisionProfileDerivedProbe>();const auto EditorResult=Runtime.RunFile(Probe,18,false,EditorActor);
 Check(!EditorResult.bSucceeded && EditorResult.Error.Contains(TEXT("live game world")) && EditorActor->Profile.Name==TEXT("Before"),TEXT("editor_world"));Runtime.ReleaseObject(EditorActor);
 const auto Missing=Runtime.RunFile(Probe,18,false);Check(!Missing.bSucceeded && Missing.Error.Contains(TEXT("self is missing")),TEXT("missing_world_context"));
 FString BindProbe;if(!FParse::Value(*Params,TEXT("BindProbe="),BindProbe))return 1;
 auto* BindActor=World->SpawnActor<ADasCollisionProfileDerivedProbe>();const auto Binding=Runtime.RunLifecycle(BindProbe,EDasCallback::BeginPlay,BindActor);
 Check(!Binding.bSucceeded && Binding.Error.Contains(TEXT("bind_events")) && BindActor->Profile.Name==TEXT("Before"),TEXT("bind_events"));Runtime.ReleaseObject(BindActor);
 UE_LOG(LogTemp,Display,TEXT("DAS_COLLISION_PROFILE_RESULT=%s checks=22"),Failures?TEXT("FAIL"):TEXT("PASS"));return Failures?1:0;
}
