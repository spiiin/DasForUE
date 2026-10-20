// Part of DasRuntime.cpp: Object handles: self object/class access, reflected object fields, authored fields and interface calls.
// Included in place; declaration order matters.
void RequireObjectContext(das::Context* Context)
{
    if (!IsInGameThread() || !ActiveContext || Context != ActiveContext)
        Context->throw_error("object handles require the active game-thread context");
}

FDasObjectHandle MakeObjectHandle(UObject* Object, UClass* Expected, das::Context* Context)
{
    RequireObjectContext(Context);
    if (Expected && Expected->HasAnyClassFlags(CLASS_Interface))
        Expected = UObject::StaticClass();
    if (!IsValid(Object) || (Object->IsA<AActor>() && CastChecked<AActor>(Object)->IsActorBeingDestroyed()))
        return {};
    for (auto It = ActiveContext->ObjectHandles.CreateIterator(); It; ++It)
    {
        if (!It.Value().Object.IsValid() || !It.Value().ExpectedClass.IsValid())
        {
            It.RemoveCurrent();
            continue;
        }
        if (It.Value().Object.Get() == Object && It.Value().ExpectedClass.Get() == Expected)
            return {It.Key()};
    }
    const uint64 Token = NextObjectHandle++;
    ActiveContext->ObjectHandles.Add(Token, {Object, Expected});
    return {Token};
}

UObject* TryObject(const FDasObjectHandle& Handle, das::Context* Context)
{
    RequireObjectContext(Context);
    const auto* Entry = ActiveContext->ObjectHandles.Find(Handle.Token);
    if (!Entry)
        return nullptr;
    UObject* Object = Entry->Object.Get();
    UClass* Expected = Entry->ExpectedClass.Get();
    if (!IsValid(Object) || !Expected || !Object->IsA(Expected))
        return nullptr;
    if (auto* Actor = Cast<AActor>(Object); Actor && Actor->IsActorBeingDestroyed())
        return nullptr;
    return Object;
}

UObject* ResolveObject(const FDasObjectHandle& Handle, das::Context* Context)
{
    UObject* Object = TryObject(Handle, Context);
    if (!Object)
        Context->throw_error("object handle is null, expired, or belongs to another context");
    return Object;
}

void LibraryScope(const FDasObjectHandle& H, bool RequiresWorld, const das::TBlock<void>& Block, das::Context* C)
{
    if (C != ActiveContext || !IsInGameThread())
        C->throw_error("Library scope requires active game-thread context");
    auto* Object = H.Token ? ResolveObject(H, C) : nullptr;
    auto* ResolvedWorld = IsValid(Object) && GEngine
                              ? GEngine->GetWorldFromContextObject(Object, EGetWorldErrorMode::ReturnNull)
                              : nullptr;
    if (RequiresWorld &&
        (!ResolvedWorld || !ResolvedWorld->IsGameWorld() || ResolvedWorld->bIsTearingDown || Object->IsTemplate() ||
         (Cast<AActor>(Object) && CastChecked<AActor>(Object)->IsActorBeingDestroyed()) ||
         (Cast<UActorComponent>(Object) && CastChecked<UActorComponent>(Object)->IsBeingDestroyed())))
        C->throw_error("Library requires a live game-world WorldContext");
    bool Good = false;
    {
        TStrongObjectPtr<UObject> Root(Object);
        TGuardValue<bool> Library(ActiveContext->bLibraryScope, true);
        TGuardValue<TWeakObjectPtr<UObject>> World(ActiveContext->LibraryWorld, Object);
        Good = C->runWithCatch([&]() { C->invoke(Block, nullptr, nullptr, nullptr); });
    }
    if (!Good)
        C->rethrow();
}
FDasObjectHandle NullObject()
{
    return {};
}
FDasObjectHandle ComponentOwner(const FDasObjectHandle& H, das::Context* C)
{
    auto* Component = Cast<UActorComponent>(ResolveObject(H, C));
    if (!Component)
        C->throw_error("expected ActorComponent handle");
    return MakeObjectHandle(Component->GetOwner(), AActor::StaticClass(), C);
}
FDasObjectHandle ObjectSelf(das::Context* Context)
{
    return MakeObjectHandle(ResolveSelf(Context), UObject::StaticClass(), Context);
}
bool ObjectValid(const FDasObjectHandle& Handle, das::Context* Context)
{
    return TryObject(Handle, Context) != nullptr;
}
UClass* ResolveClass(const char* Path, das::Context* Context)
{
    if (!Path || !*Path)
        Context->throw_error("a loaded full class path is required");
    const FString Name(UTF8_TO_TCHAR(Path));
    if (!Name.StartsWith(TEXT("/")) || !Name.Contains(TEXT(".")))
        Context->throw_error("use a full loaded class path");
    UClass* Class = FindObject<UClass>(nullptr, *Name);
    if (!Class)
        Context->throw_error("class is not loaded");
    return Class;
}
FDasObjectHandle ObjectValue(const char* Path, das::Context* C)
{
    const FString P = Path ? UTF8_TO_TCHAR(Path) : TEXT("");
    if (!P.StartsWith(TEXT("/Script/")))
        C->throw_error("Object field requires a native class path");
    ResolveClass(Path, C);
    return {};
}
FProperty* SelfObjectProperty(const char* N, bool Write, das::Context* C)
{
    auto* S = ResolveSelf(C);
    auto* P = S->GetClass()->FindPropertyByName(FName(UTF8_TO_TCHAR(N)));
    if (!P || P->ArrayDim != 1 || !DasReferencePropertyClass(P))
        C->throw_error(TCHAR_TO_UTF8(*(FString(TEXT("self reference property type mismatch: ")) + UTF8_TO_TCHAR(N))));
    if (!P->HasAnyPropertyFlags(CPF_BlueprintVisible) || (Write && P->HasAnyPropertyFlags(CPF_BlueprintReadOnly)))
        C->throw_error("self property access is not permitted");
    return P;
}
FDasObjectHandle ReadNullablePropertyObject(FProperty* P, const void* Memory, das::Context* C)
{
    // VM object getters expose destroyed references as null, including
    // nested/array/map-value getters. Queued captures still use the strict
    // value codec and their original-identity/lifetime checks.
    auto* Object = DasReferenceGet(P, Memory);
    if (Object && (!IsValid(Object) || (Cast<AActor>(Object) && CastChecked<AActor>(Object)->IsActorBeingDestroyed())))
        return {};
    const auto V = ReadNativePropertyValue(P, Memory, C);
    if (V.Kind != int32(EDasMethodSignature::Object))
        C->throw_error("Expected object/interface property");
    return MakeObjectHandle(V.ObjectValue.Get(), DasReferencePropertyClass(P), C);
}
FDasObjectHandle ReadPropertyObject(FProperty* P, UObject* O, das::Context* C)
{
    const auto Access = ObjectPropertyAccess(P, C);
    if (!Access.NativeGetter && Access.Getter.IsNone())
        return ReadNullablePropertyObject(P, P->ContainerPtrToValuePtr<void>(O), C);
    const auto V = ReadOwnerProperty(P, O, C);
    return MakeObjectHandle(V.ObjectValue.Get(), DasReferencePropertyClass(P), C);
}
void WritePropertyObject(FProperty* P, UObject* O, const FDasObjectHandle& Input, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = int32(EDasMethodSignature::Object);
    auto* Type = DasReferencePropertyClass(P);
    if (!Type)
        C->throw_error("Expected object/interface property");
    V.ObjectClass = Type->GetPathName();
    V.ObjectValue = Input.Token ? ResolveObject(Input, C) : nullptr;
    if (V.ObjectValue && !DasReferenceMatches(V.ObjectValue, Type))
        C->throw_error("object field class mismatch");
    WriteOwnerProperty(P, O, V, C);
}
UClass* ReadPropertyClass(FClassProperty* P, UObject* O, das::Context* C)
{
    return ReadOwnerProperty(P, O, C).ClassValue.Get();
}
void WritePropertyClass(FClassProperty* P, UObject* O, UClass* Class, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = int32(EDasMethodSignature::Class);
    V.ObjectClass = P->MetaClass->GetPathName();
    V.ClassValue = Class;
    WriteOwnerProperty(P, O, V, C);
}
FDasObjectHandle GetSelfObject(const char* N, das::Context* C)
{
    auto* P = SelfObjectProperty(N, false, C);
    return ReadPropertyObject(P, ResolveSelf(C), C);
}
void SetSelfObject(const char* N, const FDasObjectHandle& V, das::Context* C)
{
    ResolveSelf(C);
    if (ActiveContext->bCollectingSubscriptions)
        C->throw_error("self writes are forbidden in bind_events");
    auto* P = SelfObjectProperty(N, true, C);
    WritePropertyObject(P, ResolveSelf(C), V, C);
}
bool ObjectsEqual(const FDasObjectHandle& Left, const FDasObjectHandle& Right, das::Context* C)
{
    RequireObjectContext(C);
    auto* A = TryObject(Left, C);
    auto* B = TryObject(Right, C);
    return A && B && A == B;
}
bool ObjectIsA(const FDasObjectHandle& Handle, const char* ClassPath, das::Context* Context)
{
    UClass* Class = ResolveClass(ClassPath, Context);
    UObject* Object = TryObject(Handle, Context);
    return Object && Object->IsA(Class);
}
FDasObjectHandle ObjectCast(const FDasObjectHandle& Handle, const char* ClassPath, das::Context* Context)
{
    UObject* Object = ResolveObject(Handle, Context);
    UClass* Class = ResolveClass(ClassPath, Context);
    if (!DasReferenceMatches(Object, Class))
        Context->throw_error("object does not match requested class");
    return MakeObjectHandle(Object, Class, Context);
}
FDasObjectHandle ObjectCastNullable(const FDasObjectHandle& Handle, const char* ClassPath, das::Context* Context)
{
    RequireObjectContext(Context);
    UClass* Class = ResolveClass(ClassPath, Context);
    if (!Handle.Token)
        return {};
    UObject* Object = ResolveObject(Handle, Context);
    if (!DasReferenceMatches(Object, Class))
        Context->throw_error("object does not match requested class");
    return MakeObjectHandle(Object, Class, Context);
}

FDasObjectHandle ObjectProperty(const FDasObjectHandle& Handle, const char* Name, das::Context* Context)
{
    UObject* Object = ResolveObject(Handle, Context);
    auto* Property = ResolveProperty<FObjectProperty>(Object, Name, false, Context);
    return MakeObjectHandle(Property->GetObjectPropertyValue_InContainer(Object), Property->PropertyClass, Context);
}
UObject* ResolveAuthoredObject(const FDasObjectHandle& Handle, const char* Id, das::Context* C)
{
    auto* Object = ResolveObject(Handle, C);
    const UObject* CDO = Object->GetClass()->GetDefaultObject();
    const FString Actual = DasHostIdentity(CDO);
    bool Matches = false;
    if (Id)
        for (auto* Class = Object->GetClass(); Class; Class = Class->GetSuperClass())
        {
            const auto* Default = Class->GetDefaultObject();
            const FString Identity = DasHostIdentity(Default);
            if (Identity == UTF8_TO_TCHAR(Id) && !Identity.IsEmpty())
            {
                Matches = true;
                break;
            }
        }
    if (!Matches)
        C->throw_error("authored object identity mismatch");
    return Object;
}
bool LiveHashObject(UObject* O)
{
    return IsValid(O) && (!O->IsA<AActor>() || !CastChecked<AActor>(O)->IsActorBeingDestroyed());
}
uint64 InternObjectKey(UObject* O, das::Context* C)
{
    RequireObjectContext(C);
    if (!O)
        return 0;
    if (!LiveHashObject(O))
        C->throw_error("destroyed object cannot be a hash key");
    const FObjectKey Identity(O);
    if (const auto* Token = ActiveContext->ObjectKeyTokens.Find(Identity))
        return *Token;
    if (ActiveContext->ObjectKeys.Num() >= 65536)
        C->throw_error("object key token limit exceeded");
    static uint64 Next = 1;
    const uint64 Token = Next++;
    ActiveContext->ObjectKeyTokens.Add(Identity, Token);
    ActiveContext->ObjectKeys.Add(Token, O);
    return Token;
}
UObject* ResolveObjectKey(uint64 Token, das::Context* C)
{
    RequireObjectContext(C);
    if (!Token)
        return nullptr;
    const auto* Entry = ActiveContext->ObjectKeys.Find(Token);
    auto* O = Entry ? Entry->Get() : nullptr;
    if (!LiveHashObject(O))
        C->throw_error("object key is destroyed, expired, forged or belongs to another context");
    return O;
}
uint64 CheckedObjectKey(uint64 Token, const char* Path, const char* Id, das::Context* C)
{
    auto* O = ResolveObjectKey(Token, C);
    auto* Class = Path ? FindObject<UClass>(nullptr, UTF8_TO_TCHAR(Path)) : nullptr;
    if (!Class || Class->HasAnyClassFlags(CLASS_Interface) || (O && !O->IsA(Class)))
        C->throw_error("object key class constraint mismatch");
    if (O && Id && *Id)
        ResolveAuthoredObject(MakeObjectHandle(O, Class, C), Id, C);
    return Token;
}
uint64 ObjectKeyFromReference(const FDasObjectHandle& H, const char* Path, const char* Id, das::Context* C)
{
    return CheckedObjectKey(InternObjectKey(H.Token ? ResolveObject(H, C) : nullptr, C), Path, Id, C);
}
FDasObjectHandle ObjectKeyReference(uint64 Token, const char* Path, const char* Id, das::Context* C)
{
    CheckedObjectKey(Token, Path, Id, C);
    return MakeObjectHandle(ResolveObjectKey(Token, C), FindObject<UClass>(nullptr, UTF8_TO_TCHAR(Path)), C);
}
bool ObjectKeyValid(uint64 Token, das::Context* C)
{
    RequireObjectContext(C);
    const auto* Entry = ActiveContext->ObjectKeys.Find(Token);
    return Entry && LiveHashObject(Entry->Get());
}
uint64 StructSetElementObjectKey(const FDasStructValue& H, const char* Path, int32 Index, das::Context* C)
{
    auto F = SnapshotKeyAt(
        H, Path, SnapshotSet(StructLeaf(ResolveStructValue(H, C), Path, C), ESnapshotKind::Object, C), Index, C);
    return InternObjectKey(ReadNativePropertyValue(F.Property, F.Memory, C).ObjectValue.Get(), C);
}
FDasStructValue StructSetWithObjectKey(const FDasStructValue& H, const char* Path, uint64 Token, bool Present,
                                       das::Context* C, das::LineInfoArg* At)
{
    UObject* O = ResolveObjectKey(Token, C);
    auto View = SnapshotSet(StructLeaf(ResolveStructValue(H, C), Path, C), ESnapshotKind::Object, C);
    if (O && !DasReferenceMatches(O, DasReferencePropertyClass(View.Property->ElementProp)))
        C->throw_error("snapshot object set class mismatch");
    TObjectPtr<UObject> Key = O;
    return SetSnapshotValue(H, Path, &Key, ESnapshotKind::Object, Present, C, At);
}
TObjectPtr<UObject> ObjectMapKey(const FDasStructValue& H, const char* Path, uint64 Token, das::Context* C)
{
    UObject* O = ResolveObjectKey(Token, C);
    const auto& V = ResolveStructValue(H, C);
    auto F = StructLeaf(V, Path, C);
    auto* P = CastField<FMapProperty>(F.Property);
    if (!P || !IsDasObjectHashProperty(P->KeyProp) ||
        (O && !DasReferenceMatches(O, DasReferencePropertyClass(P->KeyProp))))
        C->throw_error("snapshot map object key class mismatch");
    return O;
}
uint64 StructMapObjectKey(const FDasStructValue& H, const char* Path, int32 Index, das::Context* C)
{
    auto F = SnapshotKeyAt(
        H, Path,
        SnapshotMap(StructLeaf(ResolveStructValue(H, C), Path, C), ESnapshotKind::Object, ESnapshotKind::Invalid, C),
        Index, C);
    return InternObjectKey(ReadNativePropertyValue(F.Property, F.Memory, C).ObjectValue.Get(), C);
}
int32 StructMapGetObjectKeyInt(const FDasStructValue& H, const char* Path, uint64 Token, das::Context* C)
{
    return StructScalarMapGet<FIntProperty>(H, Path, ObjectMapKey(H, Path, Token, C), ESnapshotKind::Object, C);
}
FDasStructValue StructMapWithObjectKeyInt(const FDasStructValue& H, const char* Path, uint64 Token, int32 Value,
                                          das::Context* C, das::LineInfoArg* At)
{
    return StructScalarMapWith<FIntProperty>(H, Path, ObjectMapKey(H, Path, Token, C), Value, ESnapshotKind::Object, C,
                                             At);
}
float StructMapGetObjectKeyFloat(const FDasStructValue& H, const char* Path, uint64 Token, das::Context* C)
{
    return StructScalarMapGet<FFloatProperty>(H, Path, ObjectMapKey(H, Path, Token, C), ESnapshotKind::Object, C);
}
FDasStructValue StructMapWithObjectKeyFloat(const FDasStructValue& H, const char* Path, uint64 Token, float Value,
                                            das::Context* C, das::LineInfoArg* At)
{
    return StructScalarMapWith<FFloatProperty>(H, Path, ObjectMapKey(H, Path, Token, C), Value, ESnapshotKind::Object,
                                               C, At);
}
double StructMapGetObjectKeyDouble(const FDasStructValue& H, const char* Path, uint64 Token, das::Context* C)
{
    return StructScalarMapGet<FDoubleProperty>(H, Path, ObjectMapKey(H, Path, Token, C), ESnapshotKind::Object, C);
}
FDasStructValue StructMapWithObjectKeyDouble(const FDasStructValue& H, const char* Path, uint64 Token, double Value,
                                             das::Context* C, das::LineInfoArg* At)
{
    return StructScalarMapWith<FDoubleProperty>(H, Path, ObjectMapKey(H, Path, Token, C), Value, ESnapshotKind::Object,
                                                C, At);
}
bool StructMapGetObjectKeyBool(const FDasStructValue& H, const char* Path, uint64 Token, das::Context* C)
{
    return StructScalarMapGet<FBoolProperty>(H, Path, ObjectMapKey(H, Path, Token, C), ESnapshotKind::Object, C);
}
FDasStructValue StructMapWithObjectKeyBool(const FDasStructValue& H, const char* Path, uint64 Token, bool Value,
                                           das::Context* C, das::LineInfoArg* At)
{
    return StructScalarMapWith<FBoolProperty>(H, Path, ObjectMapKey(H, Path, Token, C), Value, ESnapshotKind::Object, C,
                                              At);
}
char* StructMapGetObjectKeyString(const FDasStructValue& H, const char* Path, uint64 Token, das::Context* C)
{
    return CopyDSLString(
        StructScalarMapGet<FStrProperty>(H, Path, ObjectMapKey(H, Path, Token, C), ESnapshotKind::Object, C), C);
}
FDasStructValue StructMapWithObjectKeyString(const FDasStructValue& H, const char* Path, uint64 Token,
                                             const char* Value, das::Context* C, das::LineInfoArg* At)
{
    return StructScalarMapWith<FStrProperty>(H, Path, ObjectMapKey(H, Path, Token, C),
                                             FString(Value ? UTF8_TO_TCHAR(Value) : TEXT("")), ESnapshotKind::Object, C,
                                                     At);
}
FDasNameValue StructMapGetObjectKeyName(const FDasStructValue& H, const char* Path, uint64 Token, das::Context* C)
{
    return MakeDSLName(
        StructScalarMapGet<FNameProperty>(H, Path, ObjectMapKey(H, Path, Token, C), ESnapshotKind::Object, C), C);
}
FDasStructValue StructMapWithObjectKeyName(const FDasStructValue& H, const char* Path, uint64 Token,
                                           FDasNameValue Value, das::Context* C, das::LineInfoArg* At)
{
    return StructScalarMapWith<FNameProperty>(H, Path, ObjectMapKey(H, Path, Token, C), ResolveDSLName(Value, C),
                                              ESnapshotKind::Object, C, At);
}
FDasEnumValue StructMapGetObjectKeyEnum(const FDasStructValue& H, const char* Path, uint64 Token, das::Context* C)
{
    return StructMapGetEnum(H, Path, ObjectMapKey(H, Path, Token, C), ESnapshotKind::Object, C);
}
FDasStructValue StructMapWithObjectKeyEnum(const FDasStructValue& H, const char* Path, uint64 Token,
                                           FDasEnumValue Value, das::Context* C, das::LineInfoArg* At)
{
    return StructMapWithEnum(H, Path, ObjectMapKey(H, Path, Token, C), Value, ESnapshotKind::Object, C, At);
}
FDasObjectHandle StructMapGetObjectKeyObject(const FDasStructValue& H, const char* Path, uint64 Token, das::Context* C)
{
    return StructMapGetObject(H, Path, ObjectMapKey(H, Path, Token, C), ESnapshotKind::Object, C);
}
FDasStructValue StructMapWithObjectKeyObject(const FDasStructValue& H, const char* Path, uint64 Token,
                                             const FDasObjectHandle& Value, das::Context* C, das::LineInfoArg* At)
{
    return StructMapWithObject(H, Path, ObjectMapKey(H, Path, Token, C), Value, ESnapshotKind::Object, C, At);
}
FDasStructValue StructMapGetObjectKeyStruct(const FDasStructValue& H, const char* Path, uint64 Token, das::Context* C,
                                            das::LineInfoArg* At)
{
    return StructMapGetStruct(H, Path, ObjectMapKey(H, Path, Token, C), ESnapshotKind::Object, C, At);
}
FDasStructValue StructMapWithObjectKeyStruct(const FDasStructValue& H, const char* Path, uint64 Token,
                                             const FDasStructValue& Value, das::Context* C, das::LineInfoArg* At)
{
    return StructMapWithStruct(H, Path, ObjectMapKey(H, Path, Token, C), Value, ESnapshotKind::Object, C, At);
}
uint8 EnumMapKey(const FDasStructValue& H, const char* Path, FDasEnumValue Token, das::Context* C)
{
    const auto& E = ResolveEnumValue(Token, C);
    const auto& V = ResolveStructValue(H, C);
    auto F = StructLeaf(V, Path, C);
    auto* P = CastField<FMapProperty>(F.Property);
    if (!P || DasEnumPropertyType(P->KeyProp) != E.Type.Get() || P->KeyProp->GetElementSize() != 1 || E.Value < 0 ||
        E.Value > 255)
        C->throw_error("snapshot map enum key identity/storage mismatch");
    return uint8(E.Value);
}
FDasEnumValue StructMapEnumKey(const FDasStructValue& H, const char* Path, int32 Index, das::Context* C)
{
    auto F = SnapshotKeyAt(
        H, Path,
        SnapshotMap(StructLeaf(ResolveStructValue(H, C), Path, C), ESnapshotKind::Enum, ESnapshotKind::Invalid, C),
        Index, C);
    auto* E = DasEnumPropertyType(F.Property);
    return MakeEnumValue(E->GetPathName(), ReadNativeEnumNumber(F.Property, F.Memory, C), C);
}
int32 StructMapGetEnumKeyInt(const FDasStructValue& H, const char* Path, FDasEnumValue Token, das::Context* C)
{
    return StructScalarMapGet<FIntProperty>(H, Path, EnumMapKey(H, Path, Token, C), ESnapshotKind::Enum, C);
}
FDasStructValue StructMapWithEnumKeyInt(const FDasStructValue& H, const char* Path, FDasEnumValue Token, int32 Value,
                                        das::Context* C, das::LineInfoArg* At)
{
    return StructScalarMapWith<FIntProperty>(H, Path, EnumMapKey(H, Path, Token, C), Value, ESnapshotKind::Enum, C, At);
}
float StructMapGetEnumKeyFloat(const FDasStructValue& H, const char* Path, FDasEnumValue Token, das::Context* C)
{
    return StructScalarMapGet<FFloatProperty>(H, Path, EnumMapKey(H, Path, Token, C), ESnapshotKind::Enum, C);
}
FDasStructValue StructMapWithEnumKeyFloat(const FDasStructValue& H, const char* Path, FDasEnumValue Token, float Value,
                                          das::Context* C, das::LineInfoArg* At)
{
    return StructScalarMapWith<FFloatProperty>(H, Path, EnumMapKey(H, Path, Token, C), Value, ESnapshotKind::Enum, C,
                                               At);
}
double StructMapGetEnumKeyDouble(const FDasStructValue& H, const char* Path, FDasEnumValue Token, das::Context* C)
{
    return StructScalarMapGet<FDoubleProperty>(H, Path, EnumMapKey(H, Path, Token, C), ESnapshotKind::Enum, C);
}
FDasStructValue StructMapWithEnumKeyDouble(const FDasStructValue& H, const char* Path, FDasEnumValue Token,
                                           double Value, das::Context* C, das::LineInfoArg* At)
{
    return StructScalarMapWith<FDoubleProperty>(H, Path, EnumMapKey(H, Path, Token, C), Value, ESnapshotKind::Enum, C,
                                                At);
}
bool StructMapGetEnumKeyBool(const FDasStructValue& H, const char* Path, FDasEnumValue Token, das::Context* C)
{
    return StructScalarMapGet<FBoolProperty>(H, Path, EnumMapKey(H, Path, Token, C), ESnapshotKind::Enum, C);
}
FDasStructValue StructMapWithEnumKeyBool(const FDasStructValue& H, const char* Path, FDasEnumValue Token, bool Value,
                                         das::Context* C, das::LineInfoArg* At)
{
    return StructScalarMapWith<FBoolProperty>(H, Path, EnumMapKey(H, Path, Token, C), Value, ESnapshotKind::Enum, C,
                                              At);
}
char* StructMapGetEnumKeyString(const FDasStructValue& H, const char* Path, FDasEnumValue Token, das::Context* C)
{
    return CopyDSLString(
        StructScalarMapGet<FStrProperty>(H, Path, EnumMapKey(H, Path, Token, C), ESnapshotKind::Enum, C), C);
}
FDasStructValue StructMapWithEnumKeyString(const FDasStructValue& H, const char* Path, FDasEnumValue Token,
                                           const char* Value, das::Context* C, das::LineInfoArg* At)
{
    return StructScalarMapWith<FStrProperty>(H, Path, EnumMapKey(H, Path, Token, C),
                                             FString(Value ? UTF8_TO_TCHAR(Value) : TEXT("")), ESnapshotKind::Enum, C,
                                                     At);
}
FDasNameValue StructMapGetEnumKeyName(const FDasStructValue& H, const char* Path, FDasEnumValue Token, das::Context* C)
{
    return MakeDSLName(
        StructScalarMapGet<FNameProperty>(H, Path, EnumMapKey(H, Path, Token, C), ESnapshotKind::Enum, C), C);
}
FDasStructValue StructMapWithEnumKeyName(const FDasStructValue& H, const char* Path, FDasEnumValue Token,
                                         FDasNameValue Value, das::Context* C, das::LineInfoArg* At)
{
    return StructScalarMapWith<FNameProperty>(H, Path, EnumMapKey(H, Path, Token, C), ResolveDSLName(Value, C),
                                              ESnapshotKind::Enum, C, At);
}
FDasEnumValue StructMapGetEnumKeyEnum(const FDasStructValue& H, const char* Path, FDasEnumValue Token, das::Context* C)
{
    return StructMapGetEnum(H, Path, EnumMapKey(H, Path, Token, C), ESnapshotKind::Enum, C);
}
FDasStructValue StructMapWithEnumKeyEnum(const FDasStructValue& H, const char* Path, FDasEnumValue Token,
                                         FDasEnumValue Value, das::Context* C, das::LineInfoArg* At)
{
    return StructMapWithEnum(H, Path, EnumMapKey(H, Path, Token, C), Value, ESnapshotKind::Enum, C, At);
}
FDasObjectHandle StructMapGetEnumKeyObject(const FDasStructValue& H, const char* Path, FDasEnumValue Token,
                                           das::Context* C)
{
    return StructMapGetObject(H, Path, EnumMapKey(H, Path, Token, C), ESnapshotKind::Enum, C);
}
FDasStructValue StructMapWithEnumKeyObject(const FDasStructValue& H, const char* Path, FDasEnumValue Token,
                                           const FDasObjectHandle& Value, das::Context* C, das::LineInfoArg* At)
{
    return StructMapWithObject(H, Path, EnumMapKey(H, Path, Token, C), Value, ESnapshotKind::Enum, C, At);
}
FDasStructValue StructMapGetEnumKeyStruct(const FDasStructValue& H, const char* Path, FDasEnumValue Token,
                                          das::Context* C, das::LineInfoArg* At)
{
    return StructMapGetStruct(H, Path, EnumMapKey(H, Path, Token, C), ESnapshotKind::Enum, C, At);
}
FDasStructValue StructMapWithEnumKeyStruct(const FDasStructValue& H, const char* Path, FDasEnumValue Token,
                                           const FDasStructValue& Value, das::Context* C, das::LineInfoArg* At)
{
    return StructMapWithStruct(H, Path, EnumMapKey(H, Path, Token, C), Value, ESnapshotKind::Enum, C, At);
}


FDasObjectHandle AuthoredObjectCheck(const FDasObjectHandle& H, const char* Id, das::Context* C)
{
    ResolveAuthoredObject(H, Id, C);
    return H;
}
FDasObjectHandle AuthoredNullableCheck(const FDasObjectHandle& Handle, const char* Id, das::Context* Context)
{
    RequireObjectContext(Context);
    if (!Handle.Token)
        return {};
    ResolveAuthoredObject(Handle, Id, Context);
    return Handle;
}


FDasStructValue AuthoredGetstruct(const FDasObjectHandle& H, const char* Id, const char* Name, das::Context* C,
                                  das::LineInfoArg* At)
{
    auto* O = ResolveAuthoredObject(H, Id, C);
    auto* P = ResolveProperty<FProperty>(O, Name, false, C);
    return ReadPropertySnapshot(P, P->ContainerPtrToValuePtr<void>(O), C, At);
}
void AuthoredSetstruct(const FDasObjectHandle& H, const char* Id, const char* Name, const FDasStructValue& Value,
                       das::Context* C)
{
    auto* O = ResolveAuthoredObject(H, Id, C);
    if (ActiveContext->bCollectingSubscriptions)
        C->throw_error("object writes are forbidden in bind_events");
    auto* P = ResolveProperty<FProperty>(O, Name, true, C);
    WritePropertySnapshot(P, P->ContainerPtrToValuePtr<void>(O), Value, C);
}
FDasObjectHandle AuthoredGetobject(const FDasObjectHandle& H, const char* Id, const char* Name, const char* Expected,
                                   das::Context* C)
{
    auto* O = ResolveAuthoredObject(H, Id, C);
    auto* P = ResolveProperty<FProperty>(O, Name, false, C);
    auto* Type = DasReferencePropertyClass(P);
    if (!Type)
        C->throw_error("Expected authored object/interface property");
    if (Expected && *Expected && Type->GetPathName() != UTF8_TO_TCHAR(Expected))
        C->throw_error("Authored object property class identity mismatch");
    return ReadPropertyObject(P, O, C);
}
void AuthoredSetobject(const FDasObjectHandle& H, const char* Id, const char* Name, const char* Expected,
                       const FDasObjectHandle& Value, das::Context* C)
{
    auto* O = ResolveAuthoredObject(H, Id, C);
    if (ActiveContext->bCollectingSubscriptions)
        C->throw_error("object writes are forbidden in bind_events");
    auto* P = ResolveProperty<FProperty>(O, Name, true, C);
    auto* Type = DasReferencePropertyClass(P);
    if (!Type || (Expected && *Expected && Type->GetPathName() != UTF8_TO_TCHAR(Expected)))
        C->throw_error("Authored object property class identity mismatch");
    WritePropertyObject(P, O, Value, C);
}

int32 ObjectGetInt(const FDasObjectHandle& Handle, const char* Name, das::Context* Context)
{
    UObject* Object = ResolveObject(Handle, Context);
    return ReadPropertyScalar(ResolveProperty<FIntProperty>(Object, Name, false, Context), Object, Context);
}
void ObjectSetInt(const FDasObjectHandle& Handle, const char* Name, int32 Value, das::Context* Context)
{
    UObject* Object = ResolveObject(Handle, Context);
    if (ActiveContext->bCollectingSubscriptions)
        Context->throw_error("object writes are forbidden in bind_events");
    WritePropertyScalar(ResolveProperty<FIntProperty>(Object, Name, true, Context), Object, Value, Context);
}

enum class ECallKind
{
    None,
    Int,
    Float,
    Object
};
struct FCheckedCall
{
    UFunction* Function;
    FProperty* Input = nullptr;
    FProperty* Output = nullptr;
};
bool MatchesCallProperty(FProperty* Property, ECallKind Kind)
{
    if (!Property || Property->ArrayDim != 1)
        return false;
    if (Kind == ECallKind::Int)
        return CastField<FIntProperty>(Property) != nullptr;
    if (Kind == ECallKind::Float)
        return CastField<FFloatProperty>(Property) != nullptr;
    if (Kind == ECallKind::Object)
        return CastField<FObjectProperty>(Property) != nullptr && !CastField<FClassProperty>(Property);
    return false;
}
FCheckedCall CheckCall(UObject* Object, const char* Name, ECallKind Input, ECallKind Output, das::Context* Context)
{
    if (ActiveContext->bCollectingSubscriptions)
        Context->throw_error("UFunction calls are forbidden in bind_events");
    if (!Name || !*Name || FCStringAnsi::Strlen(Name) >= NAME_SIZE)
        Context->throw_error("invalid function name");
    UFunction* Function = Object->FindFunction(FName(UTF8_TO_TCHAR(Name)));
    if (!Function || !Function->HasAnyFunctionFlags(FUNC_BlueprintCallable) ||
        Function->HasAnyFunctionFlags(FUNC_Static | FUNC_Net | FUNC_Delegate | FUNC_MulticastDelegate))
        Context->throw_error("expected a local BlueprintCallable instance UFunction");
    FCheckedCall Call{Function};
    int32 Inputs = 0;
    for (TFieldIterator<FProperty> It(Function); It; ++It)
    {
        FProperty* Property = *It;
        if (!Property->HasAnyPropertyFlags(CPF_Parm))
            continue;
        if (Property->HasAnyPropertyFlags(CPF_ReturnParm))
            Call.Output = Property;
        else
        {
            ++Inputs;
            Call.Input = Property;
            if (Property->HasAnyPropertyFlags(CPF_OutParm | CPF_ReferenceParm))
                Context->throw_error("out/ref parameters are not supported");
        }
    }
    if (Inputs != (Input == ECallKind::None ? 0 : 1) ||
        (Input != ECallKind::None && !MatchesCallProperty(Call.Input, Input)) ||
        (Output == ECallKind::None ? Call.Output != nullptr : !MatchesCallProperty(Call.Output, Output)))
        Context->throw_error("UFunction signature does not match the requested call adapter");
    return Call;
}
int32 CallInt(const FDasObjectHandle& Handle, const char* Name, int32 Value, das::Context* Context)
{
    UObject* Object = ResolveObject(Handle, Context);
    const auto Call = CheckCall(Object, Name, ECallKind::Int, ECallKind::Int, Context);
    FStructOnScope Frame(Call.Function);
    FDasScalarValue Native;
    Native.Kind = int32(EDasMethodSignature::Int);
    Native.IntValue = Value;
    WriteNativePropertyValue(Call.Input, Call.Input->ContainerPtrToValuePtr<void>(Frame.GetStructMemory()), Native,
                             Context);
    TStrongObjectPtr<UObject> KeepAlive(Object);
    Object->ProcessEvent(Call.Function, Frame.GetStructMemory());
    return ReadNativePropertyValue(Call.Output, Call.Output->ContainerPtrToValuePtr<void>(Frame.GetStructMemory()),
                                   Context)
        .IntValue;
}
UClass* CheckedInterface(const char* Path, das::Context* C)
{
    auto* I = ResolveClass(Path, C);
    if ((!FString(Path).StartsWith(TEXT("/Script/")) && !FString(Path).StartsWith(TEXT("/Game/"))) ||
          !I->HasAnyClassFlags(CLASS_Interface))
        C->throw_error("expected loaded native UInterface path");
    return I;
}
FDasObjectHandle CastInterface(const FDasObjectHandle& Handle, const char* Path, das::Context* C)
{
    auto* Interface = CheckedInterface(Path, C);
    if (!Handle.Token)
        return {};
    auto* Object = ResolveObject(Handle, C);
    if (!Object->GetClass()->ImplementsInterface(Interface))
        C->throw_error("object does not implement requested interface");
    return Handle;
}
bool ObjectImplementsInterface(const FDasObjectHandle& H, const char* Path, das::Context* C)
{
    auto* I = CheckedInterface(Path, C);
    auto* O = TryObject(H, C);
    return O && O->GetClass()->ImplementsInterface(I);
}
UFunction* CheckedInterfaceFunction(UObject* O, const char* Path, const char* Name, das::Context* C)
{
    auto* I = CheckedInterface(Path, C);
    if (!O->GetClass()->ImplementsInterface(I))
        C->throw_error("object does not implement requested interface");
    if (!Name || !*Name || FCStringAnsi::Strlen(Name) >= NAME_SIZE)
        C->throw_error("expected bounded interface method name");
    auto* Declared = I->FindFunctionByName(FName(UTF8_TO_TCHAR(Name)));
    auto* Actual = O->FindFunction(FName(UTF8_TO_TCHAR(Name)));
    if (!Declared || !Declared->HasAnyFunctionFlags(FUNC_BlueprintEvent) || !Actual)
        C->throw_error("reflected interface implementation is unavailable");
    if (!Actual->IsSignatureCompatibleWith(Declared))
        C->throw_error("interface dispatch signature mismatch");
    return Actual;
}
int32 InterfaceCallInt(const FDasObjectHandle& H, const char* Path, const char* Name, int32 Input, das::Context* C);
char* InterfaceCallString(const FDasObjectHandle& H, const char* Path, const char* Name, das::Context* C);
float CallFloat(const FDasObjectHandle& Handle, const char* Name, float Value, das::Context* Context)
{
    UObject* Object = ResolveObject(Handle, Context);
    const auto Call = CheckCall(Object, Name, ECallKind::Float, ECallKind::Float, Context);
    FStructOnScope Frame(Call.Function);
    FDasScalarValue Native;
    Native.Kind = int32(EDasMethodSignature::Float);
    Native.FloatValue = Value;
    WriteNativePropertyValue(Call.Input, Call.Input->ContainerPtrToValuePtr<void>(Frame.GetStructMemory()), Native,
                             Context);
    TStrongObjectPtr<UObject> KeepAlive(Object);
    Object->ProcessEvent(Call.Function, Frame.GetStructMemory());
    return ReadNativePropertyValue(Call.Output, Call.Output->ContainerPtrToValuePtr<void>(Frame.GetStructMemory()),
                                   Context)
        .FloatValue;
}
void CallVoidInt(const FDasObjectHandle& Handle, const char* Name, int32 Value, das::Context* Context)
{
    UObject* Object = ResolveObject(Handle, Context);
    const auto Call = CheckCall(Object, Name, ECallKind::Int, ECallKind::None, Context);
    FStructOnScope Frame(Call.Function);
    FDasScalarValue Native;
    Native.Kind = int32(EDasMethodSignature::Int);
    Native.IntValue = Value;
    WriteNativePropertyValue(Call.Input, Call.Input->ContainerPtrToValuePtr<void>(Frame.GetStructMemory()), Native,
                             Context);
    TStrongObjectPtr<UObject> KeepAlive(Object);
    Object->ProcessEvent(Call.Function, Frame.GetStructMemory());
}
FDasObjectHandle CallObject(const FDasObjectHandle& Handle, const char* Name, das::Context* Context)
{
    UObject* Object = ResolveObject(Handle, Context);
    const auto Call = CheckCall(Object, Name, ECallKind::None, ECallKind::Object, Context);
    FStructOnScope Frame(Call.Function);
    TStrongObjectPtr<UObject> KeepAlive(Object);
    Object->ProcessEvent(Call.Function, Frame.GetStructMemory());
    auto* Return = CastFieldChecked<FObjectProperty>(Call.Output);
    return ReadNullablePropertyObject(Return, Return->ContainerPtrToValuePtr<void>(Frame.GetStructMemory()), Context);
}
void CallVoidObject(const FDasObjectHandle& Handle, const char* Name, const FDasObjectHandle& Value,
                    das::Context* Context)
{
    UObject* Object = ResolveObject(Handle, Context);
    const auto Call = CheckCall(Object, Name, ECallKind::Object, ECallKind::None, Context);
    UObject* Argument = Value.Token == 0 ? nullptr : ResolveObject(Value, Context);
    auto* Property = CastFieldChecked<FObjectProperty>(Call.Input);
    if (Argument && !Argument->IsA(Property->PropertyClass))
        Context->throw_error("object argument has an incompatible class");
    FStructOnScope Frame(Call.Function);
    FDasScalarValue Native;
    Native.Kind = int32(EDasMethodSignature::Object);
    Native.ObjectClass = Property->PropertyClass->GetPathName();
    Native.ObjectValue = Argument;
    WriteNativePropertyValue(Property, Property->ContainerPtrToValuePtr<void>(Frame.GetStructMemory()), Native,
                             Context);
    TStrongObjectPtr<UObject> KeepAlive(Object);
    TStrongObjectPtr<UObject> KeepArgumentAlive(Argument);
    Object->ProcessEvent(Call.Function, Frame.GetStructMemory());
}
