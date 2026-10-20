// Part of DasRuntime.cpp: Runtime state: script file access, object/handle tables, active context and world-context helpers.
// Included in place; declaration order matters.
class FUnrealTextFile final : public das::FileInfo
{
public:
    TArray<uint8> Bytes;
    virtual void getSourceAndLength(const char*& Source, uint32_t& Length) override
    {
        Source = reinterpret_cast<const char*>(Bytes.GetData());
        Length = static_cast<uint32_t>(Bytes.Num() - 1);
    }
};

// Reads through UE's platform file chain, including staged Pak/UFS files.
class FUnrealFileSystem final : public das::AnyFileSystem
{
public:
    FSourceSet* Sources = nullptr;
    explicit FUnrealFileSystem(FSourceSet* InSources = nullptr)
        : Sources(InSources ? InSources : AuthoredReferenceSources)
    {
    }
    virtual das::FileInfo* tryOpenFile(const das::string& Name) override
    {
        const FString Path(UTF8_TO_TCHAR(Name.c_str()));
        auto Snapshot = FSourceSnapshot::Read(Path);
        if (Sources)
            Sources->Add(Path, Snapshot);
        if (!Snapshot.bExists)
            return nullptr;
        auto File = MakeUnique<FUnrealTextFile>();
        File->Bytes = MoveTemp(Snapshot.Bytes);
        File->Bytes.Add(0);
        File->name = Name;
        return File.Release();
    }
};

// Resolve flat project modules before plugin modules; daslib keeps its own resolver.
class FUnrealFileAccess final : public das::FsFileAccess
{
public:
    virtual das::ModuleInfo getModuleInfo(const das::string& Request, const das::string& From) const override
    {
        const FString Name = UTF8_TO_TCHAR(Request.c_str());
        bool Identifier = !Name.IsEmpty();
        for (int32 I = 0; I < Name.Len(); ++I)
            Identifier &=
                (I == 0 ? !FChar::IsDigit(Name[I]) : true) && (FChar::IsAlnum(Name[I]) || Name[I] == TCHAR('_'));
        if (Identifier)
        {
            const auto* Runtime = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"));
            const FString Path = Runtime ? Runtime->ScriptPath(Name + TEXT(".das")) : FString();
            if (!Path.IsEmpty() && IFileManager::Get().FileExists(*Path))
            {
                das::ModuleInfo Info;
                Info.moduleName = Request;
                Info.fileName = TCHAR_TO_UTF8(*Path);
                return Info;
            }
        }
        return das::FsFileAccess::getModuleInfo(Request, From);
    }
};

struct FSubscriptionDeclaration
{
    TWeakObjectPtr<UObject> Source;
    FName Event;
    FString Handler;
    EDasMethodSignature Kind = EDasMethodSignature::Int;
    FString PayloadPath;
};

struct FObjectHandleEntry
{
    TWeakObjectPtr<UObject> Object;
    TWeakObjectPtr<UClass> ExpectedClass;
};
uint64 NextObjectHandle = 1;

enum class EMarshallingKind
{
    Int,
    Float,
    Double,
    Bool,
    String,
    Struct,
    Object,
    Name,
    Enum,
    Array,
    Map,
    Set,
    Delegate,
    Class,
    Byte,
    Text
};
// Flattened scalar signature IR: names/types/directions, independent of property addresses.
struct FMarshalledParameter
{
    FString Path;
    EMarshallingKind Kind;
    bool bInput;
    bool bOutput;
    FString TypePath;
};
struct FHashValue
{
    FProperty* Property;
    void* Memory;
    explicit FHashValue(FProperty* InProperty) : Property(InProperty)
    {
        Memory = FMemory::Malloc(Property->GetSize(), Property->GetMinAlignment());
        Property->InitializeValue(Memory);
    }
    ~FHashValue()
    {
        Property->DestroyValue(Memory);
        FMemory::Free(Memory);
    }
    FHashValue(const FHashValue&) = delete;
    FHashValue& operator=(const FHashValue&) = delete;
};
struct FPendingHash
{
    TArray<TUniquePtr<FHashValue>> Keys;
    TArray<TUniquePtr<FHashValue>> Values;
};
struct FMarshalledCall
{
    TWeakObjectPtr<UObject> Target;
    TStrongObjectPtr<UFunction> Function;
    TUniquePtr<FStructOnScope> Frame;
    TSharedPtr<FDasWidgetAnimationLease> WidgetAnimationLease;
    TMap<FString, uint64> WidgetAnimationResults;
    TMap<FString, uint64> InputEventTokens;
    uint64 PaintEpoch = 0;
    uint64 WidgetReplyEpoch = 0;
    TMap<FString, uint64> WidgetReplyFields;
    TMap<FString, FInstancedStruct> ConstraintRecords;
    TSet<FString> Assigned;
    TSet<FString> WholeInputs;
    TMap<FString, TStrongObjectPtr<UObject>> ObjectPins;
    TArray<FMarshalledParameter> Signature;
    FString SignatureKey;
    TWeakObjectPtr<UClass> RequiredInterface;
    TWeakObjectPtr<UClass> FunctionScopeClass;
    FString InterfaceSignatureKey;
    TMap<FString, TSharedPtr<FPendingHash>> PendingHashes;
    TMap<FString, TArray<int32>> HashIndices;
    TFunction<void(void*)> DelegateInvoke;
    FString AutomaticWorldContext;
    TWeakObjectPtr<UWorld> StaticWorld;
    FString CallbackFramePath;
    bool bConstructionUnsafe = false;
    bool bConstructionPolicyKnown = false;
    bool bAuthored = false;
    bool bExecuted = false;
    bool bFailed = false;
};
struct FDelegateValue
{
    FString Wire;
    TStrongObjectPtr<UFunction> Signature;
    bool Multicast = false;
    FScriptDelegate Single;
    TArray<FScriptDelegate> Listeners;
    TWeakObjectPtr<UObject> Source;
    FName Property;
};
uint64 NextCallHandle = 1;

uint64 NextTraceScope = 1;
// Generation of the self property site caches (DasScalarAccessors.inl); bumped when contexts come and go
// and after garbage collection, which may reuse class addresses.
uint64 GDasSelfSiteEpoch = 1;
bool GDasSelfSitesEnabled = true; // foundation benchmark compares both paths
class FUnrealDasContext final : public das::Context, public FGCObject
{
public:
    explicit FUnrealDasContext(uint32 StackSize) : das::Context(StackSize) { ++GDasSelfSiteEpoch; }
    virtual ~FUnrealDasContext() { ++GDasSelfSiteEpoch; }
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override
    {
        for (auto& Value : StructValues)
            Value.Value.AddStructReferencedObjects(Collector);
        for (auto& Edit : StructEdits)
            Edit.Value->Value.AddStructReferencedObjects(Collector);
    }
    virtual FString GetReferencerName() const override { return TEXT("FUnrealDasContext snapshots"); }
    uint64 TraceScope = NextTraceScope++;
    FDasWidgetLifetime WidgetLifetime;
    FDasImageSoftBrushLifetime ImageBrushes;
    FDasDragLifetime DragLifetime;
    TArray<TStrongObjectPtr<USaveGame>> SaveGames;
    TMap<FPrimaryAssetId, TSharedPtr<FDasPrimaryAssetLease>> PrimaryAssets;
    TMap<uint64, TArray<FPrimaryAssetId>> PrimaryQueries;
    TArray<FDasTypeDeclaration> Types;
    DasSnapshotContainers::FKeyOrderCache SnapshotKeyOrders;
    DasSnapshotFields::FPathCache SnapshotFieldPaths;
    struct FStructEdit
    {
        FInstancedStruct Value;
        bool Poisoned = false;
    };
    TMap<uint64, TUniquePtr<FStructEdit>> StructEdits;
    TMap<uint64, TSharedPtr<FDasWidgetAnimationLease>> WidgetAnimations;
    TMap<uint64, FInstancedStruct> StructValues;
    uint64 StructCollections = 0, StructReclaimed = 0;
    int32 StructNextCollection = 256;
    TMap<uint64, FText> TextValues;
    struct FEnumValue
    {
        TWeakObjectPtr<UEnum> Type;
        int32 Value;
    };
    TMap<uint64, FEnumValue> EnumValues;
    TMap<uint64, FName> DSLNames;
    TMap<FName, uint64> DSLNameTokens;
    TMap<FString, int32> PersistentInts;
    int32 ExecutionDepth = 0;
    bool bEnding = false;
    bool bCollectingSubscriptions = false;
    bool bInitializingObject = false;
    bool bLibraryScope = false;
    TWeakObjectPtr<UObject> LibraryWorld;
    TArray<FSubscriptionDeclaration> Subscriptions;
    TWeakObjectPtr<UObject> Self;
    struct FObjectQuery
    {
        TWeakObjectPtr<UClass> Base;
        TArray<TWeakObjectPtr<UObject>> Objects;
    };
    TMap<uint64, FObjectQuery> ObjectQueries;
    TMap<uint64, FObjectHandleEntry> ObjectHandles;
    // (object, expected class) -> token, a lookup cache over ObjectHandles; MakeObjectHandle verifies hits.
    TMap<TPair<const UObject*, const UClass*>, uint64> ObjectHandleIndex;
    TMap<FObjectKey, uint64> ObjectKeyTokens;
    TMap<uint64, TWeakObjectPtr<UObject>> ObjectKeys;
    TMap<uint64, TSharedPtr<FMarshalledCall>> Calls;
    TMap<uint64, FDelegateValue> DelegateValues;
    TMap<FString, TStrongObjectPtr<UFunction>> DelegateSignatures;
};

struct FStructFrameRoots final : FGCObject
{
    TArray<FDasScalarValue>& Values;
    explicit FStructFrameRoots(TArray<FDasScalarValue>& InValues) : Values(InValues) {}
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override
    {
        for (auto& Value : Values)
            if (Value.Kind == 12)
                Value.StructValue.AddStructReferencedObjects(Collector);
    }
    virtual FString GetReferencerName() const override { return TEXT("daScript native input frame snapshots"); }
};
thread_local FUnrealDasContext* ActiveContext = nullptr;
thread_local das::Context* RestrictedPhysicsContext = nullptr;
// Every script entry opens its own FActiveContextScope with a fresh token. Chaos marks the game thread
// frozen only around whole PreSim callback dispatches (FPhysicsSolverFrozenGTPreSimCallbacks), so the
// frozen state cannot change while one scope runs for the same owner; the world scan is done once per
// (scope, owner) instead of on every extern call.
thread_local uint64 ActiveScopeToken = 0;
thread_local uint64 NextActiveScopeToken = 1;
void GuardUEExtern()
{
    if (RestrictedPhysicsContext)
        RestrictedPhysicsContext->throw_error("UE extern is forbidden in isolated physics VM");
    bool bFrozen = false;
    if (GetDasExecutionRole() != EDasExecutionRole::PhysicsPreSim)
    {
        struct FFrozenCache
        {
            uint64 Token = 0;
            const FUnrealDasContext* Context = nullptr;
            bool bLibrary = false;
            TWeakObjectPtr<UObject> Owner;
            bool bFrozen = false;
        };
        static thread_local FFrozenCache Cache;
        if (ActiveContext && ActiveScopeToken)
        {
            const TWeakObjectPtr<UObject>& Owner =
                ActiveContext->bLibraryScope ? ActiveContext->LibraryWorld : ActiveContext->Self;
            if (Cache.Token != ActiveScopeToken || Cache.Context != ActiveContext ||
                Cache.bLibrary != ActiveContext->bLibraryScope || Cache.Owner != Owner)
                Cache = {ActiveScopeToken, ActiveContext, ActiveContext->bLibraryScope, Owner,
                         IsDasFrozenPhysicsWorld(Owner.Get())};
            bFrozen = Cache.bFrozen;
        }
        else
            bFrozen = IsDasFrozenPhysicsWorld(nullptr);
    }
    if (GetDasExecutionRole() == EDasExecutionRole::PhysicsPreSim || bFrozen)
    {
        if (ActiveContext)
            ActiveContext->throw_error("UE extern is forbidden in PhysicsPreSim execution role");
        throw std::runtime_error("UE extern is forbidden in PhysicsPreSim execution role");
    }
}
thread_local const FString* LibraryInvocationContract = nullptr;
thread_local FString LibraryInvocationScript;
struct FOutputFrame : FGCObject
{
    const TArray<FDasScalarValue>* Arguments = nullptr;
    TMap<int32, FDasScalarValue> Values;
    TArray<TStrongObjectPtr<UObject>> ObjectPins;
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override
    {
        for (auto& V : Values)
            if (V.Value.Kind == 12)
                V.Value.StructValue.AddStructReferencedObjects(Collector);
    }
    virtual FString GetReferencerName() const override { return TEXT("daScript pending frame outputs"); }
};
thread_local FOutputFrame* ActiveOutputFrame = nullptr;


thread_local AActor* ActorLifecycleTarget = nullptr;
thread_local int32 ActorSpawnDepth = 0;
thread_local UDasScriptComponent* ComponentLifecycleTarget = nullptr;
thread_local int32 ComponentLifecycleDepth = 0;
thread_local int32 AuthoredDispatchDepth = 0;
thread_local FString* AuthoredDispatchFailure = nullptr;
struct FActiveContextScope
{
    FUnrealDasContext* Previous;
    TSet<uint64> ExistingCalls;
    uint64 PreviousToken = ActiveScopeToken;
    explicit FActiveContextScope(FUnrealDasContext& Context) : Previous(ActiveContext)
    {
        ActiveScopeToken = NextActiveScopeToken++;
        for (const auto& Call : Context.Calls)
            ExistingCalls.Add(Call.Key);
        for (const auto& Query : Context.ObjectQueries)
            ExistingQueries.Add(Query.Key);
        ActiveContext = &Context;
    }
    TSet<uint64> ExistingQueries;
    ~FActiveContextScope()
    {
        for (auto It = ActiveContext->ObjectQueries.CreateIterator(); It; ++It)
            if (!ExistingQueries.Contains(It.Key()))
                It.RemoveCurrent();
        for (auto It = ActiveContext->Calls.CreateIterator(); It; ++It)
            if (!ExistingCalls.Contains(It.Key()))
                It.RemoveCurrent();
        ActiveContext = Previous;
        ActiveScopeToken = PreviousToken;
    }
};

int32 GetStateInt(const char* Name, int32 Default, das::Context* Context)
{
    if (Context != ActiveContext || !Name || !*Name)
        Context->throw_error("state access requires an active context and a nonempty key");
    return ActiveContext->PersistentInts.FindOrAdd(UTF8_TO_TCHAR(Name), Default);
}
void SetStateInt(const char* Name, int32 Value, das::Context* Context)
{
    if (Context != ActiveContext || !Name || !*Name)
        Context->throw_error("state access requires an active context and a nonempty key");
    ActiveContext->PersistentInts.Add(UTF8_TO_TCHAR(Name), Value);
}

int32 NativeAdd(int32 A, int32 B)
{
    return A + B;
}

int32 EmitType(const char* Name, const char* Parent, const char* Fields, das::Context* Context)
{
    // Copies strings while their owning context is alive. No UE objects or
    // compiled function pointers escape this execution in the foundation.
    if (Context != ActiveContext)
        Context->throw_error("UE schema emission is only allowed in the active runtime context");
    if (!Name || !Parent || !Fields)
        Context->throw_error("UE schema declaration strings must not be null");
    auto& UEContext = *ActiveContext;
    UEContext.Types.Add({UTF8_TO_TCHAR(Name), UTF8_TO_TCHAR(Parent), UTF8_TO_TCHAR(Fields)});
    return UEContext.Types.Num();
}

UObject* ResolveSelf(das::Context* Context)
{
    if (!IsInGameThread() || Context != ActiveContext || !ActiveContext)
    {
        Context->throw_error("self access requires the active game-thread runtime context");
    }
    UObject* Self = ActiveContext->Self.Get();
    if (LibraryInvocationContract || ActiveContext->bLibraryScope || (Self && Self->IsA<UDasBlueprintLibrary>()))
        Context->throw_error("Static library has no implicit self or WorldContext");
    if (!IsValid(Self) ||
        (Self->IsA<AActor>() && CastChecked<AActor>(Self)->IsActorBeingDestroyed() && !ActiveContext->bEnding))
    {
        Context->throw_error("self is missing or expired");
    }
    return Self;
}

UObject* ResolveWorldContextObject(das::Context* C)
{
    if (C != ActiveContext || !IsInGameThread())
        C->throw_error("WorldContext requires active game-thread context");
    if (!ActiveContext->bLibraryScope)
    {
        if (LibraryInvocationContract)
            C->throw_error("Library WorldContext is only available inside a method");
        return ResolveSelf(C);
    }
    auto* Object = ActiveContext->LibraryWorld.Get();
    auto* World = IsValid(Object) && GEngine
                      ? GEngine->GetWorldFromContextObject(Object, EGetWorldErrorMode::ReturnNull)
                      : nullptr;
    if (!World || !World->IsGameWorld() || World->bIsTearingDown || Object->IsTemplate() ||
        (Cast<AActor>(Object) && CastChecked<AActor>(Object)->IsActorBeingDestroyed()) ||
        (Cast<UActorComponent>(Object) && CastChecked<UActorComponent>(Object)->IsBeingDestroyed()))
        C->throw_error("Library requires a live game-world WorldContext");
    return Object;
}

// Property lookup by (class, UTF-8 name). Entries hold a weak class and a field path,
// so a recompiled or reinstanced class is resolved again instead of reusing freed fields.
FProperty* FindSelfPropertyCached(UClass* Class, const char* Name, das::Context* Context)
{
    struct FEntry
    {
        TWeakObjectPtr<UClass> Class;
        das::string Name;
        TFieldPath<FProperty> Property;
    };
    static TMap<uint64, TArray<FEntry, TInlineAllocator<1>>> Cache;
    const SIZE_T Length = FCStringAnsi::Strlen(Name);
    const uint64 Key = CityHash64(Name, Length) ^ (uint64(UPTRINT(Class)) * 0x9E3779B97F4A7C15ull);
    auto Resolve = [&]() -> FProperty*
    {
        const FString FieldName(UTF8_TO_TCHAR(Name));
        if (FieldName.Len() >= NAME_SIZE)
            Context->throw_error("property name is too long");
        return Class->FindPropertyByName(FName(*FieldName));
    };
    auto& Bucket = Cache.FindOrAdd(Key);
    for (auto& Entry : Bucket)
        if (Entry.Class.Get() == Class && Entry.Name == Name)
        {
            if (FProperty* Cached = Entry.Property.Get(Class))
                return Cached;
            FProperty* Fresh = Resolve();
            Entry.Property = TFieldPath<FProperty>(Fresh);
            return Fresh;
        }
    FProperty* Found = Resolve();
    if (Found)
        Bucket.Add(FEntry{Class, Name, TFieldPath<FProperty>(Found)});
    return Found;
}

template <class P> P* ResolveProperty(UObject* Self, const char* Name, bool bWrite, das::Context* Context)
{
    if (!Name || !*Name)
    {
        Context->throw_error("property name is empty");
    }
    FProperty* Property = FindSelfPropertyCached(Self->GetClass(), Name, Context);
    if (!Property)
    {
        Context->throw_error("self property does not exist");
    }
    P* Typed = CastField<P>(Property);
    if (!Typed || Property->ArrayDim != 1)
    {
        Context->throw_error("self property has a different scalar type");
    }
    if (!Property->HasAnyPropertyFlags(CPF_BlueprintVisible) ||
        (bWrite && Property->HasAnyPropertyFlags(CPF_BlueprintReadOnly)))
    {
        Context->throw_error("self property access is not permitted (hidden or read-only)");
    }
    return Typed;
}

thread_local bool ConstructingObject = false;
void CheckConstructionCall(bool Unsafe, das::Context* C)
{
    if (Unsafe && ConstructingObject)
        C->throw_error("UFunction is unsafe during ConstructionScript");
}
#include "DasNativePropertyValues.inl"
