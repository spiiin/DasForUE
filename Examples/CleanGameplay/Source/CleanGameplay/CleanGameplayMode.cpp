#include "CleanGameplayMode.h"
#include "DasActor.h"
#include "UnrealClient.h"
#include "HAL/FileManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Camera/CameraActor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "InputCoreTypes.h"
#include "UObject/UnrealType.h"
#if !defined(DAS_WITH_TEST_PROBES) || DAS_WITH_TEST_PROBES
#error CleanGameplay must never compile with plugin test fixtures.
#endif
ACleanGameplayMode::ACleanGameplayMode()
{
    PrimaryActorTick.bCanEverTick = true;
    DefaultPawnClass = nullptr;
}
void ACleanGameplayMode::BeginPlay()
{
    Super::BeginPlay();
    Verify = FParse::Param(FCommandLine::Get(), TEXT("CleanGameplayVerify"));
    Started = FPlatformTime::Seconds();
    auto* Type = LoadClass<ADasActor>(nullptr, TEXT("/Game/CleanGameplay/BP_Collectible.BP_Collectible_C"));
    if (!Type || FindObject<UClass>(nullptr, TEXT("/Script/DasRuntime.DasProbeActor")))
    {
        Finish(false);
        return;
    }
    Collectible = GetWorld()->SpawnActor<ADasActor>(Type);
    auto* Camera = GetWorld()->SpawnActor<ACameraActor>(FVector(0, -350, 180), FRotator(-27, 90, 0));
    if (auto* PC = GetWorld()->GetFirstPlayerController())
        PC->SetViewTarget(Camera);
    auto* Light = GetWorld()->SpawnActor<ADirectionalLight>(FVector(0, 0, 200), FRotator(-50, -40, 0));
    if (Light)
        CastChecked<UDirectionalLightComponent>(Light->GetLightComponent())->SetIntensity(4.0f);
}
void ACleanGameplayMode::Finish(bool Good)
{
    UE_LOG(LogTemp, Display, TEXT("DAS_CLEAN_GAMEPLAY=%s"), Good ? TEXT("PASS") : TEXT("FAIL"));
    if (Verify && Good)
    {
        FString Screenshot;
        if (FParse::Value(FCommandLine::Get(), TEXT("CleanGameplayScreenshot="), Screenshot))
        {
            if (!ScreenshotRequested)
            {
                FScreenshotRequest::RequestScreenshot(Screenshot, false, false);
                ScreenshotRequested = true;
                return;
            }
            if (IFileManager::Get().FileSize(*Screenshot) <= 0 && FPlatformTime::Seconds() - Started < 30)
                return;
            Good = IFileManager::Get().FileSize(*Screenshot) > 0;
        }
    }
    if (Verify)
        FPlatformMisc::RequestExitWithStatus(false, Good ? 0 : 1);
}
void ACleanGameplayMode::Tick(float Delta)
{
    Super::Tick(Delta);
    auto* A = Collectible.Get();
    if (!A)
    {
        if (Verify)
            Finish(false);
        return;
    }
    auto ReadInt = [A](const TCHAR* Name)
    {
        auto* P = FindFProperty<FIntProperty>(A->GetClass(), Name);
        return P ? P->GetPropertyValue_InContainer(A) : -1;
    };
    auto* Flag = FindFProperty<FBoolProperty>(A->GetClass(), TEXT("Available"));
    bool Available = Flag && Flag->GetPropertyValue_InContainer(A);
    auto* PC = GetWorld()->GetFirstPlayerController();
    if (Available && ((Verify && Collections < 2) || (PC && PC->WasInputKeyJustPressed(EKeys::SpaceBar))))
    {
        auto* Collect = A->FindFunction(TEXT("Collect"));
        if (!Collect)
        {
            Finish(false);
            return;
        }
        A->ProcessEvent(Collect, nullptr);
        ++Collections;
    }
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Yellow,
                                         FString::Printf(TEXT("SPACE: collect | Score: %d | Events: %d"),
                                                              ReadInt(TEXT("Score")), ReadInt(TEXT("Events"))));
    if (Verify && Collections == 2 && Available && ReadInt(TEXT("Restores")) == 2)
    {
        auto* Mesh = A->FindComponentByClass<UStaticMeshComponent>();
        Finish(ReadInt(TEXT("Score")) == 20 &&
                       ReadInt(TEXT("Events")) == 2 &&
                               ReadInt(TEXT("Begins")) == 1 &&
                                       ReadInt(TEXT("Loads")) == 1 &&
                                               ReadInt(TEXT("Ticks")) > 1 && Mesh && Mesh->GetStaticMesh() &&
                                                       Mesh->GetAttachParent() == A->GetRootComponent() &&
                                                       A->LastDasError.IsEmpty());
    }
    else if (Verify && FPlatformTime::Seconds() - Started > 30)
        Finish(false);
}
