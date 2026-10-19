// Search is a bounded, no-load snapshot. Public DAS returns value IDs, never this token.
FName PrimaryName(const char* Text, das::Context* C)
{
    const FString Value = Text ? UTF8_TO_TCHAR(Text) : TEXT("");
    if (Value.IsEmpty() || Value.Len() > 256 || Value.Equals(TEXT("None"), ESearchCase::IgnoreCase))
        C->throw_error("Primary Asset type/name must contain 1..256 characters and cannot be None");
    for (TCHAR Ch : Value)
        if (FChar::IsWhitespace(Ch) || Ch == ':' || Ch == '/' || Ch == '\\' || Ch < 32)
            C->throw_error("Primary Asset type/name cannot contain whitespace, paths or colons");
    return FName(*Value);
}
UAssetManager& PrimaryManager(das::Context* C)
{
    GameplayWorld(C);
    auto* M = UAssetManager::GetIfInitialized();
    if (!M)
        C->throw_error("AssetManager is not initialized");
    return *M;
}
FPrimaryAssetId PrimaryId(const char* Type, const char* Name, das::Context* C)
{
    return FPrimaryAssetId(FPrimaryAssetType(PrimaryName(Type, C)), PrimaryName(Name, C));
}
bool ScriptPrimaryValid(const char* Type, const char* Name, das::Context* C)
{
    RequireObjectContext(C);
    PrimaryId(Type, Name, C);
    return true;
}
uint64 NextPrimaryQuery = 1;
uint64 ScriptPrimaryFind(const char* Type, das::Context* C)
{
    auto& M = PrimaryManager(C);
    if (ActiveContext->PrimaryQueries.Num() >= 8)
        C->throw_error("Primary Asset query limit reached");
    TArray<FPrimaryAssetId> Ids;
    M.GetPrimaryAssetIdList(FPrimaryAssetType(PrimaryName(Type, C)), Ids);
    if (Ids.Num() > 4096)
        C->throw_error("Primary Asset query exceeds 4096 results");
    Ids.Sort([](const auto& A, const auto& B) { return A.ToString() < B.ToString(); });
    const auto Token = NextPrimaryQuery++;
    ActiveContext->PrimaryQueries.Add(Token, MoveTemp(Ids));
    return Token;
}
const TArray<FPrimaryAssetId>& PrimaryQuery(uint64 Token, das::Context* C)
{
    RequireObjectContext(C);
    auto* Q = ActiveContext->PrimaryQueries.Find(Token);
    if (!Q)
        C->throw_error("Invalid/foreign Primary Asset query");
    return *Q;
}
int32 ScriptPrimaryQueryCount(uint64 Token, das::Context* C)
{
    return PrimaryQuery(Token, C).Num();
}
const char* ScriptPrimaryQueryName(uint64 Token, int32 Index, das::Context* C)
{
    const auto& Q = PrimaryQuery(Token, C);
    if (!Q.IsValidIndex(Index))
        C->throw_error("Primary Asset query index out of range");
    return CopyDSLString(Q[Index].PrimaryAssetName.ToString(), C);
}
void ScriptPrimaryQueryRelease(uint64 Token, das::Context* C)
{
    RequireObjectContext(C);
    ActiveContext->PrimaryQueries.Remove(Token);
}
bool ScriptPrimaryExists(const char* Type, const char* Name, das::Context* C)
{
    return !PrimaryManager(C).GetPrimaryAssetPath(PrimaryId(Type, Name, C)).IsNull();
}
const char* ScriptPrimaryPath(const char* Type, const char* Name, das::Context* C)
{
    return CopyDSLString(PrimaryManager(C).GetPrimaryAssetPath(PrimaryId(Type, Name, C)).ToString(), C);
}
FDasObjectHandle ScriptPrimaryObject(const char* Type, const char* Name, das::Context* C)
{
    auto& M = PrimaryManager(C);
    const auto Id = PrimaryId(Type, Name, C);
    return MakeObjectHandle(ActiveContext->PrimaryAssets.Contains(Id) ? M.GetPrimaryAssetObject(Id) : nullptr,
                            UObject::StaticClass(), C);
}
void ScriptPrimaryLoad(const char* Request, const char* Type, const char* Name, const das::TArray<char*>& InputBundles,
                       const char* Handler, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    auto& M = PrimaryManager(C);
    const auto Id = PrimaryId(Type, Name, C);
    const auto N = TimerName(Request, C);
    const FString Entry = Handler ? UTF8_TO_TCHAR(Handler) : TEXT("");
    int32 Count = 0;
    for (const auto& B : S->Bindings)
        if (B->AsyncKind == 2)
        {
            ++Count;
            if (B->AsyncName == N)
                C->throw_error("Primary Asset request name is already pending");
        }
    if (Count >= 32 || (!ActiveContext->PrimaryAssets.Contains(Id) && ActiveContext->PrimaryAssets.Num() >= 32))
        C->throw_error("Primary Asset owner/request limit reached");
    auto Contract = FDasCallContract::Scalar(EDasMethodSignature::Void);
    Contract.Inputs.Add(EDasMethodSignature::Object);
    Contract.InputClasses.Add(TEXT("/Script/CoreUObject.Object"));
    if (!VerifyContract(S->Live->Program, Entry, Contract))
        C->throw_error("Primary Asset callback must be exported (UObjectHandle):void");
    if (InputBundles.size > 32)
        C->throw_error("Primary Asset bundle count exceeds 32");
    TArray<FName> Bundles;
    for (uint32 I = 0; I < InputBundles.size; ++I)
        Bundles.AddUnique(PrimaryName(InputBundles[I], C));
    Bundles.Sort(FNameLexicalLess());
    const bool Known = !M.GetPrimaryAssetPath(Id).IsNull();
    TSharedPtr<FDasPrimaryAssetLease> Lease;
    if (Known)
    {
        for (FName Bundle : Bundles)
            if (!M.GetAssetBundleEntry(Id, Bundle).IsValid())
                C->throw_error("Unknown Primary Asset bundle");
        FString Error;
        Lease = AcquireDasPrimaryAsset(Id, Bundles, Error);
        if (!Lease)
            C->throw_error(TCHAR_TO_UTF8(*Error));
    }
    auto B = MakeUnique<FManagedSubscription>();
    B->Id = NextSubscriptionId++;
    B->Handler = Entry;
    B->Kind = EDasMethodSignature::Object;
    B->PayloadPath = TEXT("/Script/CoreUObject.Object");
    B->AsyncKind = 2;
    B->AsyncName = N;
    B->PrimaryAsset = Id;
    B->AsyncProxy.Reset(NewObject<UDasAsyncAssetProxy>());
    B->Source = B->AsyncProxy.Get();
    const uint64 Subscription = B->Id;
    S->Bindings.Add(MoveTemp(B));
    S->RequiredEntries.Add(Entry, Contract);
    if (!Known)
    {
        FModuleManager::GetModuleChecked<FDasRuntimeModule>(
            TEXT("DasRuntime")).CompletePrimaryAssetRequest(Subscription, false);
        return;
    }
    ActiveContext->PrimaryAssets.Add(Id, Lease);
    FAssetManagerLoadParams Params;
    Params.OnComplete = FStreamableDelegateWithHandle::CreateLambda(
        [Subscription](TSharedPtr<FStreamableHandle>)
        {
            if (auto* R = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
                R->CompletePrimaryAssetRequest(Subscription, true);
        });
    Params.OnCancel = FStreamableDelegateWithHandle::CreateLambda(
        [Subscription](TSharedPtr<FStreamableHandle>)
        {
            if (auto* R = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
                R->CompletePrimaryAssetRequest(Subscription, false);
        });
    auto Handle = M.LoadPrimaryAsset(Id, Bundles, MoveTemp(Params));
    Lease->EngineHandle = M.GetPrimaryAssetHandle(Id);
    if (auto* Binding = S->Bindings.FindByPredicate([&](const auto& X) { return X->Id == Subscription; }))
        (*Binding)->StreamHandle = Handle;
}
bool ScriptPrimaryPending(const char* Request, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    const auto N = TimerName(Request, C);
    return S->Bindings.ContainsByPredicate([&](const auto& B) { return B->AsyncKind == 2 && B->AsyncName == N; });
}
bool ScriptPrimaryCancel(const char* Request, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    const auto N = TimerName(Request, C);
    return S->Bindings.RemoveAll([&](const auto& B) { return B->AsyncKind == 2 && B->AsyncName == N; }) > 0;
}
bool ScriptPrimaryUnload(const char* Type, const char* Name, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    PrimaryManager(C);
    const auto Id = PrimaryId(Type, Name, C);
    S->Bindings.RemoveAll([&](const auto& B) { return B->AsyncKind == 2 && B->PrimaryAsset == Id; });
    return ActiveContext->PrimaryAssets.Remove(Id) > 0;
}
