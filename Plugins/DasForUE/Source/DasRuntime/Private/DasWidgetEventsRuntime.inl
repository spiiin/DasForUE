// Replies are commands owned by one native Slate call, never persistent snapshots.
FWidgetReplyScope& WidgetReplyScope(das::Context* C)
{
    if (!ActiveWidgetReply || ActiveWidgetReply->Context != C || ActiveContext != C || !IsInGameThread())
        C->throw_error("EventReply requires the active synchronous widget callback");
    return *ActiveWidgetReply;
}
FReply ReadWidgetReply(const FDasEventReplyValue& H, das::Context* C)
{
    auto& Scope = WidgetReplyScope(C);
    const auto* Reply = Scope.Replies.Find(H.Token);
    if (!Reply)
        C->throw_error("EventReply is null, expired or belongs to another callback");
    return *Reply;
}
FDasEventReplyValue StoreWidgetReply(const FReply& Reply, das::Context* C)
{
    auto& Scope = WidgetReplyScope(C);
    if (Scope.Replies.Num() >= 128)
        C->throw_error("EventReply command limit exceeded");
    const uint64 Token = NextWidgetReplyToken++;
    Scope.Replies.Add(Token, Reply);
    return {Token};
}
FDasEventReplyValue WidgetHandled(das::Context* C)
{
    return StoreWidgetReply(FReply::Handled(), C);
}
FDasEventReplyValue WidgetUnhandled(das::Context* C)
{
    return StoreWidgetReply(FReply::Unhandled(), C);
}
FReply HandledWidgetReply(const FDasEventReplyValue& H, das::Context* C)
{
    auto Reply = ReadWidgetReply(H, C);
    if (!Reply.IsEventHandled())
        C->throw_error("EventReply commands require a handled reply");
    return Reply;
}
TSharedRef<SWidget> WidgetReplyTarget(const FDasObjectHandle& H, das::Context* C)
{
    auto& Scope = WidgetReplyScope(C);
    auto* Target = Cast<UWidget>(ResolveObject(H, C));
    if (!Target || !Scope.Widget.IsValid() || Target->GetWorld() != Scope.Widget->GetWorld() || Target->IsTemplate())
        C->throw_error("EventReply target must be a live widget in the callback world");
    auto Cached = Target->GetCachedWidget();
    if (!Cached.IsValid())
        C->throw_error("EventReply target has no constructed Slate widget");
    Scope.Targets.AddUnique(Target);
    return Cached.ToSharedRef();
}
FDasEventReplyValue WidgetReplyCapture(const FDasEventReplyValue& H, const FDasObjectHandle& Target, das::Context* C)
{
    auto R = HandledWidgetReply(H, C);
    R.CaptureMouse(WidgetReplyTarget(Target, C));
    return StoreWidgetReply(R, C);
}
FDasEventReplyValue WidgetReplyRelease(const FDasEventReplyValue& H, das::Context* C)
{
    auto R = HandledWidgetReply(H, C);
    R.ReleaseMouseCapture();
    return StoreWidgetReply(R, C);
}
FDasEventReplyValue WidgetReplyFocus(const FDasEventReplyValue& H, const FDasObjectHandle& Target, bool AllUsers,
                                     das::Context* C)
{
    auto R = HandledWidgetReply(H, C);
    R.SetUserFocus(WidgetReplyTarget(Target, C), EFocusCause::SetDirectly, AllUsers);
    return StoreWidgetReply(R, C);
}
FDasEventReplyValue WidgetReplyClearFocus(const FDasEventReplyValue& H, bool AllUsers, das::Context* C)
{
    auto R = HandledWidgetReply(H, C);
    R.ClearUserFocus(AllUsers);
    return StoreWidgetReply(R, C);
}
FDasEventReplyValue WidgetReplyLock(const FDasEventReplyValue& H, const FDasObjectHandle& Target, das::Context* C)
{
    auto R = HandledWidgetReply(H, C);
    R.LockMouseToWidget(WidgetReplyTarget(Target, C));
    return StoreWidgetReply(R, C);
}
FDasEventReplyValue WidgetReplyUnlock(const FDasEventReplyValue& H, das::Context* C)
{
    auto R = HandledWidgetReply(H, C);
    R.ReleaseMouseLock();
    return StoreWidgetReply(R, C);
}
FDasEventReplyValue WidgetReplyPosition(const FDasEventReplyValue& H, int32 X, int32 Y, das::Context* C)
{
    auto R = HandledWidgetReply(H, C);
    R.SetMousePos(FIntPoint(X, Y));
    return StoreWidgetReply(R, C);
}
FDasEventReplyValue WidgetReplyDrag(const FDasEventReplyValue& H, const FDasObjectHandle& Target, const char* Key,
                                    das::Context* C)
{
    auto R = HandledWidgetReply(H, C);
    const FKey Button(Key ? FName(UTF8_TO_TCHAR(Key)) : NAME_None);
    if (!Button.IsValid() || (!Button.IsMouseButton() && !Button.IsTouch()))
        C->throw_error("DetectDrag requires a mouse or touch key");
    R.DetectDrag(WidgetReplyTarget(Target, C), Button);
    return StoreWidgetReply(R, C);
}
FDasEventReplyValue WidgetReplyEndDrag(const FDasEventReplyValue& H, das::Context* C)
{
    auto R = HandledWidgetReply(H, C);
    R.EndDragDrop();
    return StoreWidgetReply(R, C);
}
void BindWidgetInput(const FDasObjectHandle& H, const char* Script, const char* Event, const char* Handler,
                     das::Context* C)
{
    auto* Widget = Cast<UDasScriptUserWidget>(OwnedWidget(H, C));
    const FString File = Script ? UTF8_TO_TCHAR(Script) : TEXT("");
    if (!Widget || File.IsEmpty() || File != FPaths::GetCleanFilename(File) ||
        FPaths::GetExtension(File) != TEXT("das") || !Event || !Handler || FCStringAnsi::Strlen(Event) >= NAME_SIZE ||
                                           FCStringAnsi::Strlen(Handler) >= NAME_SIZE ||
                                           !UDasScriptUserWidget::IsSupportedInputEvent(FName(UTF8_TO_TCHAR(Event))))
        C->throw_error("invalid script widget input binding");
    if (Widget->ScriptFile != File)
    {
        FModuleManager::GetModuleChecked<FDasRuntimeModule>("DasRuntime").ReleaseObject(Widget);
        Widget->InputHandlers.Empty();
        Widget->DragHandlers.Empty();
        Widget->ScriptFile = File;
    }
    Widget->BindInputHandler(FName(UTF8_TO_TCHAR(Event)), FName(UTF8_TO_TCHAR(Handler)));
}
