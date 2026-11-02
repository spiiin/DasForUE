// Called only from the active script owner's game-thread context.
TMap<TWeakObjectPtr<AActor>, TWeakObjectPtr<UObject>> CreatedScriptActors;
FDasObjectHandle ActorSpawnInstigated(FDasClassHandle H, const FTransform& Transform, const FDasObjectHandle& Owner,
                                      const FDasObjectHandle& Instigator, const char* Collision, const char* Base,
                                      const char* Id, das::Context* C)
{
    auto* Self = ResolveWorldContextObject(C);
    auto* World = Self->GetWorld();
    if (ConstructingObject || ActiveContext->bCollectingSubscriptions || ActiveContext->bEnding || !World ||
        !World->IsGameWorld() || Self->IsTemplate())
        C->throw_error("actor spawning requires script owner's active game world");
    auto* Class = CheckedClass(Cast<UClass>(ResolveObject(FDasObjectHandle{H.Token}, C)), Base, Id, C);
    if (!Class->IsChildOf(AActor::StaticClass()) || Class->HasAnyClassFlags(CLASS_Abstract))
        C->throw_error("actor spawn requires Actor class");
    if (!MethodMathValid(Transform))
        C->throw_error("invalid actor spawn transform");
    AActor* OwnerActor = Owner.Token ? Cast<AActor>(ResolveObject(Owner, C)) : nullptr;
    if (Owner.Token && (!OwnerActor || OwnerActor->IsTemplate() || OwnerActor->IsActorBeingDestroyed() ||
                        OwnerActor->GetWorld() != World))
        C->throw_error("spawn owner must be live Actor in script owner's world");
    APawn* InstigatorPawn = Instigator.Token ? Cast<APawn>(ResolveObject(Instigator, C)) : nullptr;
    if (Instigator.Token && (!InstigatorPawn || InstigatorPawn->IsTemplate() ||
                             InstigatorPawn->IsActorBeingDestroyed() || InstigatorPawn->GetWorld() != World))
        C->throw_error("spawn instigator must be live Pawn in script owner's world");
    const FString Rule = Collision ? UTF8_TO_TCHAR(Collision) : TEXT("");
    ESpawnActorCollisionHandlingMethod Handling = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    if (Rule == TEXT("AlwaysSpawn"))
    {
    }
    else if (Rule == TEXT("AdjustIfPossibleButAlwaysSpawn"))
        Handling = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    else if (Rule == TEXT("AdjustIfPossibleButDontSpawnIfColliding"))
        Handling = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
    else if (Rule == TEXT("DontSpawnIfColliding"))
        Handling = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
    else
        C->throw_error("unknown actor spawn collision rule");
    if (ActorSpawnDepth >= 16)
        C->throw_error("actor spawn nesting limit");
    for (auto I = CreatedScriptActors.CreateIterator(); I; ++I)
        if (!I.Key().IsValid())
            I.RemoveCurrent();
    if (CreatedScriptActors.Num() >= 1024)
        C->throw_error("created actor tracking limit");
    TStrongObjectPtr<UClass> KeepClass(Class);
    TStrongObjectPtr<AActor> KeepOwner(OwnerActor);
    TStrongObjectPtr<APawn> KeepInstigator(InstigatorPawn);
    FActorSpawnParameters Parameters;
    Parameters.Instigator = InstigatorPawn;
    Parameters.Owner = OwnerActor;
    Parameters.SpawnCollisionHandlingOverride = Handling;
    Parameters.bDeferConstruction = true;
    Parameters.ObjectFlags = RF_Transient;
    auto* Actor = World->SpawnActor<AActor>(Class, Transform, Parameters);
    if (!Actor)
        return {};
    TStrongObjectPtr<AActor> KeepActor(Actor);
    CreatedScriptActors.Add(TWeakObjectPtr<AActor>(Actor), TWeakObjectPtr<UObject>(Self));
    TGuardValue<AActor*> LifecycleTarget(ActorLifecycleTarget, Actor);
    TGuardValue<int32> Depth(ActorSpawnDepth, ActorSpawnDepth + 1);
    Actor->FinishSpawning(Transform);
    if (!IsValid(Actor) || Actor->IsActorBeingDestroyed())
    {
        CreatedScriptActors.Remove(TWeakObjectPtr<AActor>(Actor));
        return {};
    }
    return MakeObjectHandle(Actor, Class, C);
}
FDasObjectHandle ActorSpawn(FDasClassHandle H, const FTransform& Transform, const FDasObjectHandle& Owner,
                            const char* Collision, const char* Base, const char* Id, das::Context* C)
{
    return ActorSpawnInstigated(H, Transform, Owner, {}, Collision, Base, Id, C);
}
bool ActorDestroy(const FDasObjectHandle& H, das::Context* C)
{
    auto* Self = ResolveWorldContextObject(C);
    auto* Actor = Cast<AActor>(ResolveObject(H, C));
    if (!Actor || Actor->IsTemplate() || Actor->IsActorBeingDestroyed() || !Actor->GetWorld() ||
        !Actor->GetWorld()->IsGameWorld() || Actor->GetWorld() != Self->GetWorld() || ConstructingObject ||
        ActiveContext->bCollectingSubscriptions)
        C->throw_error("invalid actor destruction context");
    const auto* Creator = CreatedScriptActors.Find(TWeakObjectPtr<AActor>(Actor));
    if (!Creator || Creator->Get() != Self)
        C->throw_error("only actors spawned by this script owner may be destroyed");
    TGuardValue<AActor*> LifecycleTarget(ActorLifecycleTarget, Actor);
    const bool Destroyed = Actor->Destroy();
    if (Destroyed)
        CreatedScriptActors.Remove(TWeakObjectPtr<AActor>(Actor));
    return Destroyed;
}

// Native callbacks may enqueue managed notifications containing the dying actor.
// Deferring destruction to the next world tick lets those snapshots drain first.
bool ActorDestroySelfNextTick(das::Context* C)
{
    auto* Actor = Cast<AActor>(ResolveWorldContextObject(C));
    if (!Actor || Actor->IsTemplate() || Actor->IsActorBeingDestroyed() || !Actor->GetWorld() ||
        !Actor->GetWorld()->IsGameWorld() || Actor->GetWorld()->bIsTearingDown || ConstructingObject ||
        ActiveContext->bCollectingSubscriptions || ActiveContext->bEnding)
        C->throw_error("deferred self destruction requires a live Actor in an active game world");
    Actor->GetWorld()->GetTimerManager().SetTimerForNextTick(
        FTimerDelegate::CreateWeakLambda(Actor,
                                         [Actor]()
                                         {
                                             if (!IsValid(Actor) || Actor->IsActorBeingDestroyed() ||
                                                 !Actor->GetWorld() || Actor->GetWorld()->bIsTearingDown)
                                                 return;
                                             TGuardValue<AActor*> LifecycleTarget(ActorLifecycleTarget, Actor);
                                             if (Actor->Destroy())
                                                 CreatedScriptActors.Remove(TWeakObjectPtr<AActor>(Actor));
                                         }));
    return true;
}
bool ActorDestroySelf(das::Context* C)
{
    auto* Actor = Cast<AActor>(ResolveWorldContextObject(C));
    if (!Actor || Actor->IsTemplate() || Actor->IsActorBeingDestroyed() || !Actor->GetWorld() ||
        !Actor->GetWorld()->IsGameWorld() || Actor->GetWorld()->bIsTearingDown || ConstructingObject ||
        ActiveContext->bCollectingSubscriptions || ActiveContext->bEnding)
        C->throw_error("self destruction requires the script owner's live Actor in an active game world");
    TGuardValue<AActor*> LifecycleTarget(ActorLifecycleTarget, Actor);
    const bool Destroyed = Actor->Destroy();
    if (Destroyed)
        CreatedScriptActors.Remove(TWeakObjectPtr<AActor>(Actor));
    return Destroyed;
}

// Non-Actor owned objects are ordinary UE objects; reflected properties keep them alive.
FDasObjectHandle ScriptObjectNew(FDasClassHandle H, const FDasObjectHandle& OuterHandle, const char* Base,
                                 const char* Id, das::Context* C)
{
    auto* Self = ResolveWorldContextObject(C);
    auto* World = Self->GetWorld();
    auto* Outer = ResolveObject(OuterHandle, C);
    auto* Class = CheckedClass(Cast<UClass>(ResolveObject(FDasObjectHandle{H.Token}, C)), Base, "", C);
    if (!World || !World->IsGameWorld() || World->bIsTearingDown || Self->IsTemplate() || !IsValid(Outer) ||
        Outer->IsTemplate() || Outer->GetWorld() != World || ConstructingObject ||
        ActiveContext->bCollectingSubscriptions || ActiveContext->bEnding ||
        !Class->IsChildOf(UDasObject::StaticClass()) ||
        Class->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists))
        C->throw_error("object creation requires a concrete ScriptObject and live same-world Outer");
    if ((Cast<AActor>(Outer) && CastChecked<AActor>(Outer)->IsActorBeingDestroyed()) ||
        (Cast<UActorComponent>(Outer) && CastChecked<UActorComponent>(Outer)->IsBeingDestroyed()))
        C->throw_error("object Outer is being destroyed");
    // A base authored reference accepts derived classes, as normal object casts do.
    // Validate the CDO hierarchy before constructing anything; siblings still fail.
    if (Id && *Id)
        ResolveAuthoredObject(MakeObjectHandle(Class->GetDefaultObject(), UObject::StaticClass(), C), Id, C);
    TStrongObjectPtr<UObject> KeepOuter(Outer);
    TStrongObjectPtr<UClass> KeepClass(Class);
    TStrongObjectPtr<UObject> Created(NewObject<UObject>(Outer, Class, NAME_None, RF_Transient));
    return MakeObjectHandle(Created.Get(), Class, C);
}
FDasObjectHandle ScriptObjectOuter(const FDasObjectHandle& Handle, das::Context* C)
{
    auto* Object = ResolveObject(Handle, C);
    return MakeObjectHandle(Object->GetOuter(), UObject::StaticClass(), C);
}
double WorldSeconds(das::Context* C)
{
    auto* Self = ResolveWorldContextObject(C);
    auto* World = Self->GetWorld();
    if (!World || !World->IsGameWorld() || World->bIsTearingDown || Self->IsTemplate())
        C->throw_error("world time requires a live game-world context");
    return World->GetTimeSeconds();
}
