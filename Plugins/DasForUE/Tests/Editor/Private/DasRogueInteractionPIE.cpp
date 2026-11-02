#include "DasDSLService.h"
#include "UObject/StructOnScope.h"
#include <limits>
#include "DasInterfaceService.h"
#include "DasPlayerController.h"
#include "DasRuntime.h"
#include "DasCollisionTypes.h"
#include "DasCallFrame.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Engine/HitResult.h"
#include "UObject/UObjectIterator.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/DefaultPawn.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/WorldSettings.h"
#include "InputAction.h"
#include "Engine/LocalPlayer.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerInput.h"
#include "InputMappingContext.h"
#include "InputKeyEventArgs.h"
#include "GenericPlatform/GenericPlatformInputDeviceMapper.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Misc/AutomationTest.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Tests/AutomationEditorCommon.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

namespace
{
UObject* RogueObject(UObject* O, const TCHAR* N)
{
    auto* P = O ? FindFProperty<FObjectPropertyBase>(O->GetClass(), N) : nullptr;
    return P ? P->GetObjectPropertyValue_InContainer(O) : nullptr;
}
int32 RogueInt(UObject* O, const TCHAR* N)
{
    auto* P = O ? FindFProperty<FIntProperty>(O->GetClass(), N) : nullptr;
    return P ? P->GetPropertyValue_InContainer(O) : -1;
}
UObject* RogueAttack(APlayerController* PC)
{
    if (!PC || !PC->GetPawn())
        return nullptr;
    for (auto* Component : PC->GetPawn()->GetComponents())
    {
        auto* P = FindFProperty<FArrayProperty>(Component->GetClass(), TEXT("Actions"));
        auto* Inner = P ? CastField<FObjectPropertyBase>(P->Inner) : nullptr;
        if (!Inner)
            continue;
        FScriptArrayHelper Values(P, P->ContainerPtrToValuePtr<void>(Component));
        for (int32 Index = 0; Index < Values.Num(); ++Index)
        {
            auto* Action = Inner->GetObjectPropertyValue(Values.GetRawPtr(Index));
            if (Action && FindFProperty<FIntProperty>(Action->GetClass(), TEXT("ProjectilesFired")))
                return Action;
        }
    }
    return nullptr;
}
float RoguePitch(UObject* O)
{
    auto* P = O ? FindFProperty<FFloatProperty>(O->GetClass(), TEXT("CurrentAnimationPitch")) : nullptr;
    return P ? P->GetPropertyValue_InContainer(O) : -1;
}
struct FRogueInputMap
{
    TArray<TStrongObjectPtr<UObject>> Keep;
    UClass* ActionSystemClass = nullptr;
    UClass* AttackClass = nullptr;
    bool Prepare(FString& Error, bool ProjectileMode = false)
    {
        TArray<UBlueprint*> Interfaces;
        if (!FDasInterfaceService::Apply(TEXT("rogue_interaction_interface.das"), TEXT("/Game/DasRogue/Types"),
                                                                                       Interfaces, Error))
            return false;
        if (!FDasInterfaceService::Apply(TEXT("rogue_projectile_impact_interface.das"), TEXT("/Game/DasRogue/Types"),
                                                                                             Interfaces, Error))
            return false;
        if (!FDasInterfaceService::Apply(TEXT("rogue_action_host_interface.das"), TEXT("/Game/DasRogue/Types"),
                                                                                       Interfaces, Error))
            return false;
        auto* Action = LoadObject<UInputAction>(nullptr, TEXT("/Game/DasRogue/Input/IA_Interact.IA_Interact"), nullptr,
                                                              LOAD_NoWarn);
        if (!Action)
            Action = NewObject<UInputAction>(CreatePackage(TEXT("/Game/DasRogue/Input/IA_Interact")),
                                                           TEXT("IA_Interact"), RF_Public | RF_Standalone);
        Action->ValueType = EInputActionValueType::Boolean;
        auto* Mapping = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/DasRogue/Input/IMC_Rogue.IMC_Rogue"),
                                                                       nullptr, LOAD_NoWarn);
        if (!Mapping)
            Mapping = NewObject<UInputMappingContext>(CreatePackage(TEXT("/Game/DasRogue/Input/IMC_Rogue")),
                                                                    TEXT("IMC_Rogue"), RF_Public | RF_Standalone);
        Mapping->UnmapAll();
        Mapping->MapKey(Action, EKeys::E);
        auto* Fire =
            LoadObject<UInputAction>(nullptr, TEXT("/Game/DasRogue/Input/IA_Fire.IA_Fire"), nullptr, LOAD_NoWarn);
        if (!Fire)
            Fire = NewObject<UInputAction>(CreatePackage(TEXT("/Game/DasRogue/Input/IA_Fire")),
                                                         TEXT("IA_Fire"), RF_Public | RF_Standalone);
        Fire->ValueType = EInputActionValueType::Boolean;
        Mapping->MapKey(Fire, EKeys::LeftMouseButton);
        Keep.Emplace(Fire);
        Keep.Emplace(Action);
        Keep.Emplace(Mapping);
        UBlueprint *Chest = nullptr, *Component = nullptr, *Controller = nullptr, *Projectile = nullptr,
                   *Demo = nullptr;
        if (!FDasDSLService::Apply(TEXT("rogue_item_chest.das"),
                TEXT("/Game/DasRogue/BP_RogueItemChest"), TEXT("rogue.RogueItemChest"), Chest, Error) ||
                     !FDasDSLService::Apply(TEXT("rogue_interaction_component.das"),
                                                 TEXT("/Game/DasRogue/BP_RogueInteractionComponent"),
                                                      TEXT("rogue.RogueInteractionComponent"), Component, Error) ||
                                                      !FDasDSLService::Apply(
                                                          TEXT("rogue_projectile.das"),
                                                               TEXT("/Game/DasRogue/BP_RogueProjectile"),
                                                                    TEXT("rogue.RogueProjectile"), Projectile, Error) ||
                                                                    !FDasDSLService::Apply(
                                                                        TEXT("rogue_projectile_demo.das"),
                                                                             TEXT("/Game/DasRogue/BP_ProjectileDemo"),
                                                                                  TEXT("roguedemo.RogueProjectileDemo"),
                                                                                       Demo, Error))
            return false;
        struct FActionSource
        {
            const TCHAR* Script;
            const TCHAR* Asset;
            const TCHAR* Type;
        };
        for (const auto& Source : {FActionSource{
                 TEXT("rogue_action.das"), TEXT("/Game/DasRogue/Actions/BP_RogueAction"), TEXT("rogue.RogueAction")},
                      FActionSource{TEXT("rogue_action_projectile_attack.das"),
                                         TEXT("/Game/DasRogue/Actions/BP_RogueActionProjectileAttack"),
                                              TEXT("rogue.RogueActionProjectileAttack")},
                                         FActionSource{TEXT("rogue_action_system_component.das"),
                                                            TEXT("/Game/DasRogue/BP_RogueActionSystemComponent"),
                                                                 TEXT("rogue.RogueActionSystemComponent")}})
        {
            UBlueprint* BP = nullptr;
            if (!FDasDSLService::Apply(Source.Script, Source.Asset, Source.Type, BP, Error))
                return false;
            Keep.Emplace(BP);
            if (FString(Source.Type).EndsWith(TEXT("SystemComponent")))
                ActionSystemClass = BP->GeneratedClass;
            if (FString(Source.Type).EndsWith(TEXT("ProjectileAttack")))
                AttackClass = BP->GeneratedClass;
        }
        if (!FDasDSLService::Apply(TEXT("rogue_player_controller.das"),
                                        TEXT("/Game/DasRogue/BP_RoguePlayerController"),
                                             TEXT("rogue.RoguePlayerController"), Controller, Error,
                                                  EDasDefaultMigrationPolicy::Automatic))
            return false;
        if (auto* Effect =
                Cast<UNiagaraSystem>(RogueObject(Demo->GeneratedClass->GetDefaultObject(), TEXT("ExplosionEffect"))))
            Effect->WaitForCompilationComplete(true, false);
        for (auto* B : {Chest, Component, Controller, Projectile, Demo})
            Keep.Emplace(B);
        auto* GameBP = FKismetEditorUtilities::CreateBlueprint(
            AGameModeBase::StaticClass(),
            CreatePackage(*(TEXT("/Game/DasRogue/TestGameMode_") + FGuid::NewGuid().ToString(EGuidFormats::Digits))),
                          TEXT("BP_TestGameMode"), BPTYPE_Normal, UBlueprint::StaticClass(),
                               UBlueprintGeneratedClass::StaticClass());
        if (!GameBP || !GameBP->GeneratedClass)
        {
            Error = TEXT("Cannot configure test GameMode");
            return false;
        }
        Keep.Emplace(GameBP);
        auto* GameMode = CastChecked<AGameModeBase>(GameBP->GeneratedClass->GetDefaultObject());
        GameMode->PlayerControllerClass = Controller->GeneratedClass;
        GameMode->DefaultPawnClass = ADefaultPawn::StaticClass();
        FBlueprintEditorUtils::MarkBlueprintAsModified(GameBP);
        FKismetEditorUtilities::CompileBlueprint(GameBP);
        UE_LOG(LogTemp, Display,
               TEXT("DAS_ROGUE_MODE pc=%s expected=%s"),
                    *GetNameSafe(
                        CastChecked<AGameModeBase>(GameBP->GeneratedClass->GetDefaultObject())->PlayerControllerClass),
                    *GetNameSafe(Controller->GeneratedClass));
        auto* W = FAutomationEditorCommonUtils::CreateNewMap();
        W->GetWorldSettings()->DefaultGameMode = GameBP->GeneratedClass;
        W->SpawnActor<APlayerStart>(FVector(-300, 0, 120), FRotator::ZeroRotator);
        auto* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
        auto SpawnChest = [&](const TCHAR* Name, FVector Position)
        {
            FActorSpawnParameters P;
            P.Name = Name;
            auto* A = W->SpawnActor<AActor>(Chest->GeneratedClass, FTransform(Position), P);
            auto* Base = CastChecked<UStaticMeshComponent>(RogueObject(A, TEXT("BaseMeshComponent")));
            Base->SetStaticMesh(Cube);
            Base->SetCollisionProfileName(TEXT("Interaction"));
            return A;
        };
        SpawnChest(TEXT("Rogue_Center"), FVector(0, 0, 70));
        SpawnChest(TEXT("Rogue_Side"), FVector(80, 180, 70));
        auto* Invalid = W->SpawnActor<AStaticMeshActor>(FVector(-150, 0, 120), FRotator::ZeroRotator);
        Invalid->GetStaticMeshComponent()->SetStaticMesh(Cube);
        Invalid->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("Interaction"));
        if (ProjectileMode)
        {
            for (TActorIterator<AActor> I(W); I; ++I)
                if (I->GetFName() == TEXT("Rogue_Center") || I->GetFName() == TEXT("Rogue_Side"))
                    I->SetActorLocation(FVector(0, 600, 70));
            // Remove the interaction-only non-interface obstacle from the firing lane.
            for (TActorIterator<AStaticMeshActor> I(W); I; ++I)
                I->SetActorLocation(FVector(0, 900, 0));
            auto* Wall = W->SpawnActor<AStaticMeshActor>(FVector(1600, 0, 120), FRotator::ZeroRotator);
            Wall->SetActorScale3D(FVector(1, 8, 8));
            Wall->GetStaticMeshComponent()->SetStaticMesh(
                LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")));
            Wall->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
        }
        return true;
    }
};
class FRogueInteractionPIE : public IAutomationLatentCommand
{
    TSharedRef<FRogueInputMap> Map;
    FAutomationTestBase* Test;
    int32 Phase = 0;
    double Began = FPlatformTime::Seconds(), Changed = Began, GameChanged = 0;
    TWeakObjectPtr<AActor> First, Second;
    double LastDiagnostic = 0;
    void Key(APlayerController* PC, bool Down)
    {
        PC->InputKey(FInputKeyEventArgs(nullptr, IPlatformInputDeviceMapper::Get().GetDefaultInputDevice(), EKeys::E,
                                        Down ? IE_Pressed : IE_Released, Down ? 1.f : 0.f, false,
                                        FPlatformTime::Cycles64()));
    }
    void Next(int32 P)
    {
        Phase = P;
        Changed = FPlatformTime::Seconds();
        if (auto* W = GEditor->PlayWorld.Get())
            GameChanged = W->GetTimeSeconds();
    }
    void Check(bool OK, const TCHAR* Name) { Test->TestTrue(Name, OK); }
    void Stop(APlayerController* PC)
    {
        if (PC)
            Key(PC, false);
        GEditor->RequestEndPlayMap();
        Next(99);
    }

public:
    FRogueInteractionPIE(TSharedRef<FRogueInputMap> M, FAutomationTestBase* T) : Map(M), Test(T) {}
    bool Update() override
    {
        auto* W = GEditor->PlayWorld.Get();
        auto* PC = W ? W->GetFirstPlayerController() : nullptr;
        if (Phase == 99)
        {
            if (W && FPlatformTime::Seconds() - Changed < 30)
                return false;
            Check(!W && FModuleManager::LoadModuleChecked<FDasRuntimeModule>(
                            TEXT("DasRuntime")).PersistentContextCount() == 0,
                            TEXT("EndPIE releases controller and component sessions"));
            return true;
        }
        if (FPlatformTime::Seconds() - Began > 100)
        {
            Test->AddError(FString::Printf(TEXT("Rogue interaction timeout phase=%d"), Phase));
            Stop(PC);
            return false;
        }
        if (!W || !W->HasBegunPlay() || !PC)
            return false;
        auto* Ready = FindFProperty<FBoolProperty>(PC->GetClass(), TEXT("ControlsBound"));
        auto* Component = RogueObject(PC, TEXT("InteractionComponent"));
        if (!Ready || !Ready->GetPropertyValue_InContainer(PC) || !Component)
        {
            if (FPlatformTime::Seconds() - LastDiagnostic > 10)
            {
                LastDiagnostic = FPlatformTime::Seconds();
                auto* Host = Cast<ADasPlayerController>(PC);
                UE_LOG(
                    LogTemp, Display,
                    TEXT("DAS_ROGUE_INPUT_WAIT pc=%s local=%d player=%s input=%s playerInput=%s component=%s context=%s action=%s lifecycle=%s error=%s"),
                        *GetNameSafe(PC->GetClass()), PC->IsLocalController(), *GetNameSafe(PC->GetLocalPlayer()),
                        *GetNameSafe(PC->InputComponent ? PC->InputComponent->GetClass() : nullptr),
                        *GetNameSafe(PC->PlayerInput ? PC->PlayerInput->GetClass() : nullptr), *GetNameSafe(Component),
                        *GetNameSafe(RogueObject(PC, TEXT("Controls"))),
                                     *GetNameSafe(RogueObject(PC, TEXT("Input_Interact"))),
                                                  Host ? *Host->LifecycleScript
                                                       : TEXT("none"), Host ? *Host->LastDasError : TEXT("none"));
            }
            return false;
        }
        if (Phase == 0 && !First.IsValid())
        {
            for (TActorIterator<AActor> I(W); I; ++I)
            {
                if (I->GetFName() == TEXT("Rogue_Center"))
                    First = *I;
                if (I->GetFName() == TEXT("Rogue_Side"))
                    Second = *I;
            }
            if (!First.IsValid() || !Second.IsValid())
                return false;
            GameChanged = W->GetTimeSeconds();
        }
        const double Elapsed = W->GetTimeSeconds() - GameChanged;
        switch (Phase)
        {
        case 0:
            if (Elapsed < 1.5)
                break;
            Check(PC->IsA<ADasPlayerController>(), TEXT("GameMode uses authored PlayerController host"));
            Check(Cast<IGenericTeamAgentInterface>(PC)->GetGenericTeamId() == FGenericTeamId(1),
                  TEXT("Team selected by daScript"));
            Check(RoguePitch(First.Get()) == 0 && RogueInt(PC, TEXT("InteractPresses")) == 0,
                                                           TEXT("No automatic opening"));
            Check(RogueObject(Component, TEXT("SelectedActor")) == First.Get(),
                              TEXT("Distance and direction select center chest"));
            Check(RogueInt(Component, TEXT("CandidateCount")) == 2, TEXT("Non-interface overlap is ignored"));
            Key(PC, true);
            Next(1);
            break;
        case 1:
            if (Elapsed < 0.5)
                break;
            Check(
                RogueInt(PC, TEXT("InteractPresses")) == 1 &&
                         RogueInt(PC, TEXT("SuccessfulInteractions")) == 1,
                                  TEXT("Mapped E reaches daScript controller component and interface once while held"));
            Check(RoguePitch(First.Get()) > 0 && RoguePitch(First.Get()) < 120 && RoguePitch(Second.Get()) == 0,
                  TEXT("Only selected chest animates"));
            Key(PC, false);
            Next(2);
            break;
        case 2:
            if (Elapsed < 2.5)
                break;
            Check(RoguePitch(First.Get()) == 120 && !First->IsActorTickEnabled(),
                  TEXT("Chest completes and stops ticking"));
            PC->GetPawn()->SetActorLocation(FVector(-2000, 0, 120));
            Next(3);
            break;
        case 3:
            if (Elapsed < 0.2)
                break;
            Check(!RogueObject(Component, TEXT("SelectedActor")), TEXT("Leaving radius clears selection"));
            Key(PC, true);
            Next(4);
            break;
        case 4:
            if (Elapsed < 0.2)
                break;
            Check(RogueInt(PC, TEXT("InteractPresses")) == 2 && RogueInt(PC, TEXT("SuccessfulInteractions")) == 1,
                                                                         TEXT("E outside range does not interact"));
            Key(PC, false);
            PC->GetPawn()->SetActorLocation(FVector(-300, 0, 120));
            Next(5);
            break;
        case 5:
            if (Elapsed < 0.2)
                break;
            First->Destroy();
            Key(PC, true);
            Next(6);
            break;
        case 6:
            if (Elapsed < 0.4)
                break;
            Check(RogueInt(PC, TEXT("SuccessfulInteractions")) == 2 && RoguePitch(Second.Get()) > 0,
                           TEXT("Destroyed target is discarded and remaining chest opens"));
            Key(PC, false);
            PC->UnPossess();
            Next(7);
            break;
        case 7:
            if (Elapsed < 0.2)
                break;
            Check(!RogueObject(Component, TEXT("SelectedActor")), TEXT("No possessed pawn safely clears selection"));
            Check(CastChecked<ADasPlayerController>(PC)->LastDasError.IsEmpty(), TEXT("No controller runtime errors"));
            Stop(PC);
            break;
        }
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasRogueInteractionTest, "DasForUE.Ports.RogueInteractionPIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasRogueInteractionTest::RunTest(const FString&)
{
    if (!FParse::Param(FCommandLine::Get(), TEXT("DasRoguePort")))
    {
        AddInfo(TEXT("External port is opt-in; use Scripts/TestRogueInteraction.ps1."));
        return true;
    }
    auto Map = MakeShared<FRogueInputMap>();
    FString Error;
    if (!Map->Prepare(Error))
    {
        AddError(Error);
        return false;
    }
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FRogueInteractionPIE>(Map, this));
    return true;
}

namespace
{
struct FRogueProjectilePIE : IAutomationLatentCommand
{
    TSharedPtr<FRogueInputMap> Map;
    FAutomationTestBase* Test;
    double Started = FPlatformTime::Seconds(), Changed = 0;
    int Phase = 0;
    TWeakObjectPtr<AActor> Shot;
    FVector Earlier;
    FRogueProjectilePIE(TSharedPtr<FRogueInputMap> InMap, FAutomationTestBase* InTest) : Map(InMap), Test(InTest) {}
    void Check(bool Good, const TCHAR* Message) { Test->TestTrue(Message, Good); }
    void Key(APlayerController* PC, bool Down)
    {
        PC->InputKey(FInputKeyEventArgs(nullptr, IPlatformInputDeviceMapper::Get().GetDefaultInputDevice(),
                                        EKeys::LeftMouseButton, Down ? IE_Pressed : IE_Released, Down ? 1.f : 0.f,
                                        false, FPlatformTime::Cycles64()));
    }
    bool Update() override
    {
        if (FPlatformTime::Seconds() - Started > 90)
        {
            Test->AddError(TEXT("Projectile PIE timed out"));
            GEditor->RequestEndPlayMap();
            return true;
        }
        UWorld* W = GEditor->PlayWorld.Get();
        if (Phase == 9)
        {
            if (W)
                return false;
            Check(FModuleManager::LoadModuleChecked<FDasRuntimeModule>("DasRuntime").PersistentContextCount() == 0,
                  TEXT("Projectile EndPIE releases contexts"));
            return true;
        }
        auto* PC = W ? W->GetFirstPlayerController() : nullptr;
        auto* Host = Cast<ADasPlayerController>(PC);
        if (Host && !Host->LastDasError.IsEmpty())
        {
            Test->AddError(Host->LastDasError);
            GEditor->RequestEndPlayMap();
            Phase = 9;
            return false;
        }
        auto* Bound = PC ? FindFProperty<FBoolProperty>(PC->GetClass(), TEXT("FireBound")) : nullptr;
        if (!PC || !PC->GetPawn() || !Bound || !Bound->GetPropertyValue_InContainer(PC))
            return false;
        double Time = W->GetTimeSeconds();
        if (Phase == 0)
        {
            auto* Actions = NewObject<UActorComponent>(PC->GetPawn(), Map->ActionSystemClass, TEXT("ActionSystem"));
            PC->GetPawn()->AddInstanceComponent(Actions);
            Actions->RegisterComponent();
            auto* Grant = Actions->FindFunction(TEXT("GrantAction"));
            if (!Grant)
            {
                Test->AddError(TEXT("Missing GrantAction"));
                GEditor->RequestEndPlayMap();
                Phase = 9;
                return false;
            }
            FStructOnScope Args(Grant);
            auto* Kind = FindFProperty<FClassProperty>(Grant, TEXT("kind"));
            if (!Kind)
            {
                Test->AddError(TEXT("Missing action class argument"));
                GEditor->RequestEndPlayMap();
                Phase = 9;
                return false;
            }
            Kind->SetObjectPropertyValue_InContainer(Args.GetStructMemory(), Map->AttackClass);
            Actions->ProcessEvent(Grant, Args.GetStructMemory());
            Check(RogueInt(RogueAttack(PC), TEXT("ProjectilesFired")) == 0, TEXT("No automatic projectile"));
            Key(PC, true);
            Changed = Time;
            Phase = 1;
            return false;
        }
        if (Phase == 1 && Time - Changed > 0.15)
        {
            // Input dispatch and the script timer callback are distinct world ticks.
            if (RogueInt(RogueAttack(PC), TEXT("ProjectilesFired")) == 0 && Time - Changed < 1.0)
                return false;

            Check(RogueInt(RogueAttack(PC), TEXT("ProjectilesFired")) == 1,
                           TEXT("Held LMB fires once via Enhanced Input"));
            Shot = Cast<AActor>(RogueObject(RogueAttack(PC), TEXT("LastProjectile")));
            if (!Shot.IsValid())
            {
                Test->AddError(TEXT("LMB did not create projectile"));
                GEditor->RequestEndPlayMap();
                Phase = 9;
                return false;
            }
            auto* M = Shot->FindComponentByClass<UProjectileMovementComponent>();
            auto* S = Shot->FindComponentByClass<USphereComponent>();
            Check(M && M->ProjectileGravityScale == 0 && FMath::IsNearlyEqual(M->Velocity.Size(), 2000.),
                  TEXT("Native projectile movement: speed 2000, zero gravity"));
            Check(S && M && M->UpdatedComponent == S && S == Shot->GetRootComponent() &&
                      S->GetUnscaledSphereRadius() == 16,
                  TEXT("Sphere root is swept updated component"));
            Check(S && S->GetMoveIgnoreActors().Contains(PC->GetPawn()),
                  TEXT("Instigator excluded from swept movement"));
            Check(Shot->GetInstigator() == PC->GetPawn() && Shot->GetOwner() == PC,
                  TEXT("Instigator and owner assigned before BeginPlay"));
            Check(Shot->GetLifeSpan() > 29 && Shot->GetLifeSpan() <= 30, TEXT("Original 30-second lifetime"));
            Check(RogueObject(Shot.Get(), TEXT("LoopedNiagaraComponent")) &&
                              RogueObject(Shot.Get(), TEXT("LoopedAudioComponent")),
                                          TEXT("Loop effect components present"));
            Earlier = Shot->GetActorLocation();
            Key(PC, false);
            Changed = Time;
            Phase = 2;
            return false;
        }
        if (Phase == 2 && Time - Changed > 0.1)
        {
            Check(Shot.IsValid() && Shot->GetActorLocation().X > Earlier.X + 100,
                  TEXT("Projectile moves forward through real world ticks"));
            Check(Shot.IsValid() && FMath::IsNearlyEqual(Shot->GetActorLocation().Z, Earlier.Z, 0.01),
                  TEXT("Trajectory has no gravity"));
            Changed = Time;
            Phase = 3;
            return false;
        }
        if (Phase == 3 && (!Shot.IsValid() || Time - Changed > 2.0))
        {
            bool BurstSurvived = false;
            for (TObjectIterator<UNiagaraComponent> I; I; ++I)
                if (!I->IsTemplate() && I->GetWorld() == W && I->GetAsset() &&
                    I->GetAsset()->GetName() == TEXT("RadialBurst") && I->IsActive())
                    BurstSurvived = true;
            Check(BurstSurvived, TEXT("World-owned Niagara burst survives projectile Destroy"));
            Check(!Shot.IsValid(), TEXT("Swept collision invokes daScript OnComponentHit and destroys projectile"));
            // Exercise lifetime independent of impact, firing away from the wall.
            PC->SetControlRotation(FRotator(0, 180, 0));
            Key(PC, true);
            Changed = Time;
            Phase = 4;
            return false;
        }
        if (Phase == 4 && Time - Changed > 0.1)
        {
            if (RogueInt(RogueAttack(PC), TEXT("ProjectilesFired")) < 2 && Time - Changed < 1.0)
                return false;

            Key(PC, false);
            Shot = Cast<AActor>(RogueObject(RogueAttack(PC), TEXT("LastProjectile")));
            Check(RogueInt(RogueAttack(PC), TEXT("ProjectilesFired")) == 2 && Shot.IsValid(),
                           TEXT("Second physical press creates independent projectile"));
            if (Shot.IsValid())
                Shot->SetLifeSpan(0.15f);
            Changed = Time;
            Phase = 5;
            return false;
        }
        if (Phase == 5 && Time - Changed > 0.3)
        {
            Check(!Shot.IsValid(), TEXT("Lifetime expires without impact"));
            Check(Host && Host->LastDasError.IsEmpty(), TEXT("Controller has no daScript errors"));
            GEditor->RequestEndPlayMap();
            Phase = 9;
        }
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasRogueProjectileTest, "DasForUE.Ports.RogueProjectilePIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasRogueProjectileTest::RunTest(const FString&)
{
    if (!FParse::Param(FCommandLine::Get(), TEXT("DasRoguePort")))
    {
        AddInfo(TEXT("External port is opt-in."));
        return true;
    }
    auto Map = MakeShared<FRogueInputMap>();
    FString Error;
    if (!Map->Prepare(Error, true))
    {
        AddError(Error);
        return false;
    }
    auto* Delegate = FindFProperty<FMulticastDelegateProperty>(USphereComponent::StaticClass(), TEXT("OnComponentHit"));
    auto* HitProperty = Delegate ? FindFProperty<FStructProperty>(Delegate->SignatureFunction, TEXT("Hit")) : nullptr;
    FHitResult Hit;
    Hit.bBlockingHit = true;
    Hit.Time = 0.25f;
    Hit.Distance = 123;
    Hit.ImpactPoint = FVector(1, 2, 3);
    Hit.Normal = FVector(0, 0, 1);
    FDasScalarValue Snapshot;
    if (!TestTrue(
            TEXT("Native HitResult projects into owned callback value"),
                 ReadDasCollisionHitCallback(HitProperty, &Hit, FDasCollisionHit::StaticStruct(), Snapshot, Error)))
        return false;
    Hit.ImpactPoint = FVector(100, 200, 300);
    const auto& Copy = Snapshot.StructValue.Get<FDasCollisionHit>();
    TestTrue(TEXT("Collision callback is detached and retains finite hit data"),
                  Copy.BlockingHit && Copy.Time == 0.25f && Copy.Distance == 123 &&
                      Copy.ImpactPoint == FVector(1, 2, 3) && Copy.Normal == FVector(0, 0, 1));
    Hit.Time = std::numeric_limits<float>::infinity();
    TestFalse(TEXT("Nonfinite collision payload rejected"),
                   ReadDasCollisionHitCallback(HitProperty, &Hit, FDasCollisionHit::StaticStruct(), Snapshot, Error));
    TestFalse(TEXT("Wrong callback projection rejected"),
                   CanProjectDasCollisionHit(HitProperty, TBaseStructure<FVector>::Get()));
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FRogueProjectilePIE>(Map, this));
    return true;
}
