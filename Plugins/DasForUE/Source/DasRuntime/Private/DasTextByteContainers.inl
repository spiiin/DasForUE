// Typed container leaves reuse owned snapshots and the existing size limits.
uint8 StructMapKeyByte(const FDasStructValue& H, const char* Path, int32 Index, das::Context* C)
{
    auto F = SnapshotKeyAt(
        H, Path,
        SnapshotMap(StructLeaf(ResolveStructValue(H, C), Path, C), ESnapshotKind::Byte, ESnapshotKind::Invalid, C),
        Index, C);
    return ReadPropertyScalarValue(CastFieldChecked<FByteProperty>(F.Property), F.Memory, C);
}
uint8 StructSetElementByte(const FDasStructValue& H, const char* Path, int32 Index, das::Context* C)
{
    auto F = SnapshotKeyAt(H, Path, SnapshotSet(StructLeaf(ResolveStructValue(H, C), Path, C), ESnapshotKind::Byte, C),
                           Index, C);
    return ReadPropertyScalarValue(CastFieldChecked<FByteProperty>(F.Property), F.Memory, C);
}
FDasStructValue StructSetWithByte(const FDasStructValue& H, const char* Path, uint8 Key, bool Present, das::Context* C,
                                  das::LineInfoArg* At)
{
    return SetSnapshotValue(H, Path, &Key, ESnapshotKind::Byte, Present, C, At);
}
uint8 GetTextByteMapObjectByte(const FDasStructValue& H, const char* Path, uint64 Key, das::Context* C)
{
    return StructScalarMapGet<FByteProperty>(H, Path, ObjectMapKey(H, Path, Key, C), ESnapshotKind::Object, C);
}
FDasStructValue WithTextByteMapObjectByte(const FDasStructValue& H, const char* Path, uint64 Key, uint8 Value,
                                          das::Context* C, das::LineInfoArg* At)
{
    return StructScalarMapWith<FByteProperty>(H, Path, ObjectMapKey(H, Path, Key, C), Value, ESnapshotKind::Object, C,
                                              At);
}
FDasTextValue GetTextByteMapObjectText(const FDasStructValue& H, const char* Path, uint64 Key, das::Context* C)
{
    return MakeDSLText(
        StructScalarMapGet<FTextProperty>(H, Path, ObjectMapKey(H, Path, Key, C), ESnapshotKind::Object, C), C);
}
FDasStructValue WithTextByteMapObjectText(const FDasStructValue& H, const char* Path, uint64 Key, FDasTextValue Value,
                                          das::Context* C, das::LineInfoArg* At)
{
    return StructScalarMapWith<FTextProperty>(H, Path, ObjectMapKey(H, Path, Key, C), ResolveDSLText(Value, C),
                                              ESnapshotKind::Object, C, At);
}
uint8 GetTextByteMapEnumByte(const FDasStructValue& H, const char* Path, FDasEnumValue Key, das::Context* C)
{
    return StructScalarMapGet<FByteProperty>(H, Path, EnumMapKey(H, Path, Key, C), ESnapshotKind::Enum, C);
}
FDasStructValue WithTextByteMapEnumByte(const FDasStructValue& H, const char* Path, FDasEnumValue Key, uint8 Value,
                                        das::Context* C, das::LineInfoArg* At)
{
    return StructScalarMapWith<FByteProperty>(H, Path, EnumMapKey(H, Path, Key, C), Value, ESnapshotKind::Enum, C, At);
}
FDasTextValue GetTextByteMapEnumText(const FDasStructValue& H, const char* Path, FDasEnumValue Key, das::Context* C)
{
    return MakeDSLText(StructScalarMapGet<FTextProperty>(H, Path, EnumMapKey(H, Path, Key, C), ESnapshotKind::Enum, C),
                       C);
}
FDasStructValue WithTextByteMapEnumText(const FDasStructValue& H, const char* Path, FDasEnumValue Key,
                                        FDasTextValue Value, das::Context* C, das::LineInfoArg* At)
{
    return StructScalarMapWith<FTextProperty>(H, Path, EnumMapKey(H, Path, Key, C), ResolveDSLText(Value, C),
                                              ESnapshotKind::Enum, C, At);
}
FDasStructValue GetTextByteMapByteStruct(const FDasStructValue& H, const char* Path, uint8 Key, das::Context* C,
                                         das::LineInfoArg* At)
{
    return StructMapGetStruct(H, Path, Key, ESnapshotKind::Byte, C, At);
}
FDasStructValue WithTextByteMapByteStruct(const FDasStructValue& H, const char* Path, uint8 Key,
                                          const FDasStructValue& Value, das::Context* C, das::LineInfoArg* At)
{
    return StructMapWithStruct(H, Path, Key, Value, ESnapshotKind::Byte, C, At);
}
FDasEnumValue GetTextByteMapByteEnum(const FDasStructValue& H, const char* Path, uint8 Key, das::Context* C)
{
    return StructMapGetEnum(H, Path, Key, ESnapshotKind::Byte, C);
}
FDasStructValue WithTextByteMapByteEnum(const FDasStructValue& H, const char* Path, uint8 Key, FDasEnumValue Value,
                                        das::Context* C, das::LineInfoArg* At)
{
    return StructMapWithEnum(H, Path, Key, Value, ESnapshotKind::Byte, C, At);
}
FDasObjectHandle GetTextByteMapByteObject(const FDasStructValue& H, const char* Path, uint8 Key, das::Context* C)
{
    return StructMapGetObject(H, Path, Key, ESnapshotKind::Byte, C);
}
FDasStructValue WithTextByteMapByteObject(const FDasStructValue& H, const char* Path, uint8 Key,
                                          const FDasObjectHandle& Value, das::Context* C, das::LineInfoArg* At)
{
    return StructMapWithObject(H, Path, Key, Value, ESnapshotKind::Byte, C, At);
}
