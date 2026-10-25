#include "DasScriptHosts.h"
#include "DasPawn.h"
#include "DasCharacter.h"
#include "DasActor.h"
#if DAS_WITH_TEST_PROBES
#include "DasProbeActor.h"
#endif
#include "DasScriptComponent.h"
#include "DasSaveGame.h"
#include "DasDataAssets.h"
#include "UObject/UnrealType.h"
bool DasActorNativeParent(const FString& P)
{
#if DAS_WITH_TEST_PROBES
    if (P == TEXT("/Script/DasRuntime.DasProbeActor"))
        return true;
#endif
    return P == TEXT("/Script/Engine.Actor") || P == TEXT("/Script/Engine.Pawn") ||
                                                          P == TEXT("/Script/Engine.Character");
}
UClass* DasNativeScriptHost(const FString& P)
{
#if DAS_WITH_TEST_PROBES
    if (P == TEXT("DasProbeActor") || P == TEXT("/Script/DasRuntime.DasProbeActor"))
        return ADasProbeActor::StaticClass();
#endif
    if (P == TEXT("Actor") || P == TEXT("/Script/Engine.Actor"))
        return ADasActor::StaticClass();
    if (P == TEXT("Pawn") || P == TEXT("/Script/Engine.Pawn"))
        return ADasPawn::StaticClass();
    if (P == TEXT("Character") || P == TEXT("/Script/Engine.Character"))
        return ADasCharacter::StaticClass();
    if (P == TEXT("ActorComponent") || P == TEXT("/Script/Engine.ActorComponent"))
        return UDasScriptComponent::StaticClass();
    if (P == TEXT("SaveGame") || P == TEXT("/Script/Engine.SaveGame"))
        return UDasSaveGame::StaticClass();
    if (P == TEXT("DataAsset") || P == TEXT("/Script/Engine.DataAsset"))
        return UDasDataAsset::StaticClass();
    if (P == TEXT("PrimaryDataAsset") || P == TEXT("/Script/Engine.PrimaryDataAsset"))
        return UDasPrimaryDataAsset::StaticClass();
    return nullptr;
}
bool DasActorHostClass(const UClass* C)
{
    return C && (C->IsChildOf(ADasActor::StaticClass()) || C->IsChildOf(ADasPawn::StaticClass()) ||
                 C->IsChildOf(ADasCharacter::StaticClass()));
}
FString* DasHostString(UObject* O, FName N)
{
    if (!O || (!DasActorHostClass(O->GetClass()) && !O->IsA<UDasScriptComponent>() && !O->IsA<UDasSaveGame>() &&
               !O->IsA<UDasDataAsset>() && !O->IsA<UDasPrimaryDataAsset>()))
        return nullptr;
    auto* P = FindFProperty<FStrProperty>(O->GetClass(), N);
    return P ? P->ContainerPtrToValuePtr<FString>(O) : nullptr;
}
FString DasHostIdentity(const UObject* O)
{
    const auto* V = DasHostString(const_cast<UObject*>(O), TEXT("AuthoredTypeId"));
    return V ? *V : FString();
}
FString DasHostLifecycle(const UObject* O)
{
    const auto* V = DasHostString(const_cast<UObject*>(O), TEXT("LifecycleScript"));
    return V ? *V : FString();
}
TMap<FString, FString>* DasHostContracts(UObject* O)
{
    if (!O || (!DasActorHostClass(O->GetClass()) && !O->IsA<UDasScriptComponent>() && !O->IsA<UDasSaveGame>() &&
               !O->IsA<UDasDataAsset>() && !O->IsA<UDasPrimaryDataAsset>()))
        return nullptr;
    auto* P = FindFProperty<FMapProperty>(O->GetClass(), TEXT("DasDefaultSourceContracts"));
    return P ? P->ContainerPtrToValuePtr<TMap<FString, FString>>(O) : nullptr;
}
