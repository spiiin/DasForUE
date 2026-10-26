// Call frames carry epoch-tagged tokens only. Native FReply never escapes its callback scope.
void BindEventReplyCall(FMarshalledCall& Call, das::Context* C)
{
    if (DasEventReplyFunctionAllowed(Call.Function.Get()) || DasInputNavigationFunctionAllowed(Call.Function.Get()))
        Call.WidgetReplyEpoch = WidgetReplyScope(C).Epoch;
}
void CheckEventReplyCall(FMarshalledCall& Call, das::Context* C)
{
    auto& S = WidgetReplyScope(C);
    if (!DasEventReplyFunctionAllowed(Call.Function.Get()) || !Call.WidgetReplyEpoch ||
        Call.WidgetReplyEpoch != S.Epoch)
        C->throw_error("EventReply call belongs to another or expired callback");
}
void ArgumentEventReply(const FDasCallHandle& H, const char* Path, const FDasEventReplyValue& V, das::Context* C)
{
    auto& Call = ResolveCall(H, C);
    CheckEventReplyCall(Call, C);
    const auto Field = ResolveCallField(Call, Path, false, C);
    auto* P = CastField<FStructProperty>(Field.Property);
    if (!P || !DasIsEventReplyType(P->Struct))
        C->throw_error("EventReply argument identity mismatch");
    ReadWidgetReply(V, C);
    Call.WidgetReplyFields.Add(Field.Path, V.Token);
    Call.Assigned.Add(Field.Path);
}
FDasEventReplyValue ResultEventReply(const FDasCallHandle& H, const char* Path, das::Context* C)
{
    auto& Call = ResolveCall(H, C);
    CheckEventReplyCall(Call, C);
    const auto Field = ResolveCallField(Call, Path, true, C);
    auto* P = CastField<FStructProperty>(Field.Property);
    if (!P || !DasIsEventReplyType(P->Struct))
        C->throw_error("EventReply result identity mismatch");
    const auto* Token = Call.WidgetReplyFields.Find(Field.Path);
    if (!Token)
        C->throw_error("EventReply result was not published");
    ReadWidgetReply({*Token}, C);
    return {*Token};
}
bool DispatchEventReplyCall(FMarshalledCall& Call, das::Context* C)
{
    if (!DasEventReplyFunctionAllowed(Call.Function.Get()))
        return false;
    CheckEventReplyCall(Call, C);
    auto& Scope = WidgetReplyScope(C);
    const FString Name = Call.Function->GetName();
    FDasEventReplyValue Out;
    // Each adapter publishes a new immutable reply. The inout slot and return value alias that token.
    FDasEventReplyValue Input;
    if (Name != TEXT("Handled") && Name != TEXT("Unhandled") && Name != TEXT("DetectDragIfPressed"))
    {
        const auto* Token = Call.WidgetReplyFields.Find(TEXT("reply"));
        if (!Token)
            C->throw_error("missing EventReply argument");
        Input.Token = *Token;
        ReadWidgetReply(Input, C);
    }
    auto Object = [&](const TCHAR* Field)
    {
        auto* P = FindFProperty<FObjectPropertyBase>(Call.Function.Get(), Field);
        if (!P)
            C->throw_error("EventReply target signature mismatch");
        auto* O = P->GetObjectPropertyValue_InContainer(Call.Frame->GetStructMemory());
        return MakeObjectHandle(O, UWidget::StaticClass(), C);
    };
    auto AllUsers = [&]()
    {
        auto* P = FindFProperty<FBoolProperty>(Call.Function.Get(), TEXT("bInAllUsers"));
        if (!P)
            C->throw_error("EventReply focus signature mismatch");
        return P->GetPropertyValue_InContainer(Call.Frame->GetStructMemory());
    };
    if (Name == TEXT("Handled"))
        Out = WidgetHandled(C);
    else if (Name == TEXT("Unhandled"))
        Out = WidgetUnhandled(C);
    else if (Name == TEXT("CaptureMouse"))
        Out = WidgetReplyCapture(Input, Object(TEXT("CapturingWidget")), C);
    else if (Name == TEXT("ReleaseMouseCapture"))
        Out = WidgetReplyRelease(Input, C);
    else if (Name == TEXT("LockMouse"))
        Out = WidgetReplyLock(Input, Object(TEXT("CapturingWidget")), C);
    else if (Name == TEXT("UnlockMouse"))
        Out = WidgetReplyUnlock(Input, C);
    else if (Name == TEXT("SetUserFocus"))
        Out = WidgetReplyFocus(Input, Object(TEXT("FocusWidget")), AllUsers(), C);
    else if (Name == TEXT("ClearUserFocus"))
        Out = WidgetReplyClearFocus(Input, AllUsers(), C);
    else if (Name == TEXT("EndDragDrop"))
        Out = WidgetReplyEndDrag(Input, C);
    else if (Name == TEXT("SetMousePosition"))
    {
        auto* P = FindFProperty<FStructProperty>(Call.Function.Get(), TEXT("NewMousePosition"));
        if (!P || P->Struct != TBaseStructure<FVector2D>::Get())
            C->throw_error("EventReply position signature mismatch");
        const auto& V = *P->ContainerPtrToValuePtr<FVector2D>(Call.Frame->GetStructMemory());
        if (!FMath::IsFinite(V.X) || !FMath::IsFinite(V.Y) || V.X < MIN_int32 || V.X > MAX_int32 || V.Y < MIN_int32 ||
            V.Y > MAX_int32)
            C->throw_error("EventReply mouse position must fit finite int32 coordinates");
        Out = WidgetReplyPosition(Input, int32(V.X), int32(V.Y), C);
    }
    else if (Name == TEXT("DetectDrag"))
    {
        auto* P = FindFProperty<FStructProperty>(Call.Function.Get(), TEXT("DragKey"));
        if (!P || P->Struct != FKey::StaticStruct())
            C->throw_error("EventReply drag key signature mismatch");
        const FString Key = P->ContainerPtrToValuePtr<FKey>(Call.Frame->GetStructMemory())->GetFName().ToString();
        Out = WidgetReplyDrag(Input, Object(TEXT("WidgetDetectingDrag")), TCHAR_TO_UTF8(*Key), C);
    }
    else if (Name == TEXT("DetectDragIfPressed"))
    {
        const auto* Token = Call.InputEventTokens.Find(TEXT("pointerevent"));
        if (!Token || !Scope.PointerToken || *Token != Scope.PointerToken)
            C->throw_error("DetectDragIfPressed requires the current callback pointer event");
        auto* P = FindFProperty<FStructProperty>(Call.Function.Get(), TEXT("PointerEvent"));
        auto* K = FindFProperty<FStructProperty>(Call.Function.Get(), TEXT("DragKey"));
        if (!P || P->Struct != FPointerEvent::StaticStruct() || !K || K->Struct != FKey::StaticStruct())
            C->throw_error("DetectDragIfPressed signature mismatch");
        const auto& Pointer = *P->ContainerPtrToValuePtr<FPointerEvent>(Call.Frame->GetStructMemory());
        const FKey Key = *K->ContainerPtrToValuePtr<FKey>(Call.Frame->GetStructMemory());
        if (!Key.IsValid() || (!Key.IsMouseButton() && !Key.IsTouch()))
            C->throw_error("DetectDrag requires a mouse or touch key");
        if (Pointer.GetEffectingButton() == Key || Pointer.IsTouchEvent())
        {
            const FString KeyText = Key.GetFName().ToString();
            Out = WidgetReplyDrag(WidgetHandled(C), Object(TEXT("WidgetDetectingDrag")), TCHAR_TO_UTF8(*KeyText), C);
        }
        else
            Out = WidgetUnhandled(C);
    }
    else
        C->throw_error("unsupported EventReply adapter");
    for (const auto& Field : Call.Signature)
        if (Field.bOutput && DasHasNativeRolePath(Field.TypePath, EDasNativeTypeRole::EventReply))
            Call.WidgetReplyFields.Add(Field.Path, Out.Token);
    return true;
}
