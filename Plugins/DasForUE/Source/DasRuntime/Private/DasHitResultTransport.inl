// Opaque native snapshots share the bounded, GC-aware context value table.
// Only UE's copy operation touches native payload, including ActorInstanceHandle.
bool HitResultFinite(const FHitResult& H)
{
    return FMath::IsFinite(H.Time) && FMath::IsFinite(H.Distance) && FMath::IsFinite(H.PenetrationDepth) &&
           !H.Location.ContainsNaN() && !H.ImpactPoint.ContainsNaN() && !H.Normal.ContainsNaN() &&
           !H.ImpactNormal.ContainsNaN() && !H.TraceStart.ContainsNaN() && !H.TraceEnd.ContainsNaN();
}
FDasHitResultValue StoreHitResult(const FHitResult& Hit, das::Context* C, das::LineInfoArg* At)
{
    if (!ActiveContext || C != ActiveContext)
        C->throw_error("HitResult requires active context");
    if (!HitResultFinite(Hit))
        C->throw_error("nonfinite native HitResult");
    if (At && ActiveContext->StructValues.Num() >= ActiveContext->StructNextCollection)
        CollectStructValues(*ActiveContext, At);
    if (ActiveContext->StructValues.Num() >= 4096)
        C->throw_error("struct value table limit exceeded");
    FInstancedStruct Value;
    Value.InitializeAs(FHitResult::StaticStruct(), reinterpret_cast<const uint8*>(&Hit));
    const uint64 Token = NextStructValueToken++;
    ActiveContext->StructValues.Add(Token, MoveTemp(Value));
    return {Token};
}
const FHitResult& ResolveHitResult(const FDasHitResultValue& H, das::Context* C)
{
    if (!ActiveContext || ActiveContext != C)
        C->throw_error("HitResult requires active context");
    const auto* V = ActiveContext->StructValues.Find(H.Token);
    if (!V || V->GetScriptStruct() != FHitResult::StaticStruct())
        C->throw_error("HitResult is null, expired, or belongs to another context");
    return V->Get<FHitResult>();
}
void ArgumentHitResult(const FDasCallHandle& H, const char* Path, const FDasHitResultValue& Value, das::Context* C)
{
    auto& Call = ResolveCall(H, C);
    const auto Field = ResolveCallField(Call, Path, false, C);
    auto* P = CastField<FStructProperty>(Field.Property);
    if (!P || P->Struct != FHitResult::StaticStruct())
        C->throw_error("expected native HitResult argument");
    const auto& Hit = ResolveHitResult(Value, C);
    P->Struct->CopyScriptStruct(Field.Memory, &Hit);
    Call.Assigned.Add(Field.Path);
    // Every reference within FHitResult remains native weak storage; no strong pins.
}
FDasHitResultValue ResultHitResult(const FDasCallHandle& H, const char* Path, das::Context* C, das::LineInfoArg* At)
{
    auto& Call = ResolveCall(H, C);
    const auto Field = ResolveCallField(Call, Path, true, C);
    auto* P = CastField<FStructProperty>(Field.Property);
    if (!P || P->Struct != FHitResult::StaticStruct())
        C->throw_error("expected native HitResult result");
    return StoreHitResult(*static_cast<const FHitResult*>(Field.Memory), C, At);
}
FDasHitResultValue EmptyHitResult(das::Context* C, das::LineInfoArg* At)
{
    return StoreHitResult(FHitResult(ForceInit), C, At);
}
FDasTraceHit ViewHitResult(const FDasHitResultValue& H, das::Context* C)
{
    RequireObjectContext(C);
    return ProjectHitResult(ResolveHitResult(H, C), C);
}

int32 StoreGASTargetHit(const FDasObjectHandle& Source,const FDasHitResultValue& Hit,das::Context* C) {
    auto* ASC=Cast<UDasAuthoredAbilitySystemComponent>(ResolveObject(Source,C));
    if(!ASC)C->throw_error("GAS hit source requires an authored ASC");
    return ASC->RegisterAuthoredTargetHit(ResolveHitResult(Hit,C));
}
