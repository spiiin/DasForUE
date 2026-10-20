// Part of DasRuntime.cpp, inside its private namespace. Payload ownership is on GC-visible sessions.
TArray<TWeakObjectPtr<UDasGASCueSession>> PendingGASCueSessions;
void DrainOneGASCueHook()
{
    if (PendingGASCueSessions.IsEmpty())
        return;
    auto Weak = PendingGASCueSessions[0];
    PendingGASCueSessions.RemoveAt(0);
    auto* Session = Weak.Get();
    if (!Session || !Session->Live() || Session->Pending.IsEmpty())
        return;
    const auto Event = Session->Pending[0];
    Session->Pending.RemoveAt(0);
    if (!Session->Pending.IsEmpty())
        PendingGASCueSessions.AddUnique(Session);
    if (auto* R = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
    {
        const auto Result = R->DispatchGASCueHook(Session, Event);
        if (!Result.bSucceeded)
        {
            UE_LOG(LogDasRuntime, Warning,
                   TEXT("DAS_GAS_CUE_HOOK_FAILED %s: %s"), *Session->Definition->DefinitionId, *Result.Error);
            Session->Close();
        }
    }
}
void DropPendingGASCueHooks()
{
    for (auto Weak : PendingGASCueSessions)
        if (auto* S = Weak.Get())
            S->Pending.Reset();
    PendingGASCueSessions.Reset();
}
