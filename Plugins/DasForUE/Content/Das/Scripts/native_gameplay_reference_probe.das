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
    if (!pawn_alive(character_once(count)) || count!=1) { verify(false,"native gameplay probe step 1"); return false }
    let empty=CharacterRef()
    let empty_parent:NativePawnRef=empty
    if (ue_object_valid(empty_parent.handle)) { verify(false,"native gameplay probe step 2"); return false }
    var pawn:NativePawnRef=character
    pawn=character
    if (!pawn_alive(character) || !pawn_alive(parent_value(character)) || !pawn_alive(pawn)) { verify(false,"native gameplay probe step 3"); return false }
    if (select_parent(character)!=1 || select_exact(character)!=3) { verify(false,"native gameplay probe step 4"); return false }
    let movement=character.CharacterMovement
    let previous=movement.MaxWalkSpeed
    movement.MaxWalkSpeed=previous+5.0f
    if (movement.GetMaxSpeed()!=previous+5.0f) { verify(false,"native gameplay probe step 5"); return false }
    movement.MaxWalkSpeed=previous
    if (!ue_object_valid(character.CapsuleComponent.handle) || !ue_object_valid(character.Mesh.handle)) { verify(false,"native gameplay probe step 6"); return false }
    let player=ue_ref(type<PlayerControllerRef>,character.GetController().handle)
    let rotation=ue_rotator(0.0lf,30.0lf,0.0lf)
    player.SetControlRotation(rotation)
    if (player.GetControlRotation().yaw!=30.0lf) { verify(false,"native gameplay probe step 7"); return false }
    if (!ue_object_valid(player.K2_GetPawn().handle)) { verify(false,"native gameplay probe step 8"); return false }
    var location=ue_vector(999999.0lf,999999.0lf,999999.0lf)
    var view=ue_rotator(9999.0lf,9999.0lf,9999.0lf)
    player.GetPlayerViewPoint(location,view)
    if (view.yaw==9999.0lf || location.z==999999.0lf) { verify(false,"native gameplay probe step 9"); return false }
    // Possess expects Pawn; Character is implicitly converted to its parent view.
    player.Possess(character)
    let camera=ue_component_get(type<CameraComponentRef>,ue_ref(type<ActorRef>,ue_self_object()),"Camera")
    if (tune_camera(camera,79.0f)!=80.0f) { verify(false,"native gameplay probe step 10"); return false }
    let projection=camera.ProjectionMode
    camera.SetProjectionMode(ECameraProjectionMode.Orthographic)
    if (camera.ProjectionMode!=ECameraProjectionMode.Orthographic) { verify(false,"native gameplay probe step 11"); return false }
    camera.ProjectionMode=projection
    // NewCustomMode defaults to zero; native uint8 inputs are bounded DAS ints.
    let old_mode=movement.MovementMode
    let old_custom=movement.CustomMovementMode
    movement.SetMovementMode(EMovementMode.MOVE_Custom,255)
    verify(movement.CustomMovementMode==255,"native uint8 upper endpoint")
    movement.SetMovementMode(EMovementMode.MOVE_Custom)
    verify(movement.CustomMovementMode==0,"native uint8 default zero")
    movement.SetMovementMode(old_mode,old_custom)
    // The enum default TransformSpace=RTS_World is a typed enum constant.
    let socket_name=ue_name_value("None")
    let socket_default=camera.GetSocketTransform(socket_name)
    let socket_explicit=camera.GetSocketTransform(socket_name,ERelativeTransformSpace.RTS_World)
    verify(ue_transform_translation(socket_default).x==ue_transform_translation(socket_explicit).x,"native enum default")
    let text_renderer=ue_component_get(type<TextRenderComponentRef>,ue_ref(type<ActorRef>,ue_self_object()),"Label")
    let text=ue_text("DasForUE","NativeProbe","Hello")
    text_renderer.K2_SetText(text)
    verify(ue_text_to_string(text_renderer.Text)=="Hello","native text display")
    verify(ue_text_namespace(text_renderer.Text)=="DasForUE" && ue_text_key(text_renderer.Text)=="NativeProbe","native localized text identity")
    let empty_invariant=ue_text_invariant("")
    text_renderer.K2_SetText(empty_invariant)
    verify(ue_text_is_invariant(text_renderer.Text) && ue_text_to_string(text_renderer.Text)=="","empty invariant flags")
    let invariant=ue_text_invariant("fixed")
    text_renderer.K2_SetText(invariant)
    verify(ue_text_is_invariant(text_renderer.Text) && ue_text_to_string(text_renderer.Text)=="fixed","native invariant text")
    text_renderer.K2_SetText(text)
    // Real engine static functions cover return and out marshalling as well.
    let lookup=ue_begin_static_call("/Script/Engine.KismetTextLibrary","FindTextInLocalizationTable","")
    ue_arg_string(lookup,"Namespace","DasForUE")
    ue_arg_string(lookup,"Key","NativeProbe")
    ue_arg_string(lookup,"SourceString","Hello")
    ue_invoke_call(lookup)
    let found=__ue_method_result_text(lookup,"OutText")
    verify(ue_result_bool(lookup,"ReturnValue") && ue_text_key(found)=="NativeProbe","native text out identity")
    ue_release_call(lookup)
    let upper=ue_begin_static_call("/Script/Engine.KismetTextLibrary","TextToUpper","")
    __ue_method_arg_text(upper,"InText",found)
    ue_invoke_call(upper)
    let transformed=__ue_method_result_text(upper,"ReturnValue")
    verify(ue_text_to_string(transformed)=="HELLO" && ue_text_to_string(found)=="Hello","native text result snapshot")
    ue_release_call(upper)
    let byte_call=ue_begin_static_call("/Script/Engine.KismetMathLibrary","BMin","")
    ue_arg_byte(byte_call,"A",255)
    ue_arg_byte(byte_call,"B",0)
    ue_invoke_call(byte_call)
    verify(ue_result_byte(byte_call,"ReturnValue")==0,"native byte return")
    ue_release_call(byte_call)

    // Native fields are owned values. Modify a copy, then assign the whole field.
    var tags <- character.Tags
    let old_tags_count=length(tags)
    push(tags,ue_name_value("NativeArrayProbe"))
    character.Tags:=tags
    var read_tags <- character.Tags
    verify(length(read_tags)==old_tags_count+1,"native array field write/read")
    verify(ue_name_text(read_tags[old_tags_count])=="NativeArrayProbe","native array name element")
    pop(tags)
    character.Tags:=tags
    var children:array<ActorRef>
    character.GetAllChildActors(children)
    verify(length(children)==0,"native out object array")
    var attached:array<ActorRef>
    character.GetAttachedActors(attached)
    verify(length(attached)==0,"native object array out defaults")
    var components <- character.K2_GetComponentsByClass(__ue_class_loaded("/Script/Engine.ActorComponent","/Script/Engine.ActorComponent",""))
    verify(length(components)>=3,"native nonempty object array result")
    for (component in components) { verify(ue_object_valid(component.handle),"native object array retains checked references") }
    var sockets <- camera.GetAllSocketNames()
    verify(length(sockets)==0,"native array result")
    var coefficients:array<float>
    push(coefficients,0.25f)
    push(coefficients,0.5f)
    text_renderer.SetCustomPrimitiveDataFloatArray(0,coefficients)
    text_renderer.SetCustomPrimitiveDataFloatArray(0,coefficients)
    let old_group=movement.AvoidanceGroup
    var group=old_group
    group.bGroup0=false
    group.bGroup1=true
    movement.SetAvoidanceGroupMask(group)
    verify(!movement.AvoidanceGroup.bGroup0 && movement.AvoidanceGroup.bGroup1,"native struct argument and field")
    movement.SetAvoidanceGroupMask(old_group)
    var slope=text_renderer.GetWalkableSlopeOverride()
    let old_slope=slope
    slope.WalkableSlopeBehavior=EWalkableSlopeBehavior.WalkableSlope_Increase
    slope.WalkableSlopeAngle=35.0f
    text_renderer.SetWalkableSlopeOverride(slope)
    verify(text_renderer.GetWalkableSlopeOverride().WalkableSlopeAngle==35.0f,"native enum struct input/result")
    text_renderer.SetWalkableSlopeOverride(old_slope)
    var shapes <- player.StreamingSourceShapes
    let old_shape_count=length(shapes)
    var shape=StreamingSourceShape()
    shape.Radius=123.0f
    shape.LoadingRangeScale=1.0f
    shape.Location=ue_vector(1.0lf,2.0lf,3.0lf)
    shape.Rotation=ue_rotator(0.0lf,15.0lf,0.0lf)
    push_clone(shapes,shape)
    player.StreamingSourceShapes:=shapes
    var read_shapes <- player.StreamingSourceShapes
    verify(length(read_shapes)==old_shape_count+1 && read_shapes[old_shape_count].Radius==123.0f && read_shapes[old_shape_count].Location.z==3.0lf,"native struct array and nested math")
    var out_shapes:array<StreamingSourceShape>
    player.GetStreamingSourceShapes(out_shapes)
    verify(length(out_shapes)==length(read_shapes),"native out struct array")
    pop(shapes)
    player.StreamingSourceShapes:=shapes
    let axis=movement.GetPlaneConstraintAxisSetting()
    movement.SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting.Z)
    if (movement.GetPlaneConstraintAxisSetting()!=EPlaneConstraintAxisSetting.Z) { verify(false,"native gameplay probe step 12"); return false }
    movement.SetPlaneConstraintAxisSetting(axis)
    let mode=movement.DefaultLandMovementMode
    movement.DefaultLandMovementMode=mode
    let velocity=character.GetVelocity()
    character.AddMovementInput(ue_vector(1.0lf,0.0lf,0.0lf),0.25f)
    // Defaults include the trailing bForce=false; return values keep native math.
    return velocity.x==movement.Velocity.x
}

// Informational benchmark (DasPlayableCharacterProbe): one native field write plus read per iteration.
[export]
def native_field_bench(count:int):int {
    let movement=ue_character_self().CharacterMovement
    let speed=movement.MaxWalkSpeed
    var total=0.0f
    for (i in range(count)) {
        movement.MaxWalkSpeed=speed
        total+=movement.MaxWalkSpeed
    }
    return int(total/speed+0.5f)
}
[export]
def native_field_read_bench(count:int):int {
    let movement=ue_character_self().CharacterMovement
    var total=0.0f
    for (i in range(count)) {
        total+=movement.MaxWalkSpeed
    }
    return int(total/movement.MaxWalkSpeed+0.5f)
}
[export]
def native_valid_bench(count:int):int {
    let movement=ue_character_self().CharacterMovement
    var total=0
    for (i in range(count)) {
        if (ue_object_valid(movement.handle)) { total+=1 }
    }
    return total
}
[export]
def native_field_write_bench(count:int):int {
    let movement=ue_character_self().CharacterMovement
    let speed=movement.MaxWalkSpeed
    for (i in range(count)) {
        movement.MaxWalkSpeed=speed
    }
    return count
}
