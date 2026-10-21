#include "DasGASAuthoring.h"
#include "AbilitySystemGlobals.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Actor.h"

bool UDasAuthoredAbilitySystemComponent::AcceptTarget(const FDasGASTargetingQuery& Q, AActor* Actor) const
{
    auto* Class = Q.ActorClassPath.IsEmpty() ? nullptr : FindObject<UClass>(nullptr, *Q.ActorClassPath);
    if ((!Q.ActorClassPath.IsEmpty() && !IsValid(Class)) || !IsValid(Actor) || Actor->IsActorBeingDestroyed() ||
        Actor->GetWorld() != GetWorld() ||
        (Q.IgnoreSelf && (Actor == GetOwnerActor() || Actor == GetAvatarActor() || Actor == GetOwner())) ||
        Q.IgnoredActors.Contains(Actor) || (Class && !Actor->IsA(Class)))
        return false;
    auto* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, true);
    if (Q.RequireAbilitySystem && !IsValid(ASC))
        return false;
    if ((!Q.RequiredTags.IsEmpty() || !Q.BlockedTags.IsEmpty()) && !IsValid(ASC))
        return false;
    for (const auto Name : Q.RequiredTags)
        if (!ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(Name, false)))
            return false;
    for (const auto Name : Q.BlockedTags)
        if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(Name, false)))
            return false;
    return true;
}
bool UDasAuthoredAbilitySystemComponent::SelectTargets(const FDasGASTargetingQuery& Q, FTargetSelection& Out) const
{
    auto* Class = Q.ActorClassPath.IsEmpty() ? nullptr : FindObject<UClass>(nullptr, *Q.ActorClassPath);
    if (!Live() || !GetWorld() || !GetWorld()->GetPhysicsScene() ||
        (Q.Mode != TEXT("Trace") && Q.Mode != TEXT("Overlap")) || Q.Start.ContainsNaN() || Q.End.ContainsNaN() ||
         !FMath::IsFinite(Q.Radius) || Q.Radius < 0 || Q.Radius > 1000000.f ||
         (Q.Mode == TEXT("Overlap") && Q.Radius <= 0) || Q.Channel < 0 || Q.Channel >= ECC_MAX || Q.MaxTargets < 1 ||
          Q.MaxTargets > 64 || Q.IgnoredActors.Num() > 64 || Q.RequiredTags.Num() + Q.BlockedTags.Num() > 64 ||
          (!Q.ActorClassPath.IsEmpty() && (!IsValid(Class) || !Class->IsChildOf(AActor::StaticClass()) ||
                                           Class->HasAnyClassFlags(CLASS_NewerVersionExists))))
        return false;
    for (const auto& Names : {Q.RequiredTags, Q.BlockedTags})
        for (FName Name : Names)
            if (!FGameplayTag::RequestGameplayTag(Name, false).IsValid())
                return false;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(DasGASTargeting), Q.TraceComplex);
    if (Q.IgnoreSelf)
    {
        Params.AddIgnoredActor(GetOwner());
        Params.AddIgnoredActor(GetOwnerActor());
        Params.AddIgnoredActor(GetAvatarActor());
    }
    for (const auto& A : Q.IgnoredActors)
    {
        if (!IsValid(A) || A->IsActorBeingDestroyed() || A->GetWorld() != GetWorld())
            return false;
        Params.AddIgnoredActor(A.Get());
    }
    Out = {};
    if (Q.Mode == TEXT("Trace"))
    {
        TArray<FHitResult> Hits;
        if (Q.Radius == 0)
            GetWorld()->LineTraceMultiByChannel(Hits, Q.Start, Q.End, ECollisionChannel(Q.Channel), Params);
        else
            GetWorld()->SweepMultiByChannel(Hits, Q.Start, Q.End, FQuat::Identity, ECollisionChannel(Q.Channel),
                                            FCollisionShape::MakeSphere(Q.Radius), Params);
        // Native channel traces include overlaps up to the first blocking hit.
        for (const auto& H : Hits)
            if (AcceptTarget(Q, H.GetActor()) && !Out.Actors.Contains(H.GetActor()))
            {
                Out.Actors.Add(H.GetActor());
                Out.Hits.Add(H);
                if (Out.Actors.Num() == Q.MaxTargets)
                    break;
            }
    }
    else
    {
        TArray<FOverlapResult> Overlaps;
        GetWorld()->OverlapMultiByChannel(Overlaps, Q.Start, FQuat::Identity, ECollisionChannel(Q.Channel),
                                          FCollisionShape::MakeSphere(Q.Radius), Params);
        TArray<AActor*> Actors;
        for (const auto& O : Overlaps)
            if (AcceptTarget(Q, O.GetActor()))
                Actors.AddUnique(O.GetActor());
        Actors.Sort(
            [&](const AActor& A, const AActor& B)
            {
                const double DA = FVector::DistSquared(Q.Start, A.GetActorLocation());
                const double DB = FVector::DistSquared(Q.Start, B.GetActorLocation());
                return DA == DB ? A.GetUniqueID() < B.GetUniqueID() : DA < DB;
            });
        for (auto* A : Actors)
        {
            Out.Actors.Add(A);
            if (Out.Actors.Num() == Q.MaxTargets)
                break;
        }
    }
    return true;
}
int32 UDasAuthoredAbilitySystemComponent::BeginAuthoredTargeting(const FDasGASTargetingQuery& Q)
{
    FTargetSelection Selection;
    if (TargetSelections.Num() >= 64 || NextTargetSelection == MAX_int32 || !SelectTargets(Q, Selection))
        return 0;
    const int32 Id = NextTargetSelection++;
    TargetQueries.Add(Id, Q);
    TargetSelections.Add(Id, MoveTemp(Selection));
    return Id;
}
bool UDasAuthoredAbilitySystemComponent::UpdateAuthoredTargeting(int32 Id, const FDasGASTargetingQuery& Q)
{
    FTargetSelection Selection;
    if (!TargetSelections.Contains(Id) || !SelectTargets(Q, Selection))
        return false;
    TargetQueries.Add(Id, Q);
    TargetSelections.Add(Id, MoveTemp(Selection));
    return true;
}
FDasGASTargetingPreview UDasAuthoredAbilitySystemComponent::PreviewAuthoredTargeting(int32 Id) const
{
    FDasGASTargetingPreview R;
    const auto* S = TargetSelections.Find(Id);
    const auto* Q = TargetQueries.Find(Id);
    if (!Live() || !S || !Q)
        return R;
    R.Valid = true;
    for (const auto& A : S->Actors)
        if (AcceptTarget(*Q, A.Get()))
            R.Actors.Add(A.Get());
    return R;
}
FDasGASTargetingResult UDasAuthoredAbilitySystemComponent::ConfirmAuthoredTargeting(int32 Id)
{
    FDasGASTargetingResult R;
    auto* S = TargetSelections.Find(Id);
    const auto* Q = TargetQueries.Find(Id);
    if (!Live() || !S || !Q)
        return R;
    // Selection is a snapshot. Revalidate lifetime and filters; do not silently retarget moved actors.
    // Fail/consume the whole session if any selected actor became invalid.
    for (const auto& A : S->Actors)
        if (!AcceptTarget(*Q, A.Get()))
        {
            CancelAuthoredTargeting(Id);
            return R;
        }
    for (const auto& H : S->Hits)
    {
        const int32 HitId = RegisterAuthoredTargetHit(H);
        if (!HitId)
        {
            for (int32 Allocated : R.Targets.Hits)
                ReleaseAuthoredTargetHit(Allocated);
            CancelAuthoredTargeting(Id);
            return {};
        }
        R.Targets.Hits.Add(HitId);
    }
    if (S->Hits.IsEmpty())
        for (const auto& A : S->Actors)
            R.Targets.Actors.Add(A.Get());
    if (!R.Targets.Hits.IsEmpty())
        R.Targets.Source = this;
    R.Confirmed = true;
    CancelAuthoredTargeting(Id);
    return R;
}
bool UDasAuthoredAbilitySystemComponent::CancelAuthoredTargeting(int32 Id)
{
    TargetQueries.Remove(Id);
    return TargetSelections.Remove(Id) > 0;
}

int32 UDasAuthoredGameplayAbility::BeginScriptTargeting(int32 Serial, const FDasGASTargetingQuery& Q)
{
    if (!ValidScriptActivation(Serial))
        return 0;
    const int32 Id = ScriptSystem->BeginAuthoredTargeting(Q);
    if (Id)
        ScriptTargeting.Add(Id);
    return Id;
}
bool UDasAuthoredGameplayAbility::UpdateScriptTargeting(int32 Serial, int32 Id, const FDasGASTargetingQuery& Q)
{
    return ValidScriptActivation(Serial) && ScriptTargeting.Contains(Id) &&
           ScriptSystem->UpdateAuthoredTargeting(Id, Q);
}
FDasGASTargetingPreview UDasAuthoredGameplayAbility::PreviewScriptTargeting(int32 Serial, int32 Id) const
{
    return ValidScriptActivation(Serial) && ScriptTargeting.Contains(Id) ? ScriptSystem->PreviewAuthoredTargeting(Id)
                                                                         : FDasGASTargetingPreview();
}
FDasGASTargetingResult UDasAuthoredGameplayAbility::ConfirmScriptTargeting(int32 Serial, int32 Id)
{
    if (!ValidScriptActivation(Serial) || !ScriptTargeting.Contains(Id))
        return {};
    ScriptTargeting.Remove(Id);
    return ScriptSystem->ConfirmAuthoredTargeting(Id);
}
bool UDasAuthoredGameplayAbility::CancelScriptTargeting(int32 Serial, int32 Id)
{
    if (!ValidScriptActivation(Serial) || !ScriptTargeting.Remove(Id))
        return false;
    return ScriptSystem->CancelAuthoredTargeting(Id);
}
