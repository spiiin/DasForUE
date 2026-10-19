uint64 NextObjectQuery = 1;
FUnrealDasContext::FObjectQuery CollectObjectsByType(const FDasObjectHandle& H, const char* Base, const char* Id,
                                                     bool Components, bool First, das::Context* C)
{
    auto* Self = ResolveWorldContextObject(C);
    auto* World = Self->GetWorld();
    if (!World || !World->IsGameWorld() || Self->IsTemplate())
        C->throw_error("object query requires script owner's game world");
    const FString Path = Base ? UTF8_TO_TCHAR(Base) : TEXT("");
    auto* Class = FindObject<UClass>(nullptr, *Path);
    if (!Path.StartsWith(TEXT("/Script/")) || !Class ||
                         Class->HasAnyClassFlags(CLASS_Interface | CLASS_Deprecated | CLASS_NewerVersionExists) ||
                         !Class->IsChildOf(Components ? UActorComponent::StaticClass() : AActor::StaticClass()))
        C->throw_error("query type does not match Actor/component domain");
    const FString Wanted = Id ? UTF8_TO_TCHAR(Id) : TEXT("");
    FUnrealDasContext::FObjectQuery Result;
    Result.Base = Class;
    auto Add = [&](UObject* Object)
    {
        if (!IsValid(Object) || Object->IsTemplate() || Object->GetWorld() != World || !Object->IsA(Class) ||
            Object->GetClass()->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists))
            return false;
        if (auto* Actor = Cast<AActor>(Object); Actor && Actor->IsActorBeingDestroyed())
            return false;
        if (auto* Component = Cast<UActorComponent>(Object);
            Component &&
            (Component->IsBeingDestroyed() || !Component->GetOwner() || Component->GetOwner()->IsActorBeingDestroyed()))
            return false;
        if (!Wanted.IsEmpty())
        {
            const auto* Defaults = Object->GetClass()->GetDefaultObject();
            const FString Actual = DasHostIdentity(Defaults);
            if (Actual != Wanted)
                return false;
        }
        if (Result.Objects.Num() >= 4096)
            C->throw_error("object query result exceeds 4096 entries");
        Result.Objects.Add(Object);
        return true;
    };
    if (Components)
    {
        auto* Actor = Cast<AActor>(ResolveObject(H, C));
        if (!Actor || Actor->IsTemplate() || Actor->IsActorBeingDestroyed() || Actor->GetWorld() != World)
            C->throw_error("component query owner must be live Actor in script owner's world");
        for (auto* Component : Actor->GetComponents())
            if (Add(Component) && First)
                break;
    }
    else
        for (TActorIterator<AActor> It(World, Class); It; ++It)
            if (Add(*It) && First)
                break;
    return Result;
}
FDasObjectHandle ObjectFindByType(const FDasObjectHandle& H, const char* Base, const char* Id, bool Components,
                                  das::Context* C)
{
    const auto Query = CollectObjectsByType(H, Base, Id, Components, true, C);
    return MakeObjectHandle(Query.Objects.IsEmpty() ? nullptr : Query.Objects[0].Get(), Query.Base.Get(), C);
}
uint64 ObjectQueryBegin(const FDasObjectHandle& H, const char* Base, const char* Id, bool Components, das::Context* C)
{
    RequireObjectContext(C);
    if (ActiveContext->ObjectQueries.Num() >= 64)
        C->throw_error("object query snapshot limit");
    auto Query = CollectObjectsByType(H, Base, Id, Components, false, C);
    const uint64 Token = NextObjectQuery++;
    ActiveContext->ObjectQueries.Add(Token, MoveTemp(Query));
    return Token;
}
int32 ObjectQueryCount(uint64 Token, das::Context* C)
{
    RequireObjectContext(C);
    const auto* Query = ActiveContext->ObjectQueries.Find(Token);
    if (!Query)
        C->throw_error("invalid query snapshot");
    return Query->Objects.Num();
}
FDasObjectHandle ObjectQueryGet(uint64 Token, int32 Index, das::Context* C)
{
    RequireObjectContext(C);
    const auto* Query = ActiveContext->ObjectQueries.Find(Token);
    if (!Query || !Query->Objects.IsValidIndex(Index) || !Query->Base.IsValid())
        C->throw_error("invalid query snapshot/index");
    return MakeObjectHandle(Query->Objects[Index].Get(), Query->Base.Get(), C);
}
void ObjectQueryRelease(uint64 Token, das::Context* C)
{
    RequireObjectContext(C);
    ActiveContext->ObjectQueries.Remove(Token);
}
