// Context-local immutable snapshots own FText, including localization history.
FDasTextValue MakeDSLText(const FText& Value, das::Context* C)
{
    if (!ActiveContext || ActiveContext != C)
        C->throw_error("text requires active context");
    const FTextSnapshot Snapshot(Value);
    if (Snapshot.IdenticalTo(FText::GetEmpty()))
        return {};
    for (const auto& V : ActiveContext->TextValues)
        if (Snapshot.IdenticalTo(V.Value))
            return {V.Key};
    if (ActiveContext->TextValues.Num() >= 4096)
        C->throw_error("text snapshot limit reached");
    static uint64 Next = 1;
    const uint64 Token = Next++;
    ActiveContext->TextValues.Add(Token, Value);
    return {Token};
}
const FText& ResolveDSLText(FDasTextValue Value, das::Context* C)
{
    if (!ActiveContext || ActiveContext != C)
        C->throw_error("text requires active context");
    if (!Value.Token)
        return FText::GetEmpty();
    const auto* V = ActiveContext->TextValues.Find(Value.Token);
    if (!V)
        C->throw_error("invalid or foreign text value");
    return *V;
}
FDasTextValue DSLTextCreate(const char* Namespace, const char* Key, const char* Source, das::Context* C)
{
    const FString N = Namespace ? UTF8_TO_TCHAR(Namespace)
                                : TEXT(""), K = Key ? UTF8_TO_TCHAR(Key)
                                                    : TEXT(""), S = Source ? UTF8_TO_TCHAR(Source) : TEXT("");
    if (N.IsEmpty() || K.IsEmpty() || N.Len() > 1024 || K.Len() > 1024 || S.Len() > 65536)
        C->throw_error("localized text requires bounded namespace, key and source");
    return MakeDSLText(FText::AsLocalizable_Advanced_LocText(*N, *K, *S), C);
}
FDasTextValue DSLTextInvariant(const char* Value, das::Context* C)
{
    const FString S = Value ? UTF8_TO_TCHAR(Value) : TEXT("");
    if (S.Len() > 65536)
        C->throw_error("text source too long");
    return MakeDSLText(FText::AsCultureInvariant(S), C);
}
char* DSLTextToString(FDasTextValue V, das::Context* C)
{
    return CopyDSLString(ResolveDSLText(V, C).ToString(), C);
}
char* DSLTextNamespace(FDasTextValue V, das::Context* C)
{
    const auto S = FTextInspector::GetNamespace(ResolveDSLText(V, C));
    return CopyDSLString(S.IsSet() ? S.GetValue() : FString(), C);
}
char* DSLTextKey(FDasTextValue V, das::Context* C)
{
    const auto S = FTextInspector::GetKey(ResolveDSLText(V, C));
    return CopyDSLString(S.IsSet() ? S.GetValue() : FString(), C);
}
bool DSLTextIsInvariant(FDasTextValue V, das::Context* C)
{
    return ResolveDSLText(V, C).IsCultureInvariant();
}
