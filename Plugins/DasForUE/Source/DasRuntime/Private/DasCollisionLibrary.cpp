#include "DasCollisionLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

float UDasCollisionLibrary::ApplyPointDamage(AActor* Target, float Damage, FVector Direction,
                                             const FDasCollisionHit& Hit, AController* Instigator, AActor* Source)
{
    if (!IsInGameThread() || !IsValid(Target) || !FMath::IsFinite(Damage) || Direction.ContainsNaN() ||
        Hit.Location.ContainsNaN() || Hit.ImpactPoint.ContainsNaN() || Hit.Normal.ContainsNaN() ||
        Hit.ImpactNormal.ContainsNaN() || Hit.TraceStart.ContainsNaN() || Hit.TraceEnd.ContainsNaN() ||
        !FMath::IsFinite(Hit.Time) || !FMath::IsFinite(Hit.Distance) || !FMath::IsFinite(Hit.PenetrationDepth))
        return 0.f;
    FHitResult Native(Hit.Actor, Cast<UPrimitiveComponent>(Hit.Component), Hit.Location, Hit.Normal);
    Native.bBlockingHit = Hit.BlockingHit;
    Native.bStartPenetrating = Hit.StartPenetrating;
    Native.Time = Hit.Time;
    Native.Distance = Hit.Distance;
    Native.PenetrationDepth = Hit.PenetrationDepth;
    Native.Location = Hit.Location;
    Native.ImpactPoint = Hit.ImpactPoint;
    Native.Normal = Hit.Normal;
    Native.ImpactNormal = Hit.ImpactNormal;
    Native.TraceStart = Hit.TraceStart;
    Native.TraceEnd = Hit.TraceEnd;
    Native.FaceIndex = Hit.FaceIndex;
    Native.Item = Hit.Item;
    Native.MyItem = Hit.MyItem;
    Native.ElementIndex = Hit.ElementIndex;
    Native.BoneName = Hit.BoneName;
    Native.MyBoneName = Hit.MyBoneName;
    Native.PhysMaterial = Cast<UPhysicalMaterial>(Hit.PhysMaterial);
    return UGameplayStatics::ApplyPointDamage(Target, Damage, Direction, Native, Instigator, Source, nullptr);
}
