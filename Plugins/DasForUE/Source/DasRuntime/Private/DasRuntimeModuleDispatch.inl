// Part of DasRuntime.cpp: FDasRuntimeModule: catalog/registry reads and event dispatch entry points.
// Included in place; declaration order matters.
namespace
{
bool CollectDasCatalogModules(das::Program* Program, const FDasRuntimeModule& Runtime, FDasSourceCatalog& Out,
                              FString& Error)
{
    bool OK = true;
    FDasSourceCatalog Discovered;
    Program->library.foreach (
        [&](das::Module* Module)
        {
            if (!OK)
                return false;
            FDasCatalogValueModule Values;
            Values.Module = DSLText(Module->name);
            bool HasClass = false;
            Module->structures.foreach ([&](const das::StructurePtr& S)
                                        { HasClass |= DSLAnnotation(S->annotations, "ue_class") != nullptr; });
            auto Collect = [&](const das::AnnotationList& Annotations, const char* Kind, const das::LineInfo& At)
            {
                const auto* A = DSLAnnotation(Annotations, Kind);
                if (!A || !OK)
                    return;
                const auto* Wire = A->arguments.find(DSLTypesKey, das::Type::tString);
                FDasReflectedTypes Part;
                if (!Wire || !FDasReflectedTypes::FromJson(DSLText(Wire->sValue), Part, Error, true) || !At.fileInfo)
                {
                    if (Error.IsEmpty())
                        Error = TEXT("Missing imported value type source/identity");
                    OK = false;
                    return;
                }
                FString Source = FPaths::ConvertRelativePathToFull(DSLText(At.fileInfo->name));
                FPaths::NormalizeFilename(Source);
                FPaths::CollapseRelativeDirectories(Source);
                const FString File = FPaths::GetCleanFilename(Source);
                FString Expected = FPaths::ConvertRelativePathToFull(Runtime.ScriptPath(File));
                FPaths::NormalizeFilename(Expected);
                FPaths::CollapseRelativeDirectories(Expected);
                if (!FDasSourceCatalog::LocalFile(File, TEXT("das")) ||
                                                  !Source.Equals(Expected, ESearchCase::IgnoreCase) ||
                                                  (!Values.Script.IsEmpty() && Values.Script != File))
                {
                    Error = TEXT("Imported authored values require one local Scripts source per module: ")+ Source;
                    OK = false;
                    return;
                }
                Values.Script = File;
                for (const auto& E : Part.Enums)
                {
                    FDasCatalogValueType T;
                    T.Kind = TEXT("enum");
                    T.Id = E.Id;
                    T.Name = E.Name;
                    Values.Types.Add(MoveTemp(T));
                }
                for (const auto& E : Part.Structs)
                {
                    FDasCatalogValueType T;
                    T.Kind = TEXT("struct");
                    T.Id = E.Id;
                    T.Name = E.Name;
                    Values.Types.Add(MoveTemp(T));
                }
            };
            Module->enumerations.foreach ([&](const das::EnumerationPtr& E)
                                          { Collect(E->annotations, "ue_enum", E->at); });
            Module->structures.foreach ([&](const das::StructurePtr& S)
                                        { Collect(S->annotations, "ue_struct", S->at); });
            auto SourceFor = [&](const das::LineInfo& At)
            {
                if (!At.fileInfo)
                {
                    Error = TEXT("Missing catalog interface source");
                    OK = false;
                    return false;
                }
                FString Source = FPaths::ConvertRelativePathToFull(DSLText(At.fileInfo->name));
                FPaths::NormalizeFilename(Source);
                FPaths::CollapseRelativeDirectories(Source);
                const FString File = FPaths::GetCleanFilename(Source);
                FString Expected = FPaths::ConvertRelativePathToFull(Runtime.ScriptPath(File));
                FPaths::NormalizeFilename(Expected);
                FPaths::CollapseRelativeDirectories(Expected);
                if (!FDasSourceCatalog::LocalFile(File, TEXT("das")) ||
                                                  !Source.Equals(Expected, ESearchCase::IgnoreCase) ||
                                                  (!Values.Script.IsEmpty() && Values.Script != File))
                {
                    Error = TEXT("Catalog interface modules require a local Scripts source: ")+ Source;
                    OK = false;
                    return false;
                }
                Values.Script = File;
                return true;
            };
            Module->structures.foreach (
                [&](const das::StructurePtr& S)
                {
                    if (!OK)
                        return;
                    if (const auto* A = DSLAnnotation(S->annotations, "ue_interface"))
                    {
                        const auto* Wire = A->arguments.find(DSLInterfaceKey, das::Type::tString);
                        FDasAuthoringModule Model;
                        if (!Wire || !FDasAuthoringModule::FromJson(DSLText(Wire->sValue), Model, Error) ||
                            !SourceFor(S->at))
                        {
                            OK = false;
                            return;
                        }
                        for (const auto& C : Model.Classes)
                        {
                            FDasCatalogValueType T;
                            T.Kind = TEXT("interface");
                            T.Id = C.Id;
                            T.Name = C.Name;
                            Values.Types.Add(MoveTemp(T));
                        }
                    }
                    if (const auto* A = DSLAnnotation(S->annotations, "ue_interface_ref"))
                    {
                        if (!SourceFor(S->at))
                            return;
                        FDasCatalogValueType T;
                        T.Kind = TEXT("interface_ref");
                        T.Id = DSLString(A->arguments, "id");
                        T.Name = DSLText(S->name);
                        Values.Types.Add(MoveTemp(T));
                    }
                });
            if (!Values.Types.IsEmpty())
            {
                if (HasClass)
                {
                    Error =
                        TEXT("Automatic catalog imports require dedicated type/interface modules without ue_class: ")+
                             Values.Module;
                    OK = false;
                    return false;
                }
                Discovered.ValueModules.Add(MoveTemp(Values));
            }
            return OK;
        },
        "*");
    if (!OK)
        return false;
    Discovered.ValueModules.Sort([](const auto& A, const auto& B) { return A.Module < B.Module; });
    Out = MoveTemp(Discovered);
    return true;
}
}

bool FDasRuntimeModule::ReadAuthoringScript(const FString& Path, FDasAuthoringModule& Out, FString& Error,
                                            FDasSourceCatalog* ImportedValues)
{
    Error.Reset();
    if (!IsInGameThread() || !bInitialized)
    {
        Error = TEXT("DSL compilation requires initialized runtime on game thread");
        return false;
    }
    try
    {
        das::TextWriter Output;
        das::ModuleGroup Group;
        auto Access = das::make_smart<FUnrealFileAccess>();
        Access->addFileSystem(new FUnrealFileSystem(), true, false);
        const FTCHARToUTF8 File(*Path);
        auto Program = DSLCompile(File.Get(), Access, Output, Group);
        if (!Program || Program->failed())
        {
            Error = Errors(Program, Output);
            return false;
        }
        FDasAuthoringModule Candidate;
        bool OK = true;
        Program->getThisModule()->structures.foreach (
            [&](const das::StructurePtr& St)
            {
                const auto* A = DSLAnnotation(St->annotations, "ue_class");
                if (!A)
                    A = DSLAnnotation(St->annotations, "ue_library");
                if (!A || !OK)
                    return;
                const auto* Wire = A->arguments.find(DSLModelKey, das::Type::tString);
                FDasAuthoringModule Part;
                if (!Wire || !FDasAuthoringModule::FromJson(DSLText(Wire->sValue), Part, Error))
                {
                    OK = false;
                    return;
                }
                if (!Candidate.Id.IsEmpty() && Candidate.Id != Part.Id)
                {
                    Error = TEXT("Declarations in a file must share module ID");
                    OK = false;
                    return;
                }
                Candidate.Id = Part.Id;
                for (const auto& C : Part.Classes)
                {
                    if (!Candidate.Classes.ContainsByPredicate([&](const auto& P) { return P.Id == C.Id; }))
                        Candidate.Classes.Add(C);
                }
            });
        if (!OK || !Candidate.Validate(Error))
            return false;
        FDasSourceCatalog Discovered;
        if (ImportedValues && !CollectDasCatalogModules(Program.get(), *this, Discovered, Error))
            return false;
        if (ImportedValues)
            *ImportedValues = MoveTemp(Discovered);
        Out = MoveTemp(Candidate);
        return true;
    }
    catch (const std::exception& E)
    {
        Error = UTF8_TO_TCHAR(E.what());
        return false;
    }
}

bool FDasRuntimeModule::ReadSourceCatalogImports(const FString& Path, FDasSourceCatalog& Out, FString& Error)
{
    Error.Reset();
    if (!IsInGameThread() || !bInitialized)
    {
        Error = TEXT("Catalog import discovery requires initialized game-thread runtime");
        return false;
    }
    try
    {
        das::TextWriter Text;
        das::ModuleGroup Group;
        auto Access = das::make_smart<FUnrealFileAccess>();
        Access->addFileSystem(new FUnrealFileSystem(), true, false);
        const FTCHARToUTF8 File(*Path);
        auto Program = DSLCompile(File.Get(), Access, Text, Group);
        if (!Program || Program->failed())
        {
            Error = Errors(Program, Text);
            return false;
        }
        FDasSourceCatalog Candidate;
        if (!CollectDasCatalogModules(Program.get(), *this, Candidate, Error))
            return false;
        if (Candidate.ValueModules.IsEmpty())
        {
            Error = TEXT("No authored enum/struct/interface imports found");
            return false;
        }
        Out = MoveTemp(Candidate);
        return true;
    }
    catch (const std::exception& E)
    {
        Error = UTF8_TO_TCHAR(E.what());
        return false;
    }
}

bool FDasRuntimeModule::ValidateIntMethod(const FString& Path, FString& Error, const FString& EntryPoint)
{
    return ValidateScalarMethod(Path, Error, EntryPoint, EDasMethodSignature::Int);
}
bool FDasRuntimeModule::ValidateScalarMethod(const FString& Path, FString& Error, const FString& EntryPoint,
                                             EDasMethodSignature Signature)
{
    return ValidateFrameMethod(Path, Error, EntryPoint, FDasCallContract::Scalar(Signature));
}
bool FDasRuntimeModule::ValidateFrameMethod(const FString& Path, FString& Error, const FString& EntryPoint,
                                            const FDasCallContract& Contract)
{
    Error.Reset();
    if (!IsInGameThread() || !bInitialized)
    {
        Error = TEXT("Method preflight requires initialized runtime on game thread");
        return false;
    }
    try
    {
        das::TextWriter Output;
        das::ModuleGroup Modules;
        auto Access = das::make_smart<FUnrealFileAccess>();
        Access->addFileSystem(new FUnrealFileSystem(), true, false);
        const FTCHARToUTF8 Utf8Path(*Path);
        das::CodeOfPolicies Policies;
        auto Program = DSLCompile(Utf8Path.Get(), Access, Output, Modules, Policies);
        if (!Program || Program->failed())
        {
            Error = Errors(Program, Output);
            return false;
        }
        if (!VerifyContract(Program, EntryPoint, Contract))
        {
            Error = TEXT("Expected exported scalar frame signature by value: ")+ EntryPoint;
            return false;
        }
        // No simulate/eval: runtime global initializers and the method body are not executed during preflight.
        return true;
    }
    catch (const std::exception& Exception)
    {
        Error = UTF8_TO_TCHAR(Exception.what());
        return false;
    }
}

FDasExecutionResult FDasRuntimeModule::RunFile(const FString& Path, int32 Input, bool bUseAot,
                                               TWeakObjectPtr<UObject> Self)
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
    if (!IsInGameThread() || !bInitialized)
    {
        Result.Error = TEXT("daScript calls require an initialized runtime on the game thread.");
        return Result;
    }
#if WITH_EDITOR
    if (bPreservingSessions)
    {
        Result.Error = TEXT("Runtime callbacks are suspended during class migration");
        return Result;
    }
#endif
    FDrainEventsOnExit DrainOnExit;
    try
    {
        das::TextWriter Output;
        das::ModuleGroup Modules;
        auto Access = das::make_smart<FUnrealFileAccess>();
        Access->addFileSystem(new FUnrealFileSystem(), true, false);
        const FTCHARToUTF8 Utf8Path(*Path);
        das::CodeOfPolicies Policies;
        Policies.aot = bUseAot;
        auto Program = DSLCompile(Utf8Path.Get(), Access, Output, Modules, Policies);
        if (!Program || Program->failed())
        {
            Result.Error = Errors(Program, Output);
            return Result;
        }
        FUnrealDasContext Context(Program->getContextStackSize());
        Context.Self = Self;
        FActiveContextScope ActiveScope(Context);
        if (!Program->simulate(Context, Output))
        {
            Result.Error = Errors(Program, Output);
            return Result;
        }
        auto Function = Context.findFunction("main");
        if (!Function || !das::verifyCall<int32, int32>(Function->debugInfo, Modules))
        {
            Result.Error = TEXT("Expected exported main(input: int): int.");
            return Result;
        }
        vec4f Arguments[] = {das::cast<int32>::from(Input)};
        const vec4f Value = Context.evalWithCatch(Function, Arguments);
        if (const char* Exception = Context.getException())
        {
            Result.Error = UTF8_TO_TCHAR(Exception);
            return Result;
        }
        Result.bUsedAot = Function->aot;
        Result.Value = das::cast<int32>::to(Value);
        Result.Types = MoveTemp(Context.Types);
        Result.bSucceeded = true;
        return Result;
    }
    catch (const std::exception& Exception)
    {
        Result.Error = UTF8_TO_TCHAR(Exception.what());
        return Result;
    }
}

void FDasRuntimeModule::DispatchIntEvent(uint64 SubscriptionId, int32 Value)
{
    FDasScalarValue V;
    V.Kind = 0;
    V.IntValue = Value;
    DispatchScalarEvent(SubscriptionId, MoveTemp(V));
}
namespace
{
template <class ValueType> void DispatchScalarEventImpl(uint64 SubscriptionId, ValueType&& Value, bool Initialized)
{
    if (!IsInGameThread() || !Initialized)
        return;
#if WITH_EDITOR
    if (bPreservingSessions)
        return; // Old proxy IDs are detached and must never enter a VM during migration.
#endif
    FString Issue;
    if (!ValidateDasCallbackPayload(Value, EDasCallbackUsage::Event, Issue))
        return;
    if (PendingEvents.Num() < 1024)
        PendingEvents.Add({SubscriptionId, MakeShared<FDasCallbackPayload>(Forward<ValueType>(Value))});
    DrainEvents();
}
}
void FDasRuntimeModule::DispatchScalarEvent(uint64 SubscriptionId, const FDasScalarValue& Value)
{
    if (IsInGameThread())
        DispatchScalarEventImpl(SubscriptionId, Value, bInitialized);
}
void FDasRuntimeModule::DispatchScalarEvent(uint64 SubscriptionId, FDasScalarValue&& Value)
{
    if (IsInGameThread())
        DispatchScalarEventImpl(SubscriptionId, MoveTemp(Value), bInitialized);
}

void FDasRuntimeModule::CompletePrimaryAssetRequest(uint64 Id, bool Success)
{
    if (!IsInGameThread() || !bInitialized)
        return;
    for (const auto& S : Sessions)
    {
        auto* B = S->Bindings.FindByPredicate([&](const auto& X) { return X->Id == Id && X->AsyncKind == 2; });
        if (!B)
            continue;
        if (!S->Live || S->bReleaseRequested || S->bEndDelivered || !S->Owner.IsValid() || !S->World.IsValid() ||
            S->World->bIsTearingDown)
            return;
        FDasScalarValue V;
        V.Kind = 8;
        V.ObjectClass = TEXT("/Script/CoreUObject.Object");
        if (Success && S->Live->Context->PrimaryAssets.Contains((*B)->PrimaryAsset))
            if (auto* M = UAssetManager::GetIfInitialized())
                V.ObjectValue = M->GetPrimaryAssetObject((*B)->PrimaryAsset);
        DispatchScalarEvent(Id, MoveTemp(V));
        return;
    }
}

void FDasRuntimeModule::CompleteSaveGameRequest(uint64 Id, USaveGame* Loaded, bool Success)
{
    check(IsInGameThread());
    FString Key;
    if (!SaveGameIO.RemoveAndCopyValue(Id, Key))
        return;
    SaveGameSlotLeases.Remove(Key);
    if (!bInitialized)
        return;
    for (auto& S : Sessions)
    {
        auto* Found = S->Bindings.FindByPredicate([&](const auto& B) { return B->Id == Id && B->SaveGameKind; });
        if (!Found)
            continue;
        if (!S->Live || !S->Owner.IsValid() || !S->World.IsValid() || S->World->bIsTearingDown)
            return;
        FDasScalarValue Value;
        Value.Kind = int32((*Found)->Kind);
        if ((*Found)->SaveGameKind == 1)
            Value.BoolValue = Success;
        else
        {
            Value.ObjectClass = TEXT("/Script/CoreUObject.Object");
            if (IsValid(Loaded) && !Loaded->GetClass()->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists) &&
                S->Live->Context->SaveGames.Num() < 32)
            {
                S->Live->Context->SaveGames.Emplace(Loaded);
                Value.ObjectValue = Loaded;
            }
        }
        DispatchScalarEvent(Id, MoveTemp(Value));
        return;
    }
}

int32 FDasRuntimeModule::ScriptTimerCount(const UObject* Owner) const
{
    check(IsInGameThread());
    int32 Count = 0;
    for (const auto& Timer : ScriptTimers)
        if (!Owner || Timer.Value->Owner.Get() == Owner)
            ++Count;
    return Count;
}

int32 FDasRuntimeModule::SubscriptionCount(const UObject* Owner) const
{
    check(IsInGameThread());
    int32 Count = 0;
    for (const auto& Session : Sessions)
        if (!Owner || Session->Owner.Get() == Owner)
            Count += Session->Bindings.Num();
    return Count;
}

void FDasRuntimeModule::ReleaseObject(UObject* Object)
{
    check(IsInGameThread());
    UDasAsyncPhysicsBridge::RevokeOwner(Object);
    for (const auto& Session : Sessions)
        if (Session->Owner.Get(true) == Object)
            RevokeSession(*Session);
    FlushReleasedSessions();
}

bool FDasRuntimeModule::IsAuthoredMethodDispatchActive() const
{
    return IsInGameThread() && AuthoredDispatchDepth > 0;
}

bool FDasRuntimeModule::CanMigrateAuthoredTypes() const
{
#if WITH_EDITOR
    return IsInGameThread() && bInitialized && !ActiveContext && !bDrainingEvents && !bPreservingSessions &&
           PendingEvents.IsEmpty() && PendingScriptTimers.IsEmpty() && ScriptTimers.IsEmpty() &&
           !bDrainingScriptTimers && PersistentContextCount() == 0 && SubscriptionCount() == 0;
#else
    return false;
#endif
}
int32 FDasRuntimeModule::PersistentContextCount(const UWorld* World) const
{
    check(IsInGameThread());
    int32 Count = 0;
    for (const auto& Session : Sessions)
        if (Session->Live && Session->Owner.IsValid() && (!World || Session->World.Get() == World))
            ++Count;
    return Count;
}

bool FDasRuntimeModule::DispatchWidgetPaint(const FString& Path, const FString& Entry, UDasScriptUserWidget* Widget,
                                            FPaintContext& Native, FString& Error)
{
    if (!IsInGameThread() || !bInitialized || ActiveContext || ActiveWidgetPaint || ActiveWidgetReply ||
        !IsValid(Widget) || Widget->IsTemplate() || !Widget->GetWorld() || !Widget->GetWorld()->IsGameWorld() ||
        Widget->GetWorld()->bIsTearingDown || Native.MaxLayer < 0 || Native.MaxLayer > MAX_int32 - 1025)
    {
        Error = TEXT("OnPaint requires an idle game-thread runtime and a live game-world widget");
        return false;
    }
    if (!ValidateDasGeometryData(FGeometry::StaticStruct(), &Native.AllottedGeometry, Error))
        return false;
    FWidgetPaintScope Scope;
    Scope.Widget = Widget;
    Scope.Native = &Native;
    TGuardValue<FWidgetPaintScope*> PaintScope(ActiveWidgetPaint, &Scope);
    TGuardValue<bool> ContractScope(WidgetPaintContract, true);
    FDasScalarValue Arg;
    Arg.Kind = 12;
    Arg.ObjectClass = TEXT("/Script/UMG.PaintContext");
    const auto Result = RunNamedFramePersistent(Path, Entry, {Arg}, EDasMethodSignature::Void, Widget);
    if (!Result.bSucceeded || !Scope.Succeeded)
    {
        Error = Result.Error.IsEmpty() ? TEXT("OnPaint did not complete successfully"): Result.Error;
        return false;
    }
    CommitPaintCommands(Scope);
    Error.Empty();
    return true;
}
bool FDasRuntimeModule::DispatchWidgetDrag(const FString& Path, const FString& Entry, FName Event,
                                           const FGeometry* Geometry, const FPointerEvent& Pointer,
                                           UDragDropOperation* Operation, UDasScriptUserWidget* Widget,
                                           UDragDropOperation*& OutOperation, bool& Handled, FString& Error)
{
    OutOperation = nullptr;
    Handled = false;
    if (!IsInGameThread() || !bInitialized || ActiveContext || ActiveWidgetDrag || ActiveWidgetReply ||
        ActiveWidgetPaint || !IsValid(Widget) || Widget->IsTemplate() || !Widget->GetWorld() ||
        !Widget->GetWorld()->IsGameWorld() || Widget->GetWorld()->bIsTearingDown ||
        !Widget->GetCachedWidget().IsValid() || !UDasScriptUserWidget::IsSupportedDragEvent(Event))
    {
        Error = TEXT("Drag requires an idle game-thread runtime and a live game-world widget");
        return false;
    }
    const bool Detected = Event == TEXT("OnDragDetected"),
                                        NoGeometry = Event == TEXT("OnDragLeave") || Event == TEXT("OnDragCancelled");
    if ((NoGeometry && Geometry) || (!NoGeometry && !Geometry))
    {
        Error = TEXT("Drag geometry does not match event");
        return false;
    }
    auto* Managed = Cast<UDasDragDropOperation>(Operation);
    if ((Detected && Operation) || (!Detected && (!Managed || !Managed->IsLive() || !Managed->MatchesPointer(Pointer) ||
                                                  Managed->GetWorld() != Widget->GetWorld())))
    {
        Error = TEXT("Drag operation is expired, foreign or from another world");
        return false;
    }
    TStrongObjectPtr<UDasDragDropOperation> OperationPin(Managed);
    TArray<FDasScalarValue> Args;
    if (Geometry)
    {
        FInstancedStruct Copy;
        if (!CaptureDasGeometry(FGeometry::StaticStruct(), Geometry, Copy, Error))
            return false;
        FDasScalarValue A;
        A.Kind = 12;
        A.ObjectClass = FGeometry::StaticStruct()->GetPathName();
        A.StructValue = MoveTemp(Copy);
        Args.Add(MoveTemp(A));
    }
    FInstancedStruct Copy;
    if (!CaptureDasInputEvent(FPointerEvent::StaticStruct(), &Pointer, Copy, Error))
        return false;
    FDasScalarValue A;
    A.Kind = 12;
    A.ObjectClass = FPointerEvent::StaticStruct()->GetPathName();
    A.StructValue = MoveTemp(Copy);
    Args.Add(MoveTemp(A));
    if (!Detected)
    {
        FDasScalarValue O;
        O.Kind = 8;
        O.ObjectClass = TEXT("/Script/CoreUObject.Object");
        O.ObjectValue = Managed;
        Args.Add(O);
    }
    FWidgetDragScope Scope;
    Scope.Widget = Widget;
    Scope.Operation = Managed;
    Scope.Event = Event;
    Scope.UserIndex = Pointer.GetUserIndex();
    Scope.PointerIndex = Pointer.GetPointerIndex();
    TGuardValue<FWidgetDragScope*> DragScope(ActiveWidgetDrag, &Scope);
    TGuardValue<bool> ContractScope(WidgetDragContract, true);
    const auto Kind = Detected ? EDasMethodSignature::Object
                               : (Event == TEXT("OnDragOver") || Event == TEXT("OnDrop")) ? EDasMethodSignature::Bool
                                                                                          : EDasMethodSignature::Void;
    const auto Result = RunNamedFramePersistent(Path, Entry, Args, Kind, Widget,
                                                Detected ? TEXT("/Script/CoreUObject.Object"): FString());
    if (!Result.bSucceeded || !Scope.Succeeded)
    {
        Scope.Succeeded = false;
        Error = Result.Error.IsEmpty() ? TEXT("Drag callback did not complete successfully"): Result.Error;
        return false;
    }
    if (Detected)
        OutOperation = Scope.Created.Get();
    else if (Kind == EDasMethodSignature::Bool)
        Handled = Result.BoolValue;
    Error.Empty();
    return true;
}

bool FDasRuntimeModule::DispatchWidgetInput(const FString& Path, const FString& Entry, const FGeometry& Geometry,
                                            const UScriptStruct* Type, const void* Source, UDasScriptUserWidget* Widget,
                                            FReply& Out, FString& Error)
{
    if (!IsInGameThread() || !bInitialized || ActiveContext || ActiveWidgetReply || ActiveWidgetDrag ||
        ActiveWidgetPaint || !IsValid(Widget) || Widget->IsTemplate() || !Widget->GetWorld() ||
        !Widget->GetWorld()->IsGameWorld() || Widget->GetWorld()->bIsTearingDown || !Type ||
        !WidgetEventHandleName(Type->GetPathName()))
    {
        Error = TEXT("Synchronous widget input requires an idle game-thread runtime and supported live widget event");
        return false;
    }
    FInstancedStruct OwnedGeometry;
    if (!CaptureDasGeometry(FGeometry::StaticStruct(), &Geometry, OwnedGeometry, Error))
        return false;
    FDasScalarValue GeometryArg;
    GeometryArg.Kind = 12;
    GeometryArg.ObjectClass = FGeometry::StaticStruct()->GetPathName();
    GeometryArg.StructValue = MoveTemp(OwnedGeometry);
    FInstancedStruct Snapshot;
    if (!CaptureDasInputEvent(Type, Source, Snapshot, Error))
        return false;
    FDasScalarValue Arg;
    Arg.Kind = 12;
    Arg.ObjectClass = Type->GetPathName();
    Arg.StructValue = MoveTemp(Snapshot);
    FWidgetReplyScope Scope;
    Scope.Widget = Widget;
    Scope.NavigationInputType = Type;
    if (Type == FKeyEvent::StaticStruct())
        Scope.NavigationUserIndex = static_cast<const FKeyEvent*>(Source)->GetUserIndex();
    else if (Type == FAnalogInputEvent::StaticStruct())
        Scope.NavigationUserIndex = static_cast<const FAnalogInputEvent*>(Source)->GetUserIndex();
    TGuardValue<FWidgetReplyScope*> ReplyScope(ActiveWidgetReply, &Scope);
    TGuardValue<bool> ContractScope(WidgetReplyContract, true);
    if (FSlateApplication::IsInitialized())
        Scope.NavigationUser = FSlateApplication::Get().GetUser(int32(Scope.NavigationUserIndex));
    const auto Result = RunNamedFramePersistent(Path, Entry, {GeometryArg, Arg}, EDasMethodSignature::Struct, Widget,
                                                TEXT("/Script/UMG.EventReply"));
    if (!Result.bSucceeded || !Scope.Produced)
    {
        Error = Result.Error.IsEmpty() ? TEXT("Widget handler did not produce an EventReply"): Result.Error;
        if (Scope.NavigationConsumedAnalog)
        {
            Out = FReply::Handled();
            return true;
        }
        return false;
    }
    Out = Scope.Result;
    if (Scope.NavigationConsumedAnalog && !Out.IsEventHandled())
    {
        Error = TEXT("Analog navigation consumed this input; an unhandled reply was replaced with handled");
        Out = FReply::Handled();
        return true;
    }
    Error.Empty();
    return true;
}

bool FDasRuntimeModule::DispatchWidgetFocusLost(const FString& Path, const FString& Entry, const FFocusEvent& Event,
                                                UDasScriptUserWidget* Widget, FString& Error)
{
    if (!IsInGameThread() || !bInitialized || ActiveContext || ActiveWidgetReply || ActiveWidgetDrag ||
        ActiveWidgetPaint || !IsValid(Widget) || Widget->IsTemplate() || !Widget->GetWorld() ||
        !Widget->GetWorld()->IsGameWorld() || Widget->GetWorld()->bIsTearingDown)
    {
        Error = TEXT("Focus notification requires an idle game-thread runtime and live widget");
        return false;
    }
    FInstancedStruct Snapshot;
    if (!CaptureDasInputEvent(FFocusEvent::StaticStruct(), &Event, Snapshot, Error))
        return false;
    FDasScalarValue Arg;
    Arg.Kind = 12;
    Arg.ObjectClass = FFocusEvent::StaticStruct()->GetPathName();
    Arg.StructValue = MoveTemp(Snapshot);
    // Occupy the synchronous boundary, but grant no EventReply authority to void notifications.
    FWidgetReplyScope Scope;
    Scope.Widget = Widget;
    TGuardValue<FWidgetReplyScope*> NativeScope(ActiveWidgetReply, &Scope);
    TGuardValue<bool> ContractScope(FocusNotificationContract, true);
    const auto Result = RunNamedFramePersistent(Path, Entry, {Arg}, EDasMethodSignature::Void, Widget);
    if (!Result.bSucceeded)
    {
        Error = Result.Error;
        return false;
    }
    if (Scope.NavigationRevoked || !IsValid(Widget) || !Widget->GetCachedWidget().IsValid() || !Widget->GetWorld() ||
        Widget->GetWorld()->bIsTearingDown)
    {
        Error = TEXT("Widget expired during focus notification");
        return false;
    }
    Error.Empty();
    return true;
}

FDasExecutionResult FDasRuntimeModule::DispatchGeometrySnapshot(const FString& Path, const FString& Entry,
                                                                const FInstancedStruct& Value,
                                                                TWeakObjectPtr<UObject> Self)
{
    FDasExecutionResult R;
    FString E;
    if (!ValidateDasGeometrySnapshot(Value, E))
    {
        R.Error = E;
        return R;
    }
    FDasScalarValue A;
    A.Kind = 12;
    A.ObjectClass = Value.GetScriptStruct()->GetPathName();
    A.StructValue = Value;
    TGuardValue<bool> Scope(GeometrySnapshotDispatch, true);
    return RunNamedFramePersistent(Path, Entry, {A}, EDasMethodSignature::Int, Self);
}
FDasExecutionResult FDasRuntimeModule::DispatchInputEventSnapshots(const FString& Path, const FString& Entry,
                                                                   const TArray<FInstancedStruct>& Values,
                                                                   TWeakObjectPtr<UObject> Self)
{
    FDasExecutionResult Rejected;
    if (Values.IsEmpty() || Values.Num() > 16)
    {
        Rejected.Error = TEXT("input snapshot dispatch requires 1..16 detached values");
        return Rejected;
    }
    TArray<FDasScalarValue> Args;
    for (const auto& V : Values)
    {
        FString Error;
        if (!ValidateDasInputEventSnapshot(V, Error))
        {
            Rejected.Error = Error;
            return Rejected;
        }
        FDasScalarValue A;
        A.Kind = 12;
        A.ObjectClass = V.GetScriptStruct()->GetPathName();
        A.StructValue = V;
        Args.Add(MoveTemp(A));
    }
    TGuardValue<bool> Scope(InputSnapshotDispatch, true);
    return RunNamedFramePersistent(Path, Entry, Args, EDasMethodSignature::Int, Self);
}

FDasExecutionResult FDasRuntimeModule::DispatchGASAbilityHook(UDasAuthoredGameplayAbility* Ability, const FString& Entry)
{
    FDasExecutionResult Rejected;
    if (!IsInGameThread() || !IsValid(Ability) || Ability->IsTemplate() || Ability->ScriptFile.IsEmpty() ||
        Entry.IsEmpty() || (Entry != Ability->ActivateEntry && Entry != Ability->EndEntry && Entry != Ability->CancelEntry) ||
        ConstructingObject || AuthoredDispatchDepth >= 32)
    {
        Rejected.Error = TEXT("Invalid or recursive GAS ability hook dispatch");
        return Rejected;
    }
    const auto Context = Ability->ScriptAbilityContext();
    if (Context.ActivationId <= 0 || Context.GrantId <= 0 || !IsValid(Context.Owner) ||
        !IsValid(Context.System) || !Context.Owner->GetWorld() || Context.Owner->IsActorBeingDestroyed() ||
        Context.Owner->GetWorld()->bIsTearingDown)
    {
        Rejected.Error = TEXT("GAS ability context has expired");
        return Rejected;
    }
    FDasScalarValue Argument;
    Argument.Kind = 12;
    Argument.ObjectClass = FDasAuthoredAbilityContext::StaticStruct()->GetPathName();
    Argument.StructValue.InitializeAs<FDasAuthoredAbilityContext>(Context);
    TGuardValue<int32> Depth(AuthoredDispatchDepth, AuthoredDispatchDepth + 1);
    return RunNamedFramePersistent(ScriptPath(Ability->ScriptFile), Entry, {Argument}, EDasMethodSignature::Void, Ability);
}

FDasExecutionResult FDasRuntimeModule::RunNamedFramePersistent(const FString& Path, const FString& Entry,
                                                               const TArray<FDasScalarValue>& Args,
                                                               EDasMethodSignature ResultKind,
                                                               TWeakObjectPtr<UObject> Self, const FString& ReturnClass)
{
    return RunCallback(Path, EDasCallback::Main, 0, 0.f, Self, Entry, ResultKind, &Args, ReturnClass);
}
FDasExecutionResult FDasRuntimeModule::RunNamedFloatPersistent(const FString& Path, const FString& Entry, float Input,
                                                               TWeakObjectPtr<UObject> Self)
{
    return RunCallback(Path, EDasCallback::Main, 0, Input, Self, Entry, EDasMethodSignature::Float);
}
FDasExecutionResult FDasRuntimeModule::RunNamedBoolPersistent(const FString& Path, const FString& Entry, bool Input,
                                                              TWeakObjectPtr<UObject> Self)
{
    return RunCallback(Path, EDasCallback::Main, Input ? 1 : 0, 0.f, Self, Entry, EDasMethodSignature::Bool);
}
FDasExecutionResult FDasRuntimeModule::RunNamedVoidPersistent(const FString& Path, const FString& Entry,
                                                              TWeakObjectPtr<UObject> Self)
{
    return RunCallback(Path, EDasCallback::Main, 0, 0.f, Self, Entry, EDasMethodSignature::Void);
}

// Compile authoring descriptors without running script initializers or gameplay.
bool FDasRuntimeModule::ReadGASScript(const FString& Path, FDasGASDeclarations& Out, FString& Error)
{
    Error.Reset();
    Out = {};
    if (!IsInGameThread() || !bInitialized)
    {
        Error = TEXT("GAS authoring requires initialized runtime on game thread");
        return false;
    }
    try
    {
        das::TextWriter Output;
        das::ModuleGroup Group;
        auto Access = das::make_smart<FUnrealFileAccess>();
        Access->addFileSystem(new FUnrealFileSystem(), true, false);
        const FTCHARToUTF8 File(*Path);
        auto Program = DSLCompile(File.Get(), Access, Output, Group);
        if (!Program || Program->failed())
        {
            Error = Errors(Program, Output);
            return false;
        }
        TArray<TSharedPtr<FJsonValue>> Declarations;
        bool OK = true;
        Program->getThisModule()->structures.foreach (
            [&](const das::StructurePtr& S)
            {
                const auto* A = DSLAnnotation(S->annotations, "ue_attribute_set");
                if (!A)
                    A = DSLAnnotation(S->annotations, "ue_gameplay_effect");
                if (!A)
                    A = DSLAnnotation(S->annotations, "ue_gameplay_ability");
                if (!A || !OK)
                    return;
                const auto* Wire = A->arguments.find(DSLGASKey, das::Type::tString);
                TSharedPtr<FJsonObject> D;
                if (!Wire || !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(DSLText(Wire->sValue)), D))
                {
                    OK = false;
                    return;
                }
                Declarations.Add(MakeShared<FJsonValueObject>(D));
            });
        if (!OK)
        {
            Error = TEXT("Missing compiled GAS declaration");
            return false;
        }
        auto DeclarationsRoot = MakeShared<FJsonObject>();
        DeclarationsRoot->SetArrayField(TEXT("declarations"), Declarations);
        FString Json;
        FJsonSerializer::Serialize(DeclarationsRoot, TJsonWriterFactory<>::Create(&Json));
        return ParseDasGASDeclarations(Json, Out, Error);
    }
    catch (const std::exception& X)
    {
        Error = UTF8_TO_TCHAR(X.what());
        return false;
    }
}
