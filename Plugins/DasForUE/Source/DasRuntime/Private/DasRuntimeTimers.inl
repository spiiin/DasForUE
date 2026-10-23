// Part of DasRuntime.cpp: Compiled contexts, managed subscriptions and owner-managed timers.
// Included in place; declaration order matters.
FString Errors(const das::ProgramPtr& Program, das::TextWriter& Output)
{
    if (Program)
        for (const auto& Error : Program->errors)
            Output << TCHAR_TO_UTF8(*DSLDiagnosticCompilerError(Program, Error)) << "\n";
    const auto Text = Output.str();
    return UTF8_TO_TCHAR(Text.c_str());
}

// Declaration order keeps compiler owners alive until the context is destroyed.
struct FCompiledContext
{
    FString DSLDeclaration;
    TSharedPtr<FDSLAsyncPhysicsState> Physics;
    bool bPhysicsOwner = false, bPhysicsForeignSession = false;
    das::ModuleGroup Modules;
    das::FileAccessPtr Access;
    das::ProgramPtr Program;
    TUniquePtr<FUnrealDasContext> Context;
    das::SimFunction* Function = nullptr;
    das::SimFunction* Initializer = nullptr;
    das::SimFunction* Construction = nullptr;
    das::SimFunction* BeginPlay = nullptr;
    das::SimFunction* Tick = nullptr;
    das::SimFunction* EndPlay = nullptr;
    // Main entry points already found and checked against a contract; dropped with this program on reload.
    // The key includes the widget contract flags VerifyContract reads.
    struct FVerifiedEntry
    {
        das::SimFunction* Function = nullptr;
        FDasCallContract Contract;
        uint8 Flags = 0;
    };
    TMap<FString, FVerifiedEntry> VerifiedEntries;
};
#include "DasAsyncPhysicsDSLRuntime.inl"

struct FManagedSubscription
{
    uint64 Id = 0;
    TWeakObjectPtr<UObject> Source;
    FName Event;
    FString Handler;
    EDasMethodSignature Kind = EDasMethodSignature::Int;
    FString PayloadPath;
    TStrongObjectPtr<UDasIntDelegateProxy> Proxy;
    FName ProxyFunction;
    FName DirectFunction;
    TWeakObjectPtr<UObject> DirectTarget;
    int32 SaveGameKind = 0;
    FString SaveGameName;
    TStrongObjectPtr<UDasSaveGameProxy> SaveGameProxy;
    FPrimaryAssetId PrimaryAsset;
    int32 AsyncKind = 0;
    FString AsyncName;
    TStrongObjectPtr<UDasAsyncAssetProxy> AsyncProxy;
    TSharedPtr<FStreamableHandle> StreamHandle;
    int32 LatentKind = 0;
    FString LatentName;
    TWeakObjectPtr<UWorld> LatentWorld;
    TStrongObjectPtr<UDasLatentProxy> LatentProxy;
    bool InputConsume = true, InputPaused = false;
    int32 InputKind = 0, InputHandle = INDEX_NONE;
    EInputEvent InputPhase = IE_Pressed;
    uint32 EnhancedHandle = 0;
    ETriggerEvent EnhancedPhase = ETriggerEvent::Triggered;
    TStrongObjectPtr<UDasEnhancedInputProxy> EnhancedProxy;
    TStrongObjectPtr<UObject> InputAsset;
    FString AIMoveName;
    TStrongObjectPtr<UDasAIMoveProxy> AIMoveProxy;
    FString EffectName;
    TStrongObjectPtr<UDasEffectProxy> EffectProxy;
    FString MontageName;
    TStrongObjectPtr<UDasMontageProxy> MontageProxy;
    ~FManagedSubscription()
    {
        if (!DirectFunction.IsNone())
        {
            if (auto* O = Source.Get())
                if (auto* P = FindFProperty<FMulticastDelegateProperty>(O->GetClass(), Event))
                {
                    FScriptDelegate D;
                    D.BindUFunction(DirectTarget.Get(), DirectFunction);
                    P->RemoveDelegate(D, O, P->ContainerPtrToValuePtr<void>(O));
                }
            return;
        }
        if (AIMoveProxy.IsValid())
        {
            AIMoveProxy->Deactivate();
            return;
        }
        if (EffectProxy.IsValid())
        {
            EffectProxy->Deactivate();
            return;
        }
        if (MontageProxy.IsValid())
        {
            MontageProxy->Deactivate();
            return;
        }
        if (AsyncKind == 2)
            return; // AssetManager load state is owned by VM leases, not callback registrations.
        if (AsyncKind == 1)
        {
            if (StreamHandle.IsValid())
            {
                if (!StreamHandle->HasLoadCompleted())
                    StreamHandle->CancelHandle();
                else
                    StreamHandle->ReleaseHandle();
            }
            return;
        }
        if (LatentKind)
        {
            if (auto* W = LatentWorld.Get())
                if (LatentProxy.IsValid())
                    W->GetLatentActionManager().RemoveActionsForObject(TWeakObjectPtr<UObject>(LatentProxy.Get()));
            return;
        }
        if (InputKind == 3)
        {
            if (auto* Input = Cast<UEnhancedInputComponent>(Source.Get()))
                Input->RemoveBindingByHandle(EnhancedHandle);
            return;
        }
        if (InputKind)
        {
            if (auto* Input = Cast<UInputComponent>(Source.Get()))
            {
                if (InputKind == 1)
                    Input->RemoveActionBindingForHandle(InputHandle);
                else
                    Input->AxisBindings.RemoveAll([&](const FInputAxisBinding& B)
                                                  { return B.AxisDelegate.IsBoundToObject(Proxy.Get()); });
            }
            return;
        }
        if (UObject* Object = Source.Get())
            if (auto* Property = ScalarEventProperty(Object, Event, Kind))
            {
                FScriptDelegate Delegate;
                Delegate.BindUFunction(Proxy.Get(), ProxyFunction);
                Property->RemoveDelegate(Delegate, Object, Property->ContainerPtrToValuePtr<void>(Object));
            }
    }
};

struct FManagedInputContext
{
    TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem;
    TStrongObjectPtr<UInputMappingContext> Context;
    int32 Priority = 0;
    ~FManagedInputContext()
    {
        if (auto* S = Subsystem.Get())
        {
            int32 Actual = 0;
            if (Context.IsValid() && S->HasMappingContext(Context.Get(), Actual) && Actual == Priority)
            {
                FModifyContextOptions Options;
                Options.bNotifyUserSettings = false;
                S->RemoveMappingContext(Context.Get(), Options);
            }
        }
    }
};

struct FInputRecipe
{
    int32 Kind = 0;
    FName Name;
    FString Handler;
    EInputEvent Phase = IE_Pressed;
    ETriggerEvent Trigger = ETriggerEvent::Triggered;
    bool Consume = true, Paused = false;
    TStrongObjectPtr<UObject> Asset;
};
struct FInputContextRecipe
{
    TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem;
    TStrongObjectPtr<UInputMappingContext> Context;
    int32 Priority = 0;
};

struct FPersistentSession
{
    TWeakObjectPtr<UObject> Owner;
    TWeakObjectPtr<UWorld> World;
    FString Path;
    FSourceSet AttemptedSources;
    // Hot reload: sources were last compared with disk at this epoch and time (see SourceCheckFor).
    uint64 CheckedEpoch = 0;
    double CheckedAt = 0;
    TMap<FString, FDasCallContract> RequiredEntries;
    TSet<FString> NativeInputEntries, GeometrySnapshotEntries, InputSnapshotEntries, WidgetReplyEntries,
        WidgetPaintEntries, WidgetDragEntries, FocusNotificationEntries;
    FString ApprovedDSLDeclaration;
    bool bDefaultsUpdateApproved = false;
    FString AttemptedEntry;
    FDasCallContract AttemptedContract;
    TUniquePtr<FCompiledContext> Live;
    FString ReloadError;
    uint32 Revision = 0;
    bool bBeginDelivered = false;
    bool bEndDelivered = false;
    bool bReleaseRequested = false;
    TArray<TUniquePtr<FManagedSubscription>> Bindings;
    TArray<TUniquePtr<FManagedInputContext>> InputContexts;
    TArray<FInputRecipe> InputRecipes;
    TArray<FInputContextRecipe> ContextRecipes;
    bool bRestoringInput = false;
    double NextInputRestore = 0;
    FString InputRestoreError;
#if WITH_EDITOR
    bool bReinstanceOwnerInScope = false, bReinstanceOwnerChanged = false, bReinstanceTickEnabled = false;
    FString ReinstanceLifecyclePath;
#endif
};
// Delivery belongs to the UObject lifetime, independently of VM/session replacement.
enum class EInitializerDelivery : uint8
{
    Running,
    Complete,
    Failed
};
struct FInitializerDelivery
{
    EInitializerDelivery State = EInitializerDelivery::Running;
    FString Error;
};
TMap<TWeakObjectPtr<UObject>, TMap<FString, FInitializerDelivery>> InitializerDeliveries;
TArray<TUniquePtr<FPersistentSession>> Sessions;
// Hot reload source checks (FDasRuntimeModule::NotifyScriptsChanged).
uint64 ScriptSourceEpoch = 1;
TMap<FString, FString> NormalizedScriptPaths;
// ScriptPath: script name -> project or plugin path, with the time it was checked (game thread only).
struct FCachedScriptPath
{
    FString Path;
    double CheckedAt = 0;
};
TMap<FString, FCachedScriptPath> ScriptPathCache;
TAutoConsoleVariable<int32> CVarScriptChangeCheck(
    TEXT("das.ScriptChangeCheck"), -1,
    TEXT("When persistent script calls compare their sources with disk: -1 or 1 watch (after a reported change: ")
        TEXT("writes in this process, the editor file watcher, NotifyScriptsChanged; edits by other programs ")
        TEXT("within the poll interval), 0 every call (re-reads all sources), 2 never (no hot reload)."));
TAutoConsoleVariable<float> CVarScriptChangePollSeconds(
    TEXT("das.ScriptChangePollSeconds"), 1.f,
    TEXT("Watch mode: how often file size and time are compared with what scripts were compiled from, for edits ")
        TEXT("by other programs. Each file is checked once per interval for all sessions; bytes only on a change."));
#if WITH_EDITOR
TArray<TUniquePtr<FPersistentSession>> PreservedSessions;
bool bPreservingSessions = false;
#endif
// Current file stats for the watch-mode poll, shared by all sessions.
struct FPolledStat
{
    FFileStatData Stat;
    double CheckedAt = -1e9;
};
TMap<FString, FPolledStat> PolledStats;
const FString& NormalizeScriptPath(const FString& Path)
{
    if (const FString* Cached = NormalizedScriptPaths.Find(Path))
        return *Cached;
    FString Full = FPaths::ConvertRelativePathToFull(Path);
    FPaths::NormalizeFilename(Full);
    FPaths::CollapseRelativeDirectories(Full);
    return NormalizedScriptPaths.Add(Path, MoveTemp(Full));
}
// Sessions that read one of Files, or anything under Directories, compare their sources on the next call.
// Nothing named: every session does.
void MarkScriptsChanged(const TArray<FString>& Files, const TArray<FString>& Directories, bool bAddedOrRemoved)
{
    check(IsInGameThread());
    PolledStats.Reset();
    if (bAddedOrRemoved || !Directories.IsEmpty() || Files.IsEmpty())
        ScriptPathCache.Reset();
    if (Files.IsEmpty() && Directories.IsEmpty())
    {
        ++ScriptSourceEpoch;
        return;
    }
    TSet<FString> Changed;
    for (const FString& File : Files)
        Changed.Add(NormalizeScriptPath(File));
    TArray<FString> Under;
    for (const FString& Directory : Directories)
        Under.Add(NormalizeScriptPath(Directory) + TEXT("/"));
    auto Mark = [&](const TArray<TUniquePtr<FPersistentSession>>& List)
    {
        for (const auto& Session : List)
            for (const auto& Source : Session->AttemptedSources)
            {
                const FString& Path = NormalizeScriptPath(Source.Key);
                if (Changed.Contains(Path) ||
                    Under.ContainsByPredicate([&](const FString& Prefix) { return Path.StartsWith(Prefix); }))
                {
                    Session->CheckedEpoch = 0;
                    break;
                }
            }
    };
    Mark(Sessions);
#if WITH_EDITOR
    Mark(PreservedSessions);
#endif
}
// Reports script writes queued by the platform-file layer (DasScriptWriteTracker), on the game thread.
void DrainScriptWrites()
{
    TArray<FString> Files, Directories;
    bool bAddedOrRemoved = false;
    if (DasTakeScriptWrites(Files, Directories, bAddedOrRemoved))
        MarkScriptsChanged(Files, Directories, bAddedOrRemoved);
}

struct FManagedScriptTimer
{
    uint64 Id = 0;
    FString Name, Handler;
    FUnrealDasContext* Context = nullptr;
    TWeakObjectPtr<UObject> Owner;
    TWeakObjectPtr<UWorld> World;
    FTimerHandle Handle;
    bool Repeat = false;
    TSharedPtr<FDasCallbackPayload> Argument;
    ~FManagedScriptTimer()
    {
        if (auto* W = World.Get())
            W->GetTimerManager().ClearTimer(Handle);
    }
};
TMap<uint64, TUniquePtr<FManagedScriptTimer>> ScriptTimers;
TArray<uint64> PendingScriptTimers;
uint64 NextScriptTimer = 1;
bool bDrainingScriptTimers = false;
void DrainEvents();
void ClearSessionTimers(FPersistentSession& S, FUnrealDasContext* InlineReplacement = nullptr)
{
    if (!S.Live)
        return;
    TSet<FString> InlineEntries;
    TArray<FString> InlineFramePrefixes;
    if (InlineReplacement)
    {
        FDasAuthoringModule Model;
        FString Issue;
        if (FDasAuthoringModule::FromJson(S.Live->DSLDeclaration, Model, Issue))
            for (const auto& Class : Model.Classes)
                for (const auto& Method : Class.Methods)
                    if (Method.Name.StartsWith(TEXT("__ue_cb_")))
                    {
                        InlineEntries.Add(Method.EntryPoint);
                        InlineFramePrefixes.Add(Method.Name + TEXT("_frame_"));
                    }
    }
    for (auto I = ScriptTimers.CreateIterator(); I; ++I)
        if (I.Value()->Context == S.Live->Context.Get())
        {
            // Candidate RequiredEntries were verified before commit. Inline entries keep no stack closure;
            // their native payload, timer handle, pause state and queued ID can outlive the old VM.
            if (InlineReplacement && S.RequiredEntries.Contains(I.Value()->Handler) &&
                (InlineEntries.Contains(I.Value()->Handler) ||
                 InlineFramePrefixes.ContainsByPredicate([&](const FString& Prefix)
                                                         { return I.Value()->Handler.StartsWith(Prefix); })))
                I.Value()->Context = InlineReplacement;
            else
                I.RemoveCurrent();
        }
}
FPersistentSession* ManagedOwnerSession(das::Context* C)
{
    ResolveSelf(C);
    if (ConstructingObject)
        C->throw_error("managed callbacks are forbidden during ConstructionScript");
    if (ActiveContext->bCollectingSubscriptions)
        C->throw_error("managed callback operations are forbidden in bind_events");
    for (const auto& S : Sessions)
        if (S->Live && S->Live->Context.Get() == ActiveContext && !S->bReleaseRequested && !S->bEndDelivered)
            return S.Get();
    C->throw_error("managed callbacks require a live persistent owner session");
}
FString TimerName(const char* Name, das::Context* C)
{
    const FString N = Name ? UTF8_TO_TCHAR(Name) : TEXT("");
    if (N.IsEmpty() || N.Len() > 128)
        C->throw_error("timer name requires 1..128 characters");
    return N;
}
FManagedScriptTimer* FindScriptTimer(FPersistentSession* S, const FString& Name)
{
    for (auto& P : ScriptTimers)
        if (P.Value->Context == S->Live->Context.Get() && P.Value->Name == Name)
            return P.Value.Get();
    return nullptr;
}
void QueueScriptTimer(uint64 Id)
{
    if (!ScriptTimers.Contains(Id))
        return;
    if (!PendingScriptTimers.Contains(Id))
    {
        if (PendingScriptTimers.Num() >= 1024)
        {
            UE_LOG(LogDasRuntime, Warning, TEXT("Script timer dispatch queue limit reached"));
            return;
        }
        PendingScriptTimers.Add(Id);
    }
    DrainEvents();
}
void ScriptTimerStartImpl(const char* Name, float Seconds, const char* Handler, bool Repeat, das::Context* C,
                          FDasScalarValue* Argument, bool NextTick = false)
{
    auto* S = ManagedOwnerSession(C);
    if (ActiveContext->bEnding)
        C->throw_error("cannot start a timer during EndPlay");
    const FString N = TimerName(Name, C), Entry = Handler ? UTF8_TO_TCHAR(Handler) : TEXT("");
    if ((!NextTick && (!FMath::IsFinite(Seconds) || Seconds <= 0.f)) || Entry.IsEmpty() || Entry.Len() >= NAME_SIZE)
        C->throw_error("timer requires positive finite interval and handler");
    FDasCallContract Contract = FDasCallContract::Scalar(EDasMethodSignature::Void);
    if (Argument)
    {
        FString Issue;
        if (!ValidateDasCallbackPayload(*Argument, EDasCallbackUsage::Timer, Issue))
            C->throw_error(TCHAR_TO_UTF8(*Issue));
        Contract.Inputs.Add(static_cast<EDasMethodSignature>(Argument->Kind));
        if ((Argument->Kind == int32(EDasMethodSignature::Struct) ||
             Argument->Kind == int32(EDasMethodSignature::Enum) ||
             Argument->Kind == int32(EDasMethodSignature::Object) ||
             Argument->Kind == int32(EDasMethodSignature::Class)))
            Contract.InputClasses.Add(Argument->ObjectClass);
    }
    if (!VerifyContract(S->Live->Program, Entry, Contract))
        C->throw_error("timer handler must return void and match captured argument signature");
    auto* W = S->World.Get();
    if (!W || W != S->Owner->GetWorld())
        C->throw_error("timer requires owner's live world");
    auto* Previous = FindScriptTimer(S, N);
    int32 Count = 0;
    for (const auto& P : ScriptTimers)
        if (P.Value->Context == ActiveContext)
            ++Count;
    if (!Previous && (Count >= 64 || ScriptTimers.Num() >= 1024))
        C->throw_error("script timer count limit exceeded");
    if (Previous)
        ScriptTimers.Remove(Previous->Id);
    auto T = MakeUnique<FManagedScriptTimer>();
    T->Id = NextScriptTimer++;
    T->Name = N;
    T->Handler = Entry;
    T->Context = ActiveContext;
    T->Owner = S->Owner;
    T->World = W;
    T->Repeat = Repeat;
    if (Argument)
        T->Argument = MakeShared<FDasCallbackPayload>(MoveTemp(*Argument));
    const uint64 Id = T->Id;
    auto Delegate = FTimerDelegate::CreateLambda([Id]() { QueueScriptTimer(Id); });
    if (NextTick)
        T->Handle = W->GetTimerManager().SetTimerForNextTick(Delegate);
    else
        W->GetTimerManager().SetTimer(T->Handle, Delegate, Seconds, Repeat);
    ScriptTimers.Add(Id, MoveTemp(T));
    S->RequiredEntries.Add(Entry, Contract);
}
FDasCallHandle BeginCallbackFrame(const char* Path, das::Context* C)
{
    RequireObjectContext(C);
    auto* Frame = FindDasCallbackFrame(Path ? UTF8_TO_TCHAR(Path) : TEXT(""));
    auto* Function = DasCallbackFrameBuilder(Frame);
    if (!Function)
        C->throw_error("unknown callback frame");
    if (ActiveContext->Calls.Num() >= 256)
        C->throw_error("call frame limit reached");
    auto Call = MakeShared<FMarshalledCall>();
    Call->CallbackFramePath = Frame->GetPathName();
    Call->Target = ResolveSelf(C);
    Call->Function.Reset(Function);
    Call->Frame = MakeUnique<FStructOnScope>(Function);
    Call->Signature = DescribeCall(Function, C);
    const uint64 Token = NextCallHandle++;
    ActiveContext->Calls.Add(Token, MoveTemp(Call));
    return {Token};
}
void StartCallbackFrame(const char* N, float Seconds, const char* H, bool Repeat, const FDasCallHandle& Handle,
                        bool Next, das::Context* C)
{
    auto& Call = ResolveCall(Handle, C);
    auto* Type = FindDasCallbackFrame(Call.CallbackFramePath);
    if (!Type || Call.bExecuted)
        C->throw_error("invalid callback builder");
    FDasScalarValue Value;
    Value.Kind = 12;
    Value.ObjectClass = Type->GetPathName();
    Value.StructValue.InitializeAs(Type);
    for (TFieldIterator<FProperty> I(Type); I; ++I)
    {
        const FString Key = I->GetName().ToLower();
        if (!Call.Assigned.Contains(Key))
            C->throw_error("missing callback argument");
        auto* Source = Call.Function->FindPropertyByName(I->GetFName());
        FDasScalarValue Input;
        FString Error;
        if (!ReadDasPropertyValue(Source, Source->ContainerPtrToValuePtr<void>(Call.Frame->GetStructMemory()), Input,
                                  Error) ||
            !WriteDasPropertyValue(*I, I->ContainerPtrToValuePtr<void>(Value.StructValue.GetMutableMemory()), Input,
                                   Error))
            C->throw_error(TCHAR_TO_UTF8(*Error));
    }
    ScriptTimerStartImpl(N, Seconds, H, Repeat, C, &Value, Next);
    Call.bExecuted = true;
}
void StartCallbackFrameTimer(const char* N, float Seconds, const char* H, bool Repeat, const FDasCallHandle& Handle,
                             das::Context* C)
{
    StartCallbackFrame(N, Seconds, H, Repeat, Handle, false, C);
}
void NextCallbackFrameTimer(const char* N, const char* H, const FDasCallHandle& Handle, das::Context* C)
{
    StartCallbackFrame(N, 0.f, H, false, Handle, true, C);
}
void ScriptTimerStart(const char* N, float Seconds, const char* H, bool Repeat, das::Context* C)
{
    ScriptTimerStartImpl(N, Seconds, H, Repeat, C, nullptr);
}
void ScriptTimerStartArray(const char* N, float Seconds, const char* H, bool Repeat, const FDasStructValue& Value,
                           das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 12;
    V.StructValue = ResolveStructValue(Value, C);
    V.ObjectClass = V.StructValue.GetScriptStruct()->GetPathName();
    ScriptTimerStartImpl(N, Seconds, H, Repeat, C, &V);
}
FDasScalarValue ClassTimerValue(FDasClassHandle Value, const char* Path, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 15;
    V.ObjectClass = Path ? UTF8_TO_TCHAR(Path) : TEXT("");
    if (Value.Token)
    {
        V.ClassValue = Cast<UClass>(ResolveObject(FDasObjectHandle{Value.Token}, C));
        if (!V.ClassValue)
            C->throw_error("expected class capability");
    }
    return V;
}
void ScriptTimerStartClass(const char* N, float Seconds, const char* H, bool Repeat, FDasClassHandle Value,
                           const char* Path, das::Context* C)
{
    auto V = ClassTimerValue(Value, Path, C);
    ScriptTimerStartImpl(N, Seconds, H, Repeat, C, &V);
}
void ScriptTimerNextTickClass(const char* N, const char* H, FDasClassHandle Value, const char* Path, das::Context* C)
{
    auto V = ClassTimerValue(Value, Path, C);
    ScriptTimerStartImpl(N, 0.f, H, false, C, &V, true);
}
void ScriptTimerStartObject(const char* N, float Seconds, const char* H, bool Repeat, const FDasObjectHandle& Value,
                            const char* Path, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 8;
    V.ObjectValue = Value.Token ? ResolveObject(Value, C) : nullptr;
    V.ObjectClass = Path ? UTF8_TO_TCHAR(Path) : TEXT("");
    ScriptTimerStartImpl(N, Seconds, H, Repeat, C, &V);
}
void ScriptTimerNextTickObject(const char* N, const char* H, const FDasObjectHandle& Value, const char* Path,
                               das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 8;
    V.ObjectValue = Value.Token ? ResolveObject(Value, C) : nullptr;
    V.ObjectClass = Path ? UTF8_TO_TCHAR(Path) : TEXT("");
    ScriptTimerStartImpl(N, 0.f, H, false, C, &V, true);
}
void ScriptTimerStartEnum(const char* N, float Seconds, const char* H, bool Repeat, FDasEnumValue Value,
                          das::Context* C)
{
    const auto& E = ResolveEnumValue(Value, C);
    FDasScalarValue V;
    V.Kind = 13;
    V.IntValue = E.Value;
    V.ObjectClass = E.Type->GetPathName();
    ScriptTimerStartImpl(N, Seconds, H, Repeat, C, &V);
}
void ScriptTimerNextTickEnum(const char* N, const char* H, FDasEnumValue Value, das::Context* C)
{
    const auto& E = ResolveEnumValue(Value, C);
    FDasScalarValue V;
    V.Kind = 13;
    V.IntValue = E.Value;
    V.ObjectClass = E.Type->GetPathName();
    ScriptTimerStartImpl(N, 0.f, H, false, C, &V, true);
}
void ScriptTimerStartVector(const char* N, float Seconds, const char* H, bool Repeat, const FVector& Value,
                            das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 9;
    V.VectorValue = Value;
    ScriptTimerStartImpl(N, Seconds, H, Repeat, C, &V);
}
void ScriptTimerStartRotator(const char* N, float Seconds, const char* H, bool Repeat, const FRotator& Value,
                             das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 10;
    V.RotatorValue = Value;
    ScriptTimerStartImpl(N, Seconds, H, Repeat, C, &V);
}
void ScriptTimerStartTransform(const char* N, float Seconds, const char* H, bool Repeat, const FTransform& Value,
                               das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 11;
    V.TransformValue = Value;
    ScriptTimerStartImpl(N, Seconds, H, Repeat, C, &V);
}
void ScriptTimerNextTick(const char* N, const char* H, das::Context* C)
{
    ScriptTimerStartImpl(N, 0.f, H, false, C, nullptr, true);
}
void ScriptTimerNextTickArray(const char* N, const char* H, const FDasStructValue& Value, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 12;
    V.StructValue = ResolveStructValue(Value, C);
    V.ObjectClass = V.StructValue.GetScriptStruct()->GetPathName();
    ScriptTimerStartImpl(N, 0.f, H, false, C, &V, true);
}
void ScriptTimerNextTickVector(const char* N, const char* H, const FVector& Value, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 9;
    V.VectorValue = Value;
    ScriptTimerStartImpl(N, 0.f, H, false, C, &V, true);
}
void ScriptTimerNextTickRotator(const char* N, const char* H, const FRotator& Value, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 10;
    V.RotatorValue = Value;
    ScriptTimerStartImpl(N, 0.f, H, false, C, &V, true);
}
void ScriptTimerNextTickTransform(const char* N, const char* H, const FTransform& Value, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 11;
    V.TransformValue = Value;
    ScriptTimerStartImpl(N, 0.f, H, false, C, &V, true);
}
bool ScriptTimerPaused(const char* Name, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    auto* T = FindScriptTimer(S, TimerName(Name, C));
    return T && T->World.IsValid() && T->World->GetTimerManager().IsTimerPaused(T->Handle);
}
float ScriptTimerElapsed(const char* Name, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    auto* T = FindScriptTimer(S, TimerName(Name, C));
    return T && T->World.IsValid() ? T->World->GetTimerManager().GetTimerElapsed(T->Handle) : -1.f;
}
int32 ScriptTimerCancelAll(das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    int32 Count = 0;
    for (const auto& P : ScriptTimers)
        if (P.Value->Context == S->Live->Context.Get())
            ++Count;
    ClearSessionTimers(*S);
    return Count;
}
void ScriptTimerStartVector2d(const char* N, float Seconds, const char* H, bool Repeat, const FVector2D& Value,
                              das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 12;
    V.ObjectClass = TEXT("/Script/CoreUObject.Vector2D");
    V.StructValue.InitializeAs(TBaseStructure<FVector2D>::Get(), reinterpret_cast<const uint8*>(&Value));
    ScriptTimerStartImpl(N, Seconds, H, Repeat, C, &V);
}
void ScriptTimerNextTickVector2d(const char* N, const char* H, const FVector2D& Value, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 12;
    V.ObjectClass = TEXT("/Script/CoreUObject.Vector2D");
    V.StructValue.InitializeAs(TBaseStructure<FVector2D>::Get(), reinterpret_cast<const uint8*>(&Value));
    ScriptTimerStartImpl(N, 0.f, H, false, C, &V, true);
}
bool ScriptTimerCancel(const char* Name, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    auto* T = FindScriptTimer(S, TimerName(Name, C));
    if (!T)
        return false;
    ScriptTimers.Remove(T->Id);
    return true;
}
bool ScriptTimerPause(const char* Name, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    auto* T = FindScriptTimer(S, TimerName(Name, C));
    if (!T || !T->World.IsValid())
        return false;
    T->World->GetTimerManager().PauseTimer(T->Handle);
    return true;
}
bool ScriptTimerResume(const char* Name, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    auto* T = FindScriptTimer(S, TimerName(Name, C));
    if (!T || !T->World.IsValid())
        return false;
    T->World->GetTimerManager().UnPauseTimer(T->Handle);
    return true;
}
bool ScriptTimerActive(const char* Name, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    auto* T = FindScriptTimer(S, TimerName(Name, C));
    return T && T->World.IsValid() && T->World->GetTimerManager().IsTimerActive(T->Handle);
}
float ScriptTimerRemaining(const char* Name, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    auto* T = FindScriptTimer(S, TimerName(Name, C));
    return T && T->World.IsValid() ? T->World->GetTimerManager().GetTimerRemaining(T->Handle) : -1.f;
}
void DrainScriptTimers()
{
    if (ActiveContext || bDrainingScriptTimers || !IsInGameThread())
        return;
#if WITH_EDITOR
    if (bPreservingSessions)
        return;
#endif
    TGuardValue<bool> Guard(bDrainingScriptTimers, true);
    int32 Count = 0;
    while (!PendingScriptTimers.IsEmpty() && Count++ < 1024)
    {
        const uint64 Id = PendingScriptTimers[0];
        PendingScriptTimers.RemoveAt(0);
        auto* Slot = ScriptTimers.Find(Id);
        if (!Slot)
            continue;
        auto* T = Slot->Get();
        FPersistentSession* S = nullptr;
        for (const auto& Candidate : Sessions)
            if (Candidate->Live && Candidate->Live->Context.Get() == T->Context)
            {
                S = Candidate.Get();
                break;
            }
        if (!S || S->bReleaseRequested || S->bEndDelivered || !T->Owner.IsValid() || !T->World.IsValid())
        {
            ScriptTimers.Remove(Id);
            continue;
        }
        if (auto* A = Cast<AActor>(T->Owner.Get()); A && A->IsActorBeingDestroyed())
        {
            ScriptTimers.Remove(Id);
            continue;
        }
        const FString Path = S->Path, Handler = T->Handler;
        const TWeakObjectPtr<UObject> Owner = T->Owner;
        const auto Captured = T->Argument;
        FString CaptureIssue;
        if (Captured && (!Captured->IsAlive() ||
                         !ValidateDasCallbackPayload(Captured->GetValue(), EDasCallbackUsage::Timer, CaptureIssue)))
        {
            ScriptTimers.Remove(Id);
            continue;
        }
        if (!T->Repeat)
            ScriptTimers.Remove(Id);
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        TArray<FDasScalarValue> Args;
        if (Captured)
            Args.Add(Captured->GetValue());
        auto Result = Captured ? Runtime.RunNamedFramePersistent(Path, Handler, Args, EDasMethodSignature::Void, Owner)
                               : Runtime.RunNamedVoidPersistent(Path, Handler, Owner);
        if (!Result.bSucceeded)
        {
            ScriptTimers.Remove(Id);
            UE_LOG(LogDasRuntime, Warning, TEXT("Script timer %s failed: %s"), *Handler, *Result.Error);
            if (auto* Error = DasHostString(Owner.Get(), TEXT("LastDasError")))
                *Error = Result.Error;
        }
    }
    if (!PendingScriptTimers.IsEmpty())
    {
        UE_LOG(LogDasRuntime, Warning, TEXT("Script timer dispatch budget exceeded"));
        PendingScriptTimers.Reset();
    }
}

uint64 NextSubscriptionId = 1;
struct FPendingIntEvent
{
    uint64 Id;
    TSharedPtr<FDasCallbackPayload> Capture;
};
TArray<FPendingIntEvent> PendingEvents;
bool bDrainingEvents = false;

void RememberInput(FPersistentSession& S, const FManagedSubscription& B)
{
    if (S.bRestoringInput)
        return;
    FInputRecipe R;
    R.Kind = B.InputKind;
    R.Name = B.Event;
    R.Handler = B.Handler;
    R.Phase = B.InputPhase;
    R.Trigger = B.EnhancedPhase;
    R.Consume = B.InputConsume;
    R.Paused = B.InputPaused;
    R.Asset.Reset(B.InputAsset.Get());
    auto* Old = S.InputRecipes.FindByPredicate(
        [&](const auto& X)
        {
            return X.Kind == R.Kind && (R.Kind == 3 ? X.Asset.Get() == R.Asset.Get() && X.Trigger == R.Trigger
                                                    : X.Name == R.Name && (R.Kind == 2 || X.Phase == R.Phase));
        });
    if (Old)
        *Old = R;
    else
        S.InputRecipes.Add(MoveTemp(R));
    S.NextInputRestore = 0;
}
