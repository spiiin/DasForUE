#pragma once
#include "DasRuntime.h"
#include "DasContainerTypes.h"
#include "DasAuthoring.h"
#include "UObject/StrongObjectPtr.h"
#include "DasSchema.h"
#include "DasTypeRegistry.h"
#include "DasProbeActor.h"
#include "DasProbeInterface.h"
#include "Misc/Paths.h"
#include "DasAuthoredInterfaceClass.h"
#include "UObject/StructOnScope.h"
#include "UObject/Script.h"
#include "UObject/UnrealType.h"
inline void RunDasAuthoredInterfaceNativeValues(FDasRuntimeModule& R, UObject* Actor, const FString& Script,
                                                TFunctionRef<void(bool, const TCHAR*)> Check, bool Editor)
{
    auto Echo = [&](const TCHAR* Name, EDasMethodSignature Kind, const FDasScalarValue& Input)
    {
        const auto Value = R.RunNamedFramePersistent(
            R.ScriptPath(Script), TEXT("ue_CounterActor_")+ FString(Name), {Input}, Kind, Actor, Input.ObjectClass);
        bool Match = Value.bSucceeded;
        if (Kind == EDasMethodSignature::Object)
            Match &= Value.ObjectValue.Get() == Input.ObjectValue.Get();
        else if (Kind == EDasMethodSignature::Vector)
            Match &= Value.VectorValue == Input.VectorValue;
        else if (Kind == EDasMethodSignature::Rotator)
            Match &= Value.RotatorValue == Input.RotatorValue;
        else if (Kind == EDasMethodSignature::Transform)
            Match &= Value.TransformValue.Equals(Input.TransformValue);
        else if (Kind == EDasMethodSignature::Enum)
            Match &= Value.Value == Input.IntValue;
        else if (Kind == EDasMethodSignature::Struct)
            Match &= Value.StructValue.IsValid() &&
                     Value.StructValue.GetScriptStruct() == Input.StructValue.GetScriptStruct() &&
                     Input.StructValue.GetScriptStruct()->CompareScriptStruct(Value.StructValue.GetMemory(),
                                                                              Input.StructValue.GetMemory(), 0);
        if (!Value.bSucceeded)
            UE_LOG(LogTemp, Error, TEXT("Authored native signature %s: %s"), Name, *Value.Error);
        Check(Match, *(TEXT("authored_interface_native_frame_")+ FString(Name)));
        auto* Method = Actor->FindFunction(*FString(Name));
        FStructOnScope Memory(Method);
        auto* In = Method ? Method->FindPropertyByName(TEXT("value")) : nullptr;
        auto* Out = Method ? Method->GetReturnProperty() : nullptr;
        if (!In || !Out)
        {
            Check(false, TEXT("authored_interface_native_signature_pins_missing"));
            return;
        }
        if (Kind == EDasMethodSignature::Object)
            CastFieldChecked<FObjectProperty>(In)->SetObjectPropertyValue_InContainer(Memory.GetStructMemory(),
                                                                                      Input.ObjectValue.Get());
        else if (Kind == EDasMethodSignature::Enum)
            SetDasEnumPropertyNumber(In, In->ContainerPtrToValuePtr<void>(Memory.GetStructMemory()), Input.IntValue);
        else
        {
            const void* Source = Kind == EDasMethodSignature::Vector    ? static_cast<const void*>(&Input.VectorValue)
                                 : Kind == EDasMethodSignature::Rotator ? static_cast<const void*>(&Input.RotatorValue)
                                 : Kind == EDasMethodSignature::Transform
                                     ? static_cast<const void*>(&Input.TransformValue)
                                     : Input.StructValue.GetMemory();
            In->CopyCompleteValue(In->ContainerPtrToValuePtr<void>(Memory.GetStructMemory()), Source);
        }
#if WITH_EDITOR
        if (Editor)
        {
            FEditorScriptExecutionGuard Guard;
            Actor->ProcessEvent(Method, Memory.GetStructMemory());
        }
        else
#endif
        {
            Actor->ProcessEvent(Method, Memory.GetStructMemory());
        }
        if (Kind == EDasMethodSignature::Object)
            Match = CastFieldChecked<FObjectProperty>(Out)->GetObjectPropertyValue_InContainer(
                        Memory.GetStructMemory()) == Input.ObjectValue.Get();
        else if (Kind == EDasMethodSignature::Enum)
            Match = DasEnumPropertyNumber(Out, Out->ContainerPtrToValuePtr<void>(Memory.GetStructMemory())) ==
                    Input.IntValue;
        else
            Match = In->Identical(In->ContainerPtrToValuePtr<void>(Memory.GetStructMemory()),
                                  Out->ContainerPtrToValuePtr<void>(Memory.GetStructMemory()), 0);
        Check(Match, *(TEXT("authored_interface_native_ProcessEvent_")+ FString(Name)));
    };
    FDasScalarValue Object;
    Object.Kind = 8;
    Object.ObjectClass = TEXT("/Script/Engine.Actor");
    Object.ObjectValue = Actor;
    Echo(TEXT("EchoActor"), EDasMethodSignature::Object, Object);
    Object.ObjectClass = TEXT("/Script/CoreUObject.Object");
    Echo(TEXT("EchoObject"), EDasMethodSignature::Object, Object);
    Object.ObjectValue = nullptr;
    Echo(TEXT("EchoObject"), EDasMethodSignature::Object, Object);
    FDasScalarValue Vector;
    Vector.Kind = 9;
    Vector.VectorValue = FVector(1.25, -2.5, 3.75);
    Echo(TEXT("EchoVector"), EDasMethodSignature::Vector, Vector);
    FDasScalarValue Rotator;
    Rotator.Kind = 10;
    Rotator.RotatorValue = FRotator(15, 30, 45);
    Echo(TEXT("EchoRotator"), EDasMethodSignature::Rotator, Rotator);
    FDasScalarValue Transform;
    Transform.Kind = 11;
    Transform.TransformValue = FTransform(FRotator(15, 30, 45), FVector(1.25, -2.5, 3.75), FVector(2, 3, 4));
    Echo(TEXT("EchoTransform"), EDasMethodSignature::Transform, Transform);
    FDasScalarValue Mode;
    Mode.Kind = 13;
    Mode.ObjectClass = TEXT("/Script/DasRuntime.EDasCallMode");
    Mode.IntValue = 7;
    Echo(TEXT("EchoMode"), EDasMethodSignature::Enum, Mode);
    if (auto* Owner = Cast<ADasProbeActor>(Actor); Owner && Owner->TypeRegistry)
    {
        FString RegistryError;
        auto* Enum = Cast<UEnum>(Owner->TypeRegistry->Resolve(TEXT("interfacevalues.State"), true, RegistryError));
        auto* Struct =
            Cast<UScriptStruct>(Owner->TypeRegistry->Resolve(TEXT("interfacevalues.Settings"), false, RegistryError));
        if (Enum && Struct)
        {
            FDasScalarValue State;
            State.Kind = 13;
            State.ObjectClass = Enum->GetPathName();
            State.IntValue = 42;
            Echo(TEXT("EchoState"), EDasMethodSignature::Enum, State);
            FDasScalarValue Settings;
            Settings.Kind = 12;
            Settings.ObjectClass = Struct->GetPathName();
            Settings.StructValue.InitializeAs(Struct);
            for (TFieldIterator<FProperty> It(Struct); It; ++It)
            {
                if (auto* Count = CastField<FIntProperty>(*It))
                    Count->SetPropertyValue_InContainer(Settings.StructValue.GetMutableMemory(), 33);
                if (auto* Title = CastField<FStrProperty>(*It))
                    Title->SetPropertyValue_InContainer(Settings.StructValue.GetMutableMemory(), TEXT("Registry 世界"));
            }
            Echo(TEXT("EchoSettings"), EDasMethodSignature::Struct, Settings);
        }
        else
            Check(false, TEXT("authored_interface_registered_types_missing"));
    }
    auto* Payload = FindObject<UScriptStruct>(nullptr, TEXT("/Script/DasRuntime.DasCallPayload"));
    if (!Payload)
    {
        Check(false, TEXT("authored_interface_native_payload_missing"));
        return;
    }
    FDasScalarValue Data;
    Data.Kind = 12;
    Data.ObjectClass = Payload->GetPathName();
    Data.StructValue.InitializeAs(Payload);
    CastFieldChecked<FIntProperty>(Payload->FindPropertyByName(TEXT("Count")))
                                       ->SetPropertyValue_InContainer(Data.StructValue.GetMutableMemory(), 37);
    CastFieldChecked<FStrProperty>(Payload->FindPropertyByName(TEXT("Label")))
                                       ->SetPropertyValue_InContainer(Data.StructValue.GetMutableMemory(),
                                                                      TEXT("Signature 世界"));
    CastFieldChecked<FDoubleProperty>(Payload->FindPropertyByName(TEXT("Weight")))
                                          ->SetPropertyValue_InContainer(Data.StructValue.GetMutableMemory(),
                                                                         1.23456789012345);
    Echo(TEXT("EchoPayload"), EDasMethodSignature::Struct, Data);
}
inline bool RunDasAuthoredInterfaceValues(FDasRuntimeModule& R, UObject* Actor, UClass* Interface,
                                          const FString& Script, TFunctionRef<void(bool, const TCHAR*)> Check,
                                          bool Editor)
{
    bool Pass = true;
    auto Expect = [&](bool V, const TCHAR* Name)
    {
        Pass &= V;
        Check(V, Name);
    };
    RunDasAuthoredInterfaceNativeValues(R, Actor, Script, Expect, Editor);
    auto* Identity = Cast<UDasAuthoredInterfaceClass>(Interface);
    Expect(Identity && Identity->StableId == TEXT("authoredinterface_")+ FPaths::GetBaseFilename(Script).RightChop(15) +
                                                  TEXT(".CounterContract") && !Identity->ContractJson.IsEmpty(),
                                                       TEXT("authored_interface_persistent_identity"));
    Expect(Actor->GetClass()->ImplementsInterface(Interface), TEXT("authored_interface_native_membership"));
    auto Supports = R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_CounterActor_SupportsContract"), {},
                                                                         EDasMethodSignature::Bool, Actor);
    Expect(Supports.bSucceeded && Supports.BoolValue, TEXT("body_authored_interface_type_token_membership"));
    auto SupportsNative = R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_CounterActor_SupportsNative"), {},
                                                                               EDasMethodSignature::Bool, Actor);
    Expect(SupportsNative.bSucceeded && SupportsNative.BoolValue,
           TEXT("body_native_interface_short_string_membership"));
    Expect(Interface->FindFunctionByName(
        TEXT("Query"))->HasAllFunctionFlags(FUNC_BlueprintPure | FUNC_Const) &&
        Actor->FindFunction(TEXT("Query"))->HasAllFunctionFlags(FUNC_BlueprintPure | FUNC_Const),
                            TEXT("pure_interface_runtime_flags"));
    FDasScalarValue Input;
    Input.Kind = 0;
    Input.IntValue = 5;
    auto Query = R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_CounterActor_Query"), {Input},
                                                                      EDasMethodSignature::Int, Actor);
    Expect(Query.bSucceeded && Query.Value == 12, TEXT("authored_interface_typed_query"));
    auto Event = [&](UFunction* F, void* Memory)
    {
#if WITH_EDITOR
        if (Editor)
        {
            FEditorScriptExecutionGuard Guard;
            Actor->ProcessEvent(F, Memory);
        }
        else
#endif
        {
            Actor->ProcessEvent(F, Memory);
        }
    };
    auto* Score = FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("Score"));
    Expect(Score && Actor->GetClass()->ImplementsInterface(UDasEventInterface::StaticClass()),
           TEXT("interface_events_native_membership_and_state"));
    if (Score)
    {
        auto* Add = Actor->FindFunction(TEXT("AddScore"));
        FStructOnScope EventArgs(Add);
        FindFProperty<FIntProperty>(Add, TEXT("amount"))->SetPropertyValue_InContainer(EventArgs.GetStructMemory(), 5);
        Event(Add, EventArgs.GetStructMemory());
        Expect(Score->GetPropertyValue_InContainer(Actor) == 5, TEXT("authored_interface_event_scalar_ProcessEvent"));
        auto* Clear = Actor->FindFunction(TEXT("ClearScore"));
        FStructOnScope ClearArgs(Clear);
        Event(Clear, ClearArgs.GetStructMemory());
        Expect(Score->GetPropertyValue_InContainer(Actor) == 0, TEXT("authored_interface_event_no_parameters"));
        auto NativeEvent = [&](bool Reset)
        {
#if WITH_EDITOR
            if (Editor)
            {
                FEditorScriptExecutionGuard Guard;
                if (Reset)
                    IDasEventInterface::Execute_ResetCounter(Actor);
                else
                    IDasEventInterface::Execute_Notify(Actor, 9);
            }
            else
#endif
            {
                if (Reset)
                    IDasEventInterface::Execute_ResetCounter(Actor);
                else
                    IDasEventInterface::Execute_Notify(Actor, 9);
            }
        };
        NativeEvent(false);
        Expect(Score->GetPropertyValue_InContainer(Actor) == 9, TEXT("native_interface_event_Execute_input"));
        NativeEvent(true);
        Expect(Score->GetPropertyValue_InContainer(Actor) == 0, TEXT("native_interface_event_Execute_no_parameters"));
        auto* Labels = Actor->FindFunction(TEXT("PushLabels"));
        FStructOnScope LabelArgs(Labels);
        auto* Items = FindFProperty<FArrayProperty>(Labels, TEXT("Values"));
        FScriptArrayHelper Values(Items, Items->ContainerPtrToValuePtr<void>(LabelArgs.GetStructMemory()));
        Values.Resize(2);
        CastFieldChecked<FStrProperty>(Items->Inner)->SetPropertyValue(Values.GetRawPtr(0), TEXT("One 世界"));
        CastFieldChecked<FStrProperty>(Items->Inner)->SetPropertyValue(Values.GetRawPtr(1), TEXT("Two"));
        FindFProperty<FStrProperty>(
            Labels, TEXT("message"))->SetPropertyValue_InContainer(LabelArgs.GetStructMemory(), TEXT("Event 世界"));
        Event(Labels, LabelArgs.GetStructMemory());
        Expect(Score->GetPropertyValue_InContainer(Actor) == 2 &&
               FindFProperty<FStrProperty>(Actor->GetClass(),
                                           TEXT("LastMessage"))->GetPropertyValue_InContainer(Actor) ==
                                           TEXT("Event 世界") && Values.Num() == 2,
                                                TEXT("authored_interface_event_array_Unicode_input_ownership"));
        Score->SetPropertyValue_InContainer(Actor, 0);
        FDasScalarValue V;
        V.Kind = 0;
        V.IntValue = 12;
        auto Direct = R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_CounterActor_AddScore"), {V},
                                                                           EDasMethodSignature::Void, Actor);
        Expect(Direct.bSucceeded && Score->GetPropertyValue_InContainer(Actor) == 12,
               TEXT("interface_event_typed_void_adapter"));
        Score->SetPropertyValue_InContainer(Actor, 0);
    }
    auto* Label = Actor->FindFunction(TEXT("Label"));
    FStructOnScope Args(Label);
    Event(Label, Args.GetStructMemory());
    Expect(CastFieldChecked<FStrProperty>(Label->GetReturnProperty())
               ->GetPropertyValue_InContainer(Args.GetStructMemory()) ==
           TEXT("Authored 世界"), TEXT("authored_interface_ProcessEvent_Unicode"));
    auto* Stats = Actor->FindFunction(TEXT("Stats"));
    FStructOnScope Params(Stats);
    auto* Counter = FindFProperty<FIntProperty>(Stats, TEXT("Counter"));
    auto* Total = FindFProperty<FIntProperty>(Stats, TEXT("Total"));
    Counter->SetPropertyValue_InContainer(Params.GetStructMemory(), 10);
    Event(Stats, Params.GetStructMemory());
    Expect(Counter->GetPropertyValue_InContainer(Params.GetStructMemory()) == 13 &&
               Total->GetPropertyValue_InContainer(Params.GetStructMemory()) == 26,
           TEXT("authored_interface_out_ref_copyback"));
    auto* Weights = Actor->FindFunction(TEXT("Weights"));
    FStructOnScope ArrayArgs(Weights);
    auto* In = FindFProperty<FArrayProperty>(Weights, TEXT("Values"));
    auto* Out = CastFieldChecked<FArrayProperty>(Weights->GetReturnProperty());
    FScriptArrayHelper InputArray(In, In->ContainerPtrToValuePtr<void>(ArrayArgs.GetStructMemory()));
    InputArray.Resize(1);
    CastFieldChecked<FDoubleProperty>(In->Inner)->SetPropertyValue(InputArray.GetRawPtr(0), 2.5);
    Event(Weights, ArrayArgs.GetStructMemory());
    FScriptArrayHelper ResultArray(Out, Out->ContainerPtrToValuePtr<void>(ArrayArgs.GetStructMemory()));
    Expect(ResultArray.Num() == 2 &&
               CastFieldChecked<FDoubleProperty>(Out->Inner)->GetPropertyValue(ResultArray.GetRawPtr(1)) ==
                   1.23456789012345 &&
               InputArray.Num() == 1,
           TEXT("authored_interface_array_return_precision_and_ownership"));
    if (!Editor)
    {
        auto* Owner = Cast<AActor>(Actor);
        for (const auto& Candidate : Owner->GetWorld()->PersistentLevel->Actors)
            if (Candidate && Candidate->GetFName() == TEXT("WrapperReceiver"))
            {
                const FString Id = FPaths::GetBaseFilename(Script).RightChop(15);
                auto* Evaluate = Candidate->FindFunction(TEXT("EvaluatePure"));
                FStructOnScope PureArgs(Evaluate);
                if (Evaluate)
                    Candidate->ProcessEvent(Evaluate, PureArgs.GetStructMemory());
                Expect(Evaluate &&
                       FindFProperty<FIntProperty>(
                           Evaluate, TEXT("ReturnValue"))->GetPropertyValue_InContainer(PureArgs.GetStructMemory()) ==
                           123,
                           TEXT("pure_interface_Blueprint_data_node_cooked"));
                const auto Wrapped = R.RunFile(R.ScriptPath(TEXT("authored_wrapper_check_")+ Id + TEXT(".das")), 0,
                                                            false, Candidate.Get());
                if (!Wrapped.bSucceeded)
                    UE_LOG(LogTemp, Error, TEXT("Authored wrappers: %s"), *Wrapped.Error);
                Expect(Wrapped.bSucceeded && Wrapped.Value == 1, TEXT("generated_authored_interface_calls_cooked"));
            }
    }
    {
        auto* ArrayProbeF = Actor->FindFunction(TEXT("EchoDetails"));
        FStructOnScope ArrayParams(ArrayProbeF);
        auto* ArrayProbeIn = FindFProperty<FArrayProperty>(ArrayProbeF, TEXT("Values"));
        auto* ArrayProbeOut = CastFieldChecked<FArrayProperty>(ArrayProbeF->GetReturnProperty());
        auto* ArrayProbeElement = CastFieldChecked<FStructProperty>(ArrayProbeIn->Inner)->Struct.Get();
        auto* ArrayProbeAmount = CastFieldChecked<FIntProperty>(FindDasStructField(ArrayProbeElement, TEXT("Amount")));
        auto* ArrayProbeText = CastFieldChecked<FStrProperty>(FindDasStructField(ArrayProbeElement, TEXT("Text")));
        FScriptArrayHelper ArrayProbeValues(ArrayProbeIn,
                                            ArrayProbeIn->ContainerPtrToValuePtr<void>(ArrayParams.GetStructMemory()));
        ArrayProbeValues.Resize(2);
        ArrayProbeAmount->SetPropertyValue_InContainer(ArrayProbeValues.GetRawPtr(0), 31);
        ArrayProbeAmount->SetPropertyValue_InContainer(ArrayProbeValues.GetRawPtr(1), 47);
        ArrayProbeText->SetPropertyValue_InContainer(ArrayProbeValues.GetRawPtr(0), TEXT("Array 世界"));
        Event(ArrayProbeF, ArrayParams.GetStructMemory());
        FScriptArrayHelper ArrayProbeResults(
            ArrayProbeOut, ArrayProbeOut->ContainerPtrToValuePtr<void>(ArrayParams.GetStructMemory()));
        Expect(ArrayProbeResults.Num() == 2 &&
               ArrayProbeAmount->GetPropertyValue_InContainer(ArrayProbeResults.GetRawPtr(0)) == 31 &&
               ArrayProbeText->GetPropertyValue_InContainer(ArrayProbeResults.GetRawPtr(0)) ==
                   TEXT("Array 世界"), TEXT("authored_struct_array_ProcessEvent_return"));
        if (ArrayProbeResults.Num())
            ArrayProbeAmount->SetPropertyValue_InContainer(ArrayProbeResults.GetRawPtr(0), 999);
        Expect(ArrayProbeAmount->GetPropertyValue_InContainer(ArrayProbeValues.GetRawPtr(0)) == 31,
               TEXT("authored_struct_array_independent_native_copy"));
        auto* ArrayProbeMake = Actor->FindFunction(TEXT("NewDetails"));
        FStructOnScope MakeArgs(ArrayProbeMake);
        Event(ArrayProbeMake, MakeArgs.GetStructMemory());
        auto* ArrayProbeMade = CastFieldChecked<FArrayProperty>(ArrayProbeMake->GetReturnProperty());
        FScriptArrayHelper ArrayProbeFresh(ArrayProbeMade,
                                           ArrayProbeMade->ContainerPtrToValuePtr<void>(MakeArgs.GetStructMemory()));
        Expect(ArrayProbeFresh.Num() == 1 &&
                   ArrayProbeAmount->GetPropertyValue_InContainer(ArrayProbeFresh.GetRawPtr(0)) == 77,
               TEXT("authored_struct_array_return_without_input"));
        auto* ArrayProbeCopy = Actor->FindFunction(TEXT("CopyDetails"));
        FStructOnScope CopyArgs(ArrayProbeCopy);
        auto* CopyIn = FindFProperty<FArrayProperty>(ArrayProbeCopy, TEXT("Values"));
        auto* CopyOut = FindFProperty<FArrayProperty>(ArrayProbeCopy, TEXT("Copies"));
        CopyIn->CopyCompleteValue(CopyIn->ContainerPtrToValuePtr<void>(CopyArgs.GetStructMemory()),
                                  ArrayProbeIn->ContainerPtrToValuePtr<void>(ArrayParams.GetStructMemory()));
        Event(ArrayProbeCopy, CopyArgs.GetStructMemory());
        FScriptArrayHelper ArrayProbeCopies(CopyOut, CopyOut->ContainerPtrToValuePtr<void>(CopyArgs.GetStructMemory()));
        Expect(ArrayProbeCopies.Num() == 2 &&
                   ArrayProbeAmount->GetPropertyValue_InContainer(ArrayProbeCopies.GetRawPtr(1)) == 47,
               TEXT("authored_struct_array_out_copyback"));
        auto* ArrayProbeBump = Actor->FindFunction(TEXT("BumpDetails"));
        FStructOnScope BumpArgs(ArrayProbeBump);
        auto* ArrayProbeRef = FindFProperty<FArrayProperty>(ArrayProbeBump, TEXT("Values"));
        ArrayProbeRef->CopyCompleteValue(ArrayProbeRef->ContainerPtrToValuePtr<void>(BumpArgs.GetStructMemory()),
                                         ArrayProbeIn->ContainerPtrToValuePtr<void>(ArrayParams.GetStructMemory()));
        Event(ArrayProbeBump, BumpArgs.GetStructMemory());
        FScriptArrayHelper ArrayProbeBumped(ArrayProbeRef,
                                            ArrayProbeRef->ContainerPtrToValuePtr<void>(BumpArgs.GetStructMemory()));
        Expect(ArrayProbeBumped.Num() == 2 &&
                   ArrayProbeAmount->GetPropertyValue_InContainer(ArrayProbeBumped.GetRawPtr(0)) == 34 &&
                   ArrayProbeAmount->GetPropertyValue_InContainer(ArrayProbeValues.GetRawPtr(0)) == 31,
               TEXT("authored_struct_array_ref_copyback"));
        FString ArrayProbeError;
        FDasDeclaredType ArrayProbeDeclared;
        ArrayProbeDeclared.Kind = TEXT("array");
        auto ArrayElementDeclaration = MakeShared<FDasDeclaredType>();
        ArrayElementDeclaration->Kind = TEXT("struct");
        ArrayElementDeclaration->TypePath = ArrayProbeElement->GetPathName();
        ArrayProbeDeclared.Element = ArrayElementDeclaration;
        auto* ArrayProbeCarrier = DasContainerCarrier(ArrayProbeDeclared);
        Expect(ArrayProbeCarrier &&
               ArrayProbeCarrier->FindPropertyByName(TEXT("Values"))->SameType(ArrayProbeIn),
                                                     TEXT("authored_struct_array_exact_carrier_type"));
        if (ArrayProbeCarrier)
        {
            FDasScalarValue ArrayProbeInput;
            ArrayProbeInput.Kind = 12;
            ArrayProbeInput.ObjectClass = ArrayProbeCarrier->GetPathName();
            ArrayProbeInput.StructValue.InitializeAs(ArrayProbeCarrier);
            auto* ArrayProbeItems = ArrayProbeCarrier->FindPropertyByName(TEXT("Values"));
            ArrayProbeItems->CopyCompleteValue(
                ArrayProbeItems->ContainerPtrToValuePtr<void>(ArrayProbeInput.StructValue.GetMutableMemory()),
                ArrayProbeIn->ContainerPtrToValuePtr<void>(ArrayParams.GetStructMemory()));
            const auto ArrayProbeDirect = R.RunNamedFramePersistent(
                R.ScriptPath(Script), TEXT("ue_CounterActor_EchoDetails"), {ArrayProbeInput},
                                           EDasMethodSignature::Struct, Actor, ArrayProbeInput.ObjectClass);
            Expect(ArrayProbeDirect.bSucceeded && ArrayProbeDirect.StructValue.IsValid() &&
                       ArrayProbeDirect.StructValue.GetScriptStruct() == ArrayProbeCarrier,
                   TEXT("authored_struct_array_direct_runtime_transport"));
            FDasScalarValue ArrayProbeWrong = ArrayProbeInput;
            ArrayProbeWrong.ObjectClass = TEXT("/Script/DasRuntime.DasCallPayload");
            const auto ArrayProbeRejected = R.RunNamedFramePersistent(
                R.ScriptPath(Script), TEXT("ue_CounterActor_EchoDetails"), {ArrayProbeWrong},
                                           EDasMethodSignature::Struct, Actor, ArrayProbeInput.ObjectClass);
            Expect(!ArrayProbeRejected.bSucceeded, TEXT("authored_struct_array_wrong_identity_rejected"));
            auto* OtherElement = Cast<UScriptStruct>(CastChecked<ADasProbeActor>(Actor)->TypeRegistry->Resolve(
                TEXT("interfacevalues.Settings"), false, ArrayProbeError));
            FDasDeclaredType OtherDeclared;
            OtherDeclared.Kind = TEXT("array");
            auto OtherElementDeclaration = MakeShared<FDasDeclaredType>();
            OtherElementDeclaration->Kind = TEXT("struct");
            OtherElementDeclaration->TypePath = OtherElement ? OtherElement->GetPathName() : FString();
            OtherDeclared.Element = OtherElementDeclaration;
            auto* OtherCarrier = DasContainerCarrier(OtherDeclared);
            if (OtherCarrier)
            {
                FDasScalarValue OtherInput;
                OtherInput.Kind = 12;
                OtherInput.ObjectClass = OtherCarrier->GetPathName();
                OtherInput.StructValue.InitializeAs(OtherCarrier);
                const auto OtherResult = R.RunNamedFramePersistent(
                    R.ScriptPath(Script), TEXT("ue_CounterActor_EchoDetails"), {OtherInput},
                                               EDasMethodSignature::Struct, Actor, ArrayProbeInput.ObjectClass);
                Expect(!OtherResult.bSucceeded && OtherCarrier != ArrayProbeCarrier,
                       TEXT("authored_struct_array_wrong_element_type_rejected"));
            }
            else
                Expect(false, TEXT("authored_struct_array_wrong_element_type_rejected"));
            FDasScalarValue Oversized;
            Oversized.Kind = 12;
            Oversized.ObjectClass = ArrayProbeInput.ObjectClass;
            Oversized.StructValue.InitializeAs(ArrayProbeCarrier);
            auto* LargeValues = CastFieldChecked<FArrayProperty>(ArrayProbeItems);
            FScriptArrayHelper TooMany(
                LargeValues, LargeValues->ContainerPtrToValuePtr<void>(Oversized.StructValue.GetMutableMemory()));
            TooMany.Resize(4097);
            const auto LargeResult = R.RunNamedFramePersistent(
                R.ScriptPath(Script), TEXT("ue_CounterActor_EchoDetails"), {Oversized}, EDasMethodSignature::Struct,
                                           Actor, Oversized.ObjectClass);
            Expect(!LargeResult.bSucceeded &&
                   LargeResult.Error.Contains(TEXT("limit")), TEXT("authored_struct_array_snapshot_bound_rejected"));
        }
    }
    {
        TStrongObjectPtr<UObject> InterfaceActorKeep(Actor);
        TStrongObjectPtr<UWorld> InterfaceWorldKeep(CastChecked<AActor>(Actor)->GetWorld());
        auto* InterfaceField = FindFProperty<FInterfaceProperty>(Actor->GetClass(), TEXT("HeldEvents"));
        Expect(InterfaceField && InterfaceField->InterfaceClass == UDasEventInterface::StaticClass(),
               TEXT("interface_reference_real_UPROPERTY_type"));
        FDasScalarValue InterfaceInput;
        InterfaceInput.Kind = 8;
        InterfaceInput.ObjectClass = UDasEventInterface::StaticClass()->GetPathName();
        InterfaceInput.ObjectValue = Actor;
        auto* InterfaceEcho = Actor->FindFunction(TEXT("EchoEvents"));
        auto* InterfaceArg = FindFProperty<FInterfaceProperty>(InterfaceEcho, TEXT("value"));
        auto* InterfaceReturn = CastFieldChecked<FInterfaceProperty>(InterfaceEcho->GetReturnProperty());
        FStructOnScope InterfaceArgs(InterfaceEcho);
        DasReferenceSet(InterfaceArg, InterfaceArg->ContainerPtrToValuePtr<void>(InterfaceArgs.GetStructMemory()),
                        Actor);
        Event(InterfaceEcho, InterfaceArgs.GetStructMemory());
        Expect(DasReferenceGet(InterfaceReturn,
                               InterfaceReturn->ContainerPtrToValuePtr<void>(InterfaceArgs.GetStructMemory())) == Actor,
               TEXT("interface_reference_BP_only_ProcessEvent_roundtrip"));
        Expect(!Actor->GetInterfaceAddress(UDasEventInterface::StaticClass()),
               TEXT("interface_reference_BP_only_has_null_native_pointer"));
        const auto InterfaceRoundtrip = R.RunNamedFramePersistent(
            R.ScriptPath(Script), TEXT("ue_CounterActor_EchoEvents"), {InterfaceInput}, EDasMethodSignature::Object,
                                       Actor, InterfaceInput.ObjectClass);
        Expect(InterfaceRoundtrip.bSucceeded && InterfaceRoundtrip.ObjectValue.Get() == Actor,
               TEXT("interface_reference_direct_runtime_roundtrip"));
        const auto InterfaceStore =
            R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_CounterActor_SetEvents"), {InterfaceInput},
                                                                 EDasMethodSignature::Void, Actor);
        const auto InterfaceRead = R.RunNamedFramePersistent(
            R.ScriptPath(Script), TEXT("ue_CounterActor_GetEvents"), {}, EDasMethodSignature::Object, Actor,
                                       InterfaceInput.ObjectClass);
        Expect(InterfaceStore.bSucceeded && InterfaceRead.bSucceeded && InterfaceRead.ObjectValue.Get() == Actor,
               TEXT("interface_reference_DSL_field_get_set"));
        FDasScalarValue NullInterface = InterfaceInput;
        NullInterface.ObjectValue = nullptr;
        const auto InterfaceNull = R.RunNamedFramePersistent(
            R.ScriptPath(Script), TEXT("ue_CounterActor_EchoEvents"), {NullInterface}, EDasMethodSignature::Object,
                                       Actor, NullInterface.ObjectClass);
        Expect(InterfaceNull.bSucceeded && !InterfaceNull.ObjectValue.IsValid(),
               TEXT("interface_reference_null_roundtrip"));
        UObject* InvalidInterface = NewObject<UDasTypeRegistry>();
        TStrongObjectPtr<UObject> InvalidKeep(InvalidInterface);
        FDasScalarValue BadInterface = InterfaceInput;
        BadInterface.ObjectValue = InvalidInterface;
        const auto InterfaceRejected = R.RunNamedFramePersistent(
            R.ScriptPath(Script), TEXT("ue_CounterActor_SetEvents"), {BadInterface}, EDasMethodSignature::Void, Actor);
        Expect(!InterfaceRejected.bSucceeded &&
                   DasReferenceGet(InterfaceField, InterfaceField->ContainerPtrToValuePtr<void>(Actor)) == Actor,
               TEXT("interface_reference_nonimplementer_rejected_without_write"));
        R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_CounterActor_ClearEvents"), {},
                                                             EDasMethodSignature::Void, Actor);
        auto* ReferenceWorld = CastChecked<AActor>(Actor)->GetWorld();
        auto* DestroyedTarget = ReferenceWorld->SpawnActor<AActor>(Actor->GetClass());
        FDasScalarValue DestroyedInput = InterfaceInput;
        DestroyedInput.ObjectValue = DestroyedTarget;
        const auto BeforeDestroy =
            R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_CounterActor_SetEvents"), {DestroyedInput},
                                                                 EDasMethodSignature::Void, Actor);
        DestroyedTarget->Destroy();
        CollectGarbage(RF_NoFlags);
        const auto AfterDestroy = R.RunNamedFramePersistent(
            R.ScriptPath(Script), TEXT("ue_CounterActor_GetEvents"), {}, EDasMethodSignature::Object, Actor,
                                       InterfaceInput.ObjectClass);
        Expect(BeforeDestroy.bSucceeded && AfterDestroy.bSucceeded && !AfterDestroy.ObjectValue.IsValid(),
               TEXT("interface_reference_destroyed_actor_reads_null"));
    }
    {
        auto RunRefPair = [&](const TCHAR* MethodName, FDasScalarValue Value,
                              TFunctionRef<bool(const FDasScalarValue&, const FDasScalarValue&)> Validate)
        {
            auto* Function = Actor->FindFunction(MethodName);
            auto* InputProperty = Function->FindPropertyByName(TEXT("value"));
            auto* PreviousProperty = Function->FindPropertyByName(TEXT("previous"));
            FStructOnScope Frame(Function);
            auto Write = [&](FProperty* Property, void* Memory, const FDasScalarValue& Data)
            {
                if (Data.Kind == 8)
                    return DasReferenceSet(Property, Memory, Data.ObjectValue.Get());
                if (Data.Kind == 13)
                    return SetDasEnumPropertyNumber(Property, Memory, Data.IntValue);
                if (Data.Kind == 12)
                {
                    CastFieldChecked<FStructProperty>(Property)->Struct->CopyScriptStruct(Memory,
                                                                                          Data.StructValue.GetMemory());
                    return true;
                }
                if (Data.Kind == 9)
                    *static_cast<FVector*>(Memory) = Data.VectorValue;
                else if (Data.Kind == 10)
                    *static_cast<FRotator*>(Memory) = Data.RotatorValue;
                else if (Data.Kind == 11)
                    *static_cast<FTransform*>(Memory) = Data.TransformValue;
                return true;
            };
            auto Read = [&](FProperty* Property, const void* Memory)
            {
                FDasScalarValue Data;
                Data.Kind = Value.Kind;
                Data.ObjectClass = Value.ObjectClass;
                if (Data.Kind == 8)
                    Data.ObjectValue = DasReferenceGet(Property, Memory);
                else if (Data.Kind == 13)
                    Data.IntValue = int32(DasEnumPropertyNumber(Property, Memory));
                else if (Data.Kind == 12)
                    Data.StructValue.InitializeAs(CastFieldChecked<FStructProperty>(Property)->Struct,
                                                  static_cast<const uint8*>(Memory));
                else if (Data.Kind == 9)
                    Data.VectorValue = *static_cast<const FVector*>(Memory);
                else if (Data.Kind == 10)
                    Data.RotatorValue = *static_cast<const FRotator*>(Memory);
                else if (Data.Kind == 11)
                    Data.TransformValue = *static_cast<const FTransform*>(Memory);
                return Data;
            };
            Write(InputProperty, InputProperty->ContainerPtrToValuePtr<void>(Frame.GetStructMemory()), Value);
            Event(Function, Frame.GetStructMemory());
            const auto Changed =
                Read(InputProperty, InputProperty->ContainerPtrToValuePtr<void>(Frame.GetStructMemory()));
            const auto Previous =
                Read(PreviousProperty, PreviousProperty->ContainerPtrToValuePtr<void>(Frame.GetStructMemory()));
            Expect(Validate(Changed, Previous), *(FString(TEXT("extended_ref_ProcessEvent_")) + MethodName));
            Value.Direction = 2;
            FDasScalarValue Output = Value;
            Output.Direction = 1;
            const auto Direct =
                R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_CounterActor_")+ FString(MethodName),
                                                                     {Value, Output}, EDasMethodSignature::Void, Actor);
            Expect(Direct.bSucceeded && Direct.OutputValues.Num() == 2 &&
                       Validate(Direct.OutputValues[0], Direct.OutputValues[1]),
                   *(FString(TEXT("extended_ref_runtime_")) + MethodName));
        };
        FDasScalarValue RefObject;
        RefObject.Kind = 8;
        RefObject.ObjectClass = TEXT("/Script/Engine.Actor");
        RefObject.ObjectValue = Actor;
        RunRefPair(TEXT("RefActor"), RefObject, [&](const auto& Changed, const auto& Previous)
                        { return !Changed.ObjectValue.Get() && Previous.ObjectValue.Get() == Actor; });
        RefObject.ObjectClass = UDasEventInterface::StaticClass()->GetPathName();
        RunRefPair(TEXT("RefEvents"), RefObject, [&](const auto& Changed, const auto& Previous)
                        { return !Changed.ObjectValue.Get() && Previous.ObjectValue.Get() == Actor; });
        FString RefError;
        auto* RefRegistry = CastChecked<ADasProbeActor>(Actor)->TypeRegistry.Get();
        auto* StateType = Cast<UEnum>(RefRegistry->Resolve(TEXT("interfacevalues.State"), true, RefError));
        FDasScalarValue RefState;
        RefState.Kind = 13;
        RefState.ObjectClass = StateType->GetPathName();
        RefState.IntValue = 42;
        RunRefPair(TEXT("RefState"), RefState, [](const auto& Changed, const auto& Previous)
                        { return Changed.IntValue == 7 && Previous.IntValue == 42; });
        auto* SettingsType =
            Cast<UScriptStruct>(RefRegistry->Resolve(TEXT("interfacevalues.Settings"), false, RefError));
        FDasScalarValue RefSettings;
        RefSettings.Kind = 12;
        RefSettings.ObjectClass = SettingsType->GetPathName();
        RefSettings.StructValue.InitializeAs(SettingsType);
        auto* CountField = CastFieldChecked<FIntProperty>(FindDasStructField(SettingsType, TEXT("Count")));
        CountField->SetPropertyValue_InContainer(RefSettings.StructValue.GetMutableMemory(), 20);
        RunRefPair(TEXT("RefSettings"), RefSettings,
                        [&](const auto& Changed, const auto& Previous)
                        {
                            return Changed.StructValue.IsValid() && Previous.StructValue.IsValid() &&
                                   CountField->GetPropertyValue_InContainer(Changed.StructValue.GetMemory()) == 23 &&
                                   CountField->GetPropertyValue_InContainer(Previous.StructValue.GetMemory()) == 20;
                        });
        Expect(CountField->GetPropertyValue_InContainer(RefSettings.StructValue.GetMemory()) == 20,
               TEXT("extended_ref_struct_input_immutable"));
        FDasScalarValue RefMath;
        RefMath.Kind = 9;
        RefMath.VectorValue = FVector(1, 2, 3);
        RunRefPair(
            TEXT("RefVector"), RefMath, [](const auto& Changed, const auto& Previous)
                 { return Changed.VectorValue == FVector(5, 6, 7) && Previous.VectorValue == FVector(1, 2, 3); });
        RefMath.Kind = 10;
        RefMath.RotatorValue = FRotator(1, 2, 3);
        RunRefPair(TEXT("RefRotator"), RefMath, [](const auto& Changed, const auto& Previous)
            { return Changed.RotatorValue == FRotator(10, 20, 30) && Previous.RotatorValue == FRotator(1, 2, 3); });
        RefMath.Kind = 11;
        RefMath.TransformValue = FTransform(FVector(1, 2, 3));
        RunRefPair(TEXT("RefTransform"), RefMath,
                        [](const auto& Changed, const auto& Previous)
                        {
                            return Changed.TransformValue.Equals(FTransform::Identity) &&
                                   Previous.TransformValue.GetTranslation() == FVector(1, 2, 3);
                        });
    }
    {
        FDasScalarValue BadEnum;
        BadEnum.Kind = 13;
        BadEnum.ObjectClass = TEXT("/Script/DasRuntime.EDasCallMode");
        BadEnum.IntValue = 42;
        BadEnum.Direction = 2;
        FDasScalarValue EnumOut = BadEnum;
        EnumOut.Direction = 1;
        const auto EnumFailure =
            R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_CounterActor_WrongRefEnum"), {BadEnum, EnumOut},
                                                                 EDasMethodSignature::Void, Actor);
        Expect(!EnumFailure.bSucceeded && EnumFailure.OutputValues.IsEmpty() &&
               EnumFailure.Error.Contains(TEXT("enum identity")),
                                          TEXT("extended_ref_wrong_enum_output_atomic_failure"));
        FDasScalarValue BadStruct;
        BadStruct.Kind = 12;
        BadStruct.ObjectClass = TEXT("/Script/DasRuntime.DasCallPayload");
        BadStruct.Direction = 2;
        FString BadStructError;
        ParseDasStructDefault(BadStruct.ObjectClass, TEXT(""), BadStruct.StructValue, BadStructError);
        FDasScalarValue StructOut = BadStruct;
        StructOut.Direction = 1;
        const auto StructFailure = R.RunNamedFramePersistent(
            R.ScriptPath(Script), TEXT("ue_CounterActor_WrongRefStruct"), {BadStruct, StructOut},
                                       EDasMethodSignature::Void, Actor);
        Expect(!StructFailure.bSucceeded && StructFailure.OutputValues.IsEmpty() &&
               StructFailure.Error.Contains(TEXT("snapshot type")),
                                            TEXT("extended_ref_wrong_struct_output_atomic_failure"));
        FDasScalarValue FailVector;
        FailVector.Kind = 9;
        FailVector.VectorValue = FVector(1, 2, 3);
        FailVector.Direction = 2;
        FDasScalarValue VectorOut = FailVector;
        VectorOut.Direction = 1;
        const auto PanicFailure = R.RunNamedFramePersistent(
            R.ScriptPath(Script), TEXT("ue_CounterActor_FailRefVector"), {FailVector, VectorOut},
                                       EDasMethodSignature::Void, Actor);
        Expect(!PanicFailure.bSucceeded && PanicFailure.OutputValues.IsEmpty(),
               TEXT("extended_ref_panic_does_not_publish_outputs"));
        auto* PanicFunction = Actor->FindFunction(TEXT("FailRefVector"));
        FStructOnScope PanicArgs(PanicFunction);
        auto* PanicValue = PanicFunction->FindPropertyByName(TEXT("value"));
        auto* PanicPrevious = PanicFunction->FindPropertyByName(TEXT("previous"));
        *PanicValue->ContainerPtrToValuePtr<FVector>(PanicArgs.GetStructMemory()) = FVector(1, 2, 3);
        *PanicPrevious->ContainerPtrToValuePtr<FVector>(PanicArgs.GetStructMemory()) = FVector(9, 8, 7);
        Event(PanicFunction, PanicArgs.GetStructMemory());
        Expect(*PanicValue->ContainerPtrToValuePtr<FVector>(PanicArgs.GetStructMemory()) == FVector(1, 2, 3) &&
                   *PanicPrevious->ContainerPtrToValuePtr<FVector>(PanicArgs.GetStructMemory()) == FVector(9, 8, 7),
               TEXT("extended_ref_Blueprint_abort_preserves_caller_fields"));
    }
    R.ReleaseObject(Actor);
    return Pass;
}
