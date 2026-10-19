// Part of DasRuntime.cpp: Struct snapshots: creation, field paths, map/set members and frame publication.
// Included in place; declaration order matters.
FDasStructValue NewStructValue(const char* Path, das::Context* C, das::LineInfoArg* At)
{
    FString P = Path ? UTF8_TO_TCHAR(Path) : TEXT("");
    FString E;
    if (!ValidateDasValueStruct(P, E))
        C->throw_error(TCHAR_TO_UTF8(*E));
    FInstancedStruct S;
    S.InitializeAs(FindObject<UScriptStruct>(nullptr, *P));
    return StoreStructValue(MoveTemp(S), C, At);
}
template <class Byte> using TStructLeaf = DasSnapshotFields::TField<Byte>;
using FStructLeaf = TStructLeaf<uint8>;
using FConstStructLeaf = TStructLeaf<const uint8>;
template <class Byte>
TStructLeaf<Byte> StructLeafMemory(const UScriptStruct* Type, Byte* Memory, const char* Path, das::Context* C)
{
    TStructLeaf<Byte> Field{};
    FString Error;
    if (C != ActiveContext || !ActiveContext)
        C->throw_error("struct field access requires active context");
    if (!ActiveContext->SnapshotFieldPaths.Find(Type, Memory,
                                                FString(Path ? UTF8_TO_TCHAR(Path) : TEXT("")), Field, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    return Field;
}
FStructLeaf StructLeaf(FInstancedStruct& V, const char* Path, das::Context* C)
{
    return StructLeafMemory(V.GetScriptStruct(), V.GetMutableMemory(), Path, C);
}
FConstStructLeaf StructLeaf(const FInstancedStruct& V, const char* Path, das::Context* C)
{
    return StructLeafMemory(V.GetScriptStruct(), V.GetMemory(), Path, C);
}
using ESnapshotKind = EDasTypeKind;
template <class Byte> auto SnapshotMap(TStructLeaf<Byte> F, ESnapshotKind Key, ESnapshotKind Value, das::Context* C)
{
    DasSnapshotContainers::TMapView<Byte> View;
    FString Error;
    if (!DasSnapshotContainers::Map(F, Key, Value, View, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    return View;
}
template <class Byte> auto SnapshotSet(TStructLeaf<Byte> F, ESnapshotKind Element, das::Context* C)
{
    DasSnapshotContainers::TSetView<Byte> View;
    FString Error;
    if (!DasSnapshotContainers::Set(F, Element, View, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    return View;
}
template <class Byte> auto SnapshotArray(TStructLeaf<Byte> F, das::Context* C)
{
    DasSnapshotContainers::TArrayView<Byte> View;
    FString Error;
    if (!DasSnapshotContainers::Array(F, View, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    return View;
}
template <class Property> constexpr ESnapshotKind SnapshotValueKind()
{
    if constexpr (std::is_same_v<Property, FIntProperty>)
        return ESnapshotKind::Int;
    else if constexpr (std::is_same_v<Property, FByteProperty>)
        return ESnapshotKind::Byte;
    else if constexpr (std::is_same_v<Property, FTextProperty>)
        return ESnapshotKind::Text;
    else if constexpr (std::is_same_v<Property, FFloatProperty>)
        return ESnapshotKind::Float;
    else if constexpr (std::is_same_v<Property, FDoubleProperty>)
        return ESnapshotKind::Double;
    else if constexpr (std::is_same_v<Property, FBoolProperty>)
        return ESnapshotKind::Bool;
    else if constexpr (std::is_same_v<Property, FStrProperty>)
        return ESnapshotKind::String;
    else if constexpr (std::is_same_v<Property, FNameProperty>)
        return ESnapshotKind::Name;
    else
    {
        static_assert(std::is_same_v<Property, FStructProperty>);
        return ESnapshotKind::Struct;
    }
}
template <class KeyType, class Read>
auto ReadSnapshotMap(const FDasStructValue& H, const char* Path, const KeyType& Key, ESnapshotKind KeyKind,
                     ESnapshotKind ValueKind, das::Context* C, Read Reader)
{
    const auto& V = ResolveStructValue(H, C);
    auto View = SnapshotMap(StructLeaf(V, Path, C), KeyKind, ValueKind, C);
    FConstStructLeaf Value{};
    FString Error;
    if (!View.Find(&Key, Value, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    return Reader(Value.Property, Value.Memory);
}
template <class KeyType, class Write>
FDasStructValue WriteSnapshotMap(const FDasStructValue& H, const char* Path, const KeyType& Key, ESnapshotKind KeyKind,
                                 ESnapshotKind ValueKind, das::Context* C, das::LineInfoArg* At, Write Writer)
{
    FStructMutation Mutation(H, C);
    auto& V = Mutation.Value;
    auto View = SnapshotMap(StructLeaf(V, Path, C), KeyKind, ValueKind, C);
    FString Error;
    if (!View.Upsert(
            &Key,
            [&](FProperty* P, void* Memory, FString&)
            {
                Writer(P, Memory);
                return true;
            },
            Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    return Mutation.Finish(C, At);
}
template <class ValueProperty, class KeyType>
auto StructScalarMapGet(const FDasStructValue& H, const char* Path, const KeyType& Key, ESnapshotKind KeyKind,
                        das::Context* C)
{
    return ReadSnapshotMap(H, Path, Key, KeyKind, SnapshotValueKind<ValueProperty>(), C,
                           [&](FProperty* P, const void* Memory)
                           { return ReadPropertyScalarValue(CastFieldChecked<ValueProperty>(P), Memory, C); });
}
template <class ValueProperty, class KeyType, class ValueType>
FDasStructValue StructScalarMapWith(const FDasStructValue& H, const char* Path, const KeyType& Key,
                                    const ValueType& Value, ESnapshotKind KeyKind, das::Context* C,
                                    das::LineInfoArg* At)
{
    return WriteSnapshotMap(H, Path, Key, KeyKind, SnapshotValueKind<ValueProperty>(), C, At,
                            [&](FProperty* P, void* Memory)
                            { WriteNativePropertyValue(P, Memory, PropertyScalarValue<ValueProperty>(Value), C); });
}
template <class View>
FConstStructLeaf SnapshotKeyAt(const FDasStructValue& H, const char* Path, const View& Values, int32 Index,
                               das::Context* C)
{
    if (Index < 0 || Index >= Values.Num())
        C->throw_error(View::IsMap ? "map key index out of range" : "set element index out of range");
    // Draft tokens retain identity while their contents change. Immutable key-order caches cannot apply.
    TArray<int32> DraftOrder;
    if (ActiveContext->StructEdits.Contains(H.Token))
        Values.PrepareKeyOrder(DraftOrder);
    const auto& Order =
        ActiveContext->StructEdits.Contains(H.Token)
            ? DraftOrder
            : ActiveContext->SnapshotKeyOrders.Get(H.Token, Path ? UTF8_TO_TCHAR(Path) : TEXT(""), Values);
    FConstStructLeaf Field{};
    FString Error;
    if (!Values.KeyAtSlot(Order[Index], Field, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    return Field;
}
FDasStructValue SetSnapshotValue(const FDasStructValue& H, const char* Path, const void* Key, ESnapshotKind Kind,
                                 bool Present, das::Context* C, das::LineInfoArg* At)
{
    FStructMutation Mutation(H, C);
    auto& V = Mutation.Value;
    auto View = SnapshotSet(StructLeaf(V, Path, C), Kind, C);
    FString Error;
    if (!View.Set(Key, Present, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    return Mutation.Finish(C, At);
}
FDasScalarValue SnapshotObjectValue(FProperty* P, UObject* Object, das::Context* C)
{
    auto* Type = DasReferencePropertyClass(P);
    if (!Type)
        C->throw_error("expected snapshot object/interface");
    FDasScalarValue Value;
    Value.Kind = int32(EDasMethodSignature::Object);
    Value.ObjectClass = Type->GetPathName();
    Value.ObjectValue = Object;
    return Value;
}


int32 StructSetSize(const FDasStructValue& H, const char* Path, das::Context* C)
{
    return SnapshotSet(StructLeaf(ResolveStructValue(H, C), Path, C), ESnapshotKind::Invalid, C).Num();
}
int32 StructSetElementInt(const FDasStructValue& H, const char* Path, int32 Index, das::Context* C)
{
    auto F = SnapshotKeyAt(H, Path, SnapshotSet(StructLeaf(ResolveStructValue(H, C), Path, C), ESnapshotKind::Int, C),
                           Index, C);
    return ReadPropertyScalarValue(CastFieldChecked<FIntProperty>(F.Property), F.Memory, C);
}
char* StructSetElementString(const FDasStructValue& H, const char* Path, int32 Index, das::Context* C)
{
    auto F = SnapshotKeyAt(
        H, Path, SnapshotSet(StructLeaf(ResolveStructValue(H, C), Path, C), ESnapshotKind::String, C), Index, C);
    return CopyDSLString(ReadPropertyScalarValue(CastFieldChecked<FStrProperty>(F.Property), F.Memory, C), C);
}
bool StructSetContainsInt(const FDasStructValue& H, const char* Path, int32 Element, das::Context* C)
{
    return SnapshotSet(StructLeaf(ResolveStructValue(H, C), Path, C), ESnapshotKind::Int, C).Contains(&Element);
}
bool StructSetContainsString(const FDasStructValue& H, const char* Path, const char* Element, das::Context* C)
{
    FString Key = Element ? UTF8_TO_TCHAR(Element) : TEXT("");
    return SnapshotSet(StructLeaf(ResolveStructValue(H, C), Path, C), ESnapshotKind::String, C).Contains(&Key);
}
FDasStructValue StructSetWithInt(const FDasStructValue& H, const char* Path, int32 Element, bool Present,
                                 das::Context* C, das::LineInfoArg* At)
{
    return SetSnapshotValue(H, Path, &Element, ESnapshotKind::Int, Present, C, At);
}
FDasStructValue StructSetWithString(const FDasStructValue& H, const char* Path, const char* Element, bool Present,
                                    das::Context* C, das::LineInfoArg* At)
{
    FString Key = Element ? UTF8_TO_TCHAR(Element) : TEXT("");
    return SetSnapshotValue(H, Path, &Key, ESnapshotKind::String, Present, C, At);
}
int32 StructMapSize(const FDasStructValue& H, const char* Path, das::Context* C)
{
    return SnapshotMap(StructLeaf(ResolveStructValue(H, C), Path, C), ESnapshotKind::Invalid, ESnapshotKind::Invalid, C)
        .Num();
}
int32 StructMapKey(const FDasStructValue& H, const char* Path, int32 Index, das::Context* C)
{
    auto F = SnapshotKeyAt(
        H, Path,
        SnapshotMap(StructLeaf(ResolveStructValue(H, C), Path, C), ESnapshotKind::Int, ESnapshotKind::Invalid, C),
        Index, C);
    return ReadPropertyScalarValue(CastFieldChecked<FIntProperty>(F.Property), F.Memory, C);
}

char* StructMapStringKey(const FDasStructValue& H, const char* Path, int32 Index, das::Context* C)
{
    auto F = SnapshotKeyAt(
        H, Path,
        SnapshotMap(StructLeaf(ResolveStructValue(H, C), Path, C), ESnapshotKind::String, ESnapshotKind::Invalid, C),
        Index, C);
    return CopyDSLString(ReadPropertyScalarValue(CastFieldChecked<FStrProperty>(F.Property), F.Memory, C), C);
}
int32 StructArraySize(const FDasStructValue& H, const char* Path, das::Context* C)
{
    return SnapshotArray(StructLeaf(ResolveStructValue(H, C), Path, C), C).Num();
}
FDasStructValue StructArrayResize(const FDasStructValue& H, const char* Path, int32 N, das::Context* C,
                                  das::LineInfoArg* At)
{
    FStructMutation Mutation(H, C);
    auto& V = Mutation.Value;
    auto View = SnapshotArray(StructLeaf(V, Path, C), C);
    FString Error;
    if (!View.Resize(N, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    return Mutation.Finish(C, At);
}


template <typename KeyType>
FDasStructValue StructMapGetStruct(const FDasStructValue& H, const char* Path, const KeyType& Key,
                                   ESnapshotKind KeyKind, das::Context* C, das::LineInfoArg* At)
{
    return ReadSnapshotMap(H, Path, Key, KeyKind, ESnapshotKind::Struct, C,
                           [&](FProperty* P, const void* Memory) { return ReadPropertySnapshot(P, Memory, C, At); });
}
template <typename KeyType>
FDasStructValue StructMapWithStruct(const FDasStructValue& H, const char* Path, const KeyType& Key,
                                    const FDasStructValue& Value, ESnapshotKind KeyKind, das::Context* C,
                                    das::LineInfoArg* At)
{
    return WriteSnapshotMap(H, Path, Key, KeyKind, ESnapshotKind::Struct, C, At,
                            [&](FProperty* P, void* Memory) { WritePropertySnapshot(P, Memory, Value, C); });
}
FDasStructValue StructMapGetIntStruct(const FDasStructValue& H, const char* Path, int32 Key, das::Context* C,
                                      das::LineInfoArg* At)
{
    return StructMapGetStruct(H, Path, Key, ESnapshotKind::Int, C, At);
}
FDasStructValue StructMapWithIntStruct(const FDasStructValue& H, const char* Path, int32 Key, const FDasStructValue& V,
                                       das::Context* C, das::LineInfoArg* At)
{
    return StructMapWithStruct(H, Path, Key, V, ESnapshotKind::Int, C, At);
}
FDasStructValue StructMapGetStringStruct(const FDasStructValue& H, const char* Path, const char* Key, das::Context* C,
                                         das::LineInfoArg* At)
{
    return StructMapGetStruct(H, Path, FString(Key ? UTF8_TO_TCHAR(Key) : TEXT("")), ESnapshotKind::String, C, At);
}
FDasStructValue StructMapWithStringStruct(const FDasStructValue& H, const char* Path, const char* Key,
                                          const FDasStructValue& V, das::Context* C, das::LineInfoArg* At)
{
    return StructMapWithStruct(H, Path, FString(Key ? UTF8_TO_TCHAR(Key) : TEXT("")), V, ESnapshotKind::String, C, At);
}

template <class Snapshot> auto StructNestedLeaf(Snapshot& V, const char* Path, int32 Index, das::Context* C)
{
    auto F = StructLeaf(V, Path, C);
    if (Index == -1)
        return F;
    auto View = SnapshotArray(F, C);
    FString Error;
    if (!View.ElementAt(Index, F, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    if (!F.Property->template IsA<FStructProperty>() && !F.Property->template IsA<FSoftObjectProperty>())
        C->throw_error("expected snapshot value");
    return F;
}
FDasStructValue StructNestedGet(const FDasStructValue& H, const char* Path, int32 Index, das::Context* C,
                                das::LineInfoArg* At)
{
    const auto& V = ResolveStructValue(H, C);
    auto F = StructNestedLeaf(V, Path, Index, C);
    return ReadPropertySnapshot(F.Property, F.Memory, C, At);
}
FDasStructValue StructNestedWith(const FDasStructValue& H, const char* Path, int32 Index, const FDasStructValue& Value,
                                 das::Context* C, das::LineInfoArg* At)
{
    FStructMutation Mutation(H, C);
    auto& V = Mutation.Value;
    auto F = StructNestedLeaf(V, Path, Index, C);
    WritePropertySnapshot(F.Property, F.Memory, Value, C);
    return Mutation.Finish(C, At);
}
FDasObjectHandle MakeObjectHandle(UObject*, UClass*, das::Context*);
FDasObjectHandle ReadNullablePropertyObject(FProperty*, const void*, das::Context*);
uint64 InternObjectKey(UObject*, das::Context*);
UObject* ResolveObjectKey(uint64, das::Context*);
UObject* ResolveObject(const FDasObjectHandle&, das::Context*);
FDasClassHandle StructGetClass(const FDasStructValue& H, const char* P, das::Context* C)
{
    const auto& V = ResolveStructValue(H, C);
    auto F = StructLeaf(V, P, C);
    auto* T = CastField<FClassProperty>(F.Property);
    if (!T)
        C->throw_error("struct field class type mismatch");
    const auto Value = ReadNativePropertyValue(T, F.Memory, C);
    return {MakeObjectHandle(Value.ClassValue.Get(), UClass::StaticClass(), C).Token};
}
FDasObjectHandle StructGetObject(const FDasStructValue& H, const char* P, das::Context* C)
{
    const auto& V = ResolveStructValue(H, C);
    auto F = StructLeaf(V, P, C);
    auto* T = DasReferencePropertyClass(F.Property);
    if (!T)
        C->throw_error("struct field object type mismatch");
    return ReadNullablePropertyObject(F.Property, F.Memory, C);
}
FDasStructValue StructWithObject(const FDasStructValue& H, const char* P, const FDasObjectHandle& X, das::Context* C,
                                 das::LineInfoArg* At)
{
    FStructMutation Mutation(H, C);
    auto& V = Mutation.Value;
    auto F = StructLeaf(V, P, C);
    auto* T = DasReferencePropertyClass(F.Property);
    if (!T)
        C->throw_error("struct field object type mismatch");
    UObject* Object = X.Token ? ResolveObject(X, C) : nullptr;
    if (Object && !DasReferenceMatches(Object, T))
        C->throw_error("struct object field has an incompatible class");
    WriteNativePropertyValue(F.Property, F.Memory, SnapshotObjectValue(F.Property, Object, C), C);
    return Mutation.Finish(C, At);
}
char* StructGetEnum(const FDasStructValue& H, const char* Path, const char* EnumPath, das::Context* C)
{
    const auto& V = ResolveStructValue(H, C);
    auto F = StructLeaf(V, Path, C);
    UEnum* E = RequireCallEnum(F.Property, EnumPath, C);
    int32 Index = E->GetIndexByValue(ReadNativeEnumNumber(F.Property, F.Memory, C));
    if (!IsCallEnumMember(E, Index))
        C->throw_error("unknown or reserved enum value in struct");
    return CopyDSLString(E->GetNameStringByIndex(Index), C);
}
FDasStructValue StructWithEnum(const FDasStructValue& H, const char* Path, const char* EnumPath, const char* Member,
                               das::Context* C, das::LineInfoArg* At)
{
    FStructMutation Mutation(H, C);
    auto& V = Mutation.Value;
    auto F = StructLeaf(V, Path, C);
    UEnum* E = RequireCallEnum(F.Property, EnumPath, C);
    const FString Symbol = Member ? UTF8_TO_TCHAR(Member) : TEXT("");
    if (Symbol.Len() >= NAME_SIZE)
        C->throw_error("enum member is too long");
    int32 Index = INDEX_NONE;
    for (int32 I = 0; I < E->NumEnums(); ++I)
        if (Symbol.Equals(E->GetNameStringByIndex(I), ESearchCase::IgnoreCase) ||
            Symbol.Equals(E->GetNameByIndex(I).ToString(), ESearchCase::IgnoreCase))
        {
            Index = I;
            break;
        }
    if (!IsCallEnumMember(E, Index))
        C->throw_error("unknown or reserved enum member");
    WriteNativeEnumNumber(F.Property, F.Memory, E, E->GetValueByIndex(Index), C);
    return Mutation.Finish(C, At);
}
FDasEnumValue StructEnumGetValue(const FDasStructValue& H, const char* Path, const char* EnumPath, das::Context* C);
FDasStructValue StructEnumWithValue(const FDasStructValue& H, const char* Path, FDasEnumValue Value, das::Context* C,
                                    das::LineInfoArg* At);
FDasEnumValue MakeEnumValue(const FString& Path, int32 V, das::Context* C)
{
    if (!ActiveContext || ActiveContext != C)
        C->throw_error("Enum values require active context");
    FString Error;
    auto* E = ResolveDasEnum(Path, Error);
    if (!E || !ValidateDasEnumNumber(Path, V, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    for (const auto& P : ActiveContext->EnumValues)
        if (P.Value.Type.Get() == E && P.Value.Value == V)
            return {P.Key};
    if (ActiveContext->EnumValues.Num() >= 4096)
        C->throw_error("Enum value limit reached");
    static uint64 Next = 1;
    const uint64 Token = Next++;
    ActiveContext->EnumValues.Add(Token, {E, V});
    return {Token};
}
const FUnrealDasContext::FEnumValue& ResolveEnumValue(FDasEnumValue V, das::Context* C)
{
    if (!ActiveContext || ActiveContext != C)
        C->throw_error("Enum values require active context");
    auto* Value = ActiveContext->EnumValues.Find(V.Token);
    FString Error;
    if (!Value || !Value->Type.IsValid() || !ValidateDasEnumNumber(Value->Type->GetPathName(), Value->Value, Error))
        C->throw_error("Invalid or foreign enum value");
    return *Value;
}
FDasEnumValue EnumValue(const char* Path, const char* Member, das::Context* C)
{
    FString Error;
    auto* E = ResolveDasEnum(Path ? UTF8_TO_TCHAR(Path) : TEXT(""), Error);
    if (!E)
        C->throw_error(TCHAR_TO_UTF8(*Error));
    return MakeEnumValue(E->GetPathName(), int32(E->GetValueByNameString(Member ? UTF8_TO_TCHAR(Member) : TEXT(""))),
                                                 C);
}
FDasEnumValue EnumFromNumber(const char* Path, int32 V, das::Context* C)
{
    return MakeEnumValue(Path ? UTF8_TO_TCHAR(Path) : TEXT(""), V, C);
}
FDasEnumValue StructEnumGetValue(const FDasStructValue& H, const char* Path, const char* EnumPath, das::Context* C)
{
    const auto& V = ResolveStructValue(H, C);
    auto F = StructLeaf(V, Path, C);
    auto* E = RequireCallEnum(F.Property, EnumPath, C);
    return MakeEnumValue(E->GetPathName(), ReadNativeEnumNumber(F.Property, F.Memory, C), C);
}
FDasStructValue StructEnumWithValue(const FDasStructValue& H, const char* Path, FDasEnumValue Value, das::Context* C,
                                    das::LineInfoArg* At)
{
    FStructMutation Mutation(H, C);
    auto& V = Mutation.Value;
    auto F = StructLeaf(V, Path, C);
    const auto& E = ResolveEnumValue(Value, C);
    WriteNativeEnumNumber(F.Property, F.Memory, E.Type.Get(), E.Value, C);
    return Mutation.Finish(C, At);
}


template <typename KeyType>
FDasEnumValue StructMapGetEnum(const FDasStructValue& H, const char* Path, const KeyType& Key, ESnapshotKind KeyKind,
                               das::Context* C)
{
    return ReadSnapshotMap(H, Path, Key, KeyKind, ESnapshotKind::Enum, C,
                           [&](FProperty* P, const void* Memory)
                           {
                               auto* E = DasEnumPropertyType(P);
                               return MakeEnumValue(E->GetPathName(), ReadNativeEnumNumber(P, Memory, C), C);
                           });
}
template <typename KeyType>
FDasStructValue StructMapWithEnum(const FDasStructValue& H, const char* Path, const KeyType& Key, FDasEnumValue Value,
                                  ESnapshotKind KeyKind, das::Context* C, das::LineInfoArg* At)
{
    const auto& E = ResolveEnumValue(Value, C);
    return WriteSnapshotMap(H, Path, Key, KeyKind, ESnapshotKind::Enum, C, At, [&](FProperty* P, void* Memory)
                            { WriteNativeEnumNumber(P, Memory, E.Type.Get(), E.Value, C); });
}
template <typename KeyType>
FDasObjectHandle StructMapGetObject(const FDasStructValue& H, const char* Path, const KeyType& Key,
                                    ESnapshotKind KeyKind, das::Context* C)
{
    return ReadSnapshotMap(H, Path, Key, KeyKind, ESnapshotKind::Object, C,
                           [&](FProperty* P, const void* Memory) { return ReadNullablePropertyObject(P, Memory, C); });
}
template <typename KeyType>
FDasStructValue StructMapWithObject(const FDasStructValue& H, const char* Path, const KeyType& Key,
                                    const FDasObjectHandle& Value, ESnapshotKind KeyKind, das::Context* C,
                                    das::LineInfoArg* At)
{
    UObject* Object = Value.Token ? ResolveObject(Value, C) : nullptr;
    return WriteSnapshotMap(H, Path, Key, KeyKind, ESnapshotKind::Object, C, At, [&](FProperty* P, void* Memory)
                            { WriteNativePropertyValue(P, Memory, SnapshotObjectValue(P, Object, C), C); });
}
FDasEnumValue StructMapGetIntEnum(const FDasStructValue& H, const char* Path, int32 Key, das::Context* C)
{
    return StructMapGetEnum(H, Path, Key, ESnapshotKind::Int, C);
}
FDasStructValue StructMapWithIntEnum(const FDasStructValue& H, const char* Path, int32 Key, FDasEnumValue V,
                                     das::Context* C, das::LineInfoArg* At)
{
    return StructMapWithEnum(H, Path, Key, V, ESnapshotKind::Int, C, At);
}
FDasObjectHandle StructMapGetIntObject(const FDasStructValue& H, const char* Path, int32 Key, das::Context* C)
{
    return StructMapGetObject(H, Path, Key, ESnapshotKind::Int, C);
}
FDasStructValue StructMapWithIntObject(const FDasStructValue& H, const char* Path, int32 Key, const FDasObjectHandle& V,
                                       das::Context* C, das::LineInfoArg* At)
{
    return StructMapWithObject(H, Path, Key, V, ESnapshotKind::Int, C, At);
}
FDasEnumValue StructMapGetStringEnum(const FDasStructValue& H, const char* Path, const char* Key, das::Context* C)
{
    return StructMapGetEnum(H, Path, FString(Key ? UTF8_TO_TCHAR(Key) : TEXT("")), ESnapshotKind::String, C);
}
FDasStructValue StructMapWithStringEnum(const FDasStructValue& H, const char* Path, const char* Key, FDasEnumValue V,
                                        das::Context* C, das::LineInfoArg* At)
{
    return StructMapWithEnum(H, Path, FString(Key ? UTF8_TO_TCHAR(Key) : TEXT("")), V, ESnapshotKind::String, C, At);
}
FDasObjectHandle StructMapGetStringObject(const FDasStructValue& H, const char* Path, const char* Key, das::Context* C)
{
    return StructMapGetObject(H, Path, FString(Key ? UTF8_TO_TCHAR(Key) : TEXT("")), ESnapshotKind::String, C);
}
FDasStructValue StructMapWithStringObject(const FDasStructValue& H, const char* Path, const char* Key,
                                          const FDasObjectHandle& V, das::Context* C, das::LineInfoArg* At)
{
    return StructMapWithObject(H, Path, FString(Key ? UTF8_TO_TCHAR(Key) : TEXT("")), V, ESnapshotKind::String, C, At);
}

int32 EnumNumber(FDasEnumValue V, das::Context* C)
{
    return ResolveEnumValue(V, C).Value;
}
int32 CheckedEnumNumber(FDasEnumValue V, const char* Path, das::Context* C)
{
    const auto& Value = ResolveEnumValue(V, C);
    if (!Path || Value.Type->GetPathName() != UTF8_TO_TCHAR(Path))
        C->throw_error("Source enum snapshot identity mismatch");
    return Value.Value;
}
void RequireStructType(const FDasStructValue& V, const char* Path, das::Context* C)
{
    const auto& Value = ResolveStructValue(V, C);
    if (!Path || Value.GetScriptStruct()->GetPathName() != UTF8_TO_TCHAR(Path))
        C->throw_error("Source struct snapshot identity mismatch");
}
FDasEnumValue ReadPropertyEnum(FProperty* P, UObject* O, das::Context* C)
{
    const auto V = ReadOwnerProperty(P, O, C);
    if (V.Kind != int32(EDasMethodSignature::Enum))
        C->throw_error("Expected enum property");
    return MakeEnumValue(V.ObjectClass, V.IntValue, C);
}
void WritePropertyEnum(FProperty* P, UObject* O, FDasEnumValue Input, das::Context* C)
{
    const auto& E = ResolveEnumValue(Input, C);
    FDasScalarValue V;
    V.Kind = int32(EDasMethodSignature::Enum);
    V.ObjectClass = E.Type->GetPathName();
    V.IntValue = E.Value;
    WriteOwnerProperty(P, O, V, C);
}
FDasEnumValue GetSelfEnum(const char* Name, das::Context* C)
{
    auto* Self = ResolveSelf(C);
    auto* P = ResolveProperty<FProperty>(Self, Name, false, C);
    return ReadPropertyEnum(P, Self, C);
}
void SetSelfEnum(const char* Name, FDasEnumValue V, das::Context* C)
{
    auto* Self = ResolveSelf(C);
    if (ActiveContext->bCollectingSubscriptions)
        C->throw_error("self writes are forbidden in bind_events");
    auto* P = ResolveProperty<FProperty>(Self, Name, true, C);
    WritePropertyEnum(P, Self, V, C);
}
bool FrameClassMatches(UClass* Value, const FString& BasePath)
{
    auto* Base = FindObject<UClass>(nullptr, *BasePath);
    return BasePath.StartsWith(TEXT("/Script/")) && IsValid(Base) &&
                               !Base->HasAnyClassFlags(CLASS_Interface | CLASS_Deprecated | CLASS_NewerVersionExists) &&
                               (!Value || (IsValid(Value) && Value->IsChildOf(Base) &&
                                           !Value->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists)));
}
void PublishFrameValue(int32 Index, FDasScalarValue Value, das::Context* C)
{
    if (C != ActiveContext || !ActiveOutputFrame || !ActiveOutputFrame->Arguments ||
        !ActiveOutputFrame->Arguments->IsValidIndex(Index))
        C->throw_error("output requires active declared frame slot");
    const auto& Expected = (*ActiveOutputFrame->Arguments)[Index];
    if (Expected.Direction == 0 || Value.Kind != Expected.Kind || ActiveOutputFrame->Values.Contains(Index))
        C->throw_error("output slot direction/type/duplicate mismatch");
    if (Value.Kind == 13)
    {
        FString Error;
        if (Value.ObjectClass != Expected.ObjectClass ||
            !ValidateDasEnumNumber(Expected.ObjectClass, Value.IntValue, Error))
            C->throw_error("output enum identity/value mismatch");
    }
    if (Value.Kind == 15)
    {
        if (!FrameClassMatches(Value.ClassValue.Get(), Expected.ObjectClass))
            C->throw_error("output class base mismatch");
        if (Value.ClassValue)
            ActiveOutputFrame->ObjectPins.Emplace(Value.ClassValue.Get());
    }
    if (Value.Kind == 8)
    {
        auto* Object = Value.ObjectValue.Get();
        auto* Class = FindObject<UClass>(nullptr, *Expected.ObjectClass);
        if (!Class || !DasReferenceMatches(Object, Class) ||
            (Object &&
             (!IsValid(Object) || (Cast<AActor>(Object) && CastChecked<AActor>(Object)->IsActorBeingDestroyed()))))
            C->throw_error("output object/interface identity mismatch");
        if (Object)
            ActiveOutputFrame->ObjectPins.Emplace(Object);
    }
    if ((Value.Kind == 9 && Value.VectorValue.ContainsNaN()) ||
        (Value.Kind == 10 && Value.RotatorValue.ContainsNaN()) ||
        (Value.Kind == 11 && (Value.TransformValue.ContainsNaN() || !Value.TransformValue.IsRotationNormalized())))
        C->throw_error("invalid math frame output");
    Value.Direction = Expected.Direction;
    Value.ObjectClass = Expected.ObjectClass;
    if (Value.Kind == 12)
    {
        FString Error;
        if (!Value.StructValue.IsValid() ||
            Value.StructValue.GetScriptStruct()->GetPathName() != Expected.ObjectClass ||
            !ValidateDasStructSnapshot(Value.StructValue, Error))
            C->throw_error("output snapshot type/limit mismatch");
    }
    if ((Value.Kind == 1 && !FMath::IsFinite(Value.FloatValue)) ||
        (Value.Kind == 6 && !FMath::IsFinite(Value.DoubleValue)))
        C->throw_error("non-finite frame output");
    ActiveOutputFrame->Values.Add(Index, MoveTemp(Value));
}
void PublishFrameInt(int32 I, int32 X, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 0;
    V.IntValue = X;
    PublishFrameValue(I, MoveTemp(V), C);
}
void PublishFrameFloat(int32 I, float X, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 1;
    V.FloatValue = X;
    PublishFrameValue(I, MoveTemp(V), C);
}
void PublishFrameBool(int32 I, bool X, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 2;
    V.BoolValue = X;
    PublishFrameValue(I, MoveTemp(V), C);
}
void PublishFrameString(int32 I, const char* X, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 5;
    V.StringValue = X ? UTF8_TO_TCHAR(X) : TEXT("");
    PublishFrameValue(I, MoveTemp(V), C);
}
void PublishFrameDouble(int32 I, double X, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 6;
    V.DoubleValue = X;
    PublishFrameValue(I, MoveTemp(V), C);
}
// Name table keys are canonical context-owned tokens; never stringify FName keys.
FDasEnumValue StructSetElementEnum(const FDasStructValue& H, const char* Path, int32 Index, das::Context* C)
{
    auto F = SnapshotKeyAt(H, Path, SnapshotSet(StructLeaf(ResolveStructValue(H, C), Path, C), ESnapshotKind::Enum, C),
                           Index, C);
    auto* E = DasEnumPropertyType(F.Property);
    return MakeEnumValue(E->GetPathName(), ReadNativeEnumNumber(F.Property, F.Memory, C), C);
}
FDasStructValue StructSetWithEnum(const FDasStructValue& H, const char* Path, FDasEnumValue Key, bool Present,
                                  das::Context* C, das::LineInfoArg* At)
{
    const auto& E = ResolveEnumValue(Key, C);
    const auto& Source = ResolveStructValue(H, C);
    auto View = SnapshotSet(StructLeaf(Source, Path, C), ESnapshotKind::Enum, C);
    if (DasEnumPropertyType(View.Property->ElementProp) != E.Type.Get())
        C->throw_error("snapshot set enum identity/storage mismatch");
    uint8 Value = 0;
    WriteNativeEnumNumber(View.Property->ElementProp, &Value, E.Type.Get(), E.Value, C);
    return SetSnapshotValue(H, Path, &Value, ESnapshotKind::Enum, Present, C, At);
}
FDasNameValue StructSetElementName(const FDasStructValue& H, const char* Path, int32 Index, das::Context* C)
{
    auto F = SnapshotKeyAt(H, Path, SnapshotSet(StructLeaf(ResolveStructValue(H, C), Path, C), ESnapshotKind::Name, C),
                           Index, C);
    return MakeDSLName(ReadPropertyScalarValue(CastFieldChecked<FNameProperty>(F.Property), F.Memory, C), C);
}
FDasStructValue StructSetWithName(const FDasStructValue& H, const char* Path, FDasNameValue Key, bool Present,
                                  das::Context* C, das::LineInfoArg* At)
{
    const FName Name = ResolveDSLName(Key, C);
    return SetSnapshotValue(H, Path, &Name, ESnapshotKind::Name, Present, C, At);
}
FDasNameValue StructMapNameKey(const FDasStructValue& H, const char* Path, int32 Index, das::Context* C)
{
    auto F = SnapshotKeyAt(
        H, Path,
        SnapshotMap(StructLeaf(ResolveStructValue(H, C), Path, C), ESnapshotKind::Name, ESnapshotKind::Invalid, C),
        Index, C);
    return MakeDSLName(ReadPropertyScalarValue(CastFieldChecked<FNameProperty>(F.Property), F.Memory, C), C);
}
FDasStructValue StructMapGetNameStruct(const FDasStructValue& H, const char* Path, FDasNameValue Key, das::Context* C,
                                       das::LineInfoArg* At)
{
    return StructMapGetStruct(H, Path, ResolveDSLName(Key, C), ESnapshotKind::Name, C, At);
}
FDasStructValue StructMapWithNameStruct(const FDasStructValue& H, const char* Path, FDasNameValue Key,
                                        const FDasStructValue& Value, das::Context* C, das::LineInfoArg* At)
{
    return StructMapWithStruct(H, Path, ResolveDSLName(Key, C), Value, ESnapshotKind::Name, C, At);
}
FDasEnumValue StructMapGetNameEnum(const FDasStructValue& H, const char* Path, FDasNameValue Key, das::Context* C)
{
    return StructMapGetEnum(H, Path, ResolveDSLName(Key, C), ESnapshotKind::Name, C);
}
FDasStructValue StructMapWithNameEnum(const FDasStructValue& H, const char* Path, FDasNameValue Key,
                                      FDasEnumValue Value, das::Context* C, das::LineInfoArg* At)
{
    return StructMapWithEnum(H, Path, ResolveDSLName(Key, C), Value, ESnapshotKind::Name, C, At);
}
FDasObjectHandle StructMapGetNameObject(const FDasStructValue& H, const char* Path, FDasNameValue Key, das::Context* C)
{
    return StructMapGetObject(H, Path, ResolveDSLName(Key, C), ESnapshotKind::Name, C);
}
FDasStructValue StructMapWithNameObject(const FDasStructValue& H, const char* Path, FDasNameValue Key,
                                        const FDasObjectHandle& Value, das::Context* C, das::LineInfoArg* At)
{
    return StructMapWithObject(H, Path, ResolveDSLName(Key, C), Value, ESnapshotKind::Name, C, At);
}
