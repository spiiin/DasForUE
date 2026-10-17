options gen2
require ue_native_gameplay

// The same generated members work in free functions and DSL instance methods.
def tune_camera(camera:CameraComponentRef; fov:float):float {
    camera.SetFieldOfView(fov)
    camera.FieldOfView+=1.0f
    return camera.FieldOfView
}
[export]
def main():bool {
    let character=ue_character_self()
    let movement=character.CharacterMovement
    let previous=movement.MaxWalkSpeed
    movement.MaxWalkSpeed=previous+5.0f
    if (movement.GetMaxSpeed()!=previous+5.0f) { return false }
    movement.MaxWalkSpeed=previous
    if (!ue_object_valid(character.CapsuleComponent.handle) || !ue_object_valid(character.Mesh.handle)) { return false }
    let player=ue_ref(type<PlayerControllerRef>,character.GetController().handle)
    let rotation=ue_rotator(0.0lf,30.0lf,0.0lf)
    player.SetControlRotation(rotation)
    if (player.GetControlRotation().yaw!=30.0lf) { return false }
    if (!ue_object_valid(player.K2_GetPawn().handle)) { return false }
    var location=ue_vector(999999.0lf,999999.0lf,999999.0lf)
    var view=ue_rotator(9999.0lf,9999.0lf,9999.0lf)
    player.GetPlayerViewPoint(location,view)
    if (view.yaw==9999.0lf || location.z==999999.0lf) { return false }
    let camera=ue_component_get(type<CameraComponentRef>,ue_ref(type<ActorRef>,ue_self_object()),"Camera")
    if (tune_camera(camera,79.0f)!=80.0f) { return false }
    let velocity=character.GetVelocity()
    character.AddMovementInput(ue_vector(1.0lf,0.0lf,0.0lf),0.25f)
    // Defaults include the trailing bForce=false; return values keep native math.
    return velocity.x==movement.Velocity.x
}
