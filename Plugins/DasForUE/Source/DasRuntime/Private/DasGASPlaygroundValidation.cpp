#include "DasGASPlaygroundGame.h"
#if !UE_BUILD_SHIPPING || DAS_GAS_SHIPPING_VALIDATION
#include "DasGASAuthoring.h"
#include "DasGASAbility.h"
#include "DasRuntime.h"
#include "Containers/Ticker.h"
#include "Engine/World.h"
#include "AudioDevice.h"
#include "HAL/PlatformProperties.h"
#include "EngineUtils.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputKeyEventArgs.h"
#include "GenericPlatform/GenericPlatformInputDeviceMapper.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "HAL/FileManager.h"
#include "UObject/UnrealType.h"
#include "UnrealClient.h"
namespace
{
UObject* ObjectField(UObject* O, const TCHAR* N)
{
    auto* P = O ? FindFProperty<FObjectPropertyBase>(O->GetClass(), N) : nullptr;
    return P ? P->GetObjectPropertyValue_InContainer(O) : nullptr;
}
int32 IntField(UObject* O, const TCHAR* N)
{
    auto* P = O ? FindFProperty<FIntProperty>(O->GetClass(), N) : nullptr;
    return P ? P->GetPropertyValue_InContainer(O) : 0;
}
struct FDasGASPackagedProbe
{
    TWeakObjectPtr<UWorld> World;
    int32 Phase = 0, Checks = 0;
    bool Good = true, ScreenshotRequested = false;
    double Began = FPlatformTime::Seconds(), ChangedGame = 0;
    FVector Before;
    TWeakObjectPtr<UNiagaraComponent> Aura;
    FString Screenshot, Report;
    explicit FDasGASPackagedProbe(UWorld* W) : World(W)
    {
        FParse::Value(FCommandLine::Get(), TEXT("DasGASReport="), Report);
        if (!Report.IsEmpty())
        {
            Report = FPaths::ConvertRelativePathToFull(Report);
            IFileManager::Get().MakeDirectory(*FPaths::GetPath(Report), true);
            FFileHelper::SaveStringToFile(
                FString::Printf(TEXT("DAS_GAS_PACKAGE_CONFIGURATION=%s\nDAS_GAS_EXECUTION=VM\n"),
                                     UE_BUILD_SHIPPING ? TEXT("Shipping") : TEXT("Development")), *Report,
                                     FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
        }
        FParse::Value(FCommandLine::Get(), TEXT("DasGASScreenshot="), Screenshot);
        if (Screenshot.IsEmpty())
            Screenshot = FPaths::ProjectSavedDir() / TEXT("gas-ability-packaged.png");
        Screenshot = FPaths::ConvertRelativePathToFull(Screenshot);
        IFileManager::Get().MakeDirectory(*FPaths::GetPath(Screenshot), true);
        IFileManager::Get().Delete(*Screenshot);
    }
    void WriteReport(const FString& Line)
    {
        if (!Report.IsEmpty())
            FFileHelper::SaveStringToFile(Line + TEXT("\n"), *Report,
                                                      FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM,
                                                      &IFileManager::Get(), FILEWRITE_Append);
    }
    void Check(bool OK, const TCHAR* N)
    {
        ++Checks;
        Good &= OK;
        WriteReport(FString::Printf(TEXT("DAS_GAS_ABILITY_PACKAGED_%s %s"), OK ? TEXT("PASS") : TEXT("FAIL"), N));
        UE_LOG(LogTemp, Display, TEXT("DAS_GAS_ABILITY_PACKAGED_%s %s"), OK ? TEXT("PASS") : TEXT("FAIL"), N);
    }
    void Next(int32 P)
    {
        Phase = P;
        WriteReport(FString::Printf(TEXT("DAS_GAS_PACKAGE_PHASE=%d"), P));
        if (World.IsValid())
            ChangedGame = World->GetTimeSeconds();
        UE_LOG(LogTemp, Display, TEXT("DAS_GAS_ABILITY_PACKAGED_PHASE=%d"), Phase);
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
        Next(99);
    }
    bool Tick(float)
    {
        auto* W = World.Get();
        auto* PC = W ? W->GetFirstPlayerController() : nullptr;
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        if (Phase == 99)
        {
            if (PC)
                PC->UnPossess();
            TArray<AActor*> Actors;
            if (W)
                for (TActorIterator<AActor> I(W); I; ++I)
                    if (I->GetName().StartsWith(TEXT("GAS_")))
                        Actors.Add(*I);
            for (auto* A : Actors)
                A->Destroy();
            Check(Runtime.PersistentContextCount(W) == 0, TEXT("destroyed map actors release DAS contexts"));
            UE_LOG(LogTemp, Display,
                   TEXT("DAS_GAS_ABILITY_PACKAGED_RESULT=%s checks=%d"), Good ? TEXT("PASS") : TEXT("FAIL"), Checks);
            WriteReport(FString::Printf(TEXT("DAS_GAS_ABILITY_PACKAGED_RESULT=%s checks=%d"),
                                             Good ? TEXT("PASS") : TEXT("FAIL"), Checks));
            FPlatformMisc::RequestExitWithStatus(false, Good ? 0 : 1);
            return false;
        }
        if (!W || FPlatformTime::Seconds() - Began > 240)
        {
            UE_LOG(LogTemp, Error, TEXT("GAS packaged timeout phase=%d"), Phase);
            Check(false, TEXT("bounded runtime deadline"));
            Stop(PC);
            return true;
        }
        AActor *Caster = nullptr, *Target = nullptr;
        for (TActorIterator<AActor> I(W); I; ++I)
        {
            if (I->GetFName() == TEXT("GAS_Caster"))
                Caster = *I;
            if (I->GetFName() == TEXT("GAS_Target_Center"))
                Target = *I;
        }
        auto* Pawn = PC ? Cast<ACharacter>(PC->GetPawn()) : nullptr;
        auto* Bound = Pawn ? FindFProperty<FBoolProperty>(Pawn->GetClass(), TEXT("ControlsBound")) : nullptr;
        if (!W->HasBegunPlay() || !Caster || !Target || !Bound || !Bound->GetPropertyValue_InContainer(Pawn))
            return true;
        auto* Source = Caster->FindComponentByClass<UDasAuthoredAbilitySystemComponent>();
        auto* Victim = Target->FindComponentByClass<UDasAuthoredAbilitySystemComponent>();
        auto* GP = FindFProperty<FStructProperty>(Caster->GetClass(), TEXT("Grant"));
        if (!Source || !Victim || !GP)
        {
            Check(false, TEXT("authored GAS context loaded"));
            Stop(PC);
            return true;
        }
        const auto Grant = *GP->ContainerPtrToValuePtr<FDasAuthoredAbilityGrant>(Caster);
        const auto State = Source->AuthoredAbilityState(Grant.Id);
        auto Health = [&]
        {
            return Victim->ReadAuthoredAttribute(TEXT("PlaygroundStats"), TEXT("Health"));
        };
        const auto AuraTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Das.Sample.Playground.Aura"));
        const double Elapsed = W->GetTimeSeconds() - ChangedGame;
        if (Phase == 0 && Elapsed >= 1.5)
        {
            bool Bundled = true;
            for (const auto* Name :
                 {TEXT("dsl_gas_playground_player.das"), TEXT("dsl_gas_ability_playground.das"),
                                                              TEXT("dsl_gas_ability_target.das"),
                                                                   TEXT("gas_targeted_strike.das"),
                                                                        TEXT("gas_cue_playground.das"),
                                                                             TEXT("ue_gas_playground_refs.das")})
            {
                const FString Path = Runtime.ScriptPath(Name);
                FString Text;
                Bundled &= FPaths::IsUnderDirectory(
                    Path, FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT(".."))) &&
                    FFileHelper::LoadFileToString(Text, *Path) && !Text.IsEmpty();
                UE_LOG(LogTemp, Display, TEXT("DAS_GAS_BUNDLED_SCRIPT %s"), *Path);
                WriteReport(TEXT("DAS_GAS_BUNDLED_SCRIPT ") + Path);
            }
            Check(FPlatformProperties::RequiresCookedData() && !GIsEditor && Bundled,
                  TEXT("DAS modules read from the packaged project"));
#if UE_BUILD_SHIPPING
            // Verify a real linked native AOT function, not merely successful VM fallback.
            const auto Aot = Runtime.RunFile(Runtime.ScriptPath(TEXT("aot_smoke.das")), 40, true);
            Check(Aot.bSucceeded && Aot.Value == 42 && Aot.bUsedAot,
                  TEXT("linked pure DAS AOT executes without VM fallback"));
            WriteReport(TEXT("DAS_GAS_AOT_SCOPE=pure-smoke; GAS hooks remain VM (UE externs noAot)"));
            if (!Aot.bSucceeded)
                WriteReport(TEXT("DAS_GAS_AOT_ERROR=") + Aot.Error);
#endif
            bool Configured = true;
            for (const auto* Field :
                 {TEXT("Controls"), TEXT("MoveAction"), TEXT("LookAction"), TEXT("JumpAction"),
                                                                                 TEXT("AimAction"),
                                                                                      TEXT("ConfirmAction"),
                                                                                           TEXT("CancelAction")})
                Configured &= ObjectField(Pawn, Field) != nullptr;
            for (const auto* Field : {TEXT("Attributes"), TEXT("HitEffect"), TEXT("AuraEffect"), TEXT("StrikeAbility")})
                Configured &= ObjectField(Caster, Field) != nullptr;
            auto* Visual = Target->FindComponentByClass<UStaticMeshComponent>();
            Check(Configured && Visual && Visual->GetStaticMesh() && W->GetAudioDevice().IsValid(),
                  TEXT("cooked input GAS mesh assets and live audio device"));
            if (!Good)
            {
                Stop(PC);
                return true;
            }
        }
        switch (Phase)
        {
        case 0:
            if (Elapsed < 1.5)
                break;
            Check(W->WorldType == EWorldType::Game && W->FXSystem && Cast<ADasGASPlaygroundHUD>(PC->GetHUD()),
                  TEXT("cooked game possession HUD and FX"));
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
                Check(Hit && !Audio.IsEmpty() && Audio[0]->Sound && Audio[0]->IsPlaying() && Aura.IsValid() &&
                          Aura->IsRegistered() && Aura->GetAsset()->IsReadyToRun() &&
                          Victim->IsGameplayCueActive(AuraTag),
                      TEXT("hit sound Niagara and active duration aura"));
            }
            Key(PC, EKeys::LeftMouseButton, false);
            Key(PC, EKeys::RightMouseButton, false);
            Next(9);
            break;
        case 9:
#if !UE_BUILD_SHIPPING
            if (Elapsed > 0.6 && !ScreenshotRequested)
            {
                FScreenshotRequest::RequestScreenshot(Screenshot, false, false);
                ScreenshotRequested = true;
            }
#endif
            if (Victim->IsGameplayCueActive(AuraTag))
                break;
            Check(Health() == 99 && (!Aura.IsValid() || Aura->IsBeingDestroyed()),
                  TEXT("native effect expiry calls Removed and destroys aura"));
#if !UE_BUILD_SHIPPING
            Check(IFileManager::Get().FileExists(*Screenshot), TEXT("packaged viewport screenshot saved"));
#endif
            Stop(PC);
            break;
        }

        return true;
    }
};
}
void StartDasGASPlaygroundValidation(UWorld* World)
{
    auto Probe = MakeShared<FDasGASPackagedProbe>(World);
    FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateLambda([Probe](float Delta) { return Probe->Tick(Delta); }));
}
#endif
