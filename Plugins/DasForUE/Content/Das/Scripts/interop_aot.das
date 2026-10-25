options gen2
require ue_foundation
require ue_time
require ue_math
require ue_engine
require interop_objects
require ue_native_gameplay
require math

// AOT against the interpreter (DasInteropProbe.inl, C3): input = iterations * 16 + mode; the results are folded
// with XOR so neither build can drop the loop. Self is a UDasObjectFixture with health == 7. Modes 4 and 5 call
// native binds through the guarded bridge: 4 a scalar one, 5 engine values (FVector by const reference and by
// value, a date, a string result) - six calls an iteration. Mode 6 calls binds with objects (interop_objects: two
// actors made by the probe): a parameter and the receiver, an object result, the valid / handle / as_ helpers.
// Modes 7 and 8 fail (a null receiver, a null argument of a NonNullArgs bind): the probe compares the errors.
// Mode 9 passes variables to binds with reference parameters (output parameters). Mode 10 is vector arithmetic
// with ue_foundation's externs and operators; mode 11 is the same arithmetic on daScript's float3, the floor.
// Mode 12: == of a class type and a bind with an FName parameter (two calls an iteration). Mode 13: a bind with an
// array result (the actor's components: none outside a world). Mode 14 fails: a gameplay call on a native reference
// that a bind answers (Reflects: K2_GetActorLocation) needs a receiver in the owner's world.
def leaf(value:int):int { return value+1 }

[export]
def main(input:int):int {
    let mode=input % 16
    let count=input / 16
    var acc=0
    if (mode==0) {
        for (i in range(0,count)) { acc^=i & 7 }
    } elif (mode==1) {
        for (i in range(0,count)) { acc^=leaf(i) }
    } elif (mode==2) {
        for (i in range(0,count)) { acc^=ue_get_self_int("health")+i }
    } elif (mode==3) {
        for (i in range(0,count)) { acc^=ue_state_get_int("interop_missing",i) }
    } elif (mode==4) {
        for (i in range(0,count)) { acc^=days_in_year(2023)+i }
    } elif (mode==6) {
        let a=interop_actor()
        let o=interop_owner()
        for (i in range(0,count)) {
            a.set_owner(o)
            let owner=a.get_owner()
            acc^=(owner.valid() ? 1 : 0)+(as_AActor(owner.handle()).valid() ? 2 : 0)+i
        }
    } elif (mode==9) {
        var direction=zero_vector()
        var length=0.0lf
        for (i in range(0,count)) {
            let size=double(i & 7)
            forward_vector().get_clamped_to_size(size,size).to_direction_and_length(direction,length)
            acc^=int(length+0.5lf)+i
        }
    } elif (mode==10) {
        let step=ue_vector(1.0lf,0.0lf,0.0lf)
        for (i in range(0,count)) {
            let w=ue_vector(double(i & 7),0.0lf,0.0lf)+step*2.0lf
            acc^=int(ue_length(w)+0.5lf)*(i+1) // not a plain + i: that XOR is 0 over 2^22 iterations
        }
    } elif (mode==11) {
        let step=float3(1.0,0.0,0.0)
        for (i in range(0,count)) {
            let w=float3(float(i & 7),0.0,0.0)+step*2.0
            acc^=int(length(w)+0.5)*(i+1)
        }
    } elif (mode==12) {
        let a=interop_actor()
        let o=interop_owner()
        let tag=ue_name_value("interop_tag")
        for (i in range(0,count)) {
            acc^=(a==o ? 0 : 1)+(a.actor_has_tag(tag) ? 2 : 0)+i
        }
    } elif (mode==13) {
        let a=interop_actor()
        for (i in range(0,count)) {
            acc^=length(a.get_all_components())+i
        }
    } elif (mode==14) {
        let r=ue_ref(type<ActorRef>,interop_actor().handle())
        acc=int(r.K2_GetActorLocation().size_2d())
    } elif (mode==7) {
        var none : AActor
        acc=int(none.get_actor_location().size_2d())
    } elif (mode==8) {
        var none : AActor
        acc=int(interop_actor().get_distance_to(none))
    } else {
        // The string result once (24 characters), not in the loop: each call allocates it on the context heap.
        let iso=length(make_date_time(2024,2,29).to_iso8601())
        for (i in range(0,count)) {
            let turned=forward_vector().rotate_angle_axis(180.0lf,up_vector())
            acc^=int(turned.size_2d()+0.5lf)+make_date_time(2024,2,29).get_day()+iso+i
        }
    }
    return acc
}
