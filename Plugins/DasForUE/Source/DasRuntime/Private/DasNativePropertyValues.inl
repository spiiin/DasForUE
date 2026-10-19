// Included inside the runtime namespace: shared native value transport adapters.
template <class P> struct TPropertyScalarSlot;
#define DAS_PROPERTY_SCALAR_SLOT(Property, Signature, Field)                                                           \
    template <> struct TPropertyScalarSlot<Property>                                                                   \
    {                                                                                                                  \
        static constexpr EDasMethodSignature Kind = EDasMethodSignature::Signature;                                    \
        static constexpr auto Member = &FDasScalarValue::Field;                                                        \
    };
DAS_PROPERTY_SCALAR_SLOT(FIntProperty, Int, IntValue)
DAS_PROPERTY_SCALAR_SLOT(FFloatProperty, Float, FloatValue)
DAS_PROPERTY_SCALAR_SLOT(FDoubleProperty, Double, DoubleValue)
DAS_PROPERTY_SCALAR_SLOT(FBoolProperty, Bool, BoolValue)
DAS_PROPERTY_SCALAR_SLOT(FStrProperty, String, StringValue)
DAS_PROPERTY_SCALAR_SLOT(FNameProperty, Name, NameValue)
DAS_PROPERTY_SCALAR_SLOT(FByteProperty, Byte, ByteValue)
DAS_PROPERTY_SCALAR_SLOT(FTextProperty, Text, TextValue)
#undef DAS_PROPERTY_SCALAR_SLOT
FDasScalarValue ReadNativePropertyValue(FProperty* Property, const void* Memory, das::Context* Context)
{
    FDasScalarValue Value;
    FString Error;
    if (!ReadDasPropertyValue(Property, Memory, Value, Error))
        Context->throw_error(TCHAR_TO_UTF8(*Error));
    return Value;
}
void WriteNativePropertyValue(FProperty* Property, void* Memory, const FDasScalarValue& Value, das::Context* Context)
{
    FString Error;
    if (!WriteDasPropertyValue(Property, Memory, Value, Error))
        Context->throw_error(TCHAR_TO_UTF8(*Error));
}

// Symbol-based native APIs also accept wide enum storage and hidden native
// symbols. Their callers validate symbols; authored enum values stay byte-sized.
bool NativeEnumUsesValueCodec(FProperty* Property, int64 Number)
{
    FDasDeclaredType Type;
    FString Error;
    return DescribeDasProperty(Property, Type, Error) &&
           ValidateDasPropertyType(Type, EDasPropertyUsage::Value, Error) &&
           ValidateDasEnumNumber(Type.TypePath, Number, Error);
}
FNumericProperty* NativeEnumStorage(FProperty* Property, das::Context* C)
{
    auto* Storage = CastField<FEnumProperty>(Property)
                        ? CastFieldChecked<FEnumProperty>(Property)->GetUnderlyingProperty()
                        : CastField<FByteProperty>(Property);
    if (!Storage || !DasEnumPropertyType(Property))
        C->throw_error("expected native enum property");
    return Storage;
}
int64 ReadNativeEnumNumber(FProperty* Property, const void* Memory, das::Context* C)
{
    const int64 Number = NativeEnumStorage(Property, C)->GetSignedIntPropertyValue(Memory);
    return NativeEnumUsesValueCodec(Property, Number) ? ReadNativePropertyValue(Property, Memory, C).IntValue : Number;
}
void WriteNativeEnumNumber(FProperty* Property, void* Memory, UEnum* Enum, int64 Number, das::Context* C)
{
    if (!Enum || DasEnumPropertyType(Property) != Enum)
        C->throw_error("enum assignment type mismatch");
    if (NativeEnumUsesValueCodec(Property, Number))
    {
        FDasScalarValue Value;
        Value.Kind = int32(EDasMethodSignature::Enum);
        Value.ObjectClass = Enum->GetPathName();
        Value.IntValue = int32(Number);
        WriteNativePropertyValue(Property, Memory, Value, C);
    }
    else
        NativeEnumStorage(Property, C)->SetIntPropertyValue(Memory, Number);
}

// Typed VM adapters share this slot table with reflected calls. Access policy
// is checked by the caller before handing the property and its owner here.
template <class P> auto ReadPropertyScalarValue(P* Property, const void* Memory, das::Context* C)
{
    const auto Value = ReadNativePropertyValue(Property, Memory, C);
    if (Value.Kind != int32(TPropertyScalarSlot<P>::Kind))
        C->throw_error("property scalar type mismatch");
    return Value.*TPropertyScalarSlot<P>::Member;
}
template <class P, class T> FDasScalarValue PropertyScalarValue(const T& Input)
{
    FDasScalarValue Value;
    Value.Kind = int32(TPropertyScalarSlot<P>::Kind);
    Value.*TPropertyScalarSlot<P>::Member = Input;
    return Value;
}
template <class P, class T> void WritePropertyScalarValue(P* Property, void* Memory, const T& Input, das::Context* C)
{
    WriteNativePropertyValue(Property, Memory, PropertyScalarValue<P>(Input), C);
}
FDasPropertyAccess ObjectPropertyAccess(FProperty* P, das::Context* C)
{
    // FName equality matches the former case-insensitive FString comparison of names.
    if (const auto* Rows = DasNativeGameplayFieldsNamed(P->GetFName()))
        if (const UClass* Owner = P->GetOwnerClass())
        {
            const FString OwnerPath = Owner->GetPathName();
            for (const auto* Row : *Rows)
                if (OwnerPath == Row->Declaring)
                    return DasNativeAccessContract(*Row);
        }
    FDasPropertyAccess Access;
    FString Error;
    if (!DescribeDasPropertyAccess(P, Access, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    return Access;
}
FDasScalarValue ReadOwnerProperty(FProperty* P, UObject* O, das::Context* C)
{
    FDasScalarValue V;
    FString Error;
    if (!ReadDasObjectProperty(P, O, ObjectPropertyAccess(P, C), ConstructingObject, V, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    return V;
}
void WriteOwnerProperty(FProperty* P, UObject* O, const FDasScalarValue& V, das::Context* C)
{
    FString Error;
    if (!WriteDasObjectProperty(P, O, ObjectPropertyAccess(P, C), ConstructingObject, V, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
}
template <class P> auto ReadPropertyScalar(P* Property, UObject* Owner, das::Context* C)
{
    const auto V = ReadOwnerProperty(Property, Owner, C);
    if (V.Kind != int32(TPropertyScalarSlot<P>::Kind))
        C->throw_error("property scalar type mismatch");
    return V.*TPropertyScalarSlot<P>::Member;
}
template <class P, class T> void WritePropertyScalar(P* Property, UObject* Owner, const T& Input, das::Context* C)
{
    WriteOwnerProperty(Property, Owner, PropertyScalarValue<P>(Input), C);
}
