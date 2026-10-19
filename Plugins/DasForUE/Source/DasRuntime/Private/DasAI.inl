AAIController* OwnedAI(const FDasObjectHandle& H, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    auto* W = GameplayWorld(C);
    auto* AI = Cast<AAIController>(ResolveObject(H, C));
    auto* O = Cast<AActor>(S->Owner.Get());
    if (auto* Component = Cast<UActorComponent>(S->Owner.Get()))
        O = Component->GetOwner();
    if (!AI || AI->IsActorBeingDestroyed() || AI->GetWorld() != W || !AI->HasAuthority() || !IsValid(AI->GetPawn()) ||
        AI->GetPawn()->IsActorBeingDestroyed() || (O != AI && O != AI->GetPawn()) || !AI->GetPathFollowingComponent())
        C->throw_error("AI movement requires authority and the live controller of the script owner pawn");
    return AI;
}
FDasObjectHandle ScriptAISelf(das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    GameplayWorld(C);
    auto* O = Cast<AActor>(S->Owner.Get());
    if (auto* Component = Cast<UActorComponent>(S->Owner.Get()))
        O = Component->GetOwner();
    auto* AI = Cast<AAIController>(O);
    if (auto* Pawn = Cast<APawn>(O))
        AI = Cast<AAIController>(Pawn->GetController());
    return MakeObjectHandle(AI, AAIController::StaticClass(), C);
}
bool ScriptAIMove(const char* Name, const FDasObjectHandle& Controller, const FDasObjectHandle& Target,
                  const FVector& Location, bool ActorGoal, const char* Handler, float Radius, bool Pathfinding,
                  bool Partial, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    auto* AI = OwnedAI(Controller, C);
    const FString Key = TimerName(Name, C), Entry = Handler ? UTF8_TO_TCHAR(Handler) : TEXT("");
    if (!FMath::IsFinite(Radius) || Radius < 0.f ||
        (!ActorGoal && (Location.ContainsNaN() || !FAISystem::IsValidLocation(Location))))
        C->throw_error("MoveTo requires a finite goal and nonnegative acceptance radius");
    auto* Goal = ActorGoal ? Cast<AActor>(ResolveObject(Target, C)) : nullptr;
    if (ActorGoal && (!IsValid(Goal) || Goal->IsActorBeingDestroyed() || Goal->GetWorld() != AI->GetWorld()))
        C->throw_error("MoveTo actor must be live and in the same world");
    FDasCallContract Contract = FDasCallContract::Scalar(EDasMethodSignature::Void);
    Contract.Inputs.Add(EDasMethodSignature::Struct);
    Contract.InputClasses.Add(FDasMoveResult::StaticStruct()->GetPathName());
    if (!Entry.IsEmpty() && !VerifyContract(S->Live->Program, Entry, Contract))
        C->throw_error("MoveTo callback must be exported void(result:MoveResult)");
    // A controller follows one request. Replacement revokes the previous owned callback.
    S->Bindings.RemoveAll(
        [&](const auto& B)
        { return B->AIMoveProxy.IsValid() && (B->AIMoveName == Key || B->AIMoveProxy->Controller.Get() == AI); });
    auto B = MakeUnique<FManagedSubscription>();
    B->Id = NextSubscriptionId++;
    B->Handler = Entry;
    B->Kind = EDasMethodSignature::Struct;
    B->PayloadPath = FDasMoveResult::StaticStruct()->GetPathName();
    B->AIMoveName = Key;
    B->AIMoveProxy.Reset(NewObject<UDasAIMoveProxy>());
    auto* P = B->AIMoveProxy.Get();
    P->SubscriptionId = B->Id;
    P->Owner = S->Owner;
    P->Controller = AI;
    P->Following = AI->GetPathFollowingComponent();
    B->Source = P;
    S->Bindings.Add(MoveTemp(B));
    if (!Entry.IsEmpty())
        S->RequiredEntries.Add(Entry, Contract);
    FAIMoveRequest Request;
    if (ActorGoal)
        Request.SetGoalActor(Goal);
    else
        Request.SetGoalLocation(Location);
    Request.SetAcceptanceRadius(Radius);
    Request.SetUsePathfinding(Pathfinding);
    Request.SetProjectGoalLocation(Pathfinding);
    Request.SetAllowPartialPath(Partial);
    Request.SetReachTestIncludesAgentRadius(true);
    Request.SetReachTestIncludesGoalRadius(false);
    const auto Result = AI->MoveTo(Request);
    P->RequestId = Result.MoveId;
    if (Result.Code == EPathFollowingRequestResult::RequestSuccessful)
        P->Attach();
    else
        P->Queue(Result.Code == EPathFollowingRequestResult::AlreadyAtGoal ? EDasMoveCode::Success
                                                                           : EDasMoveCode::Invalid);
    return Result.Code != EPathFollowingRequestResult::Failed;
}
bool ScriptAICancel(const char* Name, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    const FString Key = TimerName(Name, C);
    return S->Bindings.RemoveAll([&](const auto& B) { return B->AIMoveProxy.IsValid() && B->AIMoveName == Key; }) > 0;
}
bool ScriptAIActive(const char* Name, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    const FString Key = TimerName(Name, C);
    for (const auto& B : S->Bindings)
        if (B->AIMoveProxy.IsValid() && B->AIMoveName == Key)
            return B->AIMoveProxy->IsLive() && !B->AIMoveProxy->Finished;
    return false;
}
