#include "DasPropertyValues.h"
#include "DasValueTraversal.h"
#include "DasPropertyTypes.h"
#include "DasContainerTypes.h"
#include "DasSchema.h"
#include "UObject/UnrealType.h"
#include "UObject/StrProperty.h"
#include "UObject/TextProperty.h"

namespace
{
bool ValueType(FProperty* P, FDasDeclaredType& Type, FString& Error)
{
    if (!IsInGameThread())
    {
        Error = TEXT("Property values require the game thread");
        return false;
    }
    return DescribeDasProperty(P, Type, Error) && ValidateDasPropertyType(Type, EDasPropertyUsage::Value, Error);
}
EDasMethodSignature FrameKind(EDasTypeKind Kind)
{
    switch (Kind)
    {
    case EDasTypeKind::Byte:
        return EDasMethodSignature::Byte;
    case EDasTypeKind::Text:
        return EDasMethodSignature::Text;
    case EDasTypeKind::Int:
        return EDasMethodSignature::Int;
    case EDasTypeKind::Float:
        return EDasMethodSignature::Float;
    case EDasTypeKind::Double:
        return EDasMethodSignature::Double;
    case EDasTypeKind::Bool:
        return EDasMethodSignature::Bool;
    case EDasTypeKind::String:
        return EDasMethodSignature::String;
    case EDasTypeKind::Name:
        return EDasMethodSignature::Name;
    case EDasTypeKind::Enum:
        return EDasMethodSignature::Enum;
    case EDasTypeKind::Object:
        return EDasMethodSignature::Object;
    case EDasTypeKind::Class:
        return EDasMethodSignature::Class;
    default:
        return EDasMethodSignature::Struct;
    }
}
UScriptStruct* SnapshotType(FProperty* P)
{
    if (auto* Struct = CastField<FStructProperty>(P))
        return Struct->Struct;
    return DasContainerCarrierForProperty(P);
}
bool WriteSnapshotMemory(FProperty* P, void* Memory, const FInstancedStruct& Snapshot, FString& Error)
{
    auto* Struct = SnapshotType(P);
    if (!Struct || Struct != Snapshot.GetScriptStruct() || !ValidateDasStructSnapshot(Snapshot, Error))
    {
        if (Error.IsEmpty())
            Error = TEXT("Property snapshot type mismatch");
        return false;
    }
    if (CastField<FStructProperty>(P))
        Struct->CopyScriptStruct(Memory, Snapshot.GetMemory());
    else
    {
        auto* Values = Struct->FindPropertyByName(TEXT("Values"));
        P->CopyCompleteValue(Memory, Values->ContainerPtrToValuePtr<void>(Snapshot.GetMemory()));
    }
    return true;
}
struct FRemapValues
{
    const TMap<UObject*, UObject*>& Replacements;
    bool Struct(const UScriptStruct*, uint8*) { return true; }
    bool Begin(FProperty*, uint8*, int32, int64) { return true; }
    bool Leaf(FProperty* P, uint8* Memory, DasValueTraversal::ERole)
    {
        if (auto* Object = CastField<FObjectProperty>(P))
        {
            if (auto* Replacement = Replacements.Find(Object->GetObjectPropertyValue(Memory)))
                Object->SetObjectPropertyValue(Memory, *Replacement);
        }
        else if (CastField<FInterfaceProperty>(P) || CastField<FWeakObjectProperty>(P))
        {
            if (auto* Replacement = Replacements.Find(DasReferenceGet(P, Memory)))
                DasReferenceSet(P, Memory, *Replacement);
        }
        return true;
    }
    void End(FProperty* P, uint8* Memory)
    {
        // Object identities participate in hashes. Repair only after all entries
        // have been visited; sparse indexes must remain stable during the walk.
        if (auto* Set = CastField<FSetProperty>(P))
            FScriptSetHelper(Set, Memory).Rehash();
        else if (auto* Map = CastField<FMapProperty>(P))
            FScriptMapHelper(Map, Memory).Rehash();
    }
};
}

bool ReadDasPropertyValue(FProperty* P, const void* Memory, FDasScalarValue& Out, FString& Error)
{
    Error.Reset();
    FDasDeclaredType Type;
    if (!ValueType(P, Type, Error))
        return false;
    if (!Memory)
    {
        Error = TEXT("Missing property value memory");
        return false;
    }
    FDasScalarValue Value;
    Value.Kind = int32(FrameKind(Type.GetKind()));
    switch (Type.GetKind())
    {
    case EDasTypeKind::Byte:
        Value.ByteValue = CastFieldChecked<FByteProperty>(P)->GetPropertyValue(Memory);
        break;
    case EDasTypeKind::Text:
        Value.TextValue = CastFieldChecked<FTextProperty>(P)->GetPropertyValue(Memory);
        break;
    case EDasTypeKind::Int:
        Value.IntValue = CastFieldChecked<FIntProperty>(P)->GetPropertyValue(Memory);
        break;
    case EDasTypeKind::Float:
        Value.FloatValue = CastFieldChecked<FFloatProperty>(P)->GetPropertyValue(Memory);
        break;
    case EDasTypeKind::Double:
        Value.DoubleValue = CastFieldChecked<FDoubleProperty>(P)->GetPropertyValue(Memory);
        break;
    case EDasTypeKind::Bool:
        Value.BoolValue = CastFieldChecked<FBoolProperty>(P)->GetPropertyValue(Memory);
        break;
    case EDasTypeKind::String:
        Value.StringValue = CastFieldChecked<FStrProperty>(P)->GetPropertyValue(Memory);
        break;
    case EDasTypeKind::Name:
        Value.NameValue = CastFieldChecked<FNameProperty>(P)->GetPropertyValue(Memory);
        break;
    case EDasTypeKind::Enum:
    {
        const int64 Number = DasEnumPropertyNumber(P, Memory);
        if (!ValidateDasEnumNumber(Type.TypePath, Number, Error))
            return false;
        Value.ObjectClass = Type.TypePath;
        Value.IntValue = int32(Number);
        break;
    }
    case EDasTypeKind::Class:
    {
        auto* Class = CastFieldChecked<FClassProperty>(P);
        auto* Object = Class->GetObjectPropertyValue(Memory);
        if (Object &&
            (!IsValid(Object) || !Object->IsA<UClass>() || !CastChecked<UClass>(Object)->IsChildOf(Class->MetaClass)))
        {
            Error = TEXT("Incompatible class value");
            return false;
        }
        Value.ObjectClass = Type.TypePath;
        Value.ClassValue = Cast<UClass>(Object);
        break;
    }
    case EDasTypeKind::Object:
        Value.ObjectClass = Type.TypePath;
        Value.ObjectValue = DasReferenceGet(P, Memory);
        if (Value.ObjectValue &&
            (!IsValid(Value.ObjectValue) || !DasReferenceMatches(Value.ObjectValue, DasReferencePropertyClass(P))))
        {
            Error = TEXT("Incompatible object/interface value");
            return false;
        }
        break;
    default:
    {
        auto* Struct = SnapshotType(P);
        if (!Struct)
        {
            Error = TEXT("No owned transport for property value");
            return false;
        }
        Value.ObjectClass = Struct->GetPathName();
        if (!ValidateDasValueStruct(Struct->GetPathName(), Error))
            return false;
        Value.StructValue.InitializeAs(Struct);
        if (CastField<FStructProperty>(P))
            Struct->CopyScriptStruct(Value.StructValue.GetMutableMemory(), Memory);
        else
        {
            auto* Values = Struct->FindPropertyByName(TEXT("Values"));
            Values->CopyCompleteValue(Values->ContainerPtrToValuePtr<void>(Value.StructValue.GetMutableMemory()),
                                      Memory);
        }
        if (!ValidateDasStructSnapshot(Value.StructValue, Error))
            return false;
        break;
    }
    }
    Out = MoveTemp(Value);
    return true;
}

bool WriteDasPropertyValue(FProperty* P, void* Memory, const FDasScalarValue& Value, FString& Error)
{
    Error.Reset();
    FDasDeclaredType Type;
    if (!ValueType(P, Type, Error))
        return false;
    if (!Memory)
    {
        Error = TEXT("Missing property value memory");
        return false;
    }
    if (Value.Kind != int32(FrameKind(Type.GetKind())))
    {
        Error = TEXT("Property value kind mismatch");
        return false;
    }
    switch (Type.GetKind())
    {
    case EDasTypeKind::Byte:
        CastFieldChecked<FByteProperty>(P)->SetPropertyValue(Memory, Value.ByteValue);
        break;
    case EDasTypeKind::Text:
        CastFieldChecked<FTextProperty>(P)->SetPropertyValue(Memory, Value.TextValue);
        break;
    case EDasTypeKind::Int:
        CastFieldChecked<FIntProperty>(P)->SetPropertyValue(Memory, Value.IntValue);
        break;
    case EDasTypeKind::Float:
        CastFieldChecked<FFloatProperty>(P)->SetPropertyValue(Memory, Value.FloatValue);
        break;
    case EDasTypeKind::Double:
        CastFieldChecked<FDoubleProperty>(P)->SetPropertyValue(Memory, Value.DoubleValue);
        break;
    case EDasTypeKind::Bool:
        CastFieldChecked<FBoolProperty>(P)->SetPropertyValue(Memory, Value.BoolValue);
        break;
    case EDasTypeKind::String:
        CastFieldChecked<FStrProperty>(P)->SetPropertyValue(Memory, Value.StringValue);
        break;
    case EDasTypeKind::Name:
        CastFieldChecked<FNameProperty>(P)->SetPropertyValue(Memory, Value.NameValue);
        break;
    case EDasTypeKind::Enum:
        if (Type.TypePath != Value.ObjectClass || !SetDasEnumPropertyNumber(P, Memory, Value.IntValue))
        {
            Error = TEXT("Property enum identity/member mismatch");
            return false;
        }
        break;
    case EDasTypeKind::Class:
    {
        auto* Target = Value.ClassValue.Get();
        auto* Class = CastFieldChecked<FClassProperty>(P);
        if (Type.TypePath != Value.ObjectClass ||
            (Target && (!IsValid(Target) || !Target->IsChildOf(Class->MetaClass))))
        {
            Error = TEXT("Property class identity/value mismatch");
            return false;
        }
        Class->SetObjectPropertyValue(Memory, Target);
        break;
    }
    case EDasTypeKind::Object:
        if (Type.TypePath != Value.ObjectClass || (Value.ObjectValue && !IsValid(Value.ObjectValue)) ||
            !DasReferenceSet(P, Memory, Value.ObjectValue))
        {
            Error = TEXT("Property object/interface identity/value mismatch");
            return false;
        }
        break;
    default:
        return WriteSnapshotMemory(P, Memory, Value.StructValue, Error);
    }
    return true;
}

bool WriteDasPropertySnapshot(FProperty* P, void* Memory, const FInstancedStruct& Snapshot, FString& Error)
{
    Error.Reset();
    FDasDeclaredType Type;
    if (!ValueType(P, Type, Error))
        return false;
    if (!Memory)
    {
        Error = TEXT("Missing property value memory");
        return false;
    }
    return WriteSnapshotMemory(P, Memory, Snapshot, Error);
}

void FDasPropertyValueRoots::AddReferencedObjects(FReferenceCollector& Collector)
{
    for (auto& Value : Values)
        Collector.AddPropertyReferencesWithStructARO(FDasScalarValue::StaticStruct(), &Value);
}
void FDasPropertyValueRoots::Remap(FDasScalarValue& Value, const TMap<UObject*, UObject*>& Map)
{
    if (auto* Replacement = Map.Find(Value.ObjectValue.Get()))
        Value.ObjectValue = *Replacement;
    if (auto* Replacement = Map.Find(Value.ClassValue.Get()))
        Value.ClassValue = Cast<UClass>(*Replacement);
    if (Value.StructValue.IsValid())
    {
        FRemapValues Policy{Map};
        DasValueTraversal::Struct(Value.StructValue.GetScriptStruct(), Value.StructValue.GetMutableMemory(), Policy);
    }
}
