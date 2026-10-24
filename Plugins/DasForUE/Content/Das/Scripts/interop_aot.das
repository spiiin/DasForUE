options gen2
require ue_foundation
require ue_time
require ue_math
require ue_engine
require interop_objects

// AOT against the interpreter (DasInteropProbe.inl, C3): input = iterations * 10 + mode; the results are folded
// with XOR so neither build can drop the loop. Self is a UDasObjectFixture with health == 7. Modes 4 and 5 call
// native binds through the guarded bridge: 4 a scalar one, 5 engine values (FVector by const reference and by
// value, a date, a string result) - six calls an iteration. Mode 6 calls binds with objects (interop_objects: two
// actors made by the probe): a parameter and the receiver, an object result, the valid / handle / as_ helpers.
// Modes 7 and 8 fail (a null receiver, a null argument of a NonNullArgs bind): the probe compares the errors.
// Mode 9 passes variables to binds with reference parameters (output parameters; ue_math values only, as
// ue_foundation's math externs keep a function in the interpreter).
def leaf(value:int):int { return value+1 }

[export]
def main(input:int):int {
    let mode=input % 10
    let count=input / 10
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
