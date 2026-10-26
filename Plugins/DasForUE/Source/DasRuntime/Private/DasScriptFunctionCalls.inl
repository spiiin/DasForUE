// Part of DasRuntime.cpp: typed calls of script functions from C++ (DasScript.h, TDasFunction).
// Included after DasRuntimeModuleExecution.inl; declaration order matters.
//
// The first call of a TDasFunction, and every call the fast path does not take, goes through RunCallback with the
// typed contract: it creates or reloads the owner's session, checks the function against the signature and runs it
// through the same Eval as the fast path (no call frame: the frame has no slots for int64, FQuat, FDateTime...).
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

// Math value names belong to their adapters (Scripts/TestTypeAdapterGate.py): the facet knows the handle.
FString DasScriptTypeName(EDasMethodSignature Kind)
{
    const IDasScriptTypeFacet* Facet = DasTypedCallFacet(Kind);
    const auto& Registry = FDasTypeAdapters::Get();
    const auto* Adapter = Registry.FindSignature(Kind);
    if (!Adapter)
        Adapter = Registry.FindKind(DasTypeKindOf(Kind));
    if (Adapter)
        for (const auto& Entry : Registry.Entries())
            if (&*Entry.Adapter == Adapter)
                Facet = Entry.Script;
    return Facet && Facet->GetHandleName() ? FString(UTF8_TO_TCHAR(Facet->GetHandleName())) : FString(TEXT("?"));
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
// Math values travel as the address of a copy (arguments) or of the result slot; the size of the copy, or 0.
int32 MathSize(EDasMethodSignature Kind)
{
    switch (Kind)
    {
    case EDasMethodSignature::Vector:
        return sizeof(FVector);
    case EDasMethodSignature::Rotator:
        return sizeof(FRotator);
    case EDasMethodSignature::Transform:
        return sizeof(FTransform);
    case EDasMethodSignature::Vector2D:
        return sizeof(FVector2D);
    case EDasMethodSignature::Quat:
        return sizeof(FQuat);
    case EDasMethodSignature::LinearColor:
        return sizeof(FLinearColor);
    case EDasMethodSignature::Color:
        return sizeof(FColor);
    default:
        return 0;
    }
}
constexpr int32 MathSlot = sizeof(FTransform); // the largest
// Finite values only, in both directions (a transform also needs a normalized rotation).
bool MathValid(EDasMethodSignature Kind, const void* Value)
{
    switch (Kind)
    {
    case EDasMethodSignature::Vector:
        return !static_cast<const FVector*>(Value)->ContainsNaN();
    case EDasMethodSignature::Rotator:
        return !static_cast<const FRotator*>(Value)->ContainsNaN();
    case EDasMethodSignature::Transform:
    {
        const auto& T = *static_cast<const FTransform*>(Value);
        return !T.ContainsNaN() && T.IsRotationNormalized();
    }
    case EDasMethodSignature::Vector2D:
        return !static_cast<const FVector2D*>(Value)->ContainsNaN();
    case EDasMethodSignature::Quat:
        return !static_cast<const FQuat*>(Value)->ContainsNaN();
    case EDasMethodSignature::LinearColor:
    {
        const auto& C = *static_cast<const FLinearColor*>(Value);
        return FMath::IsFinite(C.R) && FMath::IsFinite(C.G) && FMath::IsFinite(C.B) && FMath::IsFinite(C.A);
    }
    default:
        return true;
    }
}
// Arguments a call refuses, with the reason.
bool ArgumentsValid(TArrayView<const FDasScriptArgument> Arguments, FString& Error)
{
    if (Arguments.Num() > 16)
    {
        Error = TEXT("at most 16 arguments");
        return false;
    }
    for (const FDasScriptArgument& A : Arguments)
        if (A.Kind == EDasMethodSignature::String)
        {
            // FindChar would find the terminator: look at the characters only.
            const FString& Text = *static_cast<const FString*>(A.Value);
            for (int32 I = 0; I < Text.Len(); ++I)
                if (Text[I] == 0)
                {
                    Error = TEXT("embedded NUL is not supported in strings");
                    return false;
                }
        }
        else if (!MathValid(A.Kind, A.Value))
        {
            Error = TEXT("Invalid/non-finite math struct input");
            return false;
        }
        else if (A.Kind == EDasMethodSignature::Object)
        {
            if (UObject* Object = *static_cast<UObject* const*>(A.Value))
                if (!IsValid(Object) || !DasReferenceMatches(Object, A.Class ? A.Class : UObject::StaticClass()) ||
                    (Object->IsA<AActor>() && CastChecked<AActor>(Object)->IsActorBeingDestroyed()))
                {
                    Error = TEXT("Object argument is expired or has wrong class");
                    return false;
                }
        }
    return true;
}
// The contract the session records for the function (reloads keep it). Objects are declared as UObject, as a call
// frame would: the C++ type already guarantees the class, and TDasFunction casts the result.
FDasCallContract Contract(TArrayView<const FDasScriptArgument> Arguments, const FDasScriptArgument& Result)
{
    const FString ObjectClass = TEXT("/Script/CoreUObject.Object");
    FDasCallContract C;
    C.ReturnKind = Result.Kind;
    bool bObjects = false;
    for (const FDasScriptArgument& A : Arguments)
    {
        C.Inputs.Add(A.Kind);
        bObjects |= A.Kind == EDasMethodSignature::Object;
    }
    if (bObjects)
        for (const FDasScriptArgument& A : Arguments)
            C.InputClasses.Add(A.Kind == EDasMethodSignature::Object ? ObjectClass : FString());
    if (Result.Kind == EDasMethodSignature::Object)
        C.ReturnClass = ObjectClass;
    return C;
}
// Packs the arguments, runs Function and stores its result into Result.Value; both paths call it. False with
// Error for a value the call cannot take or give; false with the context's exception for a script error.
bool Eval(das::Context& Context, das::SimFunction* Function, TArrayView<const FDasScriptArgument> Arguments,
          const FDasScriptArgument& Result, FString& Error)
{
    vec4f Packed[16];
    FDasObjectHandle Handles[16];
    alignas(16) uint8 Math[16][MathSlot]; // copies: the script may change its parameter, never the caller's value
    TArray<TStrongObjectPtr<UObject>, TInlineAllocator<4>> Pins;
    for (int32 I = 0; I < Arguments.Num(); ++I)
    {
        const FDasScriptArgument& A = Arguments[I];
        if (const int32 Size = MathSize(A.Kind))
        {
            FMemory::Memcpy(Math[I], A.Value, Size);
            Packed[I] = das::cast<void*>::from(Math[I]);
            continue;
        }
        switch (A.Kind)
        {
        case EDasMethodSignature::Bool:
            Packed[I] = das::cast<bool>::from(*static_cast<const bool*>(A.Value));
            break;
        case EDasMethodSignature::Int:
            Packed[I] = das::cast<int32>::from(*static_cast<const int32*>(A.Value));
            break;
        case EDasMethodSignature::Int64:
            Packed[I] = das::cast<int64_t>::from(*static_cast<const int64*>(A.Value));
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
        case EDasMethodSignature::DateTime:
            Packed[I] = das::cast<FDasDateTimeValue>::from({static_cast<const FDateTime*>(A.Value)->GetTicks()});
            break;
        case EDasMethodSignature::Timespan:
            Packed[I] = das::cast<FDasTimespanValue>::from({static_cast<const FTimespan*>(A.Value)->GetTicks()});
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
    alignas(16) uint8 ReturnedMath[MathSlot] = {};
    const int32 ResultMathSize = MathSize(Result.Kind);
    void* Cmres = Result.Kind == EDasMethodSignature::Object ? static_cast<void*>(&ReturnedObject)
                  : ResultMathSize                           ? static_cast<void*>(ReturnedMath)
                                                             : nullptr;
    const vec4f Value = Context.evalWithCatch(Function, Arguments.Num() ? Packed : nullptr, Cmres);
    if (Context.getException())
        return false;
    if (ResultMathSize)
    {
        if (!MathValid(Result.Kind, ReturnedMath))
        {
            Error = TEXT("Invalid/non-finite math struct result");
            return false;
        }
        FMemory::Memcpy(Result.Value, ReturnedMath, ResultMathSize);
        return true;
    }
    switch (Result.Kind)
    {
    case EDasMethodSignature::Bool:
        *static_cast<bool*>(Result.Value) = das::cast<bool>::to(Value);
        break;
    case EDasMethodSignature::Int:
        *static_cast<int32*>(Result.Value) = das::cast<int32>::to(Value);
        break;
    case EDasMethodSignature::Int64:
        *static_cast<int64*>(Result.Value) = das::cast<int64_t>::to(Value);
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
    case EDasMethodSignature::DateTime:
        *static_cast<FDateTime*>(Result.Value) = FDateTime(das::cast<FDasDateTimeValue>::to(Value).Ticks);
        break;
    case EDasMethodSignature::Timespan:
        *static_cast<FTimespan*>(Result.Value) = FTimespan(das::cast<FDasTimespanValue>::to(Value).Ticks);
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
}

// The fast path: false when it does not apply (the caller takes the full path), otherwise the call's result.
bool FDasRuntimeModule::CallScriptSite(FDasScriptCallSite& Site, UObject* Owner, const FString& Entry,
                                       TArrayView<const FDasScriptArgument> Arguments, const FDasScriptArgument& Result,
                                       FString& Error, bool& bTaken)
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
    FString ArgumentError; // the full path reports it
    if (!DasScriptCalls::ArgumentsValid(Arguments, ArgumentError))
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

    try
    {
        if (DasScriptCalls::Eval(Context, Site.Function, Arguments, Result, Error))
            return true;
        if (Context.getException())
            Error = DSLDiagnosticRuntime(Context, Session->Live->Program, Entry);
        return false;
    }
    catch (const std::exception& Exception)
    {
        Error = UTF8_TO_TCHAR(Exception.what());
        return false;
    }
}

bool FDasRuntimeModule::CallScriptFunction(TSharedPtr<FDasScriptCallSite>& Site, UObject* Owner, const FString& Script,
                                           const FString& Entry, TArrayView<const FDasScriptArgument> Arguments,
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
    // The full path: RunCallback with the typed contract; the same Eval packs, runs and reads the result.
    if (!DasScriptCalls::ArgumentsValid(Arguments, Error))
        return false;
    auto Invoke = [&](das::Context& Context, das::SimFunction* Function, FString& InvokeError)
    {
        return DasScriptCalls::Eval(Context, Function, Arguments, Result, InvokeError);
    };
    const FDasTypedCall Typed{DasScriptCalls::Contract(Arguments, Result), Invoke};
    const FString Path = ScriptPath(Script);
    const FDasExecutionResult Out = RunCallback(Path, EDasCallback::Main, 0, 0.f, Owner, Entry,
                                                EDasMethodSignature::Void, nullptr, FString(), &Typed);
    if (!Out.bSucceeded)
    {
        Error = Out.Error;
        return false;
    }
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
    return FModuleManager::LoadModuleChecked<FDasRuntimeModule>(
        TEXT("DasRuntime")).CallScriptFunction(Site, Owner, Script, Entry, Arguments, Result, Error);
}
