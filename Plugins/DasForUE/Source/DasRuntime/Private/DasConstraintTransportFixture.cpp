#include "DasConstraintTransportFixture.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "UObject/GarbageCollection.h"
namespace
{
TWeakObjectPtr<UPhysicsConstraintComponent> LastConstraint;
}
UPhysicsConstraintComponent* UDasConstraintTransportFixture::CreateTestConstraint(const UObject* WorldContextObject)
{
    auto* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
    auto* Owner = World->SpawnActor<AActor>();
    UBoxComponent* Bodies[2]{};
    for (int32 I = 0; I < 2; ++I)
    {
        auto* Body = NewObject<UBoxComponent>(Owner);
        Owner->AddInstanceComponent(Body);
        if (I == 0)
            Owner->SetRootComponent(Body);
        Body->SetBoxExtent(FVector(20));
        Body->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        Body->SetWorldLocation(FVector(0, I * 100, 0));
        Body->RegisterComponent();
        Body->SetSimulatePhysics(true);
        Bodies[I] = Body;
    }
    auto* Joint = NewObject<UPhysicsConstraintComponent>(Owner);
    Owner->AddInstanceComponent(Joint);
    Joint->RegisterComponent();
    Joint->SetConstrainedComponents(Bodies[0], NAME_None, Bodies[1], NAME_None);
    LastConstraint = Joint;
    return Joint;
}
void UDasConstraintTransportFixture::RebuildConstraint(UPhysicsConstraintComponent* C)
{
    if (IsValid(C))
    {
        C->TermComponentConstraint();
        C->InitComponentConstraint();
    }
}
void UDasConstraintTransportFixture::DestroyOwner(UPhysicsConstraintComponent* C)
{
    if (IsValid(C) && C->GetOwner())
        C->GetOwner()->Destroy();
}
void UDasConstraintTransportFixture::CollectWeak()
{
    CollectGarbage(RF_NoFlags);
}
bool UDasConstraintTransportFixture::LastOwnerCollected()
{
    return !LastConstraint.IsValid();
}
