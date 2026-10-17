options gen2
require ue_native_gameplay

// The same generated members work in free functions and DSL instance methods.
def tune_camera(camera:CameraComponentRef; fov:float):float {
    camera.SetFieldOfView(fov)
    camera.FieldOfView+=1.0f
    return camera.FieldOfView
}
def pawn_alive(value:NativePawnRef):bool { return ue_object_valid(value.handle) }
def character_once(var count:int&):CharacterRef { count+=1; return ue_character_self() }
def parent_value(value:CharacterRef):NativePawnRef { return value }
def select_parent(value:NativePawnRef):int { return 1 }
def select_parent(value:ActorRef):int { return 2 }
def select_exact(value:CharacterRef):int { return 3 }
def select_exact(value:NativePawnRef):int { return 4 }
[export]
def main():bool {
    let character=ue_character_self()
    var count=0
    if (!pawn_alive(character_once(count)) || count!=1) { assert(false,"native gameplay probe step 1"); return false }
    let empty=CharacterRef()
    let empty_parent:NativePawnRef=empty
    if (ue_object_valid(empty_parent.handle)) { assert(false,"native gameplay probe step 2"); return false }
    var pawn:NativePawnRef=character
    pawn=character
    if (!pawn_alive(character) || !pawn_alive(parent_value(character)) || !pawn_alive(pawn)) { assert(false,"native gameplay probe step 3"); return false }
    if (select_parent(character)!=1 || select_exact(character)!=3) { assert(false,"native gameplay probe step 4"); return false }
    let movement=character.CharacterMovement
    let previous=movement.MaxWalkSpeed
    movement.MaxWalkSpeed=previous+5.0f
    if (movement.GetMaxSpeed()!=previous+5.0f) { assert(false,"native gameplay probe step 5"); return false }
    movement.MaxWalkSpeed=previous
    if (!ue_object_valid(character.CapsuleComponent.handle) || !ue_object_valid(character.Mesh.handle)) { assert(false,"native gameplay probe step 6"); return false }
    let player=ue_ref(type<PlayerControllerRef>,character.GetController().handle)
    let rotation=ue_rotator(0.0lf,30.0lf,0.0lf)
    player.SetControlRotation(rotation)
    if (player.GetControlRotation().yaw!=30.0lf) { assert(false,"native gameplay probe step 7"); return false }
    if (!ue_object_valid(player.K2_GetPawn().handle)) { assert(false,"native gameplay probe step 8"); return false }
    var location=ue_vector(999999.0lf,999999.0lf,999999.0lf)
    var view=ue_rotator(9999.0lf,9999.0lf,9999.0lf)
    player.GetPlayerViewPoint(location,view)
    if (view.yaw==9999.0lf || location.z==999999.0lf) { assert(false,"native gameplay probe step 9"); return false }
    // Possess expects Pawn; Character is implicitly converted to its parent view.
    player.Possess(character)
    let camera=ue_component_get(type<CameraComponentRef>,ue_ref(type<ActorRef>,ue_self_object()),"Camera")
    if (tune_camera(camera,79.0f)!=80.0f) { assert(false,"native gameplay probe step 10"); return false }
    let projection=camera.ProjectionMode
    camera.SetProjectionMode(ECameraProjectionMode.Orthographic)
    if (camera.ProjectionMode!=ECameraProjectionMode.Orthographic) { assert(false,"native gameplay probe step 11"); return false }
    camera.ProjectionMode=projection
    let axis=movement.GetPlaneConstraintAxisSetting()
    movement.SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting.Z)
    if (movement.GetPlaneConstraintAxisSetting()!=EPlaneConstraintAxisSetting.Z) { assert(false,"native gameplay probe step 12"); return false }
    movement.SetPlaneConstraintAxisSetting(axis)
    let mode=movement.DefaultLandMovementMode
    movement.DefaultLandMovementMode=mode
    let velocity=character.GetVelocity()
    character.AddMovementInput(ue_vector(1.0lf,0.0lf,0.0lf),0.25f)
    // Defaults include the trailing bForce=false; return values keep native math.
    return velocity.x==movement.Velocity.x
}
