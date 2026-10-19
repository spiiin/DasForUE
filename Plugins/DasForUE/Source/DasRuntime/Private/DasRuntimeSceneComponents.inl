// Part of DasRuntime.cpp: Scene component adapters, component creation/destruction and self class access.
// Included in place; declaration order matters.
TMap<TWeakObjectPtr<UActorComponent>, TWeakObjectPtr<UObject>> CreatedScriptScenes;
USceneComponent* ScriptScene(const FDasObjectHandle& H, das::Context* C)
{
    auto* S = Cast<USceneComponent>(ResolveObject(H, C));
    if (!S || S->IsBeingDestroyed())
        C->throw_error("expected live SceneComponent");
    if (auto* A = S->GetOwner(); A && A->IsActorBeingDestroyed() && !ActiveContext->bEnding)
        C->throw_error("SceneComponent owner is being destroyed");
    return S;
}
void SceneMutation(USceneComponent* S, das::Context* C)
{
    auto* Self = ResolveWorldContextObject(C);
    if (ActiveContext->bCollectingSubscriptions)
        C->throw_error("scene mutations are forbidden in bind_events");
    if (!S->GetWorld() || S->GetWorld() != Self->GetWorld() || S->IsTemplate())
        C->throw_error("scene mutation requires component in script owner's world");
}
bool SceneValid(const FDasObjectHandle& H, das::Context* C)
{
    auto* S = Cast<USceneComponent>(TryObject(H, C));
    return S && !S->IsBeingDestroyed() && (!S->GetOwner() || !S->GetOwner()->IsActorBeingDestroyed());
}
FDasObjectHandle ActorOwner(const FDasObjectHandle& H, das::Context* C)
{
    auto* A = Cast<AActor>(ResolveObject(H, C));
    if (!A)
        C->throw_error("expected Actor for owner");
    return MakeObjectHandle(A->GetOwner(), AActor::StaticClass(), C);
}
FDasObjectHandle SceneActorRoot(const FDasObjectHandle& H, das::Context* C)
{
    auto* A = Cast<AActor>(ResolveObject(H, C));
    if (!A)
        C->throw_error("expected Actor for root component");
    return MakeObjectHandle(A->GetRootComponent(), USceneComponent::StaticClass(), C);
}
FDasObjectHandle SceneParent(const FDasObjectHandle& H, das::Context* C)
{
    return MakeObjectHandle(ScriptScene(H, C)->GetAttachParent(), USceneComponent::StaticClass(), C);
}
FTransform SceneRelative(const FDasObjectHandle& H, das::Context* C)
{
    return ScriptScene(H, C)->GetRelativeTransform();
}
FTransform SceneWorld(const FDasObjectHandle& H, das::Context* C)
{
    return ScriptScene(H, C)->GetComponentTransform();
}
FVector SceneRelativeTranslation(const FDasObjectHandle& H, das::Context* C)
{
    return ScriptScene(H, C)->GetRelativeLocation();
}
FRotator SceneRelativeRotation(const FDasObjectHandle& H, das::Context* C)
{
    return ScriptScene(H, C)->GetRelativeRotation();
}
FVector SceneRelativeScale(const FDasObjectHandle& H, das::Context* C)
{
    return ScriptScene(H, C)->GetRelativeScale3D();
}
void SceneSetRelative(const FDasObjectHandle& H, const FTransform& V, bool Teleport, das::Context* C)
{
    auto* S = ScriptScene(H, C);
    SceneMutation(S, C);
    if (!MethodMathValid(V))
        C->throw_error("invalid scene transform");
    S->SetRelativeTransform(V, false, nullptr, Teleport ? ETeleportType::TeleportPhysics : ETeleportType::None);
}
void SceneSetWorld(const FDasObjectHandle& H, const FTransform& V, bool Teleport, das::Context* C)
{
    auto* S = ScriptScene(H, C);
    SceneMutation(S, C);
    if (!MethodMathValid(V))
        C->throw_error("invalid scene transform");
    S->SetWorldTransform(V, false, nullptr, Teleport ? ETeleportType::TeleportPhysics : ETeleportType::None);
}
bool SceneAttach(const FDasObjectHandle& H, const FDasObjectHandle& Parent, const char* Socket, const char* Rule,
                 bool Weld, das::Context* C)
{
    auto* Child = ScriptScene(H, C);
    auto* P = ScriptScene(Parent, C);
    SceneMutation(Child, C);
    SceneMutation(P, C);
    if (Child == P || P->IsAttachedTo(Child))
        C->throw_error("scene attachment would form a cycle");
    const FString SocketText = Socket ? UTF8_TO_TCHAR(Socket) : TEXT("");
    if (SocketText.Len() >= NAME_SIZE)
        C->throw_error("socket name too long");
    const FName SocketName = SocketText.IsEmpty() ? NAME_None : FName(*SocketText);
    if (SocketName != NAME_None && !P->DoesSocketExist(SocketName))
        C->throw_error("unknown parent socket");
    const FString R = Rule ? UTF8_TO_TCHAR(Rule) : TEXT("");
    FAttachmentTransformRules Rules = FAttachmentTransformRules::KeepRelativeTransform;
    if (R == TEXT("KeepRelative"))
    {
    }
    else if (R == TEXT("KeepWorld"))
        Rules = FAttachmentTransformRules::KeepWorldTransform;
    else if (R == TEXT("SnapIncludingScale"))
        Rules = FAttachmentTransformRules::SnapToTargetIncludingScale;
    else if (R == TEXT("SnapNotIncludingScale"))
        Rules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
    else
        C->throw_error("unknown scene attachment rule");
    Rules.bWeldSimulatedBodies = Weld;
    return Child->AttachToComponent(P, Rules, SocketName);
}
void SceneDetach(const FDasObjectHandle& H, const char* Rule, das::Context* C)
{
    auto* S = ScriptScene(H, C);
    SceneMutation(S, C);
    const FString R = Rule ? UTF8_TO_TCHAR(Rule) : TEXT("");
    if (R == TEXT("KeepWorld"))
        S->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
    else if (R == TEXT("KeepRelative"))
        S->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepRelative, false));
    else
        C->throw_error("detach rule must be KeepWorld or KeepRelative");
}
FDasObjectHandle SceneCreate(const FDasObjectHandle& H, const char* Name, das::Context* C)
{
    auto* Self = ResolveWorldContextObject(C);
    if (ConstructingObject || ActiveContext->bCollectingSubscriptions || ActiveContext->bEnding)
        C->throw_error("cannot create scene components during bind_events/EndPlay");
    auto* Actor = Cast<AActor>(ResolveObject(H, C));
    if (!Actor || Actor->IsTemplate() || !Actor->GetWorld() ||
        (!Actor->GetWorld()->IsGameWorld() && !ConstructingObject) || Actor->GetWorld() != Self->GetWorld())
        C->throw_error("scene creation requires live game-world Actor in owner's world");
    const FString N = Name ? UTF8_TO_TCHAR(Name) : TEXT("");
    if (N.IsEmpty() || N.Len() >= NAME_SIZE || N.Equals(TEXT("None"), ESearchCase::IgnoreCase))
        C->throw_error("scene component name is required");
    for (TCHAR Ch : N)
        if (!FChar::IsAlnum(Ch) && Ch != '_')
            C->throw_error("scene component name supports letters/digits/underscore");
    if (FindObject<UObject>(Actor, *N))
        C->throw_error("scene component name already exists");
    for (auto I = CreatedScriptScenes.CreateIterator(); I; ++I)
        if (!I.Key().IsValid())
            I.RemoveCurrent();
    if (CreatedScriptScenes.Num() >= 1024)
        C->throw_error("created scene component tracking limit");
    TStrongObjectPtr<AActor> Keep(Actor);
    auto* Component = NewObject<USceneComponent>(Actor, *N, RF_Transient);
    const bool Root = Actor->GetRootComponent() == nullptr;
    Actor->AddInstanceComponent(Component);
    if (Root)
        Actor->SetRootComponent(Component);
    else
        Component->SetupAttachment(Actor->GetRootComponent());
    Component->RegisterComponent();
    if (!Component->IsRegistered())
    {
        if (Root)
            Actor->SetRootComponent(nullptr);
        Actor->RemoveInstanceComponent(Component);
        Component->DestroyComponent();
        C->throw_error("scene component registration failed");
    }
    CreatedScriptScenes.Add(TWeakObjectPtr<UActorComponent>(Component), TWeakObjectPtr<UObject>(Self));
    return MakeObjectHandle(Component, USceneComponent::StaticClass(), C);
}
FDasObjectHandle SceneGet(const FDasObjectHandle& H, const char* Name, das::Context* C)
{
    auto* Self = ResolveWorldContextObject(C);
    auto* Actor = Cast<AActor>(ResolveObject(H, C));
    if (!Actor || Actor->IsActorBeingDestroyed() || Actor->IsTemplate() || !Actor->GetWorld() ||
        (!Actor->GetWorld()->IsGameWorld() && !ConstructingObject) || Actor->GetWorld() != Self->GetWorld())
        C->throw_error("scene lookup requires live game-world Actor in owner's world");
    const FString N = Name ? UTF8_TO_TCHAR(Name) : TEXT("");
    if (N.IsEmpty() || N.Len() >= NAME_SIZE || N.Equals(TEXT("None"), ESearchCase::IgnoreCase))
        C->throw_error("scene component name is required");
    for (TCHAR Ch : N)
        if (!FChar::IsAlnum(Ch) && Ch != '_')
            C->throw_error("scene component name supports letters/digits/underscore");
    USceneComponent* Found = nullptr;
    for (auto* Component : Actor->GetComponents())
        if (Component && Component->GetFName() == FName(*N))
        {
            Found = Cast<USceneComponent>(Component);
            if (!Found || Found->IsBeingDestroyed())
                C->throw_error("named component is not a live SceneComponent");
            break;
        }
    return MakeObjectHandle(Found, USceneComponent::StaticClass(), C);
}
FDasObjectHandle SceneGetOrCreate(const FDasObjectHandle& H, const char* Name, das::Context* C)
{
    auto Found = SceneGet(H, Name, C);
    if (TryObject(Found, C))
        return Found;
    return SceneCreate(H, Name, C);
}
FDasObjectHandle ComponentOperation(const FDasObjectHandle& H, UClass* Class, const char* Name, int Mode,
                                    const char* AuthoredId, das::Context* C)
{
    auto* Self = ResolveWorldContextObject(C);
    auto* Actor = Cast<AActor>(ResolveObject(H, C));
    if (!Actor || Actor->IsTemplate() || Actor->IsActorBeingDestroyed() || !Actor->GetWorld() ||
        (!Actor->GetWorld()->IsGameWorld() && !ConstructingObject) || Actor->GetWorld() != Self->GetWorld())
        C->throw_error("component operation requires live Actor in owner's game world");
    const bool Authored = AuthoredId && *AuthoredId;
    if (!Class || !Class->IsChildOf(UActorComponent::StaticClass()) ||
        Class->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists))
        C->throw_error("expected loaded concrete ActorComponent class");
    if (Authored)
    {
        const auto* Defaults = Cast<UDasScriptComponent>(Class->GetDefaultObject());
        if (!Defaults || Defaults->AuthoredTypeId != UTF8_TO_TCHAR(AuthoredId))
            C->throw_error("authored component class identity mismatch");
    }
    const FString N = Name ? UTF8_TO_TCHAR(Name) : TEXT("");
    if (N.IsEmpty() || N.Len() >= NAME_SIZE || N.Equals(TEXT("None"), ESearchCase::IgnoreCase))
        C->throw_error("component name is required");
    for (TCHAR Ch : N)
        if (!FChar::IsAlnum(Ch) && Ch != '_')
            C->throw_error("component name supports letters/digits/underscore");
    if (Mode < 0 || Mode > 2)
        C->throw_error("invalid component operation");
    if (Mode != 1)
        for (auto* Component : Actor->GetComponents())
            if (Component && Component->GetFName() == FName(*N))
            {
                if (Component->IsBeingDestroyed() || !Component->IsA(Class))
                    C->throw_error("named component has incompatible type or is being destroyed");
                return MakeObjectHandle(Component, Class, C);
            }
    if (Mode == 0)
        return MakeObjectHandle(nullptr, Class, C);
    if (ConstructingObject || ActiveContext->bCollectingSubscriptions || ActiveContext->bEnding)
        C->throw_error("cannot create components during bind_events/EndPlay");
    if (FindObject<UObject>(Actor, *N))
        C->throw_error("component name already exists");
    for (auto I = CreatedScriptScenes.CreateIterator(); I; ++I)
        if (!I.Key().IsValid())
            I.RemoveCurrent();
    if (CreatedScriptScenes.Num() >= 1024)
        C->throw_error("created component tracking limit");
    if (ComponentLifecycleDepth >= 16)
        C->throw_error("component lifecycle nesting limit");
    TStrongObjectPtr<AActor> Keep(Actor);
    auto* Component = NewObject<UActorComponent>(Actor, Class, *N, RF_Transient);
    auto* Scene = Cast<USceneComponent>(Component);
    const bool Root = Scene && !Actor->GetRootComponent();
    Actor->AddInstanceComponent(Component);
    if (Scene)
    {
        if (Root)
            Actor->SetRootComponent(Scene);
        else
            Scene->SetupAttachment(Actor->GetRootComponent());
    }
    TGuardValue<UDasScriptComponent*> LifecycleTarget(ComponentLifecycleTarget, Cast<UDasScriptComponent>(Component));
    TGuardValue<int32> LifecycleDepth(ComponentLifecycleDepth, ComponentLifecycleDepth + 1);
    Component->RegisterComponent();
    if (!IsValid(Component) || Component->IsBeingDestroyed() || !Component->IsRegistered())
    {
        if (Root)
            Actor->SetRootComponent(nullptr);
        Actor->RemoveInstanceComponent(Component);
        Component->DestroyComponent();
        C->throw_error("component registration failed");
    }
    CreatedScriptScenes.Add(TWeakObjectPtr<UActorComponent>(Component), TWeakObjectPtr<UObject>(Self));
    return MakeObjectHandle(Component, Class, C);
}
UClass* CheckedClass(UClass* Class, const char* Base, const char* Id, das::Context* C)
{
    const FString B = Base ? UTF8_TO_TCHAR(Base) : TEXT("");
    auto* Parent = FindObject<UClass>(nullptr, *B);
    if (!Parent || !IsValid(Class) || !Class->IsChildOf(Parent) ||
        Class->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists))
        C->throw_error("class reference is missing, abstract, obsolete or has incompatible base");
    if (Id && *Id)
    {
        const auto* Defaults = Class->GetDefaultObject();
        const FString Actual = DasHostIdentity(Defaults);
        if (Actual != UTF8_TO_TCHAR(Id))
            C->throw_error("class reference authored identity mismatch");
    }
    return Class;
}
FDasClassHandle ClassLoaded(const char* Path, const char* Base, const char* Id, das::Context* C)
{
    RequireObjectContext(C);
    const FString P = Path ? UTF8_TO_TCHAR(Path) : TEXT("");
    if (P.IsEmpty() || P.Len() > 4096)
        C->throw_error("class path is required");
    auto* Class = CheckedClass(FindObject<UClass>(nullptr, *P), Base, Id, C);
    return {MakeObjectHandle(Class, UClass::StaticClass(), C).Token};
}
FDasClassHandle CheckTypedClass(FDasClassHandle H, const char* Base, das::Context* C)
{
    RequireObjectContext(C);
    if (H.Token)
        CheckedClass(Cast<UClass>(ResolveObject(FDasObjectHandle{H.Token}, C)), Base, "", C);
    return H;
}
bool ClassValid(FDasClassHandle H, das::Context* C)
{
    auto* Class = Cast<UClass>(TryObject(FDasObjectHandle{H.Token}, C));
    return Class && !Class->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists);
}
char* ClassPath(FDasClassHandle H, das::Context* C)
{
    auto* Class = Cast<UClass>(ResolveObject(FDasObjectHandle{H.Token}, C));
    if (!Class)
        C->throw_error("expected class reference");
    return CopyDSLString(Class->GetPathName(), C);
}
FDasClassHandle GetSelfClass(const char* Name, das::Context* C)
{
    auto* Self = ResolveSelf(C);
    auto* P = ResolveProperty<FClassProperty>(Self, Name, false, C);
    auto* Class = ReadPropertyClass(P, Self, C);
    if (!Class)
        return {};
    CheckedClass(Class, TCHAR_TO_UTF8(*P->MetaClass->GetPathName()), "", C);
    return {MakeObjectHandle(Class, UClass::StaticClass(), C).Token};
}
void SetSelfClass(const char* Name, FDasClassHandle Value, das::Context* C)
{
    auto* Self = ResolveSelf(C);
    if (ActiveContext->bCollectingSubscriptions)
        C->throw_error("class writes are forbidden in bind_events");
    auto* P = ResolveProperty<FClassProperty>(Self, Name, true, C);
    UClass* Class = nullptr;
    if (Value.Token)
        Class = CheckedClass(Cast<UClass>(ResolveObject(FDasObjectHandle{Value.Token}, C)),
                             TCHAR_TO_UTF8(*P->MetaClass->GetPathName()), "", C);
    WritePropertyClass(P, Self, Class, C);
}
FDasClassHandle AuthoredGetclass(const FDasObjectHandle& H, const char* Id, const char* Name, das::Context* C)
{
    auto* Object = ResolveAuthoredObject(H, Id, C);
    auto* P = ResolveProperty<FClassProperty>(Object, Name, false, C);
    auto* Class = ReadPropertyClass(P, Object, C);
    if (!Class)
        return {};
    CheckedClass(Class, TCHAR_TO_UTF8(*P->MetaClass->GetPathName()), "", C);
    return {MakeObjectHandle(Class, UClass::StaticClass(), C).Token};
}
void AuthoredSetclass(const FDasObjectHandle& H, const char* Id, const char* Name, FDasClassHandle V, das::Context* C)
{
    auto* Object = ResolveAuthoredObject(H, Id, C);
    if (ActiveContext->bCollectingSubscriptions)
        C->throw_error("class writes are forbidden in bind_events");
    auto* P = ResolveProperty<FClassProperty>(Object, Name, true, C);
    UClass* Class = nullptr;
    if (V.Token)
        Class = CheckedClass(Cast<UClass>(ResolveObject(FDasObjectHandle{V.Token}, C)),
                             TCHAR_TO_UTF8(*P->MetaClass->GetPathName()), "", C);
    WritePropertyClass(P, Object, Class, C);
}
FDasObjectHandle NativeComponentOperation(const FDasObjectHandle& H, const char* Path, const char* Name, int Mode,
                                          const char* Id, das::Context* C)
{
    const FString P = Path ? UTF8_TO_TCHAR(Path) : TEXT("");
    if ((!Id || !*Id) && !P.StartsWith(TEXT("/Script/")))
        C->throw_error("expected native component class");
    return ComponentOperation(H, FindObject<UClass>(nullptr, *P), Name, Mode, Id, C);
}
FDasObjectHandle ClassComponentOperation(const FDasObjectHandle& H, FDasClassHandle Class, const char* Name, int Mode,
                                         const char* Base, const char* Id, das::Context* C)
{
    auto* Actual = CheckedClass(Cast<UClass>(ResolveObject(FDasObjectHandle{Class.Token}, C)), Base, Id, C);
    TStrongObjectPtr<UClass> Keep(Actual);
    return ComponentOperation(H, Actual, Name, Mode, Id, C);
}
FDasObjectHandle PathComponentOperation(const FDasObjectHandle& H, const char* Path, const char* Name, int Mode,
                                        const char* Base, const char* Id, das::Context* C)
{
    const FString P = Path ? UTF8_TO_TCHAR(Path) : TEXT("");
    auto* Class = CheckedClass(FindObject<UClass>(nullptr, *P), Base, Id, C);
    TStrongObjectPtr<UClass> Keep(Class);
    return ComponentOperation(H, Class, Name, Mode, Id, C);
}
#include "DasActorSpawn.inl"
#include "DasObjectQueries.inl"
#include "DasCharacterControl.inl"
#include "DasNativeGameplayRuntime.inl"
FDasObjectHandle AuthoredComponentGet(const FDasObjectHandle& H, const char* Id, const char* Name, das::Context* C)
{
    auto* Self = ResolveWorldContextObject(C);
    auto* Actor = Cast<AActor>(ResolveObject(H, C));
    if (!Actor || Actor->IsTemplate() || Actor->IsActorBeingDestroyed() || !Actor->GetWorld() ||
        (!Actor->GetWorld()->IsGameWorld() && !ConstructingObject) || Actor->GetWorld() != Self->GetWorld())
        C->throw_error("component lookup requires live Actor in owner's game world");
    const FString N = Name ? UTF8_TO_TCHAR(Name) : TEXT("");
    if (N.IsEmpty() || N.Len() >= NAME_SIZE || N.Equals(TEXT("None"), ESearchCase::IgnoreCase))
        C->throw_error("component name is required");
    for (TCHAR Ch : N)
        if (!FChar::IsAlnum(Ch) && Ch != '_')
            C->throw_error("component name supports letters/digits/underscore");
    for (auto* Component : Actor->GetComponents())
        if (Component && Component->GetFName() == FName(*N))
        {
            if (Component->IsBeingDestroyed() || !Component->IsA<UDasScriptComponent>())
                C->throw_error("named component is not a live authored component");
            auto Result = MakeObjectHandle(Component, UDasScriptComponent::StaticClass(), C);
            ResolveAuthoredObject(Result, Id, C);
            return Result;
        }
    return {};
}
bool NativeComponentDestroy(const FDasObjectHandle& H, das::Context* C)
{
    auto* Component = Cast<UActorComponent>(ResolveObject(H, C));
    auto* Self = ResolveWorldContextObject(C);
    if (!Component || Component->IsBeingDestroyed() || Component->IsTemplate() || !Component->GetWorld() ||
        Component->GetWorld() != Self->GetWorld() || ConstructingObject || ActiveContext->bCollectingSubscriptions)
        C->throw_error("invalid component destruction context");
    const auto* Creator = CreatedScriptScenes.Find(TWeakObjectPtr<UActorComponent>(Component));
    if (!Creator || Creator->Get() != Self)
        C->throw_error("only components created by this script owner may be destroyed");
    auto* Scene = Cast<USceneComponent>(Component);
    if (Scene && Scene->GetAttachChildren().Num())
        C->throw_error("detach children before destroying component");
    auto* Actor = Component->GetOwner();
    CreatedScriptScenes.Remove(TWeakObjectPtr<UActorComponent>(Component));
    if (Actor)
    {
        if (Scene && Actor->GetRootComponent() == Scene)
            Actor->SetRootComponent(nullptr);
        Actor->RemoveInstanceComponent(Component);
    }
    TGuardValue<UDasScriptComponent*> LifecycleTarget(ComponentLifecycleTarget, Cast<UDasScriptComponent>(Component));
    Component->DestroyComponent();
    return true;
}
bool SceneDestroy(const FDasObjectHandle& H, das::Context* C)
{
    if (ConstructingObject)
        C->throw_error("component destruction is forbidden during ConstructionScript");
    auto* S = ScriptScene(H, C);
    SceneMutation(S, C);
    const auto* Creator = CreatedScriptScenes.Find(TWeakObjectPtr<UActorComponent>(S));
    if (!Creator || Creator->Get() != ResolveWorldContextObject(C))
        C->throw_error("only scene components created by this script owner may be destroyed");
    if (S->GetAttachChildren().Num() != 0)
        C->throw_error("detach children before destroying created scene component");
    auto* Actor = S->GetOwner();
    CreatedScriptScenes.Remove(TWeakObjectPtr<UActorComponent>(S));
    if (Actor)
    {
        if (Actor->GetRootComponent() == S)
            Actor->SetRootComponent(nullptr);
        Actor->RemoveInstanceComponent(S);
    }
    S->DestroyComponent();
    return true;
}

FMulticastDelegateProperty* ScalarEventProperty(UObject* Source, FName Name, EDasMethodSignature Kind)
{
    auto* P = FindFProperty<FMulticastDelegateProperty>(Source->GetClass(), Name);
    if (!P || !P->HasAnyPropertyFlags(CPF_BlueprintAssignable) || P->ArrayDim != 1 || !P->SignatureFunction)
        return nullptr;
    auto* Sig = P->SignatureFunction.Get();
    if (Kind == EDasMethodSignature::Void)
        return Sig->NumParms == 0 ? P : nullptr;
    if (Sig->NumParms >= 2 && Sig->NumParms <= 16 && !Sig->GetReturnProperty())
        return Kind == EDasMethodSignature::Struct ? P : nullptr;
    if (Sig->NumParms != 1)
        return nullptr;
    auto* Arg = ScalarEventParameter(P);
    const auto* Type = DasCallbackTypeForProperty(Arg);
    if (!Type || Type->Signature != Kind || Arg->HasAnyPropertyFlags(CPF_ReturnParm))
        return nullptr;
    const bool Ref = Arg->HasAnyPropertyFlags(CPF_OutParm | CPF_ReferenceParm);
    if (Type->bDynamic)
        return !Ref || Arg->HasAnyPropertyFlags(CPF_ConstParm) ? P : nullptr;
    return Ref ? (Arg->HasAnyPropertyFlags(CPF_ConstParm) && !Type->ConstRefProxy.IsNone() ? P : nullptr)
               : (!Type->ValueProxy.IsNone() ? P : nullptr);
}

void SubscribeObjectScalar(const FDasObjectHandle& Handle, const char* Event, const char* Handler,
                           das::Context* Context, EDasMethodSignature Kind, UScriptStruct* ExpectedCarrier = nullptr)
{
    RequireObjectContext(Context);
    if (!ActiveContext->bCollectingSubscriptions)
        Context->throw_error("subscriptions may only be declared in bind_events");
    if (!Event || !*Event || !Handler || !*Handler)
        Context->throw_error("event and handler are required");
    UObject* Source = ResolveObject(Handle, Context);
    if (!IsValid(Source) || (Source->IsA<AActor>() && CastChecked<AActor>(Source)->IsActorBeingDestroyed()))
        Context->throw_error("subscription source is missing or expired");
    const FString EventName(UTF8_TO_TCHAR(Event));
    if (EventName.Len() >= NAME_SIZE)
        Context->throw_error("event name is too long");
    const FName Name(*EventName);
    auto* EventProperty = ScalarEventProperty(Source, Name, Kind);
    FString PayloadPath;
    if (ExpectedCarrier)
    {
        auto* Actual = EventProperty
                           ? (CastField<FStructProperty>(ScalarEventParameter(EventProperty))
                                  ? CastFieldChecked<FStructProperty>(ScalarEventParameter(EventProperty))->Struct.Get()
                                  : DasContainerCarrierForProperty(ScalarEventParameter(EventProperty)))
                           : nullptr;
        if (!Actual || Actual != ExpectedCarrier)
            Context->throw_error("event array element type mismatch");
        PayloadPath = ExpectedCarrier->GetPathName();
    }
    if (!EventProperty)
        Context->throw_error(Kind == EDasMethodSignature::Int
                                 ? "expected BlueprintAssignable multicast event with one int32 value"
                                 : "BlueprintAssignable event signature differs from callback type");
    if (ExpectedCarrier)
    {
        const auto* Capability = DasCallbackTypeForProperty(ScalarEventParameter(EventProperty));
        if (Capability && Capability->bDynamic)
        {
            FString Error;
            TStrongObjectPtr<UDasIntDelegateProxy> Check(NewObject<UDasIntDelegateProxy>());
            if (Check->ConfigureDynamic(EventProperty->SignatureFunction, PayloadPath, Kind, Error).IsNone())
                Context->throw_error(TCHAR_TO_UTF8(*Error));
        }
    }
    for (const auto& Existing : ActiveContext->Subscriptions)
        if (Existing.Source.Get() == Source && Existing.Event == Name && Existing.Handler == UTF8_TO_TCHAR(Handler))
            Context->throw_error("duplicate subscription declaration");
    if (ActiveContext->Subscriptions.Num() >= 256)
        Context->throw_error("subscription declaration limit exceeded");
    ActiveContext->Subscriptions.Add({Source, Name, UTF8_TO_TCHAR(Handler), Kind, PayloadPath});
}
