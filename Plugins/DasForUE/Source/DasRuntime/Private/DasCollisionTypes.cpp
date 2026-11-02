#include "DasCollisionTypes.h"
#include "DasRuntime.h"
#include "DasCallFrame.h"
#include "Engine/HitResult.h"
#include "Components/PrimitiveComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "UObject/UnrealType.h"

bool CanProjectDasCollisionHit(const FProperty* Source, const UScriptStruct* Expected)
{
    const auto* P = CastField<FStructProperty>(Source);
    return P && P->ArrayDim == 1 && P->Struct == FHitResult::StaticStruct() &&
           Expected == FDasCollisionHit::StaticStruct();
}
bool ReadDasCollisionHitCallback(const FProperty* Source, const void* Memory, const UScriptStruct* Expected,
                                 FDasScalarValue& Out, FString& Error)
{
    if (!IsInGameThread() || !Memory || !CanProjectDasCollisionHit(Source, Expected))
    {
        Error = TEXT("Expected native HitResult to owned collision callback view");
        return false;
    }
    const auto& Hit = *static_cast<const FHitResult*>(Memory);
    if (!FMath::IsFinite(Hit.Time) || !FMath::IsFinite(Hit.Distance) || !FMath::IsFinite(Hit.PenetrationDepth) ||
        Hit.Location.ContainsNaN() || Hit.ImpactPoint.ContainsNaN() || Hit.Normal.ContainsNaN() ||
        Hit.ImpactNormal.ContainsNaN() || Hit.TraceStart.ContainsNaN() || Hit.TraceEnd.ContainsNaN())
    {
        Error = TEXT("Nonfinite native collision hit");
        return false;
    }
    FDasCollisionHit View;
    View.BlockingHit = Hit.bBlockingHit;
    View.StartPenetrating = Hit.bStartPenetrating;
    View.Time = Hit.Time;
    View.Distance = Hit.Distance;
    View.PenetrationDepth = Hit.PenetrationDepth;
    View.Location = Hit.Location;
    View.ImpactPoint = Hit.ImpactPoint;
    View.Normal = Hit.Normal;
    View.ImpactNormal = Hit.ImpactNormal;
    View.TraceStart = Hit.TraceStart;
    View.TraceEnd = Hit.TraceEnd;
    View.FaceIndex = Hit.FaceIndex;
    View.Item = Hit.Item;
    View.MyItem = Hit.MyItem;
    View.ElementIndex = Hit.ElementIndex;
    View.BoneName = Hit.BoneName;
    View.MyBoneName = Hit.MyBoneName;
    View.Actor = Hit.GetActor();
    View.Component = Hit.GetComponent();
    View.PhysMaterial = Hit.PhysMaterial.Get();
    FDasScalarValue Value;
    Value.Kind = int32(EDasMethodSignature::Struct);
    Value.ObjectClass = FDasCollisionHit::StaticStruct()->GetPathName();
    Value.StructValue.InitializeAs(FDasCollisionHit::StaticStruct(), reinterpret_cast<const uint8*>(&View));
    Out = MoveTemp(Value);
    return true;
}
