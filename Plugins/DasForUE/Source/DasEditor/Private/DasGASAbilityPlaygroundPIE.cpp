#include "DasDSLService.h"
#include "DasGASAssetService.h"
#include "DasGASCueExampleAssets.h"
#include "DasGASAuthoring.h"
#include "DasGASPlaygroundGame.h"
#include "DasRuntime.h"
#include "AbilitySystemGlobals.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "EngineUtils.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/World.h"
#include "DasGASAbility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/WorldSettings.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "InputKeyEventArgs.h"
#include "GenericPlatform/GenericPlatformInputDeviceMapper.h"
#include "NiagaraComponent.h"
#include "FileHelpers.h"
#include "Misc/Paths.h"
#include "GameplayAbilitiesDeveloperSettings.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "UnrealClient.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
namespace
{
bool SetObject(UObject* O, const TCHAR* N, UObject* V)
{
    auto* P = O ? FindFProperty<FObjectPropertyBase>(O->GetClass(), N) : nullptr;
    if (!P)
        return false;
    P->SetObjectPropertyValue_InContainer(O, V);
    O->MarkPackageDirty();
    return true;
}
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
struct FPlaygroundMap
{
    TArray<TStrongObjectPtr<UBlueprint>> Assets;
    FString Screenshot =
        FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../../logs/gas-ability-pie.png"));
    bool Prepare(FString& Error)
    {
        const FString Root = TEXT("/Game/DasExamples/GASCues");
        // Configure this isolated test process before the first GAS singleton initialization.
        GetMutableDefault<UGameplayAbilitiesDeveloperSettings>()->GameplayCueNotifyPaths.AddUnique(Root);
        UAbilitySystemGlobals::Get().AddGameplayCueNotifyPath(Root);
        if (!PrepareDasCueExampleSystem(Root, TEXT("RadialBurst"), TEXT("NS_Impact")) ||
                                                   !PrepareDasCueExampleSystem(Root, TEXT("FountainLightweight"),
                                                                                          TEXT("NS_Aura")))
        {
            Error = TEXT("Cannot prepare Niagara example assets");
            return false;
        }
        // Older demo preparation put input assets in CDO fields absent from the DSL defaults.
        // Normalize only this generated example Blueprint; keep map-specific values on instances.
        if (auto* Existing = LoadObject<UBlueprint>(
                nullptr, *(Root + TEXT("/BP_GASPlaygroundPlayer.BP_GASPlaygroundPlayer")), nullptr, LOAD_NoWarn))
        {
            if (Existing->GeneratedClass)
                for (const auto* Field :
                     {TEXT("Controls"), TEXT("MoveAction"), TEXT("LookAction"), TEXT("JumpAction"),
                                                                                     TEXT("AimAction"),
                                                                                          TEXT("ConfirmAction"),
                                                                                               TEXT("CancelAction")})
                    SetObject(Existing->GeneratedClass->GetDefaultObject(), Field, nullptr);
        }
        TMap<FString, UBlueprint*> Cues, Abilities;
        UBlueprint *Caster = nullptr, *Target = nullptr, *Player = nullptr;
        if (!FDasGASAssetService::Apply(
                TEXT("gas_cue_playground.das"), Root, Cues, Error, true) ||
                !FDasDSLService::Apply(TEXT("dsl_gas_ability_playground.das"),
                    Root + TEXT("/BP_AbilityPlayground"),
                                TEXT("dsl_gas_ability_playground.AbilityPlayground"), Caster, Error) ||
                                !FDasDSLService::Apply(TEXT("dsl_gas_ability_target.das"),
                                    Root + TEXT("/BP_AbilityTarget"),
                                                TEXT("dsl_gas_ability_target.AbilityTarget"), Target, Error) ||
                                                !FDasGASAssetService::Apply(
                                                    TEXT("gas_targeted_strike.das"), Root, Abilities, Error, true) ||
                                                    !FDasDSLService::Apply(TEXT("dsl_gas_playground_player.das"),
                                                        Root +
                                                            TEXT("/BP_GASPlaygroundPlayer"),
                                                                 TEXT("dsl_gas_playground_player.GASPlaygroundPlayer"),
                                                                      Player, Error))
            return false;
        for (auto& P : Cues)
            Assets.Emplace(P.Value);
        for (auto& P : Abilities)
            Assets.Emplace(P.Value);
        Assets.Emplace(Caster);
        Assets.Emplace(Target);
        Assets.Emplace(Player);
        auto* CD = Caster->GeneratedClass->GetDefaultObject();
        if (!SetObject(
                CD, TEXT("Attributes"), Cues.FindChecked(TEXT("PlaygroundStats"))->GeneratedClass) ||
                         !SetObject(
                             CD, TEXT("HitEffect"), Cues.FindChecked(TEXT("PlaygroundHit"))->GeneratedClass) ||
                                      !SetObject(
                                          CD, TEXT("AuraEffect"),
                                                   Cues.FindChecked(TEXT("PlaygroundMapAura"))->GeneratedClass) ||
                                                   !SetObject(
                                                       CD, TEXT("StrikeAbility"),
                                                                Abilities.FindChecked(
                                                                    TEXT("TargetedStrike"))->GeneratedClass) ||
                                                                !SetObject(
                                                                    Target->GeneratedClass->GetDefaultObject(),
                                                                    TEXT("Attributes"),
                                                                         Cues.FindChecked(
                                                                             TEXT("PlaygroundStats"))->GeneratedClass))
        {
            Error = TEXT("Cannot assign GAS class defaults");
            return false;
        }
        TMap<FString, UObject*> PlayerInput;
        auto* Mapping = NewObject<UInputMappingContext>(
            CreatePackage(*(Root + TEXT("/IMC_GASPlayground"))), TEXT("IMC_GASPlayground"), RF_Public | RF_Standalone);
        auto Action = [&](const TCHAR* Field, EInputActionValueType Type)
        {
            FString Name = FString(TEXT("IA_GAS")) + Field;
            auto* A =
                NewObject<UInputAction>(CreatePackage(*(Root + TEXT("/")+ Name)), *Name, RF_Public | RF_Standalone);
            A->ValueType = Type;
            FString Property = FString(Field) + TEXT("Action");
            PlayerInput.Add(Property, A);
            if (!SaveDasCueExampleAsset(A))
                Error = TEXT("Cannot save input action");
            return A;
        };
        auto* Move = Action(TEXT("Move"), EInputActionValueType::Axis2D);
        auto* Look = Action(TEXT("Look"), EInputActionValueType::Axis2D);
        auto* Jump = Action(TEXT("Jump"), EInputActionValueType::Boolean);
        auto* Aim = Action(TEXT("Aim"), EInputActionValueType::Boolean);
        auto* Confirm = Action(TEXT("Confirm"), EInputActionValueType::Boolean);
        auto* Cancel = Action(TEXT("Cancel"), EInputActionValueType::Boolean);
        auto Map = [&](UInputAction* A, FKey K, bool Y = false, bool Negative = false)
        {
            auto& M = Mapping->MapKey(A, K);
            if (Negative)
                M.Modifiers.Add(NewObject<UInputModifierNegate>(Mapping));
            if (Y)
            {
                auto* S = NewObject<UInputModifierSwizzleAxis>(Mapping);
                S->Order = EInputAxisSwizzle::YXZ;
                M.Modifiers.Add(S);
            }
        };
        Map(Move, EKeys::W, true);
        Map(Move, EKeys::S, true, true);
        Map(Move, EKeys::D);
        Map(Move, EKeys::A, false, true);
        Map(Look, EKeys::MouseX);
        Map(Look, EKeys::MouseY, true, true);
        Map(Jump, EKeys::SpaceBar);
        Map(Aim, EKeys::RightMouseButton);
        Map(Aim, EKeys::Q);
        Map(Confirm, EKeys::LeftMouseButton);
        Map(Confirm, EKeys::E);
        Map(Cancel, EKeys::C);
        PlayerInput.Add(TEXT("Controls"), Mapping);
        if (!Error.IsEmpty() || !SaveDasCueExampleAsset(Mapping) || !SaveDasCueExampleAsset(Caster) ||
            !SaveDasCueExampleAsset(Target) || !SaveDasCueExampleAsset(Player))
        {
            Error = TEXT("Cannot save playable input and Blueprint defaults");
            return false;
        }
        auto* W = FAutomationEditorCommonUtils::CreateNewMap();
        W->GetWorldSettings()->DefaultGameMode = ADasGASPlaygroundGameMode::StaticClass();
        W->GetWorldSettings()->bForceNoPrecomputedLighting = true;
        auto* Floor = W->SpawnActor<AStaticMeshActor>(FVector(0, 0, -50), FRotator::ZeroRotator);
        Floor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
        Floor->GetStaticMeshComponent()->SetStaticMesh(
            LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")));
        Floor->SetActorScale3D(FVector(30, 30, 1));
        Floor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
        auto* Sun = W->SpawnActor<ADirectionalLight>(FVector(0, 0, 500), FRotator(-45, -30, 0));
        Sun->GetLightComponent()->SetMobility(EComponentMobility::Movable);
        Sun->GetLightComponent()->SetIntensity(3.0f);
        CastChecked<UDirectionalLightComponent>(Sun->GetLightComponent())->SetAtmosphereSunLight(true);
        W->SpawnActor<ASkyAtmosphere>();
        auto* Sky = W->SpawnActor<ASkyLight>();
        Sky->GetLightComponent()->SetMobility(EComponentMobility::Movable);
        Sky->GetLightComponent()->SetIntensity(1.0f);
        FActorSpawnParameters P;
        P.Name = TEXT("GAS_Caster");
        auto* Source = W->SpawnActor<AActor>(Caster->GeneratedClass, FTransform(FVector(0, 0, 152)), P);
        Source->Tags.Add(TEXT("GASPlaygroundCaster"));
        W->SpawnActor<APlayerStart>(FVector(0, 0, 90), FRotator::ZeroRotator);
        P.Name = TEXT("GAS_Player");
        auto* Pawn = W->SpawnActor<ACharacter>(Player->GeneratedClass, FTransform(FVector(0, 0, 90)), P);
        Pawn->AutoPossessPlayer = EAutoReceiveInput::Player0;
        Pawn->AutoPossessAI = EAutoPossessAI::Disabled;
        Pawn->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
        if (!SetObject(Pawn, TEXT("Caster"), Source))
        {
            Error = TEXT("Cannot assign player caster");
            return false;
        }
        for (const auto& Input : PlayerInput)
            if (!SetObject(Pawn, *Input.Key, Input.Value))
            {
                Error = TEXT("Cannot assign placed player input");
                return false;
            }
        auto AddTarget = [&](const TCHAR* Name, FVector Position)
        {
            P.Name = Name;
            auto* Actor = W->SpawnActor<AActor>(Target->GeneratedClass, FTransform(Position), P);
            if (auto* Visual = Actor->FindComponentByClass<UStaticMeshComponent>())
                Visual->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere")));
        };
        AddTarget(TEXT("GAS_Target_Center"), FVector(380, 0, 152));
        AddTarget(TEXT("GAS_Target_Right"), FVector(420, 160, 152));
        if (!UEditorLoadingAndSavingUtils::SaveMap(W, Root + TEXT("/GASAbilityMap")))
        {
            Error = TEXT("Cannot save GASAbilityMap");
            return false;
        }
        IFileManager::Get().Delete(*Screenshot);
        return true;
    }
};
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
