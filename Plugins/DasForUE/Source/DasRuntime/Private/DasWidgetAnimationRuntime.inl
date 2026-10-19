void ValidateWidgetAnimationCall(FMarshalledCall& Call, das::Context* C)
{
    if (!DasWidgetAnimationFunctionAllowed(Call.Function.Get()))
        return;
    auto* W = Cast<UUserWidget>(Call.Target.Get());
    auto* Self = ResolveWorldContextObject(C);
    auto* World = Self ? Self->GetWorld() : nullptr;
    if (!IsValid(W) || W->IsTemplate() || !World || !World->IsGameWorld() || World->bIsTearingDown ||
        W->GetWorld() != World || !W->GetCachedWidget().IsValid())
        C->throw_error("WidgetAnimation requires a constructed live widget in the context game world");
    for (TFieldIterator<FProperty> It(Call.Function.Get()); It; ++It)
    {
        auto* P = *It;
        if (!P->HasAnyPropertyFlags(CPF_Parm) || P->HasAnyPropertyFlags(CPF_ReturnParm))
            continue;
        const void* Memory = P->ContainerPtrToValuePtr<void>(Call.Frame->GetStructMemory());
        if (auto* Number = CastField<FNumericProperty>(P); Number && Number->IsFloatingPoint())
        {
            const double V = Number->GetFloatingPointPropertyValue(Memory);
            if (!FMath::IsFinite(V) ||
                (P->GetFName() == TEXT("PlaybackSpeed") && V <= 0) ||
                 ((P->GetFName() == TEXT("StartAtTime") || P->GetFName() == TEXT("EndAtTime")) && V < 0))
                C->throw_error("WidgetAnimation requires finite nonnegative times and positive playback speed");
        }
        else if (auto* Loops = CastField<FIntProperty>(P); Loops && Loops->GetPropertyValue(Memory) < 0)
            C->throw_error("WidgetAnimation loops must be nonnegative; zero means infinite");
        else if (auto* Object = CastField<FObjectPropertyBase>(P);
                 Object && !IsValid(Object->GetObjectPropertyValue(Memory)))
            C->throw_error("WidgetAnimation requires a live animation asset");
    }
    if (ActiveContext->StructValues.Num() >= 4096)
        C->throw_error("struct value table limit exceeded before WidgetAnimation playback");
    Call.WidgetAnimationLease = BeginDasWidgetAnimationLease(W);
}
void CaptureWidgetAnimationOutput(FMarshalledCall& Call, das::Context* C)
{
    if (!DasWidgetAnimationFunctionAllowed(Call.Function.Get()))
        return;
    auto* P = CastField<FStructProperty>(Call.Function->GetReturnProperty());
    if (!P || !DasIsWidgetAnimationType(P->Struct))
        C->throw_error("WidgetAnimation producer signature changed");
    uint64 Token = 0;
    const void* Memory = P->ContainerPtrToValuePtr<void>(Call.Frame->GetStructMemory());
    if (CompleteDasWidgetAnimationLease(Call.WidgetAnimationLease, *static_cast<const FWidgetAnimationHandle*>(Memory)))
    {
        FInstancedStruct Snapshot;
        Snapshot.InitializeAs(P->Struct, static_cast<const uint8*>(Memory));
        Token = NextStructValueToken++;
        ActiveContext->StructValues.Add(Token, MoveTemp(Snapshot));
        ActiveContext->WidgetAnimations.Add(Token, Call.WidgetAnimationLease);
    }
    Call.WidgetAnimationResults.Add(P->GetName().ToLower(), Token);
}
FDasWidgetAnimationValue ResultWidgetAnimation(const FDasCallHandle& H, const char* Path, das::Context* C,
                                               das::LineInfoArg* At)
{
    auto& Call = ResolveCall(H, C);
    const auto Field = ResolveCallField(Call, Path, true, C);
    auto* P = CastField<FStructProperty>(Field.Property);
    if (!P || !DasIsWidgetAnimationType(P->Struct) || !P->HasAnyPropertyFlags(CPF_ReturnParm) ||
        !DasWidgetAnimationFunctionAllowed(Call.Function.Get()))
        C->throw_error("WidgetAnimation requires exact scalar producer result");
    const auto* Token = Call.WidgetAnimationResults.Find(Field.Path);
    if (!Token)
        C->throw_error("WidgetAnimation result was not captured");
    if (At && ActiveContext->StructValues.Num() >= ActiveContext->StructNextCollection)
        CollectStructValues(*ActiveContext, At);
    return {*Token};
}
TSharedPtr<FDasWidgetAnimationLease> LiveWidgetAnimation(const FDasWidgetAnimationValue& H, das::Context* C)
{
    if (C != ActiveContext || !ActiveContext)
        C->throw_error("WidgetAnimation requires active context");
    if (!H.Token)
        return nullptr;
    const auto* Value = ActiveContext->WidgetAnimations.Find(H.Token);
    if (!Value || !ActiveContext->StructValues.Contains(H.Token) || !(*Value)->IsLive())
        return nullptr;
    auto* Self = ResolveWorldContextObject(C);
    if (!Self || Self->GetWorld() != (*Value)->Widget->GetWorld())
        return nullptr;
    return *Value;
}
bool WidgetAnimationValid(const FDasWidgetAnimationValue& H, das::Context* C)
{
    return LiveWidgetAnimation(H, C).IsValid();
}
bool WidgetAnimationPlaying(const FDasWidgetAnimationValue& H, das::Context* C)
{
    auto Lease = LiveWidgetAnimation(H, C);
    if (!Lease)
        return false;
    auto State = Lease->Handle.PinAnimationState();
    return State && State->GetPlaybackStatus() == EMovieScenePlayerStatus::Playing;
}
