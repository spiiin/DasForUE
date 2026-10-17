#pragma once
#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
// Explicit cooked/editor policy. Expand alongside component integration tests.
inline bool DasNativeReferenceFieldAllowed(UClass* Class,const FName Name){
 return Class==UCharacterMovementComponent::StaticClass() && (Name==TEXT("MaxWalkSpeed") || Name==TEXT("JumpZVelocity"));
}
