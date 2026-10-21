#pragma once
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
#include "Misc/ScopeExit.h"
#include "Misc/Paths.h"
#include "GameplayAbilitiesDeveloperSettings.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "UnrealClient.h"

namespace DasGASPlaygroundExample
{
inline bool SetObject(UObject* O, const TCHAR* N, UObject* V)
{
    auto* P = O ? FindFProperty<FObjectPropertyBase>(O->GetClass(), N) : nullptr;
    if (!P)
        return false;
    P->SetObjectPropertyValue_InContainer(O, V);
    O->MarkPackageDirty();
    return true;
}
inline UObject* ObjectField(UObject* O, const TCHAR* N)
{
    auto* P = O ? FindFProperty<FObjectPropertyBase>(O->GetClass(), N) : nullptr;
    return P ? P->GetObjectPropertyValue_InContainer(O) : nullptr;
}
inline int32 IntField(UObject* O, const TCHAR* N)
{
    auto* P = O ? FindFProperty<FIntProperty>(O->GetClass(), N) : nullptr;
    return P ? P->GetPropertyValue_InContainer(O) : 0;
}
struct FPlaygroundMap
{
    TArray<TStrongObjectPtr<UBlueprint>> Assets;
    FString Screenshot =
        FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../../logs/gas-ability-pie.png"));
    bool Prepare(FString& Error, bool ResetScreenshot = true)
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
        const FString MappingPath = Root + TEXT("/IMC_GASPlayground");
        auto* Mapping = LoadObject<UInputMappingContext>(nullptr, *(MappingPath + TEXT(".IMC_GASPlayground")), nullptr,
                                                                    LOAD_NoWarn);
        if (!Mapping)
            Mapping = NewObject<UInputMappingContext>(CreatePackage(*MappingPath), TEXT("IMC_GASPlayground"),
                                                                                        RF_Public | RF_Standalone);
        Mapping->GetOutermost()->FullyLoad();
        Mapping->UnmapAll();
        auto Action = [&](const TCHAR* Field, EInputActionValueType Type)
        {
            FString Name = FString(TEXT("IA_GAS")) + Field;
            const FString Path = Root + TEXT("/") + Name;
            auto* A = LoadObject<UInputAction>(nullptr, *(Path + TEXT(".") + Name), nullptr, LOAD_NoWarn);
            if (!A)
                A = NewObject<UInputAction>(CreatePackage(*Path), *Name, RF_Public | RF_Standalone);
            A->GetOutermost()->FullyLoad();
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
        // Export has no editor UI/world context. PIE keeps the normal current editor map.
        const bool Headless = IsRunningCommandlet();
        UPackage* MapPackage = Headless ? CreatePackage(*(Root + TEXT("/GASAbilityMap"))) : nullptr;
        // This dedicated generated map is replaced in full, like a newly created editor map.
        if (MapPackage)
            MapPackage->SetPackageFlags(PKG_NewlyCreated);
        auto* W = Headless                       ? UWorld::CreateWorld(EWorldType::Editor, false, TEXT("GASAbilityMap"), MapPackage)
                                                 : GEditor->NewMap();
        if (!W)
        {
            Error = TEXT("Cannot create the example world");
            return false;
        }
        W->SetFlags(RF_Public | RF_Standalone);
        W->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
        ON_SCOPE_EXIT
        {
            if (Headless)
                W->DestroyWorld(false);
        };
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
        bool Saved = false;
        if (Headless)
        {
            const FString File = FPackageName::LongPackageNameToFilename(
                Root + TEXT("/GASAbilityMap"), FPackageName::GetMapPackageExtension());
            FSavePackageArgs Args;
            Args.TopLevelFlags = RF_Public | RF_Standalone;
            // All contents of this dedicated generated map have been recreated above.
            W->GetOutermost()->MarkAsFullyLoaded();
            Saved = UPackage::SavePackage(W->GetOutermost(), W, *File, Args);
        }
        else
            Saved = UEditorLoadingAndSavingUtils::SaveMap(W, Root + TEXT("/GASAbilityMap"));
        if (!Saved)
        {
            Error = TEXT("Cannot save GASAbilityMap");
            return false;
        }
        if (ResetScreenshot)
            IFileManager::Get().Delete(*Screenshot);
        return true;
    }
};
}
