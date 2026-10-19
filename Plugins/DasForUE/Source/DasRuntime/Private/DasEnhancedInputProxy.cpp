#include "DasEnhancedInputProxy.h"
#include "DasRuntime.h"
#include "UObject/NoExportTypes.h"
#include "Modules/ModuleManager.h"
void UDasEnhancedInputProxy::Receive(FInputActionValue Value, float ElapsedTime, float TriggeredTime,
                                     const UInputAction* SourceAction)
{
    if (!IsInGameThread() || SourceAction != Action.Get())
        return;
    FDasScalarValue Payload;
    Payload.Kind = PayloadKind;
    if (TypedValue)
    {
        const auto V = Value.Get<FVector>();
        const int32 K = int32(Value.GetValueType());
        if (K < 0 || K > 3 || V.ContainsNaN())
            return;
        Payload.ObjectClass = FInputActionValue::StaticStruct()->GetPathName();
        Payload.StructValue.InitializeAs(FInputActionValue::StaticStruct(), reinterpret_cast<const uint8*>(&Value));
    }
    else if (PayloadKind == 2 && Value.GetValueType() == EInputActionValueType::Boolean)
        Payload.BoolValue = Value.Get<bool>();
    else if (PayloadKind == 1 && Value.GetValueType() == EInputActionValueType::Axis1D)
        Payload.FloatValue = Value.Get<float>();
    else if (PayloadKind == 12 && Value.GetValueType() == EInputActionValueType::Axis2D)
    {
        const auto V = Value.Get<FVector2D>();
        if (V.ContainsNaN())
            return;
        Payload.ObjectClass = TEXT("/Script/CoreUObject.Vector2D");
        Payload.StructValue.InitializeAs(TBaseStructure<FVector2D>::Get(), reinterpret_cast<const uint8*>(&V));
    }
    else if (PayloadKind == 9 && Value.GetValueType() == EInputActionValueType::Axis3D)
        Payload.VectorValue = Value.Get<FVector>();
    else
        return;
    if (auto* Runtime = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
        Runtime->DispatchScalarEvent(SubscriptionId, Payload);
}
