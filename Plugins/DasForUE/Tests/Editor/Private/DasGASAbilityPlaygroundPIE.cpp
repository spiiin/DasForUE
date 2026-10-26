#include "DasGASPlaygroundMap.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
namespace
{
using namespace DasGASPlaygroundExample;
class FPlaygroundPIE : public IAutomationLatentCommand
{
    TSharedRef<FPlaygroundMap> Map;
    FAutomationTestBase* Test;
    int32 Phase = 0, Checks = 0;
    double Began = FPlatformTime::Seconds(), Changed = Began, ChangedGame = 0;
    bool ScreenshotRequested = false;
    FVector Before;
    TWeakObjectPtr<UNiagaraComponent> Aura;
    void Next(int32 P)
    {
        Phase = P;
        Changed = FPlatformTime::Seconds();
        if (auto* World = GEditor->PlayWorld.Get())
            ChangedGame = World->GetTimeSeconds();
        UE_LOG(LogTemp, Display, TEXT("DAS_GAS_ABILITY_PIE_PHASE=%d"), Phase);
    }
    void Key(APlayerController* PC, FKey K, bool Down)
    {
        if (PC)
            PC->InputKey(FInputKeyEventArgs(nullptr, IPlatformInputDeviceMapper::Get().GetDefaultInputDevice(), K,
                                            Down ? IE_Pressed : IE_Released, Down ? 1.f : 0.f, false,
                                            FPlatformTime::Cycles64()));
    }
    void Stop(APlayerController* PC)
    {
        for (auto K : {EKeys::W, EKeys::SpaceBar, EKeys::RightMouseButton, EKeys::LeftMouseButton, EKeys::C})
            Key(PC, K, false);
        GEditor->RequestEndPlayMap();
        Next(99);
    }
    void Check(bool OK, const TCHAR* N)
    {
        ++Checks;
        Test->TestTrue(N, OK);
        UE_LOG(LogTemp, Display, TEXT("DAS_GAS_ABILITY_PIE_%s %s"), OK ? TEXT("PASS"): TEXT("FAIL"), N);
    }

public:
    FPlaygroundPIE(TSharedRef<FPlaygroundMap> M, FAutomationTestBase* T) : Map(M), Test(T) {}
    bool Update() override
    {
        auto* W = GEditor->PlayWorld.Get();
        auto* PC = W ? W->GetFirstPlayerController() : nullptr;
        double Now = FPlatformTime::Seconds(), Elapsed = Now - Changed;
        if (Phase == 99)
        {
            if (W && Elapsed < 30)
                return false;
            Check(!W && FModuleManager::LoadModuleChecked<FDasRuntimeModule>(
                            TEXT("DasRuntime")).PersistentContextCount() == 0,
                            TEXT("EndPIE releases all DAS sessions"));
            if (!Test->HasAnyErrors())
                UE_LOG(LogTemp, Display, TEXT("DAS_GAS_ABILITY_PIE_RESULT=PASS checks=%d"), Checks);
            return true;
        }
        // GPU warmup/readback can stall wall time while PIE time does not advance.
        // Keep effect waits tied to the native world clock and retain a bounded wall deadline.
        if (Phase > 0 && W)
            Elapsed = W->GetTimeSeconds() - ChangedGame;
        if (Now - Began > 240)
        {
            Test->AddError(FString::Printf(TEXT("GAS PIE timeout phase=%d"), Phase));
            Stop(PC);
            return false;
        }
        AActor *Caster = nullptr, *Target = nullptr;
        if (W)
            for (TActorIterator<AActor> I(W); I; ++I)
            {
                if (I->GetFName() == TEXT("GAS_Caster"))
                    Caster = *I;
                if (I->GetFName() == TEXT("GAS_Target_Center"))
                    Target = *I;
            }
        auto* Pawn = PC ? Cast<ACharacter>(PC->GetPawn()) : nullptr;
        auto* Bound = Pawn ? FindFProperty<FBoolProperty>(Pawn->GetClass(), TEXT("ControlsBound")) : nullptr;
        if (!W || !W->HasBegunPlay() || !PC || !Pawn || !Caster || !Target || !Bound ||
            !Bound->GetPropertyValue_InContainer(Pawn))
            return false;
        auto* Source = Caster->FindComponentByClass<UDasAuthoredAbilitySystemComponent>();
        auto* Victim = Target->FindComponentByClass<UDasAuthoredAbilitySystemComponent>();
        auto* GP = FindFProperty<FStructProperty>(Caster->GetClass(), TEXT("Grant"));
        if (!Source || !Victim || !GP)
        {
            Test->AddError(TEXT("Missing GAS context"));
            Stop(PC);
            return false;
        }
        const auto Grant = *GP->ContainerPtrToValuePtr<FDasAuthoredAbilityGrant>(Caster);
        const auto State = Source->AuthoredAbilityState(Grant.Id);
        auto Health = [&]
        {
            return Victim->ReadAuthoredAttribute(TEXT("PlaygroundStats"), TEXT("Health"));
        };
        const auto AuraTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Das.Sample.Playground.Aura"));
        switch (Phase)
        {
        case 0:
            if (Elapsed < 1.5)
                break;
            Check(W->WorldType == EWorldType::PIE && W->FXSystem && Cast<ADasGASPlaygroundHUD>(PC->GetHUD()),
                  TEXT("real PIE possession HUD and FX"));
            Before = Pawn->GetActorLocation();
            Key(PC, EKeys::W, true);
            Key(PC, EKeys::SpaceBar, true);
            Next(1);
            break;
        case 1:
            if (Elapsed < 0.3)
                break;
            Check(IntField(Pawn, TEXT("MoveEvents")) > 0 && IntField(Pawn, TEXT("JumpEvents")) > 0 &&
                                                                     Pawn->GetActorLocation().X > Before.X + 5 &&
                                                                     Pawn->GetActorLocation().Z > Before.Z + 10,
                                                                     TEXT("mapped W and Space move and jump"));
            Key(PC, EKeys::W, false);
            Key(PC, EKeys::SpaceBar, false);
            Pawn->StopJumping();
            Pawn->TeleportTo(FVector(0, 0, 90), FRotator::ZeroRotator);
            Pawn->GetCharacterMovement()->StopMovementImmediately();
            PC->SetControlRotation(FRotator::ZeroRotator);
            PC->InputKey(FInputKeyEventArgs(nullptr, IPlatformInputDeviceMapper::Get().GetDefaultInputDevice(),
                                            EKeys::MouseX, 20.f, 1.f / 60, 1, FPlatformTime::Cycles64()));
            Next(2);
            break;
        case 2:
            if (Elapsed < 0.3)
                break;
            Check(IntField(Pawn, TEXT("LookEvents")) > 0 && FMath::Abs(PC->GetControlRotation().Yaw) > 0.1,
                           TEXT("mapped mouse rotates camera"));
            PC->SetControlRotation(FRotator::ZeroRotator);
            Next(3);
            break;
        case 3:
            if (Elapsed < 0.4)
                break;
            Key(PC, EKeys::RightMouseButton, true);
            Next(4);
            break;
        case 4:
            if (ObjectField(Caster, TEXT("PreviewTarget")) != Target || !State.Active)
                break;
            Check(!State.Committed && Health() == 100, TEXT("RMB previews native trace without damage"));
            Key(PC, EKeys::C, true);
            Next(5);
            break;
        case 5:
            if (IntField(Caster, TEXT("CancelledShots")) < 1 || State.Active)
                break;
            Check(Health() == 100 && !State.Committed &&
                  !ObjectField(Caster, TEXT("PreviewTarget")), TEXT("C cancels selection without effects"));
            Key(PC, EKeys::C, false);
            Key(PC, EKeys::RightMouseButton, false);
            Next(6);
            break;
        case 6:
            if (Elapsed < 0.2)
                break;
            Key(PC, EKeys::RightMouseButton, true);
            Next(7);
            break;
        case 7:
            if (ObjectField(Caster, TEXT("PreviewTarget")) != Target || !State.Active)
                break;
            Key(PC, EKeys::LeftMouseButton, true);
            Next(8);
            break;
        case 8:
            if (IntField(Caster, TEXT("Shots")) != 1)
                break;
            Check(Health() == 99 && !State.Active && State.Committed &&
                  ObjectField(Caster, TEXT("LastTarget")) == Target &&
                              IntField(Pawn, TEXT("ConfirmEvents")) == 1,
                                       TEXT("LMB confirms DAS ability and applies damage"));
            {
                TArray<UNiagaraComponent*> FX;
                Target->GetComponents(FX);
                bool Hit = false;
                for (auto* C : FX)
                    if (IsValid(C) && C->GetAsset())
                    {
                        if (C->GetAsset()->GetFName() == TEXT("NS_Aura"))
                            Aura = C;
                        if (C->GetAsset()->GetFName() == TEXT("NS_Impact"))
                            Hit = FVector::Distance(C->GetComponentLocation(), Target->GetActorLocation()) < 35;
                    }
                TArray<UAudioComponent*> Audio;
                Target->GetComponents(Audio);
                Check(Hit && !Audio.IsEmpty() && Audio[0]->Sound && Aura.IsValid() && Aura->IsRegistered() &&
                          Victim->IsGameplayCueActive(AuraTag),
                      TEXT("hit sound Niagara and active duration aura"));
            }
            Key(PC, EKeys::LeftMouseButton, false);
            Key(PC, EKeys::RightMouseButton, false);
            Next(9);
            break;
        case 9:
            if (Elapsed > 0.6 && !ScreenshotRequested)
            {
                FScreenshotRequest::RequestScreenshot(Map->Screenshot, false, false);
                ScreenshotRequested = true;
            }
            if (Victim->IsGameplayCueActive(AuraTag))
                break;
            Check(Health() == 99 && (!Aura.IsValid() || Aura->IsBeingDestroyed()),
                  TEXT("native effect expiry calls Removed and destroys aura"));
            Check(IFileManager::Get().FileExists(*Map->Screenshot), TEXT("PIE viewport screenshot saved"));
            Stop(PC);
            break;
        }
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasGASAbilityPlaygroundPIE, "DasForUE.GAS.AbilityPlaygroundPIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter |
                                     EAutomationTestFlags::NonNullRHI)
bool FDasGASAbilityPlaygroundPIE::RunTest(const FString&)
{
    auto Map = MakeShared<FPlaygroundMap>();
    FString Error;
    if (!Map->Prepare(Error))
    {
        AddError(Error);
        return false;
    }
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FPlaygroundPIE>(Map, this));
    return true;
}
#endif
