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
