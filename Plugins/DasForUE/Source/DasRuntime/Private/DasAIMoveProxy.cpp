#include "DasAIMoveProxy.h"
#include "DasRuntime.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Pawn.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "Containers/Ticker.h"
#include "Modules/ModuleManager.h"
bool UDasAIMoveProxy::IsLive() const
{
    auto* O = Owner.Get();
    auto* C = Controller.Get();
    auto* W = IsValid(O) ? O->GetWorld() : nullptr;
    if (!Active || !W || !W->IsGameWorld() || W->bIsTearingDown || !IsValid(C) || C->IsActorBeingDestroyed() ||
        C->GetWorld() != W || !IsValid(C->GetPawn()) || C->GetPawn()->IsActorBeingDestroyed())
        return false;
    auto* A = Cast<AActor>(O);
    if (auto* Component = Cast<UActorComponent>(O))
    {
        if (Component->IsBeingDestroyed())
            return false;
        A = Component->GetOwner();
    }
    return IsValid(A) && !A->IsActorBeingDestroyed() && (A == C || A == C->GetPawn());
}
void UDasAIMoveProxy::Attach()
{
    if (auto* P = Following.Get())
        Completion = P->OnRequestFinished.AddUObject(this, &UDasAIMoveProxy::Finish);
}
void UDasAIMoveProxy::Deactivate()
{
    if (!Active)
        return;
    Active = false;
    if (auto* P = Following.Get())
    {
        P->OnRequestFinished.Remove(Completion);
        Completion.Reset();
        if (RequestId.IsValid() && P->GetCurrentRequestId() == RequestId &&
            P->GetStatus() != EPathFollowingStatus::Idle)
            P->AbortMove(*this, FPathFollowingResultFlags::OwnerFinished, RequestId);
    }
}
void UDasAIMoveProxy::BeginDestroy()
{
    Deactivate();
    Super::BeginDestroy();
}
void UDasAIMoveProxy::Finish(FAIRequestID Id, const FPathFollowingResult& Result)
{
    if (Id != RequestId || Finished)
        return;
    EDasMoveCode Code = EDasMoveCode::Invalid;
    switch (Result.Code)
    {
    case EPathFollowingResult::Success:
        Code = EDasMoveCode::Success;
        break;
    case EPathFollowingResult::Blocked:
        Code = EDasMoveCode::Blocked;
        break;
    case EPathFollowingResult::OffPath:
        Code = EDasMoveCode::OffPath;
        break;
    case EPathFollowingResult::Aborted:
        Code = EDasMoveCode::Aborted;
        break;
    default:
        break;
    }
    Queue(Code);
}
void UDasAIMoveProxy::Queue(EDasMoveCode Code)
{
    if (Finished || !IsLive())
        return;
    Finished = true;
    if (auto* P = Following.Get())
    {
        P->OnRequestFinished.Remove(Completion);
        Completion.Reset();
    }
    const TWeakObjectPtr<UDasAIMoveProxy> Weak(this);
    const int32 Id = int32(uint32(RequestId));
    FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateLambda(
            [Weak, Code, Id](float)
            {
                auto* P = Weak.Get();
                if (!P || !P->IsLive())
                    return false;
                if (auto* R = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
                {
                    FDasScalarValue V;
                    V.Kind = int32(EDasMethodSignature::Struct);
                    V.ObjectClass = FDasMoveResult::StaticStruct()->GetPathName();
                    FDasMoveResult E;
                    E.Code = Code;
                    E.RequestId = Id;
                    V.StructValue = FInstancedStruct::Make<FDasMoveResult>(E);
                    R->DispatchScalarEvent(P->SubscriptionId, MoveTemp(V));
                }
                return false;
            }),
        0.f);
}
