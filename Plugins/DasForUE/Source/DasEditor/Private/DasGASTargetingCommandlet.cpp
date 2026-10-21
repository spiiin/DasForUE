#include "DasGASAuthoredCommandlet.h"
#include "DasGASAssetService.h"
#include "DasRuntime.h"
#include "Components/SphereComponent.h"
#include "Editor.h"
#include "GameFramework/Pawn.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "UObject/StrongObjectPtr.h"
#include "Misc/ScopeExit.h"

UDasGASTargetingCommandlet::UDasGASTargetingCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasGASTargetingCommandlet::Main(const FString& Params)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const auto IVS = UWorld::InitializationValues()
                         .CreatePhysicsScene(true)
                         .ShouldSimulatePhysics(false)
                         .AllowAudioPlayback(false)
                         .CreateNavigation(false)
                         .CreateAISystem(false);
    TStrongObjectPtr<UWorld> World(
        UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &IVS));
    if (!World.IsValid() || !World->GetPhysicsScene())
        return 1;
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    auto* Host = World->SpawnActor<ADasGASAuthoredProbeHost>();
    auto MakeTarget = [&](double X, ECollisionResponse Response)
    {
        auto* A = World->SpawnActor<ADasGASAuthoredProbeHost>();
        auto* Sphere = NewObject<USphereComponent>(A);
        A->SetRootComponent(Sphere);
        A->AddInstanceComponent(Sphere);
        Sphere->InitSphereRadius(15.f);
        Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
        Sphere->SetCollisionResponseToChannel(ECC_Visibility, Response);
        Sphere->RegisterComponent();
        A->SetActorLocation(FVector(X, 0, 0));
        return A;
    };
    auto* Target = MakeTarget(100, ECR_Overlap);
    auto* Blocker = MakeTarget(200, ECR_Block);
    auto* Hidden = MakeTarget(260, ECR_Overlap);
    ON_SCOPE_EXIT
    {
        R.ReleaseObject(Host);
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    const auto Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.Healing"));
    Target->Abilities->AddLooseGameplayTag(Tag);
    Hidden->Abilities->AddLooseGameplayTag(Tag);
    auto* ASC = Host->Abilities.Get();
    int32 Checks = 0;
    auto Check = [&](bool OK, const TCHAR* Name)
    {
        ++Checks;
        UE_LOG(LogTemp, Display, TEXT("DAS_GAS_TARGETING_%s %s"), OK ? TEXT("PASS") : TEXT("FAIL"), Name);
        return OK;
    };
    FString Error;
    TMap<FString, UBlueprint*> Assets;
    const auto Folder = TEXT("/Game/Tests/GASAuthored/")+ FGuid::NewGuid().ToString(EGuidFormats::Digits);
    FEditorScriptExecutionGuard Guard;
    if (!FDasGASAssetService::Apply(TEXT("gas_targeting_probe.das"), Folder, Assets, Error, true))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        return 1;
    }
    TArray<TStrongObjectPtr<UBlueprint>> Keep;
    for (auto& P : Assets)
        Keep.Emplace(P.Value);
    const auto Call = R.RunPersistent(R.ScriptPath(TEXT("gas_targeting_probe.das")), 0, Host);
    if (!Check(Call.bSucceeded && Call.Value == 42, TEXT("das_trace_overlap_confirm_cancel")))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Call.Error);
        return 1;
    }
    FDasGASTargetingQuery Q;
    Q.Mode = TEXT("Overlap");
    Q.Channel = ECC_Visibility;
    Q.Start = FVector(150, 0, 0);
    Q.Radius = 90;
    Q.RequiredTags = {Tag.GetTagName()};
    Q.ActorClassPath = ADasGASAuthoredProbeHost::StaticClass()->GetPathName();
    Q.BlockedTags = {Tag.GetTagName()};
    auto Id = ASC->BeginAuthoredTargeting(Q);
    if (!Check(Id > 0 && ASC->PreviewAuthoredTargeting(Id).Actors.IsEmpty() && ASC->CancelAuthoredTargeting(Id),
               TEXT("blocked_tags_empty_valid_selection")))
        return 1;
    Q.BlockedTags.Empty();
    Q.IgnoredActors = {Target};
    Id = ASC->BeginAuthoredTargeting(Q);
    if (!Check(ASC->PreviewAuthoredTargeting(Id).Actors.IsEmpty() && ASC->CancelAuthoredTargeting(Id),
               TEXT("explicit_ignored_actor")))
        return 1;
    Q.IgnoredActors.Empty();
    Q.ActorClassPath = APawn::StaticClass()->GetPathName();
    Id = ASC->BeginAuthoredTargeting(Q);
    if (!Check(ASC->PreviewAuthoredTargeting(Id).Actors.IsEmpty() && ASC->CancelAuthoredTargeting(Id),
               TEXT("actor_class_filter")))
        return 1;
    Q.ActorClassPath.Reset();
    Q.Mode = TEXT("Trace");
    Q.Start = FVector::ZeroVector;
    Q.End = FVector(300, 0, 0);
    Q.Radius = 5;
    Id = ASC->BeginAuthoredTargeting(Q);
    auto Confirmed = ASC->ConfirmAuthoredTargeting(Id);
    if (!Check(Confirmed.Confirmed && Confirmed.Targets.Hits.Num() == 1, TEXT("sphere_sweep_native_hit")))
        return 1;
    auto* Stats = Assets.FindChecked(TEXT("TargetStats"))->GeneratedClass.Get();
    if (!ASC->AddAuthoredAttributes(Stats) || !Target->Abilities->AddAuthoredAttributes(Stats))
        return 1;
    auto Spec = ASC->MakeAuthoredSpec(Assets.FindChecked(TEXT("TargetDamage"))->GeneratedClass.Get(), 1);
    auto Effects = ASC->ApplyAuthoredTargets(Spec, Confirmed.Targets);
    if (!Check(Effects.Num() == 1 && Effects[0].Applied &&
               Target->Abilities->ReadAuthoredAttribute(TEXT("TargetStats"), TEXT("Health")) == 90.f,
                                                        TEXT("confirmed_selection_applies_effect_once")))
        return 1;
    for (int32 Hit : Confirmed.Targets.Hits)
        ASC->ReleaseAuthoredTargetHit(Hit);
    Id = ASC->BeginAuthoredTargeting(Q);
    Target->Abilities->RemoveLooseGameplayTag(Tag);
    if (!Check(!ASC->ConfirmAuthoredTargeting(Id).Confirmed && !ASC->PreviewAuthoredTargeting(Id).Valid,
               TEXT("confirmation_rechecks_tags_and_consumes_failure")))
        return 1;
    Target->Abilities->AddLooseGameplayTag(Tag);
    const auto Grant = ASC->GiveAuthoredAbility(Assets.FindChecked(TEXT("TargetingProbe"))->GeneratedClass.Get(), 1);
    if (!ASC->ActivateAuthoredAbility(Grant.Id))
        return 1;
    if (!ASC->CancelAuthoredAbility(Grant.Id) || !ASC->ActivateAuthoredAbility(Grant.Id) ||
        !ASC->FinishAuthoredAbility(Grant.Id))
        return 1;
    if (!Check(ASC->AuthoredAbilityState(Grant.Id).Error.IsEmpty(), TEXT("activation_cleanup_and_stale_serial")))
        return 1;
    Id = ASC->BeginAuthoredTargeting(Q);
    Target->Destroy();
    if (!Check(!ASC->ConfirmAuthoredTargeting(Id).Confirmed, TEXT("destroyed_target_refused")))
        return 1;
    UE_LOG(LogTemp, Display, TEXT("DAS_GAS_TARGETING_RESULT=PASS checks=%d"), Checks);
    return 0;
}
