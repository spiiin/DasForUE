// Called only from the active script owner's game-thread context.
TMap<TWeakObjectPtr<AActor>, TWeakObjectPtr<UObject>> CreatedScriptActors;
FDasObjectHandle ActorSpawn(FDasClassHandle H, const FTransform& Transform, const FDasObjectHandle& Owner,
                            const char* Collision, const char* Base, const char* Id, das::Context* C)
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
    FActorSpawnParameters Parameters;
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
