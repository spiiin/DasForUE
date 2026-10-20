// Session-owned direct UFunction bindings. No property pointers survive a call or reload.
void DynamicEventListen(const char* SourceName, const char* Event, const char* Handler, bool Add, das::Context* C)
{
    if (ActiveContext && ActiveContext->bEnding)
    {
        if (!Add)
            return; // Lifecycle teardown has already revoked the session's bindings.
        C->throw_error("dynamic event registration is forbidden during EndPlay");
    }
    auto* S = ManagedOwnerSession(C);
    if (!SourceName || !*SourceName || FCStringAnsi::Strlen(SourceName) >= NAME_SIZE || !Event || !*Event ||
        FCStringAnsi::Strlen(Event) >= NAME_SIZE || !Handler || !*Handler || FCStringAnsi::Strlen(Handler) >= NAME_SIZE)
        C->throw_error("dynamic event source, event and handler names are required");
    auto* Target = ResolveSelf(C);
    auto* Source =
        FCStringAnsi::Strcmp(SourceName, "self") == 0 ? Target : ResolveObject(GetSelfObject(SourceName, C), C);
    if (!LiveDelegateObject(Source) || !LiveDelegateObject(Target))
        C->throw_error("dynamic event source or owner has expired");
    const FName EventName(UTF8_TO_TCHAR(Event)), FunctionName(UTF8_TO_TCHAR(Handler));
    auto* P = FindFProperty<FMulticastDelegateProperty>(Source->GetClass(), EventName);
    if (!P || P->ArrayDim != 1 || !P->SignatureFunction || !P->HasAnyPropertyFlags(CPF_BlueprintAssignable))
        C->throw_error("dynamic event requires a BlueprintAssignable multicast property");
    DelegateHandler(Target, FunctionName, P->SignatureFunction, C);
    auto Match = [&](const auto& B)
    {
        return B->DirectFunction == FunctionName && B->DirectTarget == Target && B->Source == Source &&
               B->Event == EventName;
    };
    if (!Add)
    {
        S->Bindings.RemoveAll(Match);
        return;
    }
    if (S->Bindings.ContainsByPredicate(Match))
        return;
    if (S->Bindings.Num() >= 256)
        C->throw_error("managed subscription limit reached");
    auto B = MakeUnique<FManagedSubscription>();
    B->Source = Source;
    B->Event = EventName;
    B->DirectTarget = Target;
    B->DirectFunction = FunctionName;
    FScriptDelegate D;
    D.BindUFunction(Target, FunctionName);
    P->AddDelegate(D, Source, P->ContainerPtrToValuePtr<void>(Source));
    S->Bindings.Add(MoveTemp(B));
}
