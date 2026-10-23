options gen2
require smoke_native
require smoke_native_ext
require smoke_native_late

// Classes and methods from the smoke game module (Tests/DasForUESmoke/.../DasNativeBindsProbe.cpp). main returns
// 0 when every check passes, otherwise the number of the first failed check.
[export]
def main(input:int):int {
    let p = smoke_make_probe(5)
    if (!p.valid() || p.get_value() != 5) { return 1 }
    p.set_value(7)
    if (p.get_value() != 7) { return 2 }
    if (p.scaled(3) != 21) { return 3 }
    if (p.value_plus(1) != 8) { return 4 }
    // Inherited methods and the child's own.
    let child = smoke_make_child(2)
    if (child.get_value() != 2 || child.child_only() != 20) { return 5 }
    // Casts: up, down, and down to the wrong class.
    let base = as_Probe(child)
    if (base.get_value() != 2 || !as_ProbeChild(base).valid() || as_ProbeChild(p).valid()) { return 6 }
    // Objects as arguments and results.
    p.set_partner(base)
    if (p.get_partner() != base || !smoke_are_partners(p, base)) { return 7 }
    // A null reference is a valid argument; == compares objects.
    var nobody : Probe
    p.set_partner(nobody)
    if (p.get_partner().valid() || nobody.valid() || nobody != as_Probe(ue_null_object())) { return 8 }
    // UObjectHandle interop.
    if (as_Probe(p.handle()) != p || !ue_object_valid(p.handle()) || as_Probe(child.handle()).get_value() != 2) {
        return 9
    }
    // A method from another module, on the bound class and on a descendant built earlier.
    if (p.ext_double() != 14 || child.ext_double() != 4) { return 10 }
    // A descendant built later inherits methods of both modules.
    let g = smoke_make_grandchild(3)
    if (g.get_value() != 3 || g.child_only() != 30 || g.ext_double() != 6 || as_Probe(g).get_value() != 3) {
        return 11
    }
    // A class without a script type is an UObjectHandle checked at the call.
    if (!smoke_is_script_component(ue_self_object()) || smoke_is_script_component(ue_null_object())) { return 12 }
    if (smoke_partner_value(p) != 7) { return 13 }
    if (as_ProbeChild(ue_self_object()).valid()) { return 14 }
    return 0
}

// The call cost benchmark (interop-performance): count method calls inside the script.
[export]
def bench_method(count:int):int {
    let p = smoke_make_probe(1)
    var sum = 0
    for (i in range(0, count)) { sum += p.get_value() }
    return sum
}
