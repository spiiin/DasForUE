#include "DasGASAuthoredCommandlet.h"
#include "DasGASAbility.h"
#include "DasGASAssetService.h"
#include "DasRuntime.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Modules/ModuleManager.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"
#include "UObject/StrongObjectPtr.h"

UDasGASHooksCommandlet::UDasGASHooksCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasGASHooksCommandlet::Main(const FString& Params)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    int32 Checks = 0;
    auto Check = [&](bool OK, const TCHAR* Name)
    {
        ++Checks;
        UE_LOG(LogTemp, Display, TEXT("DAS_GAS_HOOKS_%s %s"), OK ? TEXT("PASS") : TEXT("FAIL"), Name);
        return OK;
    };
    FString Fixture, Source, Error;
    if (!FParse::Value(*Params, TEXT("Fixture="), Fixture) || !FFileHelper::LoadFileToString(Source, *Fixture))
        return 2;
    const FString File = TEXT("gas_hooks_") + FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT(".das");
    const FString Path = R.ScriptPath(File);
    if (!FFileHelper::SaveStringToFile(Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return 2;
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*Path, false, true);
    };
    FDasGASDeclarations Example;
    if (!Check(R.ReadGASScript(R.ScriptPath(TEXT("gas_scripted_abilities.das")), Example, Error) &&
                               Example.Abilities.Num() == 1,
                               TEXT("readable_scripted_example_compiles")))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        return 1;
    }
    TMap<FString, UBlueprint*> Assets;
    const FString Folder = TEXT("/Game/Tests/GASAuthored/") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
    if (!Check(FDasGASAssetService::Apply(File, Folder, Assets, Error, true), TEXT("import_scripted_ability")))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        return 1;
    }
    TArray<TStrongObjectPtr<UBlueprint>> Keep;
    for (const auto& P : Assets)
        Keep.Emplace(P.Value);
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    auto* Host = World->SpawnActor<ADasGASAuthoredProbeHost>();
    if (!Host)
        return 1;
    ON_SCOPE_EXIT
    {
        R.ReleaseObject(Host);
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    FEditorScriptExecutionGuard Guard;
    auto* ASC = Host->Abilities.Get();
    const auto Utility = ASC->GiveAuthoredAbility(Assets.FindChecked(TEXT("UtilityAbility"))->GeneratedClass.Get(), 1);
    const bool UtilityActivated = Utility.Id > 0 && ASC->ActivateAuthoredAbility(Utility.Id);
    const auto UtilityState = ASC->AuthoredAbilityState(Utility.Id);
    if (!Check(UtilityActivated && !UtilityState.Active && UtilityState.Committed && UtilityState.Error.IsEmpty() &&
                   ASC->RemoveAuthoredAbility(Utility.Id),
               TEXT("scripted_ability_without_attributes_or_preset_effect")))
    {
        UE_LOG(LogTemp, Warning, TEXT("%s"), *UtilityState.Error);
        return 1;
    }
    if (!ASC->AddAuthoredAttributes(Assets.FindChecked(TEXT("ScriptAttributes"))->GeneratedClass.Get()))
        return 1;
    const auto Grant = ASC->GiveAuthoredAbility(Assets.FindChecked(TEXT("ScriptAbility"))->GeneratedClass.Get(), 2);
    if (!Check(Grant.Id > 0, TEXT("grant_scripted_ability")))
        return 1;
    auto Advance = [&]
    {
        for (int32 I = 0; I < 15; ++I)
        {
            ++GFrameCounter;
            World->Tick(LEVELTICK_All, 0.01f);
        }
    };
    const int32 Before = R.PersistentContextCount(World.Get());
    for (int32 Phase = 1; Phase <= 5; ++Phase)
    {
        if (Phase > 1)
            Advance();
        bool Activated;
        if (Phase == 1)
        {
            // Outer DAS -> native GAS -> another persistent DAS context -> nested End hook.
            const auto Result = R.RunPersistent(Path, Grant.Id, Host);
            Activated = Result.bSucceeded && Result.Value == 42;
            if (!Activated)
                UE_LOG(LogTemp, Warning, TEXT("%s"), *Result.Error);
        }
        else
            Activated = ASC->ActivateAuthoredAbility(Grant.Id);
        const auto State = ASC->AuthoredAbilityState(Grant.Id);
        const float Mana = ASC->ReadAuthoredAttribute(TEXT("ScriptAttributes"), TEXT("Mana"));
        const float Health = ASC->ReadAuthoredAttribute(TEXT("ScriptAttributes"), TEXT("Health"));
        const bool Active = Phase == 3 || Phase == 4;
        UE_LOG(LogTemp, Display,
               TEXT("GAS_HOOK_PHASE %d active=%d committed=%d mana=%.1f health=%.1f error=%s"), Phase, State.Active,
                    State.Committed, Mana, Health, *State.Error);
        const float ExpectedMana = Phase < 3 ? 20.f - 5.f * Phase : 5.f;
        const float ExpectedHealth = Phase == 1 ? 65.f : 90.f;
        if (!Check(Activated && State.Error.IsEmpty() && State.Active == Active && State.Committed == (Phase <= 3) &&
                       FMath::IsNearlyEqual(Mana, ExpectedMana) && FMath::IsNearlyEqual(Health, ExpectedHealth),
                   TEXT("hooks_state_stale_context_and_exactly_once_cost")))
            return 1;
        if (Phase == 3)
        {
            if (!Check(!ASC->ActivateAuthoredAbility(Grant.Id) && ASC->CancelAuthoredAbility(Grant.Id) &&
                           ASC->AuthoredAbilityState(Grant.Id).Error.IsEmpty(),
                       TEXT("external_cancel_calls_cancel_then_end")))
                return 1;
        }
        if (Phase == 4)
            if (!Check(ASC->FinishAuthoredAbility(Grant.Id) && ASC->AuthoredAbilityState(Grant.Id).Error.IsEmpty(),
                       TEXT("external_finish_calls_end_without_commit")))
                return 1;
    }
    UDasAuthoredGameplayAbility* Instance = nullptr;
    for (const auto& Spec : ASC->GetActivatableAbilities())
        if (ASC->AuthoredAbilityId(Spec.Handle) == Grant.Id)
            Instance = Cast<UDasAuthoredGameplayAbility>(Spec.GetPrimaryInstance());
    TStrongObjectPtr<UDasAuthoredGameplayAbility> Pin(Instance);
    const auto Old = Instance ? Instance->ScriptAbilityContext() : FDasAuthoredAbilityContext();
    R.ReleaseObject(Host);
    if (!Check(Instance && R.PersistentContextCount(World.Get()) == Before + 1 &&
                   ASC->RemoveAuthoredAbility(Grant.Id) && R.PersistentContextCount(World.Get()) == Before &&
                   !Instance->CommitScriptAbility(Old.ActivationId) && !Instance->EndScriptAbility(Old.ActivationId) &&
                   !Instance->CancelScriptAbility(Old.ActivationId),
               TEXT("revoke_releases_vm_and_invalidates_context")))
        return 1;
    UE_LOG(LogTemp, Display, TEXT("DAS_GAS_HOOKS_RESULT=PASS checks=%d"), Checks);
    return 0;
}
