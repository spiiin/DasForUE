// Generic native AI operations; target selection and behavior remain in scripts.
FDasObjectHandle AIPawn(const FDasObjectHandle& Controller, das::Context* C)
{
    auto* AI = CharacterControlObject<AAIController>(Controller, C);
    return MakeObjectHandle(AI->GetPawn(), APawn::StaticClass(), C);
}
void AIFocus(const FDasObjectHandle& Controller, const FDasObjectHandle& Target, das::Context* C)
{
    auto* AI = CharacterControlObject<AAIController>(Controller, C, true);
    if (!AI->HasAuthority())
        C->throw_error("AI focus requires authority");
    if (!Target.Token)
    {
        AI->ClearFocus(EAIFocusPriority::Gameplay);
        return;
    }
    auto* Actor = Cast<AActor>(ResolveObject(Target, C));
    if (!Actor || Actor->GetWorld() != AI->GetWorld() || Actor->IsActorBeingDestroyed())
        C->throw_error("AI focus target must be live in the same world");
    AI->SetFocus(Actor, EAIFocusPriority::Gameplay);
}
bool AILineOfSight(const FDasObjectHandle& Controller, const FDasObjectHandle& Target, das::Context* C)
{
    auto* AI = CharacterControlObject<AAIController>(Controller, C);
    if (!Target.Token)
        return false;
    auto* Actor = Cast<AActor>(ResolveObject(Target, C));
    if (!Actor || Actor->GetWorld() != AI->GetWorld() || Actor->IsActorBeingDestroyed())
        return false;
    return AI->LineOfSightTo(Actor);
}
void PerceptionConfigure(const FDasObjectHandle& Component, float SightRadius, float LoseSightRadius, float SightAngle,
                         float HearingRadius, float MaxAge, bool DetectEnemies, bool DetectFriendlies,
                         bool DetectNeutrals, das::Context* C)
{
    auto* P = CharacterControlObject<UAIPerceptionComponent>(Component, C, true);
    if (!P->GetOwner() || !P->GetOwner()->HasAuthority() || !FMath::IsFinite(SightRadius) ||
        !FMath::IsFinite(LoseSightRadius) || !FMath::IsFinite(SightAngle) || !FMath::IsFinite(HearingRadius) ||
        !FMath::IsFinite(MaxAge) || SightRadius < 0 || LoseSightRadius < SightRadius || SightAngle < 0 ||
        SightAngle > 180 || HearingRadius < 0 || MaxAge < 0)
        C->throw_error(
            "perception config requires authority, finite ranges, lose-sight >= sight, angle 0..180 and nonnegative age");
    auto* Sight = Cast<UAISenseConfig_Sight>(P->GetSenseConfig(UAISense::GetSenseID<UAISense_Sight>()));
    if (!Sight)
        Sight = NewObject<UAISenseConfig_Sight>(P);
    Sight->SightRadius = SightRadius;
    Sight->LoseSightRadius = LoseSightRadius;
    Sight->PeripheralVisionAngleDegrees = SightAngle;
    Sight->DetectionByAffiliation.bDetectEnemies = DetectEnemies;
    Sight->DetectionByAffiliation.bDetectFriendlies = DetectFriendlies;
    Sight->DetectionByAffiliation.bDetectNeutrals = DetectNeutrals;
    Sight->SetMaxAge(MaxAge);
    P->ConfigureSense(*Sight);
    auto* Hearing = Cast<UAISenseConfig_Hearing>(P->GetSenseConfig(UAISense::GetSenseID<UAISense_Hearing>()));
    if (!Hearing)
        Hearing = NewObject<UAISenseConfig_Hearing>(P);
    Hearing->HearingRange = HearingRadius;
    Hearing->DetectionByAffiliation.bDetectEnemies = DetectEnemies;
    Hearing->DetectionByAffiliation.bDetectFriendlies = DetectFriendlies;
    Hearing->DetectionByAffiliation.bDetectNeutrals = DetectNeutrals;
    Hearing->SetMaxAge(MaxAge);
    P->ConfigureSense(*Hearing);
    P->SetDominantSense(UAISense_Sight::StaticClass());
    P->RequestStimuliListenerUpdate();
}

void PerceptionConfigureDamage(const FDasObjectHandle& Component, float MaxAge, das::Context* C)
{
    auto* P = CharacterControlObject<UAIPerceptionComponent>(Component, C, true);
    if (!P->GetOwner() || !P->GetOwner()->HasAuthority() || !FMath::IsFinite(MaxAge) || MaxAge < 0)
        C->throw_error("damage perception config requires authority and finite nonnegative age");
    auto* Damage = Cast<UAISenseConfig_Damage>(P->GetSenseConfig(UAISense::GetSenseID<UAISense_Damage>()));
    if (!Damage)
        Damage = NewObject<UAISenseConfig_Damage>(P);
    Damage->SetMaxAge(MaxAge);
    P->ConfigureSense(*Damage);
    P->RequestStimuliListenerUpdate();
}
void PerceptionReportDamage(const FDasObjectHandle& Damaged, const FDasObjectHandle& Instigator, float Amount,
                            const FVector& EventLocation, const FVector& HitLocation, das::Context* C)
{
    auto* Victim = CharacterControlObject<AActor>(Damaged, C, true);
    auto* Source = CharacterControlObject<AActor>(Instigator, C);
    if (!Victim->HasAuthority() || !FMath::IsFinite(Amount) || Amount <= 0 || EventLocation.ContainsNaN() ||
        HitLocation.ContainsNaN())
        C->throw_error("damage perception requires authority, positive finite damage and finite locations");
    UAISense_Damage::ReportDamageEvent(Victim, Victim, Source, Amount, EventLocation, HitLocation);
}
bool ControllerIsFriendly(const FDasObjectHandle& Controller, const FDasObjectHandle& Other, das::Context* C)
{
    auto* Observer = CharacterControlObject<AController>(Controller, C);
    auto* Target = CharacterControlObject<AActor>(Other, C);
    auto* Agent = Cast<IGenericTeamAgentInterface>(Observer);
    return Agent && Agent->GetTeamAttitudeTowards(*Target) == ETeamAttitude::Friendly;
}
void PawnMoveIgnoreActor(const FDasObjectHandle& Pawn, const FDasObjectHandle& Other, bool Ignore, das::Context* C)
{
    auto* P = CharacterControlObject<APawn>(Pawn, C, true);
    auto* Actor = CharacterControlObject<AActor>(Other, C);
    if (Ignore)
        P->MoveIgnoreActorAdd(Actor);
    else
        P->MoveIgnoreActorRemove(Actor);
}

bool AIBrainPause(const FDasObjectHandle& Controller, bool Paused, const char* Reason, das::Context* C)
{
    auto* AI = CharacterControlObject<AAIController>(Controller, C, true);
    if (!AI->HasAuthority())
        C->throw_error("AI brain pause requires authority");
    auto* Brain = AI->GetBrainComponent();
    if (!Brain)
        return false;
    const FString Why = UTF8_TO_TCHAR(Reason ? Reason : "");
    if (Paused)
        Brain->PauseLogic(Why);
    else
        Brain->ResumeLogic(Why);
    return Brain->IsPaused() == Paused;
}

// Blueprint wrappers expose completion and results, but not cancellation.
bool EnvironmentQueryAbort(const FDasObjectHandle& Query, das::Context* C)
{
    auto* Wrapper = CharacterControlObject<UEnvQueryInstanceBlueprintWrapper>(Query, C);
    auto* Manager = UEnvQueryManager::GetCurrent(Wrapper->GetWorld());
    const auto* Instance = Wrapper->GetQueryInstance();
    return Manager && Instance && Manager->AbortQuery(Instance->QueryID);
}
