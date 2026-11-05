options gen2
module ue_instanced_mesh public
require ue_scene_components public
require ue_pickup_components public
require ue_effects public

typedef InstancedStaticMeshComponentRef = UENativeObjectRef("/Script/Engine.InstancedStaticMeshComponent")
typedef WorldAudioComponentRef = UENativeObjectRef("/Script/Engine.AudioComponent")

// Create unregistered, configure, then register. Keep a reflected strong reference.
def ue_world_component_create(nativeClassPath : string; name : string) : ActorComponentRef {
    let call = ue_begin_static_call("/Script/DasRuntime.DasWorldComponentLibrary", "CreateNativeWorldComponent", "WorldContextObject")
    ue_arg(call, "NativeClassPath", nativeClassPath)
    ue_arg(call, "ComponentName", ue_name_value(name))
    ue_invoke_call(call)
    let component = ue_ref(type<ActorComponentRef>, ue_result_object(call, "ReturnValue"))
    ue_release_call(call)
    return component
}

def ue_world_component_register(component : auto) : bool {
    let call = ue_begin_static_call("/Script/DasRuntime.DasWorldComponentLibrary", "RegisterWorldComponent", "WorldContextObject")
    ue_arg_object(call, "Component", component.handle)
    ue_invoke_call(call)
    let registered = ue_result_bool(call, "ReturnValue")
    ue_release_call(call)
    return registered
}

def ue_world_component_destroy(component : auto) : bool {
    let call = ue_begin_static_call("/Script/DasRuntime.DasWorldComponentLibrary", "DestroyWorldComponent", "")
    ue_arg_object(call, "Component", component.handle)
    ue_invoke_call(call)
    let destroyed = ue_result_bool(call, "ReturnValue")
    ue_release_call(call)
    return destroyed
}

def ue_world_ism_create(name : string = "Instances") : InstancedStaticMeshComponentRef {
    return ue_ref(type<InstancedStaticMeshComponentRef>,
        ue_world_component_create("/Script/Engine.InstancedStaticMeshComponent", name).handle)
}

def ue_world_audio_create(name : string = "Audio") : WorldAudioComponentRef {
    return ue_ref(type<WorldAudioComponentRef>,
        ue_world_component_create("/Script/Engine.AudioComponent", name).handle)
}

// Values are native FPrimitiveInstanceId keys, scoped to this component, not array indices.
def ue_ism_add_instances(mesh : InstancedStaticMeshComponentRef; transforms : array<UETransform>;
    worldSpace : bool = true; updateNavigation : bool = false) : array<int> {
    var ids : array<int>
    __ue_ism_add_instances(mesh.handle, transforms, worldSpace, updateNavigation, ids)
    return <- ids
}

def ue_ism_remove_instances(mesh : InstancedStaticMeshComponentRef; ids : array<int>;
    updateNavigation : bool = false) : bool {
    return __ue_ism_remove_instances(mesh.handle, ids, updateNavigation)
}

def ue_ism_instance_index(mesh : InstancedStaticMeshComponentRef; id : int) : int {
    let call = ue_begin_static_call("/Script/DasRuntime.DasWorldComponentLibrary", "GetInstanceIndexById", "")
    ue_arg(call, "Mesh", mesh)
    ue_arg(call, "Id", id)
    ue_invoke_call(call)
    let index = ue_result_int(call, "ReturnValue")
    ue_release_call(call)
    return index
}
