// Part of DasRuntime.cpp: typed calls of script functions from C++ (DasScript.h, TDasFunction).
// Included after DasRuntimeModuleExecution.inl; declaration order matters.
//
// The first call of a TDasFunction, and every call the fast path does not take, goes through RunCallback with a
// call frame: it creates or reloads the owner's session, checks the function against the signature and runs it.
// The call site then remembers that session and function. The next call with the same owner runs the function
// directly, as long as nothing RunCallback would react to has changed: the session and its program (revision),
// the set of sessions, the sources (hot reload), the owner's state; and only from the top level (no script on the
// stack, no widget, delegate, construction or library dispatch around it), with valid arguments. Anything else
// takes the full path, which also produces every error.

TAutoConsoleVariable<int32> CVarScriptFunctionFastPath(
    TEXT("das.ScriptFunctionFastPath"), 1,
    TEXT("1: a TDasFunction calls its function directly while its owner's session and script are unchanged; ")
        TEXT("0: every call takes the full persistent call path (for comparisons)."));

uint64 GDasScriptFunctionFastCalls = 0;
uint64 DasScriptFunctionFastCalls()
{
    return GDasScriptFunctionFastCalls;
}

struct FDasScriptCallSite
{
    TWeakObjectPtr<UObject> Owner;
    FPersistentSession* Session = nullptr;
    FCompiledContext* Live = nullptr;
    das::SimFunction* Function = nullptr;
    uint32 Revision = 0;
    uint64 SessionEpoch = 0;
};

namespace DasScriptCalls
{
// A typed argument as a call frame value (the full path).
FDasScalarValue FrameValue(const FDasScriptArgument& A)
{
    FDasScalarValue V;
    V.Kind = int32(A.Kind);
    switch (A.Kind)
    {
    case EDasMethodSignature::Bool:
        V.BoolValue = *static_cast<const bool*>(A.Value);
        break;
    case EDasMethodSignature::Int:
        V.IntValue = *static_cast<const int32*>(A.Value);
        break;
    case EDasMethodSignature::Float:
        V.FloatValue = *static_cast<const float*>(A.Value);
        break;
    case EDasMethodSignature::Double:
        V.DoubleValue = *static_cast<const double*>(A.Value);
        break;
    case EDasMethodSignature::Byte:
        V.ByteValue = *static_cast<const uint8*>(A.Value);
        break;
    case EDasMethodSignature::String:
        V.StringValue = *static_cast<const FString*>(A.Value);
        break;
    case EDasMethodSignature::Name:
        V.NameValue = *static_cast<const FName*>(A.Value);
        break;
    case EDasMethodSignature::Text:
        V.TextValue = *static_cast<const FText*>(A.Value);
        break;
    case EDasMethodSignature::Vector:
        V.VectorValue = *static_cast<const FVector*>(A.Value);
        break;
    case EDasMethodSignature::Rotator:
        V.RotatorValue = *static_cast<const FRotator*>(A.Value);
        break;
    case EDasMethodSignature::Transform:
        V.TransformValue = *static_cast<const FTransform*>(A.Value);
        break;
    case EDasMethodSignature::Object:
        // An exported function takes UObjectHandle: the frame declares the object as UObject (the C++ type
        // already guarantees the class).
        V.ObjectValue = *static_cast<UObject* const*>(A.Value);
        V.ObjectClass = TEXT("/Script/CoreUObject.Object");
        break;
    default:
        break;
    }
    return V;
}
// The result of the full path into the C++ value.
void StoreResult(const FDasExecutionResult& R, const FDasScriptArgument& Out)
{
    switch (Out.Kind)
    {
    case EDasMethodSignature::Bool:
        *static_cast<bool*>(Out.Value) = R.BoolValue;
        break;
    case EDasMethodSignature::Int:
        *static_cast<int32*>(Out.Value) = R.Value;
        break;
    case EDasMethodSignature::Float:
        *static_cast<float*>(Out.Value) = R.FloatValue;
        break;
    case EDasMethodSignature::Double:
        *static_cast<double*>(Out.Value) = R.DoubleValue;
        break;
    case EDasMethodSignature::Byte:
        *static_cast<uint8*>(Out.Value) = R.ByteValue;
        break;
    case EDasMethodSignature::String:
        *static_cast<FString*>(Out.Value) = R.StringValue;
        break;
    case EDasMethodSignature::Name:
        *static_cast<FName*>(Out.Value) = R.NameValue;
        break;
    case EDasMethodSignature::Text:
        *static_cast<FText*>(Out.Value) = R.TextValue;
        break;
    case EDasMethodSignature::Vector:
        *static_cast<FVector*>(Out.Value) = R.VectorValue;
        break;
    case EDasMethodSignature::Rotator:
        *static_cast<FRotator*>(Out.Value) = R.RotatorValue;
        break;
    case EDasMethodSignature::Transform:
        *static_cast<FTransform*>(Out.Value) = R.TransformValue;
        break;
    case EDasMethodSignature::Object:
        *static_cast<UObject**>(Out.Value) = R.ObjectValue.Get();
        break;
    default:
        break;
    }
}
// Arguments the full path would refuse: the fast path leaves them to it, so that it reports them.
bool ArgumentsValid(TArrayView<const FDasScriptArgument> Arguments)
{
    for (const FDasScriptArgument& A : Arguments)
        switch (A.Kind)
        {
        case EDasMethodSignature::String:
        {
            // FindChar would find the terminator: look at the characters only, as RunCallback does.
            const FString& Text = *static_cast<const FString*>(A.Value);
            for (int32 I = 0; I < Text.Len(); ++I)
                if (Text[I] == 0)
                    return false;
            break;
        }
        case EDasMethodSignature::Vector:
            if (static_cast<const FVector*>(A.Value)->ContainsNaN())
                return false;
            break;
        case EDasMethodSignature::Rotator:
            if (static_cast<const FRotator*>(A.Value)->ContainsNaN())
                return false;
            break;
        case EDasMethodSignature::Transform:
        {
            const auto& T = *static_cast<const FTransform*>(A.Value);
            if (T.ContainsNaN() || !T.IsRotationNormalized())
                return false;
            break;
        }
        case EDasMethodSignature::Object:
            if (UObject* Object = *static_cast<UObject* const*>(A.Value))
                if (!IsValid(Object) || !DasReferenceMatches(Object, A.Class ? A.Class : UObject::StaticClass()) ||
                    (Object->IsA<AActor>() && CastChecked<AActor>(Object)->IsActorBeingDestroyed()))
                    return false;
            break;
        default:
            break;
        }
    return true;
}
}

// The fast path: false when it does not apply (the caller takes the full path), otherwise the call's result.
bool FDasRuntimeModule::CallScriptSite(FDasScriptCallSite& Site, UObject* Owner, const FString& Entry,
                                       TArrayView<const FDasScriptArgument> Arguments,
                                       const FDasScriptArgument& Result, FString& Error, bool& bTaken)
{
    bTaken = false;
    FPersistentSession* Session = Site.Session;
    if (CVarScriptFunctionFastPath.GetValueOnGameThread() == 0 || !Session || Site.Owner.Get() != Owner ||
        Site.SessionEpoch != SessionSetEpoch || !IsInGameThread() || !bInitialized)
        return false;
    if (Session->Owner.Get() != Owner || !Session->Live || Session->Live.Get() != Site.Live ||
        Session->Revision != Site.Revision || Session->bEndDelivered || Session->bReleaseRequested ||
        SourceCheckFor(*Session) != ESourceCheck::None)
        return false;
    // Only a top-level call: the full path handles nested, widget, delegate, construction and library dispatch.
    if (ActiveContext || ConstructingObject || ExpectedDelegateGeneration || ActiveWidgetDrag || ActiveWidgetReply ||
        ActiveWidgetPaint || WidgetContractFlags() != 0 || !LibraryInvocationScript.IsEmpty() ||
        GetDasExecutionRole() == EDasExecutionRole::PhysicsPreSim || IsDasFrozenPhysicsWorld(Owner))
        return false;
#if WITH_EDITOR
    if (bPreservingSessions)
        return false;
#endif
    if (!IsValid(Owner) || (Owner->IsA<AActor>() && CastChecked<AActor>(Owner)->IsActorBeingDestroyed()) ||
        (Cast<UDasWorldSubsystem>(Owner) && !CastChecked<UDasWorldSubsystem>(Owner)->IsDasActive()) ||
        (Cast<UDasGameInstanceSubsystem>(Owner) && !CastChecked<UDasGameInstanceSubsystem>(Owner)->IsDasActive()))
        return false;
    if (const auto* Deliveries = InitializerDeliveries.Find(Owner))
        for (const auto& Delivery : *Deliveries)
            if (Delivery.Value.State == EInitializerDelivery::Failed)
                return false;
    if (Arguments.Num() > 16 || !DasScriptCalls::ArgumentsValid(Arguments))
        return false;
    bTaken = true;
    ++GDasScriptFunctionFastCalls;

    // As in RunCallback: events queued by the call are delivered after it, the VM starts fresh.
    FDrainEventsOnExit DrainOnExit;
    ++GDasSelfSiteEpoch;
    auto& Context = *Session->Live->Context;
    Context.restart();
    Context.Types.Reset();
    TGuardValue<int32> ExecutionScope(Context.ExecutionDepth, Context.ExecutionDepth + 1);
    FActiveContextScope Scope(Context);
    FStructCollectOnExit CollectStructs{Context};
    TGuardValue<bool> EndingScope(Context.bEnding, false);
    TGuardValue<FOutputFrame*> OutScope(ActiveOutputFrame, nullptr);

    vec4f Packed[16];
    FDasObjectHandle Handles[16];
    FVector Vectors[16];   // no constructors run for these two
    FRotator Rotators[16];
    TArray<FTransform, TInlineAllocator<2>> Transforms;
    Transforms.Reserve(Arguments.Num()); // pointers into it stay valid
    TArray<TStrongObjectPtr<UObject>, TInlineAllocator<4>> Pins;
    for (int32 I = 0; I < Arguments.Num(); ++I)
    {
        const FDasScriptArgument& A = Arguments[I];
        switch (A.Kind)
        {
        case EDasMethodSignature::Bool:
            Packed[I] = das::cast<bool>::from(*static_cast<const bool*>(A.Value));
            break;
        case EDasMethodSignature::Int:
            Packed[I] = das::cast<int32>::from(*static_cast<const int32*>(A.Value));
            break;
        case EDasMethodSignature::Float:
            Packed[I] = das::cast<float>::from(*static_cast<const float*>(A.Value));
            break;
        case EDasMethodSignature::Double:
            Packed[I] = das::cast<double>::from(*static_cast<const double*>(A.Value));
            break;
        case EDasMethodSignature::Byte:
            Packed[I] = das::cast<uint8_t>::from(*static_cast<const uint8*>(A.Value));
            break;
        case EDasMethodSignature::String:
            Packed[I] = das::cast<char*>::from(CopyDSLString(*static_cast<const FString*>(A.Value), &Context));
            break;
        case EDasMethodSignature::Name:
            Packed[I] = das::cast<FDasNameValue>::from(MakeDSLName(*static_cast<const FName*>(A.Value), &Context));
            break;
        case EDasMethodSignature::Text:
            Packed[I] = das::cast<FDasTextValue>::from(MakeDSLText(*static_cast<const FText*>(A.Value), &Context));
            break;
        // Math values are copied: the script may change its parameter, never the caller's value.
        case EDasMethodSignature::Vector:
            Vectors[I] = *static_cast<const FVector*>(A.Value);
            Packed[I] = das::cast<void*>::from(&Vectors[I]);
            break;
        case EDasMethodSignature::Rotator:
            Rotators[I] = *static_cast<const FRotator*>(A.Value);
            Packed[I] = das::cast<void*>::from(&Rotators[I]);
            break;
        case EDasMethodSignature::Transform:
            Packed[I] = das::cast<void*>::from(&Transforms.Add_GetRef(*static_cast<const FTransform*>(A.Value)));
            break;
        case EDasMethodSignature::Object:
        {
            UObject* Object = *static_cast<UObject* const*>(A.Value);
            Handles[I] = MakeObjectHandle(Object, UObject::StaticClass(), &Context);
            if (Object)
                Pins.Emplace(Object);
            Packed[I] = das::cast<void*>::from(&Handles[I]);
            break;
        }
        default:
            Error = TEXT("Invalid scalar argument kind");
            return false;
        }
    }
    FDasObjectHandle ReturnedObject;
    FVector ReturnedVector = FVector::ZeroVector;
    FRotator ReturnedRotator = FRotator::ZeroRotator;
    FTransform ReturnedTransform = FTransform::Identity;
    void* Cmres = Result.Kind == EDasMethodSignature::Object      ? static_cast<void*>(&ReturnedObject)
                  : Result.Kind == EDasMethodSignature::Vector    ? static_cast<void*>(&ReturnedVector)
                  : Result.Kind == EDasMethodSignature::Rotator   ? static_cast<void*>(&ReturnedRotator)
                  : Result.Kind == EDasMethodSignature::Transform ? static_cast<void*>(&ReturnedTransform)
                                                                  : nullptr;
    try
    {
        const vec4f Value = Context.evalWithCatch(Site.Function, Arguments.Num() ? Packed : nullptr, Cmres);
        if (Context.getException())
        {
            Error = DSLDiagnosticRuntime(Context, Session->Live->Program, Entry);
            return false;
        }
        switch (Result.Kind)
        {
        case EDasMethodSignature::Bool:
            *static_cast<bool*>(Result.Value) = das::cast<bool>::to(Value);
            break;
        case EDasMethodSignature::Int:
            *static_cast<int32*>(Result.Value) = das::cast<int32>::to(Value);
            break;
        case EDasMethodSignature::Float:
            *static_cast<float*>(Result.Value) = das::cast<float>::to(Value);
            break;
        case EDasMethodSignature::Double:
            *static_cast<double*>(Result.Value) = das::cast<double>::to(Value);
            break;
        case EDasMethodSignature::Byte:
            *static_cast<uint8*>(Result.Value) = das::cast<uint8_t>::to(Value);
            break;
        case EDasMethodSignature::String:
        {
            const char* Text = das::cast<char*>::to(Value);
            *static_cast<FString*>(Result.Value) = Text ? UTF8_TO_TCHAR(Text) : TEXT("");
            break;
        }
        case EDasMethodSignature::Name:
            *static_cast<FName*>(Result.Value) = ResolveDSLName(das::cast<FDasNameValue>::to(Value), &Context);
            break;
        case EDasMethodSignature::Text:
            *static_cast<FText*>(Result.Value) = ResolveDSLText(das::cast<FDasTextValue>::to(Value), &Context);
            break;
        case EDasMethodSignature::Vector:
        case EDasMethodSignature::Rotator:
        case EDasMethodSignature::Transform:
            if ((Result.Kind == EDasMethodSignature::Vector && ReturnedVector.ContainsNaN()) ||
                (Result.Kind == EDasMethodSignature::Rotator && ReturnedRotator.ContainsNaN()) ||
                (Result.Kind == EDasMethodSignature::Transform &&
                 (ReturnedTransform.ContainsNaN() || !ReturnedTransform.IsRotationNormalized())))
            {
                Error = TEXT("Invalid/non-finite math struct result");
                return false;
            }
            if (Result.Kind == EDasMethodSignature::Vector)
                *static_cast<FVector*>(Result.Value) = ReturnedVector;
            else if (Result.Kind == EDasMethodSignature::Rotator)
                *static_cast<FRotator*>(Result.Value) = ReturnedRotator;
            else
                *static_cast<FTransform*>(Result.Value) = ReturnedTransform;
            break;
        case EDasMethodSignature::Object:
        {
            UObject* Object = ReturnedObject.Token ? TryObject(ReturnedObject, &Context) : nullptr;
            if (ReturnedObject.Token && !Object)
            {
                Error = TEXT("Object result is expired or belongs to another context");
                return false;
            }
            *static_cast<UObject**>(Result.Value) = Object; // TDasFunction casts it to its result class
            break;
        }
        default:
            break;
        }
        return true;
    }
    catch (const std::exception& Exception)
    {
        Error = UTF8_TO_TCHAR(Exception.what());
        return false;
    }
}

bool FDasRuntimeModule::CallScriptFunction(TSharedPtr<FDasScriptCallSite>& Site, UObject* Owner,
                                           const FString& Script, const FString& Entry,
                                           TArrayView<const FDasScriptArgument> Arguments,
                                           const FDasScriptArgument& Result, FString& Error)
{
    if (!IsValid(Owner))
    {
        Error = TEXT("the owner object is null or being destroyed");
        return false;
    }
    if (Site)
    {
        bool bTaken = false;
        const bool bSucceeded = CallScriptSite(*Site, Owner, Entry, Arguments, Result, Error, bTaken);
        if (bTaken)
            return bSucceeded;
    }
    // The full path: RunCallback with a call frame. An object result is read as UObject (TDasFunction casts it).
    TArray<FDasScalarValue> Frame;
    Frame.Reserve(Arguments.Num());
    for (const FDasScriptArgument& A : Arguments)
        Frame.Add(DasScriptCalls::FrameValue(A));
    const FString Path = ScriptPath(Script);
    const FDasExecutionResult Out =
        RunCallback(Path, EDasCallback::Main, 0, 0.f, Owner, Entry, Result.Kind, &Frame,
                    Result.Kind == EDasMethodSignature::Object ? TEXT("/Script/CoreUObject.Object") : FString());
    if (!Out.bSucceeded)
    {
        Error = Out.Error;
        return false;
    }
    DasScriptCalls::StoreResult(Out, Result);
    // Remember the session and the function for the next call.
    if (!Site)
        Site = MakeShared<FDasScriptCallSite>();
    *Site = FDasScriptCallSite();
    const FString FullPath = NormalizeScriptPath(Path);
    for (const auto& Existing : Sessions)
        if (Existing->Owner.Get() == Owner && Existing->Path == FullPath && Existing->Live)
            if (const auto* Verified = Existing->Live->VerifiedEntries.Find(Entry); Verified && Verified->Flags == 0)
            {
                Site->Owner = Owner;
                Site->Session = Existing.Get();
                Site->Live = Existing->Live.Get();
                Site->Function = Verified->Function;
                Site->Revision = Existing->Revision;
                Site->SessionEpoch = SessionSetEpoch;
            }
    return true;
}

bool DasCallScriptFunction(TSharedPtr<FDasScriptCallSite>& Site, UObject* Owner, const FString& Script,
                           const FString& Entry, TArrayView<const FDasScriptArgument> Arguments,
                           const FDasScriptArgument& Result, FString& Error)
{
    return FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"))
        .CallScriptFunction(Site, Owner, Script, Entry, Arguments, Result, Error);
}
