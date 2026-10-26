#include "DasOpaqueTypeAdapters.h"
#include "DasGeometry.h"
#include "DasInputEvents.h"
#include "DasEventReplyTransport.h"
#include "DasPaintTransport.h"
#include "DasWidgetAnimationTransport.h"
#include "Layout/Geometry.h"
#include "Input/Events.h"
#include "Blueprint/UserWidget.h"
#include "Components/SlateWrapperTypes.h"
#include "Engine/EngineTypes.h"
#include "Engine/ActorInstanceHandle.h"
#include "PhysicsEngine/BodyInstance.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/ConstraintInstanceBlueprintLibrary.h"
#include "UObject/UnrealType.h"

namespace
{
class FOpaqueTypeAdapter final : public IDasTypeAdapter
{
    const UScriptStruct* Type;
    FDasNativeTypePolicy Policy;

public:
    FOpaqueTypeAdapter(const UScriptStruct* T, FDasNativeTypePolicy P) : Type(T), Policy(P) {}
    EDasTypeKind GetKind() const override { return EDasTypeKind::Struct; }
    const UScriptStruct* GetStruct() const override { return Type; }
    const FDasNativeTypePolicy* GetNativePolicy() const override { return &Policy; }
    // These are transport leaves only. IsLeafValue stays false: no generic authoring/default exemption.
};
bool ExactScalar(const FProperty* P, int32 Depth)
{
    return P && Depth == 0 && P->ArrayDim == 1 && P->HasAnyPropertyFlags(CPF_Parm);
}
}
void RegisterDasOpaqueTypeAdapters(FDasTypeAdapters& R)
{
    using Role = EDasNativeTypeRole;
    auto Add = [&](const UScriptStruct* T, FDasNativeTypePolicy P)
    {
        R.Register(MakeShared<FOpaqueTypeAdapter>(T, P));
    };
    FDasNativeTypePolicy Geometry;
    Geometry.Roles = Role::Geometry;
    Geometry.Transport = TEXT("slate_geometry_snapshot_v1");
    Geometry.Handle = "UEGeometry";
    Geometry.Lifetime = TEXT("owned_snapshot");
    Geometry.StorageError = TEXT("Geometry requires owned snapshot transport, not reflected storage/defaults");
    Geometry.CallError = "Geometry requires exact scalar snapshot function allowlist";
    Geometry.AllowCall = [](const FProperty* P, bool, bool, int32 D)
    {
        return ExactScalar(P, D) && DasGeometryFunctionAllowed(Cast<UFunction>(P->GetOwnerStruct()));
    };
    Geometry.Capture = CaptureDasGeometry;
    Geometry.ValidateSnapshot = ValidateDasGeometrySnapshot;
    Add(FGeometry::StaticStruct(), Geometry);

    FDasNativeTypePolicy Input;
    Input.Roles = Role::InputEvent;
    Input.Transport = TEXT("slate_input_event_snapshot_v1");
    Input.Lifetime = TEXT("detached_snapshot");
    Input.StorageError = TEXT("Input events require detached snapshot transport, not reflected storage/defaults");
    Input.CallError =
        "input events require exact scalar snapshot function allowlist; navigation/reply need live adapters";
    Input.AllowCall = [](const FProperty* P, bool, bool, int32 D)
    {
        return ExactScalar(P, D) && DasInputEventFunctionAllowed(Cast<UFunction>(P->GetOwnerStruct()));
    };
    Input.Capture = CaptureDasInputEvent;
    Input.ValidateSnapshot = ValidateDasInputEventSnapshot;
    auto Event = [&](const UScriptStruct* T, const char* Handle, Role Extra)
    {
        auto P = Input;
        P.Handle = Handle;
        P.Roles |= Extra;
        Add(T, P);
    };
    Event(FInputEvent::StaticStruct(), "UEInputEvent", Role::None);
    Event(FKeyEvent::StaticStruct(), "UEKeyEvent", Role::WidgetEvent);
    Event(FAnalogInputEvent::StaticStruct(), "UEAnalogInputEvent", Role::WidgetEvent);
    Event(FCharacterEvent::StaticStruct(), "UECharacterEvent", Role::WidgetEvent);
    Event(FPointerEvent::StaticStruct(), "UEPointerEvent", Role::WidgetEvent | Role::PointerEvent);
    Event(FFocusEvent::StaticStruct(), "UEFocusEvent", Role::WidgetEvent | Role::FocusEvent);
    Event(FNavigationEvent::StaticStruct(), "UENavigationEvent", Role::None);

    FDasNativeTypePolicy Reply;
    Reply.Roles = Role::EventReply;
    Reply.Transport = TEXT("scoped_event_reply_v1");
    Reply.Handle = "UEEventReply";
    Reply.Lifetime = TEXT("synchronous_callback_scope");
    Reply.StorageError = TEXT("EventReply requires a synchronous callback, not reflected storage/defaults");
    Reply.CallError = "EventReply requires exact scalar synchronous callback adapter";
    Reply.AllowCall = [](const FProperty* P, bool, bool, int32 D)
    {
        return ExactScalar(P, D) && DasEventReplyFunctionAllowed(Cast<UFunction>(P->GetOwnerStruct()));
    };
    Add(FEventReply::StaticStruct(), Reply);
    FDasNativeTypePolicy Paint;
    Paint.Roles = Role::Paint;
    Paint.Transport = TEXT("scoped_paint_context_v1");
    Paint.Handle = "UEPaintContext";
    Paint.Lifetime = TEXT("synchronous_paint_scope");
    Paint.StorageError = TEXT("PaintContext requires synchronous OnPaint, not reflected storage/defaults");
    Paint.CallError = "PaintContext requires exact scalar synchronous OnPaint adapter";
    Paint.AllowCall = [](const FProperty* P, bool, bool, int32 D)
    {
        return ExactScalar(P, D) && !P->HasAnyPropertyFlags(CPF_ReturnParm) &&
               DasPaintFunctionAllowed(Cast<UFunction>(P->GetOwnerStruct()));
    };
    Add(FPaintContext::StaticStruct(), Paint);
    FDasNativeTypePolicy Animation;
    Animation.Roles = Role::WidgetAnimation;
    Animation.Transport = TEXT("weak_widget_animation_v1");
    Animation.Handle = "UEWidgetAnimationHandle";
    Animation.Lifetime = TEXT("weak_widget_context_capability");
    Animation.StorageError = TEXT("WidgetAnimation requires a weak context capability, not reflected storage/defaults");
    Animation.CallError = "WidgetAnimation requires exact scalar producer result";
    Animation.AllowCall = [](const FProperty* P, bool In, bool Out, int32 D)
    {
        return ExactScalar(P, D) && P->HasAnyPropertyFlags(CPF_ReturnParm) && !In && Out &&
               DasWidgetAnimationFunctionAllowed(Cast<UFunction>(P->GetOwnerStruct()));
    };
    Add(FWidgetAnimationHandle::StaticStruct(), Animation);
    FDasNativeTypePolicy Hit;
    Hit.Roles = Role::HitResult;
    Hit.Transport = TEXT("hit_result_v1");
    Hit.Handle = "UEHitResult";
    Hit.Lifetime = TEXT("owned_collision_snapshot");
    Hit.CallError = "fixed hit result arrays need an adapter";
    Hit.AllowCall = [](const FProperty* P, bool, bool, int32)
    {
        return P && P->ArrayDim == 1;
    };
    Add(FHitResult::StaticStruct(), Hit);
    FDasNativeTypePolicy Constraint;
    Constraint.Roles = Role::Constraint;
    Constraint.Transport = TEXT("constraint_component_v1");
    Constraint.Handle = "UEConstraintHandle";
    Constraint.Lifetime = TEXT("component_owned_capability");
    Constraint.CallError = "constraint accessors require scalar component-owned getter/library contract";
    Constraint.AllowCall = [](const FProperty* P, bool In, bool, int32)
    {
        const auto* F = P ? Cast<UFunction>(P->GetOwnerStruct()) : nullptr;
        const bool Getter = F && F->GetOuterUClass() == UPhysicsConstraintComponent::StaticClass() &&
                            F->GetFName() == TEXT("GetConstraint") && P->HasAnyPropertyFlags(CPF_ReturnParm);
        const bool Consumer = F && F->GetOuterUClass() == UConstraintInstanceBlueprintLibrary::StaticClass() &&
                              !P->HasAnyPropertyFlags(CPF_ReturnParm) && In;
        return P && P->ArrayDim == 1 && P->HasAnyPropertyFlags(CPF_Parm) && (Getter || Consumer);
    };
    Add(FConstraintInstanceAccessor::StaticStruct(), Constraint);
    FDasNativeTypePolicy Rejected;
    Rejected.Roles = Role::Rejected;
    Rejected.Lifetime = TEXT("rejected_internal_handle");
    Rejected.AllowCall = [](const FProperty*, bool, bool, int32)
    {
        return false;
    };
    Rejected.CallError = "Actor instance handles require the collision result adapter";
    Rejected.StorageError = TEXT("Actor instance handles require the collision result adapter");
    Add(FActorInstanceHandle::StaticStruct(), Rejected);
    Rejected.CallError = "async physics handles require a scoped frozen-game-thread physics callback adapter";
    Rejected.StorageError = nullptr; // Keep the ordinary reflected-storage validation of this engine struct.
    Add(FBodyInstanceAsyncPhysicsTickHandle::StaticStruct(), Rejected);
}
