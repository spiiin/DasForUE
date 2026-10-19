FDasStructValue StoreGeometry(const UScriptStruct* Type, const void* Data, das::Context* C, das::LineInfoArg* At)
{
    if (!ActiveContext || C != ActiveContext)
        C->throw_error("Geometry requires active context");
    FInstancedStruct Owned;
    FString E;
    if (!CaptureDasGeometry(Type, Data, Owned, E))
        C->throw_error(TCHAR_TO_UTF8(*E));
    if (At && ActiveContext->StructValues.Num() >= ActiveContext->StructNextCollection)
        CollectStructValues(*ActiveContext, At);
    if (ActiveContext->StructValues.Num() >= 4096)
        C->throw_error("struct value table limit exceeded");
    const uint64 Token = NextStructValueToken++;
    ActiveContext->StructValues.Add(Token, MoveTemp(Owned));
    return {Token};
}
const FInstancedStruct& ResolveGeometry(const FDasStructValue& H, das::Context* C)
{
    const auto& V = ResolveStructValue(H, C);
    FString E;
    if (!ValidateDasGeometrySnapshot(V, E))
        C->throw_error(TCHAR_TO_UTF8(*E));
    return V;
}
FDasGeometryValue GeometryFromStruct(const FDasStructValue& H, das::Context* C)
{
    ResolveGeometry(H, C);
    return {H.Token};
}
void ArgumentGeometry(const FDasCallHandle& H, const char* Path, const FDasGeometryValue& V, das::Context* C)
{
    auto& Call = ResolveCall(H, C);
    const auto Field = ResolveCallField(Call, Path, false, C);
    auto* P = CastField<FStructProperty>(Field.Property);
    if (!P || !DasIsGeometryType(P->Struct) || !DasGeometryFunctionAllowed(Call.Function.Get()))
        C->throw_error("Geometry argument requires exact scalar allowlist");
    const auto& S = ResolveGeometry({V.Token}, C);
    P->Struct->CopyScriptStruct(Field.Memory, S.GetMemory());
    Call.Assigned.Add(Field.Path);
}
FDasGeometryValue ResultGeometry(const FDasCallHandle& H, const char* Path, das::Context* C, das::LineInfoArg* At)
{
    auto& Call = ResolveCall(H, C);
    const auto Field = ResolveCallField(Call, Path, true, C);
    auto* P = CastField<FStructProperty>(Field.Property);
    if (!P || !DasIsGeometryType(P->Struct) || !DasGeometryFunctionAllowed(Call.Function.Get()))
        C->throw_error("Geometry result requires exact scalar allowlist");
    return {StoreGeometry(P->Struct, Field.Memory, C, At).Token};
}
void ValidateGeometryCall(FMarshalledCall& Call, das::Context* C, bool Outputs)
{
    FString E;
    if (!ValidateDasGeometryCall(Call.Function.Get(), Call.Frame->GetStructMemory(), E, Outputs))
        C->throw_error(TCHAR_TO_UTF8(*E));
}
