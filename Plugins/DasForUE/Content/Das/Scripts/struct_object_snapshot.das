options gen2
options gc
require ue_foundation
var saved : UEStructValue
[export]
def main(mode:int):int {
    if (mode == 0) {
        var child = ue_struct_value("/Script/DasRuntime.DasObjectSnapshot")
        child = ue_struct_with_object(child, "Object", ue_get_self_object("Target"))
        child = ue_struct_with_name(child, "Key", ue_name_value("SnapshotKey_7"))
        saved = ue_struct_value("/Script/DasRuntime.DasNestedObjectSnapshot")
        saved = ue_struct_nested_with(saved, "Details", -1, child)
        saved = ue_struct_array_resize(saved, "Items", 1)
        saved = ue_struct_nested_with(saved, "Items", 0, child)
        return 1
    }
    if (mode == 1) {
        ue_struct_collect()
        verify(ue_object_valid(ue_struct_get_object(saved, "Details.Object")))
        verify(ue_object_valid(ue_struct_get_object(saved, "Items.0.Object")))
        verify(ue_name_text(ue_struct_get_name(saved, "Details.Key")) == "SnapshotKey_7")
        let cleared = ue_struct_with_object(saved, "Details.Object", ue_null_object())
        verify(!ue_object_valid(ue_struct_get_object(cleared, "Details.Object")))
        verify(ue_object_valid(ue_struct_get_object(saved, "Details.Object")))
        return 1
    }
    if (mode == 2) {
        saved = ue_struct_value("/Script/DasRuntime.DasNestedObjectSnapshot")
        ue_struct_collect()
        return 1
    }
    if (mode == 3) {
        var child = ue_struct_value("/Script/DasRuntime.DasObjectSnapshot")
        child = ue_struct_with_object(child, "Actor", ue_get_self_object("Target"))
        return 1
    }
    if (mode == 4) {
        saved = ue_struct_value("/Script/DasRuntime.DasObjectSnapshot")
        saved = ue_struct_with_object(saved, "Actor", ue_get_self_object("Target"))
        return 1
    }
    if (mode == 5) {
        return ue_object_valid(ue_struct_get_object(saved, "Actor")) ? 1 : 0
    }
    return 0
}
