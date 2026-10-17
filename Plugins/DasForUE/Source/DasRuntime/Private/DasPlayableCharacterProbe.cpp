#include "DasStaticLibraryProbe.h"
#include "DasRuntime.h"
#include "DasScriptHosts.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedPlayerInput.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputKeyEventArgs.h"
#include "InputCoreTypes.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "UObject/StructOnScope.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "Modules/ModuleManager.h"
#include "Misc/ScopeExit.h"

bool RunDasPlayableCharacterProbe(ADasStaticLibraryProbe* Carrier,FString& Report){
 bool Good=true;auto Check=[&](bool V,const TCHAR* N){Good &= V;Report+=FString(V?TEXT("PASS "):TEXT("FAIL "))+N+TEXT("\n");};
 auto* W=Carrier?Carrier->GetWorld():nullptr;auto* PC=W?W->GetFirstPlayerController():nullptr;
 auto* Input=PC?Cast<UEnhancedPlayerInput>(PC->PlayerInput):nullptr;auto* LP=PC?PC->GetLocalPlayer():nullptr;
 auto* Subsystem=LP?ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP):nullptr;
 if(!W || !Carrier->PlayableCharacterClass || !PC || !Input || !Subsystem){Check(false,TEXT("playable_local_enhanced_player_ready"));return false;}
 auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
 TWeakObjectPtr<APawn> OldPawn=PC->GetPawn();const FRotator OldRotation=PC->GetControlRotation();
 FActorSpawnParameters Spawn;Spawn.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
 auto* C=W->SpawnActor<ACharacter>(Carrier->PlayableCharacterClass,FTransform(FVector(0,0,5000)),Spawn);
 if(!C){Check(false,TEXT("playable_spawn"));return false;}TStrongObjectPtr<ACharacter> Keep(C);
 ON_SCOPE_EXIT { PC->UnPossess();if(OldPawn.IsValid()){PC->Possess(OldPawn.Get());OldPawn->PawnClientRestart();}PC->SetControlRotation(OldRotation);C->Destroy();Runtime.ReleaseObject(C); };
 auto Int=[&](const TCHAR* N){auto* P=FindFProperty<FIntProperty>(C->GetClass(),N);return P?P->GetPropertyValue_InContainer(C):MIN_int32;};
 auto Obj=[&](const TCHAR* N)->UObject*{auto* P=FindFProperty<FObjectPropertyBase>(C->GetClass(),N);return P?P->GetObjectPropertyValue_InContainer(C):nullptr;};
 auto* Take=C->FindFunction(TEXT("TakeControl"));if(Take){FStructOnScope Args(Take);auto* Player=FindFProperty<FObjectPropertyBase>(Take,TEXT("player"));if(Player){Player->SetObjectPropertyValue_InContainer(Args.GetStructMemory(),PC);C->ProcessEvent(Take,Args.GetStructMemory());}}
 Check(PC->GetPawn()==C && C->GetController()==PC,TEXT("playable_DAS_authority_possession"));if(PC->GetPawn()!=C)return false;
 C->PawnClientRestart();auto* Component=Cast<UEnhancedInputComponent>(C->InputComponent);
 if(!Component){Check(false,TEXT("playable_possession_input_component"));return false;}
 PC->SetControlRotation(FRotator::ZeroRotator);C->Tick(0.016f);
 auto* Context=Cast<UInputMappingContext>(Obj(TEXT("Controls")));auto* Move=Cast<UInputAction>(Obj(TEXT("MoveAction")));auto* Look=Cast<UInputAction>(Obj(TEXT("LookAction")));auto* Jump=Cast<UInputAction>(Obj(TEXT("JumpAction")));
 Check(Context && Move && Look && Jump && Subsystem->HasMappingContext(Context),TEXT("playable_DAS_mapping_context_installed"));if(!Context || !Move || !Look || !Jump)return false;
 FModifyContextOptions Rebuild;Rebuild.bForceImmediately=true;Subsystem->RequestRebuildControlMappings(Rebuild,EInputMappingRebuildType::RebuildWithFlush);
 TArray<UInputComponent*> Stack{Component};
 // The real EnhancedPlayerInput evaluates triggers and invokes its managed delegates.
 Input->InjectInputForAction(Move,FInputActionValue(FVector2D(0.25,1)));Input->ProcessInputStack(Stack,0.016f,false);
 Check(Int(TEXT("MoveEvents"))==1 && C->GetPendingMovementInputVector().Equals(FVector(1,.25,0),.001),TEXT("playable_EnhancedInput_Axis2D_DAS_movement"));
 C->GetCharacterMovement()->SetMovementMode(MOVE_Falling);const auto Start=C->GetActorLocation();C->GetCharacterMovement()->TickComponent(.016f,LEVELTICK_All,nullptr);
 Check(C->GetActorLocation().X>Start.X && C->GetVelocity().X>0,TEXT("playable_native_movement_consumes_DAS_input"));
 Input->InputKey(FInputKeyEventArgs(nullptr,FInputDeviceId::CreateFromInternalId(0),EKeys::W,IE_Pressed,1.f,false,FPlatformTime::Cycles64()));Input->ProcessInputStack(Stack,.016f,false);
 Check(Int(TEXT("MoveEvents"))>=2 && C->GetPendingMovementInputVector().X>0,TEXT("playable_keyboard_W_mapping_to_DAS"));
 Input->InputKey(FInputKeyEventArgs(nullptr,FInputDeviceId::CreateFromInternalId(0),EKeys::W,IE_Released,0.f,false,FPlatformTime::Cycles64()));Input->ProcessInputStack(Stack,.016f,false);C->ConsumeMovementInputVector();
 Input->InjectInputForAction(Look,FInputActionValue(FVector2D(4,-2)));Input->ProcessInputStack(Stack,.016f,false);PC->UpdateRotation(.016f);
 Check(Int(TEXT("LookEvents"))==1 && !PC->GetControlRotation().IsNearlyZero(),TEXT("playable_EnhancedInput_DAS_controller_camera_rotation"));
 auto* Camera=C->FindComponentByClass<UCameraComponent>();FMinimalViewInfo View;C->CalcCamera(.016f,View);
 Check(Camera && Camera->bUsePawnControlRotation && View.Rotation.Equals(PC->GetControlRotation(),.01),TEXT("playable_native_camera_uses_control_rotation"));
 C->GetCharacterMovement()->SetMovementMode(MOVE_Walking);Input->InjectInputForAction(Jump,FInputActionValue(true));Input->ProcessInputStack(Stack,.016f,false);
 Check(Int(TEXT("JumpEvents"))==1 && C->bPressedJump,TEXT("playable_EnhancedInput_started_DAS_jump"));C->CheckJumpInput(.016f);
 Check(C->GetCharacterMovement()->Velocity.Z>0,TEXT("playable_native_jump_velocity"));
 Input->InjectInputForAction(Jump,FInputActionValue(false));Input->ProcessInputStack(Stack,.016f,false);
 Check(Int(TEXT("JumpEvents"))==2 && !C->bPressedJump,TEXT("playable_EnhancedInput_completed_DAS_stop_jump"));
 auto* Speed=C->FindFunction(TEXT("SetSpeed"));if(Speed){FStructOnScope Args(Speed);auto* P=FindFProperty<FFloatProperty>(Speed,TEXT("Input"));if(!P)P=FindFProperty<FFloatProperty>(Speed,TEXT("value"));if(P){P->SetPropertyValue_InContainer(Args.GetStructMemory(),320);C->ProcessEvent(Speed,Args.GetStructMemory());}}
 Check(C->GetCharacterMovement()->MaxWalkSpeed==320 && C->GetCapsuleComponent()==C->GetRootComponent() && C->GetMesh(),TEXT("playable_typed_native_components_and_movement_field"));
 const auto Rejection=Runtime.RunNamedFramePersistent(Runtime.ScriptPath(TEXT("playable_character_negative_probe.das")),TEXT("main"),{},EDasMethodSignature::Bool,C);Check(Rejection.bSucceeded && Rejection.BoolValue && C->GetCharacterMovement()->MaxWalkSpeed==320,TEXT("playable_invalid_speed_rejected_without_write"));if(!Rejection.bSucceeded)Report+=Rejection.Error+TEXT("\n");
 PC->UnPossess();C->Tick(.016f);Check(!Subsystem->HasMappingContext(Context) && Runtime.SubscriptionCount(C)==0,TEXT("playable_unpossess_releases_owned_input"));
 PC->Possess(C);C->PawnClientRestart();C->Tick(.016f);Component=Cast<UEnhancedInputComponent>(C->InputComponent);Check(Component && Subsystem->HasMappingContext(Context) && Runtime.SubscriptionCount(C)==5,TEXT("playable_repossess_rebinds_new_input_component"));
 const auto* Error=DasHostString(C,TEXT("LastDasError"));Check(Error && Error->IsEmpty(),TEXT("playable_no_DAS_runtime_errors"));if(Error && !Error->IsEmpty())Report+=*Error;
 C->Destroy();Runtime.ReleaseObject(C);Check(!Subsystem->HasMappingContext(Context),TEXT("playable_EndPlay_removes_owned_context"));Check(Runtime.SubscriptionCount(C)==0 && (!IsValid(Component) || Component->GetActionEventBindings().Num()==0),TEXT("playable_EndPlay_revokes_handlers_or_destroys_component"));
 return Good;
}

bool RunDasPlayableMapProbe(UWorld* World,FString& Report){
 bool Good=true;auto Check=[&](bool V,const TCHAR* N){Good &= V;Report+=FString(V?TEXT("PASS "):TEXT("FAIL "))+N+TEXT("\n");};
 auto* PC=World?World->GetFirstPlayerController():nullptr;auto* C=PC?Cast<ACharacter>(PC->GetPawn()):nullptr;auto* Input=PC?Cast<UEnhancedPlayerInput>(PC->PlayerInput):nullptr;auto* Component=C?Cast<UEnhancedInputComponent>(C->InputComponent):nullptr;
 auto* Bound=C?FindFProperty<FBoolProperty>(C->GetClass(),TEXT("ControlsBound")):nullptr;
 if(!C || !Input || !Component || !Bound){Check(false,TEXT("playable_saved_map_auto_possession"));return false;}
 Check(Bound->GetPropertyValue_InContainer(C),TEXT("playable_saved_map_input_ready"));
 auto* MoveProp=FindFProperty<FObjectPropertyBase>(C->GetClass(),TEXT("MoveAction"));auto* JumpProp=FindFProperty<FObjectPropertyBase>(C->GetClass(),TEXT("JumpAction"));auto* LookProp=FindFProperty<FObjectPropertyBase>(C->GetClass(),TEXT("LookAction"));
 auto* Move=MoveProp?Cast<UInputAction>(MoveProp->GetObjectPropertyValue_InContainer(C)):nullptr;auto* Jump=JumpProp?Cast<UInputAction>(JumpProp->GetObjectPropertyValue_InContainer(C)):nullptr;auto* Look=LookProp?Cast<UInputAction>(LookProp->GetObjectPropertyValue_InContainer(C)):nullptr;
 if(!Move || !Jump || !Look){Check(false,TEXT("playable_saved_map_input_assets"));return false;}
 TArray<UInputComponent*> Stack{Component};const FVector Start=C->GetActorLocation();
 Input->InjectInputForAction(Move,FInputActionValue(FVector2D(0,1)));Input->ProcessInputStack(Stack,.05f,false);C->GetCharacterMovement()->TickComponent(.05f,LEVELTICK_All,nullptr);
 Check(C->GetActorLocation().X>Start.X && C->GetCharacterMovement()->IsMovingOnGround(),TEXT("playable_saved_map_moves_on_floor"));
 Input->InjectInputForAction(Jump,FInputActionValue(true));Input->ProcessInputStack(Stack,.016f,false);C->CheckJumpInput(.016f);Check(C->GetCharacterMovement()->Velocity.Z>0,TEXT("playable_saved_map_jumps"));
 Input->InjectInputForAction(Look,FInputActionValue(FVector2D(3,1)));Input->ProcessInputStack(Stack,.016f,false);PC->UpdateRotation(.016f);FMinimalViewInfo View;C->CalcCamera(.016f,View);Check(!View.Rotation.IsNearlyZero() && View.Rotation.Equals(PC->GetControlRotation(),.01),TEXT("playable_saved_map_camera"));
 const auto* Error=DasHostString(C,TEXT("LastDasError"));Check(Error && Error->IsEmpty(),TEXT("playable_saved_map_no_runtime_error"));if(Error && !Error->IsEmpty())Report+=*Error;return Good;
}
