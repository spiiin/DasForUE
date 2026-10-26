#include "DasStaticLibraryProbe.h"
#include "DasRuntime.h"
#include "DasNativeHashProbeComponent.h"
#include "DasScriptHosts.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/TextRenderComponent.h"
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
#include "Internationalization/PolyglotTextData.h"
#include "Internationalization/TextLocalizationManager.h"

bool RunDasPlayableCharacterProbe(ADasStaticLibraryProbe* Carrier, FString& Report)
{
    bool Good = true;
    auto Check = [&](bool V, const TCHAR* N)
    {
        Good &= V;
        Report += FString(V ? TEXT("PASS "): TEXT("FAIL ")) + N + TEXT("\n");
    };
    auto* W = Carrier ? Carrier->GetWorld() : nullptr;
    auto* PC = W ? W->GetFirstPlayerController() : nullptr;
    auto* Input = PC ? Cast<UEnhancedPlayerInput>(PC->PlayerInput) : nullptr;
    auto* LP = PC ? PC->GetLocalPlayer() : nullptr;
    auto* Subsystem = LP ? ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP) : nullptr;
    if (!W || !Carrier->PlayableCharacterClass || !PC || !Input || !Subsystem)
    {
        Check(false, TEXT("playable_local_enhanced_player_ready"));
        return false;
    }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    TWeakObjectPtr<APawn> OldPawn = PC->GetPawn();
    const FRotator OldRotation = PC->GetControlRotation();
    FActorSpawnParameters Spawn;
    Spawn.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    auto* C = W->SpawnActor<ACharacter>(Carrier->PlayableCharacterClass, FTransform(FVector(0, 0, 5000)), Spawn);
    if (!C)
    {
        Check(false, TEXT("playable_spawn"));
        return false;
    }
    TStrongObjectPtr<ACharacter> Keep(C);
    ON_SCOPE_EXIT
    {
        PC->UnPossess();
        if (OldPawn.IsValid())
        {
            PC->Possess(OldPawn.Get());
            OldPawn->PawnClientRestart();
        }
        PC->SetControlRotation(OldRotation);
        C->Destroy();
        Runtime.ReleaseObject(C);
    };
    auto Int = [&](const TCHAR* N)
    {
        auto* P = FindFProperty<FIntProperty>(C->GetClass(), N);
        return P ? P->GetPropertyValue_InContainer(C) : MIN_int32;
    };
    auto Obj = [&](const TCHAR* N) -> UObject*
    {
        auto* P = FindFProperty<FObjectPropertyBase>(C->GetClass(), N);
        return P ? P->GetObjectPropertyValue_InContainer(C) : nullptr;
    };
    auto* Take = C->FindFunction(TEXT("TakeControl"));
    if (Take)
    {
        FStructOnScope Args(Take);
        auto* Player = FindFProperty<FObjectPropertyBase>(Take, TEXT("player"));
        if (Player)
        {
            Player->SetObjectPropertyValue_InContainer(Args.GetStructMemory(), PC);
            C->ProcessEvent(Take, Args.GetStructMemory());
        }
    }
    Check(PC->GetPawn() == C && C->GetController() == PC, TEXT("playable_DAS_authority_possession"));
    if (PC->GetPawn() != C)
        return false;
    C->PawnClientRestart();
    auto* Component = Cast<UEnhancedInputComponent>(C->InputComponent);
    if (!Component)
    {
        Check(false, TEXT("playable_possession_input_component"));
        return false;
    }
    PC->SetControlRotation(FRotator::ZeroRotator);
    C->Tick(0.016f);
    const int32 BoundSubscriptions = Runtime.SubscriptionCount(C);
    // Health.Changed and OnTakeAnyDamage handlers belong to the actor, not to possession.
    constexpr int32 ActorEventSubscriptions = 2;
    auto* Context = Cast<UInputMappingContext>(Obj(TEXT("Controls")));
    auto* Move = Cast<UInputAction>(Obj(TEXT("MoveAction")));
    auto* Look = Cast<UInputAction>(Obj(TEXT("LookAction")));
    auto* Jump = Cast<UInputAction>(Obj(TEXT("JumpAction")));
    Check(Context && Move && Look && Jump && Subsystem->HasMappingContext(Context),
          TEXT("playable_DAS_mapping_context_installed"));
    if (!Context || !Move || !Look || !Jump)
        return false;
    FModifyContextOptions Rebuild;
    Rebuild.bForceImmediately = true;
    Subsystem->RequestRebuildControlMappings(Rebuild, EInputMappingRebuildType::RebuildWithFlush);
    TArray<UInputComponent*> Stack{Component};
    // The real EnhancedPlayerInput evaluates triggers and invokes its managed delegates.
    Input->InjectInputForAction(Move, FInputActionValue(FVector2D(0.25, 1)));
    Input->ProcessInputStack(Stack, 0.016f, false);
    Check(Int(TEXT("MoveEvents")) == 1 && C->GetPendingMovementInputVector().Equals(FVector(1, .25, 0), .001),
              TEXT("playable_EnhancedInput_Axis2D_DAS_movement"));
    C->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
    const auto Start = C->GetActorLocation();
    C->GetCharacterMovement()->TickComponent(.016f, LEVELTICK_All, nullptr);
    Check(C->GetActorLocation().X > Start.X && C->GetVelocity().X > 0,
          TEXT("playable_native_movement_consumes_DAS_input"));
    Input->InputKey(FInputKeyEventArgs(nullptr, FInputDeviceId::CreateFromInternalId(0), EKeys::W, IE_Pressed, 1.f,
                                       false, FPlatformTime::Cycles64()));
    Input->ProcessInputStack(Stack, .016f, false);
    Check(Int(TEXT("MoveEvents")) >= 2 && C->GetPendingMovementInputVector().X > 0,
              TEXT("playable_keyboard_W_mapping_to_DAS"));
    Input->InputKey(FInputKeyEventArgs(nullptr, FInputDeviceId::CreateFromInternalId(0), EKeys::W, IE_Released, 0.f,
                                       false, FPlatformTime::Cycles64()));
    Input->ProcessInputStack(Stack, .016f, false);
    C->ConsumeMovementInputVector();
    Input->InjectInputForAction(Look, FInputActionValue(FVector2D(4, -2)));
    Input->ProcessInputStack(Stack, .016f, false);
    PC->UpdateRotation(.016f);
    Check(Int(TEXT("LookEvents")) == 1 && !PC->GetControlRotation().IsNearlyZero(),
              TEXT("playable_EnhancedInput_DAS_controller_camera_rotation"));
    auto* Camera = C->FindComponentByClass<UCameraComponent>();
    FMinimalViewInfo View;
    C->CalcCamera(.016f, View);
    Check(Camera && Camera->bUsePawnControlRotation && View.Rotation.Equals(PC->GetControlRotation(), .01),
          TEXT("playable_native_camera_uses_control_rotation"));
    C->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    Input->InjectInputForAction(Jump, FInputActionValue(true));
    Input->ProcessInputStack(Stack, .016f, false);
    Check(Int(TEXT("JumpEvents")) == 1 && C->bPressedJump, TEXT("playable_EnhancedInput_started_DAS_jump"));
    C->CheckJumpInput(.016f);
    Check(C->GetCharacterMovement()->Velocity.Z > 0, TEXT("playable_native_jump_velocity"));
    Input->InjectInputForAction(Jump, FInputActionValue(false));
    Input->ProcessInputStack(Stack, .016f, false);
    Check(Int(TEXT("JumpEvents")) == 2 && !C->bPressedJump, TEXT("playable_EnhancedInput_completed_DAS_stop_jump"));
    auto* Speed = C->FindFunction(TEXT("SetSpeed"));
    if (Speed)
    {
        FStructOnScope Args(Speed);
        auto* P = FindFProperty<FFloatProperty>(Speed, TEXT("Input"));
        if (!P)
            P = FindFProperty<FFloatProperty>(Speed, TEXT("value"));
        if (P)
        {
            P->SetPropertyValue_InContainer(Args.GetStructMemory(), 320);
            C->ProcessEvent(Speed, Args.GetStructMemory());
        }
    }
    Check(C->GetCharacterMovement()->MaxWalkSpeed == 320 && C->GetCapsuleComponent() == C->GetRootComponent() &&
              C->GetMesh(),
          TEXT("playable_typed_native_components_and_movement_field"));
    FTextLocalizationManager::Get().RegisterPolyglotTextData(FPolyglotTextData(
        ELocalizedTextSourceCategory::Game, TEXT("DasForUE"), TEXT("NativeProbe"), TEXT("Hello"), TEXT("en")), true);
    const auto NativeAPI =
        Runtime.RunNamedFramePersistent(Runtime.ScriptPath(TEXT("native_gameplay_reference_probe.das")),
                                                           TEXT("main"), {}, EDasMethodSignature::Bool, C);
    Check(
        NativeAPI.bSucceeded && NativeAPI.BoolValue,
        TEXT("native_generated_fields_methods_objects_math_defaults_out_free_function_enum_parent_upcasts_byte_text_struct_array"));
    if (!NativeAPI.bSucceeded)
        Report += NativeAPI.Error + TEXT("\n");
    {
        // Informational: one generated native field write plus read, after a warm-up call.
        const FString Probe = Runtime.ScriptPath(TEXT("native_gameplay_reference_probe.das"));
        const int32 Count = 20000;
        const auto Warm = Runtime.RunNamedPersistent(Probe, TEXT("native_field_bench"), 1, C);
        const double BenchStart = FPlatformTime::Seconds();
        const auto Bench = Runtime.RunNamedPersistent(Probe, TEXT("native_field_bench"), Count, C);
        const double Ns = (FPlatformTime::Seconds() - BenchStart) * 1e9 / Count;
        Check(Warm.bSucceeded && Bench.bSucceeded && Bench.Value == Count, TEXT("native_field_benchmark_runs"));
        Report += FString::Printf(TEXT("INFO native_field_write_read_ns=%.0f %s\n"), Ns, *Bench.Error);
        for (const TCHAR* Entry :
             {TEXT("native_field_read_bench"), TEXT("native_field_write_bench"), TEXT("native_valid_bench"),
                                                                                      TEXT("native_object_field_bench"),
                                                                                           TEXT("native_field_bench")})
        {
            Runtime.RunNamedPersistent(Probe, Entry, 1, C);
            const double EntryStart = FPlatformTime::Seconds();
            const auto R = Runtime.RunNamedPersistent(Probe, Entry, Count, C);
            Report += FString::Printf(TEXT("INFO %s_ns=%.0f ok=%d %s\n"), Entry,
                                           (FPlatformTime::Seconds() - EntryStart) * 1e9 / Count,
                                           R.bSucceeded && R.Value == Count, *R.Error);
        }
    }
    auto* Label = C->FindComponentByClass<UTextRenderComponent>();
    const auto Namespace = Label ? FTextInspector::GetNamespace(Label->Text) : TOptional<FString>();
    const auto TextKey = Label ? FTextInspector::GetKey(Label->Text) : TOptional<FString>();
    Check(Label && Namespace.IsSet() &&
          Namespace.GetValue() == TEXT("DasForUE") && TextKey.IsSet() &&
                                       TextKey.GetValue() ==
                                           TEXT("NativeProbe") && !Label->Text.IsCultureInvariant(),
                                                TEXT("native_FText_preserves_localization_identity_in_Unreal"));
    const auto ByteRejected = Runtime.RunNamedFramePersistent(
        Runtime.ScriptPath(TEXT("native_gameplay_byte_rejection.das")), TEXT("main"), {}, EDasMethodSignature::Bool, C);
    Check(ByteRejected.bSucceeded && ByteRejected.BoolValue,
          TEXT("native_uint8_rejects_negative_and_overflow_before_invoke"));
    if (!ByteRejected.bSucceeded)
        Report += ByteRejected.Error + TEXT("\n");
    auto* HashProbe = NewObject<UDasNativeHashProbeComponent>(C, TEXT("NativeHashProbe"));
    C->AddInstanceComponent(HashProbe);
    HashProbe->RegisterComponent();
    HashProbe->ReadOnlyNames.Add(TEXT("locked"));
    const auto HashAPI = Runtime.RunNamedFramePersistent(
        Runtime.ScriptPath(TEXT("native_gameplay_hash_probe.das")), TEXT("main"), {}, EDasMethodSignature::Bool, C);
    Check(HashAPI.bSucceeded && HashAPI.BoolValue,
          TEXT("native_generated_map_set_fields_arguments_results_out_ref_enum_object_keys_nested"));
    if (!HashAPI.bSucceeded)
        Report += HashAPI.Error + TEXT("\n");
    Check(HashProbe->Counts.FindRef(
        TEXT("one")) == 11 &&
        HashProbe->Positions.FindRef(
            TEXT("point")) == FVector(1, 2, 3) && HashProbe->Weights.FindRef(EDasNativeHashKey::Second) == 7 &&
            HashProbe->ActorLabels.FindRef(C) == TEXT("owner") && HashProbe->Actors.Contains(C) &&
                                                      HashProbe->Modes.Num() == 2 &&
                                                      HashProbe->Nested.Labels.FindRef(7) ==
                                                          TEXT("nested"), TEXT("native_hash_payload_confirmed_in_Cpp"));
    auto* ExpiredHashActor = C->GetWorld()->SpawnActor<AActor>();
    HashProbe->Actors.Reset();
    HashProbe->Actors.Add(ExpiredHashActor);
    HashProbe->ActorLabels.Reset();
    HashProbe->ActorLabels.Add(ExpiredHashActor, TEXT("expired"));
    ExpiredHashActor->Destroy();
    const auto StaleHash = Runtime.RunNamedFramePersistent(
        Runtime.ScriptPath(TEXT("native_gameplay_hash_probe.das")), TEXT("stale"), {}, EDasMethodSignature::Bool, C);
    Check(StaleHash.bSucceeded && StaleHash.BoolValue,
          TEXT("native_hash_destroyed_keys_rejected_and_empty_replacement_allowed"));
    if (!StaleHash.bSucceeded)
        Report += StaleHash.Error + TEXT("\n");
    HashProbe->DestroyComponent();
    const auto WrongReceiver =
        Runtime.RunNamedFramePersistent(Runtime.ScriptPath(TEXT("native_gameplay_wrong_receiver_probe.das")),
                                                           TEXT("main"), {}, EDasMethodSignature::Bool, C);
    Check(!WrongReceiver.bSucceeded && WrongReceiver.Error.Contains(TEXT("receiver class mismatch")),
                                                                    TEXT("native_generated_receiver_class_enforced"));
    const auto Rejection =
        Runtime.RunNamedFramePersistent(Runtime.ScriptPath(TEXT("playable_character_negative_probe.das")),
                                                           TEXT("main"), {}, EDasMethodSignature::Bool, C);
    Check(Rejection.bSucceeded && Rejection.BoolValue && C->GetCharacterMovement()->MaxWalkSpeed == 320,
          TEXT("playable_invalid_speed_rejected_without_write"));
    if (!Rejection.bSucceeded)
        Report += Rejection.Error + TEXT("\n");
    PC->UnPossess();
    C->Tick(.016f);
    Check(!Subsystem->HasMappingContext(Context) && BoundSubscriptions > ActorEventSubscriptions &&
              Runtime.SubscriptionCount(C) == ActorEventSubscriptions,
          TEXT("playable_unpossess_releases_owned_input"));
    PC->Possess(C);
    C->PawnClientRestart();
    C->Tick(.016f);
    Component = Cast<UEnhancedInputComponent>(C->InputComponent);
    Check(Component && Subsystem->HasMappingContext(Context) && Runtime.SubscriptionCount(C) == BoundSubscriptions,
          TEXT("playable_repossess_rebinds_new_input_component"));
    const auto* Error = DasHostString(C, TEXT("LastDasError"));
    Check(Error && Error->IsEmpty(), TEXT("playable_no_DAS_runtime_errors"));
    if (Error && !Error->IsEmpty())
        Report += *Error;
    C->Destroy();
    Runtime.ReleaseObject(C);
    Check(!Subsystem->HasMappingContext(Context), TEXT("playable_EndPlay_removes_owned_context"));
    Check(Runtime.SubscriptionCount(C) == 0 && (!IsValid(Component) || Component->GetActionEventBindings().Num() == 0),
          TEXT("playable_EndPlay_revokes_handlers_or_destroys_component"));
    return Good;
}

bool RunDasPlayableMapProbe(UWorld* World, FString& Report)
{
    bool Good = true;
    auto Check = [&](bool V, const TCHAR* N)
    {
        Good &= V;
        Report += FString(V ? TEXT("PASS "): TEXT("FAIL ")) + N + TEXT("\n");
    };
    auto* PC = World ? World->GetFirstPlayerController() : nullptr;
    auto* C = PC ? Cast<ACharacter>(PC->GetPawn()) : nullptr;
    auto* Input = PC ? Cast<UEnhancedPlayerInput>(PC->PlayerInput) : nullptr;
    auto* Component = C ? Cast<UEnhancedInputComponent>(C->InputComponent) : nullptr;
    auto* Bound = C ? FindFProperty<FBoolProperty>(C->GetClass(), TEXT("ControlsBound")) : nullptr;
    if (!C || !Input || !Component || !Bound)
    {
        Check(false, TEXT("playable_saved_map_auto_possession"));
        return false;
    }
    Check(Bound->GetPropertyValue_InContainer(C), TEXT("playable_saved_map_input_ready"));
    auto* MoveProp = FindFProperty<FObjectPropertyBase>(C->GetClass(), TEXT("MoveAction"));
    auto* JumpProp = FindFProperty<FObjectPropertyBase>(C->GetClass(), TEXT("JumpAction"));
    auto* LookProp = FindFProperty<FObjectPropertyBase>(C->GetClass(), TEXT("LookAction"));
    auto* Move = MoveProp ? Cast<UInputAction>(MoveProp->GetObjectPropertyValue_InContainer(C)) : nullptr;
    auto* Jump = JumpProp ? Cast<UInputAction>(JumpProp->GetObjectPropertyValue_InContainer(C)) : nullptr;
    auto* Look = LookProp ? Cast<UInputAction>(LookProp->GetObjectPropertyValue_InContainer(C)) : nullptr;
    if (!Move || !Jump || !Look)
    {
        Check(false, TEXT("playable_saved_map_input_assets"));
        return false;
    }
    TArray<UInputComponent*> Stack{Component};
    const FVector Start = C->GetActorLocation();
    Input->InjectInputForAction(Move, FInputActionValue(FVector2D(0, 1)));
    Input->ProcessInputStack(Stack, .05f, false);
    C->GetCharacterMovement()->TickComponent(.05f, LEVELTICK_All, nullptr);
    Check(C->GetActorLocation().X > Start.X && C->GetCharacterMovement()->IsMovingOnGround(),
          TEXT("playable_saved_map_moves_on_floor"));
    Input->InjectInputForAction(Jump, FInputActionValue(true));
    Input->ProcessInputStack(Stack, .016f, false);
    C->CheckJumpInput(.016f);
    Check(C->GetCharacterMovement()->Velocity.Z > 0, TEXT("playable_saved_map_jumps"));
    Input->InjectInputForAction(Look, FInputActionValue(FVector2D(3, 1)));
    Input->ProcessInputStack(Stack, .016f, false);
    PC->UpdateRotation(.016f);
    FMinimalViewInfo View;
    C->CalcCamera(.016f, View);
    Check(!View.Rotation.IsNearlyZero() && View.Rotation.Equals(PC->GetControlRotation(), .01),
          TEXT("playable_saved_map_camera"));
    const auto* Error = DasHostString(C, TEXT("LastDasError"));
    Check(Error && Error->IsEmpty(), TEXT("playable_saved_map_no_runtime_error"));
    if (Error && !Error->IsEmpty())
        Report += *Error;
    return Good;
}
