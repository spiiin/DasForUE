// Generated allowlist is independent of editor-only metadata and is checked in
// both compiler and runtime. No FProperty/UFunction pointer crosses a reload.
const TCHAR* NativeGameplaySnapshotKind(const char* Path, const char* Name, das::Context* C)
{
    const auto* Row =
        DasNativeGameplayField(Path ? UTF8_TO_TCHAR(Path) : TEXT(""), Name ? UTF8_TO_TCHAR(Name) : TEXT(""));
    if (!Row ||
        (FString(Row->Kind) !=
         TEXT("struct") && FString(Row->Kind) != TEXT("array") && FString(Row->Kind) !=
                                                                      TEXT("map") && FString(Row->Kind) != TEXT("set")))
        C->throw_error("native snapshot field not generated");
    return Row->Kind;
}
// Resolved generated receiver classes by path, and validated generated fields by (class path, field,
// direction, kind, receiver class). Entries hold the strings they were made for (heap strings may reuse
// addresses), weak classes and field paths, and the self site epoch (contexts, GC); a miss or a stale
// entry takes the checked path, which reports every error.
struct FDasNativeClassSite
{
    das::string Path;
    uint64 Epoch = 0;
    TWeakObjectPtr<UClass> Class;
};
UClass* DasNativeGameplayClass(const char* ClassPath)
{
    static FDasNativeClassSite Sites[64];
    const UPTRINT Key = UPTRINT(ClassPath);
    FDasNativeClassSite& S = Sites[((Key >> 4) ^ (Key >> 10)) & 63];
    if (ClassPath && S.Epoch == GDasSelfSiteEpoch && FCStringAnsi::Strcmp(S.Path.c_str(), ClassPath) == 0)
        if (UClass* Cached = S.Class.Get())
            return Cached;
    const FString Path = ClassPath ? UTF8_TO_TCHAR(ClassPath) : TEXT("");
    UClass* Class = FindObject<UClass>(nullptr, *Path);
    if (Class && ClassPath)
    {
        S.Path = ClassPath;
        S.Epoch = GDasSelfSiteEpoch;
        S.Class = Class;
    }
    return Class;
}
UObject* NativeGameplayObject(const FDasObjectHandle& H, const char* ClassPath, bool Write, das::Context* C)
{
    auto* O = ResolveObject(H, C);
    auto* Self = ResolveWorldContextObject(C);
    auto* W = Self->GetWorld();
    auto* Actor = Cast<AActor>(O);
    auto* Component = Cast<UActorComponent>(O);
    if (O->IsTemplate() || !W || (!W->IsGameWorld() && !ConstructingObject) || W->bIsTearingDown ||
        O->GetWorld() != W || (Actor && Actor->IsActorBeingDestroyed()) || (Component && Component->IsBeingDestroyed()))
        C->throw_error("native access requires live receiver in owner's world");
    if (Write && (ActiveContext->bEnding || ActiveContext->bCollectingSubscriptions))
        C->throw_error("native mutation is forbidden in bind_events/EndPlay");
    auto* Class = DasNativeGameplayClass(ClassPath);
    if (!Class || !O->IsA(Class))
        C->throw_error("native gameplay receiver class mismatch");
    return O;
}
struct FDasNativeFieldSite
{
    das::string Path, Name;
    const TCHAR* Kind = nullptr;
    bool bWrite = false;
    UClass* Class = nullptr;
    uint64 Epoch = 0;
    FProperty* Raw = nullptr;
    TFieldPath<FProperty> Property;
};
FDasNativeFieldSite& DasNativeFieldSite(const char* ClassPath, const char* Name, bool Write)
{
    static FDasNativeFieldSite Sites[512];
    // Reads and writes of one field use neighbouring slots so that they do not evict each other.
    const UPTRINT Key = UPTRINT(Name) ^ (UPTRINT(ClassPath) << 1);
    return Sites[((((Key >> 4) ^ (Key >> 13)) << 1) | UPTRINT(Write)) & 511];
}
FProperty* NativeGameplayPropertyChecked(UObject* O, const char* ClassPath, const char* Name, bool Write,
                                         const TCHAR* Kind, das::Context* C);
FProperty* NativeGameplayProperty(UObject* O, const char* ClassPath, const char* Name, bool Write, const TCHAR* Kind,
                                  das::Context* C)
{
    FDasNativeFieldSite& S = DasNativeFieldSite(ClassPath, Name, Write);
    if (ClassPath && Name && Kind && S.Epoch == GDasSelfSiteEpoch && S.bWrite == Write && S.Class == O->GetClass() &&
        S.Kind && FCString::Strcmp(S.Kind, Kind) == 0 && FCStringAnsi::Strcmp(S.Name.c_str(), Name) == 0 &&
        FCStringAnsi::Strcmp(S.Path.c_str(), ClassPath) == 0)
        if (FProperty* P = S.Property.Get(); P && P == S.Raw)
            return P;
    FProperty* P = NativeGameplayPropertyChecked(O, ClassPath, Name, Write, Kind, C);
    if (ClassPath && Name && Kind)
    {
        S.Path = ClassPath;
        S.Name = Name;
        S.Kind = Kind;
        S.bWrite = Write;
        S.Class = O->GetClass();
        S.Epoch = GDasSelfSiteEpoch;
        S.Raw = P;
        S.Property = TFieldPath<FProperty>(P);
    }
    return P;
}
FProperty* NativeGameplayPropertyChecked(UObject* O, const char* ClassPath, const char* Name, bool Write,
                                         const TCHAR* Kind, das::Context* C)
{
    const FString Class =
                      ClassPath ? UTF8_TO_TCHAR(ClassPath) : TEXT(""), Field = Name ? UTF8_TO_TCHAR(Name) : TEXT("");
    if (Field.IsEmpty() || Field.Len() >= NAME_SIZE)
        C->throw_error("invalid native field name");
    const auto* Entry = DasNativeGameplayField(Class, Field);
    if (!Entry || FString(Entry->Kind) != Kind || (Write && Entry->ReadOnly))
        C->throw_error("native field is not generated or is readonly");
    auto* P = ResolveProperty<FProperty>(O, Name, Write, C);
    FString Shape;
    AppendCallShape(P, Shape);
    if (Shape != Entry->Signature || P->GetOwnerClass()->GetPathName() != Entry->Declaring ||
        P->HasAnyPropertyFlags(CPF_Deprecated | CPF_EditorOnly) ||
        P->HasAnyPropertyFlags(CPF_BlueprintReadOnly) != Entry->ReadOnly)
        C->throw_error("native field shape/policy changed; regenerate bindings");
    FString AccessError;
    if (!ValidateDasPropertyAccess(P, DasNativeAccessContract(*Entry), AccessError))
        C->throw_error(TCHAR_TO_UTF8(*AccessError));
    return P;
}
FDasCallHandle BeginNativeGameplayCall(const FDasObjectHandle& H, const char* ClassPath, const char* Name,
                                       das::Context* C)
{
    auto* O = NativeGameplayObject(H, ClassPath, true, C);
    const FString Class =
                      ClassPath ? UTF8_TO_TCHAR(ClassPath) : TEXT(""), Method = Name ? UTF8_TO_TCHAR(Name) : TEXT("");
    if (Method.IsEmpty() || Method.Len() >= NAME_SIZE)
        C->throw_error("invalid native method name");
    const auto* Entry = DasNativeGameplayMethod(Class, Method);
    auto* F = Entry ? O->FindFunction(*Method) : nullptr;
    if (!F || !F->HasAnyFunctionFlags(FUNC_BlueprintCallable) ||
        F->HasAnyFunctionFlags(FUNC_Net | FUNC_Static | FUNC_Delegate | FUNC_MulticastDelegate | FUNC_EditorOnly) ||
        CallSignatureKey(F) != Entry->Signature)
        C->throw_error("native method missing or shape/policy changed; regenerate bindings");
    if (F->HasAnyFunctionFlags(FUNC_BlueprintAuthorityOnly))
    {
        auto* A = Cast<AActor>(O);
        if (!A)
            if (auto* Component = Cast<UActorComponent>(O))
                A = Component->GetOwner();
        if (A && !A->HasAuthority())
            C->throw_error("native method requires authority");
    }
    CheckConstructionCall(Entry->ConstructionUnsafe, C);
    auto Handle = BeginCall(H, Name, C);
    ResolveCall(Handle, C).bConstructionUnsafe |= Entry->ConstructionUnsafe;
    ResolveCall(Handle, C).bConstructionPolicyKnown = true;
    return Handle;
}

FDasStructValue NativeReferenceGetstruct(const FDasObjectHandle& H, const char* Path, const char* Name, das::Context* C,
                                         das::LineInfoArg* At)
{
    auto* O = NativeGameplayObject(H, Path, false, C);
    auto* P = NativeGameplayProperty(O, Path, Name, false, NativeGameplaySnapshotKind(Path, Name, C), C);
    const auto Access = ObjectPropertyAccess(P, C);
    if (!Access.NativeGetter && Access.Getter.IsNone())
        return ReadPropertySnapshot(P, P->ContainerPtrToValuePtr<void>(O), C, At);
    auto Value = ReadOwnerProperty(P, O, C);
    return StoreStructValue(MoveTemp(Value.StructValue), C, At);
}
void NativeReferenceSetstruct(const FDasObjectHandle& H, const char* Path, const char* Name,
                              const FDasStructValue& Value, das::Context* C)
{
    auto* O = NativeGameplayObject(H, Path, true, C);
    auto* P = NativeGameplayProperty(O, Path, Name, true, NativeGameplaySnapshotKind(Path, Name, C), C);
    const auto Access = ObjectPropertyAccess(P, C);
    if (!Access.NativeSetter && Access.Setter.IsNone())
    {
        WritePropertySnapshot(P, P->ContainerPtrToValuePtr<void>(O), Value, C);
        return;
    }
    FDasScalarValue V;
    V.Kind = int32(EDasMethodSignature::Struct);
    V.StructValue = ResolveStructValue(Value, C);
    WriteOwnerProperty(P, O, V, C);
}
FDasObjectHandle NativeReferenceGetobject(const FDasObjectHandle& H, const char* Path, const char* Name,
                                          const char* Expected, das::Context* C)
{
    auto* O = NativeGameplayObject(H, Path, false, C);
    auto* P = CastFieldChecked<FObjectProperty>(NativeGameplayProperty(O, Path, Name, false, TEXT("object"), C));
    if (P->PropertyClass->GetPathName() != UTF8_TO_TCHAR(Expected))
        C->throw_error("native object field type mismatch");
    return ReadPropertyObject(P, O, C);
}
void NativeReferenceSetobject(const FDasObjectHandle& H, const char* Path, const char* Name, const char* Expected,
                              const FDasObjectHandle& Value, das::Context* C)
{
    auto* O = NativeGameplayObject(H, Path, true, C);
    auto* P = CastFieldChecked<FObjectProperty>(NativeGameplayProperty(O, Path, Name, true, TEXT("object"), C));
    auto* V = Value.Token ? ResolveObject(Value, C) : nullptr;
    if (P->PropertyClass->GetPathName() != UTF8_TO_TCHAR(Expected) || (V && !V->IsA(P->PropertyClass)))
        C->throw_error("native object field type mismatch");
    WritePropertyObject(P, O, Value, C);
}
