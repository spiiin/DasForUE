#pragma once
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "DasContainerTypes.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"
#include "UObject/Script.h"

// Shared editor/cooked integration probe: native container UFunction parameters and results.
inline bool RunDasContainerCallsProbe(FDasRuntimeModule& Runtime, UObject* Actor, const FString& Script,
                                      TFunctionRef<void(bool, const TCHAR*)> Check, bool Editor)
{
    bool Pass = true;
    auto Expect = [&](bool OK, const FString& Name)
    {
        Pass &= OK;
        Check(OK, *Name);
    };
    auto Write = [](FProperty* P, void* Memory, int32 Index)
    {
        if (auto* Scalar11 = CastField<FIntProperty>(P))
            Scalar11->SetPropertyValue(Memory, Index == 0 ? -3 : 7);
        else if (auto* Scalar12 = CastField<FFloatProperty>(P))
            Scalar12->SetPropertyValue(Memory, Index == 0 ? 1.25f : -0.5f);
        else if (auto* Scalar13 = CastField<FDoubleProperty>(P))
            Scalar13->SetPropertyValue(Memory, Index == 0 ? 1.23456789012345 : -9.87654321098765);
        else if (auto* Scalar14 = CastField<FBoolProperty>(P))
            Scalar14->SetPropertyValue(Memory, Index == 0);
        else if (auto* Scalar15 = CastField<FStrProperty>(P))
            Scalar15->SetPropertyValue(Memory, Index == 0 ? TEXT("Hello 世界"): TEXT(""));
        else if (auto* Scalar16 = CastField<FNameProperty>(P))
            Scalar16->SetPropertyValue(Memory, Index == 0 ? FName(TEXT("Key_17")) : NAME_None);
    };
    for (const FString& Kind : TArray<FString>{TEXT("array"), TEXT("set"), TEXT("map")})
    {
        const TArray<FString> Scalars = {TEXT("int"),  TEXT("float"),  TEXT("double"),
                                                                            TEXT("bool"), TEXT("string"), TEXT("name")};
        const TArray<FString> Keys = Kind == TEXT("array")? Scalars : TArray<FString>{TEXT("int"), TEXT("string")};
        for (const FString& Key : Keys)
            for (const FString& Value : Kind == TEXT("map")? Scalars : TArray<FString>{TEXT("")})
            {
                FDasDeclaredType Type;
                Type.Kind = Kind;
                auto K = MakeShared<FDasDeclaredType>();
                K->Kind = Key;
                if (Kind == TEXT("map"))
                {
                    Type.Key = K;
                    auto V = MakeShared<FDasDeclaredType>();
                    V->Kind = Value;
                    Type.Value = V;
                }
                else
                    Type.Element = K;
                auto Title = [](FString S)
                {
                    if (!S.IsEmpty())
                        S[0] = FChar::ToUpper(S[0]);
                    return S;
                };
                const FString Name = TEXT("Echo")+ Title(Kind) + Title(Key) + Title(Value);
                auto* Carrier = DasContainerCarrier(Type);
                auto* P = Carrier ? Carrier->FindPropertyByName(TEXT("Values")) : nullptr;
                auto* Function = Actor->FindFunction(*Name);
                Expect(Carrier && P && Function &&
                       P->SameType(Function->FindPropertyByName(TEXT("input"))) &&
                                   P->SameType(Function->GetReturnProperty()),
                                   Name + TEXT("_native_signature"));
                if (!Carrier || !P || !Function)
                    continue;
                FDasScalarValue Input;
                Input.Kind = 12;
                Input.ObjectClass = Carrier->GetPathName();
                Input.StructValue.InitializeAs(Carrier);
                void* Memory = P->ContainerPtrToValuePtr<void>(Input.StructValue.GetMutableMemory());
                if (auto* Array = CastField<FArrayProperty>(P))
                {
                    FScriptArrayHelper Items(Array, Memory);
                    Items.Resize(2);
                    for (int32 I = 0; I < 2; ++I)
                        Write(Array->Inner, Items.GetRawPtr(I), I);
                }
                else if (auto* Set = CastField<FSetProperty>(P))
                {
                    FScriptSetHelper Items(Set, Memory);
                    for (int32 I = 0; I < 2; ++I)
                    {
                        int32 Number = I == 0 ? -3 : 7;
                        FString Text = I == 0 ? TEXT("Hello 世界"): TEXT("");
                        Items.AddElement(Key == TEXT("int")? static_cast<void*>(&Number) : static_cast<void*>(&Text));
                    }
                }
                else
                {
                    auto* Map = CastFieldChecked<FMapProperty>(P);
                    FScriptMapHelper Items(Map, Memory);
                    for (int32 I = 0; I < 2; ++I)
                    {
                        const int32 At = Items.AddDefaultValue_Invalid_NeedsRehash();
                        Write(Map->KeyProp, Items.GetKeyPtr(At), I);
                        Write(Map->ValueProp, Items.GetValuePtr(At), I);
                    }
                    Items.Rehash();
                }
                FInstancedStruct Original = Input.StructValue;
                auto Matches = [&](const FInstancedStruct& V)
                {
                    return V.IsValid() && V.GetScriptStruct() == Carrier &&
                           Carrier->CompareScriptStruct(Original.GetMemory(), V.GetMemory(), 0);
                };
                auto Result = Runtime.RunNamedFramePersistent(
                    Runtime.ScriptPath(Script), TEXT("ue_NestedActor_")+ Name, {Input}, EDasMethodSignature::Struct,
                                                     Actor, Carrier->GetPathName());
                Expect(Result.bSucceeded && Matches(Result.StructValue), Name + TEXT("_typed_roundtrip"));
                FStructOnScope Args(Function);
                auto* Arg = Function->FindPropertyByName(TEXT("input"));
                Arg->CopyCompleteValue(Arg->ContainerPtrToValuePtr<void>(Args.GetStructMemory()), Memory);
#if WITH_EDITOR
                if (Editor)
                {
                    FEditorScriptExecutionGuard Guard;
                    Actor->ProcessEvent(Function, Args.GetStructMemory());
                }
                else
#endif
                {
                    Actor->ProcessEvent(Function, Args.GetStructMemory());
                }
                auto* Return = Function->GetReturnProperty();
                Expect(Return && P->Identical(Return->ContainerPtrToValuePtr<void>(Args.GetStructMemory()), Memory, 0),
                       Name + TEXT("_ProcessEvent_roundtrip"));
                Return->ClearValue(Return->ContainerPtrToValuePtr<void>(Args.GetStructMemory()));
                Expect(Matches(Input.StructValue), Name + TEXT("_returned_storage_independent"));
                FDasScalarValue Empty = Input;
                Empty.StructValue.InitializeAs(Carrier);
                auto Zero = Runtime.RunNamedFramePersistent(
                    Runtime.ScriptPath(Script), TEXT("ue_NestedActor_")+ Name, {Empty}, EDasMethodSignature::Struct,
                                                     Actor, Carrier->GetPathName());
                Expect(Zero.bSucceeded && Zero.StructValue.IsValid() &&
                           Carrier->CompareScriptStruct(Empty.StructValue.GetMemory(), Zero.StructValue.GetMemory(), 0),
                       Name + TEXT("_empty_roundtrip"));
                Runtime.ReleaseObject(Actor);
            }
    }
    {
        FDasScalarValue Numbers, Tags, Weights, Offset;
        Numbers.Kind = Tags.Kind = Weights.Kind = 12;
        Offset.Kind = 0;
        Offset.IntValue = 4;
        Numbers.ObjectClass = FDasArrayDoubleCarrier::StaticStruct()->GetPathName();
        Numbers.StructValue.InitializeAs(FDasArrayDoubleCarrier::StaticStruct());
        Numbers.StructValue.GetMutable<FDasArrayDoubleCarrier>().Values = {1.25, 2.5};
        Tags.ObjectClass = FDasSetStringCarrier::StaticStruct()->GetPathName();
        Tags.StructValue.InitializeAs(FDasSetStringCarrier::StaticStruct());
        Tags.StructValue.GetMutable<FDasSetStringCarrier>().Values = {TEXT("one"), TEXT("世界")};
        Weights.ObjectClass = FDasMapIntFloatCarrier::StaticStruct()->GetPathName();
        Weights.StructValue.InitializeAs(FDasMapIntFloatCarrier::StaticStruct());
        Weights.StructValue.GetMutable<FDasMapIntFloatCarrier>().Values.Add(-3, 3.0f);
        auto Mixed = Runtime.RunNamedFramePersistent(Runtime.ScriptPath(Script),
                                                     TEXT("ue_NestedActor_Mixed"), {Numbers, Tags, Weights, Offset},
                                                          EDasMethodSignature::Double, Actor);
        Expect(Mixed.bSucceeded && Mixed.DoubleValue == 12.75, TEXT("containers_mixed_frame_order_and_scalar"));
        Runtime.ReleaseObject(Actor);
        auto* Function = Actor->FindFunction(TEXT("Mixed"));
        FStructOnScope Args(Function);
        for (const auto& Item : TArray<TPair<FName, FDasScalarValue>>{
                 {TEXT("numbers"), Numbers}, {TEXT("tags"), Tags}, {TEXT("weights"), Weights}})
        {
            auto* Native = DasContainerProperty(Item.Value.ObjectClass);
            Function->FindPropertyByName(Item.Key)->CopyCompleteValue(
                Function->FindPropertyByName(Item.Key)->ContainerPtrToValuePtr<void>(Args.GetStructMemory()),
                Native->ContainerPtrToValuePtr<void>(Item.Value.StructValue.GetMemory()));
        }
        CastFieldChecked<FIntProperty>(
            Function->FindPropertyByName(TEXT("offset")))->SetPropertyValue_InContainer(Args.GetStructMemory(), 4);
#if WITH_EDITOR
        if (Editor)
        {
            FEditorScriptExecutionGuard Guard;
            Actor->ProcessEvent(Function, Args.GetStructMemory());
        }
        else
#endif
        {
            Actor->ProcessEvent(Function, Args.GetStructMemory());
        }
        Expect(CastFieldChecked<FDoubleProperty>(Function->GetReturnProperty())
                       ->GetPropertyValue_InContainer(Args.GetStructMemory()) == 12.75,
               TEXT("containers_mixed_ProcessEvent"));
        Runtime.ReleaseObject(Actor);
        auto Wrong = Runtime.RunNamedFramePersistent(Runtime.ScriptPath(Script),
                                                     TEXT("ue_NestedActor_Mixed"), {Tags, Tags, Weights, Offset},
                                                          EDasMethodSignature::Double, Actor);
        Expect(!Wrong.bSucceeded, TEXT("containers_wrong_carrier_rejected"));
        Runtime.ReleaseObject(Actor);
        Numbers.StructValue.GetMutable<FDasArrayDoubleCarrier>().Values.SetNum(4097);
        auto Large = Runtime.RunNamedFramePersistent(Runtime.ScriptPath(Script),
                                                     TEXT("ue_NestedActor_Mixed"), {Numbers, Tags, Weights, Offset},
                                                          EDasMethodSignature::Double, Actor);
        Expect(!Large.bSucceeded && Large.Error.Contains(TEXT("limit")), TEXT("containers_oversized_input_rejected"));
        Runtime.ReleaseObject(Actor);
    }
    return Pass;
}
