// Native module ue_engine: script types Actor, Pawn, ActorComponent, SceneComponent, World and GameInstance with
// their most used methods, bound directly (no reflection). Game modules declare their own classes on top of these
// (Class<AMyPickup>(TEXT("Pickup")) inherits the Actor methods) or add methods to them (Class<AActor>()).
// Built on the public native binds API (DasNativeBinds.h). Engine functions are called from small lambdas: the
// address of a function imported from the engine DLL is not a template argument.
#include "DasNativeBinds.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

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
// Actor: relations.
DAS_ENGINE_FN(GetActorOwner, (const AActor& A), A.GetOwner())
DAS_ENGINE_FN(SetActorOwner, (AActor & A, AActor* Owner), A.SetOwner(Owner))
DAS_ENGINE_FN(GetInstigator, (const AActor& A), A.GetInstigator())
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
DAS_ENGINE_FN(GetGameInstanceWorld, (const UGameInstance& G), G.GetWorld())
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
}

DAS_NATIVE_BINDS(UEEngine, "ue_engine")
{
    using namespace DasNativeEngine;
    auto Actor = Binds.Class<AActor>(TEXT("Actor"));
    Actor.Method<GetActorLocation>(TEXT("get_actor_location"));
    Actor.Method<SetActorLocation>(TEXT("set_actor_location"));
    Actor.Method<GetActorRotation>(TEXT("get_actor_rotation"));
    Actor.Method<SetActorRotation>(TEXT("set_actor_rotation"));
    Actor.Method<GetActorTransform>(TEXT("get_actor_transform"));
    Actor.Method<SetActorTransform>(TEXT("set_actor_transform"));
    Actor.Method<GetActorScale3D>(TEXT("get_actor_scale3d"));
    Actor.Method<SetActorScale3D>(TEXT("set_actor_scale3d"));
    Actor.Method<AddActorWorldOffset>(TEXT("add_actor_world_offset"));
    Actor.Method<AddActorLocalOffset>(TEXT("add_actor_local_offset"));
    Actor.Method<AddActorWorldRotation>(TEXT("add_actor_world_rotation"));
    Actor.Method<AddActorLocalRotation>(TEXT("add_actor_local_rotation"));
    Actor.Method<GetActorForwardVector>(TEXT("get_actor_forward_vector"));
    Actor.Method<GetActorRightVector>(TEXT("get_actor_right_vector"));
    Actor.Method<GetActorUpVector>(TEXT("get_actor_up_vector"));
    Actor.Method<GetDistanceTo>(TEXT("get_distance_to"), EDasBind::NonNullArgs);
    Actor.Method<GetSquaredDistanceTo>(TEXT("get_squared_distance_to"), EDasBind::NonNullArgs);
    Actor.Method<GetHorizontalDistanceTo>(TEXT("get_horizontal_distance_to"), EDasBind::NonNullArgs);
    Actor.Method<GetVelocity>(TEXT("get_velocity"));
    Actor.Method<IsHidden>(TEXT("is_hidden"));
    Actor.Method<SetActorHiddenInGame>(TEXT("set_actor_hidden_in_game"));
    Actor.Method<SetActorEnableCollision>(TEXT("set_actor_enable_collision"));
    Actor.Method<GetActorEnableCollision>(TEXT("get_actor_enable_collision"));
    Actor.Method<HasActorBegunPlay>(TEXT("has_actor_begun_play"));
    Actor.Method<IsActorInitialized>(TEXT("is_actor_initialized"));
    Actor.Method<IsActorBeingDestroyed>(TEXT("is_actor_being_destroyed"));
    Actor.Method<Destroy>(TEXT("destroy"));
    Actor.Method<GetLifeSpan>(TEXT("get_life_span"));
    Actor.Method<SetLifeSpan>(TEXT("set_life_span"));
    Actor.Method<GetGameTimeSinceCreation>(TEXT("get_game_time_since_creation"));
    Actor.Method<SetActorTickEnabled>(TEXT("set_actor_tick_enabled"));
    Actor.Method<IsActorTickEnabled>(TEXT("is_actor_tick_enabled"));
    Actor.Method<GetActorNameOrLabel>(TEXT("get_actor_name_or_label"));
    Actor.Method<ActorHasTag>(TEXT("actor_has_tag"));
    Actor.Method<GetActorOwner>(TEXT("get_owner"));
    Actor.Method<SetActorOwner>(TEXT("set_owner"));
    Actor.Method<GetInstigator>(TEXT("get_instigator"));
    Actor.Method<GetRootComponent>(TEXT("get_root_component"));
    Actor.Method<GetActorWorld>(TEXT("get_world"));

    auto Pawn = Binds.Class<APawn>(TEXT("Pawn"));
    Pawn.Method<GetController>(TEXT("get_controller"));
    Pawn.Method<IsLocallyControlled>(TEXT("is_locally_controlled"));
    Pawn.Method<IsPlayerControlled>(TEXT("is_player_controlled"));
    Pawn.Method<AddMovementInput>(TEXT("add_movement_input"));

    auto Component = Binds.Class<UActorComponent>(TEXT("ActorComponent"));
    Component.Method<GetComponentOwner>(TEXT("get_owner"));
    Component.Method<IsActive>(TEXT("is_active"));
    Component.Method<Activate>(TEXT("activate"));
    Component.Method<Deactivate>(TEXT("deactivate"));
    Component.Method<SetActive>(TEXT("set_active"));
    Component.Method<HasBegunPlay>(TEXT("has_begun_play"));
    Component.Method<IsRegistered>(TEXT("is_registered"));
    Component.Method<SetComponentTickEnabled>(TEXT("set_component_tick_enabled"));
    Component.Method<IsComponentTickEnabled>(TEXT("is_component_tick_enabled"));
    Component.Method<DestroyComponent>(TEXT("destroy_component"));
    Component.Method<ComponentHasTag>(TEXT("component_has_tag"));
    Component.Method<GetComponentWorld>(TEXT("get_world"));

    auto Scene = Binds.Class<USceneComponent>(TEXT("SceneComponent"));
    Scene.Method<GetComponentLocation>(TEXT("get_component_location"));
    Scene.Method<GetComponentRotation>(TEXT("get_component_rotation"));
    Scene.Method<GetComponentTransform>(TEXT("get_component_transform"));
    Scene.Method<SetWorldLocation>(TEXT("set_world_location"));
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
    Scene.Method<GetForwardVector>(TEXT("get_forward_vector"));
    Scene.Method<GetRightVector>(TEXT("get_right_vector"));
    Scene.Method<GetUpVector>(TEXT("get_up_vector"));
    Scene.Method<GetSocketLocation>(TEXT("get_socket_location"));
    Scene.Method<GetSocketRotation>(TEXT("get_socket_rotation"));
    Scene.Method<DoesSocketExist>(TEXT("does_socket_exist"));
    Scene.Method<GetComponentVelocity>(TEXT("get_component_velocity"));
    Scene.Method<SetComponentVelocity>(TEXT("set_component_velocity"));
    Scene.Method<GetAttachParent>(TEXT("get_attach_parent"));
    Scene.Method<GetAttachSocketName>(TEXT("get_attach_socket_name"));
    Scene.Method<&AttachTo>(TEXT("attach_to"), EDasBind::NonNullArgs);
    Scene.Method<&Detach>(TEXT("detach"));
    Scene.Method<SetVisibility>(TEXT("set_visibility"));
    Scene.Method<IsVisible>(TEXT("is_visible"));
    Scene.Method<SetHiddenInGame>(TEXT("set_hidden_in_game"));

    auto World = Binds.Class<UWorld>(TEXT("World"));
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
    Binds.Class<UGameInstance>(TEXT("GameInstance")).Method<GetGameInstanceWorld>(TEXT("get_world"));
    Binds.Function<&WorldOf>(TEXT("world_of"));
    Binds.Function<&FrameNumber>(TEXT("frame_number"));
}
