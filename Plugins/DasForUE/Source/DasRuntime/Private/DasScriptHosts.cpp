#include "DasScriptHosts.h"
#include "DasPawn.h"
#include "DasCharacter.h"
#include "DasProbeActor.h"
#include "DasScriptComponent.h"
#include "DasSaveGame.h"
#include "UObject/UnrealType.h"
bool DasActorNativeParent(const FString& P){return P==TEXT("/Script/Engine.Actor") || P==TEXT("/Script/Engine.Pawn") || P==TEXT("/Script/Engine.Character");}
UClass* DasNativeScriptHost(const FString& P){return P==TEXT("Actor") || P==TEXT("/Script/Engine.Actor")?ADasProbeActor::StaticClass():P==TEXT("Pawn") || P==TEXT("/Script/Engine.Pawn")?ADasPawn::StaticClass():P==TEXT("Character") || P==TEXT("/Script/Engine.Character")?ADasCharacter::StaticClass():P==TEXT("ActorComponent") || P==TEXT("/Script/Engine.ActorComponent")?UDasScriptComponent::StaticClass():P==TEXT("SaveGame") || P==TEXT("/Script/Engine.SaveGame")?UDasSaveGame::StaticClass():nullptr;}
bool DasActorHostClass(const UClass* C){return C && (C->IsChildOf(ADasProbeActor::StaticClass()) || C->IsChildOf(ADasPawn::StaticClass()) || C->IsChildOf(ADasCharacter::StaticClass()));}
FString* DasHostString(UObject* O,FName N){if(!O || (!DasActorHostClass(O->GetClass()) && !O->IsA<UDasScriptComponent>() && !O->IsA<UDasSaveGame>()))return nullptr;auto* P=FindFProperty<FStrProperty>(O->GetClass(),N);return P?P->ContainerPtrToValuePtr<FString>(O):nullptr;}
FString DasHostIdentity(const UObject* O){const auto* V=DasHostString(const_cast<UObject*>(O),TEXT("AuthoredTypeId"));return V?*V:FString();}
FString DasHostLifecycle(const UObject* O){const auto* V=DasHostString(const_cast<UObject*>(O),TEXT("LifecycleScript"));return V?*V:FString();}
TMap<FString,FString>* DasHostContracts(UObject* O){if(!O || (!DasActorHostClass(O->GetClass()) && !O->IsA<UDasScriptComponent>() && !O->IsA<UDasSaveGame>()))return nullptr;auto* P=FindFProperty<FMapProperty>(O->GetClass(),TEXT("DasDefaultSourceContracts"));return P?P->ContainerPtrToValuePtr<TMap<FString,FString>>(O):nullptr;}
