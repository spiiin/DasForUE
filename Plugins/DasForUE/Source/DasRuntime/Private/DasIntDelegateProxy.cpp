#include "DasIntDelegateProxy.h"
#include "DasRuntime.h"
#include "DasContainerTypes.h"
#include "UObject/NoExportTypes.h"
#include "Modules/ModuleManager.h"

void UDasIntDelegateProxy::Receive(int32 Value)
{
    if (!IsInGameThread()) return; // This bridge intentionally supports game-thread events only.
    if (auto* Runtime = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
        Runtime->DispatchIntEvent(SubscriptionId, Value);
}

void UDasIntDelegateProxy::ReceiveFloat(float Value){if(!IsInGameThread())return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=1;V.FloatValue=Value;R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveBool(bool Value){if(!IsInGameThread())return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=2;V.BoolValue=Value;R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveString(const FString& Value){if(!IsInGameThread())return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=5;V.StringValue=Value;R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveVoid(){if(!IsInGameThread())return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=3;R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveDouble(double Value){if(!IsInGameThread())return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=6;V.DoubleValue=Value;R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveObject(UObject* Value){if(!IsInGameThread())return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=8;V.ObjectValue=Value;R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveVector(FVector Value){if(!IsInGameThread())return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=9;V.VectorValue=Value;R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveVectorRef(const FVector& Value){if(!IsInGameThread())return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=9;V.VectorValue=Value;R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveRotator(FRotator Value){if(!IsInGameThread())return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=10;V.RotatorValue=Value;R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveRotatorRef(const FRotator& Value){if(!IsInGameThread())return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=10;V.RotatorValue=Value;R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveTransform(FTransform Value){if(!IsInGameThread())return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=11;V.TransformValue=Value;R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveTransformRef(const FTransform& Value){if(!IsInGameThread())return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=11;V.TransformValue=Value;R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveName(FName Value){if(!IsInGameThread())return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=7;V.NameValue=Value;R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveArrayInt(const TArray<int32>& Value){if(!IsInGameThread() || Value.Num()>4096)return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=12;V.ObjectClass=FDasArrayIntCarrier::StaticStruct()->GetPathName();V.StructValue.InitializeAs(FDasArrayIntCarrier::StaticStruct());V.StructValue.GetMutable<FDasArrayIntCarrier>().Values=Value;R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveArrayDouble(const TArray<double>& Value){if(!IsInGameThread() || Value.Num()>4096)return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=12;V.ObjectClass=FDasArrayDoubleCarrier::StaticStruct()->GetPathName();V.StructValue.InitializeAs(FDasArrayDoubleCarrier::StaticStruct());V.StructValue.GetMutable<FDasArrayDoubleCarrier>().Values=Value;R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveArrayString(const TArray<FString>& Value){if(!IsInGameThread() || Value.Num()>4096)return;int64 Chars=0;for(const auto& S:Value){if(S.Len()>4096 || (Chars+=S.Len())>1048576)return;for(TCHAR C:S)if(C==0)return;}if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=12;V.ObjectClass=FDasArrayStringCarrier::StaticStruct()->GetPathName();V.StructValue.InitializeAs(FDasArrayStringCarrier::StaticStruct());V.StructValue.GetMutable<FDasArrayStringCarrier>().Values=Value;R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveVector2D(FVector2D Value){if(!IsInGameThread() || Value.ContainsNaN())return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=12;V.ObjectClass=TEXT("/Script/CoreUObject.Vector2D");V.StructValue.InitializeAs(TBaseStructure<FVector2D>::Get(),reinterpret_cast<const uint8*>(&Value));R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveVector2DRef(const FVector2D& Value){if(!IsInGameThread() || Value.ContainsNaN())return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=12;V.ObjectClass=TEXT("/Script/CoreUObject.Vector2D");V.StructValue.InitializeAs(TBaseStructure<FVector2D>::Get(),reinterpret_cast<const uint8*>(&Value));R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveByte(uint8 Value){if(!IsInGameThread())return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=17;V.ByteValue=Value;R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveText(FText Value){if(!IsInGameThread())return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=18;V.TextValue=Value;R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveTextRef(const FText& Value){if(!IsInGameThread())return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=18;V.TextValue=Value;R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveArrayByte(const TArray<uint8>& Value){if(!IsInGameThread() || Value.Num()>4096)return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=12;V.ObjectClass=FDasArrayByteCarrier::StaticStruct()->GetPathName();V.StructValue.InitializeAs(FDasArrayByteCarrier::StaticStruct());V.StructValue.GetMutable<FDasArrayByteCarrier>().Values=Value;R->DispatchScalarEvent(SubscriptionId,V);}}

void UDasIntDelegateProxy::ReceiveArrayText(const TArray<FText>& Value){if(!IsInGameThread() || Value.Num()>4096)return;if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=12;V.ObjectClass=FDasArrayTextCarrier::StaticStruct()->GetPathName();V.StructValue.InitializeAs(FDasArrayTextCarrier::StaticStruct());V.StructValue.GetMutable<FDasArrayTextCarrier>().Values=Value;R->DispatchScalarEvent(SubscriptionId,V);}}
