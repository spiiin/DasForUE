// Synchronous FRealCurve::Eval preserves UE interpolation/extrapolation, without exporting row pointers.
UCurveTable* ScriptCurveTable(const FDasObjectHandle& H, das::Context* C)
{
    RequireObjectContext(C);
    auto* Curves = Cast<UCurveTable>(ResolveObject(H, C));
    if (!IsValid(Curves) || Curves->IsTemplate())
        C->throw_error("expected a live CurveTable");
    return Curves;
}
FName ScriptCurveTableName(const char* Text, das::Context* C)
{
    const FString Name = Text ? UTF8_TO_TCHAR(Text) : TEXT("");
    if (Name.IsEmpty() || Name.Len() > 1024 || Name.Equals(TEXT("None"), ESearchCase::IgnoreCase))
        C->throw_error("CurveTable row name must contain 1..1024 characters and cannot be None");
    return FName(*Name);
}
bool ScriptCurveTableHas(const FDasObjectHandle& H, const char* Name, das::Context* C)
{
    auto* Curves = ScriptCurveTable(H, C);
    const auto* Row = Curves->GetRowMap().Find(ScriptCurveTableName(Name, C));
    return Row && *Row;
}
FDasStructValue ScriptCurveTableNames(const FDasObjectHandle& H, das::Context* C, das::LineInfoArg* At)
{
    auto* Curves = ScriptCurveTable(H, C);
    if (Curves->GetRowMap().Num() > 4096)
        C->throw_error("CurveTable name listing exceeds 4096 rows");
    TArray<FName> Names;
    Curves->GetRowMap().GetKeys(Names);
    Names.Sort(FNameLexicalLess());
    FInstancedStruct V;
    V.InitializeAs<FDasArrayStringCarrier>();
    for (auto Name : Names)
        V.GetMutable<FDasArrayStringCarrier>().Values.Add(Name.ToString());
    return StoreStructValue(MoveTemp(V), C, At);
}
float ScriptCurveTableEval(const FDasObjectHandle& H, const char* Name, float Time, float DefaultValue, das::Context* C)
{
    auto* Curves = ScriptCurveTable(H, C);
    if (!FMath::IsFinite(Time) || !FMath::IsFinite(DefaultValue))
        C->throw_error("CurveTable time and default value must be finite");
    const auto* Row = Curves->GetRowMap().Find(ScriptCurveTableName(Name, C));
    if (!Row || !*Row)
        C->throw_error("CurveTable row not found");
    const float Value = (*Row)->Eval(Time, DefaultValue);
    if (!FMath::IsFinite(Value))
        C->throw_error("CurveTable evaluation produced a nonfinite value");
    return Value;
}
