
uint8 StructGetbyte(const FDasStructValue& H, const char* Path, das::Context* C)
{
    const auto& V = ResolveStructValue(H, C);
    auto F = StructLeaf(V, Path, C);
    auto* P = CastField<FByteProperty>(F.Property);
    if (!P || P->Enum)
        C->throw_error("expected raw byte struct leaf");
    return ReadPropertyScalarValue(P, F.Memory, C);
}
FDasStructValue StructWithbyte(const FDasStructValue& H, const char* Path, uint8 X, das::Context* C,
                               das::LineInfoArg* At)
{
    FStructMutation Mutation(H, C);
    auto& V = Mutation.Value;
    auto F = StructLeaf(V, Path, C);
    auto* P = CastField<FByteProperty>(F.Property);
    if (!P || P->Enum)
        C->throw_error("expected raw byte struct leaf");
    WritePropertyScalarValue(P, F.Memory, X, C);
    return Mutation.Finish(C, At);
}
FDasTextValue StructGettext(const FDasStructValue& H, const char* Path, das::Context* C)
{
    const auto& V = ResolveStructValue(H, C);
    auto F = StructLeaf(V, Path, C);
    auto* P = CastField<FTextProperty>(F.Property);
    if (!P)
        C->throw_error("expected text struct leaf");
    return MakeDSLText(ReadPropertyScalarValue(P, F.Memory, C), C);
}
FDasStructValue StructWithtext(const FDasStructValue& H, const char* Path, FDasTextValue X, das::Context* C,
                               das::LineInfoArg* At)
{
    FStructMutation Mutation(H, C);
    auto& V = Mutation.Value;
    auto F = StructLeaf(V, Path, C);
    auto* P = CastField<FTextProperty>(F.Property);
    if (!P)
        C->throw_error("expected text struct leaf");
    WritePropertyScalarValue(P, F.Memory, ResolveDSLText(X, C), C);
    return Mutation.Finish(C, At);
}
void PublishFrameByte(int32 I, uint8 X, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 17;
    V.ByteValue = X;
    PublishFrameValue(I, MoveTemp(V), C);
}
void PublishFrameText(int32 I, FDasTextValue X, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 18;
    V.TextValue = ResolveDSLText(X, C);
    PublishFrameValue(I, MoveTemp(V), C);
}
