#pragma once
#include "DasRuntime.h"
#include "DasProbeActor.h"
#include "Curves/CurveFloat.h"
#include "Engine/World.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/StructOnScope.h"
#include "UObject/Script.h"
#include "UObject/UnrealType.h"
inline bool RunDasTypedObjectValues(FDasRuntimeModule& R, ADasProbeActor* Actor, AActor* Target, const FString& Script,
                                    TFunctionRef<void(bool, const TCHAR*)> Check, bool Editor)
{
    bool Pass = true;
    auto Expect = [&](bool V, const TCHAR* Name)
    {
        Pass &= V;
        Check(V, Name);
    };
    const FString Path = R.ScriptPath(Script), ActorPath = TEXT("/Script/Engine.Actor"),
                                                                ObjectPath = TEXT("/Script/CoreUObject.Object");
    TStrongObjectPtr<UWorld> WorldPin(Actor->GetWorld());
    TStrongObjectPtr<ADasProbeActor> OwnerPin(Actor);
    auto* Field = FindFProperty<FObjectProperty>(Actor->GetClass(), TEXT("Target"));
    auto* Held = FindFProperty<FObjectProperty>(Actor->GetClass(), TEXT("Held"));
    Expect(Field && Field->PropertyClass == AActor::StaticClass() && Held &&
               Held->PropertyClass == UObject::StaticClass(),
           TEXT("typed_object_native_property_constraints"));
    if (!Field || !Held)
        return false;
    Expect(!Field->GetObjectPropertyValue_InContainer(Actor->GetClass()->GetDefaultObject()),
           TEXT("typed_object_CDO_null_default"));
    auto Argument = [&](UObject* O, const FString& Class)
    {
        FDasScalarValue V;
        V.Kind = 8;
        V.ObjectValue = O;
        V.ObjectClass = Class;
        return V;
    };
    auto Run = [&](const TCHAR* Method, const TArray<FDasScalarValue>& Args, EDasMethodSignature Kind,
                   const FString& Class = FString())
    {
        return R.RunNamedFramePersistent(Path, TEXT("ue_TargetHolder_")+ FString(Method), Args, Kind, Actor, Class);
    };
    auto IsActor = Run(TEXT("IsActor"), {Argument(Target, ActorPath)}, EDasMethodSignature::Bool);
    Expect(IsActor.bSucceeded && IsActor.BoolValue, TEXT("body_class_type_token_cast_and_short_name_check"));
    auto NullIsActor = Run(TEXT("IsActor"), {Argument(nullptr, ActorPath)}, EDasMethodSignature::Bool);
    Expect(NullIsActor.bSucceeded && !NullIsActor.BoolValue, TEXT("body_class_type_token_preserves_null_semantics"));
    auto Get = Run(TEXT("GetTarget"), {}, EDasMethodSignature::Object, ActorPath);
    Expect(Get.bSucceeded && Get.ObjectValue.Get() == Target, TEXT("typed_object_saved_field_getter"));
    auto Echo = Run(TEXT("EchoActor"), {Argument(Target, ActorPath)}, EDasMethodSignature::Object, ActorPath);
    Expect(Echo.bSucceeded && Echo.ObjectValue.Get() == Target, TEXT("typed_object_direct_macro_argument_and_result"));
    auto Null = Run(TEXT("SetTarget"), {Argument(nullptr, ActorPath)}, EDasMethodSignature::Void);
    Expect(Null.bSucceeded && !Field->GetObjectPropertyValue_InContainer(Actor), TEXT("typed_object_null_input"));
    auto Set = Run(TEXT("SetTarget"), {Argument(Target, ActorPath)}, EDasMethodSignature::Void);
    Expect(Set.bSucceeded && Field->GetObjectPropertyValue_InContainer(Actor) == Target,
           TEXT("typed_object_property_assignment"));
    auto* Method = Actor->FindFunction(TEXT("GetTarget"));
    FStructOnScope Memory(Method);
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
    Expect(CastFieldChecked<FObjectProperty>(Method->GetReturnProperty())
                   ->GetObjectPropertyValue_InContainer(Memory.GetStructMemory()) == Target,
           TEXT("typed_object_Blueprint_ProcessEvent_result"));
    auto* Owned = NewObject<UCurveFloat>(GetTransientPackage());
    TWeakObjectPtr<UCurveFloat> Weak = Owned;
    auto Wrong = Run(TEXT("SetTarget"), {Argument(Owned, ActorPath)}, EDasMethodSignature::Void);
    Expect(!Wrong.bSucceeded && Field->GetObjectPropertyValue_InContainer(Actor) == Target,
           TEXT("typed_object_wrong_native_argument_refused_before_body"));
    auto Any = Run(TEXT("EchoObject"), {Argument(Owned, ObjectPath)}, EDasMethodSignature::Object, ObjectPath);
    Expect(Any.bSucceeded && Any.ObjectValue.Get() == Owned, TEXT("typed_object_base_object_constraint"));
    auto Forged = Run(TEXT("Forged"), {Argument(Owned, ObjectPath)}, EDasMethodSignature::Object, ActorPath);
    Expect(!Forged.bSucceeded, TEXT("typed_object_forged_wrapper_wrong_result_refused"));
    auto Hold = Run(TEXT("SetHeld"), {Argument(Owned, ObjectPath)}, EDasMethodSignature::Void);
    CollectGarbage(RF_NoFlags);
    Expect(Hold.bSucceeded && Weak.IsValid() && Held->GetObjectPropertyValue_InContainer(Actor) == Weak.Get(),
           TEXT("typed_object_UPROPERTY_keeps_object_alive"));
    auto ClearHeld = Run(TEXT("ClearHeld"), {}, EDasMethodSignature::Void);
    CollectGarbage(RF_NoFlags);
    Expect(ClearHeld.bSucceeded && !Weak.IsValid(), TEXT("typed_object_VM_handles_do_not_root_object"));
    auto* Temporary = Actor->GetWorld()->SpawnActor<ADasProbeActor>();
    auto Remember = Run(TEXT("Remember"), {Argument(Temporary, ActorPath)}, EDasMethodSignature::Void);
    Temporary->Destroy();
    auto Recall = Run(TEXT("Recall"), {}, EDasMethodSignature::Object, ActorPath);
    Expect(Remember.bSucceeded && !Recall.bSucceeded, TEXT("typed_object_expired_actor_reference_refused"));
    if (Temporary)
        R.ReleaseObject(Temporary);
    auto Clear = Run(TEXT("Clear"), {}, EDasMethodSignature::Void);
    auto Empty = Run(TEXT("GetTarget"), {}, EDasMethodSignature::Object, ActorPath);
    Expect(Clear.bSucceeded && Empty.bSucceeded && !Empty.ObjectValue.IsValid(),
           TEXT("typed_object_empty_constructor_clears_field"));
    Field->SetObjectPropertyValue_InContainer(Actor, Target);
    R.ReleaseObject(Actor);
    return Pass;
}
