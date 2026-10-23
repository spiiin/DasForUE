// Called only while constructing a candidate VM; snapshots never modify the live VM.
TMap<FString, FDasPersistentGlobalValue> SnapshotPersistentGlobals(const das::ProgramPtr& P, FUnrealDasContext& C)
{
    auto Values = C.PersistentGlobals;
    P->library.foreach (
        [&](das::Module* M)
        {
            M->globals.foreach (
                [&](das::Variable* V)
                {
                    if (!V->annotation.find("ue_state", das::Type::tBool))
                        return;
                    void* Address = C.PersistentGlobalStorage(V->getMangledNameHash());
                    if (!Address)
                        throw std::runtime_error("Persistent state has no VM storage");
                    FDasPersistentGlobalValue Value;
                    Value.Kind = V->type->baseType;
                    switch (Value.Kind)
                    {
                    case das::Type::tInt:
                        Value.Int = *static_cast<int32*>(Address);
                        break;
                    case das::Type::tBool:
                        Value.Bool = *static_cast<bool*>(Address);
                        break;
                    case das::Type::tFloat:
                        Value.Number = *static_cast<float*>(Address);
                        break;
                    case das::Type::tDouble:
                        Value.Number = *static_cast<double*>(Address);
                        break;
                    case das::Type::tString:
                    {
                        const char* Text = *static_cast<char**>(Address);
                        Value.String = Text ? UTF8_TO_TCHAR(Text) : TEXT("");
                        break;
                    }
                    default:
                        throw std::runtime_error("Unsupported persistent state storage");
                    }
                    Values.Add(DSLStateKey(V), MoveTemp(Value));
                });
            return true;
        },
        "*");
    return Values;
}
template <class T, das::Type Kind> T InitializePersistentGlobal(const char* Key, T Default, das::Context* C)
{
    if (C != ActiveContext || !Key || !*Key)
        C->throw_error("Persistent state requires the active context");
    auto* Existing = ActiveContext->PersistentGlobals.Find(UTF8_TO_TCHAR(Key));
    if (Existing && Existing->Kind != Kind)
        C->throw_error("ue_state type changed; rename the variable or release its script session");
    if (!Existing)
    {
        FDasPersistentGlobalValue Value;
        Value.Kind = Kind;
        if constexpr (Kind == das::Type::tInt)
            Value.Int = Default;
        else if constexpr (Kind == das::Type::tBool)
            Value.Bool = Default;
        else
            Value.Number = Default;
        ActiveContext->PersistentGlobals.Add(UTF8_TO_TCHAR(Key), MoveTemp(Value));
        return Default;
    }
    if constexpr (Kind == das::Type::tInt)
        return Existing->Int;
    else if constexpr (Kind == das::Type::tBool)
        return Existing->Bool;
    else
        return T(Existing->Number);
}
char* InitializePersistentString(const char* Key, const char* Default, das::Context* C)
{
    if (C != ActiveContext || !Key || !*Key)
        C->throw_error("Persistent state requires the active context");
    auto* Existing = ActiveContext->PersistentGlobals.Find(UTF8_TO_TCHAR(Key));
    if (Existing && Existing->Kind != das::Type::tString)
        C->throw_error("ue_state type changed; rename the variable or release its script session");
    if (!Existing)
    {
        FDasPersistentGlobalValue Value;
        Value.Kind = das::Type::tString;
        Value.String = Default ? UTF8_TO_TCHAR(Default) : TEXT("");
        Existing = &ActiveContext->PersistentGlobals.Add(UTF8_TO_TCHAR(Key), MoveTemp(Value));
    }
    return C->allocateString(das::string(TCHAR_TO_UTF8(*Existing->String)), nullptr);
}
FDasObjectHandle ClassDefaultObject(const char* Base, FDasClassHandle Concrete, const char* Id, bool HasConcrete,
                                    das::Context* C)
{
    RequireObjectContext(C);
    if (!ActiveContext->Self.IsValid() && !ActiveContext->bLibraryScope)
        C->throw_error(
            "CDO access requires an initialized owner or library call; unavailable in global initialization");
    if (HasConcrete && !Concrete.Token)
        C->throw_error("Concrete class handle is null");
    auto* Class = HasConcrete ? Cast<UClass>(ResolveObject(FDasObjectHandle{Concrete.Token}, C))
                              : (Base ? FindObject<UClass>(nullptr, UTF8_TO_TCHAR(Base)) : nullptr);
    if (!Class || Class->HasAnyClassFlags(CLASS_Interface | CLASS_Deprecated | CLASS_NewerVersionExists))
        C->throw_error("Class default requires a loaded object class");
    Class = CheckedClass(Class, Base, Id, C);
    return MakeObjectHandle(Class->GetDefaultObject(), Class, C);
}

FDasObjectHandle SubsystemObject(const char* Base, FDasClassHandle Concrete, const char* Id, bool HasConcrete,
                                 das::Context* C)
{
    RequireObjectContext(C);
    auto* Owner = ResolveWorldContextObject(C);
    auto* Class = HasConcrete ? Cast<UClass>(ResolveObject(FDasObjectHandle{Concrete.Token}, C))
                              : (Base ? FindObject<UClass>(nullptr, UTF8_TO_TCHAR(Base)) : nullptr);
    Class = CheckedClass(Class, Base, Id, C);
    UObject* Result = nullptr;
    if (Class->IsChildOf(UWorldSubsystem::StaticClass()))
    {
        auto* World = Owner->GetWorld();
        if (World && World->IsGameWorld() && !World->bIsTearingDown)
            Result = World->GetSubsystemBase(Class);
    }
    else if (Class->IsChildOf(UGameInstanceSubsystem::StaticClass()))
    {
        auto* GI = Cast<UGameInstance>(Owner);
        if (auto* S = Cast<UGameInstanceSubsystem>(Owner))
            GI = S->GetGameInstance();
        if (!GI && Owner->GetWorld())
            GI = Owner->GetWorld()->GetGameInstance();
        if (GI)
            Result = GI->GetSubsystemBase(Class);
    }
    else
        C->throw_error("ue_subsystem requires WorldSubsystem or GameInstanceSubsystem class");
    if (!IsValid(Result))
        C->throw_error("subsystem is unavailable: preload its class before the owner collection initializes");
    if ((Cast<UDasWorldSubsystem>(Result) && !CastChecked<UDasWorldSubsystem>(Result)->IsDasActive()) ||
        (Cast<UDasGameInstanceSubsystem>(Result) && !CastChecked<UDasGameInstanceSubsystem>(Result)->IsDasActive()))
        C->throw_error("subsystem is deinitialized");
    return MakeObjectHandle(Result, Class, C);
}
