// Native I/O outlives cancellation of delivery. A slot remains leased until the
// engine completion, even if its originating VM/session has been released.
TMap<FString, uint64> SaveGameSlotLeases;
TMap<uint64, FString> SaveGameIO;
FString SaveGameSlot(const char* Text, int32 User, das::Context* C)
{
    GameplayWorld(C);
    const FString Slot = Text ? UTF8_TO_TCHAR(Text) : TEXT("");
    if (User < 0 || Slot.IsEmpty() || Slot.Len() > 128)
        C->throw_error("SaveGame requires a slot name of 1..128 characters and nonnegative user index");
    for (const TCHAR Ch : Slot)
        if (!((Ch >= 'A' && Ch <= 'Z') || (Ch >= 'a' && Ch <= 'z') || (Ch >= '0' && Ch <= '9') || Ch == '_' ||
              Ch == '-'))
            C->throw_error("SaveGame slot permits letters, digits, underscore and hyphen; no paths");
    return Slot;
}
FString SaveGameSlotKey(const FString& Slot, int32 User)
{
    return Slot.ToLower();
} // Some platforms ignore UserIndex; serialize conservatively.
void CheckSaveGameSlotIdle(const FString& Slot, int32 User, das::Context* C)
{
    if (SaveGameSlotLeases.Contains(SaveGameSlotKey(Slot, User)))
        C->throw_error("SaveGame slot has an unfinished native I/O operation");
}
USaveGame* ResolveSaveGame(const FDasObjectHandle& H, das::Context* C)
{
    auto* Game = Cast<USaveGame>(ResolveObject(H, C));
    if (!Game || Game->IsTemplate())
        C->throw_error("SaveGame requires a live non-template USaveGame object");
    return Game;
}
FDasObjectHandle HoldSaveGame(USaveGame* Game, das::Context* C)
{
    if (!Game)
        return {};
    if (!IsValid(Game) || Game->GetClass()->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists))
        C->throw_error("SaveGame class is obsolete");
    if (!ActiveContext->SaveGames.ContainsByPredicate([&](const auto& P) { return P.Get() == Game; }))
    {
        if (ActiveContext->SaveGames.Num() >= 32)
            C->throw_error("SaveGame owned object limit reached; release unused objects");
        ActiveContext->SaveGames.Emplace(Game);
    }
    return MakeObjectHandle(Game, USaveGame::StaticClass(), C);
}
FDasObjectHandle ScriptSaveGameCreate(const FDasClassHandle& H, das::Context* C)
{
    GameplayWorld(C);
    auto* Class = Cast<UClass>(ResolveObject({H.Token}, C));
    if (!Class || !Class->IsChildOf(USaveGame::StaticClass()) ||
        Class->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists))
        C->throw_error("SaveGame creation requires a concrete USaveGame class");
    if (ActiveContext->SaveGames.Num() >= 32)
        C->throw_error("SaveGame owned object limit reached");
    return HoldSaveGame(UGameplayStatics::CreateSaveGameObject(Class), C);
}
FDasObjectHandle ScriptSaveGameCreatePath(const char* Path, das::Context* C)
{
    GameplayWorld(C);
    const FString Name = Path ? UTF8_TO_TCHAR(Path) : TEXT("");
    if (Name.Len() > 1024 || !FPackageName::IsValidObjectPath(Name))
        C->throw_error("SaveGame requires a class object path");
    auto* Class = LoadObject<UClass>(nullptr, *Name);
    if (!Class)
        C->throw_error("SaveGame class could not be loaded");
    return ScriptSaveGameCreate({MakeObjectHandle(Class, UClass::StaticClass(), C).Token}, C);
}
bool ScriptSaveGameSave(const FDasObjectHandle& H, const char* Slot, int32 User, das::Context* C)
{
    const FString Name = SaveGameSlot(Slot, User, C);
    CheckSaveGameSlotIdle(Name, User, C);
    return UGameplayStatics::SaveGameToSlot(ResolveSaveGame(H, C), Name, User);
}
FDasObjectHandle ScriptSaveGameLoad(const char* Slot, int32 User, das::Context* C)
{
    const FString Name = SaveGameSlot(Slot, User, C);
    CheckSaveGameSlotIdle(Name, User, C);
    if (ActiveContext->SaveGames.Num() >= 32)
        C->throw_error("SaveGame owned object limit reached");
    return HoldSaveGame(UGameplayStatics::LoadGameFromSlot(Name, User), C);
}
bool ScriptSaveGameExists(const char* Slot, int32 User, das::Context* C)
{
    return UGameplayStatics::DoesSaveGameExist(SaveGameSlot(Slot, User, C), User);
}
bool ScriptSaveGameDelete(const char* Slot, int32 User, das::Context* C)
{
    const FString Name = SaveGameSlot(Slot, User, C);
    CheckSaveGameSlotIdle(Name, User, C);
    return UGameplayStatics::DeleteGameInSlot(Name, User);
}
bool ScriptSaveGameRelease(const FDasObjectHandle& H, das::Context* C)
{
    RequireObjectContext(C);
    auto* Game = TryObject(H, C);
    return ActiveContext->SaveGames.RemoveAll([&](const auto& P) { return P.Get() == Game; }) > 0;
}
bool ScriptSaveGameValid(const FDasObjectHandle& H, das::Context* C)
{
    RequireObjectContext(C);
    return Cast<USaveGame>(TryObject(H, C)) != nullptr;
}
void ScriptSaveGameAsync(const char* Request, const FDasObjectHandle* Save, const char* Slot, int32 User,
                         const char* Handler, das::Context* C)
{
    auto* Session = ManagedOwnerSession(C);
    const FString Name = SaveGameSlot(Slot, User, C), Key = SaveGameSlotKey(Name, User), N = TimerName(Request, C),
                  Entry = Handler ? UTF8_TO_TCHAR(Handler) : TEXT("");
    if (ActiveContext->bEnding)
        C->throw_error("SaveGame async operations are forbidden during EndPlay");
    CheckSaveGameSlotIdle(Name, User, C);
    if (SaveGameIO.Num() >= 256)
        C->throw_error("SaveGame native I/O limit reached");
    int32 Count = 0;
    for (const auto& Binding : Session->Bindings)
        if (Binding->SaveGameKind)
        {
            ++Count;
            if (Binding->SaveGameName == N)
                C->throw_error("SaveGame request name is already pending");
        }
    if (Count >= 32 || (!Save && ActiveContext->SaveGames.Num() >= 32))
        C->throw_error("SaveGame request/object limit reached");
    USaveGame* Object = Save ? ResolveSaveGame(*Save, C) : nullptr;
    const auto Kind = Save ? EDasMethodSignature::Bool : EDasMethodSignature::Object;
    auto Contract = FDasCallContract::Scalar(EDasMethodSignature::Void);
    Contract.Inputs.Add(Kind);
    if (!Save)
        Contract.InputClasses.Add(TEXT("/Script/CoreUObject.Object"));
    if (!VerifyContract(Session->Live->Program, Entry, Contract))
        C->throw_error("SaveGame callback must be exported (bool):void for save or (UObjectHandle):void for load");
    auto Binding = MakeUnique<FManagedSubscription>();
    Binding->Id = NextSubscriptionId++;
    Binding->Kind = Kind;
    Binding->SaveGameKind = Save ? 1 : 2;
    Binding->SaveGameName = N;
    Binding->Handler = Entry;
    if (!Save)
        Binding->PayloadPath = TEXT("/Script/CoreUObject.Object");
    Binding->SaveGameProxy.Reset(NewObject<UDasSaveGameProxy>());
    Binding->SaveGameProxy->SubscriptionId = Binding->Id;
    Binding->SaveGameProxy->SaveObject = Object;
    Binding->Source = Binding->SaveGameProxy.Get();
    const uint64 Id = Binding->Id;
    Session->Bindings.Add(MoveTemp(Binding));
    Session->RequiredEntries.Add(Entry, Contract);
    SaveGameIO.Add(Id, Key);
    SaveGameSlotLeases.Add(Key, Id);
    if (Save)
        UGameplayStatics::AsyncSaveGameToSlot(
            Object, Name, User,
            FAsyncSaveGameToSlotDelegate::CreateLambda(
                [Id](const FString&, int32, bool Success)
                {
                    if (auto* Runtime = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
                        Runtime->CompleteSaveGameRequest(Id, nullptr, Success);
                }));
    else
        UGameplayStatics::AsyncLoadGameFromSlot(
            Name, User,
            FAsyncLoadGameFromSlotDelegate::CreateLambda(
                [Id](const FString&, int32, USaveGame* Loaded)
                {
                    if (auto* Runtime = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
                        Runtime->CompleteSaveGameRequest(Id, Loaded, Loaded != nullptr);
                }));
}
void ScriptSaveGameSaveAsync(const char* Request, const FDasObjectHandle& Game, const char* Slot, int32 User,
                             const char* Handler, das::Context* C)
{
    ScriptSaveGameAsync(Request, &Game, Slot, User, Handler, C);
}
void ScriptSaveGameLoadAsync(const char* Request, const char* Slot, int32 User, const char* Handler, das::Context* C)
{
    ScriptSaveGameAsync(Request, nullptr, Slot, User, Handler, C);
}
bool ScriptSaveGameCancel(const char* Request, das::Context* C)
{
    auto* Session = ManagedOwnerSession(C);
    const auto Name = TimerName(Request, C);
    return Session->Bindings.RemoveAll([&](const auto& B) { return B->SaveGameKind && B->SaveGameName == Name; }) > 0;
}
bool ScriptSaveGamePending(const char* Request, das::Context* C)
{
    auto* Session = ManagedOwnerSession(C);
    const auto Name = TimerName(Request, C);
    return Session->Bindings.ContainsByPredicate([&](const auto& B)
                                                 { return B->SaveGameKind && B->SaveGameName == Name; });
}
