#include "DasActorHostRuntime.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Logging/MessageLog.h"
#include "Misc/CoreMisc.h"

void DasReportScriptError(const UObject& Owner, const FString& Error)
{
    // Automation and commandlets check script errors through the log (many tests trigger them on purpose);
    // only interactive game worlds get the Message Log entry and the on-screen message.
    if (Error.IsEmpty() || IsRunningCommandlet() || GIsAutomationTesting)
        return;
    const UWorld* World = Owner.GetWorld();
    if (!World || !World->IsGameWorld())
        return;
    // A Blueprint applied from an older version of its script keeps the old declaration; the runtime refuses
    // to run the new script on it until the declaration is applied again.
    const bool bStale = Error.Contains(TEXT("apply/reimport")) || Error.Contains(TEXT("mismatch"));
    const FString Text = FString::Printf(
        TEXT("daScript did not run on %s (%s): %s%s"), *Owner.GetName(), *Owner.GetClass()->GetName(),
             *Error.TrimStartAndEnd(),
             bStale ? TEXT(". The Blueprint was made from an older version of the script: apply the script again or "
                      "recreate the example.")
                    : TEXT(""));
#if WITH_EDITOR
    FMessageLog(TEXT("PIE")).Error(FText::FromString(Text));
#endif
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, Text);
}
