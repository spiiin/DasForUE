// Native module ue_engine: script types AActor, APawn, UActorComponent, USceneComponent, UWorld and UGameInstance
// with their most used methods, bound directly (no reflection). The types are named as in C++: the short names
// (Actor, Pawn, ...) are DSL base classes in ue_dsl and would be ambiguous in a script that requires both. Game
// modules declare their own classes on top of these (Class<AMyPickup>(TEXT("AMyPickup")) inherits the AActor
// methods) or add methods to them (Class<AActor>()).
// Built on the public native binds API (DasNativeBinds.h). Engine functions are called from small lambdas: the
// address of a function imported from the engine DLL is not a template argument. Methods that do what a UFUNCTION
// does reflect it (Reflects): gameplay scripts calling that UFUNCTION on a native reference call the bind.
#include "DasNativeBinds.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/Level.h"
#include "Engine/LevelScriptActor.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

namespace DasNativeEngine
{
#define DAS_ENGINE_FN(Name, Params, Body)                                                                              \
    constexpr auto Name = +[] Params                                                                                   \
    {                                                                                                                  \
        return Body;                                                                                                   \
    };

// Actor: placement. Moves teleport without sweeping (the sweep result is not bound yet).
DAS_ENGINE_FN(GetActorLocation, (const AActor& A), A.GetActorLocation())
DAS_ENGINE_FN(SetActorLocation, (AActor & A, const FVector& V), A.SetActorLocation(V))
DAS_ENGINE_FN(GetActorRotation, (const AActor& A), A.GetActorRotation())
DAS_ENGINE_FN(SetActorRotation, (AActor & A, const FRotator& R), A.SetActorRotation(R))
DAS_ENGINE_FN(GetActorTransform, (const AActor& A), A.GetActorTransform())
DAS_ENGINE_FN(SetActorTransform, (AActor & A, const FTransform& T), A.SetActorTransform(T))
DAS_ENGINE_FN(GetActorScale3D, (const AActor& A), A.GetActorScale3D())
DAS_ENGINE_FN(SetActorScale3D, (AActor & A, const FVector& V), A.SetActorScale3D(V))
DAS_ENGINE_FN(AddActorWorldOffset, (AActor & A, const FVector& V), A.AddActorWorldOffset(V))
DAS_ENGINE_FN(AddActorLocalOffset, (AActor & A, const FVector& V), A.AddActorLocalOffset(V))
DAS_ENGINE_FN(AddActorWorldRotation, (AActor & A, const FRotator& R), A.AddActorWorldRotation(R))
DAS_ENGINE_FN(AddActorLocalRotation, (AActor & A, const FRotator& R), A.AddActorLocalRotation(R))
DAS_ENGINE_FN(GetActorForwardVector, (const AActor& A), A.GetActorForwardVector())
DAS_ENGINE_FN(GetActorRightVector, (const AActor& A), A.GetActorRightVector())
DAS_ENGINE_FN(GetActorUpVector, (const AActor& A), A.GetActorUpVector())
DAS_ENGINE_FN(GetDistanceTo, (const AActor& A, const AActor* Other), A.GetDistanceTo(Other))
DAS_ENGINE_FN(GetSquaredDistanceTo, (const AActor& A, const AActor* Other), A.GetSquaredDistanceTo(Other))
DAS_ENGINE_FN(GetHorizontalDistanceTo, (const AActor& A, const AActor* Other), A.GetHorizontalDistanceTo(Other))
DAS_ENGINE_FN(GetVelocity, (const AActor& A), A.GetVelocity())
// Output parameters are T& (gameplay calls of these UFUNCTIONs pass their out arguments to them).
DAS_ENGINE_FN(GetActorBounds,
              (const AActor& A, bool bOnlyColliding, FVector& Origin, FVector& Extent, bool bChildActors),
              A.GetActorBounds(bOnlyColliding, Origin, Extent, bChildActors))
DAS_ENGINE_FN(GetActorEyesViewPoint, (const AActor& A, FVector& Location, FRotator& Rotation),
              A.GetActorEyesViewPoint(Location, Rotation))
// Enums pass as their int32 values (gameplay calls convert the script's native enums).
DAS_ENGINE_FN(GetLocalRole, (const AActor& A), A.GetLocalRole())
// Actor: state.
DAS_ENGINE_FN(IsHidden, (const AActor& A), A.IsHidden())
DAS_ENGINE_FN(SetActorHiddenInGame, (AActor & A, bool bHidden), A.SetActorHiddenInGame(bHidden))
DAS_ENGINE_FN(SetActorEnableCollision, (AActor & A, bool bEnable), A.SetActorEnableCollision(bEnable))
DAS_ENGINE_FN(GetActorEnableCollision, (const AActor& A), A.GetActorEnableCollision())
DAS_ENGINE_FN(HasActorBegunPlay, (const AActor& A), A.HasActorBegunPlay())
DAS_ENGINE_FN(IsActorInitialized, (const AActor& A), A.IsActorInitialized())
DAS_ENGINE_FN(IsActorBeingDestroyed, (const AActor& A), A.IsActorBeingDestroyed())
DAS_ENGINE_FN(Destroy, (AActor & A), A.Destroy())
DAS_ENGINE_FN(GetLifeSpan, (const AActor& A), A.GetLifeSpan())
DAS_ENGINE_FN(SetLifeSpan, (AActor & A, float Seconds), A.SetLifeSpan(Seconds))
DAS_ENGINE_FN(GetGameTimeSinceCreation, (const AActor& A), A.GetGameTimeSinceCreation())
DAS_ENGINE_FN(SetActorTickEnabled, (AActor & A, bool bEnabled), A.SetActorTickEnabled(bEnabled))
DAS_ENGINE_FN(IsActorTickEnabled, (const AActor& A), A.IsActorTickEnabled())
DAS_ENGINE_FN(GetActorNameOrLabel, (const AActor& A), A.GetActorNameOrLabel())
DAS_ENGINE_FN(ActorHasTag, (const AActor& A, FName Tag), A.ActorHasTag(Tag))
DAS_ENGINE_FN(SetReplicates, (AActor & A, bool bReplicates), A.SetReplicates(bReplicates))
// Actor: relations.
DAS_ENGINE_FN(GetActorOwner, (const AActor& A), A.GetOwner())
DAS_ENGINE_FN(SetActorOwner, (AActor & A, AActor* Owner), A.SetOwner(Owner))
DAS_ENGINE_FN(GetInstigator, (const AActor& A), A.GetInstigator())
DAS_ENGINE_FN(GetInstigatorController, (const AActor& A), A.GetInstigatorController())
DAS_ENGINE_FN(GetRootComponent, (const AActor& A), A.GetRootComponent())
DAS_ENGINE_FN(GetActorWorld, (const AActor& A), A.GetWorld())

// Pawn.
DAS_ENGINE_FN(GetController, (const APawn& P), P.GetController())
DAS_ENGINE_FN(IsLocallyControlled, (const APawn& P), P.IsLocallyControlled())
DAS_ENGINE_FN(IsPlayerControlled, (const APawn& P), P.IsPlayerControlled())
DAS_ENGINE_FN(AddMovementInput, (APawn & P, const FVector& Direction, float Scale),
              P.AddMovementInput(Direction, Scale))

// ActorComponent.
DAS_ENGINE_FN(GetComponentOwner, (const UActorComponent& C), C.GetOwner())
DAS_ENGINE_FN(IsActive, (const UActorComponent& C), C.IsActive())
DAS_ENGINE_FN(Activate, (UActorComponent & C), C.Activate())
DAS_ENGINE_FN(Deactivate, (UActorComponent & C), C.Deactivate())
DAS_ENGINE_FN(SetActive, (UActorComponent & C, bool bActive), C.SetActive(bActive))
DAS_ENGINE_FN(HasBegunPlay, (const UActorComponent& C), C.HasBegunPlay())
DAS_ENGINE_FN(IsRegistered, (const UActorComponent& C), C.IsRegistered())
DAS_ENGINE_FN(SetComponentTickEnabled, (UActorComponent & C, bool bEnabled), C.SetComponentTickEnabled(bEnabled))
DAS_ENGINE_FN(IsComponentTickEnabled, (const UActorComponent& C), C.IsComponentTickEnabled())
DAS_ENGINE_FN(DestroyComponent, (UActorComponent & C), C.DestroyComponent())
DAS_ENGINE_FN(ComponentHasTag, (const UActorComponent& C, FName Tag), C.ComponentHasTag(Tag))
DAS_ENGINE_FN(GetComponentWorld, (const UActorComponent& C), C.GetWorld())
// EComponentCreationMethod as its number: 0 native, 1 construction script (simple), 2 user construction script,
// 3 instance.
DAS_ENGINE_FN(GetComponentCreationMethod, (const UActorComponent& C), int32(C.CreationMethod))
DAS_ENGINE_FN(MarkRenderStateDirty, (UActorComponent & C), C.MarkRenderStateDirty())

// SceneComponent: placement.
DAS_ENGINE_FN(GetComponentLocation, (const USceneComponent& C), C.GetComponentLocation())
DAS_ENGINE_FN(GetComponentRotation, (const USceneComponent& C), C.GetComponentRotation())
DAS_ENGINE_FN(GetComponentTransform, (const USceneComponent& C), C.GetComponentTransform())
DAS_ENGINE_FN(SetWorldLocation, (USceneComponent & C, const FVector& V), C.SetWorldLocation(V))
DAS_ENGINE_FN(SetWorldRotation, (USceneComponent & C, const FRotator& R), C.SetWorldRotation(R))
DAS_ENGINE_FN(GetRelativeLocation, (const USceneComponent& C), C.GetRelativeLocation())
DAS_ENGINE_FN(SetRelativeLocation, (USceneComponent & C, const FVector& V), C.SetRelativeLocation(V))
DAS_ENGINE_FN(GetRelativeRotation, (const USceneComponent& C), C.GetRelativeRotation())
DAS_ENGINE_FN(SetRelativeRotation, (USceneComponent & C, const FRotator& R), C.SetRelativeRotation(R))
DAS_ENGINE_FN(GetRelativeScale3D, (const USceneComponent& C), C.GetRelativeScale3D())
DAS_ENGINE_FN(SetRelativeScale3D, (USceneComponent & C, const FVector& V), C.SetRelativeScale3D(V))
DAS_ENGINE_FN(AddWorldOffset, (USceneComponent & C, const FVector& V), C.AddWorldOffset(V))
DAS_ENGINE_FN(AddLocalOffset, (USceneComponent & C, const FVector& V), C.AddLocalOffset(V))
DAS_ENGINE_FN(AddWorldRotation, (USceneComponent & C, const FRotator& R), C.AddWorldRotation(R))
DAS_ENGINE_FN(AddLocalRotation, (USceneComponent & C, const FRotator& R), C.AddLocalRotation(R))
DAS_ENGINE_FN(GetForwardVector, (const USceneComponent& C), C.GetForwardVector())
DAS_ENGINE_FN(GetRightVector, (const USceneComponent& C), C.GetRightVector())
DAS_ENGINE_FN(GetUpVector, (const USceneComponent& C), C.GetUpVector())
DAS_ENGINE_FN(GetSocketLocation, (const USceneComponent& C, FName Socket), C.GetSocketLocation(Socket))
DAS_ENGINE_FN(GetSocketRotation, (const USceneComponent& C, FName Socket), C.GetSocketRotation(Socket))
DAS_ENGINE_FN(DoesSocketExist, (const USceneComponent& C, FName Socket), C.DoesSocketExist(Socket))
DAS_ENGINE_FN(GetComponentVelocity, (const USceneComponent& C), C.GetComponentVelocity())
DAS_ENGINE_FN(SetComponentVelocity, (USceneComponent & C, const FVector& V), void(C.ComponentVelocity = V))
DAS_ENGINE_FN(SetMobility, (USceneComponent & C, EComponentMobility::Type Mobility), C.SetMobility(Mobility))
// SceneComponent: attachment and visibility.
DAS_ENGINE_FN(GetAttachParent, (const USceneComponent& C), C.GetAttachParent())
DAS_ENGINE_FN(GetAttachSocketName, (const USceneComponent& C), C.GetAttachSocketName())
DAS_ENGINE_FN(SetVisibility, (USceneComponent & C, bool bVisible), C.SetVisibility(bVisible))
DAS_ENGINE_FN(IsVisible, (const USceneComponent& C), C.IsVisible())
DAS_ENGINE_FN(SetHiddenInGame, (USceneComponent & C, bool bHidden), C.SetHiddenInGame(bHidden))
bool AttachTo(USceneComponent& C, USceneComponent* Parent, FName Socket, bool bKeepWorld)
{
    const FAttachmentTransformRules& Rules =
        bKeepWorld ? FAttachmentTransformRules::KeepWorldTransform : FAttachmentTransformRules::KeepRelativeTransform;
    return C.AttachToComponent(Parent, Rules, Socket);
}
void Detach(USceneComponent& C, bool bKeepWorld)
{
    C.DetachFromComponent(bKeepWorld ? FDetachmentTransformRules::KeepWorldTransform
                                     : FDetachmentTransformRules::KeepRelativeTransform);
}

// World: time, kind, network mode.
DAS_ENGINE_FN(GetTimeSeconds, (const UWorld& W), W.GetTimeSeconds())
DAS_ENGINE_FN(GetUnpausedTimeSeconds, (const UWorld& W), W.GetUnpausedTimeSeconds())
DAS_ENGINE_FN(GetRealTimeSeconds, (const UWorld& W), W.GetRealTimeSeconds())
DAS_ENGINE_FN(GetAudioTimeSeconds, (const UWorld& W), W.GetAudioTimeSeconds())
DAS_ENGINE_FN(GetDeltaSeconds, (const UWorld& W), W.GetDeltaSeconds())
DAS_ENGINE_FN(IsGameWorld, (const UWorld& W), W.IsGameWorld())
DAS_ENGINE_FN(IsEditorWorld, (const UWorld& W), W.IsEditorWorld())
DAS_ENGINE_FN(IsPreviewWorld, (const UWorld& W), W.IsPreviewWorld())
DAS_ENGINE_FN(IsTearingDown, (const UWorld& W), W.bIsTearingDown != 0)
DAS_ENGINE_FN(IsPaused, (const UWorld& W), W.IsPaused())
DAS_ENGINE_FN(IsStandalone, (const UWorld& W), W.GetNetMode() == NM_Standalone)
DAS_ENGINE_FN(IsClient, (const UWorld& W), W.GetNetMode() == NM_Client)
DAS_ENGINE_FN(IsListenServer, (const UWorld& W), W.GetNetMode() == NM_ListenServer)
DAS_ENGINE_FN(IsDedicatedServer, (const UWorld& W), W.GetNetMode() == NM_DedicatedServer)
DAS_ENGINE_FN(IsServer, (const UWorld& W), W.GetNetMode() == NM_ListenServer || W.GetNetMode() == NM_DedicatedServer)
DAS_ENGINE_FN(GetGameInstance, (const UWorld& W), W.GetGameInstance())
// ENetMode as its number: 0 standalone, 1 dedicated server, 2 listen server, 3 client.
DAS_ENGINE_FN(GetNetMode, (const UWorld& W), int32(W.GetNetMode()))
DAS_ENGINE_FN(IsStartingUp, (const UWorld& W), W.bStartup != 0)
DAS_ENGINE_FN(GetPersistentLevel, (const UWorld& W), W.PersistentLevel.Get())
DAS_ENGINE_FN(GetLevelScriptActor, (const UWorld& W), W.GetLevelScriptActor())

// GameInstance: local players (null for an index or controller id it does not have).
DAS_ENGINE_FN(GetGameInstanceWorld, (const UGameInstance& G), G.GetWorld())
DAS_ENGINE_FN(GetNumLocalPlayers, (const UGameInstance& G), G.GetNumLocalPlayers())
DAS_ENGINE_FN(GetLocalPlayerByIndex, (const UGameInstance& G, int32 Index),
              Index >= 0 && Index < G.GetNumLocalPlayers() ? G.GetLocalPlayerByIndex(Index) : nullptr)
DAS_ENGINE_FN(FindLocalPlayerFromControllerId, (const UGameInstance& G, int32 Id),
              G.FindLocalPlayerFromControllerId(Id))
DAS_ENGINE_FN(GetFirstGamePlayer, (const UGameInstance& G), G.GetFirstGamePlayer())
DAS_ENGINE_FN(GetFirstLocalPlayerController, (const UGameInstance& G), G.GetFirstLocalPlayerController())
DAS_ENGINE_FN(GetFirstLocalPlayerControllerIn, (const UGameInstance& G, const UWorld* World),
              G.GetFirstLocalPlayerController(World))
#undef DAS_ENGINE_FN

// The world of any object (UObjectHandle): an actor, a component, a widget...; null when it has none.
UWorld* WorldOf(UObject* Object)
{
    return Object && GEngine ? GEngine->GetWorldFromContextObject(Object, EGetWorldErrorMode::ReturnNull) : nullptr;
}
int64 FrameNumber()
{
    return int64(GFrameCounter);
}
// Components by class (a null class finds nothing, as in the engine).
TArray<UActorComponent*> GetAllComponents(const AActor& A)
{
    TArray<UActorComponent*> Components;
    A.GetComponents(Components);
    return Components;
}
TArray<UActorComponent*> GetComponentsByClass(const AActor& A, TSubclassOf<UActorComponent> Class)
{
    return Class ? A.K2_GetComponentsByClass(Class) : TArray<UActorComponent*>();
}
TArray<UActorComponent*> GetComponentsByClassWithTag(const AActor& A, TSubclassOf<UActorComponent> Class, FName Tag)
{
    return Class ? A.GetComponentsByTag(Class, Tag) : TArray<UActorComponent*>();
}
// As AngelScript: the first direct child of the class.
USceneComponent* GetChildComponentByClass(const USceneComponent& C, TSubclassOf<USceneComponent> Class)
{
    if (!Class)
        return nullptr;
    for (int32 I = 0; I < C.GetNumChildrenComponents(); ++I)
        if (USceneComponent* Child = C.GetChildComponent(I); Child && Child->IsA(Class))
            return Child;
    return nullptr;
}
TArray<USceneComponent*> GetChildrenComponentsByClass(const USceneComponent& C, TSubclassOf<USceneComponent> Class,
                                                      bool bIncludeAllDescendants)
{
    TArray<USceneComponent*> Children;
    if (Class)
    {
        C.GetChildrenComponents(bIncludeAllDescendants, Children);
        Children.RemoveAll([&](const USceneComponent* Child) { return !Child || !Child->IsA(Class); });
    }
    return Children;
}
// Local players: Error receives why a player was not created (null result).
ULocalPlayer* CreateLocalPlayer(UGameInstance& G, int32 ControllerId, FString& Error, bool bSpawnPlayerController)
{
    return G.CreateLocalPlayer(ControllerId, Error, bSpawnPlayerController);
}
ULocalPlayer* CreateInitialPlayer(UGameInstance& G, FString& Error)
{
    return G.CreateInitialPlayer(Error);
}
bool RemoveLocalPlayer(UGameInstance& G, ULocalPlayer* Player)
{
    return Player && G.RemoveLocalPlayer(Player);
}
}

DAS_NATIVE_BINDS(UEEngine, "ue_engine")
{
    using namespace DasNativeEngine;
    auto Actor = Binds.Class<AActor>(TEXT("AActor"));
    Actor.Method<GetActorLocation>(TEXT("get_actor_location")).Reflects(TEXT("K2_GetActorLocation"));
    Actor.Method<SetActorLocation>(TEXT("set_actor_location"));
    Actor.Method<GetActorRotation>(TEXT("get_actor_rotation")).Reflects(TEXT("K2_GetActorRotation"));
    Actor.Method<SetActorRotation>(TEXT("set_actor_rotation"));
    Actor.Method<GetActorTransform>(TEXT("get_actor_transform")).Reflects(TEXT("GetTransform"));
    Actor.Method<SetActorTransform>(TEXT("set_actor_transform"));
    Actor.Method<GetActorScale3D>(TEXT("get_actor_scale3d")).Reflects(TEXT("GetActorScale3D"));
    Actor.Method<SetActorScale3D>(TEXT("set_actor_scale3d")).Reflects(TEXT("SetActorScale3D"));
    Actor.Method<AddActorWorldOffset>(TEXT("add_actor_world_offset"));
    Actor.Method<AddActorLocalOffset>(TEXT("add_actor_local_offset"));
    Actor.Method<AddActorWorldRotation>(TEXT("add_actor_world_rotation"));
    Actor.Method<AddActorLocalRotation>(TEXT("add_actor_local_rotation"));
    Actor.Method<GetActorForwardVector>(TEXT("get_actor_forward_vector")).Reflects(TEXT("GetActorForwardVector"));
    Actor.Method<GetActorRightVector>(TEXT("get_actor_right_vector")).Reflects(TEXT("GetActorRightVector"));
    Actor.Method<GetActorUpVector>(TEXT("get_actor_up_vector")).Reflects(TEXT("GetActorUpVector"));
    Actor.Method<GetActorBounds>(TEXT("get_actor_bounds")).Reflects(TEXT("GetActorBounds"));
    Actor.Method<GetActorEyesViewPoint>(TEXT("get_actor_eyes_view_point")).Reflects(TEXT("GetActorEyesViewPoint"));
    Actor.Method<GetLocalRole>(TEXT("get_local_role")).Reflects(TEXT("GetLocalRole"));
    // NonNullArgs (null is an error here), while the UFUNCTIONs return 0 for null: these do not reflect them.
    Actor.Method<GetDistanceTo>(TEXT("get_distance_to"), EDasBind::NonNullArgs);
    Actor.Method<GetSquaredDistanceTo>(TEXT("get_squared_distance_to"), EDasBind::NonNullArgs);
    Actor.Method<GetHorizontalDistanceTo>(TEXT("get_horizontal_distance_to"), EDasBind::NonNullArgs);
    Actor.Method<GetVelocity>(TEXT("get_velocity")).Reflects(TEXT("GetVelocity"));
    Actor.Method<IsHidden>(TEXT("is_hidden"));
    Actor.Method<SetActorHiddenInGame>(TEXT("set_actor_hidden_in_game")).Reflects(TEXT("SetActorHiddenInGame"));
    Actor.Method<SetActorEnableCollision>(TEXT("set_actor_enable_collision")).Reflects(TEXT("SetActorEnableCollision"));
    Actor.Method<GetActorEnableCollision>(TEXT("get_actor_enable_collision")).Reflects(TEXT("GetActorEnableCollision"));
    Actor.Method<HasActorBegunPlay>(TEXT("has_actor_begun_play"));
    Actor.Method<IsActorInitialized>(TEXT("is_actor_initialized"));
    Actor.Method<IsActorBeingDestroyed>(TEXT("is_actor_being_destroyed"));
    Actor.Method<Destroy>(TEXT("destroy"));
    Actor.Method<GetLifeSpan>(TEXT("get_life_span")).Reflects(TEXT("GetLifeSpan"));
    Actor.Method<SetLifeSpan>(TEXT("set_life_span")).Reflects(TEXT("SetLifeSpan"));
    Actor.Method<GetGameTimeSinceCreation>(TEXT("get_game_time_since_creation"))
        .Reflects(TEXT("GetGameTimeSinceCreation"));
    Actor.Method<SetActorTickEnabled>(TEXT("set_actor_tick_enabled")).Reflects(TEXT("SetActorTickEnabled"));
    Actor.Method<IsActorTickEnabled>(TEXT("is_actor_tick_enabled")).Reflects(TEXT("IsActorTickEnabled"));
    Actor.Method<GetActorNameOrLabel>(TEXT("get_actor_name_or_label"));
    Actor.Method<ActorHasTag>(TEXT("actor_has_tag")).Reflects(TEXT("ActorHasTag"));
    // BlueprintAuthorityOnly: gameplay scripts keep the reflected call, which checks authority.
    Actor.Method<SetReplicates>(TEXT("set_replicates")).Reflects(TEXT("SetReplicates"));
    Actor.Method<GetActorOwner>(TEXT("get_owner")).Reflects(TEXT("GetOwner"));
    Actor.Method<SetActorOwner>(TEXT("set_owner")).Reflects(TEXT("SetOwner"));
    Actor.Method<GetInstigator>(TEXT("get_instigator"));
    Actor.Method<GetInstigator>(TEXT("get_actor_instigator")); // AngelScript's name
    Actor.Method<GetInstigatorController>(TEXT("get_actor_instigator_controller"));
    Actor.Method<GetRootComponent>(TEXT("get_root_component")).Reflects(TEXT("K2_GetRootComponent"));
    Actor.Method<GetActorWorld>(TEXT("get_world"));
    Actor.Method<&GetAllComponents>(TEXT("get_all_components"));
    Actor.Method<&GetComponentsByClass>(TEXT("get_components_by_class"));
    Actor.Method<&GetComponentsByClassWithTag>(TEXT("get_components_by_class_with_tag"));

    auto Pawn = Binds.Class<APawn>(TEXT("APawn"));
    Pawn.Method<GetController>(TEXT("get_controller")).Reflects(TEXT("GetController"));
    Pawn.Method<IsLocallyControlled>(TEXT("is_locally_controlled")).Reflects(TEXT("IsLocallyControlled"));
    Pawn.Method<IsPlayerControlled>(TEXT("is_player_controlled")).Reflects(TEXT("IsPlayerControlled"));
    Pawn.Method<AddMovementInput>(TEXT("add_movement_input"));

    auto Component = Binds.Class<UActorComponent>(TEXT("UActorComponent"));
    Component.Method<GetComponentOwner>(TEXT("get_owner")).Reflects(TEXT("GetOwner"));
    Component.Method<IsActive>(TEXT("is_active")).Reflects(TEXT("IsActive"));
    Component.Method<Activate>(TEXT("activate"));
    Component.Method<Deactivate>(TEXT("deactivate"));
    Component.Method<SetActive>(TEXT("set_active"));
    Component.Method<HasBegunPlay>(TEXT("has_begun_play"));
    Component.Method<IsRegistered>(TEXT("is_registered"));
    Component.Method<SetComponentTickEnabled>(TEXT("set_component_tick_enabled"))
        .Reflects(TEXT("SetComponentTickEnabled"));
    Component.Method<IsComponentTickEnabled>(TEXT("is_component_tick_enabled"))
        .Reflects(TEXT("IsComponentTickEnabled"));
    Component.Method<DestroyComponent>(TEXT("destroy_component"));
    Component.Method<ComponentHasTag>(TEXT("component_has_tag")).Reflects(TEXT("ComponentHasTag"));
    Component.Method<GetComponentWorld>(TEXT("get_world"));
    Component.Method<GetComponentCreationMethod>(TEXT("get_component_creation_method"));
    Component.Method<MarkRenderStateDirty>(TEXT("mark_render_state_dirty"));

    auto Scene = Binds.Class<USceneComponent>(TEXT("USceneComponent"));
    Scene.Method<GetComponentLocation>(TEXT("get_component_location")).Reflects(TEXT("K2_GetComponentLocation"));
    Scene.Method<GetComponentRotation>(TEXT("get_component_rotation")).Reflects(TEXT("K2_GetComponentRotation"));
    Scene.Method<GetComponentTransform>(TEXT("get_component_transform")).Reflects(TEXT("K2_GetComponentToWorld"));
    Scene.Method<SetWorldLocation>(TEXT("set_world_location"));
    Scene.Method<SetMobility>(TEXT("set_mobility")).Reflects(TEXT("SetMobility"));
    Scene.Method<SetWorldRotation>(TEXT("set_world_rotation"));
    Scene.Method<GetRelativeLocation>(TEXT("get_relative_location"));
    Scene.Method<SetRelativeLocation>(TEXT("set_relative_location"));
    Scene.Method<GetRelativeRotation>(TEXT("get_relative_rotation"));
    Scene.Method<SetRelativeRotation>(TEXT("set_relative_rotation"));
    Scene.Method<GetRelativeScale3D>(TEXT("get_relative_scale3d"));
    Scene.Method<SetRelativeScale3D>(TEXT("set_relative_scale3d"));
    Scene.Method<AddWorldOffset>(TEXT("add_world_offset"));
    Scene.Method<AddLocalOffset>(TEXT("add_local_offset"));
    Scene.Method<AddWorldRotation>(TEXT("add_world_rotation"));
    Scene.Method<AddLocalRotation>(TEXT("add_local_rotation"));
    Scene.Method<GetForwardVector>(TEXT("get_forward_vector")).Reflects(TEXT("GetForwardVector"));
    Scene.Method<GetRightVector>(TEXT("get_right_vector")).Reflects(TEXT("GetRightVector"));
    Scene.Method<GetUpVector>(TEXT("get_up_vector")).Reflects(TEXT("GetUpVector"));
    Scene.Method<GetSocketLocation>(TEXT("get_socket_location")).Reflects(TEXT("GetSocketLocation"));
    Scene.Method<GetSocketRotation>(TEXT("get_socket_rotation")).Reflects(TEXT("GetSocketRotation"));
    Scene.Method<DoesSocketExist>(TEXT("does_socket_exist")).Reflects(TEXT("DoesSocketExist"));
    Scene.Method<GetComponentVelocity>(TEXT("get_component_velocity")).Reflects(TEXT("GetComponentVelocity"));
    Scene.Method<SetComponentVelocity>(TEXT("set_component_velocity"));
    Scene.Method<GetAttachParent>(TEXT("get_attach_parent")).Reflects(TEXT("GetAttachParent"));
    Scene.Method<GetAttachSocketName>(TEXT("get_attach_socket_name")).Reflects(TEXT("GetAttachSocketName"));
    Scene.Method<&AttachTo>(TEXT("attach_to"), EDasBind::NonNullArgs);
    Scene.Method<&Detach>(TEXT("detach"));
    Scene.Method<&GetChildComponentByClass>(TEXT("get_child_component_by_class"));
    Scene.Method<&GetChildrenComponentsByClass>(TEXT("get_children_components_by_class"));
    Scene.Method<SetVisibility>(TEXT("set_visibility"));
    Scene.Method<IsVisible>(TEXT("is_visible")).Reflects(TEXT("IsVisible"));
    Scene.Method<SetHiddenInGame>(TEXT("set_hidden_in_game"));

    auto World = Binds.Class<UWorld>(TEXT("UWorld"));
    World.Method<GetTimeSeconds>(TEXT("get_time_seconds"));
    World.Method<GetUnpausedTimeSeconds>(TEXT("get_unpaused_time_seconds"));
    World.Method<GetRealTimeSeconds>(TEXT("get_real_time_seconds"));
    World.Method<GetAudioTimeSeconds>(TEXT("get_audio_time_seconds"));
    World.Method<GetDeltaSeconds>(TEXT("get_delta_seconds"));
    World.Method<IsGameWorld>(TEXT("is_game_world"));
    World.Method<IsEditorWorld>(TEXT("is_editor_world"));
    World.Method<IsPreviewWorld>(TEXT("is_preview_world"));
    World.Method<IsTearingDown>(TEXT("is_tearing_down"));
    World.Method<IsPaused>(TEXT("is_paused"));
    World.Method<IsStandalone>(TEXT("is_standalone"));
    World.Method<IsClient>(TEXT("is_client"));
    World.Method<IsServer>(TEXT("is_server"));
    World.Method<IsListenServer>(TEXT("is_listen_server"));
    World.Method<IsDedicatedServer>(TEXT("is_dedicated_server"));
    World.Method<GetGameInstance>(TEXT("get_game_instance"));
    World.Method<GetNetMode>(TEXT("get_net_mode"));
    World.Method<IsStartingUp>(TEXT("is_starting_up"));
    World.Method<GetPersistentLevel>(TEXT("get_persistent_level"));
    World.Method<GetLevelScriptActor>(TEXT("get_level_script_actor"));
    auto GameInstance = Binds.Class<UGameInstance>(TEXT("UGameInstance"));
    GameInstance.Method<GetGameInstanceWorld>(TEXT("get_world"));
    GameInstance.Method<GetNumLocalPlayers>(TEXT("get_num_local_players"));
    GameInstance.Method<GetLocalPlayerByIndex>(TEXT("get_local_player_by_index"));
    GameInstance.Method<FindLocalPlayerFromControllerId>(TEXT("find_local_player_from_controller_id"));
    GameInstance.Method<GetFirstGamePlayer>(TEXT("get_first_game_player"));
    GameInstance.Method<GetFirstLocalPlayerController>(TEXT("get_first_local_player_controller"));
    GameInstance.Method<GetFirstLocalPlayerControllerIn>(TEXT("get_first_local_player_controller"));
    GameInstance.Method<&CreateLocalPlayer>(TEXT("create_local_player"));
    GameInstance.Method<&CreateInitialPlayer>(TEXT("create_initial_player"));
    GameInstance.Method<&RemoveLocalPlayer>(TEXT("remove_local_player"));
    Binds.Function<&WorldOf>(TEXT("world_of"));
    Binds.Function<&FrameNumber>(TEXT("frame_number"));
}
