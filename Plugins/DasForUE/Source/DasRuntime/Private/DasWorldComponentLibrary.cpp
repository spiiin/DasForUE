#include "DasWorldComponentLibrary.h"
#include "Components/ActorComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

UActorComponent* UDasWorldComponentLibrary::CreateNativeWorldComponent(
    UObject* WorldContextObject, const FString& NativeClassPath, FName ComponentName)
{
    UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull) : nullptr;
    UClass* Class = NativeClassPath.StartsWith(TEXT("/Script/")) ? FindObject<UClass>(nullptr, *NativeClassPath) : nullptr;
    if (!IsValid(World) || World->bIsTearingDown || !Class || !Class->IsChildOf(UActorComponent::StaticClass()) ||
        Class->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists | CLASS_CompiledFromBlueprint) ||
        (Class->ClassWithin && !World->IsA(Class->ClassWithin)))
        return nullptr;
    // The script owner must retain the result in a reflected strong field.
    return NewObject<UActorComponent>(World, Class, MakeUniqueObjectName(World, Class, ComponentName), RF_Transient);
}

bool UDasWorldComponentLibrary::RegisterWorldComponent(UObject* WorldContextObject, UActorComponent* Component)
{
    UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull) : nullptr;
    if (!IsValid(World) || World->bIsTearingDown || !IsValid(Component) || Component->GetOuter() != World || Component->GetOwner())
        return false;
    if (!Component->IsRegistered())
        Component->RegisterComponentWithWorld(World);
    return Component->IsRegistered() && Component->GetWorld() == World;
}

bool UDasWorldComponentLibrary::DestroyWorldComponent(UActorComponent* Component)
{
    if (!IsValid(Component) || !Component->GetOuter()->IsA<UWorld>() || Component->GetOwner())
        return false;
    Component->DestroyComponent();
    return true;
}

TArray<int32> UDasWorldComponentLibrary::AddInstancesById(UInstancedStaticMeshComponent* Mesh,
    const TArray<FTransform>& Transforms, bool WorldSpace, bool UpdateNavigation)
{
    TArray<int32> Result;
    if (!IsValid(Mesh) || !Mesh->IsRegistered())
        return Result;
    for (const FTransform& Transform : Transforms)
        if (!Transform.IsValid())
            return Result;
    const TArray<FPrimitiveInstanceId> Ids = Mesh->AddInstancesById(MakeArrayView(Transforms), WorldSpace, UpdateNavigation);
    Result.Reserve(Ids.Num());
    for (FPrimitiveInstanceId Id : Ids)
        Result.Add(Id.GetAsIndex());
    return Result;
}

bool UDasWorldComponentLibrary::RemoveInstancesById(UInstancedStaticMeshComponent* Mesh, const TArray<int32>& Ids,
                                                   bool UpdateNavigation)
{
    if (!IsValid(Mesh) || !Mesh->IsRegistered())
        return false;
    TArray<FPrimitiveInstanceId> NativeIds;
    NativeIds.Reserve(Ids.Num());
    TSet<int32> Seen;
    for (int32 Value : Ids)
    {
        FPrimitiveInstanceId Id;
        Id.Id = Value;
        if (Seen.Contains(Value) || !Mesh->IsValidId(Id))
            return false;
        Seen.Add(Value);
        NativeIds.Add(Id);
    }
    Mesh->RemoveInstancesById(MakeArrayView(NativeIds), UpdateNavigation);
    return true;
}

int32 UDasWorldComponentLibrary::GetInstanceIndexById(UInstancedStaticMeshComponent* Mesh, int32 Value)
{
    FPrimitiveInstanceId Id;
    Id.Id = Value;
    return IsValid(Mesh) && Mesh->IsValidId(Id) ? Mesh->GetInstanceIndexForId(Id) : INDEX_NONE;
}
