// Part of DasRuntime.cpp: Scalar, text and name access to properties of the running script owner (self).
// Included in place; declaration order matters.
FDasNameValue MakeDSLName(FName Name, das::Context* C)
{
    if (C != ActiveContext || !ActiveContext || !IsInGameThread())
        C->throw_error("name requires active context");
    if (Name.IsNone())
        return {};
    if (auto* T = ActiveContext->DSLNameTokens.Find(Name))
        return {*T};
    if (ActiveContext->DSLNames.Num() >= 65536)
        C->throw_error("name table limit exceeded");
    static uint64 NextToken = 1;
    const uint64 Token = NextToken++;
    ActiveContext->DSLNames.Add(Token, Name);
    ActiveContext->DSLNameTokens.Add(Name, Token);
    return {Token};
}
FName ResolveDSLName(FDasNameValue V, das::Context* C)
{
    if (C != ActiveContext || !ActiveContext)
        C->throw_error("name requires active context");
    if (!V.Token)
        return NAME_None;
    auto* N = ActiveContext->DSLNames.Find(V.Token);
    if (!N)
        C->throw_error("name belongs to another context");
    return *N;
}
FDasNameValue DSLNameFromString(const char* S, das::Context* C)
{
    const FString T = S ? UTF8_TO_TCHAR(S) : TEXT("");
    if (T.Len() >= NAME_SIZE)
        C->throw_error("FName value is too long");
    return MakeDSLName(FName(*T), C);
}
char* CopyDSLString(const FString& S, das::Context* C)
{
    for (int32 I = 0; I < S.Len(); ++I)
        if (S[I] == 0)
            C->throw_error("embedded NUL is not supported in strings");
    const FTCHARToUTF8 U(*S);
    return C->allocateString(U.Get(), U.Length(), nullptr);
}
char* DSLNameToString(FDasNameValue V, das::Context* C)
{
    return CopyDSLString(ResolveDSLName(V, C).ToString(), C);
}
bool DSLNamesEqual(FDasNameValue A, FDasNameValue B, das::Context* C)
{
    return ResolveDSLName(A, C) == ResolveDSLName(B, C);
}
FVector DSLVector(double X, double Y, double Z, das::Context* C)
{
    if (!FMath::IsFinite(X) || !FMath::IsFinite(Y) || !FMath::IsFinite(Z))
        C->throw_error("vector components must be finite");
    return FVector(X, Y, Z);
}
FRotator DSLRotator(double Pitch, double Yaw, double Roll, das::Context* C)
{
    if (!FMath::IsFinite(Pitch) || !FMath::IsFinite(Yaw) || !FMath::IsFinite(Roll))
        C->throw_error("rotator components must be finite");
    return FRotator(Pitch, Yaw, Roll);
}
FTransform DSLTransform(const FVector& Translation, const FRotator& Rotation, const FVector& Scale, das::Context* C)
{
    if (Translation.ContainsNaN() || Rotation.ContainsNaN() || Scale.ContainsNaN())
        C->throw_error("transform components must be finite");
    return FTransform(Rotation, Translation, Scale);
}
FTransform DSLIdentityTransform()
{
    return FTransform::Identity;
}
FVector DSLTransformPosition(const FTransform& T, const FVector& V, das::Context* C)
{
    if (T.ContainsNaN() || !T.IsRotationNormalized() || V.ContainsNaN())
        C->throw_error("invalid transform or position");
    const FVector Result = T.TransformPosition(V);
    if (Result.ContainsNaN())
        C->throw_error("non-finite transformed position");
    return Result;
}
FVector DSLTranslation(const FTransform& T)
{
    return T.GetTranslation();
}
FVector DSLScale(const FTransform& T)
{
    return T.GetScale3D();
}
FRotator DSLRotation(const FTransform& T)
{
    return T.Rotator();
}
UEnum* RequireCallEnum(FProperty*, const char*, das::Context*);
FNumericProperty* CallEnumStorage(FProperty*);
bool IsCallEnumMember(UEnum*, int32);
struct FStructRootWalker : das::DataWalker
{
    TSet<uint64> Tokens, DelegateTokens;
    bool Complete = true;
    virtual void beforeStructureField(char*, das::StructInfo*, char* Data, das::VarInfo* Info, bool) override
    {
        if (Info && Info->name && das::string(Info->name) == "__delegate_token" && Info->type == das::Type::tUInt64)
            DelegateTokens.Add(*reinterpret_cast<uint64*>(Data));
    }
    virtual bool canVisitHandle(char* Data, das::TypeInfo* Info) override
    {
        auto* A = Info->getAnnotation();
        if (A && (A->name == "UEWidgetAnimationHandle" || A->name == "UEGeometry" || A->name == "UEStructValue" ||
                  A->name == "UEHitResult" || A->name == "UEConstraintHandle" || A->name == "UEInputEvent" ||
                  A->name == "UEKeyEvent" || A->name == "UEAnalogInputEvent" || A->name == "UECharacterEvent" ||
                  A->name == "UEPointerEvent" || A->name == "UENavigationEvent" || A->name == "UEFocusEvent"))
            Tokens.Add(*reinterpret_cast<const uint64*>(Data));
        return false;
    }
};
bool CollectStructValues(FUnrealDasContext& C, const das::LineInfo* At, bool Boundary = false)
{
    if (C.StructValues.IsEmpty() && C.DelegateValues.IsEmpty())
        return true;
    // Active compiled/position-less frames cannot prove reachability.
    if (!Boundary && !C.gcEnabled)
        return false;
    FStructRootWalker W;
    W.context = &C;
    C.foreach_gc_root(
        [&](void* P, das::TypeInfo* T)
        {
            if (T)
                W.walk(static_cast<char*>(P), T);
            else
            {
                W.Complete = false;
            }
        });
    for (int I = 0; I < C.getTotalVariables(); ++I)
    {
        const auto V = C.getGlobalVariable(I);
        if (!V.debugInfo)
        {
            W.Complete = false;
            break;
        }
        char* Base = V.shared ? C.shared : C.globals;
        if (Base)
            W.walk(Base + V.offset, V.debugInfo);
    }
    if (!Boundary)
    {
        char* SP = C.stack.ap();
        const das::LineInfo* Handoff = At;
        while (SP < C.stack.top())
        {
            auto* P = reinterpret_cast<das::Prologue*>(SP);
            das::FuncInfo* Info = nullptr;
            char* Locals = SP;
            if (P->info)
            {
                intptr_t B = reinterpret_cast<intptr_t>(P->block);
                if (B & 1)
                {
                    auto* Block = reinterpret_cast<das::Block*>(B & ~intptr_t(1));
                    Info = Block->info;
                    Locals = C.stack.bottom() + Block->stackOffset;
                }
                else
                    Info = P->info;
            }
            if (Info)
            {
                const uint32 Pos = das::lineFramePos(Handoff, Info->spaceId);
                if (Info->localCount && !Pos)
                    return false;
                for (uint32 I = 0; I < Info->count; ++I)
                    W.walk(P->arguments[I], Info->fields[I]);
                for (uint32 I = 0; I < Info->localCount; ++I)
                {
                    auto* L = Info->locals[I];
                    if (das::isLiveAt(L, Pos))
                    {
                        char* Addr = L->cmres ? static_cast<char*>(P->cmres) : Locals + L->stackTop;
                        if (Addr)
                            W.walk(Addr, L);
                    }
                }
            }
            const uint32 Step = Info ? Info->stackSize : P->stackSize;
            if (!Step)
                return false;
            Handoff = Info ? P->line : nullptr;
            SP += Step;
        }
    }
    if (!W.Complete)
        return false;
    for (const auto& Call : C.Calls)
        for (const auto& Value : Call.Value->WidgetAnimationResults)
            if (Value.Value)
                W.Tokens.Add(Value.Value);
    const int32 Before = C.StructValues.Num();
    for (auto It = C.StructValues.CreateIterator(); It; ++It)
        if (!W.Tokens.Contains(It.Key()))
            It.RemoveCurrent();
    for (auto It = C.WidgetAnimations.CreateIterator(); It; ++It)
        if (!C.StructValues.Contains(It.Key()))
            It.RemoveCurrent();
    C.SnapshotKeyOrders.Prune([&](uint64 Token) { return C.StructValues.Contains(Token); });
    for (auto It = C.DelegateValues.CreateIterator(); It; ++It)
        if (!W.DelegateTokens.Contains(It.Key()))
            It.RemoveCurrent();
    ++C.StructCollections;
    C.StructReclaimed += Before - C.StructValues.Num();
    C.StructNextCollection = FMath::Min(4096, FMath::Max(256, C.StructValues.Num() + 256));
    return true;
}
void ExplicitStructCollect(das::Context* C, das::LineInfoArg* At)
{
    if (C != ActiveContext || !ActiveContext || !CollectStructValues(*ActiveContext, At))
        C->throw_error("struct collection requires options gc and attributable interpreter frames");
}
int32 StructValueCount(das::Context* C)
{
    if (C != ActiveContext || !ActiveContext)
        C->throw_error("struct stats require active context");
    return ActiveContext->StructValues.Num();
}
struct FStructCollectOnExit
{
    FUnrealDasContext& C;
    ~FStructCollectOnExit()
    {
        if (C.ExecutionDepth > 1)
            return;
        try
        {
            CollectStructValues(C, nullptr, true);
        }
        catch (...)
        { /* Failed root traversal must not turn a result into an exception. */
        }
    }
};
// The context reports every stored snapshot through FInstancedStruct's GC collector.
// This roots its reflected objects and lets UE clear destroyed references in native memory.
bool MathSnapshotValid(const FInstancedStruct& Snapshot)
{
    const auto* Type = Snapshot.GetScriptStruct();
    if (Type == TBaseStructure<FVector2D>::Get())
        return !Snapshot.Get<FVector2D>().ContainsNaN();
    if (Type == TBaseStructure<FVector>::Get())
        return !Snapshot.Get<FVector>().ContainsNaN();
    if (Type == TBaseStructure<FRotator>::Get())
        return !Snapshot.Get<FRotator>().ContainsNaN();
    if (Type == TBaseStructure<FTransform>::Get())
        return !Snapshot.Get<FTransform>().ContainsNaN() && Snapshot.Get<FTransform>().IsRotationNormalized();
    return true;
}
// Borrowed snapshots copy only on insertion. Owned temporaries transfer their
// buffer after validation/deduplication; all routes share one token counter.
uint64 NextStructValueToken = 1;
FDasStructValue StoreGeometry(const UScriptStruct*, const void*, das::Context*, das::LineInfoArg*);
FDasStructValue StoreInputEvent(const UScriptStruct*, const void*, das::Context*, das::LineInfoArg*);
FDasStructValue StoreStructValueImpl(const FInstancedStruct& V, FInstancedStruct* Owned, das::Context* C,
                                     das::LineInfoArg* At)
{
    if (C != ActiveContext || !ActiveContext)
        C->throw_error("struct requires active context");
    if (DasIsGeometryType(V.GetScriptStruct()))
        return StoreGeometry(V.GetScriptStruct(), V.GetMemory(), C, At);
    if (DasIsInputEventType(V.GetScriptStruct()))
        return StoreInputEvent(V.GetScriptStruct(), V.GetMemory(), C, At);
    FString Error;
    if (!ValidateDasStructSnapshot(V, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    if (!MathSnapshotValid(V))
        C->throw_error("invalid math snapshot value");
    for (const auto& P : ActiveContext->StructValues)
        if (IdenticalDasStructSnapshot(P.Value, V))
            return {P.Key};
    if (At && ActiveContext->StructValues.Num() >= ActiveContext->StructNextCollection)
        CollectStructValues(*ActiveContext, At);
    if (ActiveContext->StructValues.Num() >= 4096)
        C->throw_error("struct value table limit exceeded");
    uint64 Token = NextStructValueToken++;
    if (Owned)
        ActiveContext->StructValues.Add(Token, MoveTemp(*Owned));
    else
        ActiveContext->StructValues.Add(Token, V);
    return {Token};
}
FDasStructValue StoreStructValue(const FInstancedStruct& V, das::Context* C, das::LineInfoArg* At = nullptr)
{
    return StoreStructValueImpl(V, nullptr, C, At);
}
FDasStructValue StoreStructValue(FInstancedStruct&& V, das::Context* C, das::LineInfoArg* At = nullptr)
{
    return StoreStructValueImpl(V, &V, C, At);
}
const FInstancedStruct& ResolveStructValue(FDasStructValue V, das::Context* C)
{
    if (C != ActiveContext || !ActiveContext)
        C->throw_error("struct requires active context");
    if (const auto* Edit = ActiveContext->StructEdits.Find(V.Token))
    {
        if ((*Edit)->Poisoned)
            C->throw_error("snapshot edit was invalidated by a failed write");
        return (*Edit)->Value;
    }
    const auto* S = ActiveContext->StructValues.Find(V.Token);
    if (!S)
        C->throw_error("struct is null or belongs to another context");
    return *S;
}
#include "DasSnapshotEdits.inl"
FDasStructValue NewStructDefault(const char* Path, const char* Text, das::Context* C, das::LineInfoArg* At)
{
    FInstancedStruct Value;
    FString Error;
    if (!ParseDasStructDefault(Path ? UTF8_TO_TCHAR(Path)
                                    : TEXT(""), Text ? UTF8_TO_TCHAR(Text) : TEXT(""), Value, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    return StoreStructValue(MoveTemp(Value), C, At);
}
FDasStructValue ReadPropertySnapshot(FProperty* P, const void* Memory, das::Context* C, das::LineInfoArg* At)
{
    auto V = ReadNativePropertyValue(P, Memory, C);
    if (V.Kind != int32(EDasMethodSignature::Struct))
        C->throw_error("Expected struct/container property");
    return StoreStructValue(MoveTemp(V.StructValue), C, At);
}
void WritePropertySnapshot(FProperty* P, void* Memory, const FDasStructValue& Handle, das::Context* C)
{
    const auto& Snapshot = ResolveStructValue(Handle, C);
    if (!MathSnapshotValid(Snapshot))
        C->throw_error("Invalid math property value");
    FString Error;
    if (!WriteDasPropertySnapshot(P, Memory, Snapshot, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
}
FDasStructValue GetSelfStruct(const char* N, das::Context* C, das::LineInfoArg* At)
{
    auto* Self = ResolveSelf(C);
    auto* P = ResolveProperty<FProperty>(Self, N, false, C);
    return ReadPropertySnapshot(P, P->ContainerPtrToValuePtr<void>(Self), C, At);
}
void SetSelfStruct(const char* N, const FDasStructValue& V, das::Context* C)
{
    auto* Self = ResolveSelf(C);
    if (ActiveContext->bCollectingSubscriptions)
        C->throw_error("self writes are forbidden in bind_events");
    auto* P = ResolveProperty<FProperty>(Self, N, true, C);
    WritePropertySnapshot(P, P->ContainerPtrToValuePtr<void>(Self), V, C);
}
