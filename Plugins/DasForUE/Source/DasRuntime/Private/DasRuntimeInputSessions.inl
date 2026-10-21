// Part of DasRuntime.cpp: Managed input, asset loading, persistent sessions and event delivery.
// Included in place; declaration order matters.
UInputComponent* ScriptOwnerInput(das::Context* C)
{
    auto* Self = ResolveSelf(C);
    auto* Actor = Cast<AActor>(Self);
    if (!Actor)
        if (auto* Component = Cast<UActorComponent>(Self))
            Actor = Component->GetOwner();
    return Actor && IsValid(Actor->InputComponent) ? Actor->InputComponent.Get() : nullptr;
}
FName ScriptInputName(const char* Name, das::Context* C)
{
    const FString N = Name ? UTF8_TO_TCHAR(Name) : TEXT("");
    if (N.IsEmpty() || N.Len() >= NAME_SIZE || N.Equals(TEXT("None"), ESearchCase::IgnoreCase))
        C->throw_error("input mapping name is required");
    return FName(*N);
}
EInputEvent ScriptInputPhase(const char* Phase, das::Context* C)
{
    const FString P = Phase ? UTF8_TO_TCHAR(Phase) : TEXT("");
    if (P == TEXT("Pressed"))
        return IE_Pressed;
    if (P == TEXT("Released"))
        return IE_Released;
    if (P == TEXT("Repeat"))
        return IE_Repeat;
    if (P == TEXT("DoubleClick"))
        return IE_DoubleClick;
    C->throw_error("input action phase must be Pressed/Released/Repeat/DoubleClick");
    return IE_Pressed;
}
void BindScriptInput(const char* Name, const char* Handler, int32 Kind, EInputEvent Phase, bool Consume, bool Paused,
                     das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    if (ActiveContext->bEnding)
        C->throw_error("cannot bind input during EndPlay");
    auto* Input = ScriptOwnerInput(C);
    if (!Input)
        C->throw_error("owner has no InputComponent; enable input or complete possession before binding");
    const FName Mapping = ScriptInputName(Name, C);
    const FString Entry = Handler ? UTF8_TO_TCHAR(Handler) : TEXT("");
    FDasCallContract Contract = FDasCallContract::Scalar(EDasMethodSignature::Void);
    if (Kind == 2)
        Contract.Inputs.Add(EDasMethodSignature::Float);
    if (!VerifyContract(S->Live->Program, Entry, Contract))
        C->throw_error("input requires matching exported void action/float axis handler");
    int32 Old = INDEX_NONE, Count = 0;
    for (int32 I = 0; I < S->Bindings.Num(); ++I)
    {
        const auto& B = *S->Bindings[I];
        if (B.InputKind)
            ++Count;
        if (B.InputKind == Kind && B.Event == Mapping && (Kind == 2 || B.InputPhase == Phase))
            Old = I;
    }
    if (Old == INDEX_NONE && Count >= 64)
        C->throw_error("input binding limit exceeded");
    if (Old != INDEX_NONE)
        S->Bindings.RemoveAt(Old);
    auto B = MakeUnique<FManagedSubscription>();
    B->Id = NextSubscriptionId++;
    B->Source = Input;
    B->Event = Mapping;
    B->Handler = Entry;
    B->Kind = Kind == 1 ? EDasMethodSignature::Void : EDasMethodSignature::Float;
    B->InputKind = Kind;
    B->InputPhase = Phase;
    B->InputConsume = Consume;
    B->InputPaused = Paused;
    B->Proxy.Reset(NewObject<UDasIntDelegateProxy>());
    B->Proxy->SubscriptionId = B->Id;
    if (Kind == 1)
    {
        FInputActionBinding Action(Mapping, Phase);
        Action.bConsumeInput = Consume;
        Action.bExecuteWhenPaused = Paused;
        Action.ActionDelegate.BindDelegate(B->Proxy.Get(),
                                           GET_FUNCTION_NAME_CHECKED(UDasIntDelegateProxy, ReceiveVoid));
        B->InputHandle = Input->AddActionBinding(MoveTemp(Action)).GetHandle();
    }
    else
    {
        FInputAxisBinding Axis(Mapping);
        Axis.bConsumeInput = Consume;
        Axis.bExecuteWhenPaused = Paused;
        Axis.AxisDelegate.BindDelegate(B->Proxy.Get(), GET_FUNCTION_NAME_CHECKED(UDasIntDelegateProxy, ReceiveFloat));
        Input->AxisBindings.Add(MoveTemp(Axis));
    }
    RememberInput(*S, *B);
    S->Bindings.Add(MoveTemp(B));
    S->RequiredEntries.Add(Entry, Contract);
}
void ScriptEnhancedAddContext(const FDasObjectHandle&, const FDasObjectHandle&, int32, das::Context*);
bool ScriptEnhancedRemoveContext(const FDasObjectHandle&, const FDasObjectHandle&, das::Context*);
bool ScriptEnhancedHasContext(const FDasObjectHandle&, const FDasObjectHandle&, das::Context*);
int32 ScriptEnhancedClearContexts(das::Context*);
void ScriptEnhancedBind(const FDasObjectHandle&, const char*, const char*, das::Context*);
bool ScriptEnhancedUnbind(const FDasObjectHandle&, const char*, das::Context*);
bool ScriptEnhancedAvailable(das::Context*);
void ScriptInputAction(const char* Name, const char* Phase, const char* Handler, bool Consume, bool Paused,
                       das::Context* C)
{
    BindScriptInput(Name, Handler, 1, ScriptInputPhase(Phase, C), Consume, Paused, C);
}
void ScriptInputAxis(const char* Name, const char* Handler, bool Consume, bool Paused, das::Context* C)
{
    BindScriptInput(Name, Handler, 2, IE_Pressed, Consume, Paused, C);
}
bool ScriptInputAvailable(das::Context* C)
{
    ManagedOwnerSession(C);
    return ScriptOwnerInput(C) != nullptr;
}
bool ScriptInputUnbindAction(const char* Name, const char* Phase, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    const FName N = ScriptInputName(Name, C);
    const auto P = ScriptInputPhase(Phase, C);
    const int32 Wanted =
        S->InputRecipes.RemoveAll([&](const auto& R) { return R.Kind == 1 && R.Name == N && R.Phase == P; });
    return S->Bindings.RemoveAll([&](const auto& B)
                                 { return B->InputKind == 1 && B->Event == N && B->InputPhase == P; }) > 0 ||
           Wanted > 0;
}
bool ScriptInputUnbindAxis(const char* Name, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    const FName N = ScriptInputName(Name, C);
    const int32 Wanted = S->InputRecipes.RemoveAll([&](const auto& R) { return R.Kind == 2 && R.Name == N; });
    return S->Bindings.RemoveAll([&](const auto& B) { return B->InputKind == 2 && B->Event == N; }) > 0 || Wanted > 0;
}
int32 ScriptInputUnbindAll(das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    const int32 Count = S->InputRecipes.Num();
    S->InputRecipes.Empty();
    S->Bindings.RemoveAll([](const auto& B) { return B->InputKind != 0; });
    return Count;
}


ETriggerEvent ScriptEnhancedPhase(const char* Phase, das::Context* C)
{
    const FString P = Phase ? UTF8_TO_TCHAR(Phase) : TEXT("");
    if (P == TEXT("Triggered"))
        return ETriggerEvent::Triggered;
    if (P == TEXT("Started"))
        return ETriggerEvent::Started;
    if (P == TEXT("Ongoing"))
        return ETriggerEvent::Ongoing;
    if (P == TEXT("Canceled"))
        return ETriggerEvent::Canceled;
    if (P == TEXT("Completed"))
        return ETriggerEvent::Completed;
    C->throw_error("Enhanced Input trigger must be Triggered/Started/Ongoing/Canceled/Completed");
    return ETriggerEvent::None;
}
void ScriptEnhancedAddContext(const FDasObjectHandle&, const FDasObjectHandle&, int32, das::Context*);
bool ScriptEnhancedRemoveContext(const FDasObjectHandle&, const FDasObjectHandle&, das::Context*);
bool ScriptEnhancedHasContext(const FDasObjectHandle&, const FDasObjectHandle&, das::Context*);
int32 ScriptEnhancedClearContexts(das::Context*);
void ScriptEnhancedBind(const FDasObjectHandle& ActionHandle, const char* Trigger, const char* Handler, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    if (ActiveContext->bEnding)
        C->throw_error("cannot bind Enhanced Input during EndPlay");
    auto* Input = Cast<UEnhancedInputComponent>(ScriptOwnerInput(C));
    if (!Input)
        C->throw_error("owner requires an existing EnhancedInputComponent");
    auto* Action = Cast<UInputAction>(ResolveObject(ActionHandle, C));
    if (!Action)
        C->throw_error("Enhanced Input requires an InputAction asset");
    const auto Phase = ScriptEnhancedPhase(Trigger, C);
    EDasMethodSignature Kind;
    switch (Action->ValueType)
    {
    case EInputActionValueType::Boolean:
        Kind = EDasMethodSignature::Bool;
        break;
    case EInputActionValueType::Axis1D:
        Kind = EDasMethodSignature::Float;
        break;
    case EInputActionValueType::Axis2D:
        Kind = EDasMethodSignature::Struct;
        break;
    case EInputActionValueType::Axis3D:
        Kind = EDasMethodSignature::Vector;
        break;
    default:
        C->throw_error("unsupported Enhanced Input value shape");
        return;
    }
    const FString Entry = Handler ? UTF8_TO_TCHAR(Handler) : TEXT("");
    FDasCallContract Contract = FDasCallContract::Scalar(EDasMethodSignature::Void);
    Contract.Inputs.Add(EDasMethodSignature::Struct);
    Contract.InputClasses.Add(TEXT("/Script/EnhancedInput.InputActionValue"));
    const bool TypedValue = VerifyContract(S->Live->Program, Entry, Contract, true);
    if (TypedValue)
        Kind = EDasMethodSignature::Struct;
    else
    {
        Contract.Inputs.Reset();
        Contract.InputClasses.Reset();
        Contract.Inputs.Add(Kind);
        if (Kind == EDasMethodSignature::Struct)
            Contract.InputClasses.Add(TEXT("/Script/CoreUObject.Vector2D"));
        if (!VerifyContract(S->Live->Program, Entry, Contract))
            C->throw_error(
                "Enhanced Input requires InputActionValue or a matching bool/float/Vector2D/Vector void handler");
    }
    int32 Old = INDEX_NONE, Count = 0;
    for (int32 I = 0; I < S->Bindings.Num(); ++I)
    {
        const auto& B = *S->Bindings[I];
        if (B.InputKind)
            ++Count;
        if (B.InputKind == 3 && B.InputAsset.Get() == Action && B.EnhancedPhase == Phase)
            Old = I;
    }
    if (Old == INDEX_NONE && Count >= 64)
        C->throw_error("input binding limit exceeded");
    if (Old != INDEX_NONE)
        S->Bindings.RemoveAt(Old);
    auto B = MakeUnique<FManagedSubscription>();
    B->Id = NextSubscriptionId++;
    B->Source = Input;
    B->Handler = Entry;
    B->Kind = Kind;
    if (Kind == EDasMethodSignature::Struct)
        B->PayloadPath =
            TypedValue ? TEXT("/Script/EnhancedInput.InputActionValue"): TEXT("/Script/CoreUObject.Vector2D");
    B->InputKind = 3;
    B->EnhancedPhase = Phase;
    B->InputAsset.Reset(Action);
    B->EnhancedProxy.Reset(NewObject<UDasEnhancedInputProxy>());
    B->EnhancedProxy->SubscriptionId = B->Id;
    B->EnhancedProxy->PayloadKind = int32(Kind);
    B->EnhancedProxy->TypedValue = TypedValue;
    B->EnhancedProxy->Action = Action;
    if (TypedValue)
        S->NativeInputEntries.Add(Entry);
    B->EnhancedHandle = Input
                            ->BindAction(Action, Phase, B->EnhancedProxy.Get(),
                                         GET_FUNCTION_NAME_CHECKED(UDasEnhancedInputProxy, Receive))
                            .GetHandle();
    RememberInput(*S, *B);
    S->Bindings.Add(MoveTemp(B));
    S->RequiredEntries.Add(Entry, Contract);
}
bool ScriptEnhancedUnbind(const FDasObjectHandle& H, const char* Trigger, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    auto* Action = Cast<UInputAction>(ResolveObject(H, C));
    if (!Action)
        C->throw_error("Enhanced Input requires an InputAction asset");
    const auto Phase = ScriptEnhancedPhase(Trigger, C);
    const int32 Wanted = S->InputRecipes.RemoveAll(
        [&](const auto& R) { return R.Kind == 3 && R.Asset.Get() == Action && R.Trigger == Phase; });
    return S->Bindings.RemoveAll(
               [&](const auto& B)
               { return B->InputKind == 3 && B->InputAsset.Get() == Action && B->EnhancedPhase == Phase; }) > 0 ||
           Wanted > 0;
}
bool ScriptEnhancedAvailable(das::Context* C)
{
    ManagedOwnerSession(C);
    return Cast<UEnhancedInputComponent>(ScriptOwnerInput(C)) != nullptr;
}


UEnhancedInputLocalPlayerSubsystem* ScriptEnhancedSubsystem(const FDasObjectHandle& H, FPersistentSession* S,
                                                            das::Context* C)
{
    auto* Player = Cast<APlayerController>(ResolveObject(H, C));
    if (!Player || Player->GetWorld() != S->World.Get() || !Player->IsLocalController() || !Player->GetLocalPlayer())
        C->throw_error("mapping context requires explicit local PlayerController in owner's world");
    auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Player->GetLocalPlayer());
    if (!Subsystem || !Subsystem->GetPlayerInput())
        C->throw_error("Enhanced Input LocalPlayer subsystem/player input is not ready");
    return Subsystem;
}
UInputMappingContext* ScriptMappingContext(const FDasObjectHandle& H, das::Context* C)
{
    auto* Context = Cast<UInputMappingContext>(ResolveObject(H, C));
    if (!Context)
        C->throw_error("expected InputMappingContext asset");
    return Context;
}
void ScriptEnhancedAddContext(const FDasObjectHandle& Player, const FDasObjectHandle& Mapping, int32 Priority,
                              das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    if (ActiveContext->bEnding)
        C->throw_error("cannot add mapping contexts during EndPlay");
    auto* Subsystem = ScriptEnhancedSubsystem(Player, S, C);
    auto* Context = ScriptMappingContext(Mapping, C);
    auto* Slot = S->InputContexts.FindByPredicate(
        [&](const auto& L) { return L->Subsystem.Get() == Subsystem && L->Context.Get() == Context; });
    int32 Actual = 0;
    const bool Present = Subsystem->HasMappingContext(Context, Actual);
    if (!Slot && Present)
        C->throw_error(
            "mapping context already installed outside this session; bind to it without acquiring ownership");
    if (Slot && Present && Actual != (*Slot)->Priority)
        C->throw_error("owned mapping context priority changed externally; refusing to overwrite it");
    if (!Slot && S->InputContexts.Num() >= 32)
        C->throw_error("mapping context count limit exceeded");
    auto New = MakeUnique<FManagedInputContext>();
    New->Subsystem = Subsystem;
    New->Context.Reset(Context);
    New->Priority = Priority;
    FModifyContextOptions Options;
    Options.bNotifyUserSettings = false;
    Subsystem->AddMappingContext(Context, Priority, Options);
    if (!Subsystem->HasMappingContext(Context, Actual) || Actual != Priority)
        C->throw_error("mapping context installation/priority could not be confirmed");
    if (Slot)
    {
        (*Slot)->Priority = Priority;
        New->Context.Reset();
    }
    else
        S->InputContexts.Add(MoveTemp(New));
    if (!S->bRestoringInput)
    {
        FInputContextRecipe R;
        R.Subsystem = Subsystem;
        R.Context.Reset(Context);
        R.Priority = Priority;
        auto* Prior = S->ContextRecipes.FindByPredicate(
            [&](const auto& X) { return X.Subsystem.Get() == Subsystem && X.Context.Get() == Context; });
        if (Prior)
            *Prior = R;
        else
            S->ContextRecipes.Add(MoveTemp(R));
    }
}
bool ScriptEnhancedRemoveContext(const FDasObjectHandle& Player, const FDasObjectHandle& Mapping, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    auto* Subsystem = ScriptEnhancedSubsystem(Player, S, C);
    auto* Context = ScriptMappingContext(Mapping, C);
    const int32 Wanted = S->ContextRecipes.RemoveAll(
        [&](const auto& R) { return R.Subsystem.Get() == Subsystem && R.Context.Get() == Context; });
    return S->InputContexts.RemoveAll([&](const auto& L)
                                      { return L->Subsystem.Get() == Subsystem && L->Context.Get() == Context; }) > 0 ||
           Wanted > 0;
}
bool ScriptEnhancedHasContext(const FDasObjectHandle& Player, const FDasObjectHandle& Mapping, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    auto* Subsystem = ScriptEnhancedSubsystem(Player, S, C);
    return Subsystem->HasMappingContext(ScriptMappingContext(Mapping, C));
}
int32 ScriptEnhancedClearContexts(das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    const int32 Count = S->ContextRecipes.Num();
    S->ContextRecipes.Empty();
    S->InputContexts.Empty();
    return Count;
}


void ReconcileScriptInput(UWorld* World)
{
    if (ActiveContext || !IsInGameThread())
        return;
#if WITH_EDITOR
    if (bPreservingSessions)
        return;
#endif
    const double Now = FPlatformTime::Seconds();
    for (auto& Owned : Sessions)
    {
        auto& S = *Owned;
        if (!S.Live || S.bReleaseRequested || S.bEndDelivered || S.World.Get() != World || !S.Owner.IsValid() ||
            Now < S.NextInputRestore || (S.InputRecipes.IsEmpty() && S.ContextRecipes.IsEmpty()))
            continue;
        if (auto* A = Cast<AActor>(S.Owner.Get()); A && A->IsActorBeingDestroyed())
            continue;
        auto& C = *S.Live->Context;
        FActiveContextScope Scope(C);
        TGuardValue<bool> Restoring(S.bRestoringInput, true);
        try
        {
            for (const auto& R : S.ContextRecipes)
            {
                auto* Subsystem = R.Subsystem.Get();
                if (!Subsystem || !Subsystem->GetPlayerInput())
                    continue;
                auto* Player = Cast<APlayerController>(Subsystem->GetPlayerInput()->GetOuter());
                if (!Player || Player->GetWorld() != World)
                    continue;
                const bool OwnedContext = S.InputContexts.ContainsByPredicate(
                    [&](const auto& L)
                    { return L->Subsystem.Get() == Subsystem && L->Context.Get() == R.Context.Get(); });
                if (!OwnedContext)
                    ScriptEnhancedAddContext(MakeObjectHandle(Player, APlayerController::StaticClass(), &C),
                                             MakeObjectHandle(R.Context.Get(), UInputMappingContext::StaticClass(), &C),
                                             R.Priority, &C);
            }
            auto* Input = ScriptOwnerInput(&C);
            S.Bindings.RemoveAll([&](const auto& B) { return B->InputKind && B->Source.Get() != Input; });
            if (Input)
            {
                for (const auto& R : S.InputRecipes)
                {
                    const bool Bound = S.Bindings.ContainsByPredicate(
                        [&](const auto& B)
                        {
                            return B->InputKind == R.Kind && B->Source.Get() == Input &&
                                   (R.Kind == 3 ? B->InputAsset.Get() == R.Asset.Get() && B->EnhancedPhase == R.Trigger
                                                : B->Event == R.Name && (R.Kind == 2 || B->InputPhase == R.Phase));
                        });
                    if (Bound)
                        continue;
                    const FTCHARToUTF8 Name(*R.Name.ToString()), Handler(*R.Handler);
                    if (R.Kind == 1)
                    {
                        const char* Phase = R.Phase == IE_Pressed    ? "Pressed"
                                            : R.Phase == IE_Released ? "Released"
                                            : R.Phase == IE_Repeat   ? "Repeat"
                                                                     : "DoubleClick";
                        ScriptInputAction(Name.Get(), Phase, Handler.Get(), R.Consume, R.Paused, &C);
                    }
                    else if (R.Kind == 2)
                        ScriptInputAxis(Name.Get(), Handler.Get(), R.Consume, R.Paused, &C);
                    else if (Cast<UEnhancedInputComponent>(Input))
                    {
                        const char* Phase = R.Trigger == ETriggerEvent::Triggered  ? "Triggered"
                                            : R.Trigger == ETriggerEvent::Started  ? "Started"
                                            : R.Trigger == ETriggerEvent::Ongoing  ? "Ongoing"
                                            : R.Trigger == ETriggerEvent::Canceled ? "Canceled"
                                                                                   : "Completed";
                        ScriptEnhancedBind(MakeObjectHandle(R.Asset.Get(), UInputAction::StaticClass(), &C), Phase,
                                           Handler.Get(), &C);
                    }
                }
            }
            S.InputRestoreError.Empty();
        }
        catch (const std::exception& E)
        {
            const FString Error = UTF8_TO_TCHAR(E.what());
            if (S.InputRestoreError != Error)
                UE_LOG(LogDasRuntime, Warning, TEXT("Input restore deferred: %s"), *Error);
            S.InputRestoreError = Error;
            S.NextInputRestore = Now + 1.0;
            C.restart();
        }
    }
}


class FScriptDelayAction : public FPendingLatentAction
{
    float Remaining;
    FName Function;
    int32 Linkage;
    FWeakObjectPtr Target;

public:
    FScriptDelayAction(float Seconds, const FLatentActionInfo& Info)
        : Remaining(Seconds), Function(Info.ExecutionFunction), Linkage(Info.Linkage), Target(Info.CallbackTarget)
    {
    }
    virtual void UpdateOperation(FLatentResponse& Response) override
    {
        Remaining -= Response.ElapsedTime();
        Response.FinishAndTriggerIf(Remaining <= 0.f, Function, Linkage, Target);
    }
};
void ScriptAssetLoad(const char*, const char*, const char*, const char*, das::Context*);
void ScriptAssetLoadGeneric(const char*, const char*, const char*, das::Context*);
bool ScriptAssetCancel(const char*, das::Context*);
int32 ScriptAssetCancelAll(das::Context*);
bool ScriptAssetPending(const char*, das::Context*);
void ScriptLatentDelay(const char* Name, float Seconds, const char* Handler, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    if (ActiveContext->bEnding)
        C->throw_error("cannot start latent work during EndPlay");
    auto* W = S->World.Get();
    if (!W || W != S->Owner->GetWorld() || !W->IsGameWorld())
        C->throw_error("latent delay requires owner's live game world");
    const FString N = TimerName(Name, C), Entry = Handler ? UTF8_TO_TCHAR(Handler) : TEXT("");
    if (!FMath::IsFinite(Seconds) || Seconds < 0.f)
        C->throw_error("latent delay duration must be finite and nonnegative");
    const auto Contract = FDasCallContract::Scalar(EDasMethodSignature::Void);
    if (!VerifyContract(S->Live->Program, Entry, Contract))
        C->throw_error("latent delay requires exported handler():void");
    int32 Count = 0, Old = INDEX_NONE;
    for (int32 I = 0; I < S->Bindings.Num(); ++I)
    {
        const auto& B = *S->Bindings[I];
        if (B.LatentKind)
            ++Count;
        if (B.LatentKind == 1 && B.LatentName == N)
            Old = I;
    }
    if (Old == INDEX_NONE && Count >= 64)
        C->throw_error("latent delay count limit exceeded");
    if (Old != INDEX_NONE)
        S->Bindings.RemoveAt(Old);
    auto B = MakeUnique<FManagedSubscription>();
    B->Id = NextSubscriptionId++;
    B->Handler = Entry;
    B->Kind = EDasMethodSignature::Void;
    B->LatentKind = 1;
    B->LatentName = N;
    B->LatentWorld = W;
    B->LatentProxy.Reset(NewObject<UDasLatentProxy>());
    B->LatentProxy->SubscriptionId = B->Id;
    B->Source = B->LatentProxy.Get();
    FLatentActionInfo Info;
    Info.CallbackTarget = B->LatentProxy.Get();
    Info.ExecutionFunction = GET_FUNCTION_NAME_CHECKED(UDasLatentProxy, Complete);
    Info.Linkage = 0;
    Info.UUID = 1;
    W->GetLatentActionManager().AddNewAction(B->LatentProxy.Get(), Info.UUID, new FScriptDelayAction(Seconds, Info));
    S->Bindings.Add(MoveTemp(B));
    S->RequiredEntries.Add(Entry, Contract);
}
bool ScriptLatentCancel(const char* Name, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    const FString N = TimerName(Name, C);
    return S->Bindings.RemoveAll([&](const auto& B) { return B->LatentKind == 1 && B->LatentName == N; }) > 0;
}
int32 ScriptLatentCancelAll(das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    return S->Bindings.RemoveAll([](const auto& B) { return B->LatentKind != 0; });
}
bool ScriptLatentPending(const char* Name, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    const FString N = TimerName(Name, C);
    return S->Bindings.ContainsByPredicate([&](const auto& B) { return B->LatentKind == 1 && B->LatentName == N; });
}


void ScriptAssetLoadImpl(const char* Name, const char* Asset, const char* Handler, const char* Class,
                         bool ClassReference, bool AllowNull, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    if (ActiveContext->bEnding)
        C->throw_error("cannot start asset loading during EndPlay");
    if (!S->World.IsValid() || !S->World->IsGameWorld())
        C->throw_error("async asset loading requires owner's game world");
    const FString N = TimerName(Name, C),
                  PathText = Asset ? UTF8_TO_TCHAR(Asset)
                                   : TEXT(""), Entry = Handler ? UTF8_TO_TCHAR(Handler)
                                                               : TEXT(""), ClassPath =
                                                                               Class ? UTF8_TO_TCHAR(Class) : TEXT("");
    if ((PathText.IsEmpty() && !AllowNull) || PathText.Len() > 1024 ||
        (!PathText.IsEmpty() && !FPackageName::IsValidObjectPath(PathText)))
        C->throw_error("async asset load requires a valid Unreal object path");
    auto* Expected = FindObject<UClass>(nullptr, *ClassPath);
    if (!Expected || !ClassPath.StartsWith(TEXT("/Script/")) || Expected->HasAnyClassFlags(CLASS_Interface))
        C->throw_error("async asset callback requires a loaded native object class");
    FDasCallContract Contract = FDasCallContract::Scalar(EDasMethodSignature::Void);
    Contract.Inputs.Add(ClassReference ? EDasMethodSignature::Class : EDasMethodSignature::Object);
    Contract.InputClasses.Add(ClassPath);
    if (!VerifyContract(S->Live->Program, Entry, Contract))
        C->throw_error("async asset handler must return void with matching object input");
    int32 Old = INDEX_NONE, Count = 0;
    for (int32 I = 0; I < S->Bindings.Num(); ++I)
    {
        const auto& B = *S->Bindings[I];
        if (B.AsyncKind == 1)
            ++Count;
        if (B.AsyncKind == 1 && B.AsyncName == N)
            Old = I;
    }
    if (Old == INDEX_NONE && Count >= 64)
        C->throw_error("async asset request count limit exceeded");
    auto B = MakeUnique<FManagedSubscription>();
    B->Id = NextSubscriptionId++;
    B->Handler = Entry;
    B->Kind = ClassReference ? EDasMethodSignature::Class : EDasMethodSignature::Object;
    B->PayloadPath = ClassPath;
    B->AsyncKind = 1;
    B->AsyncName = N;
    B->AsyncProxy.Reset(NewObject<UDasAsyncAssetProxy>());
    B->AsyncProxy->SubscriptionId = B->Id;
    B->AsyncProxy->Path = FSoftObjectPath(PathText);
    B->AsyncProxy->ExpectedClass = ClassPath;
    B->AsyncProxy->ClassReference = ClassReference;
    B->Source = B->AsyncProxy.Get();
    const TWeakObjectPtr<UDasAsyncAssetProxy> Target(B->AsyncProxy.Get());
    if (!PathText.IsEmpty())
        B->StreamHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
            B->AsyncProxy->Path,
            FStreamableDelegate::CreateLambda(
                [Target]()
                {
                    if (auto* P = Target.Get())
                        P->Complete();
                }),
            FStreamableManager::DefaultAsyncLoadPriority, false, true, TEXT("DasForUE:")+ N);
    if (!PathText.IsEmpty() && !B->StreamHandle.IsValid())
        C->throw_error("async asset request could not be created");
    auto Handle = B->StreamHandle;
    if (Old != INDEX_NONE)
        S->Bindings.RemoveAt(Old);
    S->Bindings.Add(MoveTemp(B));
    S->RequiredEntries.Add(Entry, Contract);
    if (Handle.IsValid())
        Handle->StartStalledHandle();
    else if (auto* Proxy = Target.Get())
        Proxy->Complete();
}
void ScriptAssetLoad(const char* N, const char* Path, const char* Handler, const char* Class, das::Context* C)
{
    ScriptAssetLoadImpl(N, Path, Handler, Class, false, false, C);
}
void ScriptSoftAssetLoad(const char* N, const FDasStructValue& H, const char* Handler, das::Context* C)
{
    const auto& V = SoftValue(H, C);
    auto* P = DasSoftProperty(V.GetScriptStruct()->GetPathName());
    auto* Class = CastField<FSoftClassProperty>(P);
    const FTCHARToUTF8 Path(*SoftAssetPath(V).ToString()),
        Base(*(Class ? Class->MetaClass.Get() : P->PropertyClass.Get())->GetPathName());
    ScriptAssetLoadImpl(N, Path.Get(), Handler, Base.Get(), Class != nullptr, true, C);
}
void ScriptAssetLoadGeneric(const char* N, const char* Path, const char* Handler, das::Context* C)
{
    ScriptAssetLoad(N, Path, Handler, "/Script/CoreUObject.Object", C);
}
bool ScriptAssetCancel(const char* Name, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    const FString N = TimerName(Name, C);
    return S->Bindings.RemoveAll([&](const auto& B) { return B->AsyncKind == 1 && B->AsyncName == N; }) > 0;
}
int32 ScriptAssetCancelAll(das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    return S->Bindings.RemoveAll([](const auto& B) { return B->AsyncKind == 1; });
}
bool ScriptAssetPending(const char* Name, das::Context* C)
{
    auto* S = ManagedOwnerSession(C);
    const FString N = TimerName(Name, C);
    return S->Bindings.ContainsByPredicate([&](const auto& B) { return B->AsyncKind == 1 && B->AsyncName == N; });
}
#include "DasSaveGameRuntime.inl"
#include "DasGameplayTagsRuntime.inl"
#include "DasCurveTableRuntime.inl"
#include "DasDataTableRuntime.inl"
#include "DasPrimaryAssetsRuntime.inl"


#include "DasMontage.inl"
#include "DasAI.inl"
#include "DasEffects.inl"

void RevokeSession(FPersistentSession& Session)
{
    UDasAsyncPhysicsBridge::RevokeOwner(Session.Owner.GetEvenIfUnreachable());
    ClearSessionTimers(Session);
    Session.InputRecipes.Empty();
    Session.ContextRecipes.Empty();
    if (Session.Live)
    {
        Session.Live->Context->DelegateLease->Active = false;
        Session.Live->Context->OwnedScriptDelegateEvents.Empty();
        Session.Live->Context->DragLifetime.Revoke();
        Session.Live->Context->ImageBrushes.Revoke();
    }
    if (ActiveWidgetReply && ActiveWidgetReply->Widget.Get() == Session.Owner.Get())
        ActiveWidgetReply->NavigationRevoked = true;
    Session.bReleaseRequested = true;
    Session.Bindings.Empty();
    Session.InputContexts.Empty();
}
void FlushReleasedSessions()
{
    if (!ActiveContext)
        Sessions.RemoveAll([](const auto& Session) { return Session->bReleaseRequested; });
}
void DrainEvents();
struct FDrainEventsOnExit
{
    ~FDrainEventsOnExit() { DrainEvents(); }
};

void InstallSubscriptions(FPersistentSession& Session)
{
    for (const auto& Declaration : Session.Live->Context->Subscriptions)
    {
        UObject* Source = Declaration.Source.Get();
        if (!IsValid(Source))
            continue;
        auto* Property = ScalarEventProperty(Source, Declaration.Event, Declaration.Kind);
        if (!Property)
            continue;
        auto Binding = MakeUnique<FManagedSubscription>();
        Binding->Id = NextSubscriptionId++;
        Binding->Source = Source;
        Binding->Event = Declaration.Event;
        Binding->Handler = Declaration.Handler;
        Binding->Kind = Declaration.Kind;
        Binding->PayloadPath = Declaration.PayloadPath;
        Binding->Proxy.Reset(NewObject<UDasIntDelegateProxy>());
        Binding->Proxy->SubscriptionId = Binding->Id;
        FScriptDelegate Delegate;
        const auto* CallbackType = DasCallbackTypeForProperty(ScalarEventParameter(Property));
        if (FindDasCallbackFrame(Binding->PayloadPath) || (CallbackType && CallbackType->bDynamic))
        {
            FString Error;
            Binding->ProxyFunction = Binding->Proxy->ConfigureDynamic(
                Property->SignatureFunction,
                Binding->Kind == EDasMethodSignature::Object
                    ? DasReferencePropertyClass(ScalarEventParameter(Property))->GetPathName()
                    : Binding->PayloadPath,
                Binding->Kind, Error);
            if (Binding->ProxyFunction.IsNone())
            {
                UE_LOG(LogDasRuntime, Error, TEXT("Dynamic event receiver: %s"), *Error);
                continue;
            }
        }
        else
            Binding->ProxyFunction = EventProxyFunction(Binding->Kind, Property);
        Delegate.BindUFunction(Binding->Proxy.Get(), Binding->ProxyFunction);
        Property->AddDelegate(Delegate, Source, Property->ContainerPtrToValuePtr<void>(Source));
        Session.Bindings.Add(MoveTemp(Binding));
    }
}

void DrainEvents()
{
    if (ConstructingObject || ActiveContext || ActiveWidgetDrag || ActiveWidgetReply || bDrainingEvents ||
        !IsInGameThread())
        return;
    DrainScriptTimers();
#if WITH_EDITOR
    if (bPreservingSessions)
        return;
#endif
    TGuardValue<bool> Draining(bDrainingEvents, true);
    FlushReleasedSessions();
    int32 Index = 0, DispatchBudget = 0;
    while ((Index < PendingEvents.Num() || !PendingGASCueSessions.IsEmpty()) && ++DispatchBudget <= 1024)
    {
        FlushReleasedSessions();
        if (!PendingGASCueSessions.IsEmpty())
            DrainOneGASCueHook();
        if (Index >= PendingEvents.Num())
            continue;
        const auto Event = PendingEvents[Index++];
        const auto& Payload = Event.Capture->GetValue();
        FString CaptureIssue;
        if (!Event.Capture->IsAlive() || !ValidateDasCallbackPayload(Payload, EDasCallbackUsage::Event, CaptureIssue))
            continue;
        FPersistentSession* Session = nullptr;
        FString Handler, EventObjectPath;
        for (const auto& Candidate : Sessions)
        {
            for (const auto& Binding : Candidate->Bindings)
                if (Binding->Id == Event.Id && Binding->Source.IsValid() && int32(Binding->Kind) == Payload.Kind &&
                    ((Payload.Kind != 12 && Payload.Kind != 13 && Payload.Kind != 15) ||
                     Binding->PayloadPath == Payload.ObjectClass))
                {
                    Session = Candidate.Get();
                    Handler = Binding->Handler;
                    EventObjectPath = Binding->PayloadPath;
                    break;
                }
            if (Session)
                break;
        }
        if (!Session || !Session->Live || !Session->Owner.IsValid() || Session->bEndDelivered ||
            Session->bReleaseRequested)
            continue;
        if (auto* Actor = Cast<AActor>(Session->Owner.Get()); Actor && Actor->IsActorBeingDestroyed())
            continue;
        auto& Context = *Session->Live->Context;
        const auto* MontagePayload = Payload.Kind == int32(EDasMethodSignature::Struct)
                                         ? Payload.StructValue.GetPtr<FDasMontageEvent>()
                                         : nullptr;
        const bool MontageTerminal = MontagePayload && (MontagePayload->Kind == EDasMontageEventKind::Completed ||
                                                        MontagePayload->Kind == EDasMontageEventKind::Interrupted);
        Session->Bindings.RemoveAll(
            [&](const auto& B)
            {
                return B->Id == Event.Id &&
                       (B->LatentKind != 0 || B->AsyncKind != 0 || B->SaveGameKind != 0 || B->AIMoveProxy.IsValid() ||
                        B->EffectProxy.IsValid() || (B->MontageProxy.IsValid() && MontageTerminal));
            }); // Consume completion before user rearm.
        Context.restart();
        TGuardValue<int32> ExecutionScope(Context.ExecutionDepth, Context.ExecutionDepth + 1);
        FActiveContextScope Scope(Context);
        FStructCollectOnExit CollectStructs{Context};
        FString Error;
        try
        {
            const FTCHARToUTF8 Name(*Handler);
            auto* Function = Context.findFunction(Name.Get());
            auto Declaration = Session->Live->Program->getThisModule()->findUniqueFunction(Name.Get());
            if (!Function)
                continue; // Candidate validation makes this unreachable for a current binding.
            FDasInputActionValue InputArg;
            FDasStructValue StructArg;
            FDasObjectHandle ObjectArg;
            FVector VectorArg = Payload.VectorValue;
            FRotator RotatorArg = Payload.RotatorValue;
            FTransform TransformArg = Payload.TransformValue;
            vec4f Args[1];
            if (Payload.Kind == 15)
                Args[0] = das::cast<FDasClassHandle>::from(
                    {MakeObjectHandle(Payload.ClassValue.Get(), UClass::StaticClass(), &Context).Token});
            else if (Payload.Kind == 13)
                Args[0] =
                    das::cast<FDasEnumValue>::from(MakeEnumValue(Payload.ObjectClass, Payload.IntValue, &Context));
            else if (Payload.Kind == 17)
                Args[0] = das::cast<uint8_t>::from(Payload.ByteValue);
            else if (Payload.Kind == 18)
                Args[0] = das::cast<FDasTextValue>::from(MakeDSLText(Payload.TextValue, &Context));
            else if (Payload.Kind == 7)
                Args[0] = das::cast<FDasNameValue>::from(MakeDSLName(Payload.NameValue, &Context));
            else if (Payload.Kind == 12)
            {
                if (Payload.ObjectClass == TEXT("/Script/EnhancedInput.InputActionValue") && Declaration &&
                                                Declaration->arguments.size() == 1 &&
                                                Declaration->arguments[0]->type->annotation &&
                                                Declaration->arguments[0]->type->annotation->name == "InputActionValue")
                {
                    InputArg = SnapshotInputValue(Payload.StructValue.Get<FInputActionValue>(), &Context);
                    Args[0] = das::cast<void*>::from(&InputArg);
                }
                else
                {
                    StructArg = StoreStructValue(Payload.StructValue, &Context);
                    Args[0] = das::cast<void*>::from(&StructArg);
                }
            }
            else if (Payload.Kind == 6)
                Args[0] = das::cast<double>::from(Payload.DoubleValue);
            else if (Payload.Kind == 8)
            {
                auto* Expected = FindObject<UClass>(
                    nullptr,
                    *(EventObjectPath.IsEmpty() ? FString(TEXT("/Script/CoreUObject.Object")) : EventObjectPath));
                if (!Expected || !DasReferenceMatches(Payload.ObjectValue.Get(), Expected))
                    continue;
                ObjectArg = MakeObjectHandle(Payload.ObjectValue.Get(), Expected, &Context);
                Args[0] = das::cast<void*>::from(&ObjectArg);
            }
            else if (Payload.Kind == 9)
                Args[0] = das::cast<void*>::from(&VectorArg);
            else if (Payload.Kind == 10)
                Args[0] = das::cast<void*>::from(&RotatorArg);
            else if (Payload.Kind == 11)
                Args[0] = das::cast<void*>::from(&TransformArg);
            else if (Payload.Kind == 0)
                Args[0] = das::cast<int32>::from(Payload.IntValue);
            else if (Payload.Kind == 1)
                Args[0] = das::cast<float>::from(Payload.FloatValue);
            else if (Payload.Kind == 2)
                Args[0] = das::cast<bool>::from(Payload.BoolValue);
            else if (Payload.Kind == 5)
                Args[0] = das::cast<char*>::from(CopyDSLString(Payload.StringValue, &Context));
            Context.evalWithCatch(Function, Payload.Kind == 3 ? nullptr : Args);
            if (const char* Exception = Context.getException())
                Error = UTF8_TO_TCHAR(Exception);
        }
        catch (const std::exception& Exception)
        {
            Error = UTF8_TO_TCHAR(Exception.what());
        }
        if (!Error.IsEmpty())
        {
            UE_LOG(LogDasRuntime, Display, TEXT("Event handler %s failed: %s"), *Handler, *Error);
            if (auto* HostError = DasHostString(Session->Owner.Get(), TEXT("LastDasError")))
                *HostError = Error;
        }
    }
    if (Index < PendingEvents.Num() || !PendingGASCueSessions.IsEmpty())
        UE_LOG(LogDasRuntime, Display, TEXT("Event dispatch budget exceeded; remaining recursive events dropped."));
    PendingEvents.Reset();
    DropPendingGASCueHooks();
    FlushReleasedSessions();
}

void PruneSessions()
{
    ++GDasSelfSiteEpoch;
    for (auto It = DSLPhysicsOwners.CreateIterator(); It; ++It)
        if (!It.Key().IsValid() || !It.Value().IsValid())
            It.RemoveCurrent();
    for (auto It = InitializerDeliveries.CreateIterator(); It; ++It)
        if (!It.Key().IsValid())
            It.RemoveCurrent();
    for (const auto& Session : Sessions)
    {
        if (!Session->Owner.IsValid())
            RevokeSession(*Session);
        else
            Session->Bindings.RemoveAll([](const auto& Binding) { return !Binding->Source.IsValid(); });
    }
    FlushReleasedSessions();
}
