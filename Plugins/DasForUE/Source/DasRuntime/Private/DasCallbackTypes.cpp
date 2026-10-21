#include "DasCallbackTypes.h"
#include "DasTypeAdapters.h"
#include "InputActionValue.h"
#include "DasSoftReferences.h"
#include "DasCallFrame.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "DasContainerTypes.h"
#include "DasIntDelegateProxy.h"
#include "DasPropertyTypes.h"
#include "UObject/UnrealType.h"
#include "UObject/TextProperty.h"
#include "UObject/StrProperty.h"

namespace
{
bool CallbackText(const FText& Text, int64& Characters, FString& Error)
{
    const auto Namespace = FTextInspector::GetNamespace(Text), Key = FTextInspector::GetKey(Text);
    const auto* Source = FTextInspector::GetSourceString(Text);
    const int32 DisplayLength = Text.ToString().Len(), SourceLength = Source ? Source->Len() : 0;
    Characters += int64(DisplayLength) + SourceLength;
    if (DisplayLength > 65536 || SourceLength > 65536 || (Namespace.IsSet() && Namespace.GetValue().Len() > 1024) ||
        (Key.IsSet() && Key.GetValue().Len() > 1024) || Characters > 1048576)
    {
        Error = TEXT("Callback text exceeds source/display/identity limits");
        return false;
    }
    return true;
}
bool CallbackString(const FString& Text, int64& Characters, FString& Error)
{
    Characters += Text.Len();
    if (Text.Len() > 4096 || Characters > 1048576)
    {
        Error = TEXT("Callback string exceeds limit");
        return false;
    }
    for (TCHAR Ch : Text)
        if (Ch == 0)
        {
            Error = TEXT("Callback string contains NUL");
            return false;
        }
    return true;
}
bool QueuedProperty(const FProperty* P, TSet<const UScriptStruct*>& Active, int32 Depth, FString& Error)
{
    if (!P || P->ArrayDim != 1 || Depth > 16)
    {
        Error = TEXT("Unsupported callback value nesting");
        return false;
    }
    if (CastField<FSoftObjectProperty>(P))
        return DasSoftCarrierForProperty(P) != nullptr;
    switch (DasPropertyKind(P))
    {
    case EDasTypeKind::Object:
        return DasReferencePropertyClass(P) != nullptr;
    case EDasTypeKind::Class:
    {
        FDasDeclaredType T;
        if (!DescribeDasProperty(P, T, Error))
            return false;
        return ValidateDasCallbackTypePath(TEXT("class"), T.TypePath, Error);
    }
    case EDasTypeKind::Delegate:
    case EDasTypeKind::Event:
    case EDasTypeKind::Invalid:
        Error = TEXT("Queued structs cannot contain delegate references or unsupported fields");
        return false;
    case EDasTypeKind::Struct:
    {
        auto* S = CastFieldChecked<FStructProperty>(P)->Struct.Get();
        if (Active.Contains(S))
        {
            Error = TEXT("Recursive callback struct");
            return false;
        }
        Active.Add(S);
        for (TFieldIterator<FProperty> It(S); It; ++It)
            if (!QueuedProperty(*It, Active, Depth + 1, Error))
                return false;
        Active.Remove(S);
        return true;
    }
    case EDasTypeKind::Array:
        return QueuedProperty(CastFieldChecked<FArrayProperty>(P)->Inner, Active, Depth + 1, Error);
    case EDasTypeKind::Set:
        return QueuedProperty(CastFieldChecked<FSetProperty>(P)->ElementProp, Active, Depth + 1, Error);
    case EDasTypeKind::Map:
    {
        auto* M = CastFieldChecked<FMapProperty>(P);
        return QueuedProperty(M->KeyProp, Active, Depth + 1, Error) &&
               QueuedProperty(M->ValueProp, Active, Depth + 1, Error);
    }
    case EDasTypeKind::Enum:
    {
        FDasDeclaredType T;
        if (!DescribeDasProperty(P, T, Error))
            return false;
        return ValidateDasPropertyType(T, EDasPropertyUsage::Value, Error);
    }
    default:
        return true;
    }
}
bool ValueBudget(const FProperty* P, const void* Memory, int64& Characters, int32& Elements, FString& Error)
{
    if (auto* Soft = CastField<FSoftObjectProperty>(P))
        return ValidateDasSoftValue(Soft, Memory, Error) &&
               CallbackString(Soft->GetPropertyValue(Memory).ToSoftObjectPath().ToString(), Characters, Error);
    if (auto* C = CastField<FClassProperty>(P))
    {
        auto* Raw = C->GetObjectPropertyValue(Memory);
        auto* O = Cast<UClass>(Raw);
        if ((Raw && !O) || !C->MetaClass || !DasCallbackObjectAlive(O) || (O && !O->IsChildOf(C->MetaClass)))
        {
            Error = TEXT("Callback class is obsolete or has an incompatible base");
            return false;
        }
    }
    else if (auto* ObjectClass = DasReferencePropertyClass(P))
    {
        auto* O = DasReferenceGet(P, Memory);
        if (!DasCallbackObjectAlive(O) || !DasReferenceMatches(O, ObjectClass))
        {
            Error = TEXT("Callback object is destroyed or has an incompatible class/interface");
            return false;
        }
    }
    else if (auto* S = CastField<FStructProperty>(P))
    {
        for (TFieldIterator<FProperty> It(S->Struct); It; ++It)
            if (!ValueBudget(*It, It->ContainerPtrToValuePtr<void>(Memory), Characters, Elements, Error))
                return false;
    }
    else if (auto* A = CastField<FArrayProperty>(P))
    {
        FScriptArrayHelper V(A, Memory);
        Elements += V.Num();
        if (Elements > 4096)
        {
            Error = TEXT("Callback collection exceeds element limit");
            return false;
        }
        for (int32 I = 0; I < V.Num(); ++I)
            if (!ValueBudget(A->Inner, V.GetRawPtr(I), Characters, Elements, Error))
                return false;
    }
    else if (auto* Set = CastField<FSetProperty>(P))
    {
        FScriptSetHelper V(Set, Memory);
        Elements += V.Num();
        if (Elements > 4096)
        {
            Error = TEXT("Callback collection exceeds element limit");
            return false;
        }
        for (int32 I = 0; I < V.GetMaxIndex(); ++I)
            if (V.IsValidIndex(I) && !ValueBudget(Set->ElementProp, V.GetElementPtr(I), Characters, Elements, Error))
                return false;
    }
    else if (auto* M = CastField<FMapProperty>(P))
    {
        FScriptMapHelper V(M, Memory);
        Elements += V.Num();
        if (Elements > 4096)
        {
            Error = TEXT("Callback collection exceeds element limit");
            return false;
        }
        for (int32 I = 0; I < V.GetMaxIndex(); ++I)
            if (V.IsValidIndex(I) && (!ValueBudget(M->KeyProp, V.GetKeyPtr(I), Characters, Elements, Error) ||
                                      !ValueBudget(M->ValueProp, V.GetValuePtr(I), Characters, Elements, Error)))
                return false;
    }
    else if (auto* T = CastField<FTextProperty>(P))
    {
        if (!CallbackText(T->GetPropertyValue(Memory), Characters, Error))
            return false;
    }
    else if (auto* String = CastField<FStrProperty>(P))
    {
        if (!CallbackString(String->GetPropertyValue(Memory), Characters, Error))
            return false;
    }
    else if (auto* F = CastField<FFloatProperty>(P); F && !FMath::IsFinite(F->GetPropertyValue(Memory)))
    {
        Error = TEXT("Nonfinite callback float field");
        return false;
    }
    else if (auto* D = CastField<FDoubleProperty>(P); D && !FMath::IsFinite(D->GetPropertyValue(Memory)))
    {
        Error = TEXT("Nonfinite callback double field");
        return false;
    }
    return true;
}

}

TConstArrayView<FDasCallbackType> DasCallbackTypes()
{
    using K = EDasMethodSignature;
#define PROXY(Name) GET_FUNCTION_NAME_CHECKED(UDasIntDelegateProxy, Name)
    static const FDasCallbackType Types[] = {
        {TEXT("class"), K::Class, nullptr, true, NAME_None, NAME_None, true},
         {TEXT("enum"), K::Enum, nullptr, true, NAME_None, NAME_None, true},
          {TEXT("struct"), K::Struct, nullptr, true, NAME_None, NAME_None, true},
           {TEXT("void"), K::Void, nullptr, true, PROXY(ReceiveVoid), NAME_None},
            {TEXT("byte"), K::Byte, nullptr, true, PROXY(ReceiveByte), NAME_None},
             {TEXT("text"), K::Text, nullptr, true, PROXY(ReceiveText), PROXY(ReceiveTextRef)},
              {TEXT("int"), K::Int, nullptr, true, PROXY(Receive), NAME_None},
               {TEXT("float"), K::Float, nullptr, true, PROXY(ReceiveFloat), NAME_None},
                {TEXT("double"), K::Double, nullptr, true, PROXY(ReceiveDouble), NAME_None},
                 {TEXT("bool"), K::Bool, nullptr, true, PROXY(ReceiveBool), NAME_None},
                  {TEXT("string"), K::String, nullptr, true, PROXY(ReceiveString), PROXY(ReceiveString)},
                   {TEXT("name"), K::Name, nullptr, true, PROXY(ReceiveName), NAME_None},
                    {TEXT("object"), K::Object, nullptr, true, PROXY(ReceiveObject), NAME_None, true},
                     {TEXT("vector"), K::Vector, nullptr, true, PROXY(ReceiveVector), PROXY(ReceiveVectorRef)},
                      {TEXT("rotator"), K::Rotator, nullptr, true, PROXY(ReceiveRotator), PROXY(ReceiveRotatorRef)},
                       {TEXT("transform"), K::Transform, nullptr, true, PROXY(ReceiveTransform),
                             PROXY(ReceiveTransformRef)},
                        {TEXT("vector2d"), K::Struct, &TBaseStructure<FVector2D>::Get, true, PROXY(ReceiveVector2D),
                              PROXY(ReceiveVector2DRef)},
                         {TEXT("array_byte"), K::Struct, &FDasArrayByteCarrier::StaticStruct, true, NAME_None,
                               PROXY(ReceiveArrayByte), true},
                          {TEXT("array_text"), K::Struct, &FDasArrayTextCarrier::StaticStruct, true, NAME_None,
                                PROXY(ReceiveArrayText), true},
                           {TEXT("array_int"), K::Struct, &FDasArrayIntCarrier::StaticStruct, true, NAME_None,
                                 PROXY(ReceiveArrayInt), true},
                            {TEXT("array_float"), K::Struct, &FDasArrayFloatCarrier::StaticStruct, true, NAME_None,
                                  PROXY(ReceiveArrayFloat), true},
                             {TEXT("array_double"), K::Struct, &FDasArrayDoubleCarrier::StaticStruct, true, NAME_None,
                                   PROXY(ReceiveArrayDouble), true},
                              {TEXT("array_bool"), K::Struct, &FDasArrayBoolCarrier::StaticStruct, true, NAME_None,
                                    PROXY(ReceiveArrayBool), true},
                               {TEXT("array_string"), K::Struct, &FDasArrayStringCarrier::StaticStruct, true, NAME_None,
                                     PROXY(ReceiveArrayString), true},
                                {TEXT("array_name"), K::Struct, &FDasArrayNameCarrier::StaticStruct, true, NAME_None,
                                      PROXY(ReceiveArrayName), true},
                               };
#undef PROXY
    return MakeArrayView(Types);
}
const FDasCallbackType* FindDasCallbackType(const FString& Name)
{
    for (const auto& Type : DasCallbackTypes())
        if (Name == Type.Name)
            return &Type;
    return nullptr;
}
const FDasCallbackType* FindDasCallbackType(EDasMethodSignature Signature, const UScriptStruct* Snapshot)
{
    for (const auto& Type : DasCallbackTypes())
        if (Type.Signature == Signature && (Type.Snapshot ? Type.Snapshot() : nullptr) == Snapshot)
            return &Type;
    if (Signature == EDasMethodSignature::Struct && Snapshot)
        return FindDasCallbackType(TEXT("struct"));
    return nullptr;
}
const FDasCallbackType* DasCallbackTypeForProperty(const FProperty* Property)
{
    if (!Property || Property->ArrayDim != 1)
        return nullptr;
    switch (DasPropertyKind(Property))
    {
    case EDasTypeKind::Enum:
    {
        FDasDeclaredType T;
        FString Error;
        if (!DescribeDasProperty(Property, T, Error) || !ValidateDasCallbackTypePath(TEXT("enum"), T.TypePath, Error))
            return nullptr;
        return FindDasCallbackType(TEXT("enum"));
    }
    case EDasTypeKind::Array:
    case EDasTypeKind::Set:
    case EDasTypeKind::Map:
    {
        auto* C = DasCallbackSnapshotForProperty(Property);
        FString Error;
        return C && ValidateDasCallbackTypePath(TEXT("struct"), C->GetPathName(), Error)
                                                ? FindDasCallbackType(EDasMethodSignature::Struct, C)
                                                : nullptr;
    }
    case EDasTypeKind::Struct:
    {
        auto* Type = CastFieldChecked<FStructProperty>(Property)->Struct.Get();
        // Structs with their own signature (Vector, Rotator, Transform) have a callback type of their kind name.
        if (const auto* Adapter = FDasTypeAdapters::Get().FindStruct(Type); Adapter && Adapter->GetSignature())
            return FindDasCallbackType(Adapter->GetDefaultKind());
        FString Error;
        if (!ValidateDasCallbackTypePath(TEXT("struct"), Type->GetPathName(), Error))
            return nullptr;
        return FindDasCallbackType(EDasMethodSignature::Struct, Type);
    }
    case EDasTypeKind::Class:
    {
        auto* C = CastFieldChecked<FClassProperty>(Property);
        FString Error;
        return C->MetaClass && ValidateDasCallbackTypePath(TEXT("class"), C->MetaClass->GetPathName(), Error)
               ? FindDasCallbackType(TEXT("class"))
               : nullptr;
    }
    case EDasTypeKind::Object:
        return DasReferencePropertyClass(Property) ? FindDasCallbackType(TEXT("object")) : nullptr;
    default:
        return FindDasCallbackType(DasTypeKindName(DasPropertyKind(Property)));
    }
}
bool ValidateDasCallbackTypePath(const FString& Kind, const FString& Path, FString& Error)
{
    Error.Reset();
    if (Path.IsEmpty() || Path.Len() > 1024)
    {
        Error = TEXT("Concrete loaded callback type identity is required");
        return false;
    }
    if (Kind == TEXT("object"))
    {
        auto* C = FindObject<UClass>(nullptr, *Path);
        if (!C || C->HasAnyClassFlags(CLASS_NewerVersionExists))
        {
            Error = TEXT("Missing or obsolete callback object/interface type");
            return false;
        }
        return true;
    }
    if (Kind == TEXT("class"))
    {
        auto* C = FindObject<UClass>(nullptr, *Path);
        if (!C || C->HasAnyClassFlags(CLASS_Interface | CLASS_Deprecated | CLASS_NewerVersionExists))
        {
            Error = TEXT("Missing or obsolete callback class base");
            return false;
        }
        return true;
    }
    if (Kind == TEXT("enum"))
    {
        FDasDeclaredType T;
        T.Kind = Kind;
        T.TypePath = Path;
        return ValidateDasPropertyType(T, EDasPropertyUsage::Value, Error);
    }
    if (Kind != TEXT("struct"))
    {
        Error = TEXT("Unexpected callback type identity");
        return false;
    }
    if (!ValidateDasValueStruct(Path, Error))
        return false;
    auto* S = FindObject<UScriptStruct>(nullptr, *Path);
    TSet<const UScriptStruct*> Active;
    Active.Add(S);
    for (TFieldIterator<FProperty> It(S); It; ++It)
        if (!QueuedProperty(*It, Active, 0, Error))
            return false;
    return true;
}
bool ValidateDasCallbackPayload(const FDasScalarValue& Value, EDasCallbackUsage Usage, FString& Error)
{
    Error.Reset();
    if (Value.Kind < 0 || Value.Kind > MAX_uint8)
    {
        Error = TEXT("Invalid callback ABI kind");
        return false;
    }
    const auto* Type = FindDasCallbackType(
        static_cast<EDasMethodSignature>(Value.Kind),
        Value.Kind == int32(EDasMethodSignature::Struct) ? Value.StructValue.GetScriptStruct() : nullptr);
    if (!Type || (Usage == EDasCallbackUsage::Timer && !Type->bTimer) || Value.Direction != 0)
    {
        Error = TEXT("Unsupported callback payload or direction");
        return false;
    }
    const auto K = Type->Signature;
    if ((K == EDasMethodSignature::Float && !FMath::IsFinite(Value.FloatValue)) ||
        (K == EDasMethodSignature::Double && !FMath::IsFinite(Value.DoubleValue)) ||
        (K == EDasMethodSignature::Vector && Value.VectorValue.ContainsNaN()) ||
        (K == EDasMethodSignature::Rotator && Value.RotatorValue.ContainsNaN()) ||
        (K == EDasMethodSignature::Transform &&
         (Value.TransformValue.ContainsNaN() || !Value.TransformValue.IsRotationNormalized())))
    {
        Error = TEXT("Nonfinite or invalid callback math value");
        return false;
    }
    int64 Characters = 0;
    if (K == EDasMethodSignature::String)
        return CallbackString(Value.StringValue, Characters, Error);
    if (K == EDasMethodSignature::Text)
        return CallbackText(Value.TextValue, Characters, Error);
    if (K == EDasMethodSignature::Enum)
        return ValidateDasCallbackTypePath(TEXT("enum"), Value.ObjectClass, Error) &&
                                           ValidateDasEnumNumber(Value.ObjectClass, Value.IntValue, Error);
    if (K == EDasMethodSignature::Object)
    {
        const FString Path = Value.ObjectClass.IsEmpty() ? TEXT("/Script/CoreUObject.Object"): Value.ObjectClass;
        if (!ValidateDasCallbackTypePath(TEXT("object"), Path, Error))
            return false;
        if (!DasCallbackObjectAlive(Value.ObjectValue.Get()) ||
            !DasReferenceMatches(Value.ObjectValue.Get(), FindObject<UClass>(nullptr, *Path)))
        {
            Error = TEXT("Callback object is destroyed or has an incompatible class/interface");
            return false;
        }
        return true;
    }
    if (K == EDasMethodSignature::Class)
    {
        if (!ValidateDasCallbackTypePath(TEXT("class"), Value.ObjectClass, Error))
            return false;
        auto* C = Value.ClassValue.Get();
        if (!DasCallbackObjectAlive(C) || (C && !C->IsChildOf(FindObject<UClass>(nullptr, *Value.ObjectClass))))
        {
            Error = TEXT("Callback class is obsolete or has an incompatible base");
            return false;
        }
        return true;
    }
    if (K != EDasMethodSignature::Struct)
        return true;
    auto* Snapshot = Value.StructValue.GetScriptStruct();
    if (!Value.StructValue.IsValid() || !Snapshot || Snapshot->GetPathName() != Value.ObjectClass ||
        (Type->Snapshot && Type->Snapshot() != Snapshot))
    {
        Error = TEXT("Callback snapshot identity mismatch");
        return false;
    }
    if (!ValidateDasCallbackTypePath(TEXT("struct"), Value.ObjectClass, Error) ||
                                     !ValidateDasStructSnapshot(Value.StructValue, Error))
        return false;
    if (Snapshot == FInputActionValue::StaticStruct())
    {
        const auto& V = Value.StructValue.Get<FInputActionValue>();
        const int32 Kind = int32(V.GetValueType());
        if (Kind < 0 || Kind > 3 || V.Get<FVector>().ContainsNaN())
        {
            Error = TEXT("Invalid InputActionValue callback snapshot");
            return false;
        }
    }
    int32 Elements = 0;
    for (TFieldIterator<FProperty> It(Snapshot); It; ++It)
        if (!ValueBudget(*It, It->ContainerPtrToValuePtr<void>(Value.StructValue.GetMemory()), Characters, Elements,
                         Error))
            return false;
    return true;
}

bool DasCallbackObjectAlive(const UObject* Object)
{
    if (!Object)
        return true;
    if (!IsValid(Object))
        return false;
    if (auto* C = Cast<UClass>(Object))
        return !C->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists);
    if (auto* A = Cast<AActor>(Object))
        return !A->IsActorBeingDestroyed();
    if (auto* C = Cast<UActorComponent>(Object))
        return !C->IsBeingDestroyed() && (!C->GetOwner() || DasCallbackObjectAlive(C->GetOwner()));
    return true;
}

UScriptStruct* DasCallbackSnapshotForProperty(const FProperty* Property)
{
    if (auto* Struct = CastField<FStructProperty>(Property))
        return Struct->Struct;
    return Property ? DasContainerCarrierForProperty(Property) : nullptr;
}
