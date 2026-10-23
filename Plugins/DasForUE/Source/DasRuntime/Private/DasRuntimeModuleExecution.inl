// Part of DasRuntime.cpp: FDasRuntimeModule: script execution, callbacks, construction and library frames.
// Included in place; declaration order matters.
const FString& NormalizeScriptPath(const FString& Path)
{
    if (const FString* Cached = NormalizedScriptPaths.Find(Path))
        return *Cached;
    FString Full = FPaths::ConvertRelativePathToFull(Path);
    FPaths::NormalizeFilename(Full);
    FPaths::CollapseRelativeDirectories(Full);
    return NormalizedScriptPaths.Add(Path, MoveTemp(Full));
}
// Whether a call must compare its session sources with disk (das.ScriptChangeCheck).
bool SourcesNeedCheck(const FPersistentSession& Session)
{
    int32 Mode = CVarScriptChangeCheck.GetValueOnGameThread();
    if (Mode < 0)
        Mode = bScriptWatcherActive && !GIsAutomationTesting ? 1 : 0;
    if (Mode == 2)
        return false;
    return Mode != 1 || Session.CheckedEpoch != ScriptSourceEpoch ||
           FPlatformTime::Seconds() - Session.CheckedAt >= CVarScriptChangePollSeconds.GetValueOnGameThread();
}
void MarkSourcesChecked(FPersistentSession& Session)
{
    Session.CheckedEpoch = ScriptSourceEpoch;
    Session.CheckedAt = FPlatformTime::Seconds();
}
uint8 WidgetContractFlags()
{
    return uint8(WidgetPaintContract) | uint8(WidgetDragContract) << 1 | uint8(FocusNotificationContract) << 2 |
           uint8(WidgetReplyContract) << 3;
}
void FDasRuntimeModule::NotifyScriptsChanged(const TArray<FString>& Files, bool bFilesAddedOrRemoved)
{
    check(IsInGameThread());
    if (Files.IsEmpty() || bFilesAddedOrRemoved)
        ScriptPathCache.Reset();
    if (Files.IsEmpty())
    {
        ++ScriptSourceEpoch;
        return;
    }
    TSet<FString> Changed;
    for (const FString& File : Files)
        Changed.Add(NormalizeScriptPath(File));
    auto Mark = [&](const TArray<TUniquePtr<FPersistentSession>>& List)
    {
        for (const auto& Session : List)
            for (const auto& Source : Session->AttemptedSources)
                if (Changed.Contains(NormalizeScriptPath(Source.Key)))
                {
                    Session->CheckedEpoch = 0;
                    break;
                }
    };
    Mark(Sessions);
#if WITH_EDITOR
    Mark(PreservedSessions);
#endif
}
void FDasRuntimeModule::SetScriptWatcherActive(bool bActive)
{
    bScriptWatcherActive = bActive;
    ScriptPathCache.Reset();
    ++ScriptSourceEpoch; // Changes made while no watcher listened were not reported.
}
FDasExecutionResult FDasRuntimeModule::RunNamedPersistent(const FString& Path, const FString& EntryPoint, int32 Input,
                                                          TWeakObjectPtr<UObject> Self)
{
    if (EntryPoint.IsEmpty() || EntryPoint.Len() >= NAME_SIZE || FChar::IsDigit(EntryPoint[0]))
    {
        FDasExecutionResult Result;
        Result.Error = TEXT("Invalid entry point name");
        return Result;
    }
    for (TCHAR Ch : EntryPoint)
        if (!FChar::IsAlnum(Ch) && Ch != '_')
        {
            FDasExecutionResult Result;
            Result.Error = TEXT("Invalid entry point name");
            return Result;
        }
    return RunCallback(Path, EDasCallback::Main, Input, 0.f, Self, EntryPoint);
}

FDasExecutionResult FDasRuntimeModule::RunPersistent(const FString& Path, int32 Input, TWeakObjectPtr<UObject> Self)
{
    return RunCallback(Path, EDasCallback::Main, Input, 0.f, Self);
}

FDasExecutionResult FDasRuntimeModule::RunConstruction(const FString& Path, TWeakObjectPtr<UObject> Self)
{
    FDasExecutionResult Result;
    auto* Object = Self.Get();
    auto* Component = Cast<UDasScriptComponent>(Object);
    auto* Actor = Component ? Component->GetOwner() : Cast<AActor>(Object);
    const bool Begun = Component ? Component->HasBegunPlay() : (Actor && Actor->HasActorBegunPlay());
    if (!IsInGameThread() || !bInitialized || ConstructingObject || !IsValid(Object) || Object->IsTemplate() ||
        !IsValid(Actor) || Actor->IsTemplate() || !Actor->GetWorld() || Actor->IsActorBeingDestroyed() ||
        (Component && Component->IsBeingDestroyed()) || Begun)
    {
        Result.Error =
            TEXT("ConstructionScript requires a live Actor or ActorComponent before its BeginPlay and cannot recurse");
        return Result;
    }
#if WITH_EDITOR
    if (bPreservingSessions)
    {
        Result.bSucceeded = true;
        return Result;
    }
#endif
    // Construction contexts are temporary: never reset or reuse gameplay VMs.
    auto SavedSessions = MoveTemp(Sessions);
    ON_SCOPE_EXIT
    {
        Sessions.Empty();
        Sessions = MoveTemp(SavedSessions);
    };
    TGuardValue<bool> ConstructionScope(ConstructingObject, true);
#if WITH_EDITOR
    // Actor ProcessEvent otherwise skips Blueprint method graphs in editor worlds.
    FEditorScriptExecutionGuard ScriptExecution;
#endif
    return RunCallback(Path, EDasCallback::Construction, 0, 0.f, Self);
}

FDasExecutionResult FDasRuntimeModule::RunLifecycle(const FString& Path, EDasCallback Event,
                                                    TWeakObjectPtr<UObject> Self, float DeltaSeconds, int32 EndReason)
{
    if (Event == EDasCallback::EndPlay && IsInGameThread() && GetDasExecutionRole() == EDasExecutionRole::Gameplay &&
        !IsDasFrozenPhysicsWorld(Self.Get()))
        UDasAsyncPhysicsBridge::RevokeOwner(Self.Get());
    if (Event == EDasCallback::Main)
    {
        FDasExecutionResult Result;
        Result.Error = TEXT("Use RunPersistent for main.");
        return Result;
    }
    return RunCallback(Path, Event, EndReason, DeltaSeconds, Self);
}

FDasExecutionResult FDasRuntimeModule::RunCallback(const FString& Path, EDasCallback Event, int32 Input,
                                                   float DeltaSeconds, TWeakObjectPtr<UObject> Self,
                                                   const FString& EntryPoint, EDasMethodSignature Signature,
                                                   const TArray<FDasScalarValue>* Frame, const FString& ReturnClass)
{
    TGuardValue<const FString*> LibraryContractScope(
        LibraryInvocationContract,
        FPaths::GetCleanFilename(Path) == LibraryInvocationScript ? LibraryInvocationContract : nullptr);
    FDasExecutionResult Result;
    if (GetDasExecutionRole() == EDasExecutionRole::PhysicsPreSim || IsDasFrozenPhysicsWorld(Self.Get()))
    {
        Result.Error = TEXT("gameplay VM entry is forbidden during PhysicsPreSim");
        return Result;
    }
    ON_SCOPE_EXIT
    {
        if (AuthoredDispatchFailure && !Result.bSucceeded)
            *AuthoredDispatchFailure = Result.Error;
    };
    if (Event == EDasCallback::Main)
    {
        bool Valid = !EntryPoint.IsEmpty() && EntryPoint.Len() < NAME_SIZE && !FChar::IsDigit(EntryPoint[0]);
        for (TCHAR Ch : EntryPoint)
            Valid &= FChar::IsAlnum(Ch) || Ch == TEXT('_');
        if (!Valid)
        {
            Result.Error = TEXT("Invalid entry point name");
            return Result;
        }
    }
    const auto* LifecycleComponent = Cast<UDasScriptComponent>(Self.Get());
    const bool ActorLifecycle =
        ActorLifecycleTarget &&
        (Self.Get() == ActorLifecycleTarget ||
         (LifecycleComponent && LifecycleComponent->GetOwner() == ActorLifecycleTarget)) &&
        (Event == EDasCallback::BeginPlay || Event == EDasCallback::EndPlay || Event == EDasCallback::Construction);
    const bool ComponentLifecycle =
        ComponentLifecycleTarget && Self.Get() == ComponentLifecycleTarget &&
        (Event == EDasCallback::BeginPlay || Event == EDasCallback::EndPlay || Event == EDasCallback::Construction);
    const bool ParentLifecycle =
        ParentLifecycleTarget && Self.Get() == ParentLifecycleTarget && Event != EDasCallback::Main;
    if (!IsInGameThread() || !bInitialized ||
        (ActiveContext && !ActorLifecycle && !ComponentLifecycle && !ParentLifecycle &&
         (AuthoredDispatchDepth == 0 || Event != EDasCallback::Main)))
    {
        Result.Error =
            TEXT("Persistent calls require an initialized runtime and authorized method dispatch on the game thread.");
        return Result;
    }
#if WITH_EDITOR
    if (bPreservingSessions)
    {
        // Native teardown/reconstruction is not a gameplay lifecycle transition.
        Result.bSucceeded = Event != EDasCallback::Main;
        if (!Result.bSucceeded)
            Result.Error = TEXT("Runtime callbacks are suspended during class migration");
        return Result;
    }
#endif
    if (Event == EDasCallback::Construction && !ConstructingObject)
    {
        Result.Error = TEXT("Use RunConstruction for ConstructionScript");
        return Result;
    }
    // Nested authored UFunction calls share this construction's temporary session set.
    // They may evaluate methods, but must never deliver gameplay lifecycle callbacks.
    const bool ConstructionMethod =
        ConstructingObject && ActiveContext && AuthoredDispatchDepth > 0 && Event == EDasCallback::Main;
    if (ConstructingObject && Event != EDasCallback::Construction && !ConstructionMethod)
    {
        Result.Error = TEXT("Gameplay callbacks are forbidden during ConstructionScript");
        return Result;
    }
    DasCallPhaseMark(EDasCallPhase::Entry);
    TArray<FDasScalarValue> NormalizedFrame, PendingOutputs;
    if (Frame)
    {
        NormalizedFrame = *Frame;
        for (auto& V : NormalizedFrame)
        {
            if (V.Direction < 0 || V.Direction > 2)
            {
                Result.Error = TEXT("Invalid frame direction");
                return Result;
            }
            if (V.Direction == 1)
                V = UDasFrameLibrary::DefaultValue(V.Kind, V.ObjectClass, 1);
        }
        Frame = &NormalizedFrame;
    }
    FStructFrameRoots FrameRoots(NormalizedFrame);
    FDasCallContract Expected = FDasCallContract::Scalar(Signature);
    if (Frame)
    {
        Expected.Inputs.Reset();
        if (Frame->Num() > 16 || ((static_cast<uint8>(Signature) > 13 && Signature != EDasMethodSignature::Class &&
                                   Signature != EDasMethodSignature::Delegate &&
                                   Signature != EDasMethodSignature::Byte && Signature != EDasMethodSignature::Text) ||
                                  Signature == EDasMethodSignature::Frame))
        {
            Result.Error = TEXT("Invalid scalar frame");
            return Result;
        }
        for (const auto& V : *Frame)
        {
            if (V.Kind < 0 || (V.Kind > 13 && V.Kind != 15 && V.Kind != 16 && V.Kind != 17 && V.Kind != 18) ||
                V.Kind == 3 || V.Kind == 4)
            {
                Result.Error = TEXT("Invalid scalar argument kind");
                return Result;
            }
            if (V.Kind == 5)
                for (int32 I = 0; I < V.StringValue.Len(); ++I)
                    if (V.StringValue[I] == 0)
                    {
                        Result.Error = TEXT("embedded NUL is not supported in strings");
                        return Result;
                    }
            if ((V.Kind == 9 && V.VectorValue.ContainsNaN()) || (V.Kind == 10 && V.RotatorValue.ContainsNaN()) ||
                (V.Kind == 11 && (V.TransformValue.ContainsNaN() || !V.TransformValue.IsRotationNormalized())))
            {
                Result.Error = TEXT("Invalid/non-finite math struct input");
                return Result;
            }
            Expected.Inputs.Add(static_cast<EDasMethodSignature>(V.Kind));
        }
    }
    TArray<TStrongObjectPtr<UObject>> ObjectPins;
    if (Frame)
    {
        bool Directed = false;
        for (const auto& V : *Frame)
            Directed |= V.Direction != 0;
        if (Directed)
            for (const auto& V : *Frame)
                Expected.InputDirections.Add(V.Direction);
    }
    if (Frame)
    {
        bool Objects = false;
        for (const auto& V : *Frame)
            Objects |= V.Kind == 15 || V.Kind == 8 || V.Kind == 12 || V.Kind == 13 || V.Kind == 16;
        if (Objects)
            for (const auto& V : *Frame)
            {
                const FString PathClass =
                    (V.Kind == 12 || V.Kind == 13 || V.Kind == 15 || V.Kind == 16) ? V.ObjectClass
                                                                                   : V.Kind == 8
                        ? (V.ObjectClass.IsEmpty() ? TEXT("/Script/CoreUObject.Object"): V.ObjectClass) : FString();
                Expected.InputClasses.Add(PathClass);
                if (V.Kind == 15)
                {
                    if (!FrameClassMatches(V.ClassValue.Get(), PathClass))
                    {
                        Result.Error = TEXT("Class argument has wrong base or is obsolete");
                        return Result;
                    }
                    if (V.ClassValue)
                        ObjectPins.Emplace(V.ClassValue.Get());
                }
                if (V.Kind == 8)
                {
                    UClass* Class = FindObject<UClass>(nullptr, *PathClass);
                    UObject* Object = V.ObjectValue.Get();
                    if (!Class ||
                        (Object && (!IsValid(Object) || !DasReferenceMatches(Object, Class) ||
                                    (Object->IsA<AActor>() && CastChecked<AActor>(Object)->IsActorBeingDestroyed()))))
                    {
                        Result.Error = TEXT("Object argument is expired or has wrong class");
                        return Result;
                    }
                    if (Object)
                        ObjectPins.Emplace(Object);
                }
            }
    }
    const bool WidgetPaintEntry = WidgetPaintContract && ActiveWidgetPaint && Signature == EDasMethodSignature::Void &&
                                  Frame && Frame->Num() == 1 && (*Frame)[0].Kind == 12 &&
                                  (*Frame)[0].ObjectClass == TEXT("/Script/UMG.PaintContext");
    if (Frame)
        for (const auto& V : *Frame)
            if (V.Kind == 12)
            {
                if (WidgetPaintEntry)
                    continue;
                FString E;
                if (!V.StructValue.IsValid() || V.StructValue.GetScriptStruct()->GetPathName() != V.ObjectClass ||
                    !(DasIsGeometryType(V.StructValue.GetScriptStruct()) ? ValidateDasGeometrySnapshot(V.StructValue, E)
                      : DasIsInputEventType(V.StructValue.GetScriptStruct())
                          ? ValidateDasInputEventSnapshot(V.StructValue, E)
                          : ValidateDasStructSnapshot(V.StructValue, E)))
                {
                    Result.Error = TEXT("Invalid reflected struct input: ")+ E;
                    return Result;
                }
            }
    if (Signature == EDasMethodSignature::Delegate)
    {
        Expected.ReturnClass = ReturnClass;
        FString E;
        TStrongObjectPtr<UFunction> Sig(MakeDasSingleDelegateSignature(ReturnClass, E));
        if (!Sig)
        {
            Result.Error = E;
            return Result;
        }
    }
    if (Signature == EDasMethodSignature::Enum)
    {
        Expected.ReturnClass = ReturnClass;
        FString E;
        if (!ResolveDasEnum(ReturnClass, E))
        {
            Result.Error = E;
            return Result;
        }
    }
    if (Frame)
        for (const auto& V : *Frame)
            if (V.Kind == 13)
            {
                FString E;
                if (!ValidateDasEnumNumber(V.ObjectClass, V.IntValue, E))
                {
                    Result.Error = E;
                    return Result;
                }
            }
    const bool WidgetReplyEntry = WidgetReplyContract && ActiveWidgetReply &&
                                  Signature == EDasMethodSignature::Struct &&
                                  ReturnClass == TEXT("/Script/UMG.EventReply");
    if (Signature == EDasMethodSignature::Struct)
    {
        Expected.ReturnClass = ReturnClass;
        if (!WidgetReplyEntry)
        {
            DasContainerProperty(ReturnClass);
            FString E;
            if (!ValidateDasValueStruct(ReturnClass, E))
            {
                Result.Error = E;
                return Result;
            }
        }
    }
    if (Signature == EDasMethodSignature::Class)
    {
        Expected.ReturnClass = ReturnClass;
        if (!FrameClassMatches(nullptr, ReturnClass))
        {
            Result.Error = TEXT("Class result requires a loaded native base");
            return Result;
        }
    }
    if (Signature == EDasMethodSignature::Object)
    {
        Expected.ReturnClass = ReturnClass.IsEmpty() ? TEXT("/Script/CoreUObject.Object"): ReturnClass;
        if (!FindObject<UClass>(nullptr, *Expected.ReturnClass))
        {
            Result.Error = TEXT("Object result class is not loaded");
            return Result;
        }
    }
    FDrainEventsOnExit DrainOnExit;
    UObject* Owner = Self.Get();
    if (!IsValid(Owner) ||
        (Owner->IsA<AActor>() && CastChecked<AActor>(Owner)->IsActorBeingDestroyed() && Event != EDasCallback::EndPlay))
    {
        Result.Error = TEXT("self is missing or expired");
        return Result;
    }
    DasCallPhaseMark(EDasCallPhase::Frame);
    PruneSessions();
    DasCallPhaseMark(EDasCallPhase::PruneSessions);
    const FString FullPath = NormalizeScriptPath(Path);
    if (Event == EDasCallback::Initialize &&
        (!Owner->GetWorld() || !Owner->GetWorld()->IsGameWorld() || Owner->IsTemplate()))
    {
        Result.Error = TEXT("Initialize requires a live game-world instance");
        return Result;
    }
    if (Event != EDasCallback::EndPlay)
        if (const auto* Deliveries = InitializerDeliveries.Find(Self))
            for (const auto& Delivery : *Deliveries)
                if (Delivery.Value.State == EInitializerDelivery::Failed)
                {
                    Result.Error = Delivery.Value.Error;
                    return Result;
                }
    FPersistentSession* Session = nullptr;
    for (const auto& Existing : Sessions)
        if (Existing->Owner == Self && Existing->Path == FullPath)
        {
            Session = Existing.Get();
            break;
        }
    if (!Session && Event == EDasCallback::EndPlay)
    {
        Result.bSucceeded = true; // Never start a script during teardown.
        return Result;
    }
    if (!Session)
    {
        auto NewSession = MakeUnique<FPersistentSession>();
        NewSession->Owner = Self;
        NewSession->World = Owner->GetWorld();
        NewSession->Path = FullPath;
        Session = NewSession.Get();
        Sessions.Add(MoveTemp(NewSession));
    }
    if (Session->bEndDelivered)
    {
        Result.bSucceeded = Event == EDasCallback::EndPlay;
        if (!Result.bSucceeded)
            Result.Error = TEXT("Script lifecycle has already ended.");
        Result.Revision = Session->Revision;
        return Result;
    }
    DasCallPhaseMark(EDasCallPhase::Session);
    bool bChanged =
        Session->AttemptedSources.IsEmpty() ||
        (!Session->Live && (Session->AttemptedEntry != EntryPoint || Session->AttemptedContract != Expected));
    if (!bChanged && SourcesNeedCheck(*Session))
    {
        for (const auto& Source : Session->AttemptedSources)
            if (!Source.Value.Matches(Source.Key))
            {
                bChanged = true;
                break;
            }
        if (!bChanged)
            MarkSourcesChecked(*Session);
    }
    DasCallPhaseMark(EDasCallPhase::SourceCheck);
    if (Session->bReleaseRequested)
    {
        Result.Error = TEXT("Script session was released");
        return Result;
    }
    const bool Reentering = Session->Live && Session->Live->Context->ExecutionDepth > 0;
    if (Reentering && bChanged)
    {
        Result.Error = TEXT("Cannot reload an executing script during reentrant dispatch");
        return Result;
    }
    if (bChanged && Event != EDasCallback::EndPlay)
    {
        // Rejected sources are remembered too: no compile storm for an unchanged broken file.
        Session->AttemptedEntry = EntryPoint;
        Session->AttemptedContract = Expected;
        Session->AttemptedSources.Reset();
        Session->AttemptedSources.Add(FullPath, FSourceSnapshot::Read(FullPath));
        MarkSourcesChecked(*Session); // The compile below snapshots every source it reads.
        FString CandidateError;
        auto Candidate = MakeUnique<FCompiledContext>();
        try
        {
            das::TextWriter Output;
            auto Access = das::make_smart<FUnrealFileAccess>();
            Access->addFileSystem(new FUnrealFileSystem(&Session->AttemptedSources), true, false);
            Candidate->Access = Access;
            const FTCHARToUTF8 Utf8Path(*FullPath);
            {
                TGuardValue<FSourceSet*> ReferenceDependencies(AuthoredReferenceSources, &Session->AttemptedSources);
                Candidate->Program = DSLCompile(Utf8Path.Get(), Access, Output, Candidate->Modules);
            }
            if (!Candidate->Program || Candidate->Program->failed())
                CandidateError = Errors(Candidate->Program, Output);
            else if (!DSLValidateOwner(Candidate->Program, EntryPoint, Self.Get(), Candidate->DSLDeclaration,
                                       CandidateError))
            {
            }
            else if (Session->Live && Session->Live->DSLDeclaration != Candidate->DSLDeclaration &&
                     (Session->ApprovedDSLDeclaration.IsEmpty() ||
                      Session->ApprovedDSLDeclaration != Candidate->DSLDeclaration) &&
                     !(
                         [&]
                         {
                             if (!Session->bDefaultsUpdateApproved)
                                 return false;
                             // A default update may follow an approved declaration change (one save that
                             // adds members and changes defaults); compare with what Unreal approved.
                             FDasAuthoringModule Old, Next;
                             FString Issue;
                             const FString& Base = Session->ApprovedDSLDeclaration.IsEmpty()
                                                       ? Session->Live->DSLDeclaration
                                                       : Session->ApprovedDSLDeclaration;
                             return FDasAuthoringModule::FromJson(Base, Old, Issue) &&
                                    FDasAuthoringModule::FromJson(Candidate->DSLDeclaration, Next, Issue) &&
                                    FDasAuthoringModule::DefaultsOnly(Old, Next, Issue);
                         })())
                CandidateError = TEXT("DSL declaration changed; apply/reimport the Blueprint before runtime reload");
            else
            {
                Candidate->Context = MakeUnique<FUnrealDasContext>(Candidate->Program->getContextStackSize());
                if (Session->Live)
                    Candidate->Context->PersistentGlobals =
                        SnapshotPersistentGlobals(Session->Live->Program, *Session->Live->Context);
                FActiveContextScope Scope(*Candidate->Context);
                // No self during initialization: rejected code cannot alter the live Actor.
                if (!Candidate->Program->simulate(*Candidate->Context, Output))
                {
                    CandidateError = Errors(Candidate->Program, Output);
                    if (CandidateError.IsEmpty())
                        CandidateError = TEXT("Candidate initialization failed.");
                }
                else
                {
                    Candidate->Function = Candidate->Context->findFunction("main");
                    Candidate->Initializer = Candidate->Context->findFunction("__ue_initialize");
                    Candidate->Construction = Candidate->Context->findFunction("__ue_construct");
                    Candidate->BeginPlay = Candidate->Context->findFunction("begin_play");
                    Candidate->Tick = Candidate->Context->findFunction("tick");
                    Candidate->EndPlay = Candidate->Context->findFunction("end_play");
                    const FTCHARToUTF8 EntryUtf8(*EntryPoint);
                    auto* Selected = Event == EDasCallback::Main ? Candidate->Context->findFunction(EntryUtf8.Get())
                                                                 : Candidate->Function;
                    if ((Event == EDasCallback::Main && !Selected) ||
                        (Selected &&
                         !VerifyContract(Candidate->Program,
                                         Event == EDasCallback::Main
                                             ? EntryPoint
                                             : TEXT("main"), Event == EDasCallback::Main
                                                                 ? Expected
                                                                 : FDasCallContract::Scalar(EDasMethodSignature::Int))))
                        CandidateError =
                            TEXT("Expected exported ")+ EntryPoint + TEXT(" with requested scalar signature.");
                    else if (Candidate->Construction &&
                             !das::verifyCall<void>(Candidate->Construction->debugInfo, Candidate->Modules))
                        CandidateError = TEXT("Expected exported __ue_construct(): void.");
                    else if (Candidate->Initializer &&
                             !das::verifyCall<void>(Candidate->Initializer->debugInfo, Candidate->Modules))
                        CandidateError = TEXT("Expected exported __ue_initialize(): void.");
                    else if (Candidate->BeginPlay &&
                             !das::verifyCall<void>(Candidate->BeginPlay->debugInfo, Candidate->Modules))
                        CandidateError = TEXT("Expected exported begin_play(): void.");
                    else if (Candidate->Tick &&
                             !das::verifyCall<void, float>(Candidate->Tick->debugInfo, Candidate->Modules))
                        CandidateError = TEXT("Expected exported tick(delta_time: float): void.");
                    else if (Candidate->EndPlay &&
                             !das::verifyCall<void, int32>(Candidate->EndPlay->debugInfo, Candidate->Modules))
                        CandidateError = TEXT("Expected exported end_play(reason: int): void.");
                    if (CandidateError.IsEmpty())
                        for (const auto& Required : Session->RequiredEntries)
                        {
                            TGuardValue<bool> GeometryContractScope(
                                GeometrySnapshotDispatch,
                                GeometrySnapshotDispatch || Session->GeometrySnapshotEntries.Contains(Required.Key));
                            TGuardValue<bool> SnapshotContractScope(
                                InputSnapshotDispatch,
                                InputSnapshotDispatch || Session->InputSnapshotEntries.Contains(Required.Key));
                            TGuardValue<bool> DragContractScope(WidgetDragContract,
                                                                WidgetDragContract ||
                                                                    Session->WidgetDragEntries.Contains(Required.Key));
                            TGuardValue<bool> PaintContractScope(
                                WidgetPaintContract,
                                WidgetPaintContract || Session->WidgetPaintEntries.Contains(Required.Key));
                            TGuardValue<bool> ReplyContractScope(
                                WidgetReplyContract,
                                WidgetReplyContract || Session->WidgetReplyEntries.Contains(Required.Key));
                            TGuardValue<bool> FocusContractScope(
                                FocusNotificationContract,
                                FocusNotificationContract || Session->FocusNotificationEntries.Contains(Required.Key));
                            const FTCHARToUTF8 RequiredUtf8(*Required.Key);
                            auto* RequiredFunction = Candidate->Context->findFunction(RequiredUtf8.Get());
                            if (!RequiredFunction ||
                                !VerifyContract(Candidate->Program, Required.Key, Required.Value,
                                                Session->NativeInputEntries.Contains(Required.Key)))
                            {
                                CandidateError = TEXT("Previously used entry point signature changed: ")+ Required.Key;
                                break;
                            }
                        }
                }
                if (CandidateError.IsEmpty())
                {
                    if (Session->Live)
                        Candidate->Context->PersistentInts = Session->Live->Context->PersistentInts;
                    Candidate->Context->Self = Self;
                    auto* BindEvents = Candidate->Context->findFunction("bind_events");
                    if (BindEvents && !ConstructingObject && !Cast<UDasGASCueSession>(Self.Get()))
                    {
                        if (!das::verifyCall<void>(BindEvents->debugInfo, Candidate->Modules))
                            CandidateError = TEXT("Expected exported bind_events(): void.");
                        else
                        {
                            TGuardValue<bool> Collect(Candidate->Context->bCollectingSubscriptions, true);
                            Candidate->Context->evalWithCatch(BindEvents, nullptr);
                        }
                    }
                    for (const auto& Declaration : Candidate->Context->Subscriptions)
                    {
                        const FTCHARToUTF8 Name(*Declaration.Handler);
                        auto* Handler = Candidate->Context->findFunction(Name.Get());
                        if (!VerifyEventHandler(Handler, Candidate->Modules, Declaration.Kind))
                            CandidateError = (Declaration.Kind == EDasMethodSignature::Int
                                              ? TEXT("Expected exported event handler(value: int): void: ")
                                              : TEXT("Expected event handler matching declared scalar signature: ")) +
                                              Declaration.Handler;
                    }
                }
                if (const char* Exception = Candidate->Context->getException())
                    CandidateError = UTF8_TO_TCHAR(Exception);
            }
        }
        catch (const std::exception& Exception)
        {
            CandidateError = UTF8_TO_TCHAR(Exception.what());
        }
        if (CandidateError.IsEmpty() && Candidate->Context && Candidate->Program && !Candidate->Program->failed())
            CompileDSLPhysics(*Candidate, Session->Live.Get(), FullPath, Owner, Session->AttemptedSources,
                              ParentLifecycle, Session->bBeginDelivered && DSLPhysicsOwnerReady(Owner), CandidateError);
        if (CandidateError.IsEmpty() && Candidate->Context && Candidate->Program && !Candidate->Program->failed())
        {
            ClearSessionTimers(*Session, Candidate->Context.Get());
            Session->Bindings.Empty();
            Session->InputContexts.Empty(); // Revoke old IDs before destroying any old VM state.
            Candidate->Context->Self = Self;
            const uint64 NewPhysicsRevision =
                Candidate->Physics && Candidate->Physics->VM ? Candidate->Physics->VM->BindingRevision() : 0;
            if (Session->Revision && !ParentLifecycle && !Candidate->bPhysicsForeignSession)
                UDasAsyncPhysicsBridge::RevokeOwnerExcept(Session->Owner.Get(), NewPhysicsRevision);
            if (Session->Live)
                Session->Live->Context->ImageBrushes.Revoke();
            if (Session->Live)
            {
                Session->Live->Context->DelegateLease->Active = false;
                Session->Live->Context->OwnedScriptDelegateEvents.Empty();
            }
            Session->Live = MoveTemp(Candidate);
            if (Session->Live->bPhysicsOwner)
            {
                if (Session->Live->Physics)
                    DSLPhysicsOwners.Add(Self, Session->Live->Physics);
                else
                    DSLPhysicsOwners.Remove(Self);
            }
            Session->ApprovedDSLDeclaration.Empty();
            Session->bDefaultsUpdateApproved = false;
            InstallSubscriptions(*Session);
            // Old physics callbacks were revoked before committing the staged replacement.
            ++Session->Revision;
            Session->ReloadError.Empty();
        }
        else
        {
            Session->ReloadError = CandidateError.IsEmpty() ? TEXT("Candidate initialization failed.") : CandidateError;
            UE_LOG(LogDasRuntime, Display,
                   TEXT("Reload rejected for %s; keeping revision %u: %s"), *FullPath, Session->Revision,
                        *Session->ReloadError);
        }
    }
    DasCallPhaseMark(EDasCallPhase::Reload);
    Result.Revision = Session->Revision;
    Result.ReloadError = Session->ReloadError;
    if (Event == EDasCallback::EndPlay && (!Session->Live || !Session->bBeginDelivered || Session->bEndDelivered))
    {
        if (Session->Live)
        {
            Session->Live->Context->DelegateLease->Active = false;
            Session->Live->Context->OwnedScriptDelegateEvents.Empty();
        }
        ClearSessionTimers(*Session);
        Session->Bindings.Empty();
        Session->InputContexts.Empty();
        Session->InputRecipes.Empty();
        Session->ContextRecipes.Empty();
        Session->bEndDelivered = true;
        Result.bSucceeded = true;
        return Result;
    }
    if (!Session->Live)
    {
        Result.Error = Session->ReloadError;
        return Result;
    }
    if (Session->bEndDelivered)
    {
        Result.Error = TEXT("Script lifecycle has already ended.");
        return Result;
    }
    auto& Context = *Session->Live->Context;
    if (ExpectedDelegateGeneration &&
        (Context.DelegateLease.Get() != ExpectedDelegateGeneration || !ExpectedDelegateGeneration->Active))
    {
        Result.Error = TEXT("Script delegate VM generation was revoked during reload");
        return Result;
    }
    if (!Reentering)
    {
        Context.restart();
        Context.Types.Reset();
    }
    TGuardValue<int32> ExecutionScope(Context.ExecutionDepth, Context.ExecutionDepth + 1);
    FActiveContextScope Scope(Context);
    FStructCollectOnExit CollectStructs{Context};
    TGuardValue<bool> EndingScope(Context.bEnding, Event == EDasCallback::EndPlay);
    try
    {
        if (Event == EDasCallback::Construction)
        {
            if (Session->Live->Construction)
                Context.evalWithCatch(Session->Live->Construction, nullptr);
            if (const char* Exception = Context.getException())
            {
                Result.Error = FString(TEXT("ConstructionScript failed: ")) + UTF8_TO_TCHAR(Exception);
                return Result;
            }
            Result.bSucceeded = true;
            return Result;
        }
        if (ExplicitParentTick && Event == EDasCallback::Tick && Session->Live->BeginPlay && !Session->bBeginDelivered)
        {
            Result.Error = TEXT("Call Super.BeginPlay before parent Tick");
            return Result;
        }
        // Parent BeginPlay does not implicitly opt back into an overridden initializer.
        const bool CanInitialize = Owner->GetWorld() && Owner->GetWorld()->IsGameWorld() && !Owner->IsTemplate();
        if (!CanInitialize && Session->Live->Initializer &&
            (Event == EDasCallback::BeginPlay || Event == EDasCallback::Tick))
        {
            Result.Error = TEXT("Initialize requires a live game-world instance");
            return Result;
        }
        const bool NeedInitialize =
            Event == EDasCallback::Initialize ||
            (CanInitialize && !ParentLifecycle && (Event == EDasCallback::BeginPlay || Event == EDasCallback::Tick));
        if (NeedInitialize)
        {
            if (!Owner->GetWorld() || !Owner->GetWorld()->IsGameWorld() || Owner->IsTemplate())
            {
                Result.Error = TEXT("Initialize requires a live game-world instance");
                return Result;
            }
            const auto* ExistingOwner = InitializerDeliveries.Find(Self);
            const auto* Existing = ExistingOwner ? ExistingOwner->Find(FullPath) : nullptr;
            if (Existing && Existing->State == EInitializerDelivery::Running)
            {
                Result.Error = TEXT("Recursive object initialization is forbidden");
                return Result;
            }
            if (!Existing)
            {
                InitializerDeliveries.FindOrAdd(Self).Add(FullPath, FInitializerDelivery{});
                bool Initialized = false;
                ON_SCOPE_EXIT
                {
                    if (!Initialized)
                    {
                        auto& Delivery = InitializerDeliveries.FindOrAdd(Self).FindOrAdd(FullPath);
                        Delivery.State = EInitializerDelivery::Failed;
                        Delivery.Error =
                            Result.Error.IsEmpty() ? TEXT("Object initialization failed; recreate the instance")
                                                   : Result.Error;
                        for (const auto& S : Sessions)
                            if (S->Owner == Self)
                            {
                                ClearSessionTimers(*S);
                                S->Bindings.Empty();
                                S->InputContexts.Empty();
                                S->InputRecipes.Empty();
                                S->ContextRecipes.Empty();
                            }
                    }
                };
                TGuardValue<bool> Initializing(Context.bInitializingObject, true);
                if (Session->Live->Initializer)
                    Context.evalWithCatch(Session->Live->Initializer, nullptr);
                if (const char* Exception = Context.getException())
                {
                    Result.Error = FString(TEXT("Initialize failed: ")) + UTF8_TO_TCHAR(Exception);
                    return Result;
                }
                if (!IsValid(Owner) || (Cast<AActor>(Owner) && CastChecked<AActor>(Owner)->IsActorBeingDestroyed()) ||
                    (Cast<UActorComponent>(Owner) && CastChecked<UActorComponent>(Owner)->IsBeingDestroyed()))
                {
                    Result.Error = TEXT("Owner destroyed during Initialize");
                    return Result;
                }
                InitializerDeliveries.FindOrAdd(Self).FindOrAdd(FullPath).State = EInitializerDelivery::Complete;
                Initialized = true;
            }
            if (Event == EDasCallback::Initialize)
            {
                Result.bSucceeded = true;
                return Result;
            }
        }
        // Delivery flags belong to the session, so swapping code cannot replay BeginPlay.
        if ((Event == EDasCallback::BeginPlay || Event == EDasCallback::Tick) && !Session->bBeginDelivered)
        {
            Session->bBeginDelivered = true;
            if (Session->Live->BeginPlay)
                Context.evalWithCatch(Session->Live->BeginPlay, nullptr);
            if (const char* Exception = Context.getException())
            {
                Result.Error = UTF8_TO_TCHAR(Exception);
                if (Session->Live->Physics)
                {
                    Session->Live->Physics->Activated = true;
                    Session->Live->Physics->ActivationError =
                        TEXT("Async Physics registration blocked after failed BeginPlay: ")+ Result.Error;
                }
                return Result;
            }
        }
        DasCallPhaseMark(EDasCallPhase::PrepareVM);
        das::SimFunction* Function = nullptr;
        vec4f Arguments[] = {
            Event == EDasCallback::Tick || (Event == EDasCallback::Main && Signature == EDasMethodSignature::Float)
                ? das::cast<float>::from(DeltaSeconds)
            : Event == EDasCallback::Main && Signature == EDasMethodSignature::Bool ? das::cast<bool>::from(Input != 0)
                                                                                    : das::cast<int32>::from(Input)};
        if (Event == EDasCallback::Main)
        {
            const uint8 Flags = WidgetContractFlags();
            const auto* Verified = Session->Live->VerifiedEntries.Find(EntryPoint);
            if (Verified && Verified->Flags == Flags && Verified->Contract == Expected)
                Function = Verified->Function;
            else
            {
                const FTCHARToUTF8 EntryUtf8(*EntryPoint);
                Function = Context.findFunction(EntryUtf8.Get());
                if (!Function || !VerifyContract(Session->Live->Program, EntryPoint, Expected))
                {
                    Result.Error = TEXT("Expected exported ") + EntryPoint + TEXT(" with requested scalar signature.");
                    return Result;
                }
                Session->Live->VerifiedEntries.Add(EntryPoint, {Function, Expected, Flags});
            }
            Session->RequiredEntries.Add(EntryPoint, Expected);
            if (WidgetDragContract && ActiveWidgetDrag)
                Session->WidgetDragEntries.Add(EntryPoint);
            if (WidgetPaintEntry)
                Session->WidgetPaintEntries.Add(EntryPoint);
            if (GeometrySnapshotDispatch)
                Session->GeometrySnapshotEntries.Add(EntryPoint);
            if (InputSnapshotDispatch)
                Session->InputSnapshotEntries.Add(EntryPoint);
            if (WidgetReplyEntry)
                Session->WidgetReplyEntries.Add(EntryPoint);
            if (FocusNotificationContract && Signature == EDasMethodSignature::Void && Frame && Frame->Num() == 1 &&
                (*Frame)[0].ObjectClass == TEXT("/Script/SlateCore.FocusEvent"))
                Session->FocusNotificationEntries.Add(EntryPoint);
        }
        else if (Event == EDasCallback::Tick)
            Function = Session->Live->Tick;
        else if (Event == EDasCallback::EndPlay)
        {
            ClearSessionTimers(*Session);
            Session->Live->Context->DelegateLease->Active = false;
            Session->Live->Context->OwnedScriptDelegateEvents.Empty();
            Session->Live->Context->DragLifetime.Revoke();
            Session->Live->Context->ImageBrushes.Revoke();
            Session->bEndDelivered = true;
            Session->Bindings.Empty();
            Session->InputContexts.Empty();
            Session->InputRecipes.Empty();
            Session->ContextRecipes.Empty();
            Function = Session->Live->EndPlay;
        }
        if (Function)
        {
            DasCallPhaseMark(EDasCallPhase::Function);
            FDasPaintValue PaintArgument{WidgetPaintEntry ? ActiveWidgetPaint->Epoch : 0};
            das::vector<vec4f> Packed;
            TArray<FDasObjectHandle> ObjectArgs;
            if (Frame)
                ObjectArgs.Reserve(Frame->Num());
            FDasStructValue ReturnedStructToken;
            TArray<FDasStructValue> StructArgs;
            if (Frame)
                StructArgs.Reserve(Frame->Num());
            FDasObjectHandle ReturnedObject;
            TArray<FVector> VectorArgs;
            TArray<FRotator> RotatorArgs;
            TArray<FTransform> TransformArgs;
            if (Frame)
            {
                VectorArgs.Reserve(Frame->Num());
                RotatorArgs.Reserve(Frame->Num());
                TransformArgs.Reserve(Frame->Num());
            }
            FVector ReturnedVector = FVector::ZeroVector;
            FRotator ReturnedRotator = FRotator::ZeroRotator;
            FTransform ReturnedTransform = FTransform::Identity;
            if (Frame)
                for (const auto& V : *Frame)
                {
                    if (V.Kind == 17)
                    {
                        Packed.push_back(das::cast<uint8_t>::from(V.ByteValue));
                    }
                    else if (V.Kind == 18)
                    {
                        Packed.push_back(das::cast<FDasTextValue>::from(MakeDSLText(V.TextValue, &Context)));
                    }
                    else if (V.Kind == 15)
                    {
                        Packed.push_back(das::cast<FDasClassHandle>::from(FDasClassHandle{
                            V.ClassValue ? MakeObjectHandle(V.ClassValue.Get(), UClass::StaticClass(), &Context).Token
                                         : 0}));
                    }
                    else if (V.Kind == 16)
                    {
                        auto D = ResolveDelegateValue(0, TCHAR_TO_UTF8(*V.ObjectClass), false, &Context);
                        D.Single = V.DelegateValue;
                        Packed.push_back(das::cast<uint64>::from(StoreDelegateValue(MoveTemp(D), &Context)));
                    }
                    else if (V.Kind == 0)
                        Packed.push_back(das::cast<int32>::from(V.IntValue));
                    else if (V.Kind == 1)
                        Packed.push_back(das::cast<float>::from(V.FloatValue));
                    else if (V.Kind == 2)
                        Packed.push_back(das::cast<bool>::from(V.BoolValue));
                    else if (V.Kind == 5)
                        Packed.push_back(das::cast<char*>::from(CopyDSLString(V.StringValue, &Context)));
                    else if (V.Kind == 6)
                        Packed.push_back(das::cast<double>::from(V.DoubleValue));
                    else if (V.Kind == 13)
                        Packed.push_back(
                            das::cast<FDasEnumValue>::from(MakeEnumValue(V.ObjectClass, V.IntValue, &Context)));
                    else if (V.Kind == 7)
                        Packed.push_back(das::cast<FDasNameValue>::from(MakeDSLName(V.NameValue, &Context)));
                    else if (V.Kind == 9)
                    {
                        VectorArgs.Add(V.VectorValue);
                        Packed.push_back(das::cast<void*>::from(&VectorArgs.Last()));
                    }
                    else if (V.Kind == 10)
                    {
                        RotatorArgs.Add(V.RotatorValue);
                        Packed.push_back(das::cast<void*>::from(&RotatorArgs.Last()));
                    }
                    else if (V.Kind == 12 && WidgetPaintEntry)
                    {
                        Packed.push_back(das::cast<void*>::from(&PaintArgument));
                    }
                    else if (V.Kind == 12)
                    {
                        StructArgs.Add(StoreStructValue(V.StructValue, &Context));
                        if (WidgetReplyEntry && DasIsInputEventType(V.StructValue.GetScriptStruct()))
                            ActiveWidgetReply->NavigationInputToken = StructArgs.Last().Token;
                        if (WidgetReplyEntry && V.ObjectClass == TEXT("/Script/SlateCore.PointerEvent"))
                            ActiveWidgetReply->PointerToken = StructArgs.Last().Token;
                        Packed.push_back(das::cast<void*>::from(&StructArgs.Last()));
                    }
                    else if (V.Kind == 11)
                    {
                        TransformArgs.Add(V.TransformValue);
                        Packed.push_back(das::cast<void*>::from(&TransformArgs.Last()));
                    }
                    else
                    {
                        const FString ClassPath =
                            V.ObjectClass.IsEmpty() ? TEXT("/Script/CoreUObject.Object"): V.ObjectClass;
                        ObjectArgs.Add(
                            MakeObjectHandle(V.ObjectValue.Get(), FindObject<UClass>(nullptr, *ClassPath), &Context));
                        Packed.push_back(das::cast<void*>::from(&ObjectArgs.Last()));
                    }
                }
            FOutputFrame OutFrame;
            OutFrame.Arguments = Frame;
            TGuardValue<FOutputFrame*> OutScope(ActiveOutputFrame, &OutFrame);
            das::Context* NoDragContext = nullptr;
            TGuardValue<das::Context*> DragContextScope(ActiveWidgetDrag ? ActiveWidgetDrag->Context : NoDragContext,
                                                        ActiveWidgetDrag ? &Context : nullptr);
            if (ActiveWidgetDrag && ActiveWidgetDrag->Operation.IsValid())
            {
                if (!ActiveWidgetDrag->Operation->IsLive() ||
                    !Context.DragLifetime.Track(ActiveWidgetDrag->Operation.Get()))
                {
                    Result.Error = TEXT("Drag operation expired or lifetime limit exceeded");
                    return Result;
                }
            }
            das::Context* NoReplyContext = nullptr;
            TGuardValue<das::Context*> ReplyContextScope(WidgetReplyEntry ? ActiveWidgetReply->Context : NoReplyContext,
                                                         WidgetReplyEntry ? &Context : nullptr);
            das::Context* NoPaintContext = nullptr;
            TGuardValue<das::Context*> PaintContextScope(WidgetPaintEntry ? ActiveWidgetPaint->Context : NoPaintContext,
                                                         WidgetPaintEntry ? &Context : nullptr);
            ON_SCOPE_EXIT
            {
                if (WidgetPaintEntry)
                    for (auto It = Context.Calls.CreateIterator(); It; ++It)
                        if (It.Value()->PaintEpoch == ActiveWidgetPaint->Epoch)
                            It.RemoveCurrent();
            };
            ON_SCOPE_EXIT
            {
                if (WidgetReplyEntry)
                    for (auto It = Context.Calls.CreateIterator(); It; ++It)
                        if (It.Value()->WidgetReplyEpoch == ActiveWidgetReply->Epoch)
                            It.RemoveCurrent();
            };
            DasCallPhaseMark(EDasCallPhase::Arguments);
            const auto Value = Context.evalWithCatch(
                Function,
                Frame ? (Packed.empty() ? nullptr : Packed.data())
                : Event == EDasCallback::Main && Signature == EDasMethodSignature::Void ? nullptr
                                                                                        : Arguments,
                Signature == EDasMethodSignature::Struct      ? static_cast<void*>(&ReturnedStructToken)
                : Signature == EDasMethodSignature::Object    ? static_cast<void*>(&ReturnedObject)
                : Signature == EDasMethodSignature::Vector    ? static_cast<void*>(&ReturnedVector)
                : Signature == EDasMethodSignature::Rotator   ? static_cast<void*>(&ReturnedRotator)
                : Signature == EDasMethodSignature::Transform ? static_cast<void*>(&ReturnedTransform)
                                                              : nullptr);
            DasCallPhaseMark(EDasCallPhase::Eval);
            if (Event == EDasCallback::Main)
            {
                if (Signature == EDasMethodSignature::Byte && !Context.getException())
                {
                    Result.ByteValue = das::cast<uint8_t>::to(Value);
                }
                else if (Signature == EDasMethodSignature::Text && !Context.getException())
                {
                    Result.TextValue = ResolveDSLText(das::cast<FDasTextValue>::to(Value), &Context);
                }
                else if (Signature == EDasMethodSignature::Class && !Context.getException())
                {
                    auto H = das::cast<FDasClassHandle>::to(Value);
                    UObject* O = H.Token ? TryObject(FDasObjectHandle{H.Token}, &Context) : nullptr;
                    auto* K = Cast<UClass>(O);
                    if ((H.Token && !K) || !FrameClassMatches(K, Expected.ReturnClass))
                    {
                        Result.Error = TEXT("Class result is expired, belongs to another context or has wrong base");
                        return Result;
                    }
                    Result.ObjectValue = K;
                }
                else if (Signature == EDasMethodSignature::Delegate && !Context.getException())
                {
                    Result.DelegateValue = ResolveDelegateValue(das::cast<uint64>::to(Value),
                                                                TCHAR_TO_UTF8(*Expected.ReturnClass), false, &Context)
                                               .Single;
                }
                else if (Signature == EDasMethodSignature::Enum && !Context.getException())
                {
                    const auto& E = ResolveEnumValue(das::cast<FDasEnumValue>::to(Value), &Context);
                    if (E.Type->GetPathName() != Expected.ReturnClass)
                    {
                        Result.Error = TEXT("Enum result type mismatch");
                        return Result;
                    }
                    Result.Value = E.Value;
                }
                else if (WidgetReplyEntry && !Context.getException())
                {
                    auto Reply = ReadWidgetReply({ReturnedStructToken.Token}, &Context);
                    for (const auto& Target : ActiveWidgetReply->Targets)
                        if (!Target.IsValid() || Target->GetWorld() != Owner->GetWorld())
                        {
                            Result.Error = TEXT("EventReply target expired during callback");
                            return Result;
                        }
                    if (Session->bReleaseRequested || !ActiveWidgetReply->Widget.IsValid() ||
                        !ActiveWidgetReply->Widget->GetCachedWidget().IsValid() || !Owner->GetWorld() ||
                        Owner->GetWorld()->bIsTearingDown)
                    {
                        Result.Error = TEXT("Widget expired during callback");
                        return Result;
                    }
                    ActiveWidgetReply->Result = Reply;
                    ActiveWidgetReply->Produced = true;
                }
                else if (Signature == EDasMethodSignature::Struct && !Context.getException())
                {
                    const auto& S = ResolveStructValue(ReturnedStructToken, &Context);
                    if (S.GetScriptStruct()->GetPathName() != Expected.ReturnClass)
                    {
                        Result.Error = TEXT("Reflected struct result type mismatch");
                        return Result;
                    }
                    Result.StructValue = S;
                }
                else if (Signature == EDasMethodSignature::Vector)
                    Result.VectorValue = ReturnedVector;
                else if (Signature == EDasMethodSignature::Rotator)
                    Result.RotatorValue = ReturnedRotator;
                else if (Signature == EDasMethodSignature::Transform)
                    Result.TransformValue = ReturnedTransform;
                else if (Signature == EDasMethodSignature::Int)
                    Result.Value = das::cast<int32>::to(Value);
                else if (Signature == EDasMethodSignature::Float)
                    Result.FloatValue = das::cast<float>::to(Value);
                else if (Signature == EDasMethodSignature::Bool)
                    Result.BoolValue = das::cast<bool>::to(Value);
                else if (!Context.getException() && Signature == EDasMethodSignature::String)
                {
                    const auto* Text = das::cast<char*>::to(Value);
                    Result.StringValue = Text ? UTF8_TO_TCHAR(Text) : TEXT("");
                }
                else if (Signature == EDasMethodSignature::Double)
                    Result.DoubleValue = das::cast<double>::to(Value);
                else if (!Context.getException() && Signature == EDasMethodSignature::Name)
                    Result.NameValue = ResolveDSLName(das::cast<FDasNameValue>::to(Value), &Context);
                else if (!Context.getException() && Signature == EDasMethodSignature::Object)
                {
                    UObject* Object = ReturnedObject.Token ? TryObject(ReturnedObject, &Context) : nullptr;
                    if (ReturnedObject.Token && !Object)
                    {
                        Result.Error = TEXT("Object result is expired or belongs to another context");
                        return Result;
                    }
                    UClass* Class = FindObject<UClass>(nullptr, *Expected.ReturnClass);
                    if (Object && !DasReferenceMatches(Object, Class))
                    {
                        Result.Error = TEXT("Object result has wrong class");
                        return Result;
                    }
                    Result.ObjectValue = Object;
                }
            }
            if (ActiveWidgetDrag && !Context.getException())
            {
                auto& D = *ActiveWidgetDrag;
                if (Session->bReleaseRequested || !D.Widget.IsValid() || !Owner->GetWorld() ||
                    Owner->GetWorld()->bIsTearingDown || !D.Widget->GetCachedWidget().IsValid() ||
                    (D.Operation.IsValid() && !D.Operation->IsLive()))
                {
                    Result.Error = TEXT("Widget or drag operation expired during callback");
                    return Result;
                }
                if (D.Event == TEXT("OnDragDetected") && (Result.ObjectValue.Get() != D.Created.Get() ||
                                                          (D.Created && !D.Created->IsLive())))
                {
                    Result.Error = TEXT("OnDragDetected must return its newly created operation or null");
                    return Result;
                }
                D.Succeeded = true;
            }
            if (WidgetPaintEntry && !Context.getException())
            {
                if (Session->bReleaseRequested || !ActiveWidgetPaint->Widget.IsValid() || !Owner->GetWorld() ||
                    Owner->GetWorld()->bIsTearingDown || !ActiveWidgetPaint->Widget->GetCachedWidget().IsValid())
                {
                    Result.Error = TEXT("Widget expired during OnPaint");
                    return Result;
                }
                ActiveWidgetPaint->Succeeded = true;
            }
            if (Frame && !Context.getException())
            {
                TArray<FDasScalarValue> Published = *Frame;
                for (int32 I = 0; I < Frame->Num(); ++I)
                    if ((*Frame)[I].Direction)
                    {
                        auto* Out = OutFrame.Values.Find(I);
                        if (!Out)
                        {
                            Result.Error = TEXT("Declared output slot not published");
                            return Result;
                        }
                        Published[I] = *Out;
                    }
                if (!Expected.InputDirections.IsEmpty())
                    PendingOutputs = MoveTemp(Published);
            }
        }
        if ((Signature == EDasMethodSignature::Vector && Result.VectorValue.ContainsNaN()) ||
            (Signature == EDasMethodSignature::Rotator && Result.RotatorValue.ContainsNaN()) ||
            (Signature == EDasMethodSignature::Transform &&
             (Result.TransformValue.ContainsNaN() || !Result.TransformValue.IsRotationNormalized())))
        {
            Result.Error = TEXT("Invalid/non-finite math struct result");
            return Result;
        }
        if (const char* Exception = Context.getException())
            Result.Error = UTF8_TO_TCHAR(Exception);
        else
        {
            if (!ParentLifecycle && (Event == EDasCallback::BeginPlay || Event == EDasCallback::Tick) &&
                Session->Live->Physics && DSLPhysicsOwnerReady(Owner) &&
                !ActivateDSLPhysics(*Session->Live->Physics, Owner, Result.Error))
                return Result;
            Result.Types = Context.Types;
            Result.bSucceeded = true;
            Result.OutputValues = MoveTemp(PendingOutputs);
            DasCallPhaseMark(EDasCallPhase::Result);
        }
    }
    catch (const std::exception& Exception)
    {
        Result.Error = UTF8_TO_TCHAR(Exception.what());
    }
    return Result;
}
