FWidgetPaintScope& WidgetPaintScope(das::Context* C)
{
    if (!ActiveWidgetPaint || ActiveWidgetPaint->Context != C || ActiveContext != C || !IsInGameThread() ||
        !ActiveWidgetPaint->Widget.IsValid())
        C->throw_error("PaintContext requires the active synchronous OnPaint callback");
    return *ActiveWidgetPaint;
}
void CheckPaintToken(const FDasPaintValue& V, das::Context* C)
{
    auto& S = WidgetPaintScope(C);
    if (!V.Token || V.Token != S.Epoch)
        C->throw_error("PaintContext is null, expired or belongs to another callback");
}
void BindPaintCall(FMarshalledCall& Call, das::Context* C)
{
    if (DasPaintFunctionAllowed(Call.Function.Get()))
        Call.PaintEpoch = WidgetPaintScope(C).Epoch;
}
void CheckPaintCall(FMarshalledCall& Call, das::Context* C)
{
    auto& S = WidgetPaintScope(C);
    if (!DasPaintFunctionAllowed(Call.Function.Get()) || Call.PaintEpoch != S.Epoch)
        C->throw_error("PaintContext call belongs to another or expired callback");
}
void ArgumentPaintContext(const FDasCallHandle& H, const char* Path, const FDasPaintValue& V, das::Context* C)
{
    auto& Call = ResolveCall(H, C);
    CheckPaintCall(Call, C);
    const auto Field = ResolveCallField(Call, Path, false, C);
    auto* P = CastField<FStructProperty>(Field.Property);
    if (!P || !DasIsPaintContextType(P->Struct))
        C->throw_error("PaintContext argument identity mismatch");
    CheckPaintToken(V, C);
    Call.Assigned.Add(Field.Path);
}
FDasPaintValue ResultPaintContext(const FDasCallHandle& H, const char* Path, das::Context* C)
{
    auto& Call = ResolveCall(H, C);
    CheckPaintCall(Call, C);
    const auto Field = ResolveCallField(Call, Path, true, C);
    auto* P = CastField<FStructProperty>(Field.Property);
    if (!P || !DasIsPaintContextType(P->Struct))
        C->throw_error("PaintContext result identity mismatch");
    return {Call.PaintEpoch};
}
bool StagePaintCall(FMarshalledCall& Call, das::Context* C)
{
    if (!DasPaintFunctionAllowed(Call.Function.Get()))
        return false;
    CheckPaintCall(Call, C);
    auto& S = WidgetPaintScope(C);
    if (S.Commands.Num() >= 1024 || S.Native->MaxLayer > MAX_int32 - 1025)
        C->throw_error("PaintContext command or layer limit exceeded");
    int32 Points = 0, TextChars = 0;
    TFunction<void(FProperty*, const void*)> Validate;
    auto Coordinate = [&](const FVector2D& V)
    {
        if (!FMath::IsFinite(V.X) || !FMath::IsFinite(V.Y) || FMath::Abs(V.X) > 1.e7 || FMath::Abs(V.Y) > 1.e7)
            C->throw_error("PaintContext coordinate must be finite and within 1e7 Slate units");
        float A, B, D, E;
        S.Native->AllottedGeometry.GetAccumulatedRenderTransform().GetMatrix().GetMatrix(A, B, D, E);
        auto T = S.Native->AllottedGeometry.GetAccumulatedRenderTransform().GetTranslation();
        if (FMath::Abs(double(A) * V.X) + FMath::Abs(double(D) * V.Y) + FMath::Abs(T.X) > double(MAX_flt) / 8 ||
            FMath::Abs(double(B) * V.X) + FMath::Abs(double(E) * V.Y) + FMath::Abs(T.Y) > double(MAX_flt) / 8)
            C->throw_error("PaintContext transform would overflow Slate float math");
    };
    Validate = [&](FProperty* P, const void* M)
    {
        if (auto* A = CastField<FArrayProperty>(P))
        {
            FScriptArrayHelper H(A, M);
            if (H.Num() > 4096 || Points + H.Num() > 4096)
                C->throw_error("PaintContext point limit exceeded");
            Points += H.Num();
            for (int32 I = 0; I < H.Num(); ++I)
                Validate(A->Inner, H.GetRawPtr(I));
        }
        else if (auto* Struct = CastField<FStructProperty>(P))
        {
            if (Struct->Struct == TBaseStructure<FVector2D>::Get())
            {
                const auto V = *static_cast<const FVector2D*>(M);
                Coordinate(V);
                if (P->GetFName() == TEXT("Size") && (V.X < 0 || V.Y < 0))
                    C->throw_error("PaintContext box size must be nonnegative");
            }
            else
                for (TFieldIterator<FProperty> It(Struct->Struct); It; ++It)
                    Validate(*It, It->ContainerPtrToValuePtr<void>(M));
        }
        else if (auto* N = CastField<FNumericProperty>(P); N && N->IsFloatingPoint())
        {
            const double V = N->GetFloatingPointPropertyValue(M);
            if (!FMath::IsFinite(V) || FMath::Abs(V) > 1.e7)
                C->throw_error("PaintContext scalar/color must be finite and bounded");
            if (P->GetFName() == TEXT("Thickness") && (V < 0 || V > 10000))
                C->throw_error("PaintContext thickness must be between 0 and 10000");
            if (P->GetFName() == TEXT("FontSize") && (V <= 0 || V > 4096))
                C->throw_error("PaintContext font size must be between 0 and 4096");
        }
        else if (auto* T = CastField<FTextProperty>(P))
        {
            const int32 TextCount = T->GetPropertyValue(M).ToString().Len();
            if (TextCount > 32768)
                C->throw_error("PaintContext text limit exceeded");
            TextChars += TextCount;
        }
    };
    for (TFieldIterator<FProperty> It(Call.Function.Get()); It; ++It)
    {
        auto* P = *It;
        if (!P->HasAnyPropertyFlags(CPF_Parm) ||
            (CastField<FStructProperty>(P) && DasIsPaintContextType(CastFieldChecked<FStructProperty>(P)->Struct)))
            continue;
        Validate(P, P->ContainerPtrToValuePtr<void>(Call.Frame->GetStructMemory()));
    }
    if (S.Points + Points > 65536 || S.TextChars + TextChars > 131072)
        C->throw_error("PaintContext aggregate data limit exceeded");
    FPaintCommand Command;
    Command.Function.Reset(Call.Function.Get());
    Command.Arguments = MakeUnique<FStructOnScope>(Call.Function.Get());
    for (TFieldIterator<FProperty> It(Call.Function.Get()); It; ++It)
    {
        auto* P = *It;
        if (!P->HasAnyPropertyFlags(CPF_Parm) ||
            (CastField<FStructProperty>(P) && DasIsPaintContextType(CastFieldChecked<FStructProperty>(P)->Struct)))
            continue;
        P->CopyCompleteValue(P->ContainerPtrToValuePtr<void>(Command.Arguments->GetStructMemory()),
                             P->ContainerPtrToValuePtr<void>(Call.Frame->GetStructMemory()));
        if (auto* O = CastField<FObjectPropertyBase>(P))
            Command.Pins.Emplace(O->GetObjectPropertyValue_InContainer(Call.Frame->GetStructMemory()));
    }
    S.Points += Points;
    S.TextChars += TextChars;
    S.Commands.Add(MoveTemp(Command));
    return true;
}
void CommitPaintCommands(FWidgetPaintScope& S)
{
    for (auto& Command : S.Commands)
    {
        auto* P = FindFProperty<FStructProperty>(Command.Function.Get(), TEXT("Context"));
        // Borrow the real context only in this native stack, after DAS has returned successfully.
        P->Struct->CopyScriptStruct(P->ContainerPtrToValuePtr<void>(Command.Arguments->GetStructMemory()), S.Native);
        Command.Function->GetOuterUClass()->GetDefaultObject()->ProcessEvent(Command.Function.Get(),
                                                                             Command.Arguments->GetStructMemory());
        S.Native->MaxLayer =
            static_cast<FPaintContext*>(P->ContainerPtrToValuePtr<void>(Command.Arguments->GetStructMemory()))
                ->MaxLayer;
    }
}
