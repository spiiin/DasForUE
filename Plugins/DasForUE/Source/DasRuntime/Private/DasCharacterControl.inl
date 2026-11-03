// Checked, explicit gameplay operations; no global first-player lookup.
template <class T> T* CharacterControlObject(const FDasObjectHandle& H, das::Context* C, bool Mutation = false)
{
    auto* O = Cast<T>(ResolveObject(H, C));
    auto* Self = ResolveWorldContextObject(C);
    auto* W = Self->GetWorld();
    auto* Actor = Cast<AActor>(static_cast<UObject*>(O));
    auto* Component = Cast<UActorComponent>(static_cast<UObject*>(O));
    if (Component)
        Actor = Component->GetOwner();
    if (!O || O->IsTemplate() || !W || !W->IsGameWorld() || W->bIsTearingDown || O->GetWorld() != W ||
        (Actor && Actor->IsActorBeingDestroyed()) || (Component && Component->IsBeingDestroyed()))
        C->throw_error("character control requires live object in script owner's game world");
    if (Mutation && (ConstructingObject || ActiveContext->bEnding || ActiveContext->bCollectingSubscriptions))
        C->throw_error("character control is forbidden during construction/bind_events/EndPlay");
    return O;
}
FDasObjectHandle CharacterController(const FDasObjectHandle& H, das::Context* C)
{
    auto* P = CharacterControlObject<APawn>(H, C);
    return MakeObjectHandle(Cast<APlayerController>(P->GetController()), APlayerController::StaticClass(), C);
}
bool CharacterControlsReady(const FDasObjectHandle& H, das::Context* C)
{
    auto* P = CharacterControlObject<APawn>(H, C);
    auto* PC = Cast<APlayerController>(P->GetController());
    return PC && PC->IsLocalController() && PC->GetLocalPlayer() && Cast<UEnhancedPlayerInput>(PC->PlayerInput) &&
           Cast<UEnhancedInputComponent>(P->InputComponent);
}
bool CharacterPossess(const FDasObjectHandle& Controller, const FDasObjectHandle& Pawn, das::Context* C)
{
    auto* PC = CharacterControlObject<APlayerController>(Controller, C, true);
    auto* P = CharacterControlObject<APawn>(Pawn, C, true);
    if (!PC->HasAuthority())
        C->throw_error("possession requires authority");
    PC->Possess(P);
    return PC->GetPawn() == P;
}
void CharacterMove(const FDasObjectHandle& H, float Forward, float Right, das::Context* C)
{
    auto* P = CharacterControlObject<ACharacter>(H, C, true);
    if (!FMath::IsFinite(Forward) || !FMath::IsFinite(Right))
        C->throw_error("movement input must be finite");
    if (!P->GetController() || !P->IsLocallyControlled())
        C->throw_error("movement input requires locally controlled Character");
    const FRotationMatrix Basis(FRotator(0, P->GetControlRotation().Yaw, 0));
    P->AddMovementInput(Basis.GetUnitAxis(EAxis::X), Forward);
    P->AddMovementInput(Basis.GetUnitAxis(EAxis::Y), Right);
}
void CharacterLook(const FDasObjectHandle& H, float Yaw, float Pitch, das::Context* C)
{
    auto* P = CharacterControlObject<ACharacter>(H, C, true);
    if (!FMath::IsFinite(Yaw) || !FMath::IsFinite(Pitch))
        C->throw_error("look input must be finite");
    if (!P->IsLocallyControlled() || !Cast<APlayerController>(P->GetController()))
        C->throw_error("look input requires local PlayerController");
    P->AddControllerYawInput(Yaw);
    P->AddControllerPitchInput(Pitch);
}
void CharacterJump(const FDasObjectHandle& H, bool Held, das::Context* C)
{
    auto* P = CharacterControlObject<ACharacter>(H, C, true);
    if (!P->GetController() || !P->IsLocallyControlled())
        C->throw_error("jump input requires locally controlled Character");
    if (Held)
        P->Jump();
    else
        P->StopJumping();
}

bool PlayerControlsReady(const FDasObjectHandle& H, das::Context* C)
{
    auto* PC = CharacterControlObject<APlayerController>(H, C);
    return PC->IsLocalController() && PC->GetLocalPlayer() &&
           PC->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() &&
           Cast<UEnhancedPlayerInput>(PC->PlayerInput) && Cast<UEnhancedInputComponent>(PC->InputComponent);
}
void ControllerSetTeam(const FDasObjectHandle& H, int32 TeamId, das::Context* C)
{
    auto* PC = CharacterControlObject<AController>(H, C, true);
    auto* Agent = Cast<IGenericTeamAgentInterface>(PC);
    if (!Agent || TeamId < 0 || TeamId > 255)
        C->throw_error("controller team requires IGenericTeamAgentInterface and a byte team id");
    Agent->SetGenericTeamId(FGenericTeamId(static_cast<uint8>(TeamId)));
}

// Bind the swept component and initialize velocity after SCS registration.
void ProjectileMovementLaunch(const FDasObjectHandle& Movement, const FDasObjectHandle& Updated,
                              const FVector& LocalVelocity, float GravityScale, das::Context* C)
{
    auto* M = CharacterControlObject<UProjectileMovementComponent>(Movement, C, true);
    auto* U = CharacterControlObject<USceneComponent>(Updated, C, true);
    if (!M->GetOwner() || M->GetOwner() != U->GetOwner() || LocalVelocity.ContainsNaN() ||
        !FMath::IsFinite(GravityScale) ||
        (!FMath::IsFinite(LocalVelocity.Size()) || LocalVelocity.Size() > TNumericLimits<float>::Max()))
        C->throw_error("projectile launch requires same-owner components and finite velocity/gravity");
    M->SetUpdatedComponent(U);
    M->ProjectileGravityScale = GravityScale;
    M->InitialSpeed = LocalVelocity.Size();
    M->SetVelocityInLocalSpace(LocalVelocity);
    M->UpdateComponentVelocity();
    M->Activate(true);
}
