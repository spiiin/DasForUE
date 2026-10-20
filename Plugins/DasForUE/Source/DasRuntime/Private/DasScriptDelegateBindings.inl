FDasObjectHandle DelegateSelfTarget(const FDasObjectHandle& Target, das::Context* C)
{
    if (ResolveObject(Target, C) != ResolveSelf(C))
        C->throw_error("self.Method delegate reference requires the current self target");
    return Target;
}
uint64 DelegateBindScript(uint64 Token, const char* Wire, bool Multi, const FDasObjectHandle& Target, const char* Entry,
                          bool Remove, das::Context* C)
{
    DelegateSelfTarget(Target, C);
    auto* S = ManagedOwnerSession(C);
    if (ActiveContext->bEnding)
        C->throw_error("script delegate binding is forbidden during EndPlay");
    auto Value = ResolveDelegateValue(Token, Wire, Multi, C);
    if (!Entry || !*Entry || FCStringAnsi::Strlen(Entry) >= NAME_SIZE)
        C->throw_error("invalid script delegate entry");
    const FString Key = UTF8_TO_TCHAR(Entry) + FString(TEXT(":")) + UTF8_TO_TCHAR(Wire);
    auto* Found = ActiveContext->ScriptDelegateProxies.Find(Key);
    if (Remove && !Found)
        return Token;
    if (!Found)
    {
        if (ActiveContext->ScriptDelegateProxies.Num() >= 256)
            C->throw_error("script delegate proxy limit reached");
        FDasAuthoringModule Model;
        FDasClassSchema Schema;
        FString Error;
        if (!FDasAuthoringModule::FromJson(UTF8_TO_TCHAR(Wire), Model, Error) ||
            !Model.ToActorSchema(Model.Classes[0].Id, Schema, Error))
            C->throw_error(TCHAR_TO_UTF8(*Error));
        auto Method = Schema.EffectiveMethods()[0];
        Method.Signature = EDasMethodSignature::Frame;
        const auto Contract = Method.Contract();
        if (!VerifyContract(S->Live->Program, UTF8_TO_TCHAR(Entry), Contract))
            C->throw_error("script delegate ABI contract mismatch");
        S->RequiredEntries.Add(UTF8_TO_TCHAR(Entry), Contract);
        TStrongObjectPtr<UDasScriptDelegateProxy> Proxy(NewObject<UDasScriptDelegateProxy>());
        Proxy->Owner = S->Owner;
        Proxy->Script = S->Path;
        Proxy->Entry = UTF8_TO_TCHAR(Entry);
        Proxy->Schema = Method;
        Proxy->Lease = ActiveContext->DelegateLease;
        const FName Function = Proxy->Configure(Value.Signature.Get(), Error);
        if (Function.IsNone())
            C->throw_error(TCHAR_TO_UTF8(*Error));
        // Configure caches by signature; the receiver function name is stable within this process.
        Proxy->Schema.EntryPoint = Function.ToString();
        ActiveContext->ScriptDelegateProxies.Add(Key, MoveTemp(Proxy));
        Found = ActiveContext->ScriptDelegateProxies.Find(Key);
    }
    auto* Proxy = Found->Get();
    const FName Function(*Proxy->Schema.EntryPoint);
    const auto Handle = MakeObjectHandle(Proxy, UDasScriptDelegateProxy::StaticClass(), C);
    if (Remove)
    {
        ActiveContext->OwnedScriptDelegateEvents.RemoveAll(
            [&](const auto& B)
            { return B->Source == Value.Source && B->Event == Value.Property && B->Delegate.GetUObject() == Proxy; });
        return DelegateRemove(Token, Wire, Multi, Handle, TCHAR_TO_UTF8(*Function.ToString()), C);
    }
    const uint64 Result = DelegateBind(Token, Wire, Multi, Handle, TCHAR_TO_UTF8(*Function.ToString()), C);
    if (!Value.Property.IsNone() &&
        !ActiveContext->OwnedScriptDelegateEvents.ContainsByPredicate(
            [&](const auto& B)
            { return B->Source == Value.Source && B->Event == Value.Property && B->Delegate.GetUObject() == Proxy; }))
    {
        auto B = MakeUnique<FDasOwnedScriptDelegate>();
        B->Source = Value.Source;
        B->Event = Value.Property;
        B->Delegate.BindUFunction(Proxy, Function);
        ActiveContext->OwnedScriptDelegateEvents.Add(MoveTemp(B));
    }
    return Result;
}
