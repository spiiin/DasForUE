// No native-layout aliasing: UE constructors/Get implement the numeric semantics.
FInputActionValue NativeInputValue(const FDasInputActionValue& V, das::Context* C)
{
    if (V.Kind < 0 || V.Kind > 3 || !FMath::IsFinite(V.X) || !FMath::IsFinite(V.Y) || !FMath::IsFinite(V.Z))
        C->throw_error("invalid InputActionValue type or non-finite components");
    return FInputActionValue(static_cast<EInputActionValueType>(V.Kind), FVector(V.X, V.Y, V.Z));
}
FDasInputActionValue SnapshotInputValue(const FInputActionValue& V, das::Context* C)
{
    auto P = V.Get<FVector>();
    FDasInputActionValue Out{P.X, P.Y, P.Z, int32(V.GetValueType())};
    NativeInputValue(Out, C);
    return Out;
}
FDasInputActionValue MakeInputValue(int32 Kind, double X, double Y, double Z, das::Context* C)
{
    return SnapshotInputValue(NativeInputValue({X, Y, Z, Kind}, C), C);
}
int32 InputValueKind(const FDasInputActionValue& V, das::Context* C)
{
    return int32(NativeInputValue(V, C).GetValueType());
}
bool InputValueBool(const FDasInputActionValue& V, das::Context* C)
{
    return NativeInputValue(V, C).Get<bool>();
}
double InputValueAxis1D(const FDasInputActionValue& V, das::Context* C)
{
    return double(NativeInputValue(V, C).Get<FInputActionValue::Axis1D>());
}
FVector2D InputValueAxis2D(const FDasInputActionValue& V, das::Context* C)
{
    return NativeInputValue(V, C).Get<FVector2D>();
}
FVector InputValueAxis3D(const FDasInputActionValue& V, das::Context* C)
{
    return NativeInputValue(V, C).Get<FVector>();
}
void ArgumentInputValue(const FDasCallHandle& H, const char* Path, const FDasInputActionValue& V, das::Context* C)
{
    auto& Call = ResolveCall(H, C);
    auto Field = ResolveCallField(Call, Path, false, C);
    auto* P = CastField<FStructProperty>(Field.Property);
    if (!P || P->Struct != FInputActionValue::StaticStruct())
        C->throw_error("expected native InputActionValue argument");
    *static_cast<FInputActionValue*>(Field.Memory) = NativeInputValue(V, C);
    Call.Assigned.Add(Field.Path);
}
FDasInputActionValue ResultInputValue(const FDasCallHandle& H, const char* Path, das::Context* C)
{
    auto& Call = ResolveCall(H, C);
    auto Field = ResolveCallField(Call, Path, true, C);
    auto* P = CastField<FStructProperty>(Field.Property);
    if (!P || P->Struct != FInputActionValue::StaticStruct())
        C->throw_error("expected native InputActionValue result");
    return SnapshotInputValue(*static_cast<const FInputActionValue*>(Field.Memory), C);
}
struct FInputActionValueAnnotation : FMathValueAnnotation<FDasInputActionValue>
{
    FInputActionValueAnnotation(das::ModuleLibrary& L)
        : FMathValueAnnotation("InputActionValue", L, "FDasInputActionValue")
    {
    }
    virtual bool isPod() const override { return true; }
    virtual bool canBePlacedInContainer() const override { return true; }
};

FDasStructValue PackInputValue(const FDasInputActionValue& V, das::Context* C, das::LineInfoArg* At)
{
    auto Native = NativeInputValue(V, C);
    FInstancedStruct S;
    S.InitializeAs(FInputActionValue::StaticStruct(), reinterpret_cast<const uint8*>(&Native));
    return StoreStructValue(MoveTemp(S), C, At);
}
FDasInputActionValue UnpackInputValue(const FDasStructValue& V, das::Context* C)
{
    const auto& S = ResolveStructValue(V, C);
    if (S.GetScriptStruct() != FInputActionValue::StaticStruct())
        C->throw_error("InputActionValue snapshot identity mismatch");
    return SnapshotInputValue(S.Get<FInputActionValue>(), C);
}
