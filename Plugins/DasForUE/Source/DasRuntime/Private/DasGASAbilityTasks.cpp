#include "DasGASAbilityTaskRelay.h"
#include "DasGASAbility.h"
#include "DasGASAuthoring.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Actor.h"
void UDasGASAbilityTaskRelay::Stop()
{
    if (auto* T = Cast<UAbilityTask_WaitDelay>(Native))
        T->OnFinish.RemoveAll(this);
    if (auto* T = Cast<UAbilityTask_WaitGameplayEvent>(Native))
        T->EventReceived.RemoveAll(this);
    if (auto* T = Cast<UAbilityTask_PlayMontageAndWait>(Native))
    {
        T->OnCompleted.RemoveAll(this);
        T->OnBlendedIn.RemoveAll(this);
        T->OnBlendOut.RemoveAll(this);
        T->OnInterrupted.RemoveAll(this);
        T->OnCancelled.RemoveAll(this);
    }
    if (IsValid(Native))
        Native->EndTask();
    Native = nullptr;
}
void UDasGASAbilityTaskRelay::Emit(FName Phase, bool Terminal, const FGameplayEventData* P)
{
    auto* A = Ability.Get();
    if (!A)
        return;
    FDasGASAbilityTaskResult R;
    R.Id = Id;
    R.Kind = Kind;
    R.Phase = Phase;
    if (P)
    {
        R.EventTag = P->EventTag.GetTagName();
        R.Magnitude = P->EventMagnitude;
        R.Instigator = const_cast<AActor*>(P->Instigator.Get());
        R.Target = const_cast<AActor*>(P->Target.Get());
    }
    A->DeliverScriptTask(this, R, Terminal);
}
void UDasGASAbilityTaskRelay::Finished()
{
    Emit(TEXT("Finished"), true);
}
void UDasGASAbilityTaskRelay::Event(FGameplayEventData P)
{
    Emit(TEXT("Received"), Once, &P);
}
void UDasGASAbilityTaskRelay::Completed()
{
    Emit(TEXT("Completed"), true);
}
void UDasGASAbilityTaskRelay::BlendedIn()
{
    Emit(TEXT("BlendedIn"), false);
}
void UDasGASAbilityTaskRelay::BlendOut()
{
    Emit(TEXT("BlendOut"), false);
}
void UDasGASAbilityTaskRelay::Interrupted()
{
    Emit(TEXT("Interrupted"), true);
}
void UDasGASAbilityTaskRelay::Cancelled()
{
    Emit(TEXT("Cancelled"), true);
}
UDasGASAbilityTaskRelay* UDasAuthoredGameplayAbility::NewScriptTask(int32 Serial, FName Kind)
{
    if (!ValidScriptActivation(Serial) || TaskEntry.IsEmpty() || ScriptTasks.Num() >= 64 || NextScriptTask == MAX_int32)
        return nullptr;
    auto* R = NewObject<UDasGASAbilityTaskRelay>(this);
    R->Ability = this;
    R->Id = NextScriptTask++;
    R->Serial = Serial;
    R->Kind = Kind;
    ScriptTasks.Add(R->Id, R);
    return R;
}
int32 UDasAuthoredGameplayAbility::WaitScriptDelay(int32 Serial, float Seconds)
{
    if (!FMath::IsFinite(Seconds) || Seconds < 0.01f || Seconds > 3600)
        return 0;
    auto* R = NewScriptTask(Serial, TEXT("Delay"));
    if (!R)
        return 0;
    auto* T = UAbilityTask_WaitDelay::WaitDelay(this, Seconds);
    if (!T)
    {
        ScriptTasks.Remove(R->Id);
        return 0;
    }
    R->Native = T;
    T->OnFinish.AddDynamic(R, &UDasGASAbilityTaskRelay::Finished);
    T->ReadyForActivation();
    return R->Id;
}
int32 UDasAuthoredGameplayAbility::WaitScriptEvent(int32 Serial, FName Name, bool Once, bool Exact)
{
    const auto Tag = FGameplayTag::RequestGameplayTag(Name, false);
    if (!Tag.IsValid())
        return 0;
    auto* R = NewScriptTask(Serial, TEXT("GameplayEvent"));
    if (!R)
        return 0;
    R->Once = Once;
    auto* T = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tag, nullptr, Once, Exact);
    if (!T)
    {
        ScriptTasks.Remove(R->Id);
        return 0;
    }
    R->Native = T;
    T->EventReceived.AddDynamic(R, &UDasGASAbilityTaskRelay::Event);
    T->ReadyForActivation();
    return R->Id;
}
int32 UDasAuthoredGameplayAbility::PlayScriptMontage(int32 Serial, UAnimMontage* Montage, float Rate, FName Section)
{
    if (!ValidScriptActivation(Serial) || !IsValid(Montage) || !FMath::IsFinite(Rate) || Rate <= 0 || !GetActorInfo().AnimInstance.IsValid() ||
        (!Section.IsNone() && Montage->GetSectionIndex(Section) == INDEX_NONE))
        return 0;
    auto* R = NewScriptTask(Serial, TEXT("Montage"));
    if (!R)
        return 0;
    auto* T =
        UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Montage, Rate, Section, true);
    if (!T)
    {
        ScriptTasks.Remove(R->Id);
        return 0;
    }
    R->Native = T;
    T->OnCompleted.AddDynamic(R, &UDasGASAbilityTaskRelay::Completed);
    T->OnBlendedIn.AddDynamic(R, &UDasGASAbilityTaskRelay::BlendedIn);
    T->OnBlendOut.AddDynamic(R, &UDasGASAbilityTaskRelay::BlendOut);
    T->OnInterrupted.AddDynamic(R, &UDasGASAbilityTaskRelay::Interrupted);
    T->OnCancelled.AddDynamic(R, &UDasGASAbilityTaskRelay::Cancelled);
    T->ReadyForActivation();
    return R->Id;
}
bool UDasAuthoredGameplayAbility::CancelScriptTask(int32 Serial, int32 Id)
{
    if (!ValidScriptActivation(Serial))
        return false;
    auto* P = ScriptTasks.Find(Id);
    if (!P)
        return false;
    auto* R = P->Get();
    ScriptTasks.Remove(Id);
    R->Stop();
    return true;
}
void UDasAuthoredGameplayAbility::ClearScriptTasks()
{
    if (auto* ASC = ScriptSystem.Get())
        for (int32 Id : ScriptTargeting)
            ASC->CancelAuthoredTargeting(Id);
    ScriptTargeting.Empty();
    auto Pending = MoveTemp(ScriptTasks);
    ScriptTasks.Empty();
    for (auto& P : Pending)
        if (IsValid(P.Value))
            P.Value->Stop();
}
void UDasAuthoredGameplayAbility::DeliverScriptTask(UDasGASAbilityTaskRelay* R, const FDasGASAbilityTaskResult& Result,
                                                    bool Terminal)
{
    if (!R || !ValidScriptActivation(R->Serial) || ScriptTasks.FindRef(R->Id) != R)
        return;
    if (Terminal)
        ScriptTasks.Remove(R->Id);
    TGuardValue<FDasGASAbilityTaskResult> Scope(CurrentTask, Result);
    if (!RunScriptHook(TaskEntry, TEXT("Task")) && IsActive())
        CancelScriptAbility(R->Serial);
}
