#include "DasStaticLibraryProbe.h"
#include "DasScriptHosts.h"
#include "DasRuntime.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/StructOnScope.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "Modules/ModuleManager.h"

bool RunDasPawnCharacterProbe(ADasStaticLibraryProbe* Carrier,FString& Report){
 bool Good=true;auto Check=[&](bool V,const FString& Name){Good &= V;Report+=FString(V?TEXT("PASS "):TEXT("FAIL "))+Name+TEXT("\n");};
 if(!Carrier || !Carrier->GetWorld() || !Carrier->GetWorld()->IsGameWorld() || Carrier->PawnCharacterClasses.Num()!=8){Check(false,TEXT("pawn_character_cooked_dependencies"));return false;}
 auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
 auto Int=[](UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FIntProperty>(O->GetClass(),N):nullptr;return P?P->GetPropertyValue_InContainer(O):MIN_int32;};
 for(int32 I=0;I<6;++I){
  UClass* Class=Carrier->PawnCharacterClasses[I];const bool Character=I>=3,Child=I==1 || I==4,Consumer=I==2 || I==5;
  Check(Class && Class->IsChildOf(Character?ACharacter::StaticClass():APawn::StaticClass()),TEXT("pawn_character_real_native_parent_")+FString::FromInt(I));if(!Class)continue;
  FActorSpawnParameters Spawn;Spawn.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
  auto* A=Carrier->GetWorld()->SpawnActor<APawn>(Class,FTransform::Identity,Spawn);if(!A){Check(false,TEXT("pawn_character_spawn"));continue;}TStrongObjectPtr<APawn> Keep(A);
  Check(A->HasActorBegunPlay() && Int(A,TEXT("Score"))==(Child?23:13),TEXT("pawn_character_initialize_construction_BeginPlay_")+FString::FromInt(I));
  if(Character){auto* C=CastChecked<ACharacter>(A);Check(C->GetRootComponent()==C->GetCapsuleComponent() && C->GetMesh() && C->GetCharacterMovement() && C->GetCharacterMovement()->MaxWalkSpeed==450.f,TEXT("character_native_components_and_defaults"));}
  auto* Query=A->FindFunction(TEXT("Probe"));int32 Value=0;if(Query)A->ProcessEvent(Query,&Value);Check(Query && Value==(Child?30:15),TEXT("pawn_character_self_virtual_Super_")+FString::FromInt(I));
  const auto Reference=R.RunNamedFramePersistent(R.ScriptPath(Character?TEXT("dsl_character_player_calls.das"):TEXT("dsl_pawn_player_calls.das")),TEXT("main"),{},EDasMethodSignature::Int,A);Check(Reference.bSucceeded && Reference.Value==(Child?60:30),TEXT("pawn_character_typed_reference_dispatch_")+FString::FromInt(I));if(!Reference.bSucceeded)Report+=Reference.Error+TEXT("\n");
  if(Consumer){Value=0;auto* Evaluate=A->FindFunction(TEXT("EvaluatePure"));if(Evaluate)A->ProcessEvent(Evaluate,&Value);Check(Evaluate && Value==15,TEXT("pawn_character_Blueprint_consumer"));}
  auto* Native=A->FindFunction(TEXT("NativeHeight"));if(Native){FStructOnScope Args(Native);auto* Input=FindFProperty<FFloatProperty>(Native,TEXT("Input"));if(!Input)Input=FindFProperty<FFloatProperty>(Native,TEXT("value"));if(Input){Input->SetPropertyValue_InContainer(Args.GetStructMemory(),72.f);A->ProcessEvent(Native,Args.GetStructMemory());}Check(Input && A->BaseEyeHeight==72.f,TEXT("pawn_character_live_native_self"));}
  auto* Adjust=A->FindFunction(TEXT("Adjust"));if(Adjust){FStructOnScope Args(Adjust);auto* Total=FindFProperty<FIntProperty>(Adjust,TEXT("total"));auto* Doubled=FindFProperty<FIntProperty>(Adjust,TEXT("doubled"));if(Total && Doubled){Total->SetPropertyValue_InContainer(Args.GetStructMemory(),4);A->ProcessEvent(Adjust,Args.GetStructMemory());Check(Total->GetPropertyValue_InContainer(Args.GetStructMemory())==4+Int(A,TEXT("Score")) && Doubled->GetPropertyValue_InContainer(Args.GetStructMemory())==(4+Int(A,TEXT("Score")))*2,TEXT("pawn_character_ref_out_frame_copyback"));}else Check(false,TEXT("pawn_character_ref_out_pins"));}else Check(false,TEXT("pawn_character_ref_out_method"));
  A->Tick(0.01f);Check(Int(A,TEXT("Ticks"))>0,TEXT("pawn_character_Tick"));
  const auto* Error=DasHostString(A,TEXT("LastDasError"));Check(Error && Error->IsEmpty(),TEXT("pawn_character_no_runtime_errors"));
  A->Destroy();Check(Int(A,TEXT("Ends"))==1,TEXT("pawn_character_EndPlay_once"));R.ReleaseObject(A);
 }
 for(int32 I=6;I<8;++I){auto* Class=Carrier->PawnCharacterClasses[I].Get();FActorSpawnParameters Spawn;Spawn.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;auto* A=Class?Carrier->GetWorld()->SpawnActor<APawn>(Class,FTransform::Identity,Spawn):nullptr;if(!A){Check(false,TEXT("pawn_character_explicit_parent_spawn"));continue;}TStrongObjectPtr<APawn> Keep(A);auto* CDO=CastChecked<APawn>(Class->GetDefaultObject());Check(A->IsActorTickEnabled()==CDO->PrimaryActorTick.bStartWithTickEnabled,TEXT("pawn_character_native_tick_without_DAS_lifecycle"));int32 Value=0;auto* Function=A->FindFunction(TEXT("Value"));if(Function)A->ProcessEvent(Function,&Value);Check(Function && Value==(I==6?7:9),TEXT("pawn_character_explicit_parent_frame_call"));if(I==6)Check(A->BaseEyeHeight==54.f,TEXT("pawn_short_parent_computed_native_defaults"));else {auto* C=Cast<ACharacter>(A);Check(C && C->GetRootComponent()==C->GetCapsuleComponent(),TEXT("character_explicit_parent_capsule"));}A->Destroy();R.ReleaseObject(A);}
 return Good;
}
