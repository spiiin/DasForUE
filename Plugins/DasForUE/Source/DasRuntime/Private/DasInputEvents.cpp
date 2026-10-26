#include "DasTypeAdapters.h"
#include "DasInputEvents.h"
#include "DasExecutionRole.h"
#include "Input/Events.h"
#include "UObject/Class.h"

namespace
{
template <class T> struct TDetachedInputEvent final : T
{
    explicit TDetachedInputEvent(const T& Source) : T(Source) { this->EventPath = nullptr; }
};
const FInputEvent* BaseEvent(const UScriptStruct* Type, const void* Data)
{
    if (!Data)
        return nullptr;
#define INPUT_KIND(T)                                                                                                  \
    if (Type == T::StaticStruct())                                                                                     \
        return static_cast<const T*>(Data);
    INPUT_KIND(FInputEvent)
    INPUT_KIND(FKeyEvent)
    INPUT_KIND(FAnalogInputEvent)
    INPUT_KIND(FCharacterEvent)
    INPUT_KIND(FPointerEvent)
    INPUT_KIND(FNavigationEvent)
#undef INPUT_KIND
    return nullptr;
}
bool ValidateSource(const UScriptStruct* Type, const void* Data, FString& Error)
{
    if (Type == FFocusEvent::StaticStruct() && Data)
    {
        if (uint8(static_cast<const FFocusEvent*>(Data)->GetCause()) > uint8(EFocusCause::WindowActivate))
        {
            Error = TEXT("invalid focus cause");
            return false;
        }
        return true;
    }
    if (!BaseEvent(Type, Data))
    {
        Error = TEXT("unsupported input event identity");
        return false;
    }
    if (Type == FAnalogInputEvent::StaticStruct() &&
        !FMath::IsFinite(static_cast<const FAnalogInputEvent*>(Data)->GetAnalogValue()))
    {
        Error = TEXT("nonfinite analog input event");
        return false;
    }
    if (Type == FPointerEvent::StaticStruct())
    {
        const auto& P = *static_cast<const FPointerEvent*>(Data);
        if (P.GetPressedButtons().Num() > 256)
        {
            Error = TEXT("input event pressed-key limit exceeded");
            return false;
        }
        const FVector2D A = P.GetScreenSpacePosition(), B = P.GetLastScreenSpacePosition(), D = P.GetCursorDelta(),
                        G = P.GetGestureDelta();
        if (A.ContainsNaN() || B.ContainsNaN() || D.ContainsNaN() || G.ContainsNaN() ||
            !FMath::IsFinite(P.GetWheelDelta()) || !FMath::IsFinite(P.GetTouchForce()))
        {
            Error = TEXT("nonfinite pointer input event");
            return false;
        }
    }
    return true;
}
}
bool DasIsInputEventType(const UScriptStruct* Type)
{
    return DasHasNativeRole(Type, EDasNativeTypeRole::InputEvent);
}
bool ValidateDasInputEventSnapshot(const FInstancedStruct& Value, FString& Error)
{
    return ValidateDasInputEventData(Value.GetScriptStruct(), Value.GetMemory(), Error);
}
bool ValidateDasInputEventData(const UScriptStruct* Type, const void* Data, FString& Error)
{
    Error.Reset();
    if (!IsInGameThread() || GetDasExecutionRole() != EDasExecutionRole::Gameplay || IsDasFrozenPhysicsWorld(nullptr))
    {
        Error = TEXT("input events require gameplay game thread");
        return false;
    }
    if (!ValidateSource(Type, Data, Error))
        return false;
    if (Type != FFocusEvent::StaticStruct() && BaseEvent(Type, Data)->GetEventPath() != nullptr)
    {
        Error = TEXT("input event snapshot has a borrowed EventPath");
        return false;
    }
    return true;
}
bool CaptureDasInputEvent(const UScriptStruct* Type, const void* Source, FInstancedStruct& Out, FString& Error)
{
    Error.Reset();
    if (!IsInGameThread() || GetDasExecutionRole() != EDasExecutionRole::Gameplay || IsDasFrozenPhysicsWorld(nullptr))
    {
        Error = TEXT("input capture requires gameplay game thread");
        return false;
    }
    if (!ValidateSource(Type, Source, Error))
        return false;
    FInstancedStruct Candidate;
    if (Type == FFocusEvent::StaticStruct())
        Candidate.InitializeAs(Type, reinterpret_cast<const uint8*>(Source));
#define CAPTURE_KIND(T)                                                                                                \
    if (Type == T::StaticStruct())                                                                                     \
    {                                                                                                                  \
        TDetachedInputEvent<T> Copy(*static_cast<const T*>(Source));                                                   \
        Candidate.InitializeAs(T::StaticStruct(), reinterpret_cast<const uint8*>(static_cast<const T*>(&Copy)));       \
    }
    CAPTURE_KIND(FInputEvent)
    CAPTURE_KIND(FKeyEvent)
    CAPTURE_KIND(FAnalogInputEvent)
    CAPTURE_KIND(FCharacterEvent)
    CAPTURE_KIND(FPointerEvent)
    CAPTURE_KIND(FNavigationEvent)
#undef CAPTURE_KIND
    if (!ValidateDasInputEventSnapshot(Candidate, Error))
        return false;
    Out = MoveTemp(Candidate);
    return true;
}
bool DasInputEventFunctionAllowed(const UFunction* Function)
{
    if (!Function || !Function->HasAllFunctionFlags(FUNC_Native | FUNC_Static))
        return false;
    if (DasInputNavigationFunctionAllowed(Function))
        return true;
    const FString Path = Function->GetOuterUClass()->GetPathName() + TEXT(".")+ Function->GetName();
    if (Function->GetOuterUClass()->GetPathName() == TEXT("/Script/UMG.WidgetBlueprintLibrary") &&
                                                          Function->GetFName() == TEXT("DetectDragIfPressed"))
        return true;
    static const TCHAR* Allowed[] = {
        TEXT("/Script/Engine.KismetInputLibrary.GetAnalogValue"),
            TEXT("/Script/Engine.KismetInputLibrary.GetKey"),
                TEXT("/Script/Engine.KismetInputLibrary.GetUserIndex"),
                    TEXT("/Script/Engine.KismetInputLibrary.InputEvent_IsAltDown"),
                        TEXT("/Script/Engine.KismetInputLibrary.InputEvent_IsCommandDown"),
                            TEXT("/Script/Engine.KismetInputLibrary.InputEvent_IsControlDown"),
                                TEXT("/Script/Engine.KismetInputLibrary.InputEvent_IsLeftAltDown"),
                                    TEXT("/Script/Engine.KismetInputLibrary.InputEvent_IsLeftCommandDown"),
                                        TEXT("/Script/Engine.KismetInputLibrary.InputEvent_IsLeftControlDown"),
                                            TEXT("/Script/Engine.KismetInputLibrary.InputEvent_IsLeftShiftDown"),
                                                TEXT("/Script/Engine.KismetInputLibrary.InputEvent_IsRepeat"),
                                                    TEXT("/Script/Engine.KismetInputLibrary.InputEvent_IsRightAltDown"),
                                                        TEXT("/Script/Engine.KismetInputLibrary.InputEvent_IsRightCommandDown"),
                                                            TEXT("/Script/Engine.KismetInputLibrary.InputEvent_IsRightControlDown"),
                                                                TEXT("/Script/Engine.KismetInputLibrary.InputEvent_IsRightShiftDown"),
                                                                    TEXT("/Script/Engine.KismetInputLibrary.InputEvent_IsShiftDown"),
                                                                        TEXT("/Script/Engine.KismetInputLibrary.PointerEvent_GetCursorDelta"),
                                                                            TEXT("/Script/Engine.KismetInputLibrary.PointerEvent_GetEffectingButton"),
                                                                                TEXT("/Script/Engine.KismetInputLibrary.PointerEvent_GetGestureDelta"),
                                                                                    TEXT("/Script/Engine.KismetInputLibrary.PointerEvent_GetGestureType"),
                                                                                        TEXT("/Script/Engine.KismetInputLibrary.PointerEvent_GetLastScreenSpacePosition"),
                                                                                            TEXT("/Script/Engine.KismetInputLibrary.PointerEvent_GetPointerIndex"),
                                                                                                TEXT("/Script/Engine.KismetInputLibrary.PointerEvent_GetScreenSpacePosition"),
                                                                                                    TEXT("/Script/Engine.KismetInputLibrary.PointerEvent_GetTouchpadIndex"),
                                                                                                        TEXT("/Script/Engine.KismetInputLibrary.PointerEvent_GetUserIndex"),
                                                                                                            TEXT("/Script/Engine.KismetInputLibrary.PointerEvent_GetWheelDelta"),
                                                                                                                TEXT("/Script/Engine.KismetInputLibrary.PointerEvent_IsMouseButtonDown"),
                                                                                                                    TEXT("/Script/Engine.KismetInputLibrary.PointerEvent_IsTouchEvent"),
                                                                                                                        TEXT("/Script/UMG.WidgetBlueprintLibrary.GetInputEventFromCharacterEvent"),
                                                                                                                            TEXT("/Script/UMG.WidgetBlueprintLibrary.GetInputEventFromKeyEvent"),
                                                                                                                                TEXT("/Script/UMG.WidgetBlueprintLibrary.GetInputEventFromNavigationEvent"),
                                                                                                                                    TEXT("/Script/UMG.WidgetBlueprintLibrary.GetInputEventFromPointerEvent"),
                                                                                                                                        TEXT("/Script/UMG.WidgetBlueprintLibrary.GetKeyEventFromAnalogInputEvent"),
                                                                                                                                    };
    for (const TCHAR* Name : Allowed)
        if (Path == Name)
            return true;
    return false;
}

bool DasInputNavigationFunctionAllowed(const UFunction* F)
{
    if (!F || !F->HasAllFunctionFlags(FUNC_Native | FUNC_Static) ||
        F->GetOuterUClass()->GetPathName() != TEXT("/Script/Engine.KismetInputLibrary"))
        return false;
    static const TCHAR* Names[] = {TEXT("Key_GetNavigationActionFromKey"),
                                        TEXT("Key_GetNavigationDirectionFromKey"),
                                             TEXT("Key_GetNavigationDirectionFromAnalog")};
    for (const TCHAR* Name : Names)
        if (F->GetFName() == Name)
            return true;
    return false;
}
