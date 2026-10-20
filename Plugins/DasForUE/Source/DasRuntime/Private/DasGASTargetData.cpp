#include "DasGASAuthoring.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameFramework/Actor.h"
bool UDasAuthoredAbilitySystemComponent::SendAuthoredGameplayEvent(FName Name, float Magnitude, AActor* Instigator,
                                                                   AActor* Target)
{
    const auto Tag = FGameplayTag::RequestGameplayTag(Name, false);
    if (!Live() || !Tag.IsValid() || !FMath::IsFinite(Magnitude))
        return false;
    for (auto* A : {Instigator, Target})
        if (A && (!IsValid(A) || A->IsActorBeingDestroyed() || A->GetWorld() != GetWorld()))
            return false;
    FGameplayEventData P;
    P.EventTag = Tag;
    P.EventMagnitude = Magnitude;
    P.Instigator = Instigator;
    P.Target = Target;
    HandleGameplayEvent(Tag, &P);
    return true;
}
int32 UDasAuthoredAbilitySystemComponent::RegisterAuthoredTargetHit(const FHitResult& Hit)
{
    auto* Actor = Hit.GetActor();
    if (!Live() || !IsValid(Actor) || Actor->IsActorBeingDestroyed() || Actor->GetWorld() != GetWorld() ||
        !FMath::IsFinite(Hit.Time) || !FMath::IsFinite(Hit.Distance) || !FMath::IsFinite(Hit.PenetrationDepth) ||
        Hit.Location.ContainsNaN() || Hit.ImpactPoint.ContainsNaN() || Hit.Normal.ContainsNaN() ||
        Hit.ImpactNormal.ContainsNaN() || Hit.TraceStart.ContainsNaN() || Hit.TraceEnd.ContainsNaN() ||
        TargetHits.Num() >= 1024 || NextTargetHit == MAX_int32)
        return 0;
    const int32 Id = NextTargetHit++;
    TargetHits.Add(Id, Hit);
    return Id;
}
bool UDasAuthoredAbilitySystemComponent::ReleaseAuthoredTargetHit(int32 Id)
{
    return Live() && TargetHits.Remove(Id) > 0;
}
FDasGASTargetData UDasAuthoredAbilitySystemComponent::MakeAuthoredTargets(const TArray<AActor*>& Actors,
                                                                          const TArray<int32>& Hits) const
{
    FDasGASTargetData R;
    if (!Live() || Actors.Num() + Hits.Num() > 64)
        return R;
    for (auto* A : Actors)
    {
        if (!IsValid(A) || A->IsActorBeingDestroyed() || A->GetWorld() != GetWorld())
            return {};
        R.Actors.AddUnique(A);
    }
    for (int32 Id : Hits)
    {
        const auto* H = TargetHits.Find(Id);
        if (!H || !IsValid(H->GetActor()) || H->GetActor()->IsActorBeingDestroyed())
            return {};
        R.Hits.AddUnique(Id);
    }
    if (!Hits.IsEmpty())
        R.Source = const_cast<UDasAuthoredAbilitySystemComponent*>(this);
    return R;
}
TArray<FDasAuthoredEffectResult>
UDasAuthoredAbilitySystemComponent::ApplyAuthoredTargets(const FDasAuthoredEffectSpec& Spec,
                                                         const FDasGASTargetData& Targets)
{
    TArray<FDasAuthoredEffectResult> Results;
    if (!Live() || Spec.Source != this || Targets.Actors.Num() + Targets.Hits.Num() > 64 ||
        (!Targets.Hits.IsEmpty() && Targets.Source != this))
        return Results;
    FGameplayAbilityTargetDataHandle Native;
    if (!Targets.Actors.IsEmpty())
    {
        auto* D = new FGameplayAbilityTargetData_ActorArray();
        for (const auto& A : Targets.Actors)
            D->TargetActorArray.Add(A.Get());
        Native.Add(D);
    }
    for (int32 Id : Targets.Hits)
    {
        const auto* H = TargetHits.Find(Id);
        if (!H)
            return {};
        Native.Add(new FGameplayAbilityTargetData_SingleTargetHit(*H));
    }
    TArray<AActor*> Actors;
    TMap<AActor*, FHitResult> Hits;
    for (int32 I = 0; I < Native.Num(); ++I)
    {
        const auto* D = Native.Get(I);
        for (auto Weak : D->GetActors())
        {
            auto* Actor = Weak.Get();
            if (!IsValid(Actor) || Actor->IsActorBeingDestroyed() || Actor->GetWorld() != GetWorld())
                return {};
            auto* ASC = Cast<UDasAuthoredAbilitySystemComponent>(
                UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, true));
            if (!ASC || !ASC->Live())
                return {};
            Actors.AddUnique(Actor);
            if (const auto* H = D->GetHitResult())
                Hits.Add(Actor, *H);
        }
    }
    for (auto* Actor : Actors)
    {
        auto* ASC = CastChecked<UDasAuthoredAbilitySystemComponent>(
            UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, true));
        Results.Add(ASC->ApplyAuthoredSpecInternal(Spec, Hits.Find(Actor)));
    }
    return Results;
}
