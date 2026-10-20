void ResolveWidgetLayout(const FDasObjectHandle& H, const char* Spec, das::Context* C)
{
    auto* W = OwnedWidget(H, C);
    FDasWidgetLayoutUse L;
    FDasResolvedWidgetLayout Value;
    FString Error;
    const FString Json = Spec ? UTF8_TO_TCHAR(Spec) : TEXT("");
    if (!W->GetCachedWidget().IsValid() || !ParseDasWidgetLayout(Json, L, Error) ||
        !ValidateDasWidgetLayout(W->WidgetTree, L, Error, &Value))
        C->throw_error(TCHAR_TO_UTF8(
            *(Error.IsEmpty() ? FString(TEXT("Widget layout requires a constructed UserWidget")) : Error)));
    for (auto It = ActiveContext->WidgetLayouts.CreateIterator(); It; ++It)
        if (!It.Value().Owner.IsValid())
            It.RemoveCurrent();
    const FString Key = FString::FromInt(W->GetUniqueID()) + TEXT(":")+ Json;
    if (!ActiveContext->WidgetLayouts.Contains(Key) && ActiveContext->WidgetLayouts.Num() >= 64)
        C->throw_error("widget layout cache limit exceeded");
    Value.Owner = W;
    ActiveContext->WidgetLayouts.Add(Key, MoveTemp(Value));
}
FDasObjectHandle WidgetLayoutField(const FDasObjectHandle& H, const char* Spec, const char* Name, das::Context* C)
{
    auto* W = OwnedWidget(H, C);
    const FString Key = FString::FromInt(W->GetUniqueID()) + TEXT(":")+ (Spec ? UTF8_TO_TCHAR(Spec) : TEXT(""));
    const auto* View = ActiveContext->WidgetLayouts.Find(Key);
    const auto* Field = View && Name ? View->Fields.Find(FName(UTF8_TO_TCHAR(Name))) : nullptr;
    auto* Widget = Field ? Field->Get() : nullptr;
    if (!View || View->Owner.Get() != W || !W->GetCachedWidget().IsValid() || !IsValid(Widget))
        C->throw_error("widget layout field is missing or expired; resolve the view again");
    return MakeObjectHandle(Widget, Widget->GetClass(), C);
}
void GameplayWidgetTextRef(const FDasObjectHandle& H, FDasTextValue Text, das::Context* C)
{
    auto* T = Cast<UTextBlock>(ResolveObject(H, C));
    auto* W = T ? T->GetTypedOuter<UUserWidget>() : nullptr;
    if (!T || !W || OwnedWidget(MakeObjectHandle(W, UUserWidget::StaticClass(), C), C) != W ||
        W->GetWidgetFromName(T->GetFName()) != T)
        C->throw_error("TextBlock is not a live member of an owned widget");
    T->SetText(ResolveDSLText(Text, C));
}
void GameplayWidgetClickRef(const FDasObjectHandle& H, const char* Handler, das::Context* C)
{
    auto* Button = Cast<UButton>(ResolveObject(H, C));
    auto* W = Button ? Button->GetTypedOuter<UUserWidget>() : nullptr;
    if (!Button || !W || OwnedWidget(MakeObjectHandle(W, UUserWidget::StaticClass(), C), C) != W ||
        W->GetWidgetFromName(Button->GetFName()) != Button)
        C->throw_error("Button is not a live member of an owned widget");
    // Shared implementation retains exact callback checks and managed cleanup.
    GameplayWidgetClick(MakeObjectHandle(W, UUserWidget::StaticClass(), C), TCHAR_TO_UTF8(*Button->GetName()), Handler,
                        C);
}
