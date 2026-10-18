#include "DasHitTransportLibrary.h"
#include "Components/BoxComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "UObject/GarbageCollection.h"

void UDasHitTransportLibrary::CollectWeak(){CollectGarbage(RF_NoFlags);}

FHitResult UDasHitTransportLibrary::MakeWeakHit(){
 FHitResult Hit(ForceInit);Hit.Item=123;
 Hit.Component=NewObject<UBoxComponent>();Hit.PhysMaterial=NewObject<UPhysicalMaterial>();
 return Hit;
}
