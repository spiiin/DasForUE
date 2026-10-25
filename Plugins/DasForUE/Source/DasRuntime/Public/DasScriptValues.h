#pragma once
#include "CoreMinimal.h"

// Script values with a C++ layout that generated AOT code and game modules share with the runtime: FDateTime and
// FTimespan as ticks (the ue_foundation types UEDateTime and UETimespan), an object reference as the token of its
// context's handle table (UObjectHandle and the class types of native binds), names and texts as tokens of the
// context's tables. Plain data, no daScript headers.
struct FDasDateTimeValue
{
    int64 Ticks = 0;
};
struct FDasTimespanValue
{
    int64 Ticks = 0;
};
struct FDasObjectHandle
{
    uint64 Token = 0;
};
// A class reference (UEClassHandle): the token of the class object in the context's handle table.
struct FDasClassHandle
{
    uint64 Token = 0;
};
// An FName (UENameValue) and an FText (UETextValue): tokens of the context's name and text tables, 0 for None and
// the empty text.
struct FDasNameValue
{
    uint64 Token = 0;
};
struct FDasTextValue
{
    uint64 Token = 0;
};
// Tokens of the context's tables: enum values, constraints, hit results, struct snapshots, UI values (event
// replies, widget animations, paint contexts, geometry, input events) and reflected call frames (UCallHandle).
struct FDasEnumValue
{
    uint64 Token = 0;
};
struct FDasConstraintHandle
{
    uint64 Token = 0;
};
struct FDasHitResultValue
{
    uint64 Token = 0;
};
struct FDasStructValue
{
    uint64 Token = 0;
};
struct FDasEventReplyValue
{
    uint64 Token = 0;
};
struct FDasWidgetAnimationValue
{
    uint64 Token = 0;
};
struct FDasPaintValue
{
    uint64 Token = 0;
};
struct FDasGeometryValue
{
    uint64 Token = 0;
};
struct FDasInputEventValue
{
    uint64 Token = 0;
};
struct FDasKeyEventValue
{
    uint64 Token = 0;
};
struct FDasAnalogInputEventValue
{
    uint64 Token = 0;
};
struct FDasCharacterEventValue
{
    uint64 Token = 0;
};
struct FDasPointerEventValue
{
    uint64 Token = 0;
};
struct FDasFocusEventValue
{
    uint64 Token = 0;
};
struct FDasNavigationEventValue
{
    uint64 Token = 0;
};
struct FDasCallHandle
{
    uint64 Token = 0;
};
// Values with fields that scripts read (InputActionValue, UETraceHit, UEOverlapResult).
struct FDasInputActionValue
{
    double X = 0, Y = 0, Z = 0;
    int32 Kind = 0;
};
// A source-side checked view of a native FHitResult. References are weak handles.
struct FDasTraceHit
{
    uint64 Scope = 0;
    bool BlockingHit = false, StartPenetrating = false;
    double Distance = 0, Time = 0;
    FVector Location = FVector::ZeroVector, ImpactPoint = FVector::ZeroVector, Normal = FVector::ZeroVector,
            ImpactNormal = FVector::ZeroVector;
    FVector TraceStart = FVector::ZeroVector, TraceEnd = FVector::ZeroVector;
    double PenetrationDepth = 0;
    int32 FaceIndex = -1, Item = -1, MyItem = -1, ElementIndex = 0;
    FName BoneName, MyBoneName;
    FDasObjectHandle Actor{}, Component{}, PhysMaterial{}, PhysicsObjectOwner{};
};
struct FDasOverlapResult
{
    uint64 Scope = 0;
    bool BlockingHit = false;
    int32 ItemIndex = -1, MyItemIndex = -1;
    FDasObjectHandle Actor{}, Component{}, PhysicsObjectOwner{};
};
// An array as the script holds it (array<T>), in the layout of das::Array (DasRuntime checks it): native binds
// return arrays of objects in it, the AOT bridge hands generated code a das::TArray.
struct alignas(8) FDasArrayValue
{
    uint8 Bytes[40];
};
