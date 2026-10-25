// Legacy fixture helpers sharing the runtime subscription context.
void SubscribeInt(const char* SourceKind, const char* Event, const char* Handler, das::Context* Context)
{
    RequireObjectContext(Context);
    if (!SourceKind)
        Context->throw_error("subscription source is required");
    UObject* Source = ResolveSelf(Context);
    const FString Kind(UTF8_TO_TCHAR(SourceKind));
    if (Kind == TEXT("peer"))
    {
        auto* Actor = Cast<ADasProbeActor>(Source);
        Source = Actor ? Actor->Peer.Get() : nullptr;
    }
    else if (Kind != TEXT("self"))
        Context->throw_error("subscription source must be self or peer");
    SubscribeObjectInt(MakeObjectHandle(Source, UObject::StaticClass(), Context), Event, Handler, Context);
}

void EmitSignal(int32 Value, das::Context* Context)
{
    if (ConstructingObject || (ActiveContext && ActiveContext->bCollectingSubscriptions))
        Context->throw_error("event emission is forbidden in bind_events/ConstructionScript");
    auto* Actor = Cast<ADasProbeActor>(ResolveSelf(Context));
    if (!Actor)
        Context->throw_error("signal emission requires a DasProbeActor");
    Actor->EmitDasSignal(Value);
}
