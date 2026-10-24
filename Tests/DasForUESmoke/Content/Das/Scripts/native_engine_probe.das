options gen2
require ue_engine
require smoke_engine
require math

// The plugin's native module ue_engine (DasNativeEngine.cpp), run with the probe actor as self. input: bit 0 — the
// world has begun play, bit 1 — it has a game instance. main returns 0 when every check passes, otherwise the
// number of the first failed check.
def near(a, b : UEVector) : bool {
    return ue_nearly_equal(a, b, 0.001lf)
}

[export]
def main(input:int):int {
    let a = as_AActor(ue_self_object())
    // Actor: placement.
    if (!a.valid() || a.get_actor_location() != ue_vector(100.0lf, 200.0lf, 300.0lf)) { return 1 }
    let moved = a.set_actor_location(ue_vector(1.0lf, 2.0lf, 3.0lf))
    if (!moved || a.get_actor_location() != ue_vector(1.0lf, 2.0lf, 3.0lf)) { return 2 }
    a.set_actor_rotation(ue_rotator(0.0lf, 90.0lf, 0.0lf))
    let forward = a.get_actor_forward_vector()
    if (abs(a.get_actor_rotation().yaw - 90.0lf) > 0.001lf || !near(forward, ue_vector(0.0lf, 1.0lf, 0.0lf))) { return 3 }
    let right = a.get_actor_right_vector()
    if (!near(right, ue_vector(-1.0lf, 0.0lf, 0.0lf)) || !near(a.get_actor_up_vector(), ue_vector(0.0lf, 0.0lf, 1.0lf))) {
        return 4
    }
    a.set_actor_scale3d(ue_vector(2.0lf, 2.0lf, 2.0lf))
    let scaled = a.get_actor_scale3d() == ue_vector(2.0lf, 2.0lf, 2.0lf)
    a.set_actor_scale3d(ue_vector(1.0lf, 1.0lf, 1.0lf))
    let transform = a.get_actor_transform()
    let translation = ue_transform_translation(transform)
    if (!scaled || !a.set_actor_transform(transform) || !near(translation, a.get_actor_location())) { return 5 }
    a.add_actor_world_offset(ue_vector(10.0lf, 0.0lf, 0.0lf))
    a.add_actor_local_offset(ue_vector(10.0lf, 0.0lf, 0.0lf))
    if (!near(a.get_actor_location(), ue_vector(11.0lf, 12.0lf, 3.0lf))) { return 6 }
    a.add_actor_world_rotation(ue_rotator(0.0lf, 90.0lf, 0.0lf))
    a.add_actor_local_rotation(ue_rotator(0.0lf, -90.0lf, 0.0lf))
    if (!near(a.get_actor_forward_vector(), ue_vector(0.0lf, 1.0lf, 0.0lf))) { return 7 }
    // Distances to another actor.
    let other = smoke_engine_other()
    let diff = ue_vector(1000.0lf, 0.0lf, 0.0lf) - a.get_actor_location()
    if (abs(double(a.get_distance_to(other)) - ue_length(diff)) > 0.01lf) { return 8 }
    if (abs(double(a.get_squared_distance_to(other)) - ue_length_squared(diff)) > 1.0lf) { return 9 }
    if (abs(double(a.get_horizontal_distance_to(other)) - ue_length(ue_vector(diff.x, diff.y, 0.0lf))) > 0.01lf) {
        return 10
    }
    // Actor: state.
    a.set_actor_hidden_in_game(true)
    a.set_actor_enable_collision(false)
    if (!a.is_hidden() || a.get_actor_enable_collision()) { return 11 }
    a.set_actor_enable_collision(true)
    if (!a.is_actor_initialized() || a.is_actor_being_destroyed() || a.has_actor_begun_play() != ((input & 1) != 0)) {
        return 12
    }
    a.set_life_span(100.0f)
    let life = a.get_life_span()
    a.set_life_span(0.0f)
    if (life < 99.0f || a.get_game_time_since_creation() < 0.0f || a.get_velocity() != ue_vector(0.0lf, 0.0lf, 0.0lf)) {
        return 13
    }
    a.set_actor_tick_enabled(false)
    if (a.is_actor_tick_enabled()) { return 14 }
    // Actor: relations and names.
    a.set_owner(other)
    if (a.get_owner() != other || a.get_instigator().valid() || a.get_actor_name_or_label() == "") { return 15 }
    if (!a.actor_has_tag(ue_name_value("probe_tag")) || a.actor_has_tag(ue_name_value("other_tag"))) { return 16 }
    // Components: the root and an attached child.
    let root = a.get_root_component()
    if (!root.valid() || !near(root.get_component_location(), a.get_actor_location()) || root.get_owner() != a) {
        return 17
    }
    let child = smoke_engine_child()
    if (child.get_attach_parent() != root || !child.is_registered() || child.get_owner() != a) { return 18 }
    child.set_relative_location(ue_vector(0.0lf, 0.0lf, 50.0lf))
    if (child.get_relative_location() != ue_vector(0.0lf, 0.0lf, 50.0lf)) { return 19 }
    if (!near(child.get_component_location(), a.get_actor_location() + ue_vector(0.0lf, 0.0lf, 50.0lf))) { return 20 }
    child.set_relative_rotation(ue_rotator(0.0lf, 45.0lf, 0.0lf))
    child.set_relative_scale3d(ue_vector(3.0lf, 3.0lf, 3.0lf))
    let relative = abs(child.get_relative_rotation().yaw - 45.0lf) < 0.001lf
    if (!relative || child.get_relative_scale3d() != ue_vector(3.0lf, 3.0lf, 3.0lf)) { return 21 }
    child.set_relative_scale3d(ue_vector(1.0lf, 1.0lf, 1.0lf))
    // Attachment.
    child.detach(true)
    if (child.get_attach_parent().valid()) { return 22 }
    if (!child.attach_to(root, ue_name_value(""), false) || child.get_attach_parent() != root) { return 23 }
    // World placement of a component.
    child.set_world_location(ue_vector(5.0lf, 5.0lf, 5.0lf))
    child.add_world_offset(ue_vector(1.0lf, 0.0lf, 0.0lf))
    if (!near(child.get_component_location(), ue_vector(6.0lf, 5.0lf, 5.0lf))) { return 24 }
    child.set_world_rotation(ue_rotator(0.0lf, 0.0lf, 0.0lf))
    child.add_local_rotation(ue_rotator(0.0lf, 90.0lf, 0.0lf))
    if (!near(child.get_forward_vector(), ue_vector(0.0lf, 1.0lf, 0.0lf))) { return 25 }
    if (!near(child.get_up_vector(), ue_vector(0.0lf, 0.0lf, 1.0lf))) { return 26 }
    // Sockets: a scene component has only the component itself.
    if (root.does_socket_exist(ue_name_value("missing"))) { return 27 }
    if (!near(root.get_socket_location(ue_name_value("")), root.get_component_location())) { return 28 }
    // Activation, tags, velocity, visibility, tick.
    child.set_active(false)
    let inactive = !child.is_active()
    child.activate()
    let active = child.is_active()
    child.deactivate()
    if (!inactive || !active || child.is_active() || !child.component_has_tag(ue_name_value("probe_tag"))) { return 29 }
    child.set_component_velocity(ue_vector(1.0lf, 2.0lf, 3.0lf))
    if (child.get_component_velocity() != ue_vector(1.0lf, 2.0lf, 3.0lf)) { return 30 }
    child.set_visibility(false)
    let invisible = !child.is_visible()
    child.set_visibility(true)
    if (!invisible || !child.is_visible()) { return 31 }
    child.set_component_tick_enabled(false)
    if (child.is_component_tick_enabled()) { return 32 }
    // World.
    let w = a.get_world()
    if (!w.valid() || w != child.get_world() || w != world_of(ue_self_object()) || !w.is_game_world()) { return 33 }
    if (w.is_editor_world() || w.is_preview_world() || w.is_tearing_down()) { return 34 }
    if (w.get_time_seconds() < 0.0lf || w.get_real_time_seconds() < 0.0lf || w.get_delta_seconds() < 0.0f) { return 35 }
    let modes = (w.is_standalone() ? 1 : 0) + (w.is_client() ? 1 : 0) + (w.is_server() ? 1 : 0)
    if (modes != 1 || (w.is_listen_server() && w.is_dedicated_server())) { return 36 }
    let gi = w.get_game_instance()
    if (gi.valid() != ((input & 2) != 0) || (gi.valid() && gi.get_world() != w)) { return 37 }
    if (frame_number() < 0l) { return 38 }
    // Pawn: its own methods and the inherited Actor ones.
    let p = smoke_engine_pawn()
    if (!p.valid() || p.is_locally_controlled() || p.is_player_controlled() || ue_object_valid(p.get_controller())) {
        return 39
    }
    if (!near(p.get_actor_location(), ue_vector(0.0lf, 0.0lf, 500.0lf)) || !as_AActor(p).valid()) { return 40 }
    p.add_movement_input(ue_vector(1.0lf, 0.0lf, 0.0lf), 1.0f)
    // Coverage part 2: the world's mode and level, local players, the component's creation, the instigator.
    let mode = w.get_net_mode()
    if ((mode == 0) != w.is_standalone() || (mode == 3) != w.is_client() || w.is_starting_up()) { return 41 }
    if (!ue_object_valid(w.get_persistent_level())) { return 42 }
    let script_actor = w.get_level_script_actor()
    if (ue_object_valid(script_actor) && !ue_object_is_a(script_actor, "/Script/Engine.LevelScriptActor")) { return 43 }
    if (gi.valid()) {
        let players = gi.get_num_local_players()
        if (players < 0 || ue_object_valid(gi.get_local_player_by_index(-1))) { return 44 }
        if (ue_object_valid(gi.get_local_player_by_index(players))) { return 45 }
        if (players > 0 && !ue_object_valid(gi.get_first_game_player())) { return 46 }
        if (ue_object_valid(gi.find_local_player_from_controller_id(-12345))) { return 47 }
        let first = gi.get_first_local_player_controller()
        let first_here = gi.get_first_local_player_controller(w)
        if (ue_object_valid(first) != ue_object_valid(first_here)) { return 48 }
        if (ue_object_valid(first) && !__ue_same_object(first, first_here)) { return 48 }
        // Coverage part 3: a local player made and removed again (the error is an output parameter).
        var error = "unchanged"
        let made = gi.create_local_player(7, error, false)
        if (!ue_object_valid(made) || gi.get_num_local_players() != players + 1) { return 51 }
        if (!gi.remove_local_player(made) || gi.get_num_local_players() != players) { return 52 }
        if (players > 0) {
            var initial_error = ""
            let again = gi.create_initial_player(initial_error) // controller 0 has a player already
            if (ue_object_valid(again) || initial_error == "") { return 53 }
        }
    }
    if (child.get_component_creation_method() != 0) { return 49 } // created with NewObject: native
    child.mark_render_state_dirty()
    if (a.get_actor_instigator().valid() || ue_object_valid(a.get_actor_instigator_controller())) { return 50 }
    // Coverage part 4: components by class (UEClassHandle) as arrays. The actor has a root_component and its child.
    let scene_class = __ue_class_loaded("/Script/Engine.SceneComponent", "/Script/CoreUObject.Object", "")
    let mesh_class = __ue_class_loaded("/Script/Engine.StaticMeshComponent", "/Script/CoreUObject.Object", "")
    let actor_class = __ue_class_loaded("/Script/Engine.Actor", "/Script/CoreUObject.Object", "")
    let all = a.get_all_components()
    var has_child = false
    for (component in all) {
        has_child = has_child || __ue_same_object(component.handle(), child.handle())
    }
    if (length(all) != 2 || !has_child) { return 54 }
    if (length(a.get_components_by_class(scene_class)) != 2 || length(a.get_components_by_class(mesh_class)) != 0) {
        return 55
    }
    let tagged = a.get_components_by_class_with_tag(scene_class, ue_name_value("probe_tag"))
    if (length(tagged) != 1 || !__ue_same_object(tagged[0].handle(), child.handle())) { return 56 }
    let root_component = a.get_root_component()
    if (!__ue_same_object(root_component.get_child_component_by_class(scene_class).handle(), child.handle())) { return 57 }
    var no_class : UEClassHandle
    if (root_component.get_child_component_by_class(no_class).valid() || length(a.get_components_by_class(no_class)) != 0) {
        return 58
    }
    if (length(root_component.get_children_components_by_class(scene_class, true)) != 1) { return 59 }
    var refused = false
    try {
        let wrong = root_component.get_child_component_by_class(actor_class) // not a SceneComponent
        refused = !wrong.valid() && false
    } recover {
        refused = true
    }
    if (!refused) { return 60 }
    return 0
}

[export]
def null_actor(input:int):int {
    var a : AActor
    return int(a.get_actor_location().x)
}

[export]
def destroyed_actor(input:int):int {
    let victim = smoke_engine_victim()
    victim.destroy()
    return int(victim.get_actor_location().x)
}

[export]
def null_attach_parent(input:int):int {
    var parent : USceneComponent
    return smoke_engine_child().attach_to(parent, ue_name_value(""), false) ? 1 : 0
}

// The call cost benchmark (interop-performance): actor.get_actor_location() inside the script.
[export]
def bench_actor_location(count:int):int {
    let a = as_AActor(ue_self_object())
    var n = 0
    for (i in range(0, count)) {
        if (a.get_actor_location().x > -1.0e30lf) { n += 1 }
    }
    return n
}
