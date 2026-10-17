#include "DasCallbackTypes.h"
#include "DasTypeRegistry.h"
#include "DasCallbackPayload.h"
#include "UObject/GarbageCollection.h"
#include "Components/SceneComponent.h"
#include "UObject/Package.h"
#include "DasCallFrame.h"
#include "DasObjectFixture.h"
#include "DasProbeActor.h"
#include "DasDelegateSignatures.h"
#include "DasContainerTypes.h"
#include "DasIntDelegateProxy.h"
#include "DasPropertyTypes.h"
#include "DasPropertyValues.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "UObject/StructOnScope.h"
#include <limits>

bool CheckDasCallbackTypes(FString& Error)
{
    auto Fail=[&](const FString& Why){Error=Why;return false;};
    TSet<FString> Names;int32 Arrays=0;
    for(const auto& Type:DasCallbackTypes()){
        if(Names.Contains(Type.Name))return Fail(TEXT("Duplicate callback capability"));
        Names.Add(Type.Name);
        auto* Snapshot=Type.Snapshot?Type.Snapshot():nullptr;
        if(FindDasCallbackType(Type.Name)!=&Type || FindDasCallbackType(Type.Signature,Snapshot)!=&Type)return Fail(TEXT("Ambiguous callback ABI lookup"));
        for(const FName ProxyName:{Type.ValueProxy,Type.ConstRefProxy}){
            if(ProxyName.IsNone())continue;
            auto* Proxy=UDasIntDelegateProxy::StaticClass()->FindFunctionByName(ProxyName);
            if(!Proxy || Proxy->GetReturnProperty() || Proxy->NumParms!=(Type.Signature==EDasMethodSignature::Void?0:1))return Fail(TEXT("Missing or incompatible callback proxy: ")+ProxyName.ToString());
            for(TFieldIterator<FProperty> It(Proxy);It;++It)if(It->HasAnyPropertyFlags(CPF_Parm))
                if(DasCallbackTypeForProperty(*It)!=&Type)return Fail(TEXT("Callback proxy disagrees with capability: ")+ProxyName.ToString());
        }
        if(!FString(Type.Name).StartsWith(TEXT("array_")))continue;
        ++Arrays;
        const auto* Values=Snapshot?Snapshot->FindPropertyByName(TEXT("Values")):nullptr;
        if(!Type.bTimer || !Type.ValueProxy.IsNone() || Type.ConstRefProxy.IsNone() || DasCallbackTypeForProperty(Values)!=&Type)return Fail(TEXT("Array callback is missing a transport or proxy"));
        FDasScalarValue V;V.Kind=int32(EDasMethodSignature::Struct);V.StructValue.InitializeAs(Snapshot);V.ObjectClass=Snapshot->GetPathName();
        for(auto Usage:{EDasCallbackUsage::Timer,EDasCallbackUsage::Event})if(!ValidateDasCallbackPayload(V,Usage,Error))return false;
        V.ObjectClass=TEXT("/Script/DasRuntime.WrongIdentity");if(ValidateDasCallbackPayload(V,EDasCallbackUsage::Timer,Error))return Fail(TEXT("Callback accepted mismatched snapshot identity"));
        V.ObjectClass=Snapshot->GetPathName();auto* A=CastFieldChecked<FArrayProperty>(Values);
        FScriptArrayHelper Elements(A,A->ContainerPtrToValuePtr<void>(V.StructValue.GetMutableMemory()));Elements.AddValues(4097);
        if(ValidateDasCallbackPayload(V,EDasCallbackUsage::Event,Error))return Fail(TEXT("Callback accepted oversized array"));
    }
    if(Arrays!=8)return Fail(TEXT("Queued callbacks must cover all eight primitive array types"));
    for(const TCHAR* Unsupported:{TEXT("array_struct"),TEXT("array_object"),TEXT("class"),TEXT("map"),TEXT("set"),TEXT("unknown")})
        if(FindDasCallbackType(Unsupported))return Fail(TEXT("Unexpected callback capability"));
    FDasScalarValue V;V.Kind=int32(EDasMethodSignature::Object);
    if(!ValidateDasCallbackPayload(V,EDasCallbackUsage::Timer,Error) || !ValidateDasCallbackPayload(V,EDasCallbackUsage::Event,Error))return Fail(TEXT("Callback object ownership policy changed"));
    V={};V.Kind=int32(EDasMethodSignature::Float);V.FloatValue=std::numeric_limits<float>::infinity();
    if(ValidateDasCallbackPayload(V,EDasCallbackUsage::Timer,Error))return Fail(TEXT("Nonfinite callback scalar accepted"));
    V={};V.Kind=int32(EDasMethodSignature::Text);V.TextValue=FText::AsCultureInvariant(FString::ChrN(65537,TEXT('x')));
    if(ValidateDasCallbackPayload(V,EDasCallbackUsage::Event,Error))return Fail(TEXT("Oversized callback text accepted"));
    V={};V.Kind=int32(EDasMethodSignature::Struct);V.ObjectClass=FDasArrayTextCarrier::StaticStruct()->GetPathName();V.StructValue.InitializeAs(FDasArrayTextCarrier::StaticStruct());
    auto& Texts=V.StructValue.GetMutable<FDasArrayTextCarrier>().Values;for(int I=0;I<9;++I)Texts.Add(FText::AsCultureInvariant(FString::ChrN(65536,TEXT('x'))));
    if(ValidateDasCallbackPayload(V,EDasCallbackUsage::Event,Error))return Fail(TEXT("Aggregate callback text budget not enforced"));
    V={};V.Kind=256;
    if(ValidateDasCallbackPayload(V,EDasCallbackUsage::Timer,Error))return Fail(TEXT("Callback ABI kind was truncated"));
    V={};V.Kind=int32(EDasMethodSignature::Enum);V.ObjectClass=StaticEnum<EDasCallMode>()->GetPathName();V.IntValue=7;
    if(!ValidateDasCallbackPayload(V,EDasCallbackUsage::Timer,Error))return false;
    V.IntValue=254;if(ValidateDasCallbackPayload(V,EDasCallbackUsage::Event,Error))return Fail(TEXT("Unknown callback enum member accepted"));
    V={};V.Kind=int32(EDasMethodSignature::Struct);V.ObjectClass=FDasCallPayload::StaticStruct()->GetPathName();V.StructValue.InitializeAs(FDasCallPayload::StaticStruct());
    if(!ValidateDasCallbackPayload(V,EDasCallbackUsage::Timer,Error))return false;
    V.StructValue.GetMutable<FDasCallPayload>().Metrics.Ratio=std::numeric_limits<float>::infinity();
    if(ValidateDasCallbackPayload(V,EDasCallbackUsage::Event,Error))return Fail(TEXT("Nonfinite nested callback field accepted"));
    if(!ValidateDasCallbackTypePath(TEXT("struct"),FDasObjectSnapshot::StaticStruct()->GetPathName(),Error))return false;
    if(ValidateDasCallbackTypePath(TEXT("enum"),TEXT("/Script/DasRuntime.DoesNotExist"),Error))return Fail(TEXT("Missing callback enum accepted"));
    for(const auto Kind:{EDasMethodSignature::Enum,EDasMethodSignature::Struct,EDasMethodSignature::Object})for(const TCHAR* Direction:{TEXT("in"),TEXT("inout")}){
        // Mutable ref callbacks are forbidden, but a const-ref native event uses
        // the same physical layout as a delegate ref parameter with ConstParm.
        FDasDeclaredMethod M;M.Name=TEXT("Invoke");M.ReturnType.Kind=TEXT("void");FDasDeclaredParameter P;P.Name=TEXT("Value");P.Direction=Direction;P.Type.Kind=Kind==EDasMethodSignature::Object?TEXT("object"):Kind==EDasMethodSignature::Enum?TEXT("enum"):TEXT("struct");P.Type.TypePath=Kind==EDasMethodSignature::Object?UDasEventInterface::StaticClass()->GetPathName():Kind==EDasMethodSignature::Enum?StaticEnum<EDasCallMode>()->GetPathName():FDasCallPayload::StaticStruct()->GetPathName();M.Parameters.Add(P);
        TStrongObjectPtr<UFunction> Sig(MakeDasDelegateSignature(M,true,GetTransientPackage(),NAME_None,Error));if(!Sig)return false;
        TStrongObjectPtr<UDasIntDelegateProxy> Negative(NewObject<UDasIntDelegateProxy>());
        const FString WrongPath=Kind==EDasMethodSignature::Object?UObject::StaticClass()->GetPathName():Kind==EDasMethodSignature::Struct?FDasCallMetrics::StaticStruct()->GetPathName():StaticEnum<EDasCallbackMode>()->GetPathName();
        if(!Negative->ConfigureDynamic(Sig.Get(),WrongPath,Kind,Error).IsNone())return Fail(TEXT("Dynamic receiver accepted another concrete type"));
        if(P.Direction==TEXT("inout") && !Negative->ConfigureDynamic(Sig.Get(),P.Type.TypePath,Kind,Error).IsNone())return Fail(TEXT("Dynamic receiver accepted mutable ref source"));
        if(P.Direction==TEXT("inout"))for(TFieldIterator<FProperty> It(Sig.Get());It;++It)if(It->HasAnyPropertyFlags(CPF_Parm))It->SetPropertyFlags(CPF_ConstParm);
        TStrongObjectPtr<UDasIntDelegateProxy> Proxy(NewObject<UDasIntDelegateProxy>());
        const auto Name=Proxy->ConfigureDynamic(Sig.Get(),P.Type.TypePath,Kind,Error);if(Name.IsNone())return false;
        auto* Function=Proxy->FindFunction(Name);if(!Function || !Function->IsSignatureCompatibleWith(Sig.Get()) || Proxy->ConfigureDynamic(Sig.Get(),P.Type.TypePath,Kind,Error)!=Name)return Fail(TEXT("Dynamic native callback proxy layout/reuse mismatch"));
    }
    {
        UObject* Object=NewObject<UDasTypeRegistry>();TWeakObjectPtr<UObject> Weak(Object);
        FDasScalarValue Input;Input.Kind=12;Input.ObjectClass=FDasNestedObjectSnapshot::StaticStruct()->GetPathName();Input.StructValue.InitializeAs(FDasNestedObjectSnapshot::StaticStruct());
        auto& Native=Input.StructValue.GetMutable<FDasNestedObjectSnapshot>();Native.Details.Object=Object;Native.Items.AddDefaulted_GetRef().Object=Object;
        const uint8* SourceMemory=Input.StructValue.GetMemory();
        TSharedPtr<FDasCallbackPayload> Capture=MakeShared<FDasCallbackPayload>(Input);
        if(Capture->GetValue().StructValue.GetMemory()==SourceMemory)return Fail(TEXT("Borrowed callback capture aliased mutable source"));
        Native.Details.Object=nullptr;Native.Items.Reset();
        if(Capture->GetValue().StructValue.Get<FDasNestedObjectSnapshot>().Details.Object!=Object || Capture->GetValue().StructValue.Get<FDasNestedObjectSnapshot>().Items.Num()!=1)return Fail(TEXT("Borrowed callback capture lost isolation"));
        Input={};
        CollectGarbage(RF_NoFlags);
        if(!Weak.IsValid() || !ValidateDasCallbackPayload(Capture->GetValue(),EDasCallbackUsage::Timer,Error))return Fail(TEXT("Nested queued object lost during GC"));
        Capture.Reset();CollectGarbage(RF_NoFlags);if(Weak.IsValid())return Fail(TEXT("Queued callback retained object after release"));
    }
    {
        TStrongObjectPtr<USceneComponent> Component(NewObject<USceneComponent>());
        FDasScalarValue Input;Input.Kind=8;Input.ObjectClass=USceneComponent::StaticClass()->GetPathName();Input.ObjectValue=Component.Get();
        if(!ValidateDasCallbackPayload(Input,EDasCallbackUsage::Timer,Error))return false;
        Input.ObjectClass=AActor::StaticClass()->GetPathName();if(ValidateDasCallbackPayload(Input,EDasCallbackUsage::Timer,Error))return Fail(TEXT("Queued object class mismatch accepted"));
        Input.ObjectClass=USceneComponent::StaticClass()->GetPathName();auto Capture=MakeShared<FDasCallbackPayload>(Input);Component->DestroyComponent();
        if(ValidateDasCallbackPayload(Input,EDasCallbackUsage::Event,Error))return Fail(TEXT("Destroyed component callback accepted"));
        CollectGarbage(RF_NoFlags);if(Capture->IsAlive())return Fail(TEXT("Destroyed capture identity was lost after GC"));
    }
    {
        TStrongObjectPtr<UDasReferenceFixture> Implementer(NewObject<UDasReferenceFixture>());
        FDasScalarValue Input;Input.Kind=8;Input.ObjectClass=UDasEventInterface::StaticClass()->GetPathName();Input.ObjectValue=Implementer.Get();
        if(!ValidateDasCallbackPayload(Input,EDasCallbackUsage::Timer,Error))return Fail(TEXT("Callback interface implementer rejected"));
        Input.ObjectValue=GetTransientPackage();if(ValidateDasCallbackPayload(Input,EDasCallbackUsage::Timer,Error))return Fail(TEXT("Callback interface nonimplementer accepted"));
        Input.ObjectValue=nullptr;if(!ValidateDasCallbackPayload(Input,EDasCallbackUsage::Timer,Error))return Fail(TEXT("Null callback interface rejected"));
    }
    {
        TArray<FDasDeclaredType> Containers;
        auto Leaf=[](const TCHAR* Kind,const FString& Path=FString()){auto T=MakeShared<FDasDeclaredType>();T->Kind=Kind;T->TypePath=Path;return T;};
        for(const auto& Element:{Leaf(TEXT("object"),AActor::StaticClass()->GetPathName()),Leaf(TEXT("enum"),StaticEnum<EDasCallMode>()->GetPathName()),Leaf(TEXT("struct"),FDasNestedObjectSnapshot::StaticStruct()->GetPathName())}){FDasDeclaredType T;T.Kind=TEXT("array");T.Element=Element;Containers.Add(T);}
        for(const auto& Key:{Leaf(TEXT("int")),Leaf(TEXT("string")),Leaf(TEXT("enum"),StaticEnum<EDasCallMode>()->GetPathName())}){FDasDeclaredType T;T.Kind=TEXT("set");T.Element=Key;Containers.Add(T);}
        for(const auto& Value:{Leaf(TEXT("object"),UObject::StaticClass()->GetPathName()),Leaf(TEXT("struct"),FDasNestedObjectSnapshot::StaticStruct()->GetPathName()),Leaf(TEXT("text"))}){FDasDeclaredType T;T.Kind=TEXT("map");T.Key=Leaf(TEXT("string"));T.Value=Value;Containers.Add(T);}
        for(const auto& T:Containers){auto* Carrier=DasContainerCarrier(T);if(!Carrier)return Fail(TEXT("Missing callback container carrier"));
            FDasScalarValue Value;Value.Kind=12;Value.ObjectClass=Carrier->GetPathName();Value.StructValue.InitializeAs(Carrier);
            if(!ValidateDasCallbackPayload(Value,EDasCallbackUsage::Event,Error) || !ValidateDasCallbackPayload(Value,EDasCallbackUsage::Timer,Error))return false;
            auto* Values=Carrier->FindPropertyByName(TEXT("Values"));const auto* Capability=DasCallbackTypeForProperty(Values);if(!Capability || !Capability->bDynamic)return Fail(TEXT("Generic container receiver not selected"));
            FDasDeclaredMethod M;M.Name=TEXT("Container");M.ReturnType.Kind=TEXT("void");FDasDeclaredParameter P;P.Name=TEXT("Value");P.Type=T;P.Direction=TEXT("in");M.Parameters.Add(P);
            TStrongObjectPtr<UFunction> Signature(MakeDasDelegateSignature(M,true,GetTransientPackage(),NAME_None,Error));if(!Signature)return false;
            TStrongObjectPtr<UDasIntDelegateProxy> Proxy(NewObject<UDasIntDelegateProxy>());
            const auto Name=Proxy->ConfigureDynamic(Signature.Get(),Value.ObjectClass,EDasMethodSignature::Struct,Error);if(Name.IsNone())return false;
            auto* ContainerReceiver=Proxy->FindFunction(Name);
            if(!ContainerReceiver || !ContainerReceiver->IsSignatureCompatibleWith(Signature.Get(),0))return Fail(TEXT("Container callback clone changed property flags"));
            FStructOnScope Frame(Signature.Get());Proxy->ProcessEvent(ContainerReceiver,Frame.GetStructMemory());

            // Exercise physical native by-value and const-ref frames, independently
            // of Kismet's input-array convention. Mutable refs must be rejected.
            auto* Input=CastFieldChecked<FProperty>(Signature->ChildProperties);
            for(const bool ConstRef:{false,true}){
                Input->ClearPropertyFlags(CPF_OutParm|CPF_ReferenceParm|CPF_ConstParm);
                if(ConstRef)Input->SetPropertyFlags(CPF_OutParm|CPF_ReferenceParm|CPF_ConstParm);
                TStrongObjectPtr<UDasIntDelegateProxy> Physical(NewObject<UDasIntDelegateProxy>());
                const auto PhysicalName=Physical->ConfigureDynamic(Signature.Get(),Value.ObjectClass,EDasMethodSignature::Struct,Error);if(PhysicalName.IsNone())return false;
                FStructOnScope Arguments(Signature.Get());Physical->ProcessEvent(Physical->FindFunction(PhysicalName),Arguments.GetStructMemory());
            }
            Input->ClearPropertyFlags(CPF_ConstParm);Input->SetPropertyFlags(CPF_OutParm|CPF_ReferenceParm);
            TStrongObjectPtr<UDasIntDelegateProxy> Mutable(NewObject<UDasIntDelegateProxy>());
            if(!Mutable->ConfigureDynamic(Signature.Get(),Value.ObjectClass,EDasMethodSignature::Struct,Error).IsNone())return Fail(TEXT("Container receiver accepted mutable ref/out"));
            Value.ObjectClass=FDasArrayIntCarrier::StaticStruct()->GetPathName();if(ValidateDasCallbackPayload(Value,EDasCallbackUsage::Timer,Error))return Fail(TEXT("Container callback accepted another carrier identity"));
        }
    }
    // Real UHT const-ref array parameters carry ZeroConstructor, unlike some
    // factory-built signatures. Exercise the exact regression in both targets.
    for(const TCHAR* Field:{TEXT("OnDasArrayIntSignal"),TEXT("OnDasArrayFloatSignal"),TEXT("OnDasArrayDoubleSignal"),TEXT("OnDasArrayBoolSignal"),TEXT("OnDasArrayStringSignal"),TEXT("OnDasArrayNameSignal"),TEXT("OnDasArrayByteSignal"),TEXT("OnDasArrayTextSignal"),TEXT("OnDasCallbackActors"),TEXT("OnDasCallbackRecordRef")}){
        auto* Event=FindFProperty<FMulticastDelegateProperty>(ADasProbeActor::StaticClass(),Field);
        UFunction* Signature=Event?Event->SignatureFunction.Get():nullptr;
        auto* Parameter=Signature?CastField<FProperty>(Signature->ChildProperties):nullptr;
        auto* Snapshot=DasCallbackSnapshotForProperty(Parameter);
        if(!Snapshot)return Fail(TEXT("Missing native callback regression fixture: ")+FString(Field));
        TStrongObjectPtr<UDasIntDelegateProxy> Proxy(NewObject<UDasIntDelegateProxy>());
        const auto Name=Proxy->ConfigureDynamic(Signature,Snapshot->GetPathName(),EDasMethodSignature::Struct,Error);
        if(Name.IsNone())return false;
        auto* Receiver=Proxy->FindFunction(Name);
        if(!Receiver || !Receiver->IsSignatureCompatibleWith(Signature,0) || Proxy->ConfigureDynamic(Signature,Snapshot->GetPathName(),EDasMethodSignature::Struct,Error)!=Name)
            return Fail(TEXT("Native callback clone lost flags or cache identity: ")+FString(Field));
        FStructOnScope Frame(Signature);
        FDasScalarValue Payload;
        if(!ReadDasPropertyValue(Parameter,Parameter->ContainerPtrToValuePtr<void>(Frame.GetStructMemory()),Payload,Error))return false;
        if(Payload.Kind!=int32(EDasMethodSignature::Struct) || Payload.ObjectClass!=Snapshot->GetPathName())
            return Fail(TEXT("Native callback snapshot lost dispatch identity: ")+FString(Field));
        Proxy->ProcessEvent(Receiver,Frame.GetStructMemory());
        TStrongObjectPtr<UFunction> Mutable(DuplicateObject<UFunction>(Signature,GetTransientPackage()));
        auto* MutableParameter=CastFieldChecked<FProperty>(Mutable->ChildProperties);
        MutableParameter->ClearPropertyFlags(CPF_ConstParm);
        MutableParameter->SetPropertyFlags(CPF_OutParm|CPF_ReferenceParm);
        if(!Proxy->ConfigureDynamic(Mutable.Get(),Snapshot->GetPathName(),EDasMethodSignature::Struct,Error).IsNone())
            return Fail(TEXT("Native callback accepted mutable ref/out: ")+FString(Field));
    }
    Error.Reset();return true;
}
