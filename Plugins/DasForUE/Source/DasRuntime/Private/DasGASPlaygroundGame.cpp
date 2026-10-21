#include "DasGASPlaygroundGame.h"
#include "DasGASAuthoring.h"
#include "Engine/Canvas.h"
#include "EngineUtils.h"
#include "UObject/UnrealType.h"
#include "AbilitySystemGlobals.h"
#include "GameplayAbilitiesDeveloperSettings.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#if !UE_BUILD_SHIPPING || DAS_GAS_SHIPPING_VALIDATION
void StartDasGASPlaygroundValidation(UWorld* World);
#endif

ADasGASPlaygroundGameMode::ADasGASPlaygroundGameMode()
{
    DefaultPawnClass = nullptr; // The saved map contains the auto-possessed DAS Character.
    HUDClass = ADasGASPlaygroundHUD::StaticClass();
}
void ADasGASPlaygroundHUD::DrawHUD()
{
    Super::DrawHUD();
    if (!Canvas)
        return;
    const float X = Canvas->SizeX * 0.5f, Y = Canvas->SizeY * 0.5f;
    DrawLine(X - 8, Y, X + 8, Y, FLinearColor::White, 1.5f);
    DrawLine(X, Y - 8, X, Y + 8, FLinearColor::White, 1.5f);
    DrawText(TEXT("DAS / GAS - Targeted Strike"), FLinearColor::White, 24, 24, nullptr, 1.4f);
    DrawText(TEXT("WASD: move | Mouse: look | Space: jump"), FLinearColor::White, 24, 60);
    DrawText(TEXT("Hold RMB (or Q): aim | LMB (or E): confirm | C: cancel"), FLinearColor::White, 24, 84);
    DrawText(TEXT("Hit: sound + burst | Aura: 5 seconds, removed by native GameplayEffect expiry"), FLinearColor::White,
                  24, 108);
    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        auto* Caster = *It;
        if (!Caster->ActorHasTag(TEXT("GASPlaygroundCaster")))
            continue;
        auto Object = [&](const TCHAR* Name)
        {
            auto* P = FindFProperty<FObjectPropertyBase>(Caster->GetClass(), Name);
            return P ? Cast<AActor>(P->GetObjectPropertyValue_InContainer(Caster)) : nullptr;
        };
        auto* Target = Object(TEXT("PreviewTarget"));
        const bool Aiming = Target != nullptr;
        if (!Target)
            Target = Object(TEXT("LastTarget"));
        const auto* Shot = FindFProperty<FIntProperty>(Caster->GetClass(), TEXT("Shots"));
        const int32 Shots = Shot ? Shot->GetPropertyValue_InContainer(Caster) : 0;
        FString Status =
            FString::Printf(TEXT("%s | Shots: %d"), Aiming ? TEXT("Target selected") : TEXT("Ready - hold RMB"), Shots);
        if (Target)
            if (auto* ASC = Target->FindComponentByClass<UDasAuthoredAbilitySystemComponent>())
                Status += FString::Printf(TEXT(" | Target health: %.0f"),
                                               ASC->ReadAuthoredAttribute(TEXT("PlaygroundStats"), TEXT("Health")));
        DrawText(Status, Aiming ? FLinearColor(0.2f, 1.f, 0.5f) : FLinearColor::White, 24, 144, nullptr, 1.2f);
        break;
    }
}

void ADasGASPlaygroundGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    // The example's cue assets must be discoverable in both editor and cooked games.
    GetMutableDefault<UGameplayAbilitiesDeveloperSettings>()->GameplayCueNotifyPaths.AddUnique(
        TEXT("/Game/DasExamples/GASCues"));
    UAbilitySystemGlobals::Get().AddGameplayCueNotifyPath(TEXT("/Game/DasExamples/GASCues"));
    Super::InitGame(MapName, Options, ErrorMessage);
}
void ADasGASPlaygroundGameMode::BeginPlay()
{
    Super::BeginPlay();
#if !UE_BUILD_SHIPPING || DAS_GAS_SHIPPING_VALIDATION
    if (FParse::Param(FCommandLine::Get(), TEXT("DasGASAbilityPackaged")))
        StartDasGASPlaygroundValidation(GetWorld());
#endif
}
