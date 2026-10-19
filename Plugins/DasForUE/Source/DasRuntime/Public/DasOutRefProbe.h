#pragma once
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "DasContainerTypes.h"
#include "UObject/StructOnScope.h"
#include "UObject/Script.h"
#include "UObject/UnrealType.h"
inline bool RunDasOutRefProbe(FDasRuntimeModule& R, UObject* Actor, const FString& Script,
                              TFunctionRef<void(bool, const TCHAR*)> Check, bool Editor)
{
    bool Pass = true;
    auto Expect = [&](bool V, const TCHAR* Name)
    {
        Pass &= V;
        Check(V, Name);
    };
    auto Event = [&](UFunction* Function, void* Memory)
    {
#if WITH_EDITOR
        if (Editor)
        {
            FEditorScriptExecutionGuard Guard;
            Actor->ProcessEvent(Function, Memory);
        }
        else
#endif
        {
            Actor->ProcessEvent(Function, Memory);
        }
    };
    FDasScalarValue Counter, Total, Amount;
    Counter.Kind = Total.Kind = Amount.Kind = 0;
    Counter.Direction = 2;
    Counter.IntValue = 10;
    Total.Direction = 1;
    Total.IntValue = 777;
    Amount.IntValue = 3;
    auto Adjust = R.RunNamedFramePersistent(
        R.ScriptPath(Script), TEXT("ue_NestedActor_Adjust"), {Counter, Total, Amount}, EDasMethodSignature::Int, Actor);
    Expect(Adjust.bSucceeded && Adjust.Value == 13 && Adjust.OutputValues.Num() == 3 &&
               Adjust.OutputValues[0].IntValue == 13 && Adjust.OutputValues[1].IntValue == 26 &&
               Counter.IntValue == 10 && Total.IntValue == 777,
           TEXT("out_ref_typed_return_and_deferred_owned_outputs"));
    R.ReleaseObject(Actor);
    auto* Function = Actor->FindFunction(TEXT("Adjust"));
    auto* C = Function ? FindFProperty<FIntProperty>(Function, TEXT("counter")) : nullptr;
    auto* T = Function ? FindFProperty<FIntProperty>(Function, TEXT("total")) : nullptr;
    auto* A = Function ? FindFProperty<FIntProperty>(Function, TEXT("amount")) : nullptr;
    Expect(C && T && A && C->HasAllPropertyFlags(CPF_OutParm | CPF_ReferenceParm) &&
               T->HasAnyPropertyFlags(CPF_OutParm) && !T->HasAnyPropertyFlags(CPF_ReferenceParm),
           TEXT("out_ref_native_flags"));
    if (C && T && A)
    {
        FStructOnScope Params(Function);
        C->SetPropertyValue_InContainer(Params.GetStructMemory(), 10);
        T->SetPropertyValue_InContainer(Params.GetStructMemory(), 777);
        A->SetPropertyValue_InContainer(Params.GetStructMemory(), 3);
        Event(Function, Params.GetStructMemory());
        Expect(C->GetPropertyValue_InContainer(Params.GetStructMemory()) == 13 &&
                   T->GetPropertyValue_InContainer(Params.GetStructMemory()) == 26 &&
                   CastFieldChecked<FIntProperty>(Function->GetReturnProperty())
                           ->GetPropertyValue_InContainer(Params.GetStructMemory()) == 13,
               TEXT("out_ref_ProcessEvent_copyback_and_output"));
    }
    R.ReleaseObject(Actor);
    auto Failed = R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_NestedActor_Fail"), {Counter, Total},
                                                                       EDasMethodSignature::Void, Actor);
    Expect(!Failed.bSucceeded && Failed.Error.Contains(TEXT("fixture failure")) && Failed.OutputValues.IsEmpty() &&
                                                       Counter.IntValue == 10,
                                                       TEXT("out_ref_failure_publishes_no_values"));
    R.ReleaseObject(Actor);
    auto* Fail = Actor->FindFunction(TEXT("Fail"));
    if (Fail)
    {
        FStructOnScope Params(Fail);
        auto* FC = FindFProperty<FIntProperty>(Fail, TEXT("counter"));
        auto* FT = FindFProperty<FIntProperty>(Fail, TEXT("total"));
        FC->SetPropertyValue_InContainer(Params.GetStructMemory(), 10);
        FT->SetPropertyValue_InContainer(Params.GetStructMemory(), 777);
        Event(Fail, Params.GetStructMemory());
        Expect(FC->GetPropertyValue_InContainer(Params.GetStructMemory()) == 10 &&
                   FT->GetPropertyValue_InContainer(Params.GetStructMemory()) == 0,
               TEXT("out_ref_ProcessEvent_failure_preserves_ref_defaults_out"));
    }
    R.ReleaseObject(Actor);
    FDasScalarValue Numbers, Tags, Weights, Message;
    Numbers.Kind = Tags.Kind = Weights.Kind = 12;
    Numbers.Direction = Tags.Direction = Weights.Direction = 2;
    Message.Kind = 5;
    Message.Direction = 1;
    Message.StringValue = TEXT("ignored");
    Numbers.ObjectClass = FDasArrayDoubleCarrier::StaticStruct()->GetPathName();
    Numbers.StructValue.InitializeAs(FDasArrayDoubleCarrier::StaticStruct());
    Numbers.StructValue.GetMutable<FDasArrayDoubleCarrier>().Values = {2.5};
    Tags.ObjectClass = FDasSetStringCarrier::StaticStruct()->GetPathName();
    Tags.StructValue.InitializeAs(FDasSetStringCarrier::StaticStruct());
    Weights.ObjectClass = FDasMapIntBoolCarrier::StaticStruct()->GetPathName();
    Weights.StructValue.InitializeAs(FDasMapIntBoolCarrier::StaticStruct());
    auto Change = R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_NestedActor_Change"),
                                                                       {Numbers, Tags, Weights, Message},
                                                                       EDasMethodSignature::Void, Actor);
    auto Changed = [&](const TArray<FDasScalarValue>& Values)
    {
        return Values.Num() == 4 &&
               Values[0].StructValue.Get<FDasArrayDoubleCarrier>().Values == TArray<double>{2.5, 1.23456789012345} &&
               Values[1].StructValue.Get<FDasSetStringCarrier>().Values.Contains(
                   TEXT("世界")) && Values[2].StructValue.Get<FDasMapIntBoolCarrier>().Values.FindRef(-3) &&
                   Values[3].StringValue == TEXT("Hello 世界");
    };
    Expect(Change.bSucceeded && Changed(Change.OutputValues) &&
               Numbers.StructValue.Get<FDasArrayDoubleCarrier>().Values.Num() == 1 &&
               Tags.StructValue.Get<FDasSetStringCarrier>().Values.IsEmpty() &&
               Weights.StructValue.Get<FDasMapIntBoolCarrier>().Values.IsEmpty(),
           TEXT("out_ref_containers_typed_mutation_preserves_inputs"));
    R.ReleaseObject(Actor);
    auto* CF = Actor->FindFunction(TEXT("Change"));
    if (CF)
    {
        FStructOnScope Params(CF);
        TArray<TPair<FName, FDasScalarValue>> Inputs = {{TEXT("numbers"), Numbers}, {TEXT("tags"), Tags},
                                                                                     {TEXT("weights"), Weights}};
        for (const auto& Pair : Inputs)
        {
            auto* Value = DasContainerProperty(Pair.Value.ObjectClass);
            CF->FindPropertyByName(Pair.Key)->CopyCompleteValue(
                CF->FindPropertyByName(Pair.Key)->ContainerPtrToValuePtr<void>(Params.GetStructMemory()),
                Value->ContainerPtrToValuePtr<void>(Pair.Value.StructValue.GetMemory()));
        }
        Event(CF, Params.GetStructMemory());
        TArray<FDasScalarValue> Out;
        for (const auto& Pair : Inputs)
        {
            auto Copy = Pair.Value;
            auto* Value = DasContainerProperty(Copy.ObjectClass);
            Value->CopyCompleteValue(
                Value->ContainerPtrToValuePtr<void>(Copy.StructValue.GetMutableMemory()),
                CF->FindPropertyByName(Pair.Key)->ContainerPtrToValuePtr<void>(Params.GetStructMemory()));
            Out.Add(Copy);
        }
        auto Text = Message;
        Text.StringValue =
            FindFProperty<FStrProperty>(CF, TEXT("message"))->GetPropertyValue_InContainer(Params.GetStructMemory());
        Out.Add(Text);
        Expect(Changed(Out), TEXT("out_ref_containers_ProcessEvent_copyback"));
    }
    R.ReleaseObject(Actor);
    for (const FString& Kind : TArray<FString>{TEXT("scalar"), TEXT("array"), TEXT("set"), TEXT("map")})
    {
        const TArray<FString> Scalars = {TEXT("int"),  TEXT("float"),  TEXT("double"),
                                                                            TEXT("bool"), TEXT("string"), TEXT("name")};
        const TArray<FString> Keys =
            (Kind == TEXT("scalar") || Kind == TEXT("array")) ? Scalars : TArray<FString>{TEXT("int"), TEXT("string")};
        for (const auto& Key : Keys)
            for (const auto& Value : Kind == TEXT("map")? Scalars : TArray<FString>{TEXT("")})
            {
                auto Title = [](FString V)
                {
                    if (!V.IsEmpty())
                        V[0] = FChar::ToUpper(V[0]);
                    return V;
                };
                const FString Name =
                    TEXT("Ref")+ (Kind == TEXT("scalar")? FString() : Title(Kind)) + Title(Key) + Title(Value);
                FDasScalarValue Initial, Expected;
                FProperty* Native = nullptr;
                UScriptStruct* Carrier = nullptr;
                if (Kind == TEXT("scalar"))
                {
                    Initial.Kind = Expected.Kind =
                        Key == TEXT("int")
                                    ? 0
                                    : Key == TEXT("float")
                                                  ? 1
                                                  : Key == TEXT("bool")
                                                                ? 2
                                                                : Key == TEXT("string")? 5
                                                                                       : Key == TEXT("double")? 6 : 7;
                    Expected.IntValue = 31;
                    Expected.FloatValue = 1.25f;
                    Expected.DoubleValue = 1.23456789012345;
                    Expected.BoolValue = true;
                    Expected.StringValue = TEXT("Hello 世界");
                    Expected.NameValue = FName(TEXT("Key_17"));
                }
                else
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
                    Carrier = DasContainerCarrier(Type);
                    Native = Carrier->FindPropertyByName(TEXT("Values"));
                    Initial.Kind = Expected.Kind = 12;
                    Initial.ObjectClass = Expected.ObjectClass = Carrier->GetPathName();
                    Initial.StructValue.InitializeAs(Carrier);
                    Expected.StructValue.InitializeAs(Carrier);
                    void* Memory = Native->ContainerPtrToValuePtr<void>(Expected.StructValue.GetMutableMemory());
                    auto Write = [](FProperty* P, void* M)
                    {
                        if (auto* I = CastField<FIntProperty>(P))
                            I->SetPropertyValue(M, 31);
                        else if (auto* F = CastField<FFloatProperty>(P))
                            F->SetPropertyValue(M, 1.25f);
                        else if (auto* D = CastField<FDoubleProperty>(P))
                            D->SetPropertyValue(M, 1.23456789012345);
                        else if (auto* B = CastField<FBoolProperty>(P))
                            B->SetPropertyValue(M, true);
                        else if (auto* S = CastField<FStrProperty>(P))
                            S->SetPropertyValue(M, TEXT("Hello 世界"));
                        else if (auto* N = CastField<FNameProperty>(P))
                            N->SetPropertyValue(M, FName(TEXT("Key_17")));
                    };
                    if (auto* Array = CastField<FArrayProperty>(Native))
                    {
                        FScriptArrayHelper H(Array, Memory);
                        H.Resize(1);
                        Write(Array->Inner, H.GetRawPtr(0));
                    }
                    else if (auto* Set = CastField<FSetProperty>(Native))
                    {
                        FScriptSetHelper H(Set, Memory);
                        int32 Number = 31;
                        FString Text = TEXT("Hello 世界");
                        H.AddElement(Key == TEXT("int")? static_cast<void*>(&Number) : static_cast<void*>(&Text));
                    }
                    else
                    {
                        auto* Map = CastFieldChecked<FMapProperty>(Native);
                        FScriptMapHelper H(Map, Memory);
                        int32 At = H.AddDefaultValue_Invalid_NeedsRehash();
                        if (Key == TEXT("int"))
                            CastFieldChecked<FIntProperty>(Map->KeyProp)->SetPropertyValue(H.GetKeyPtr(At), -3);
                        else
                            CastFieldChecked<FStrProperty>(Map->KeyProp)
                                ->SetPropertyValue(H.GetKeyPtr(At), TEXT("Hello 世界"));
                        Write(Map->ValueProp, H.GetValuePtr(At));
                        H.Rehash();
                    }
                }
                auto Matches = [&](const FDasScalarValue& V)
                {
                    if (V.Kind != Expected.Kind)
                        return false;
                    switch (V.Kind)
                    {
                    case 0:
                        return V.IntValue == Expected.IntValue;
                    case 1:
                        return V.FloatValue == Expected.FloatValue;
                    case 2:
                        return V.BoolValue == Expected.BoolValue;
                    case 5:
                        return V.StringValue == Expected.StringValue;
                    case 6:
                        return V.DoubleValue == Expected.DoubleValue;
                    case 7:
                        return V.NameValue == Expected.NameValue;
                    case 12:
                        return V.StructValue.IsValid() && V.StructValue.GetScriptStruct() == Carrier &&
                               Carrier->CompareScriptStruct(V.StructValue.GetMemory(), Expected.StructValue.GetMemory(),
                                                            0);
                    default:
                        return false;
                    }
                };
                Initial.Direction = 2;
                auto Out = Initial;
                Out.Direction = 1;
                auto V = R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_NestedActor_")+ Name, {Initial, Out},
                                                                              EDasMethodSignature::Void, Actor);
                Expect(V.bSucceeded && V.OutputValues.Num() == 2 && Matches(V.OutputValues[0]) &&
                           Matches(V.OutputValues[1]),
                       *(Name + TEXT("_typed_out_ref")));
                R.ReleaseObject(Actor);
                auto* Fn = Actor->FindFunction(*Name);
                FStructOnScope Params(Fn);
                Event(Fn, Params.GetStructMemory());
                FDasScalarValue RefValue, OutValue;
                RefValue.Kind = OutValue.Kind = Expected.Kind;
                for (const auto& Pair :
                     TArray<TPair<FName, FDasScalarValue*>>{{TEXT("value"), &RefValue}, {TEXT("output"), &OutValue}})
                {
                    auto* P = Fn->FindPropertyByName(Pair.Key);
                    void* M = P->ContainerPtrToValuePtr<void>(Params.GetStructMemory());
                    auto& Slot = *Pair.Value;
                    if (Carrier)
                    {
                        Slot.StructValue.InitializeAs(Carrier);
                        Native->CopyCompleteValue(
                            Native->ContainerPtrToValuePtr<void>(Slot.StructValue.GetMutableMemory()), M);
                    }
                    else if (Slot.Kind == 0)
                        Slot.IntValue = CastFieldChecked<FIntProperty>(P)->GetPropertyValue(M);
                    else if (Slot.Kind == 1)
                        Slot.FloatValue = CastFieldChecked<FFloatProperty>(P)->GetPropertyValue(M);
                    else if (Slot.Kind == 2)
                        Slot.BoolValue = CastFieldChecked<FBoolProperty>(P)->GetPropertyValue(M);
                    else if (Slot.Kind == 5)
                        Slot.StringValue = CastFieldChecked<FStrProperty>(P)->GetPropertyValue(M);
                    else if (Slot.Kind == 6)
                        Slot.DoubleValue = CastFieldChecked<FDoubleProperty>(P)->GetPropertyValue(M);
                    else
                        Slot.NameValue = CastFieldChecked<FNameProperty>(P)->GetPropertyValue(M);
                }
                Expect(Matches(RefValue) && Matches(OutValue), *(Name + TEXT("_ProcessEvent_out_ref")));
                R.ReleaseObject(Actor);
            }
    }
    FDasScalarValue OutNumbers;
    OutNumbers.Kind = 12;
    OutNumbers.Direction = 1;
    OutNumbers.ObjectClass = FDasArrayIntCarrier::StaticStruct()->GetPathName();
    OutNumbers.StructValue.InitializeAs(FDasArrayIntCarrier::StaticStruct());
    auto Late = R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_NestedActor_LateFail"), {Counter, OutNumbers},
                                                                     EDasMethodSignature::Void, Actor);
    Expect(!Late.bSucceeded && Late.OutputValues.IsEmpty() && Counter.IntValue == 10,
           TEXT("out_ref_late_pack_failure_discards_earlier_publication"));
    R.ReleaseObject(Actor);
    auto NoDirections = Counter;
    NoDirections.Direction = 0;
    auto Wrong =
        R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_NestedActor_Adjust"), {NoDirections, Total, Amount},
                                                             EDasMethodSignature::Int, Actor);
    Expect(!Wrong.bSucceeded && Wrong.OutputValues.IsEmpty(), TEXT("out_ref_wrong_direction_contract_refused"));
    R.ReleaseObject(Actor);
    auto* LF = Actor->FindFunction(TEXT("LateFail"));
    if (LF)
    {
        FStructOnScope Params(LF);
        auto* LCounter = FindFProperty<FIntProperty>(LF, TEXT("counter"));
        LCounter->SetPropertyValue_InContainer(Params.GetStructMemory(), 10);
        Event(LF, Params.GetStructMemory());
        auto* LArray = FindFProperty<FArrayProperty>(LF, TEXT("numbers"));
        Expect(LCounter->GetPropertyValue_InContainer(Params.GetStructMemory()) == 10 &&
                   FScriptArrayHelper(LArray, LArray->ContainerPtrToValuePtr<void>(Params.GetStructMemory())).Num() ==
                       0,
               TEXT("out_ref_late_failure_native_copyback_preserves_ref"));
    }
    R.ReleaseObject(Actor);
    return Pass;
}
